#include "correspondence_model.hpp"

#include <algorithm>
#include <cctype>
#include <utility>

namespace gui_forms::correspondence {
namespace {

struct FixtureRow final {
    Folder folder;
    std::string_view correspondent;
    std::string_view address;
    std::string_view subject;
    std::string_view date;
    std::string_view preview;
    std::string_view body;
    bool read;
    bool flagged;
};

// Fictional Harbour Lane Observatory volunteer society. Names, addresses, and
// events are invented; the `.example` domain is reserved and never resolves.
constexpr std::array<FixtureRow, 40> fixture_rows{{
    // Inbox (16; six unread)
    {Folder::inbox, "Priya Raman", "priya.raman@harbourlane.example",
     "Dome shutter motor: replacement quote", "2026-10-10 17:42",
     "The second quote came in under budget, so I would like to order on Monday.",
     "Hello all,\n\nThe second quote for the dome shutter motor came in at 1,840, which is "
     "under the 2,000 the committee approved in September. Lead time is three weeks.\n\n"
     "Unless anyone objects by Monday noon I will place the order and book the "
     "crane for the first clear weekend in November.\n\nPriya", false, true},
    {Folder::inbox, "Night Sky Bulletin", "bulletin@harbourlane.example",
     "Observing conditions, week 41", "2026-10-10 06:00",
     "Clear skies forecast Tuesday and Wednesday night; seeing expected to be above average.",
     "Observing conditions for week 41\n\nTuesday: clear, transparency good, seeing 2/5.\n"
     "Wednesday: clear until 02:00, seeing 1/5 (excellent).\nThursday to Saturday: "
     "broken cloud, no public session.\n\nThe Tuesday public session is confirmed for "
     "20:30.", false, false},
    {Folder::inbox, "Tomasz Wierzba", "t.wierzba@harbourlane.example",
     "Re: Rota for the October public nights", "2026-10-09 21:15",
     "I can take the 14th and the 28th but not the 21st; could someone swap?",
     "Hi,\n\nI can take the 14th and the 28th but not the 21st, as I am away that week. "
     "Would anyone be willing to swap the 21st for a November date?\n\nTomasz", false, false},
    {Folder::inbox, "Mei-Ling Chao", "mei-ling.chao@harbourlane.example",
     "Photometry results for the eclipsing binary", "2026-10-09 14:05",
     "The light curve from last Friday is attached in the shared folder; minimum came 11 minutes early.",
     "Everyone,\n\nThe light curve from last Friday is in the shared folder under "
     "photometry/2026-10-03. The primary minimum arrived eleven minutes earlier than "
     "the ephemeris predicted, which is consistent with the period change Dana suggested "
     "in March.\n\nI would like to repeat the run on the 17th if the sky cooperates.\n\n"
     "Mei-Ling", true, true},
    {Folder::inbox, "Facilities Desk", "facilities@harbourlane.example",
     "Heating maintenance on 15 October", "2026-10-09 09:30",
     "The boiler service is booked for the 15th between 08:00 and 12:00; the warm room will be closed.",
     "The annual boiler service is booked for Thursday 15 October between 08:00 and "
     "12:00. The warm room and kitchen will be closed during the visit.\n\nThe dome and "
     "the library are unaffected.", true, false},
    {Folder::inbox, "Dana Whitfield", "dana.whitfield@harbourlane.example",
     "Library: three overdue loans", "2026-10-08 18:20",
     "Norton's Star Atlas, the Burnham volumes, and the 2025 Handbook are all overdue.",
     "Hello,\n\nThree library loans are now more than a month overdue: Norton's Star "
     "Atlas, Burnham's Celestial Handbook volume 2, and the 2025 Observer's Handbook.\n\n"
     "If you have one of them, please return it to the shelf by the next public night. "
     "No fines, just the shelf.\n\nDana", false, false},
    {Folder::inbox, "Grants Office", "grants@harbourlane.example",
     "Outreach grant: report due 31 October", "2026-10-08 11:00",
     "The end-of-year report for the schools outreach grant is due on the 31st.",
     "Dear committee,\n\nThe end-of-year report for the schools outreach grant is due on "
     "31 October. The template is unchanged from last year: attendance figures, a short "
     "narrative, and the expenditure summary.\n\nPlease send it to this address.", true, false},
    {Folder::inbox, "Ólafur Sigurðsson", "olafur@harbourlane.example",
     "Spectrograph calibration lamp", "2026-10-07 22:48",
     "The neon lamp is flickering again; I have swapped to the argon lamp for now.",
     "Hi all,\n\nThe neon calibration lamp started flickering again during tonight's run. "
     "I have swapped the mount over to the argon lamp, so the wavelength solution for "
     "tonight uses the argon lines only.\n\nI will order a replacement neon lamp unless "
     "someone has a spare.\n\nÓlafur", false, false},
    {Folder::inbox, "Priya Raman", "priya.raman@harbourlane.example",
     "Committee minutes, 1 October", "2026-10-06 13:10",
     "Minutes from the October committee meeting are in the shared folder for review.",
     "The minutes from the 1 October committee meeting are in the shared folder under "
     "committee/2026-10-01. Corrections by Friday please; they will be adopted at the "
     "November meeting.\n\nPriya", true, false},
    {Folder::inbox, "Sam Okafor", "sam.okafor@harbourlane.example",
     "New member introduction", "2026-10-05 19:55",
     "Hello, I joined last week after the open evening and would love to help with the imaging group.",
     "Hello everyone,\n\nI joined last week after the open evening. I have a small "
     "refractor and some experience with planetary imaging, and I would love to help "
     "with the imaging group if there is room.\n\nLooking forward to meeting you on a "
     "public night.\n\nSam", false, false},
    {Folder::inbox, "Tomasz Wierzba", "t.wierzba@harbourlane.example",
     "Mirror recoating schedule", "2026-10-04 16:30",
     "The coating lab can take the primary in the first week of December.",
     "Hi,\n\nThe coating lab has confirmed they can take the 400 mm primary in the first "
     "week of December, with a two-week turnaround. That means no imaging over the "
     "holidays, but the mirror would be ready for the January sessions.\n\nTomasz", true, false},
    {Folder::inbox, "Facilities Desk", "facilities@harbourlane.example",
     "Car park gate code change", "2026-10-03 08:45",
     "The gate code changes on Monday; the new code is on the notice board in the warm room.",
     "The car park gate code changes on Monday 6 October. The new code is posted on the "
     "notice board in the warm room and is not sent by mail.", true, false},
    {Folder::inbox, "Mei-Ling Chao", "mei-ling.chao@harbourlane.example",
     "Re: Photometry software licence", "2026-10-02 12:15",
     "The educational licence renewal went through; the key is in the shared folder.",
     "Hi,\n\nThe educational licence renewal went through this morning. The key is in "
     "the shared folder under software/licences. It covers five seats until October "
     "2027.\n\nMei-Ling", true, false},
    {Folder::inbox, "Dana Whitfield", "dana.whitfield@harbourlane.example",
     "Talk proposal: variable stars for beginners", "2026-10-01 20:05",
     "I could give a 40-minute talk on variable star observing at the November members' evening.",
     "Hello,\n\nI could give a forty-minute talk on variable star observing at the "
     "November members' evening, aimed at people who have never submitted an estimate. "
     "Would that fit the programme?\n\nDana", true, true},
    {Folder::inbox, "Night Sky Bulletin", "bulletin@harbourlane.example",
     "Observing conditions, week 40", "2026-09-29 06:00",
     "Unsettled all week; the public session on the 30th is cancelled.",
     "Observing conditions for week 40\n\nUnsettled weather all week with low cloud "
     "most nights. The public session on the 30th is cancelled.", true, false},
    {Folder::inbox, "Sam Okafor", "sam.okafor@harbourlane.example",
     "Thank you for the open evening", "2026-09-27 23:40",
     "Thank you to everyone who ran the open evening; the view of Saturn was unforgettable.",
     "Thank you to everyone who ran Saturday's open evening. The view of Saturn through "
     "the main telescope was unforgettable, and the volunteers were endlessly "
     "patient with my questions.\n\nSam", true, false},
    // Drafts (3)
    {Folder::drafts, "Grants Office", "grants@harbourlane.example",
     "Outreach grant report, draft", "2026-10-10 15:20",
     "Attendance across the six school visits totalled 412 pupils; expenditure summary to follow.",
     "Dear Grants Office,\n\nPlease find below the end-of-year report for the schools "
     "outreach grant.\n\nAttendance across the six school visits totalled 412 pupils. "
     "The expenditure summary follows.\n\n[expenditure table to be added]", true, false},
    {Folder::drafts, "Tomasz Wierzba", "t.wierzba@harbourlane.example",
     "Re: Rota for the October public nights", "2026-10-09 21:30",
     "I can swap you the 21st for 11 November if that helps.",
     "Hi Tomasz,\n\nI can swap you the 21st for 11 November if that helps.\n\n", true, false},
    {Folder::drafts, "All members", "members@harbourlane.example",
     "November members' evening programme", "2026-10-07 10:00",
     "Programme for the November members' evening: talk, equipment clinic, and observing if clear.",
     "Dear members,\n\nThe programme for the November members' evening is:\n\n19:30 "
     "Welcome\n19:45 Talk\n20:30 Equipment clinic\n21:00 Observing, weather permitting", true, false},
    // Sent (8)
    {Folder::sent, "Priya Raman", "priya.raman@harbourlane.example",
     "Re: Committee minutes, 1 October", "2026-10-06 14:02",
     "One correction: the treasurer's figure in item 4 should read 2,000, not 200.",
     "Hi Priya,\n\nOne correction: the treasurer's figure in item 4 should read 2,000, "
     "not 200. Otherwise the minutes look right to me.\n\nThanks", true, false},
    {Folder::sent, "Sam Okafor", "sam.okafor@harbourlane.example",
     "Re: New member introduction", "2026-10-05 20:30",
     "Welcome! The imaging group meets on the second Wednesday; bring the refractor.",
     "Hi Sam,\n\nWelcome to the society. The imaging group meets on the second Wednesday "
     "of the month in the warm room; do bring the refractor along.\n\nSee you on a public "
     "night.", true, false},
    {Folder::sent, "Facilities Desk", "facilities@harbourlane.example",
     "Dome door closer", "2026-10-04 09:12",
     "The dome door closer is sticking again; could the next maintenance visit look at it?",
     "Hello,\n\nThe dome door closer is sticking again and the door no longer latches "
     "on its own. Could the next maintenance visit have a look at it?\n\nThank you", true, false},
    {Folder::sent, "Mei-Ling Chao", "mei-ling.chao@harbourlane.example",
     "Re: Photometry software licence", "2026-10-02 12:40",
     "Thank you; I have installed the key on the warm room machine.",
     "Thank you, Mei-Ling. I have installed the key on the warm room machine and it "
     "reports five seats available.", true, false},
    {Folder::sent, "Dana Whitfield", "dana.whitfield@harbourlane.example",
     "Re: Talk proposal: variable stars for beginners", "2026-10-01 21:10",
     "Yes please; forty minutes at 19:45 on the November members' evening is yours.",
     "Hi Dana,\n\nYes please. Forty minutes at 19:45 on the November members' evening "
     "is yours. I will put it on the programme.\n\nThanks", true, false},
    {Folder::sent, "Grants Office", "grants@harbourlane.example",
     "Outreach grant: interim figures", "2026-09-26 11:45",
     "Interim attendance figures for the first four school visits, as requested.",
     "Dear Grants Office,\n\nAs requested, the interim attendance figures for the first "
     "four school visits are 71, 64, 88, and 59 pupils.\n\nThe final report will follow "
     "in October.", true, false},
    {Folder::sent, "All members", "members@harbourlane.example",
     "Open evening volunteers needed", "2026-09-20 18:00",
     "We need four more volunteers for the open evening on the 27th.",
     "Dear members,\n\nWe need four more volunteers for the open evening on Saturday "
     "27 September: two on the main telescope, one on the door, and one in the warm "
     "room.\n\nPlease reply if you can help.", true, false},
    {Folder::sent, "Ólafur Sigurðsson", "olafur@harbourlane.example",
     "Spare neon lamp", "2026-09-18 22:05",
     "There is a spare neon lamp in the second drawer of the spectrograph cabinet.",
     "Hi Ólafur,\n\nThere should be a spare neon lamp in the second drawer of the "
     "spectrograph cabinet, still boxed.", true, false},
    // Archive (7)
    {Folder::archive, "Priya Raman", "priya.raman@harbourlane.example",
     "Committee minutes, 3 September", "2026-09-08 12:30",
     "Minutes from the September committee meeting, adopted without correction.",
     "The minutes from the 3 September committee meeting were adopted without "
     "correction and are filed in the shared folder.\n\nPriya", true, false},
    {Folder::archive, "Grants Office", "grants@harbourlane.example",
     "Outreach grant awarded", "2026-01-14 10:20",
     "We are pleased to confirm the schools outreach grant for 2026.",
     "Dear committee,\n\nWe are pleased to confirm the schools outreach grant for 2026. "
     "Conditions and the reporting template are in the shared folder.", true, true},
    {Folder::archive, "Tomasz Wierzba", "t.wierzba@harbourlane.example",
     "Mirror cell inspection report", "2026-08-21 15:50",
     "The mirror cell inspection found no cracks; the coating is the only concern.",
     "Hi,\n\nThe mirror cell inspection found no cracks in the glass or the cell. The "
     "coating is the only concern, hence the recoating proposal.\n\nTomasz", true, false},
    {Folder::archive, "Facilities Desk", "facilities@harbourlane.example",
     "Electrical inspection certificate", "2026-07-02 09:00",
     "The five-year electrical inspection certificate has been renewed.",
     "The five-year electrical inspection certificate has been renewed and a copy is on "
     "the notice board.", true, false},
    {Folder::archive, "Mei-Ling Chao", "mei-ling.chao@harbourlane.example",
     "Photometry results for the spring campaign", "2026-05-30 13:15",
     "Final results of the spring campaign are in the shared folder.",
     "Everyone,\n\nThe final results of the spring campaign are in the shared folder "
     "under photometry/2026-spring. Thirty-one nights, nine targets.\n\nMei-Ling", true, false},
    {Folder::archive, "Dana Whitfield", "dana.whitfield@harbourlane.example",
     "Library catalogue update", "2026-04-11 17:25",
     "The library catalogue has been updated with the 2026 donations.",
     "The library catalogue has been updated with the 2026 donations. A printed copy is "
     "on the library desk.\n\nDana", true, false},
    {Folder::archive, "Night Sky Bulletin", "bulletin@harbourlane.example",
     "Observing conditions, week 12", "2026-03-16 06:00",
     "Clear and cold all week; excellent conditions for the Messier marathon.",
     "Observing conditions for week 12\n\nClear and cold all week with excellent "
     "transparency. Conditions for Saturday's Messier marathon look ideal.", true, false},
    // Junk (4; three unread)
    {Folder::junk, "Prize Draw", "winner@prizes.example",
     "You have been selected", "2026-10-09 03:12",
     "Congratulations, you have been selected for a reward. Confirm your details today.",
     "Congratulations, you have been selected for a reward. Confirm your details today "
     "to claim it.", false, false},
    {Folder::junk, "Telescope Outlet", "deals@telescope-outlet.example",
     "Final hours: 70% off eyepieces", "2026-10-08 00:01",
     "Final hours of our biggest eyepiece sale ever. Do not miss out.",
     "Final hours of our biggest eyepiece sale ever. Do not miss out.", false, false},
    {Folder::junk, "Account Services", "noreply@account-services.example",
     "Your account will be suspended", "2026-10-06 04:45",
     "Your account will be suspended unless you verify your password within 24 hours.",
     "Your account will be suspended unless you verify your password within 24 hours.", false, false},
    {Folder::junk, "Newsletter", "news@newsletter.example",
     "Ten gadgets every stargazer needs", "2026-10-02 07:30",
     "Ten gadgets every stargazer needs this autumn.",
     "Ten gadgets every stargazer needs this autumn.", true, false},
    // Trash (2)
    {Folder::trash, "Facilities Desk", "facilities@harbourlane.example",
     "Test message, please ignore", "2026-10-07 08:00",
     "This is a test of the new mailing list; please ignore.",
     "This is a test of the new mailing list; please ignore.", true, false},
    {Folder::trash, "Night Sky Bulletin", "bulletin@harbourlane.example",
     "Observing conditions, week 39", "2026-09-22 06:00",
     "Mixed conditions; Thursday is the best bet.",
     "Observing conditions for week 39\n\nMixed conditions; Thursday is the best bet.", true, false},
}};

[[nodiscard]] bool is_outgoing(Folder folder) noexcept {
    return folder == Folder::drafts || folder == Folder::sent;
}

[[nodiscard]] std::string quoted_body(const Message& original) {
    std::string result = "On " + original.date + ", " + original.correspondent + " wrote:\n";
    std::string line;
    for (const char character : original.body) {
        if (character == '\n') {
            result += "> " + line + "\n";
            line.clear();
        } else {
            line.push_back(character);
        }
    }
    if (!line.empty()) {
        result += "> " + line + "\n";
    }
    return result;
}

[[nodiscard]] std::string first_line(std::string_view text) {
    const std::size_t newline = text.find('\n');
    return std::string(newline == std::string_view::npos ? text : text.substr(0, newline));
}

}  // namespace

std::string_view folder_name(const Folder folder) noexcept {
    switch (folder) {
    case Folder::inbox: return "Inbox";
    case Folder::drafts: return "Drafts";
    case Folder::sent: return "Sent";
    case Folder::archive: return "Archive";
    case Folder::junk: return "Junk";
    case Folder::trash: return "Trash";
    }
    return "Inbox";
}

std::string_view folder_token(const Folder folder) noexcept {
    switch (folder) {
    case Folder::inbox: return "inbox";
    case Folder::drafts: return "drafts";
    case Folder::sent: return "sent";
    case Folder::archive: return "archive";
    case Folder::junk: return "junk";
    case Folder::trash: return "trash";
    }
    return "inbox";
}

std::optional<Folder> folder_from_token(const std::string_view token) noexcept {
    for (const Folder folder : all_folders) {
        if (folder_token(folder) == token) {
            return folder;
        }
    }
    return std::nullopt;
}

std::vector<Message> fixture_messages() {
    std::vector<Message> result;
    result.reserve(fixture_rows.size());
    std::uint32_t id = 1U;
    for (const FixtureRow& row : fixture_rows) {
        Message message;
        message.id = id;
        message.folder = row.folder;
        message.correspondent = std::string(row.correspondent);
        message.address = std::string(row.address);
        message.subject = std::string(row.subject);
        message.date = std::string(row.date);
        message.preview = std::string(row.preview);
        message.body = std::string(row.body);
        message.outgoing = is_outgoing(row.folder);
        message.read = row.read;
        message.flagged = row.flagged;
        result.push_back(std::move(message));
        ++id;
    }
    return result;
}

bool contains_ignoring_case(const std::string_view haystack,
                            const std::string_view needle) noexcept {
    if (needle.empty()) return true;
    if (needle.size() > haystack.size()) return false;
    for (std::size_t start = 0; start + needle.size() <= haystack.size(); ++start) {
        bool matched = true;
        for (std::size_t offset = 0; offset < needle.size(); ++offset) {
            const unsigned char left = static_cast<unsigned char>(haystack[start + offset]);
            const unsigned char right = static_cast<unsigned char>(needle[offset]);
            if (std::tolower(left) != std::tolower(right)) {
                matched = false;
                break;
            }
        }
        if (matched) return true;
    }
    return false;
}

CorrespondenceModel::CorrespondenceModel()
    : messages_(fixture_messages()),
      next_id_(static_cast<std::uint32_t>(messages_.size()) + 1U) {}

const Message* CorrespondenceModel::find(const std::uint32_t id) const noexcept {
    for (const Message& message : messages_) {
        if (message.id == id) return &message;
    }
    return nullptr;
}

Message* CorrespondenceModel::find_mutable(const std::uint32_t id) noexcept {
    for (Message& message : messages_) {
        if (message.id == id) return &message;
    }
    return nullptr;
}

bool CorrespondenceModel::select_folder(const Folder folder) {
    if (folder_ == folder) return false;
    folder_ = folder;
    selected_.reset();
    bump();
    return true;
}

bool CorrespondenceModel::select_message(const std::optional<std::uint32_t> id) {
    if (!id) {
        if (!selected_) return false;
        selected_.reset();
        bump();
        return true;
    }
    Message* message = find_mutable(*id);
    if (message == nullptr || (*message).folder != folder_ || !matches_search(*message)) {
        return false;
    }
    if (selected_ == id) return false;
    selected_ = id;
    (*message).read = true;
    bump();
    return true;
}

bool CorrespondenceModel::mark_read(const std::uint32_t id, const bool read) {
    Message* message = find_mutable(id);
    if (message == nullptr || (*message).read == read) return false;
    (*message).read = read;
    bump();
    return true;
}

bool CorrespondenceModel::toggle_flag(const std::uint32_t id) {
    Message* message = find_mutable(id);
    if (message == nullptr) return false;
    (*message).flagged = !(*message).flagged;
    bump();
    return true;
}

std::optional<std::uint32_t> CorrespondenceModel::neighbour_of(const std::uint32_t id) const {
    const std::vector<std::uint32_t> visible = visible_messages();
    const std::vector<std::uint32_t>::const_iterator position =
        std::find(visible.begin(), visible.end(), id);
    if (position == visible.end()) return std::nullopt;
    const std::size_t index = static_cast<std::size_t>(position - visible.begin());
    if (index + 1U < visible.size()) return visible[index + 1U];
    if (index > 0U) return visible[index - 1U];
    return std::nullopt;
}

bool CorrespondenceModel::move_to(const std::uint32_t id, const Folder destination) {
    Message* message = find_mutable(id);
    if (message == nullptr || (*message).folder == destination) return false;
    const std::optional<std::uint32_t> replacement =
        selected_ == id ? neighbour_of(id) : std::optional<std::uint32_t>{};
    (*message).folder = destination;
    if (selected_ == id) {
        selected_ = replacement;
    }
    bump();
    return true;
}

bool CorrespondenceModel::archive(const std::uint32_t id) {
    return move_to(id, Folder::archive);
}

bool CorrespondenceModel::delete_message(const std::uint32_t id) {
    const Message* message = find(id);
    if (message == nullptr) return false;
    if ((*message).folder != Folder::trash) {
        return move_to(id, Folder::trash);
    }
    const std::optional<std::uint32_t> replacement =
        selected_ == id ? neighbour_of(id) : std::optional<std::uint32_t>{};
    for (std::vector<Message>::iterator it = messages_.begin(); it != messages_.end(); ++it) {
        if ((*it).id == id) {
            messages_.erase(it);
            break;
        }
    }
    if (selected_ == id) {
        selected_ = replacement;
    }
    bump();
    return true;
}

std::size_t CorrespondenceModel::count(const Folder folder) const noexcept {
    std::size_t result = 0U;
    for (const Message& message : messages_) {
        if (message.folder == folder) ++result;
    }
    return result;
}

std::size_t CorrespondenceModel::unread_count(const Folder folder) const noexcept {
    std::size_t result = 0U;
    for (const Message& message : messages_) {
        if (message.folder == folder && !message.read) ++result;
    }
    return result;
}

bool CorrespondenceModel::set_search(std::string text) {
    if (search_ == text) return false;
    search_ = std::move(text);
    if (selected_) {
        const Message* message = find(*selected_);
        if (message == nullptr || !matches_search(*message)) {
            selected_.reset();
        }
    }
    bump();
    return true;
}

bool CorrespondenceModel::matches_search(const Message& message) const noexcept {
    if (search_.empty()) return true;
    return contains_ignoring_case(message.correspondent, search_) ||
           contains_ignoring_case(message.subject, search_) ||
           contains_ignoring_case(message.preview, search_);
}

std::vector<std::uint32_t> CorrespondenceModel::visible_messages() const {
    std::vector<std::uint32_t> result;
    for (const Message& message : messages_) {
        if (message.folder == folder_ && matches_search(message)) {
            result.push_back(message.id);
        }
    }
    return result;
}

std::uint32_t CorrespondenceModel::add_draft(std::string correspondent, std::string address,
                                             std::string subject, std::string body) {
    Message draft;
    draft.id = next_id_++;
    draft.folder = Folder::drafts;
    draft.correspondent = std::move(correspondent);
    draft.address = std::move(address);
    draft.subject = std::move(subject);
    // A fixed authoring time keeps the fixture deterministic; drafts sort to
    // the top of the Drafts list because the list is newest-first by position.
    draft.date = "2026-10-11 09:00";
    draft.preview = first_line(body);
    draft.body = std::move(body);
    draft.outgoing = true;
    draft.read = true;
    // Insert before the first existing draft so the newest draft leads.
    std::vector<Message>::iterator position = messages_.begin();
    while (position != messages_.end() && (*position).folder != Folder::drafts) {
        ++position;
    }
    const std::uint32_t id = draft.id;
    messages_.insert(position, std::move(draft));
    folder_ = Folder::drafts;
    search_.clear();
    selected_ = id;
    bump();
    return id;
}

std::uint32_t CorrespondenceModel::create_draft() {
    return add_draft("", "", "Untitled message", "");
}

std::optional<std::uint32_t> CorrespondenceModel::reply_to(const std::uint32_t id) {
    const Message* original = find(id);
    if (original == nullptr) return std::nullopt;
    const std::string subject = (*original).subject.rfind("Re: ", 0) == 0
        ? (*original).subject : "Re: " + (*original).subject;
    return add_draft((*original).correspondent, (*original).address, subject,
                     "\n\n" + quoted_body(*original));
}

std::optional<std::uint32_t> CorrespondenceModel::forward(const std::uint32_t id) {
    const Message* original = find(id);
    if (original == nullptr) return std::nullopt;
    const std::string subject = (*original).subject.rfind("Fwd: ", 0) == 0
        ? (*original).subject : "Fwd: " + (*original).subject;
    return add_draft("", "", subject,
                     "\n\n---------- Forwarded message ----------\nFrom: " +
                     (*original).correspondent + " <" + (*original).address + ">\nDate: " +
                     (*original).date + "\nSubject: " + (*original).subject + "\n\n" +
                     (*original).body);
}

}  // namespace gui_forms::correspondence
