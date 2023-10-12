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

DisplayView display_curr_view;

void Init()
{
	lcdInit(&lcdConfig);
	lcdBackLightOn();
	WelcomeScreen();
}

void Clear()
{
  	lcdClrScr();
}

void WelcomeScreen()
{
    display_curr_view = DisplayView::EMPTY;
    char msg[] = "    DESTYLARKA    ";
    imitationPrinting(msg);
}

void ViewAction(Key key)
{
    switch (display_curr_view)
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

void MainMenuAction(Key key)
{
    char msg0[] = "Odczyt temperatury";
    char msg1[] = "Przek. AC malej W";
    char msg2[] = "Przek. AC duzej W";
    char msg3[] = "Przek. AC DC";

    switch (key)
    {
    case K_ARROW_UP:
        if(menu_pos.y == 0)
            menu_pos.y = 3;
        else
            --menu_pos.y;
        break;
    case K_ARROW_DOWN:
        if(menu_pos.y == 3)
            menu_pos.y = 0;
        else
            ++menu_pos.y;
        break;
    case K_ENTER:
        switch (menu_pos.y)
        {
        case 0:
            display_curr_view = VIEW_TEMP_SENSORS;
            break;
        case 1:
            display_curr_view = VIEW_AC_LOW_RELAYS;
            break;
        case 2:
            display_curr_view = VIEW_AC_HIGH_RELAYS;
            break;
        case 3:
            display_curr_view = VIEW_DC_AC_RELAYS;
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

  	lcdGoto(LCD_1st_LINE,1);
  	lcdPuts(msg0);
  	lcdGoto(LCD_2nd_LINE,1);
    lcdPuts(msg1);
  	lcdGoto(LCD_3rd_LINE,1);
    lcdPuts(msg2);
  	lcdGoto(LCD_4th_LINE,1);
    lcdPuts(msg3);

    switch (menu_pos.y)
    {
    case 0:
        lcdGoto(LCD_1st_LINE,0);
        break;
    case 1:
        lcdGoto(LCD_2nd_LINE,0);
        break;
    case 2:
        lcdGoto(LCD_3rd_LINE,0);
        break;
    case 3:
        lcdGoto(LCD_4th_LINE,0);
        break;
    default:
        break;
    }
    lcdPuts(">");
}

void TempSensorsAction(Key key)
{
    char msg0[] = "Zb.pianka";
    char msg1[] = "Zb.kapil.";
    char msg2[] = "Kol.dol  ";
    char msg3[] = "Kol.gora ";
    char msg4[] = "Chlod.zas";
    char msg5[] = "Chlod.pow";

    char msgError[] = "BLAD";
    char msgNotFound[] = "BRAK";

    std::string msg("asd");

    float temperature[4];
    uint8_t error[4];
    char * msgPtr[4];

    switch (key)
    {
    case K_ARROW_UP:
        if(menu_pos.y == 0)
            menu_pos.y = 2;
        else
            --menu_pos.y;
        break;
    case K_ARROW_DOWN:
        if(menu_pos.y == 2)
            menu_pos.y = 0;
        else
            ++menu_pos.y;
        break;
    case K_ESC:
        display_curr_view = VIEW_MAIN_MENU;
        ViewAction(K_NONE);
        return;
    default:
        break;
    }

    lcdClrScr();

    for(uint8_t i = menu_pos.y; i < DS_SENSORS_NUM - (2 - menu_pos.y); ++i)
    {
        error[i - menu_pos.y] = DS18B20_GetTemperatureById((DsNamesId)(i + 1),
            &(temperature[i - menu_pos.y]));
    }

    switch (menu_pos.y)
    {
    case 0:
    default:
        msgPtr[0] = (char *)&msg0;
        msgPtr[1] = (char *)&msg1;
        msgPtr[2] = (char *)&msg2;
        msgPtr[3] = (char *)&msg3;
        break;
    case 1:
        msgPtr[0] = (char *)&msg1;
        msgPtr[1] = (char *)&msg2;
        msgPtr[2] = (char *)&msg3;
        msgPtr[3] = (char *)&msg4;
        break;
    case 2:
        msgPtr[0] = (char *)&msg2;
        msgPtr[1] = (char *)&msg3;
        msgPtr[2] = (char *)&msg4;
        msgPtr[3] = (char *)&msg5;
        break;
    }
  	
    for(int i = 0; i < 4; ++i)
    {
        lcdGoto((unsigned int)i + 1,0);
        lcdPuts(msgPtr[i]);
        lcdGoto((unsigned int)i + 1,11);

        if(error[i])
        {
            if(error[i] == 1)
                lcdPuts(msgError);
            else if(error[i] == 2)
                lcdPuts(msgNotFound);
        }
        else
        {
            lcdFtos(temperature[i], 3);
        }

    }

}


void AcLowRelaysAction(Key key)
{
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
        if(menu_pos.y == 0)
            menu_pos.y = RELAY_AC_LOW_NUM - 1;
        else
            --menu_pos.y;
        break;
    case K_ARROW_DOWN:
        if(menu_pos.y == RELAY_AC_LOW_NUM - 1)
            menu_pos.y = 0;
        else
            ++menu_pos.y;
        break;
    case K_ESC:
        display_curr_view = VIEW_MAIN_MENU;
        ViewAction(K_NONE);
        return;
    case K_ENTER:
    	HAL_GPIO_TogglePin(msg[menu_pos.y].dev->output_pin.gpio, msg[menu_pos.y].dev->output_pin.pin);
        break;
    default:
        break;
    }

    lcdClrScr();

    GPIO_PinState state;
    for(uint8_t line = 1, i = (menu_pos.y > 3 ? menu_pos.y - 3 : 0);
		i < (menu_pos.y > 3 ? menu_pos.y + 1 : 4); ++i, ++line)
    {
    	state = HAL_GPIO_ReadPin(msg[i].dev->output_pin.gpio, msg[i].dev->output_pin.pin);

    	if(menu_pos.y == i)
    		snprintf(display_line, 21, ">%s  %s", msg[i].text, (state ? msgOff : msgOn));
    	else
    		snprintf(display_line, 21, " %s  %s", msg[i].text, (state ? msgOff : msgOn));

    	lcdGoto(line, 0);
		lcdPuts(display_line);
    }
}

void AcHighRelaysAction(Key key)
{
    const std::string msgOn("ON");
    const std::string msgOff("OFF");

    static DisplayViewPos menu_pos(0, 0);
    static const size_t relaysSize = config::ac_high_relays.Size();

	std::string display_line;

    switch (key)
    {
    case K_ARROW_UP:
        if(menu_pos.y == 0)
            menu_pos.y = relaysSize - 1;
        else
            --menu_pos.y;
        break;
    case K_ARROW_DOWN:
        if(menu_pos.y == relaysSize - 1)
            menu_pos.y = 0;
        else
            ++menu_pos.y;
        break;
    case K_ESC:
        display_curr_view = DisplayView::MAIN_MENU;
        ViewAction(K_NONE);
        return;
    case K_ENTER:
        config::ac_high_relays
        HAL_GPIO_TogglePin(msg[menu_pos.y].dev->gpio, msg[menu_pos.y].dev->pin);
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

void DcAcRelaysAction(Key key)
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

void imitationPrinting(char *ptr)
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
