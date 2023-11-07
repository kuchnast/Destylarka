#include <config/RelaysAcHigh.hpp>
#include <config/RelaysAcLow.hpp>
#include <config/RelaysDcAc.hpp>
#include <display/Display.hpp>
#include <display/LcdHd44780.hpp>
#include <sensors/Ds18b20.hpp>

#include <algorithm>

namespace display
{

    void Display::init(I2C_HandleTypeDef *hi2c, uint8_t address, uint8_t lines, uint8_t rows)
    {
        lcdInit(hi2c, address, lines, rows);
        lcdBacklightOn();
        welcomeScreen();
    }

    void Display::clearScreen()
    {
        lcdDisplayClear();
    }

    void Display::viewAction(const config::Key& key)
    {
        switch (current_view_)
        {
            case DisplayView::EMPTY:
            case DisplayView::MAIN_MENU:
                mainMenuAction(key);
                break;
            case DisplayView::TEMP_SENSORS:
                tempSensorsAction(key);
                break;
            case DisplayView::AC_LOW_RELAYS:
                acLowRelaysAction(key);
                break;
            case DisplayView::AC_HIGH_RELAYS:
                acHighRelaysAction(key);
                break;
            case DisplayView::DC_AC_RELAYS:
                dcAcRelaysAction(key);
                break;
            case DisplayView::SET_ALARM:
                setAlarmAction(key);
                break;
            default:
                break;
        }
    }

    const DisplayView &Display::getCurrentView()
    {
        return current_view_;
    }

    void Display::setCurrentView(const DisplayView& display_view)
    {
        current_view_ = display_view;
    }

    void Display::imitationPrinting(const std::string& str)
    {
        for(const auto& c : str)
        {
            lcdPrintChar(c);
            HAL_Delay(100);
        }
    }

    std::string Display::fitStringToLine(const std::string & str1, char prefix)
    {
        if(str1.size() <= LINE_SIZE - 1)
            return prefix + str1;
        else
            return prefix + str1.substr(0, LINE_SIZE - 1);
    }

    std::string Display::fitStringsToLine(const std::string & str1, const std::string & str2, char prefix)
    {
        if(str1.size() + str2.size() <= LINE_SIZE - 1)
            return prefix + str1 + std::string(LINE_SIZE - 1 - str1.size() - str2.size(), ' ')  + str2;
        else
        {
            auto tmp = str1.substr(0, LINE_SIZE - 2 - str2.size());
            return prefix + tmp + std::string(LINE_SIZE - 1 - tmp.size() - str2.size(), ' ')  + str2;
        }
    }

    void Display::printMenu(const std::vector<std::string> &msgs, uint8_t possition, const std::vector<std::string> & add_msgs)
    {
        if(possition >= msgs.size() || msgs.size() != add_msgs.size())
            Error_Handler();

        for (uint8_t i = 0; i < (msgs.size() > LINES_NUM ? LINES_NUM : msgs.size()); ++i)
        {
            lcdSetCursorPosition(0, i);
            if(possition + i < msgs.size())
                lcdPrintStr(fitStringsToLine(msgs[possition + i], add_msgs[possition + i], (i ? ' ' : '>')));
            else
                lcdPrintStr(fitStringsToLine(msgs[possition + i - msgs.size()], add_msgs[possition + i - msgs.size()], (i ? ' ' : '>')));
        }
    }

    void Display::printMenu(const std::vector<std::string> &msgs, uint8_t possition)
    {
        if(possition >= msgs.size())
            Error_Handler();

        for (uint8_t i = 0; i < (msgs.size() > LINES_NUM ? LINES_NUM : msgs.size()); ++i)
        {
            lcdSetCursorPosition(0, i);
            if(possition + i < msgs.size())
                lcdPrintStr(fitStringToLine(msgs[possition + i], (i ? ' ' : '>')));
            else
                lcdPrintStr(fitStringToLine(msgs[possition + i - msgs.size()], (i ? ' ' : '>')));
        }
    }

    void Display::welcomeScreen()
    {
        current_view_ = DisplayView::EMPTY;
        imitationPrinting("....DESTYLARKA....");
    }

