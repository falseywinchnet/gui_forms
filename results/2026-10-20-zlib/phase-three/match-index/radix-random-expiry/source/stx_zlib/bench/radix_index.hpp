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
#ifdef STX_MATCH_INDEX_RADIX_SMALL
        if (ancestor < leaf_count || ancestor == none) {
            // A directory holding at most four leaves uses the parent field
            // as its next link. Internal-parent links occupy the other half.
            if (root == leaf) root = ancestor;
            else {
                Link preceding = root;
                while (parents[preceding] != leaf) {
                    preceding = parents[preceding];
                    if constexpr (Count) ++work.visits;
                }
                parents[preceding] = ancestor;
            }
            // advance immediately overwrites this unlinked leaf's position.
            if constexpr (Count) { ++work.root_expirations; ++work.writes; }
            return;
        }
#endif
        if (ancestor == none) {
            root = none;
            if constexpr (Count) ++work.root_expirations;
        }
        else {
            if constexpr (Count) ++work.branch_expirations;
            Branch& branch = branches[ancestor - leaf_count];
            const Link sibling = branch.children[branch.children[0] == leaf ? 1 : 0];
            replace<Count>(ancestor, sibling, branch.parent, work);
            branch.children[0] = free_branch;
            free_branch = ancestor;
            if constexpr (Count) work.writes += 2;
        }
        // The globally oldest leaf cannot be the newest surviving source in
        // any ancestor. No upward maximum repair is required after contraction.
        // Its ring slot is overwritten by advance before the next operation.
    }
#ifdef STX_MATCH_INDEX_RADIX_SMALL
    template<bool Count>
    void promote(std::span<const std::uint8_t> input, Work& work) {
        std::array<Link, 4> leaves{};
        Link cursor = root;
        for (unsigned index = 0; index < leaves.size(); ++index) {
            leaves[index] = cursor;
            cursor = parents[cursor];
        }
        root = leaves[3];
        parents[root] = none;
        for (unsigned index = 3; index != 0; --index) {
            const Link added = leaves[index - 1];
            const std::uint32_t position = positions[added];
            const unsigned maximum = static_cast<unsigned>(std::min<std::size_t>(258, input.size() - position));
            Link leaf = root;
            while (leaf >= leaf_count) {
                const Branch& branch = branches[leaf - leaf_count];
                leaf = branch.children[direction(input, position, branch.bit)];
                if constexpr (Count) { ++work.visits; ++work.compared; }
            }
            const std::uint32_t source = positions[leaf];
            const unsigned length = extend<Count>(input, position, source, 2, maximum, work);
            const unsigned bit = different_bit<Count>(input, position, source, length, maximum, work);
            require(bit != 258 * 8);
            Link subtree = leaf;
            Link ancestor = parents[leaf];
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
            branch.children[side] = added;
            branch.children[1 - side] = subtree;
            branch.bit = static_cast<Link>(bit);
            branch.newest = position;
            replace<Count>(subtree, inserted, ancestor, work);
            set_parent(subtree, inserted);
            parents[added] = inserted;
            if constexpr (Count) { ++work.branch_insertions; work.writes += 6; }
            while (ancestor != none) {
                Branch& enclosing = branches[ancestor - leaf_count];
                enclosing.newest = position;
                ancestor = enclosing.parent;
                if constexpr (Count) { ++work.visits; ++work.writes; }
            }
        }
    }
    template<bool Count>
#ifdef STX_MATCH_INDEX_RADIX_INLINE
    [[gnu::always_inline]]
#endif
    bool insert_small(std::span<const std::uint8_t> input, std::uint32_t position,
        unsigned maximum, bool query, Match& best, Work& work) {
        if (root != none && root >= leaf_count) return false;
        if (position != 0) {
            Link ancestor = parents[(position - 1) & (window - 1)];
            // Any pending branch path belongs to a different directory.
            while (ancestor != none && ancestor >= leaf_count) {
                Branch& branch = branches[ancestor - leaf_count];
                branch.newest = position - 1;
                ancestor = branch.parent;
                if constexpr (Count) { ++work.visits; ++work.writes; }
            }
        }
        adjacent_length = 0;
        unsigned count = 0;
        Link preceding = none;
        Link cursor = root;
        while (cursor != none) {
            const std::uint32_t source = positions[cursor];
            unsigned known = 2;
            unsigned length = 2;
            if (input[position + 2] != input[source + 2]) {
                if constexpr (Count) ++work.compared;
            } else {
                const unsigned shift = position - carried.position;
                if constexpr (share_prefixes) {
                    if (shift < carried.length && source == carried.source + shift) known = std::max(known, carried.length - shift);
                }
                length = extend<Count>(input, position, source, known, maximum, work);
            }
            if constexpr (Count) { ++work.visits; work.reused += known; }
            if (query && length >= 3 && length > best.length) best = Match{length, position - source};
            const bool duplicate = length == maximum && different_bit<Count>(input, position, source, length, maximum, work) == 258 * 8;
            if (duplicate) {
                if constexpr (share_prefixes) carried = Carried{position, source, length};
                const Link following = parents[cursor];
                if (preceding == none) root = following;
                else parents[preceding] = following;
                positions[cursor] = absent;
                if constexpr (Count) { ++work.duplicate_insertions; work.writes += 2; }
                break;
            }
            preceding = cursor;
            cursor = parents[cursor];
            ++count;
        }
        if (count == 4) {
            promote<Count>(input, work);
            return false;
        }
        const Link added = static_cast<Link>(position & (window - 1));
        parents[added] = root;
        positions[added] = position;
        root = added;
        if constexpr (Count) { ++work.root_insertions; work.writes += 3; }
        return true;
    }
