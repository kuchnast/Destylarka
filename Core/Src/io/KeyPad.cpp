#include <io/KeyPad.hpp>
#include <io/KeyPadConfig.hpp>
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#if (_KEYPAD_USE_FREERTOS==1)
#include "cmsis_os.h"
#define _KEYPAD_DELAY(x)      osDelay(x)
#else
#define _KEYPAD_DELAY(x)      HAL_Delay(x)
#endif

KeyPad_t	KeyPad;

#define keyName_SIZE 20
char keyName[keyName_SIZE];

//#############################################################################################
void	KeyPad_Init(void)
{
  GPIO_InitTypeDef	gpio;
  KeyPad.ColumnSize = sizeof(_KEYPAD_COLUMN_GPIO_PIN) / 2;
  KeyPad.RowSize = sizeof(_KEYPAD_ROW_GPIO_PIN) / 2;
  for(uint8_t	i=0 ; i<KeyPad.ColumnSize ; i++)
  {
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Pin = _KEYPAD_COLUMN_GPIO_PIN[i];
    HAL_GPIO_Init((GPIO_TypeDef*)_KEYPAD_COLUMN_GPIO_PORT[i], &gpio);
    HAL_GPIO_WritePin((GPIO_TypeDef*)_KEYPAD_COLUMN_GPIO_PORT[i], _KEYPAD_COLUMN_GPIO_PIN[i], GPIO_PIN_SET);
  }
  for(uint8_t	i=0 ; i<KeyPad.RowSize ; i++)
  {
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Pin = _KEYPAD_ROW_GPIO_PIN[i];
    HAL_GPIO_Init((GPIO_TypeDef*)_KEYPAD_ROW_GPIO_PORT[i], &gpio);		
  }
}
//#############################################################################################
uint16_t	KeyPad_Scan(void)
{
  uint16_t  key=0;
  for(uint8_t c=0 ; c<KeyPad.ColumnSize ; c++)
  {
    for(uint8_t i=0 ; i<KeyPad.ColumnSize ; i++)
      HAL_GPIO_WritePin((GPIO_TypeDef*)_KEYPAD_COLUMN_GPIO_PORT[i], _KEYPAD_COLUMN_GPIO_PIN[i], GPIO_PIN_SET);
    HAL_GPIO_WritePin((GPIO_TypeDef*)_KEYPAD_COLUMN_GPIO_PORT[c], _KEYPAD_COLUMN_GPIO_PIN[c], GPIO_PIN_RESET);
    _KEYPAD_DELAY(5);
    for(uint8_t r=0 ; r<KeyPad.RowSize ; r++)
    {
      if(HAL_GPIO_ReadPin((GPIO_TypeDef*)_KEYPAD_ROW_GPIO_PORT[r], _KEYPAD_ROW_GPIO_PIN[r]) == GPIO_PIN_RESET)
      {
        _KEYPAD_DELAY(_KEYPAD_DEBOUNCE_TIME_MS);
        if(HAL_GPIO_ReadPin((GPIO_TypeDef*)_KEYPAD_ROW_GPIO_PORT[r], _KEYPAD_ROW_GPIO_PIN[r]) == GPIO_PIN_RESET)
        {
          key |= 1<<c;					
          key |= 1<<(r+8);
          while(HAL_GPIO_ReadPin((GPIO_TypeDef*)_KEYPAD_ROW_GPIO_PORT[r], _KEYPAD_ROW_GPIO_PIN[r]) == GPIO_PIN_RESET)
            _KEYPAD_DELAY(5);
          return key;
        }
      }			
    }		
  }
  return key;
}
//#############################################################################################
uint16_t KeyPad_WaitForKeyGetUint(uint32_t  Timeout_ms)
{	
	uint16_t  keyRead;
	while(Timeout_ms==0)
	{
		keyRead = KeyPad_Scan();
		if(keyRead!=0)
		{
			KeyPad.LastKey = keyRead;
			return keyRead;	
		}
		_KEYPAD_DELAY(_KEYPAD_DEBOUNCE_TIME_MS);	
	}
	
	uint32_t	StartTime = HAL_GetTick();
	while(HAL_GetTick()-StartTime < Timeout_ms)
	{
		keyRead = KeyPad_Scan();
		if(keyRead!=0)
		{
			KeyPad.LastKey = keyRead;
			return keyRead;	
		}
		_KEYPAD_DELAY(_KEYPAD_DEBOUNCE_TIME_MS);	
	}
	KeyPad.LastKey=0;
	return 0;
}
//#############################################################################################
char* KeyPad_WaitForKeyGetName(uint32_t	Timeout_ms)
{
  memset(keyName, 0, sizeof(keyName));
  uint16_t i = KeyPad_WaitForKeyGetUint(Timeout_ms);
  switch(i)
  {
    case 0x0000:
    	strncpy(keyName, "ERROR", keyName_SIZE);
    	break;
	case 0x1001:
		strncpy(keyName, "F1", keyName_SIZE);
		break;
	case 0x1002:
		strncpy(keyName, "F2", keyName_SIZE);
		break;
	case 0x1004:
		strncpy(keyName, "#", keyName_SIZE);
		break;
	case 0x1008:
		strncpy(keyName, "*", keyName_SIZE);
		break;
	case 0x0801:
		strncpy(keyName, "1", keyName_SIZE);
		break;
	case 0x0802:
		strncpy(keyName, "2", keyName_SIZE);
		break;
	case 0x0804:
		strncpy(keyName, "3", keyName_SIZE);
		break;
	case 0x0808:
		strncpy(keyName, "UP", keyName_SIZE);
		break;
	case 0x0401:
		strncpy(keyName, "4", keyName_SIZE);
		break;
	case 0x0402:
		strncpy(keyName, "5", keyName_SIZE);
		break;
	case 0x0404:
		strncpy(keyName, "6", keyName_SIZE);
		break;
	case 0x0408:
		strncpy(keyName, "DOWN", keyName_SIZE);
		break;
	case 0x0201:
		strncpy(keyName, "7", keyName_SIZE);
		break;
	case 0x0202:
		strncpy(keyName, "8", keyName_SIZE);
		break;
	case 0x0204:
		strncpy(keyName, "9", keyName_SIZE);
		break;
	case 0x0208:
		strncpy(keyName, "ESC", keyName_SIZE);
		break;
	case 0x0101:
		strncpy(keyName, "LEFT", keyName_SIZE);
		break;
	case 0x0102:
		strncpy(keyName, "0", keyName_SIZE);
		break;
	case 0x0104:
		strncpy(keyName, "RIGHT", keyName_SIZE);
		break;
	case 0x0108:
		strncpy(keyName, "ENTER", keyName_SIZE);
		break;
	default:
		sprintf(keyName, "UNKNOWN: %04X", i);
		break;
  }

  return keyName;
}