    void Display::mainMenuAction(const config::Key& key)
    {
        static DisplayViewPos pos(0, 0);
        static const std::vector<std::pair<DisplayView, std::string>> msgs =
                {{DisplayView::TEMP_SENSORS, "Odczyt temperatury"},
                 {DisplayView::AC_LOW_RELAYS, "Przek. AC malej W"},
                 {DisplayView::AC_HIGH_RELAYS, "Przek. AC duzej W"},
                 {DisplayView::DC_AC_RELAYS, "Przek. AC DC"},
                 {DisplayView::SET_ALARM, "Ustaw alarm"}};

        switch (key)
        {
            case config::Key::ARROW_UP:
                if(pos.y == 0)
                    pos.y = msgs.size() - 1;
                else
                    --pos.y;
                break;
            case config::Key::ARROW_DOWN:
                if(pos.y == (msgs.size() - 1))
                    pos.y = 0;
                else
                    ++pos.y;
                break;
            case config::Key::ENTER:
                setCurrentView(msgs[pos.y].first);
                viewAction(config::Key::NONE);
                return;
            default:
                break;
        }

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
                       [](const std::pair<DisplayView, std::string>& el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y);
    }

    void Display::tempSensorsAction(const config::Key& key)
    {
        static DisplayViewPos pos(0, 0);

        static const std::vector<std::pair<config::Ds18b20NameId, std::string>> msgs =
                {{config::Ds18b20NameId::ZBIORNIK_POD_PIANKA, "Zb.pianka"},
                 {config::Ds18b20NameId::ZBIORNIK_W_KAPILARZE, "Zb.kapil."},
                 {config::Ds18b20NameId::KOLUMNA_GORA, "Kol.dol"},
                 {config::Ds18b20NameId::KOLUMNA_DOL, "Kol.gora"},
                 {config::Ds18b20NameId::CHLODNICA_ZASILANIE, "Chlod.zas"},
                 {config::Ds18b20NameId::CHLODNICA_POWROT, "Chlod.pow"}};

        constexpr std::string_view msgError("BLAD");
        constexpr std::string_view msgNotFound("BRAK");

        std::vector<std::string> temperature(msgs.size());

        switch (key)
        {
            case config::Key::ARROW_UP:
                if(pos.y == 0)
                    pos.y = msgs.size() - 1;
                else
                    --pos.y;
                break;
            case config::Key::ARROW_DOWN:
                if(pos.y == (msgs.size() - 1))
                    pos.y = 0;
                else
                    ++pos.y;
                break;
            case config::Key::ESC:
                setCurrentView(DisplayView::MAIN_MENU);
                viewAction(config::Key::NONE);
                return;
            default:
                break;
        }

        for(uint8_t idx, i = 0; i < LINES_NUM; ++i)
        {
            idx = pos.y + i - (pos.y + i > msgs.size() ? msgs.size(): 0);
            auto temp_maybe = ds_collection_.getTemperatureMaybe(msgs[idx].first);

            if(temp_maybe.has_value())
                temperature[idx] = std::to_string(temp_maybe.value()).substr(0, 6);
            else
                temperature[idx] = msgError;

        }

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
                       [](const std::pair<config::Ds18b20NameId, std::string>& el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y, temperature);
    }

    void Display::acLowRelaysAction(const config::Key& key)
    {
        static DisplayViewPos pos(0, 0);

        static const std::vector<std::pair<config::RelayACLowId, std::string>> msgs =
                {{config::RelayACLowId::ZAWOR_VM_ODBIORU_OTWARCIE, "VM odbior otw"},
                 {config::RelayACLowId::ZAWOR_VM_ODBIORU_ZAMKNIECIE, "VM odbior zam"},
                 {config::RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_OTWARCIE, "LM + prec otw"},
                 {config::RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_ZAMKNIECIE, "LM + prec zam"},
                 {config::RelayACLowId::ZAWOR_LM_SAM_OTWARCIE, "LM sam otw"},
                 {config::RelayACLowId::ZAWOR_LM_SAM_ZAMKNIECIE, "LM sam zam"},
                 {config::RelayACLowId::LAMPA, "Lampa"},
                 {config::RelayACLowId::WOLNY, "Wolne"}};

        constexpr std::string_view msgOn("ON");
        constexpr std::string_view msgOff("OFF");

        std::vector<std::string> states{};
        states.reserve(msgs.size());

        switch (key)
        {
            case config::Key::ARROW_UP:
                if(pos.y == 0)
                    pos.y = msgs.size() - 1;
                else
                    --pos.y;
                break;
            case config::Key::ARROW_DOWN:
                if(pos.y == (msgs.size() - 1))
                    pos.y = 0;
                else
                    ++pos.y;
                break;
            case config::Key::ESC:
                setCurrentView(DisplayView::MAIN_MENU);
                viewAction(config::Key::NONE);
                return;
            case config::Key::ENTER:
                config::ac_low_relays.Find(msgs[pos.y].first).toggle();
                break;
            default:
                break;
        }

        for(const auto& el : msgs)
            states.emplace_back(config::ac_low_relays.Find(el.first).read() == io::PinState::SET ? msgOff : msgOn);

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
                       [](const std::pair<config::RelayACLowId, std::string>& el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y, states);
    }

