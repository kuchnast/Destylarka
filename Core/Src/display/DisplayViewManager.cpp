#include "display/lcd_hd_44780/LcdHd44780I2C.hpp"
#include <display/DisplayViewManager.hpp>

#include <display/views/MainMenuView.hpp>
#include <string>

namespace display
{

    std::string toString(const View &view)
    {
        switch (view)
        {
            case View::EMPTY:
                return "EMPTY";
            case View::MAIN_MENU:
                return "MAIN_MENU";
            case View::TEMP_SENSORS:
                return "TEMP_SENSORS";
            case View::DC_AC_RELAYS:
                return "DC_AC_RELAYS";
            case View::AC_HIGH_RELAYS:
                return "AC_HIGH_RELAYS";
            case View::AC_LOW_RELAYS:
                return "AC_LOW_RELAYS";
            case View::SET_ALARM:
                return "SET_ALARM";
            default:
                return "UNKNOWN";
        }
    }

    DisplayViewManager::DisplayViewManager(const LcdHd44780I2C::Config &lcdDisplayConfig)
        : display_(lcdDisplayConfig), logger_("DisplayViewManager")
    {
    }

    void DisplayViewManager::setView(View view)
    {
        current_view_ = view;
    }

    View DisplayViewManager::getCurrentView()
    {
        return current_view_;
    }

    void DisplayViewManager::handleKeyInput(config::Key key)
    {
        logger_.info() << "View " << toString(current_view_) << " updating with key " << config::toString(key);

        switch (current_view_)
        {
            case View::EMPTY:
            case View::MAIN_MENU:
                mainMenuAction(key);
                break;
            case View::TEMP_SENSORS:
                tempSensorsAction(key);
                break;
            case View::AC_LOW_RELAYS:
                acLowRelaysAction(key);
                break;
            case View::AC_HIGH_RELAYS:
                acHighRelaysAction(key);
                break;
            case View::DC_AC_RELAYS:
                dcAcRelaysAction(key);
                break;
            case View::SET_ALARM:
                setAlarmAction(key);
                break;
            default:
                break;
        }
    }

    void Display::welcomeScreen()
    {
        current_view_ = DisplayView::EMPTY;
        imitationPrinting("....DESTYLARKA....");
    }