#endif
    template<bool Count>
#ifdef STX_MATCH_INDEX_RADIX_INLINE
    [[gnu::always_inline]]
#endif
    Match advance(std::span<const std::uint8_t> input, std::uint32_t position,
        bool query, Work& work) {
        Match best{};
        const unsigned maximum = static_cast<unsigned>(std::min<std::size_t>(258, input.size() - position));
        if (maximum < 3) return best;
        // Exclude exactly one still-legal source before recycling its slot.
        // Query it separately below, after nearer sources have settled ties.
        const Link new_leaf = static_cast<Link>(position & (window - 1));
        const unsigned current_directory = directory(input, position);
        bool oldest_can_match = false;
        if (position >= window && positions[new_leaf] == position - window) {
            const unsigned old_directory = directory(input, position - window);
            oldest_can_match = old_directory == current_directory;
            root = roots[old_directory];
            const Link old_root = root;
            erase_oldest<Count>(position - window, work);
            if (root != old_root) {
                roots[old_directory] = root;
                if constexpr (Count) ++work.writes;
            }
        }
        root = roots[current_directory];
#ifdef STX_MATCH_INDEX_RADIX_SMALL
        if (insert_small<Count>(input, position, maximum, query, best, work)) {
            roots[current_directory] = root;
            if constexpr (Count) ++work.writes;
            if (query && oldest_can_match && best.length < maximum) {
                if constexpr (Count) { ++work.visits; ++work.compared; }
                if (input[position + best.length] == input[position - window + best.length]) {
                    const unsigned length = extend<Count>(input, position, position - window, 0, maximum, work);
                    if (length >= 3 && length > best.length) best = Match{length, window};
                }
            }
            return best;
        }
#endif
        Link start = root;
#ifdef STX_MATCH_INDEX_RADIX_FINGER
        unsigned adjacent_bit = 0;
        if (position != 0) {
            const unsigned known_adjacent = adjacent_length > 0 ? adjacent_length - 1 : 0;
            if constexpr (Count) work.reused += known_adjacent;
            if (known_adjacent == 0 && input[position] != input[position - 1]) {
                adjacent_length = 0;
                if constexpr (Count) ++work.compared;
                if constexpr (Count) ++work.predictor_rejections;
            } else adjacent_length = extend<Count>(input, position, position - 1, known_adjacent, maximum, work);
            adjacent_bit = different_bit<Count>(input, position, position - 1, adjacent_length, maximum, work);
            const Link previous_leaf = static_cast<Link>((position - 1) & (window - 1));
            if (adjacent_bit == 258 * 8) {
                if constexpr (Count) ++work.duplicate_insertions;
                // The previous leaf is the exact key and wins every query tie.
                // Its ancestors stay on the implicit newest-source path.
                replace<Count>(previous_leaf, new_leaf, parents[previous_leaf], work);
                positions[previous_leaf] = absent;
                positions[new_leaf] = position;
                roots[current_directory] = root;
                if constexpr (Count) work.writes += 3;
                if constexpr (share_prefixes) carried = Carried{position, position - 1, adjacent_length};
                if (query) return Match{maximum, 1};
                return best;
            }
            Link departed = previous_leaf;
            Link ancestor = parents[previous_leaf];
#ifdef STX_MATCH_INDEX_RADIX_SMALL
            if (ancestor < leaf_count) ancestor = none;
#endif
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
            if constexpr (Count) ++work.root_insertions;
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
                if constexpr (Count) ++work.duplicate_insertions;
                replace<Count>(leaf, new_leaf, ancestor, work);
                positions[leaf] = absent;
                if constexpr (Count) ++work.writes;
            } else {
                if constexpr (Count) ++work.branch_insertions;
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
        if (query && oldest_can_match && best.length < maximum) {
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
