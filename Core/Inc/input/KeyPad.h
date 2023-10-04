#ifndef	_KEYPAD_H
#define	_KEYPAD_H

#include "input/key_defs.h"

#include <stdint.h>
#include <stdbool.h>


/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    1.0.0
  
  
  Reversion History:
  
  (1.0.0)
  First Release.
*/

typedef struct
{
	uint8_t   ColumnSize;
	uint8_t   RowSize;
	uint16_t  LastKey;	
	
}KeyPad_t;

void      KeyPad_Init(void);
uint16_t  KeyPad_WaitForKeyGetUint(uint32_t	Timeout_ms);
char*     KeyPad_WaitForKeyGetName(uint32_t	Timeout_ms);
Key       KeyPad_WaitForKeyGetKey(uint32_t	Timeout_ms);

#endif
