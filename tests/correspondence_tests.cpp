// Model-only tests for the Correspondence demonstration. No controls, no
// window: the fixture and CorrespondenceModel must be deterministic and
// self-consistent on their own.

#include "../demo/correspondence/correspondence_model.hpp"

#include <cstdlib>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace {

using gui_forms::correspondence::CorrespondenceModel;
using gui_forms::correspondence::Folder;
using gui_forms::correspondence::Message;

void expect(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "correspondence model failure: " << message << '\n';
        std::exit(1);
    }
}

[[nodiscard]] bool same_message(const Message& left, const Message& right)
{
    return left.id == right.id && left.folder == right.folder &&
           left.correspondent == right.correspondent && left.address == right.address &&
           left.subject == right.subject && left.date == right.date &&
           left.preview == right.preview && left.body == right.body &&
           left.outgoing == right.outgoing && left.read == right.read &&
           left.flagged == right.flagged;
}

[[nodiscard]] bool contains(const std::vector<std::uint32_t>& values, std::uint32_t value)
{
    for (const std::uint32_t candidate : values) {
        if (candidate == value) return true;
    }
    return false;
}

}  // namespace

int main()
{
    using gui_forms::correspondence::all_folders;
    using gui_forms::correspondence::contains_ignoring_case;
    using gui_forms::correspondence::fixture_messages;
    using gui_forms::correspondence::folder_from_token;
    using gui_forms::correspondence::folder_name;
    using gui_forms::correspondence::folder_token;

    // Fixture determinism and shape.
    const std::vector<Message> first = fixture_messages();
    const std::vector<Message> second = fixture_messages();
    expect(first.size() == 40U, "fixture holds forty messages");
    expect(first.size() == second.size(), "fixture size is deterministic");
    std::set<std::uint32_t> ids;
    for (std::size_t index = 0; index < first.size(); ++index) {
        expect(same_message(first[index], second[index]), "fixture content is deterministic");
        expect(ids.insert(first[index].id).second, "fixture IDs are unique");
        expect(first[index].id == static_cast<std::uint32_t>(index) + 1U,
               "fixture IDs are dense and ordered");
        expect(!first[index].subject.empty() && !first[index].body.empty() &&
                   !first[index].date.empty(),
               "every fixture message has subject, body and date");
        expect(first[index].outgoing ==
                   (first[index].folder == Folder::drafts || first[index].folder == Folder::sent),
               "drafts and sent mail are outgoing; everything else is incoming");
    }
    for (const Folder folder : all_folders) {
        expect(folder_from_token(folder_token(folder)) == folder, "folder tokens round-trip");
        expect(!folder_name(folder).empty(), "every folder has a display name");
    }
    expect(!folder_from_token("outbox").has_value(), "unknown folder tokens are refused");

    expect(contains_ignoring_case("Photometry Results", "photometry"), "search ignores case");
    expect(contains_ignoring_case("abc", ""), "empty needle matches everything");
    expect(!contains_ignoring_case("ab", "abc"), "longer needle never matches");

    // Initial state and counts.
    CorrespondenceModel model;
    expect(model.total_count() == 40U, "model starts with the full fixture");
    expect(model.selected_folder() == Folder::inbox, "model starts in Inbox");
    expect(!model.selected_message().has_value(), "model starts with no selection");
    expect(model.count(Folder::inbox) == 16U && model.count(Folder::drafts) == 3U &&
               model.count(Folder::sent) == 8U && model.count(Folder::archive) == 7U &&
               model.count(Folder::junk) == 4U && model.count(Folder::trash) == 2U,
           "folder counts match the fixture");
    expect(model.unread_count(Folder::inbox) == 6U && model.unread_count(Folder::junk) == 3U &&
               model.unread_count(Folder::drafts) == 0U && model.unread_count(Folder::sent) == 0U &&
               model.unread_count(Folder::archive) == 0U && model.unread_count(Folder::trash) == 0U,
           "unread counts match the fixture");

    // Folder selection filters the visible list.
    std::vector<std::uint32_t> visible = model.visible_messages();
    expect(visible.size() == 16U, "Inbox shows sixteen messages");
    for (const std::uint32_t id : visible) {
        const Message* message = model.find(id);
        expect(message != nullptr && (*message).folder == Folder::inbox,
               "visible messages belong to the selected folder");
    }
    const std::uint64_t revision_before_folder = model.revision();
    expect(model.select_folder(Folder::sent), "folder selection changes state");
    expect(!model.select_folder(Folder::sent), "reselecting the same folder is a no-op");
    expect(model.revision() == revision_before_folder + 1U, "folder selection bumps revision once");
    expect(model.visible_messages().size() == 8U, "Sent shows eight messages");
    expect(model.select_folder(Folder::inbox), "return to Inbox");

    // Message selection marks read and is bounded to the visible list.
    expect(!model.select_message(20U), "a message in another folder cannot be selected");
    expect(!(*model.find(1U)).read, "fixture message 1 begins unread");
    expect(model.select_message(1U), "visible message selects");
    expect(model.selected_message() == std::optional<std::uint32_t>{1U}, "selection is retained");
    expect((*model.find(1U)).read, "selecting a message marks it read");
    expect(model.unread_count(Folder::inbox) == 5U, "unread count follows the read mark");
    expect(!model.select_message(1U), "reselecting the same message is a no-op");
    expect(model.mark_read(1U, false), "mark unread changes state");
    expect(model.unread_count(Folder::inbox) == 6U, "unread count follows mark unread");
    expect(!model.mark_read(1U, false), "mark unread twice is a no-op");
    expect(model.mark_read(1U, true), "mark read again");

    // Flags.
    expect((*model.find(1U)).flagged, "fixture message 1 begins flagged");
    expect(model.toggle_flag(1U), "toggle flag changes state");
    expect(!(*model.find(1U)).flagged, "flag toggles off");
    expect(model.toggle_flag(1U) && (*model.find(1U)).flagged, "flag toggles back on");
    expect(!model.toggle_flag(999U), "unknown IDs are refused");

    // Archive moves the message, updates counts, and selects a neighbour.
    expect(model.archive(1U), "archive moves the selected message");
    expect((*model.find(1U)).folder == Folder::archive, "archived message lives in Archive");
    expect(model.count(Folder::inbox) == 15U && model.count(Folder::archive) == 8U,
           "archive updates both folder counts");
    expect(model.selected_message() == std::optional<std::uint32_t>{2U},
           "archiving the selected message selects the next visible one");
    expect((*model.find(2U)).read == false,
           "neighbour selection after removal does not mark the neighbour read");
    expect(!model.archive(1U), "archiving an archived message is refused");
    expect(!contains(model.visible_messages(), 1U), "archived message leaves the Inbox list");

    // Delete moves to Trash, then removes permanently.
    expect(model.select_message(2U) == false && model.selected_message() == std::optional<std::uint32_t>{2U},
           "selection already points at message 2");
    expect(model.delete_message(2U), "delete moves to Trash");
    expect((*model.find(2U)).folder == Folder::trash, "deleted message lives in Trash");
    expect(model.count(Folder::trash) == 3U && model.count(Folder::inbox) == 14U,
           "delete updates both folder counts");
    expect(model.selected_message() == std::optional<std::uint32_t>{3U},
           "deleting the selected message selects the next visible one");
    expect(model.select_folder(Folder::trash) && model.select_message(2U),
           "deleted message is selectable in Trash");
    expect(model.delete_message(2U), "deleting from Trash removes permanently");
    expect(model.find(2U) == nullptr, "permanently deleted message is gone");
    expect(model.total_count() == 39U && model.count(Folder::trash) == 2U,
           "permanent delete lowers the totals");
    expect(model.selected_message().has_value() &&
               (*model.find(*model.selected_message())).folder == Folder::trash,
           "permanent delete selects a Trash neighbour");
    expect(model.select_folder(Folder::inbox), "return to Inbox");

    // Search filters within the folder and deselects hidden messages.
    expect(model.select_message(4U), "select the photometry message");
    expect(model.set_search("PHOTOMETRY"), "search text changes state");
    visible = model.visible_messages();
    expect(visible.size() == 2U && contains(visible, 4U) && contains(visible, 13U),
           "search matches subject text case-insensitively within the Inbox");
    expect(model.selected_message() == std::optional<std::uint32_t>{4U},
           "a matching selection survives the search");
    expect(model.set_search("bulletin"), "search by correspondent");
    visible = model.visible_messages();
    expect(visible.size() == 1U && contains(visible, 15U),
           "search matches the correspondent name (message 2 is in Trash now)");
    expect(!model.selected_message().has_value(),
           "a selection hidden by the search is cleared");
    expect(!model.set_search("bulletin"), "same search text is a no-op");
    expect(model.set_search(""), "clearing search changes state");
    expect(model.visible_messages().size() == 14U, "clearing search restores the folder");

    // Compose operations create Drafts fixtures without any clock or I/O.
    const std::uint32_t draft = model.create_draft();
    expect(draft == 41U, "new drafts take the next dense ID");
    expect(model.selected_folder() == Folder::drafts, "new message switches to Drafts");
    expect(model.selected_message() == std::optional<std::uint32_t>{draft},
           "new message selects the draft");
    expect((*model.find(draft)).subject == "Untitled message" && (*model.find(draft)).outgoing &&
               (*model.find(draft)).read,
           "new draft is an empty outgoing read message");
    expect(model.visible_messages().front() == draft, "newest draft leads the Drafts list");
    expect(model.count(Folder::drafts) == 4U, "draft count grows");

    expect(model.select_folder(Folder::inbox) && model.select_message(3U), "select message 3");
    const std::optional<std::uint32_t> reply = model.reply_to(3U);
    expect(reply.has_value() && *reply == 42U, "reply creates the next draft");
    const Message* reply_message = model.find(*reply);
    expect(reply_message != nullptr && (*reply_message).folder == Folder::drafts &&
               (*reply_message).subject == "Re: Rota for the October public nights" &&
               (*reply_message).correspondent == "Tomasz Wierzba" &&
               (*reply_message).body.find("Tomasz Wierzba wrote:") != std::string::npos &&
               (*reply_message).body.find("> Hi,") != std::string::npos,
           "reply quotes the original and keeps an existing Re: prefix");
    expect(model.selected_folder() == Folder::drafts &&
               model.selected_message() == std::optional<std::uint32_t>{*reply},
           "reply switches to Drafts and selects the reply");

    expect(model.select_folder(Folder::inbox) && model.select_message(4U), "select message 4");
    const std::optional<std::uint32_t> forward = model.forward(4U);
    expect(forward.has_value() && (*model.find(*forward)).subject ==
                                      "Fwd: Photometry results for the eclipsing binary" &&
               (*model.find(*forward)).body.find("Forwarded message") != std::string::npos,
           "forward prefixes the subject and embeds the original");
    expect(!model.reply_to(999U).has_value() && !model.forward(999U).has_value(),
           "compose operations refuse unknown IDs");

    // A fresh model is unaffected by the mutations above.
    const CorrespondenceModel fresh;
    expect(fresh.total_count() == 40U && fresh.unread_count(Folder::inbox) == 6U &&
               fresh.revision() == 0U,
           "each model instance starts from the same fixture");

    std::cout << "correspondence_tests: ok\n";
    return 0;
}
