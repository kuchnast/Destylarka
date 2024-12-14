#ifndef DISPLAY_VIEWS_BASE_VIEW_ACTION_HPP_
#define DISPLAY_VIEWS_BASE_VIEW_ACTION_HPP_

#include <config/Keypad.hpp>
#include <display/Display.hpp>

namespace display::view
{

    class IBaseView
    {
    public:
        virtual ~IBaseView() = default;

        virtual void loadView(Display &display) = 0;

        virtual void handleKeyInput(config::Key key) = 0;

    private:
    };

}// namespace display::view

#endif// DISPLAY_VIEWS_BASE_VIEW_ACTION_HPP_