    void Display::tempSensorsAction(const config::Key &key)
    {
        static DisplayViewPos pos(0, 0);

        static const std::vector<std::pair<config::Ds18b20NameId, std::string>> msgs = {
            {config::Ds18b20NameId::ZBIORNIK_POD_PIANKA, "Zb.pianka"},
            {config::Ds18b20NameId::ZBIORNIK_W_KAPILARZE, "Zb.kapil."}, {config::Ds18b20NameId::KOLUMNA_DOL, "Kol.dol"},
            {config::Ds18b20NameId::KOLUMNA_GORA, "Kol.gora"},
            {config::Ds18b20NameId::CHLODNICA_ZASILANIE, "Chlod.zas"},
            {config::Ds18b20NameId::CHLODNICA_POWROT, "Chlod.pow"}};

        constexpr std::string_view msgError("BLAD");

        std::vector<std::string> temperature(msgs.size());

        switch (key)
        {
            case config::Key::ARROW_UP:
                if (pos.y == 0)
                    pos.y = 4 - 1;
                //    pos.y = msgs.size() - 1;
                else
                    --pos.y;
                break;
            case config::Key::ARROW_DOWN:
                //if(pos.y == (msgs.size() - 1)) TODO: find why fault when y=4
                if (pos.y == (4 - 1))
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

        for (uint8_t idx, i = 0; i < LINES_NUM; ++i)
        {
            idx = pos.y + i - (pos.y + i > msgs.size() ? msgs.size() : 0);
            auto temp_maybe = ds_collection_.getTemperatureMaybe(msgs[idx].first);

            if (temp_maybe.has_value())
            {
                std::stringstream stream;
                stream << std::fixed << std::setprecision(2) << temp_maybe.value();
                temperature[idx] = std::string(stream.str());
            }
            else
                temperature[idx] = msgError;
        }

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
            [](const std::pair<config::Ds18b20NameId, std::string> &el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y, temperature);
    }

    void Display::acLowRelaysAction(const config::Key &key)
    {
        static DisplayViewPos pos(0, 0);

        static const std::vector<std::pair<config::RelayACLowId, std::string>> msgs = {
            {config::RelayACLowId::ZAWOR_VM_ODBIORU_OTWARCIE, "VM odbior otw"},
            {config::RelayACLowId::ZAWOR_VM_ODBIORU_ZAMKNIECIE, "VM odbior zam"},
            {config::RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_OTWARCIE, "LM + prec otw"},
            {config::RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_ZAMKNIECIE, "LM + prec zam"},
            {config::RelayACLowId::ZAWOR_LM_SAM_OTWARCIE, "LM sam otw"},
            {config::RelayACLowId::ZAWOR_LM_SAM_ZAMKNIECIE, "LM sam zam"}, {config::RelayACLowId::LAMPA, "Lampa"},
            {config::RelayACLowId::WOLNY, "Wolne"}};

        constexpr std::string_view msgOn("ON");
        constexpr std::string_view msgOff("OFF");

        std::vector<std::string> states{};
        states.reserve(msgs.size());

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

        for (const auto &el: msgs)
            states.emplace_back(config::ac_low_relays.Find(el.first).read() == io::PinState::SET ? msgOff : msgOn);

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
            [](const std::pair<config::RelayACLowId, std::string> &el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y, states);
    }

    void Display::acHighRelaysAction(const config::Key &key)
    {
        static DisplayViewPos pos(0, 0);

        static const std::vector<std::pair<config::RelayACHighId, std::string>> msgs = {
            {config::RelayACHighId::GRZALKA_1, "Grzalka 1"}, {config::RelayACHighId::GRZALKA_2, "Grzalka 2"},
            {config::RelayACHighId::POMPA_WODY, "Pompa wody"}};

        constexpr std::string_view msgOn("ON");
        constexpr std::string_view msgOff("OFF");

        std::vector<std::string> states;
        states.reserve(msgs.size());

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

        for (auto &el: msgs)
            states.emplace_back(config::ac_high_relays.Find(el.first).read() == io::PinState::SET ? msgOn : msgOff);

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
            [](const std::pair<config::RelayACHighId, std::string> &el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y, states);
    }

    void Display::dcAcRelaysAction(const config::Key &key)
    {
        static DisplayViewPos pos(0, 0);

        static const std::vector<std::pair<config::RelayDCACId, std::string>> msgs = {
            {config::RelayDCACId::PUSTY_1, "Pusty 1"}, {config::RelayDCACId::PUSTY_2, "Pusty 2"},
            {config::RelayDCACId::PUSTY_3, "Pusty 3"}, {config::RelayDCACId::PUSTY_4, "Pusty 4"}};

        constexpr std::string_view msgOn("ON");
        constexpr std::string_view msgOff("OFF");

        std::vector<std::string> states(msgs.size());

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

        for (auto &el: msgs)
            states.emplace_back(config::dc_ac_relays.Find(el.first).read() == io::PinState::SET ? msgOff : msgOn);

        std::vector<std::string> names;
        names.reserve(msgs.size());
        std::transform(msgs.cbegin(), msgs.cend(), std::back_inserter(names),
            [](const std::pair<config::RelayDCACId, std::string> &el) { return el.second; });

        clearScreen();
        printMenu(names, pos.y, states);
    }

    void Display::setAlarmAction(const config::Key &key)
    {
        static DisplayViewPos pos(0, 0);

        constexpr char line1[] = "Ustaw temp. alarmu:";
        constexpr char msgSet[] = "Ustaw";
        constexpr char msgReset[] = "Reset";
        constexpr char msgRunning[] = "Zalaczone...";

        static std::string tempSet = "00.00";
        static std::optional<std::uint32_t> alarm_task_id, emergency_stop_task_id;

        std::vector<std::string> lines(4);

        switch (key)
        {
            case config::Key::N0:
            case config::Key::N1:
            case config::Key::N2:
            case config::Key::N3:
            case config::Key::N4:
            case config::Key::N5:
            case config::Key::N6:
            case config::Key::N7:
            case config::Key::N8:
            case config::Key::N9:
                if (pos.x < 2)
                    tempSet[pos.x] = config::toString(key)[0];
                else
                    tempSet[pos.x + 1] = config::toString(key)[0];
                break;
            case config::Key::ARROW_UP:
                if (pos.y == 0)
                {
                    pos.y = 1;
                    if (pos.x > 1)
                    {
                        pos.x = 1;
                    }
                }
                else
                    --pos.y;
                break;
            case config::Key::ARROW_DOWN:
                if (pos.y == 1)
                    pos.y = 0;
                else
                {
                    ++pos.y;
                    if (pos.x > 1)
                    {
                        pos.x = 1;
                    }
                }
                break;
            case config::Key::ARROW_RIGHT:
                if (pos.y == 0)
                {
                    if (pos.x == 3)
                        pos.x = 0;
                    else
                        ++pos.x;
                }
                else
                {
                    if (pos.x == 1)
                        pos.x = 0;
                    else
                        ++pos.x;
                }
                break;
            case config::Key::ARROW_LEFT:
                if (pos.y == 0)
                {
                    if (pos.x == 0)
                        pos.x = 3;
                    else
                        --pos.x;
                }
                else
                {
                    if (pos.x == 0)
                        pos.x = 1;
                    else
                        --pos.x;
                }
                break;
            case config::Key::ESC:
                setCurrentView(DisplayView::MAIN_MENU);
                viewAction(config::Key::NONE);
                return;
            case config::Key::ENTER:
                if (pos.y == 1)
                {
                    switch (pos.x)
                    {
                        case 0:
                            if (alarm_task_id.has_value())
                                break;

                            alarm_task_id = io::FunctionTimer::addFunction(
                                [this]()
                                {
                                    auto tempMaybe =
                                        this->ds_collection_.getTemperatureMaybe(config::Ds18b20NameId::KOLUMNA_DOL);
                                    if (tempMaybe)
                                    {
                                        if (tempMaybe.value() > std::atof(tempSet.c_str()))
                                        {
                                            config::alarm_with_notification.enable("TFSetAlarm", "Temp above limit");

                                            if (!emergency_stop_task_id)
                                            {
                                                emergency_stop_task_id = io::FunctionTimer::addFunction(
                                                    []()
                                                    {
                                                        io::Logger("emergencyStopWhenAlarm").warning()
                                                            << "Begin closing valve VM intake and LM precise";
                                                        auto zawor_vm_zamkniecie = config::ac_low_relays.Find(
                                                            config::RelayACLowId::ZAWOR_VM_ODBIORU_ZAMKNIECIE);
                                                        auto zawor_lm_plus_precyzyjny_zamkniecie =
                                                            config::ac_low_relays.Find(config::RelayACLowId::
                                                                    ZAWOR_LM_PLUS_PRECYZYJNY_ZAMKNIECIE);
                                                        zawor_vm_zamkniecie.reset();
                                                        zawor_lm_plus_precyzyjny_zamkniecie.reset();

                                                        emergency_stop_task_id = io::FunctionTimer::addFunction(
                                                            []()
                                                            {
                                                                auto zawor_vm_zamkniecie = config::ac_low_relays.Find(
                                                                    config::RelayACLowId::ZAWOR_VM_ODBIORU_ZAMKNIECIE);
                                                                auto zawor_lm_plus_precyzyjny_zamkniecie =
                                                                    config::ac_low_relays.Find(config::RelayACLowId::
                                                                            ZAWOR_LM_PLUS_PRECYZYJNY_ZAMKNIECIE);
                                                                auto zawor_lm_otwarcie = config::ac_low_relays.Find(
                                                                    config::RelayACLowId::ZAWOR_LM_SAM_OTWARCIE);
                                                                zawor_vm_zamkniecie.set();
                                                                zawor_lm_plus_precyzyjny_zamkniecie.set();
                                                                zawor_lm_otwarcie.reset();
                                                                io::Logger("emergencyStopWhenAlarm").warning()
                                                                    << "End closing valve VM intake and LM precise, "
                                                                       "begin opening LM valve";

                                                                emergency_stop_task_id = io::FunctionTimer::addFunction(
                                                                    []()
                                                                    {
                                                                        auto zawor_lm_otwarcie =
                                                                            config::ac_low_relays.Find(
                                                                                config::RelayACLowId::
                                                                                    ZAWOR_LM_SAM_OTWARCIE);
                                                                        zawor_lm_otwarcie.set();
                                                                        io::Logger("emergencyStopWhenAlarm").warning()
                                                                            << "End opening LM valve";

                                                                        io::FunctionTimer::addFunction(
                                                                            []()
                                                                            {
                                                                                config::ac_high_relays
                                                                                    .Find(config::RelayACHighId::
                                                                                            GRZALKA_1)
                                                                                    .reset();
                                                                                config::ac_high_relays
                                                                                    .Find(config::RelayACHighId::
                                                                                            GRZALKA_2)
                                                                                    .reset();
                                                                            },
                                                                            {1200000},
                                                                            "DisableHeatersWhenDistillationFinished",
                                                                            false);
                                                                    },
                                                                    {60000}, "emergencyStopWhenAlarm", false);
                                                            },
                                                            {60000}, "emergencyStopWhenAlarm", false);
                                                    },
                                                    {180000}, "emergencyStopWhenAlarm", false);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        io::Logger("setAlarmAction").error() << "Can't get temperature.";
                                    }
                                },
                                {5000}, "setAlarmAction", true);
                            break;
                        case 1:
                            if (alarm_task_id.has_value())
                            {
                                io::FunctionTimer::removeFunction(alarm_task_id.value());
                                alarm_task_id.reset();
                            }
                            if (emergency_stop_task_id.has_value())
                            {
                                io::FunctionTimer::removeFunction(emergency_stop_task_id.value());
                                emergency_stop_task_id.reset();
                            }
                            config::alarm_with_notification.disable();
                            break;
                    }
                }
                break;
            default:
                break;
        }

        lines[0] = line1;
        lines[1] = tempSet;

        if (pos.y == 0)
        {
            lines[2] = std::string(" ") + msgSet + std::string("  ") + msgReset;

            if (pos.x < 2)
                lines[1][pos.x] = '_';
            else
                lines[1][pos.x + 1] = '_';
        }
        else if (pos.y == 1)
        {
            if (pos.x == 0)
                lines[2] = std::string(">") + msgSet + std::string("  ") + msgReset;
            else
                lines[2] = std::string(" ") + msgSet + std::string(" >") + msgReset;
        }

        if (alarm_task_id.has_value())
            lines[3] = msgRunning;

        clearScreen();
        printMenu(lines);
    }

    void Display::alarmNotificationAction(const config::Key &key)
    {
        constexpr char line1[] = "Ostatni alarm:";
        constexpr char line4[] = "Reset";

        std::vector<std::string> lines(4);

        switch (key)
        {
            case config::Key::ESC:
                setCurrentView(DisplayView::MAIN_MENU);
                viewAction(config::Key::NONE);
                return;
            case config::Key::ENTER:
                config::alarm_with_notification.disable();
                break;
            default:
                break;
        }

        lines[0] = line1;
        lines[3] = line4;

        if (config::alarm_with_notification.is_enabled())
        {
            // #TODO: Add printing of longer messages
            lines[1] = config::alarm_with_notification.getSource().substr(0, LINE_SIZE);
            lines[2] = config::alarm_with_notification.getReason().substr(0, LINE_SIZE);
        }

        clearScreen();
        printMenu(lines);
    }

}// namespace display
