#ifndef DISPLAY_VIEWS_MAIN_MENU_VIEW_ACTION_HPP_
#define DISPLAY_VIEWS_MAIN_MENU_VIEW_ACTION_HPP_

#include <display/views/IBaseView.hpp>

namespace display::view
{


    class MainMenuView : public IBaseView
    {
        struct MainMenuAction
        {
            View view;
            std::string text;

            MainMenuAction(View view, const std::string &text) : view(view), text(text)
            {
            }
        };

    public:
        MainMenuView() : line_(0)
        {
        }

        void loadView(Display &display) override
        {
            auto first_it = msgs_.begin() + (line_ / display.getMaxRows()) * display.getMaxRows();
            std::for_each(first_it, first_it + display.getMaxRows(), [&](const MainMenuAction &action)
                { display.addTextWithWrapping(action.text, display.getMaxColumns()); });
            display.show();
        }

        void handleKeyInput(config::Key key) override
        {
            switch (key)
            {
                case config::Key::ARROW_UP:
                    if (line_ > 0)
                    {
                        --line_;
                    }
                    break;
                case config::Key::ARROW_DOWN:
                    if (line_ < (msgs_.size() - 1))
                    {
                        ++line_;
                    }
                    break;
                case config::Key::ARROW_LEFT:
                    break;
                case config::Key::ARROW_RIGHT:
                    break;
                case config::Key::ENTER:
                    DisplayViewManager::setView(msgs_[line_].view);
                    return;
                default:
                    break;
            }
        }

    private:
        uint8_t line_;

        const std::vector<MainMenuAction> msgs_ = {{View::TEMP_SENSORS, "Odczyt temperatury"},
            {View::AC_LOW_RELAYS, "Przekazniki AC malej mocy"}, {View::AC_HIGH_RELAYS, "Przekazniki AC duzej mocy"},
            {View::DC_AC_RELAYS, "Przekazniki AC/DC"}, {View::SET_ALARM, "Ustaw alarm"},
            {View::ALARM_NOTIFICATION, "Komunikaty alarmu"}};
    };
}// namespace display::view

#endif// DISPLAY_VIEWS_MAIN_MENU_VIEW_ACTION_HPP_
