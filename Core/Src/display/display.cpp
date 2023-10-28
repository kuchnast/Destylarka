/**
 * @file display.c
 * @author Kamil Uchnast
 * @brief Plik zawiera definicje funkcji zadeklarowanych w pliku display.h
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <config/relays_ac_high.hpp>
#include <config/relays_ac_low.hpp>
#include <config/relays_dc_ac.hpp>
#include <config/sensors_ds.hpp>
#include <display/display.hpp>
#include <display/hd44780.hpp>
#include <display/stm32_device.hpp>
#include <sensors/ds18b20.hpp>
#include <string>
#include <stdio.h>

namespace display
{

void Display::Init()
{
	lcdInit(&lcdConfig);
	lcdBackLightOn();
	WelcomeScreen();
}

void Display::Clear()
{
  	lcdClrScr();
}

void Display::ViewAction(Key key)
{
    switch (Display::display_curr_view)
    {
    case DisplayView::EMPTY:
    case DisplayView::MAIN_MENU:
        MainMenuAction(key);
        break;
    case DisplayView::TEMP_SENSORS:
        TempSensorsAction(key);
        break;    
    case DisplayView::AC_LOW_RELAYS:
        AcLowRelaysAction(key);
        break;
    case DisplayView::AC_HIGH_RELAYS:
        AcHighRelaysAction(key);
        break;
    case DisplayView::DC_AC_RELAYS:
        DcAcRelaysAction(key);
        break;
    case DisplayView::SET_ALARM:
        SetAlarmAction(key);
        break;
    default:
        break;
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
        lcdGoto(i + 1, 0);
        if(possition + i < msgs.size())
            lcdPuts(fitStringsToLine(msgs[possition + i], add_msgs[possition + i], (i ? ' ' : '>')).c_str());
        else
            lcdPuts(fitStringsToLine(msgs[possition + i - msgs.size()], add_msgs[possition + i - msgs.size()], (i ? ' ' : '>')).c_str());
    }
}

void Display::printMenu(const std::vector<std::string> &msgs, uint8_t possition)
{
    if(possition >= msgs.size())
        Error_Handler();

    for (uint8_t i = 0; i < (msgs.size() > LINES_NUM ? LINES_NUM : msgs.size()); ++i)
    {
        lcdGoto(i + 1, 0);
        if(possition + i < msgs.size())
            lcdPuts(fitStringToLine(msgs[possition + i], (i ? ' ' : '>')).c_str());
        else
            lcdPuts(fitStringToLine(msgs[possition + i - msgs.size()], (i ? ' ' : '>')).c_str());
    }
}

void Display::WelcomeScreen()
{
    Display::display_curr_view = DisplayView::EMPTY;
    char msg[] = "    DESTYLARKA    ";
    imitationPrinting(msg);
}

void Display::MainMenuAction(Key key)
{
    static DisplayViewPos pos(0, 0);
    static const std::vector<std::string> msgs =
        {"Odczyt temperatury", 
        "Przek. AC malej W", 
        "Przek. AC duzej W", 
        "Przek. AC DC",
        "Ustaw alarm"};

    switch (key)
    {
    case K_ARROW_UP:
        if(pos.y == 0)
            pos.y = msgs.size() - 1;
        else
            --pos.y;
        break;
    case K_ARROW_DOWN:
        if(pos.y == (msgs.size() - 1))
            pos.y = 0;
        else
            ++pos.y;
        break;
    case K_ENTER:
        switch (pos.y)
        {
        case 0:
            display_curr_view = DisplayView::TEMP_SENSORS;
            break;
        case 1:
            display_curr_view = DisplayView::AC_LOW_RELAYS;
            break;
        case 2:
            display_curr_view = DisplayView::AC_HIGH_RELAYS;
            break;
        case 3:
            display_curr_view = DisplayView::DC_AC_RELAYS;
            break;
        default:
            break;
        }
        ViewAction(K_NONE);
        return;
    default:
        break;
    }

    lcdClrScr();
    printMenu(msgs, pos.y);
}

void Display::TempSensorsAction(Key key)
{
    static DisplayViewPos pos(0, 0);

    static const std::vector<std::pair<DsNameId, std::string>> msgs =
        {{DsNameId::ZBIORNIK_POD_PIANKA, "Zb.pianka"}, 
        {DsNameId::ZBIORNIK_W_KAPILARZE, "Zb.kapil."},
        {DsNameId::KOLUMNA_GORA, "Kol.dol"}, 
        {DsNameId::KOLUMNA_DOL, "Kol.gora"},
        {DsNameId::CHLODNICA_ZASILANIE, "Chlod.zas"},
        {DsNameId::CHLODNICA_POWROT, "Chlod.pow"}};

    const std::string msgError = "BLAD";
    const std::string msgNotFound = "BRAK";

    std::vector<std::string> temperature(msgs.size());
    float measurement;
    
    switch (key)
    {
     if(pos.y == 0)
            pos.y = msgs.size() - 1;
        else
            --pos.y;
        break;
    case K_ARROW_DOWN:
        if(pos.y == (msgs.size() - 1))
            pos.y = 0;
        else
            ++pos.y;
        break;
    case K_ESC:
        display_curr_view = DisplayView::MAIN_MENU;
        ViewAction(K_NONE);
        return;
    default:
        break;
    }


    for(uint8_t i = 0; i < LINES_NUM; ++i)
    {
        uint8_t error;
        uint8_t idx = pos.y + i - (pos.y + i > msgs.size() ? msgs.size(): 0);

        error = DS18B20_GetTemperatureById(msgs[idx].first, &measurement);

        if(!error)
            temperature[idx] = std::to_string(measurement).substr(0, 6);
        else if(error == 2)
            temperature[idx] = msgNotFound;
        else
            temperature[idx] = msgError;

    }

    std::vector<std::string> names;
    names.reserve(msgs.size());
    std::transform(msgs.begin(), msgs.end(), std::back_inserter(names), [](const std::pair<DsNameId, std::string>& el) { return el.first; });

    lcdClrScr();
    printMenu(names, pos.y, temperature);
}


void Display::AcLowRelaysAction(Key key)
{
    static DisplayViewPos pos(0, 0);

    static const std::vector<std::pair<DsNameId, std::string>> msgs =
        {{DsNameId::ZBIORNIK_POD_PIANKA, "Zb.pianka"}, 
        {DsNameId::ZBIORNIK_W_KAPILARZE, "Zb.kapil."},
        {DsNameId::KOLUMNA_GORA, "Kol.dol"}, 
        {DsNameId::KOLUMNA_DOL, "Kol.gora"},
        {DsNameId::CHLODNICA_ZASILANIE, "Chlod.zas"},
        {DsNameId::CHLODNICA_POWROT, "Chlod.pow"}};

    const std::string msgError = "BLAD";
    const std::string msgNotFound = "BRAK";

    std::vector<std::string> temperature(msgs.size());
    float measurement;
    
    switch (key)
    {
     if(pos.y == 0)
            pos.y = msgs.size() - 1;
        else
            --pos.y;
        break;
    case K_ARROW_DOWN:
        if(pos.y == (msgs.size() - 1))
            pos.y = 0;
        else
            ++pos.y;
        break;
    case K_ESC:
        display_curr_view = DisplayView::MAIN_MENU;
        ViewAction(K_NONE);
        return;
    default:
        break;
    }


	RelayACLowDisplay msg[RELAY_AC_LOW_NUM] = {
		{"VM odbior otw", &known_ac_low_relays[7]},
		{"VM odbior zam", &known_ac_low_relays[6]},
		{"LM + prec otw", &known_ac_low_relays[5]},
		{"LM + prec zam", &known_ac_low_relays[4]},
		{"LM sam otw   ", &known_ac_low_relays[3]},
		{"LM sam zam   ", &known_ac_low_relays[2]},
		{"Lampa        ", &known_ac_low_relays[1]},
		{"Wolne        ", &known_ac_low_relays[0]},
	};

    char msgOn[] = "ON";
    char msgOff[] = "OFF";

    char display_line[21];

    switch (key)
    {
    case K_ARROW_UP:
        if(pos.y == 0)
            pos.y = RELAY_AC_LOW_NUM - 1;
        else
            --pos.y;
        break;
    case K_ARROW_DOWN:
        if(pos.y == RELAY_AC_LOW_NUM - 1)
            pos.y = 0;
        else
            ++pos.y;
        break;
    case K_ESC:
        display_curr_view = VIEW_MAIN_MENU;
        ViewAction(K_NONE);
        return;
    case K_ENTER:
    	HAL_GPIO_TogglePin(msg[pos.y].dev->output_pin.gpio, msg[pos.y].dev->output_pin.pin);
        break;
    default:
        break;
    }

    lcdClrScr();

    GPIO_PinState state;
    for(uint8_t line = 1, i = (pos.y > 3 ? pos.y - 3 : 0);
		i < (pos.y > 3 ? pos.y + 1 : 4); ++i, ++line)
    {
    	state = HAL_GPIO_ReadPin(msg[i].dev->output_pin.gpio, msg[i].dev->output_pin.pin);

    	if(pos.y == i)
    		snprintf(display_line, 21, ">%s  %s", msg[i].text, (state ? msgOff : msgOn));
    	else
    		snprintf(display_line, 21, " %s  %s", msg[i].text, (state ? msgOff : msgOn));

    	lcdGoto(line, 0);
		lcdPuts(display_line);
    }
}

void Display::AcHighRelaysAction(Key key)
{
    const std::string msgOn("ON");
    const std::string msgOff("OFF");

    static DisplayViewPos pos(0, 0);
    static const size_t relaysSize = config::ac_high_relays.Size();

	std::string display_line;

    switch (key)
    {
    case K_ARROW_UP:
        if(pos.y == 0)
            pos.y = relaysSize - 1;
        else
            --pos.y;
        break;
    case K_ARROW_DOWN:
        if(pos.y == relaysSize - 1)
            pos.y = 0;
        else
            ++pos.y;
        break;
    case K_ESC:
        display_curr_view = DisplayView::MAIN_MENU;
        ViewAction(K_NONE);
        return;
    case K_ENTER:
        config::ac_high_relays
        HAL_GPIO_TogglePin(msg[pos.y].dev->gpio, msg[pos.y].dev->pin);
        break;
    default:
    	break;
    }

    lcdClrScr();

    GPIO_PinState state;
    for(uint8_t i = 0; i < 3; ++i)
    {
    	state = HAL_GPIO_ReadPin(msg[i].dev->gpio, msg[i].dev->pin);

    	if(view_ac_high_relays_params.y == i)
    		snprintf(display_line, 21, ">%s  %s", msg[i].text, (state ? msgOn : msgOff));
    	else
    		snprintf(display_line, 21, " %s  %s", msg[i].text, (state ? msgOn : msgOff));

    	lcdGoto(i + 1, 0);
		lcdPuts(display_line);
    }
}

void Display::DcAcRelaysAction(Key key)
{
    char msg0[] = "Pusty 1";
    char msg1[] = "Pusty 2";
    char msg2[] = "Pusty 3";
    char msg3[] = "Pusty 4";

    char msgOn[] = "ON";
    char msgOff[] = "OFF";

    switch (key)
    {
    case K_ARROW_UP:
        if(view_dc_ac_relays_params.y == 0)
            view_dc_ac_relays_params.y = 3;
        else
            --view_dc_ac_relays_params.y;
        break;
    case K_ARROW_DOWN:
        if(view_dc_ac_relays_params.y == 3)
            view_dc_ac_relays_params.y = 0;
        else
            ++view_dc_ac_relays_params.y;
        break;
    case K_ESC:
        display_curr_view = VIEW_MAIN_MENU;
        ViewAction(K_NONE);
        return;
    case K_ENTER:
        HAL_GPIO_TogglePin(known_dc_ac_relays[view_dc_ac_relays_params.y].gpio, known_dc_ac_relays[view_dc_ac_relays_params.y].pin);
        break;
    default:
    	break;
    }

    lcdClrScr();

    lcdGoto(view_dc_ac_relays_params.y, 0);
    lcdPutc('>');

    lcdGoto(LCD_1st_LINE, 1);
    lcdPuts(msg0);
    lcdGoto(LCD_2nd_LINE, 1);
    lcdPuts(msg1);
    lcdGoto(LCD_3rd_LINE, 1);
    lcdPuts(msg2);
    lcdGoto(LCD_4th_LINE, 1);
    lcdPuts(msg3);

    for(int i = 0; i < 4; ++i)
    {
        lcdGoto((unsigned int)i + 1, 16);

        if(HAL_GPIO_ReadPin(
            known_dc_ac_relays[i].gpio, 
            known_dc_ac_relays[i].pin))
        {
            lcdPuts(msgOn);
        }
        else
        {
            lcdPuts(msgOff);
        }
    }
}

void Display::imitationPrinting(char *ptr)
{
	lcdSetMode(VIEW_MODE_DispOn_BlkOff_CrsOn);

	while(*ptr != 0)
	{
		lcdPutc(*ptr++);
		HAL_Delay(100);
	}

	lcdSetMode(VIEW_MODE_DispOn_BlkOff_CrsOff);
}

}
