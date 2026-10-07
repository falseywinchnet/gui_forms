#include <gui_forms/basic_controls.hpp>
#include <gui_forms/event.hpp>

namespace gf = gui_forms;

class Settings final : public gf::Component {
public:
    void fixed() { ++calls; }
    void toggle(const int index) { selected = index; }
    void choose(const int row, const int choice, gf::ButtonBase&) {
        selected = row * 10 + choice;
    }
    int calls{};
    int selected{};
};

int main() {
    Settings settings{};
    gf::Button back(gf::StableId("back"), "Back");
    gf::Button option(gf::StableId("option"), "Option");
    gf::Button tile(gf::StableId("tile"), "Tile");
    gf::on(back.clicked(), settings, &Settings::fixed);
    gf::on(option.clicked(), settings, &Settings::toggle, 3);
    gf::on(tile.clicked(), settings, &Settings::choose, 2, 4);
    back.perform_click();
    option.perform_click();
    if (settings.calls != 1 || settings.selected != 3) return 1;
    tile.perform_click();
    if (settings.selected != 24) return 1;
    return 0;
}
