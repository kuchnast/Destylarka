/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"

#include <config/AlarmWithNotification.hpp>
#include <config/RelaysAcHigh.hpp>
#include <config/RelaysAcLow.hpp>
#include <display/Display.hpp>
#include <io/FunctionTimer.hpp>
#include <io/Keypad.hpp>
#include <io/Logger.hpp>
#include <io/Valve.hpp>
#include <sensors/Ds18b20.hpp>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string>
#include <cstdio>
#include <cstdlib>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
io::Logger logger("Main");
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void UpdateTemperatures(sensors::Ds18b20Collection& ds_collection)
{
    ds_collection.startRangingAll();

    uint32_t start_time = HAL_GetTick();
    bool is_all_done;
    while(HAL_GetTick() - start_time < config::ds_ranging_timeout_ms)
    {
        is_all_done = ds_collection.allDone();
        if(is_all_done)
        {
            break;
        }
        HAL_Delay(config::ds_ranging_delay_ms);
    }

    if(!is_all_done)
        logger.warning() << "Ranging timed out - not all sensors are ready.";

    if(ds_collection.readAll())
    {
        logger.error() << "Error occurred when reading one or many DS18B20 sensor temperatures. Continuing..";
    }
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN init */

    /* USER CODE END init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_I2C1_Init();
    MX_TIM1_Init();
    /* USER CODE BEGIN 2 */
    io::Keypad keypad{};
    bool tempUpdated = false;

    io::GpioPin oneWirePin(DS18B20_GPIO_Port, DS18B20_Pin);
    communication::OneWire oneWire{oneWirePin, htim1};
    sensors::Ds18b20Collection ds_collection(oneWire, true);

    if(ds_collection.addSensors(config::ds_sensors, 12))
        logger.error() << "Error occurred when adding one or many DS18B20 sensors. Continuing..";

    display::Display display{ds_collection};
    display.init(&hi2c1);
    display.viewAction(config::Key::NONE);

    io::FunctionTimer::addFunction([&ds_collection, &tempUpdated]()
        {
    		UpdateTemperatures(ds_collection);
    		tempUpdated = true;
        }, {3000}, "UpdateTemperatures", true);

    io::FunctionTimer::addFunction([]()
        {
            config::ac_low_relays.Find(config::RelayACLowId::ZAWOR_LM_SAM_ZAMKNIECIE).toggle();
            config::ac_low_relays.Find(config::RelayACLowId::ZAWOR_LM_PLUS_PRECYZYJNY_ZAMKNIECIE).toggle();
            config::ac_low_relays.Find(config::RelayACLowId::ZAWOR_VM_ODBIORU_ZAMKNIECIE).toggle();
        }, {1, 50000}, "ToggleCloseOfAllValves");

    io::FunctionTimer::addFunction([&ds_collection]()
    {
        constexpr float limitTemp = 40.0;
        constexpr float hysteresis = 2.0;

        auto tempMaybe = ds_collection.getTemperatureMaybe(config::Ds18b20NameId::KOLUMNA_DOL);
        auto radiator = config::ac_high_relays.Find(config::RelayACHighId::POMPA_WODY);
        if(tempMaybe)
        {
            if(radiator.read() == io::PinState::RESET)
            {
                if(tempMaybe.value() > limitTemp)
                {
                    io::Logger("RadiatorControl").error() << "Enable radiator";
                    radiator.set();
                }
            }
            else
            {
                if(tempMaybe.value() < limitTemp - hysteresis)
                {
                    io::Logger("RadiatorControl").error() << "Disable radiator";
                    radiator.reset();
                }
            }
        }
        else
        {
            io::Logger("RadiatorControl").error() << "Can't get temperature of Kolumna Dol";
        }
    }, {10000}, "RadiatorControl", true);

    io::FunctionTimer::addFunction([&ds_collection]()
        {
            constexpr float limitTemp = 42.0;

            auto tempMaybe = ds_collection.getTemperatureMaybe(config::Ds18b20NameId::CHLODNICA_POWROT);

            if(tempMaybe)
            {
                if(tempMaybe.value() > limitTemp)
                {
                    auto heater_1 = config::ac_high_relays.Find(config::RelayACHighId::GRZALKA_1);
                    auto heater_2 = config::ac_high_relays.Find(config::RelayACHighId::GRZALKA_2);

                    if(heater_1.read() != io::PinState::RESET || heater_2.read() != io::PinState::RESET)
                    {
                        io::Logger("DisableHeatersWhenCoolingFailed").error() << "Disable heaters";

                        heater_1.reset();
                        heater_2.reset();
                        config::alarm_with_notification.enable("TFCoolingFailed", "Disable heaters");
                    }
                }
            }
            else
            {
                io::Logger("DisableHeatersWhenCoolingFailed").error() << "Can't get temperature of Chlodnica Powrot";
            }
        }, {10000}, "DisableHeatersWhenCoolingFailed", true);

    io::FunctionTimer::addFunction([&ds_collection]()
        {
            constexpr float limitTemp_Kapilara = 99.88;
            constexpr float limitTemp_Pianka = 99.88;
            auto tempMaybe_Kapilara = ds_collection.getTemperatureMaybe(config::Ds18b20NameId::ZBIORNIK_W_KAPILARZE);
            auto tempMaybe_Pianka = ds_collection.getTemperatureMaybe(config::Ds18b20NameId::ZBIORNIK_POD_PIANKA);
            static uint8_t countdown_kapilara = 0, countdown_pianka = 0;
            constexpr uint8_t countdown_limit = 6; // about 60 seconds countdown
            static bool task_added_flag = false;

            if(!task_added_flag)
            {
                if (tempMaybe_Kapilara)
                {
                    if (tempMaybe_Kapilara.value() > limitTemp_Kapilara)
                    {
                        if (countdown_kapilara++ > countdown_limit)
                        {
                            io::FunctionTimer::addFunction([]()
                                {
                                    config::ac_high_relays.Find(config::RelayACHighId::GRZALKA_1).reset();
                                    config::ac_high_relays.Find(config::RelayACHighId::GRZALKA_2).reset();
                                }, {900000}, "DisableHeatersWhenDistillationFinishedLatch", false);
                            task_added_flag = true;
                            return;
                        }
                    }
                    else if (countdown_kapilara > 0)
                    {
                        countdown_kapilara = 0;
                    }
                }
                else
                {
                    io::Logger("DisableHeatersWhenDistillationFinished").error() << "Can't get temperature of Zbiornik w kapilarze";
                }

                if (tempMaybe_Pianka)
                {
                    if (tempMaybe_Pianka.value() > limitTemp_Pianka)
                    {
                        if (countdown_pianka++ > countdown_limit)
                        {
                            io::FunctionTimer::addFunction([]()
                                {
                                    config::ac_high_relays.Find(config::RelayACHighId::GRZALKA_1).reset();
                                    config::ac_high_relays.Find(config::RelayACHighId::GRZALKA_2).reset();
                                }, {900000}, "DisableHeatersWhenDistillationFinishedLatch", false);
                            task_added_flag = true;
                            return;
                        }
                    }
                    else if (countdown_pianka > 0)
                    {
                        countdown_pianka = 0;
                    }
                }
                else
                {
                    io::Logger("DisableHeatersWhenDistillationFinished").error() << "Can't get temperature of Zbiornik pod pianka";
                }
            }



        }, {10000}, "DisableHeatersWhenDistillationFinished", true);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    constexpr uint16_t buf_size = 20;
    char buf[buf_size];
    uint8_t pos = 0;
    HAL_StatusTypeDef status;
    bool command_from_stdi;
    config::Key key;


    while (true)
    {
    	command_from_stdi = false;
        do
        {
            if(pos < 20)
            {
                status = HAL_UART_Receive(&huart2, (uint8_t *)(buf + pos), 1, 100);
                if (status == HAL_OK)
                {
                	if (buf[pos] == '\n')
                	{
                		buf[pos] = '\0';
                		++pos;
                		break;
                	}
                	++pos;
                }
            }
            else
            {
                buf[19] = '\0';
                pos = 19;
                break;
            }
        }
        while(pos);

        if(pos and buf[pos - 1] == '\0')
        {
            std::string str(buf, pos - 1);
            logger.info() << "Received string (" << str.size() << "): " << str << "\n";
            key = config::toKey(str);
            pos = 0;
            command_from_stdi = true;
        }

        switch (display.getCurrentView())
        {
            case display::DisplayView::EMPTY:
            case display::DisplayView::MAIN_MENU:
            case display::DisplayView::AC_LOW_RELAYS:
            case display::DisplayView::AC_HIGH_RELAYS:
            case display::DisplayView::DC_AC_RELAYS:
            case display::DisplayView::SET_ALARM:
            case display::DisplayView::TEMP_SENSORS:
            {
                if(!command_from_stdi or key == config::Key::NONE)
                {
                    key = keypad.waitForKey(config::keypad_debounce_time_ms);
                }

                if (key != config::Key::NONE || tempUpdated)
                {
                    if (key == config::Key::F1)
                    {
                        if(config::alarm_with_notification.is_muted())
                            config::alarm_with_notification.unmute();
                        else
                            config::alarm_with_notification.mute();
                        key = config::Key::NONE;
                    }

                    display.viewAction(key);
                    tempUpdated = false;
                }
                break;
            }
            default:
                break;
        }

        io::FunctionTimer::handleFunctionsWithTimeout();

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
  */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