Key KeyPad_WaitForKeyGetKey(uint32_t Timeout_ms)
{
  uint16_t i = KeyPad_WaitForKeyGetUint(Timeout_ms);
  switch(i)
  {
    case 0x0000:
		return K_NONE;
    	break;
	case 0x1001:
		return K_F1;
		break;
	case 0x1002:
		return K_F2;
		break;
	case 0x1004:
		return K_HASH;
		break;
	case 0x1008:
		return K_STAR;
		break;
	case 0x0801:
		return K_1;
		break;
	case 0x0802:
		return K_2;
		break;
	case 0x0804:
		return K_3;
		break;
	case 0x0808:
		return K_ARROW_UP;
		break;
	case 0x0401:
		return K_4;
		break;
	case 0x0402:
		return K_5;
		break;
	case 0x0404:
		return K_6;
		break;
	case 0x0408:
		return K_ARROW_DOWN;
		break;
	case 0x0201:
		return K_7;
		break;
	case 0x0202:
		return K_8;
		break;
	case 0x0204:
		return K_9;
		break;
	case 0x0208:
		return K_ESC;
		break;
	case 0x0101:
		return K_ARROW_LEFT;
		break;
	case 0x0102:
		return K_0;
		break;
	case 0x0104:
		return K_ARROW_RIGHT;
		break;
	case 0x0108:
		return K_ENTER;
		break;
	default:
		return K_NONE;
		break;
  }
}
