// Experimental capped critical-bit index. Included after the match-study
// primitives, inside its private namespace. The immutable input outlives a run.
// Keys contain 258 bytes, with virtual zero padding only at the end of input.
// This is a bounded index experiment, not yet a streaming-buffer implementation.
struct Radix final {
    using Link = std::uint16_t;
    static constexpr Link none = UINT16_MAX;
    static constexpr unsigned leaf_count = 32768;
    struct Branch final {
        std::array<Link, 2> children{none, none};
        Link parent = none;
        Link bit = 0;
        std::uint32_t newest = absent;
    };
    static_assert(sizeof(Branch) == 12);
    // Leaves occupy [0,32768); branches occupy [32768,65535). Leaf slots
    // are recycled by position modulo window, after explicit oldest removal.
    std::array<Branch, leaf_count - 1> branches{};
    std::array<std::uint32_t, leaf_count> positions{};
    std::array<Link, leaf_count> parents{};
    // Exact two-byte directory removes the first sixteen discriminating bits
    // from each walk. It partitions one index; no workload classification.
    std::array<Link, 65536> roots{};
    Link root = none;
    Link free_branch = none;
    unsigned allocated = 0;
    Carried carried{};
    unsigned adjacent_length = 0;

    void reset() {
        root = none;
        free_branch = none;
        allocated = 0;
        positions.fill(absent);
        roots.fill(none);
        carried = {};
        adjacent_length = 0;
    }
    Link parent(Link link) const {
        if (link < leaf_count) return parents[link];
        return branches[link - leaf_count].parent;
    }
    void set_parent(Link link, Link value) {
        if (link < leaf_count) parents[link] = value;
        else branches[link - leaf_count].parent = value;
    }
    std::uint32_t newest(Link link) const {
        if (link < leaf_count) return positions[link];
        return branches[link - leaf_count].newest;
    }
    static unsigned byte(std::span<const std::uint8_t> input,
        std::uint32_t position, unsigned offset) {
        const std::size_t index = static_cast<std::size_t>(position) + offset;
        if (index < input.size()) return input[index];
        return 0;
    }
    static unsigned direction(std::span<const std::uint8_t> input,
        std::uint32_t position, unsigned bit) {
        const unsigned value = byte(input, position, bit / 8);
        const unsigned result = (value >> (7 - bit % 8)) & 1U;
        return result;
    }
    static unsigned directory(std::span<const std::uint8_t> input, std::uint32_t position) {
        const unsigned result = (static_cast<unsigned>(input[position]) << 8) | input[position + 1];
        return result;
    }
    template<bool Count>
    static unsigned different_bit(std::span<const std::uint8_t> input,
        std::uint32_t position, std::uint32_t source, unsigned length,
        unsigned maximum, Work& work) {
        unsigned different = length;
        while (different < 258 && different >= maximum) {
            if constexpr (Count) ++work.compared;
            if (byte(input, position, different) != byte(input, source, different)) break;
            ++different;
        }
        if (different == 258) return 258 * 8;
        const unsigned difference = byte(input, position, different) ^ byte(input, source, different);
        const unsigned bit = different * 8 + static_cast<unsigned>(std::countl_zero(difference)) - 24;
        return bit;
    }
    template<bool Count>
    void replace(Link old_link, Link replacement, Link ancestor, Work& work) {
        if (ancestor == none) root = replacement;
        else {
            Branch& branch = branches[ancestor - leaf_count];
            const unsigned side = branch.children[0] == old_link ? 0U : 1U;
            branch.children[side] = replacement;
        }
        if (replacement != none) set_parent(replacement, ancestor);
        if constexpr (Count) work.writes += replacement == none ? 1 : 2;
    }
    template<bool Count>
    void erase_oldest(std::uint32_t position, Work& work) {
        const Link leaf = static_cast<Link>(position & (window - 1));
        // An identical, newer key may already have displaced this source.
        if (positions[leaf] != position) return;
        const Link ancestor = parents[leaf];
        if (ancestor == none) root = none;
        else {
            Branch& branch = branches[ancestor - leaf_count];
            const Link sibling = branch.children[branch.children[0] == leaf ? 1 : 0];
            replace<Count>(ancestor, sibling, branch.parent, work);
            branch.children[0] = free_branch;
            free_branch = ancestor;
            if constexpr (Count) work.writes += 2;
        }
        // The globally oldest leaf cannot be the newest surviving source in
        // any ancestor. No upward maximum repair is required after contraction.
        positions[leaf] = absent;
        if constexpr (Count) ++work.writes;
    }
    template<bool Count>
    Match advance(std::span<const std::uint8_t> input, std::uint32_t position,
        bool query, Work& work) {
        Match best{};
        const unsigned maximum = static_cast<unsigned>(std::min<std::size_t>(258, input.size() - position));
        if (maximum < 3) return best;
        // Exclude exactly one still-legal source before recycling its slot.
        // Query it separately below, after nearer sources have settled ties.
        if (position >= window) {
            const unsigned old_directory = directory(input, position - window);
            root = roots[old_directory];
            erase_oldest<Count>(position - window, work);
            roots[old_directory] = root;
            if constexpr (Count) ++work.writes;
        }
        const unsigned current_directory = directory(input, position);
        root = roots[current_directory];
        const Link new_leaf = static_cast<Link>(position & (window - 1));
        Link start = root;
#ifdef STX_MATCH_INDEX_RADIX_FINGER
        unsigned adjacent_bit = 0;
        if (position != 0) {
            const unsigned known_adjacent = adjacent_length > 0 ? adjacent_length - 1 : 0;
            if constexpr (Count) work.reused += known_adjacent;
            adjacent_length = extend<Count>(input, position, position - 1, known_adjacent, maximum, work);
            adjacent_bit = different_bit<Count>(input, position, position - 1, adjacent_length, maximum, work);
            const Link previous_leaf = static_cast<Link>((position - 1) & (window - 1));
            Link departed = previous_leaf;
            Link ancestor = parents[previous_leaf];
            // Every ancestor of the last inserted leaf has an implicit newest
            // source of position-1. Finalize only the path being departed.
            // Shared ancestors remain implicit for the new current source.
            while (ancestor != none && branches[ancestor - leaf_count].bit >= adjacent_bit) {
                Branch& branch = branches[ancestor - leaf_count];
                branch.newest = position - 1;
                departed = ancestor;
                ancestor = branch.parent;
                if constexpr (Count) { ++work.visits; ++work.writes; }
            }
            if (adjacent_length >= 2) {
                start = departed;
                // A first difference before this subtree's first split proves
                // its entire key group ties the adjacent source. Insert above
                // the subtree, using that source as the representative.
                if (start >= leaf_count && branches[start - leaf_count].bit > adjacent_bit) start = previous_leaf;
            }
            if (query && adjacent_length >= 3) best = Match{adjacent_length, 1};
        }
#endif
        if (root == none) {
            root = new_leaf;
            positions[new_leaf] = position;
            parents[new_leaf] = none;
            if constexpr (Count) work.writes += 3;
        } else {
            Link leaf = start;
            while (leaf >= leaf_count) {
                const Branch& branch = branches[leaf - leaf_count];
                const unsigned side = direction(input, position, branch.bit);
                leaf = branch.children[side];
                if constexpr (Count) { ++work.visits; ++work.compared; }
            }
            if constexpr (Count) ++work.visits;
            const std::uint32_t source = positions[leaf];
            const unsigned shift = position - carried.position;
            unsigned known = 2;
#ifdef STX_MATCH_INDEX_RADIX_FINGER
            if (source == position - 1) known = adjacent_length;
#endif
            if constexpr (share_prefixes) {
                if (shift < carried.length && source == carried.source + shift) known = std::max(known, carried.length - shift);
            }
            if constexpr (Count) work.reused += known;
            const unsigned length = extend<Count>(input, position, source, known, maximum, work);
            if constexpr (share_prefixes) carried = Carried{position, source, length};
            // Blind critical-bit descent finds a leaf with maximal common
            // prefix. Ascend through splits inside/after the mismatch byte:
            // this entire subtree shares the winning whole-byte prefix.
            if (query && length >= 3 && length > best.length) {
                Link group = leaf;
                Link ancestor = parents[leaf];
                while (ancestor != none && branches[ancestor - leaf_count].bit >= length * 8) {
                    group = ancestor;
                    ancestor = branches[ancestor - leaf_count].parent;
                    if constexpr (Count) ++work.visits;
                }
                best = Match{length, position - newest(group)};
            }
            const unsigned bit = different_bit<Count>(input, position, source, length, maximum, work);
            Link ancestor = parents[leaf];
            if (bit == 258 * 8) {
                replace<Count>(leaf, new_leaf, ancestor, work);
                positions[leaf] = absent;
                if constexpr (Count) ++work.writes;
            } else {
                Link subtree = leaf;
                while (ancestor != none && branches[ancestor - leaf_count].bit >= bit) {
                    subtree = ancestor;
                    ancestor = branches[ancestor - leaf_count].parent;
                    if constexpr (Count) ++work.visits;
                }
                Link inserted = free_branch;
                if (inserted == none) {
                    require(allocated < branches.size());
                    inserted = static_cast<Link>(leaf_count + allocated);
                    ++allocated;
                } else free_branch = branches[inserted - leaf_count].children[0];
                Branch& branch = branches[inserted - leaf_count];
                const unsigned side = direction(input, position, bit);
                branch.children[side] = new_leaf;
                branch.children[1 - side] = subtree;
                branch.bit = static_cast<Link>(bit);
                branch.newest = position;
                replace<Count>(subtree, inserted, ancestor, work);
                set_parent(subtree, inserted);
                parents[new_leaf] = inserted;
                if constexpr (Count) work.writes += 6;
            }
            positions[new_leaf] = position;
            if constexpr (Count) ++work.writes;
#ifndef STX_MATCH_INDEX_RADIX_FINGER
            while (ancestor != none) {
                Branch& branch = branches[ancestor - leaf_count];
                branch.newest = position;
                ancestor = branch.parent;
                if constexpr (Count) { ++work.visits; ++work.writes; }
            }
#endif
        }
        roots[current_directory] = root;
        if constexpr (Count) ++work.writes;
        if (query && position >= window && best.length < maximum) {
            if constexpr (Count) ++work.visits;
            if constexpr (Count) ++work.compared;
            if (input[position + best.length] == input[position - window + best.length]) {
                const unsigned length = extend<Count>(input, position, position - window, 0, maximum, work);
                if (length >= 3 && length > best.length) best = Match{length, window};
            }
        }
        return best;
    }
};
