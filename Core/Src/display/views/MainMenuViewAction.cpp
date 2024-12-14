#include <display/DisplayViewManager.hpp>
#include <display/views/MainMenuView.hpp>

namespace display::views
{
    void mainMenuAction(const config::Key &key)
    {
        static DisplayViewPos pos(0, 0);
        static const std::vector<std::pair<View, std::string>> msgs = {{View::TEMP_SENSORS, "Odczyt temperatury"},
            {View::AC_LOW_RELAYS, "Przek. AC malej W"}, {View::AC_HIGH_RELAYS, "Przek. AC duzej W"},
            {View::DC_AC_RELAYS, "Przek. AC DC"}, {View::SET_ALARM, "Ustaw alarm"},
            {View::ALARM_NOTIFICATION, "Komunikaty alarmu"}};

        switch (key)
        {
            case config::Key::ARROW_UP:
                if (pos.y == 0)
                    pos.y = msgs.size() - 1;
                else
                    --pos.y;
                break;
            case config::Key::ARROW_DOWN:
                if (pos.y == (msgs.size() - 1))
                    pos.y = 0;
                else
                    ++pos.y;
                break;
            case config::Key::ENTER:
                DisplayViewManager::setView(msgs[pos.y].first);
                return;
            default:
                break;
        }

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
            [](const std::pair<View, std::string> &el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y);
    }
}// namespace display::views