    void Display::acHighRelaysAction(const config::Key& key)
    {
        static DisplayViewPos pos(0, 0);

        static const std::vector<std::pair<config::RelayACHighId, std::string>> msgs =
                {{config::RelayACHighId::GRZALKA_1, "Grzalka 1"},
                 {config::RelayACHighId::GRZALKA_2, "Grzalka 2"},
                 {config::RelayACHighId::POMPA_WODY, "Pompa wody"}};

        constexpr std::string_view msgOn("ON");
        constexpr std::string_view msgOff("OFF");

        std::vector<std::string> states(msgs.size());

        switch (key)
        {
            case config::Key::ARROW_UP:
                if(pos.y == 0)
                    pos.y = msgs.size() - 1;
                else
                    --pos.y;
                break;
            case config::Key::ARROW_DOWN:
                if(pos.y == (msgs.size() - 1))
                    pos.y = 0;
                else
                    ++pos.y;
                break;
            case config::Key::ESC:
                setCurrentView(DisplayView::MAIN_MENU);
                viewAction(config::Key::NONE);
                return;
            case config::Key::ENTER:
                config::ac_high_relays.Find(msgs[pos.y].first).toggle();
                break;
            default:
                break;
        }

        for(auto & el : msgs)
            states.emplace_back(config::ac_high_relays.Find(el.first).read() == io::PinState::SET ? msgOff : msgOn);

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
                       [](const std::pair<config::RelayACHighId, std::string>& el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y, states);
    }

    void Display::dcAcRelaysAction(const config::Key& key)
    {
        static DisplayViewPos pos(0, 0);

        static const std::vector<std::pair<config::RelayDCACId, std::string>> msgs =
                {{config::RelayDCACId::PUSTY_1, "Pusty 1"},
                 {config::RelayDCACId::PUSTY_2, "Pusty 2"},
                 {config::RelayDCACId::PUSTY_3, "Pusty 3"},
                 {config::RelayDCACId::PUSTY_4, "Pusty 4"}};

        constexpr std::string_view msgOn("ON");
        constexpr std::string_view msgOff("OFF");

        std::vector<std::string> states(msgs.size());

        switch (key)
        {
            case config::Key::ARROW_UP:
                if(pos.y == 0)
                    pos.y = msgs.size() - 1;
                else
                    --pos.y;
                break;
            case config::Key::ARROW_DOWN:
                if(pos.y == (msgs.size() - 1))
                    pos.y = 0;
                else
                    ++pos.y;
                break;
            case config::Key::ESC:
                setCurrentView(DisplayView::MAIN_MENU);
                viewAction(config::Key::NONE);
                return;
            case config::Key::ENTER:
                config::dc_ac_relays.Find(msgs[pos.y].first).toggle();
                break;
            default:
                break;
        }

        for(auto & el : msgs)
            states.emplace_back(config::dc_ac_relays.Find(el.first).read() == io::PinState::SET ? msgOff : msgOn);

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
                       [](const std::pair<config::RelayDCACId, std::string>& el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y, states);
    }

    void Display::setAlarmAction(const config::Key& key)
    {

        static DisplayViewPos pos(0, 0);

        static const std::vector<std::pair<config::RelayDCACId, std::string>> msgs =
                {{config::RelayDCACId::PUSTY_1, "Pusty 1"},
                 {config::RelayDCACId::PUSTY_2, "Pusty 2"},
                 {config::RelayDCACId::PUSTY_3, "Pusty 3"},
                 {config::RelayDCACId::PUSTY_4, "Pusty 4"}};

        constexpr std::string_view msgOn("ON");
        constexpr std::string_view msgOff("OFF");

        std::vector<std::string> states(msgs.size());

        switch (key)
        {
            case config::Key::ARROW_UP:
                if(pos.y == 0)
                    pos.y = msgs.size() - 1;
                else
                    --pos.y;
                break;
            case config::Key::ARROW_DOWN:
                if(pos.y == (msgs.size() - 1))
                    pos.y = 0;
                else
                    ++pos.y;
                break;
            case config::Key::ESC:
                setCurrentView(DisplayView::MAIN_MENU);
                viewAction(config::Key::NONE);
                return;
            case config::Key::ENTER:
                config::dc_ac_relays.Find(msgs[pos.y].first).toggle();
                break;
            default:
                break;
        }

        for(auto & el : msgs)
            states.emplace_back(config::dc_ac_relays.Find(el.first).read() == io::PinState::SET ? msgOn : msgOff);

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
                       [](const std::pair<config::RelayDCACId, std::string>& el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y, states);
    }

}
