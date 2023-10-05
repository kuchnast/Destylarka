/**
 * @file display.h
 * @author Kamil Uchnast
 * @brief Plik zawiera deklaracje funkcji obsługujących wyświetlacz OLED
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <io/key_defs.hpp>
#include "stm32f4xx_hal.h"

/**
 * @brief Typ wyliczeniowy dostępnych widoków wyświetlacza
 * 
 */
typedef enum
{
    VIEW_EMPTY, ///< Widok pusty
    VIEW_MAIN_MENU, ///< Widok głównego menu
    VIEW_TEMP_SENSORS, ///< Widok mapy sensorów
    VIEW_AC_LOW_RELAYS, ///< Widok menu przekaźników AC małej mocy
    VIEW_AC_HIGH_RELAYS, ///< Widok menu przekaźników AC dużej mocy
    VIEW_DC_AC_RELAYS, ///< Widok menu przekaźników DC/AC
} DisplayView;

extern DisplayView display_curr_view;

/**
 * @brief Struktura przechowująca pozycję kursora widoku
 * 
 */
typedef struct
{
    uint8_t pos_x;
    uint8_t pos_y;
} DisplayViewVariables;

/**
 * @brief Inicjalizuje wyświetlacz i wyświetla ekran powitalny
 * 
 */
void Display_Init();

/**
 * @brief Czyści ekran wyświetlacza
 * 
 */
void Display_Clear();

/**
 * @brief Wyświetla widok ekranu powitalnego aplikacji
 * 
 */
void Display_WelcomeScreen();

/**
 * @brief Aktualizuje aktywnegy widok oraz przekazuje mu informację o kliknięcia odpowiedniego przycisku lub #K_NONE
 * 
 * @param key [in] -- aktywowany przycisk
 */
void Display_ViewAction(Key key);

/**
 * @brief Wyświetla widok głównego menu urządzenia
 * 
 * @param key [in] -- aktywowany przycisk
 */
void Display_MainMenuAction(Key key);

/**
 * @brief Wyświetla widok czujników temperatury
 * 
 * @param key [in] -- aktywowany przycisk
 */
void Display_TempSensorsAction(Key key);

/**
 * @brief Wyświetla widok menu
 * 
 * @param key [in] -- aktywowany przycisk
 */
void Display_AcLowRelaysAction(Key key);

/**
 * @brief Wyświetla widok menu
 * 
 * @param key [in] -- aktywowany przycisk
 */
void Display_AcHighRelaysAction(Key key);

/**
 * @brief Wyświetla widok menu
 * 
 * @param key [in] -- aktywowany przycisk
 */
void Display_DcAcRelaysAction(Key key);

#endif
