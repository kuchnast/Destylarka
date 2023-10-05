/**
 * @file display.c
 * @author Kamil Uchnast
 * @brief Plik zawiera definicje funkcji zadeklarowanych w pliku display.h
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <config/ac_high_relays.hpp>
#include <config/ac_low_relays.hpp>
#include <config/dc_ac_relays.hpp>
#include <config/ds_sensors.hpp>
#include <display/display.hpp>
#include <display/hd44780.hpp>
#include <display/stm32_device.hpp>
#include <sensors/ds18b20.hpp>
#include <string>
#include <stdio.h>

DisplayView display_curr_view;

void imitationPrinting(char *ptr);

static DisplayViewVariables view_main_menu_params = (DisplayViewVariables){0, 0};
static DisplayViewVariables view_temp_sensors_params = (DisplayViewVariables){0, 0};
static DisplayViewVariables view_ac_low_relays_params = (DisplayViewVariables){0, 0};
static DisplayViewVariables view_ac_high_relays_params = (DisplayViewVariables){0, 0};
static DisplayViewVariables view_dc_ac_relays_params = (DisplayViewVariables){0, 0};

void Display_Init()
{
	lcdInit(&lcdConfig);
	lcdBackLightOn();
	Display_WelcomeScreen();
}

void Display_Clear()
{
  	lcdClrScr();
}

void Display_WelcomeScreen()
{
    display_curr_view = VIEW_EMPTY;
    char msg[] = "    DESTYLARKA    ";
    imitationPrinting(msg);
}

void Display_ViewAction(Key key)
{
    switch (display_curr_view)
    {
    case VIEW_EMPTY:
    case VIEW_MAIN_MENU:
        Display_MainMenuAction(key);
        break;
    case VIEW_TEMP_SENSORS:
        Display_TempSensorsAction(key);
        break;    
    case VIEW_AC_LOW_RELAYS:
        Display_AcLowRelaysAction(key);
        break;
    case VIEW_AC_HIGH_RELAYS:
        Display_AcHighRelaysAction(key);
        break;
    case VIEW_DC_AC_RELAYS:
        Display_DcAcRelaysAction(key);
        break;
    default:
        break;
    }
}

void Display_MainMenuAction(Key key)
{
    char msg0[] = "Odczyt temperatury";
    char msg1[] = "Przek. AC malej W";
    char msg2[] = "Przek. AC duzej W";
    char msg3[] = "Przek. AC DC";

    switch (key)
    {
    case K_ARROW_UP:
        if(view_main_menu_params.pos_y == 0)
            view_main_menu_params.pos_y = 3;
        else
            --view_main_menu_params.pos_y;
        break;
    case K_ARROW_DOWN:
        if(view_main_menu_params.pos_y == 3)
            view_main_menu_params.pos_y = 0;
        else
            ++view_main_menu_params.pos_y;
        break;
    case K_ENTER:
        switch (view_main_menu_params.pos_y)
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
        Display_ViewAction(K_NONE);
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

    switch (view_main_menu_params.pos_y)
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

void Display_TempSensorsAction(Key key)
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
        if(view_temp_sensors_params.pos_y == 0)
            view_temp_sensors_params.pos_y = 2;
        else
            --view_temp_sensors_params.pos_y;
        break;
    case K_ARROW_DOWN:
        if(view_temp_sensors_params.pos_y == 2)
            view_temp_sensors_params.pos_y = 0;
        else
            ++view_temp_sensors_params.pos_y;
        break;
    case K_ESC:
        display_curr_view = VIEW_MAIN_MENU;
        Display_ViewAction(K_NONE);
        return;
    default:
        break;
    }

    lcdClrScr();

    for(uint8_t i = view_temp_sensors_params.pos_y; i < DS_SENSORS_NUM - (2 - view_temp_sensors_params.pos_y); ++i)
    {
        error[i - view_temp_sensors_params.pos_y] = DS18B20_GetTemperatureById((DsNamesId)(i + 1),
            &(temperature[i - view_temp_sensors_params.pos_y]));
    }

    switch (view_temp_sensors_params.pos_y)
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


void Display_AcLowRelaysAction(Key key)
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
        if(view_ac_low_relays_params.pos_y == 0)
            view_ac_low_relays_params.pos_y = RELAY_AC_LOW_NUM - 1;
        else
            --view_ac_low_relays_params.pos_y;
        break;
    case K_ARROW_DOWN:
        if(view_ac_low_relays_params.pos_y == RELAY_AC_LOW_NUM - 1)
            view_ac_low_relays_params.pos_y = 0;
        else
            ++view_ac_low_relays_params.pos_y;
        break;
    case K_ESC:
        display_curr_view = VIEW_MAIN_MENU;
        Display_ViewAction(K_NONE);
        return;
    case K_ENTER:
    	HAL_GPIO_TogglePin(msg[view_ac_low_relays_params.pos_y].dev->output_pin.gpio, msg[view_ac_low_relays_params.pos_y].dev->output_pin.pin);
        break;
    default:
        break;
    }

    lcdClrScr();

    GPIO_PinState state;
    for(uint8_t line = 1, i = (view_ac_low_relays_params.pos_y > 3 ? view_ac_low_relays_params.pos_y - 3 : 0);
		i < (view_ac_low_relays_params.pos_y > 3 ? view_ac_low_relays_params.pos_y + 1 : 4); ++i, ++line)
    {
    	state = HAL_GPIO_ReadPin(msg[i].dev->output_pin.gpio, msg[i].dev->output_pin.pin);

    	if(view_ac_low_relays_params.pos_y == i)
    		snprintf(display_line, 21, ">%s  %s", msg[i].text, (state ? msgOff : msgOn));
    	else
    		snprintf(display_line, 21, " %s  %s", msg[i].text, (state ? msgOff : msgOn));

    	lcdGoto(line, 0);
		lcdPuts(display_line);
    }
}

void Display_AcHighRelaysAction(Key key)
{
	RelayACHighDisplay msg[RELAY_AC_HIGH_NUM] = {
		{"Grzalka 1 ", &known_ac_high_relays[2]},
		{"Grzalka 2 ", &known_ac_high_relays[1]},
		{"Pompa wody", &known_ac_high_relays[0]},
	};

    char msgOn[] = "ON";
    char msgOff[] = "OFF";

	char display_line[21];

    switch (key)
    {
    case K_ARROW_UP:
        if(view_ac_high_relays_params.pos_y == 0)
            view_ac_high_relays_params.pos_y = RELAY_AC_HIGH_NUM - 1;
        else
            --view_ac_high_relays_params.pos_y;
        break;
    case K_ARROW_DOWN:
        if(view_ac_high_relays_params.pos_y == RELAY_AC_HIGH_NUM - 1)
            view_ac_high_relays_params.pos_y = 0;
        else
            ++view_ac_high_relays_params.pos_y;
        break;
    case K_ESC:
        display_curr_view = VIEW_MAIN_MENU;
        Display_ViewAction(K_NONE);
        return;
    case K_ENTER:
        HAL_GPIO_TogglePin(msg[view_ac_high_relays_params.pos_y].dev->gpio, msg[view_ac_high_relays_params.pos_y].dev->pin);
        break;
    default:
    	break;
    }

    lcdClrScr();

    GPIO_PinState state;
    for(uint8_t i = 0; i < 3; ++i)
    {
    	state = HAL_GPIO_ReadPin(msg[i].dev->gpio, msg[i].dev->pin);

    	if(view_ac_high_relays_params.pos_y == i)
    		snprintf(display_line, 21, ">%s  %s", msg[i].text, (state ? msgOn : msgOff));
    	else
    		snprintf(display_line, 21, " %s  %s", msg[i].text, (state ? msgOn : msgOff));

    	lcdGoto(i + 1, 0);
		lcdPuts(display_line);
    }
}

void Display_DcAcRelaysAction(Key key)
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
        if(view_dc_ac_relays_params.pos_y == 0)
            view_dc_ac_relays_params.pos_y = 3;
        else
            --view_dc_ac_relays_params.pos_y;
        break;
    case K_ARROW_DOWN:
        if(view_dc_ac_relays_params.pos_y == 3)
            view_dc_ac_relays_params.pos_y = 0;
        else
            ++view_dc_ac_relays_params.pos_y;
        break;
    case K_ESC:
        display_curr_view = VIEW_MAIN_MENU;
        Display_ViewAction(K_NONE);
        return;
    case K_ENTER:
        HAL_GPIO_TogglePin(known_dc_ac_relays[view_dc_ac_relays_params.pos_y].gpio, known_dc_ac_relays[view_dc_ac_relays_params.pos_y].pin);
        break;
    default:
    	break;
    }

    lcdClrScr();

    lcdGoto(view_dc_ac_relays_params.pos_y, 0);
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
