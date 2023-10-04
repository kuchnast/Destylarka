/*------------------------------------------------------*/
/* File:       Library for HD44780 compatible displays  */
/* Version:	   v4.15  						 			*/
/* Author:     Andrii Honcharenko						*/
/* 			   https://stm32withoutfear.blogspot.com	*/
/* Tested on:  STM32 HAL library, PSoC4 PDL2 library	*/
/* License:	   GNU LGPLv2.1		 		 	 			*/
/*------------------------------------------------------*/
/* Copyright (C)2021 AH All right reserved				*/
/*------------------------------------------------------*/
#include "display/hd44780.h"
#include "stddef.h"

/*!	\brief	Macro-definitions. */
#if (USE_PROGRESS_BAR)
/*!	\brief	Progress bar definitions. */
#define CGROM_PROGRESS_BAR_SIZE		6u
#define FULL_LOAD					5u
#define EMPTY_LOAD					0u
#define EMPTY_ROW					0x00u /* xxx00000 */

static const uint8_t progress_bar[CGROM_PROGRESS_BAR_SIZE] = {0x00u,0x10u,0x18u,0x1Cu,0x1Eu,0x1Fu};
static uint8_t current_bar_pixel;
static uint8_t current_cell_load;
static void lcdInitBar(void);
#endif

static lcdI2C_ConfigStruct* i2cConfig;
static uint8_t sendInternal(uint8_t data, uint8_t flags);


/*!	\brief	Low-level functions. */
static void lcdConfig(uint8_t param);
static uint32_t lcdPow10(uint8_t n);

static uint8_t sendInternal(uint8_t data, uint8_t flags)
{
	if(i2cConfig->SendData != NULL)
	{
		return i2cConfig->SendData(data, flags);
	}
	else
	{
		return 0;
	}
}

/*!	\brief	Initializing by instruction. 4-bit or i2c interface initialization. */
static void lcdConfig(uint8_t param)
{
/* Low level function. */
	sendInternal(param, 0);
}

							//-----------------------------//
							/*         LCDlib API          */
							//-----------------------------//

/*!	\details	Clear display writes space code 20H into all DDRAM addresses.
 * 				It then sets DDRAM address 0 into the address counter,
 * 				and returns the display to its original status if it was shifted.
 * 				In other words, the display disappears and the cursor
 * 				or blinking goes to the left edge of the display (in the first line if 2 lines are displayed).
 * 				It also sets I/D to 1 (increment mode) in entry mode (S of entry mode does not change). */
void lcdClrScr(void)
{
	/* Send a command to LCD. */
	sendInternal(0x01u, 0);
}

/*!	\details	"Return home" sets DDRAM address 0 into the address counter,
 * 				and returns the display to its original status if it was shifted.
 * 				The DDRAM contents do not change.
 * 				The cursor or blinking go to the left edge of the display
 * 				(in the first line if 2 lines are displayed). */
void lcdReturn(void)
{
	/* Send a command to LCD. */
	sendInternal(0x02u, 0);
}

/*!	\details	lcdScroll shifts the display to the right or left without writing or reading display data.
 * 				This function is used to correct or search the display.
 *	\note		The first and second line displays will shift at the same time. */
void lcdScroll(uint8_t direction)
{
	/* Send a command to LCD. */
	switch (direction)
	{
	/* To left */
		case LEFT  :
			sendInternal(0x18u, 0);
			break;

		/* To right */
		case RIGHT :
			sendInternal(0x1Cu, 0);
			break;

		default:
			/* Ignore this command */
			break;
	}
}

/*!	\details	"Cursor shift" shifts the cursor position to the right or left,
 * 				without writing or reading display data.
 * 				This function is used to correct or search the display.
 * 				In a 2-line display, the cursor moves to the second line
 * 				when it passes the 40th digit of the first line. */
void cursorShift(uint8_t direction)
{
	/* Send a command to LCD. */
	switch (direction)
	{
		/* To left */
		case LEFT  :
			sendInternal(0x10u, 0);
			break;

		/* To right */
		case RIGHT :
			sendInternal(0x14u, 0);
			break;

		default:
			/* Ignore this command */
			break;
	}
}

/*!	\details	Go to the specified (DDRAM/CGRAM) memory address.*/
void lcdGoto(uint8_t line, uint8_t address)
{
	/* Send a command to LCD. */
	switch (line)
	{
		/* Set DDRAM address. */
		case LCD_1st_LINE: sendInternal(0x80u | START_ADDRESS_1st_LINE | address, 0); break;
		case LCD_2nd_LINE: sendInternal(0x80u | START_ADDRESS_2nd_LINE | address, 0); break;
		case LCD_3rd_LINE: sendInternal(0x80u | START_ADDRESS_3rd_LINE | address, 0); break;
		case LCD_4th_LINE: sendInternal(0x80u | START_ADDRESS_4th_LINE | address, 0); break;

		/* Set CGRAM address. */
		case CGRAM : sendInternal(0x40u | address, 0); break;

		default:
			/* Ignore this command */
			break;
	}
}

/*!	\details	Change LCD settings. */
void lcdSetMode(uint8_t param)
{
	/* Send a command to LCD. */
	sendInternal(param, 0);
}

/*!	\details	Write a single char to the current memory space (DDRAM/CGRAM). */
void lcdPutc(uint8_t data)
{
	/* Send data to LCD. */
	sendInternal(data, i2cConfig->rs_pin);
}

/*!	\details	Writes ANSI-C string to LCD (DDRAM memory space). */
//void lcdPuts(const uint8_t *str)
void lcdPuts(const char *str)
{
	/* Send a ANSI-C string to LCD. */
	while ('\0' != *str)
	{
#if ( USE_FORMATTED_OUTPUT )
		if(('\n' == *str))
		{/*New line */
			lcdGoto(LCD_2nd_LINE, 0u);
		}
		else if(('\r' == *str))
		{/* Return home */
			lcdReturn();
		}
		else if(('\t' == *str))
		{/* Tab space */
			uint8_t i;

			for(i=0u; i<TAB_SPACE; i++)
			{/* Shift cursor to the right. */
				cursorShift(RIGHT);
			}
		}
		else
#endif
		{
			/* Display a symbol. */
			lcdPutc(*str);
		}
		/* Get the next symbol. */
		str++;
	}
}

/*!	\details	Load the user-defined symbol into the CGRAM memory. */
void lcdLoadChar(uint8_t* vector, uint8_t position)
{
	uint8_t i;
	/* Go to the CGRAM memory space: 0 to 7 */
	lcdGoto(CGRAM, (position * FONT_HEIGHT));

	for(i = 0u; i < FONT_HEIGHT; i++)
	{/* Load one row of pixels into the CGRAM register. */
		lcdPutc(vector[i]);
	}

	/* Return to the DDRAM memory space. */
	lcdGoto(LCD_1st_LINE, 0u);
}

/*!	\details	Load and display the user-defined symbol. */
void lcdDrawChar( uint8_t* vector,
			   	  uint8_t position,
			   	  uint8_t line,
			   	  uint8_t address )
{
	/* Load the user-defined symbol into the CGRAM memory. */
	lcdLoadChar(vector, position);
	/* Select LCD position. */
	lcdGoto(line, address);
	/* Display the user-defined symbol. */
	lcdPutc(position);
}

/*!	\details	Erase a symbol from the left of the cursor. */
void lcdBackSpace(void)
{
	cursorShift(LEFT);
	lcdPutc(' ');
	cursorShift(LEFT);
}

/*!	\brief	Returns 10^n value. */
static uint32_t lcdPow10(uint8_t n)
{
	uint32_t retval = 1u;

	while (n > 0u)
	{
		retval *= 10u;
		n--;
	}

	return retval;
}

/*!	\brief	Display a integer number: +/- 2147483647. */
void lcdItos(int32_t value)
{
	int32_t i;

	if (value < 0)
	{
		lcdPutc('-');
		value = -value;
	}

	i = 1;
	while ((value / i) > 9)
	{
		i *= 10;
	}

	lcdPutc(value/i + '0');	/* Display at least one symbol */
	i /= 10;

	while (i > 0)
	{
		lcdPutc('0' + ((value % (i*10)) / i));
		i /= 10;
	}
}

/*!	\brief	Display a floating point number. */
void lcdFtos(float value, uint8_t n)
{
	if (value < 0.0)
	{
		lcdPutc('-');
		value = -value;
	}

	lcdItos((int32_t)value);

	if (n > 0u)
	{
		lcdPutc('.');

		lcdNtos((uint32_t)(value * (float)lcdPow10(n)), n);
	}
}

/*!	\brief	Display "n" right digits of "value". */
void lcdNtos(uint32_t value, uint8_t n)
{
	if (n > 0u)
	{
		uint32_t i = lcdPow10(n - 1u);

		while (i > 0u)	/* Display at least one symbol */
		{
			lcdPutc('0' + ((value/i) % 10u));

			i /= 10u;
		}
	}
}

#if ( USE_PROGRESS_BAR )
/*!	\brief	Initialize the progress bar
 * 			(i.e. preload elements of the progress bar into CGRAM and reset all variables). */
static void lcdInitBar(void)
{
	uint8_t i, j;

	for (i = 0u; i < CGROM_PROGRESS_BAR_SIZE; i++)
	{
		lcdGoto(CGRAM, (i * FONT_HEIGHT));

		for (j = 0u; j < FONT_HEIGHT; j++)
		{
			if (j < PROGRESS_BAR_HEIGHT)
			{
				lcdPutc(progress_bar[i]);
			}
			else
			{/* Load an empty row of pixels in CGRAM. */
				lcdPutc(EMPTY_ROW);
			}
		}
	}

	/* Clear the entire bar and initialize all variables. */
	lcdClrBar();
}

/*!	\brief	Draw progress bar. */
void lcdDrawBar(uint8_t next_bar_pixel)
{
	/* Go to the current cell position in the progress bar. */
	lcdGoto(PROGRESS_BAR_LINE, (current_bar_pixel / FONT_WIDTH));

	if (next_bar_pixel > current_bar_pixel)
	{
		/* Increment LCD cursor */
		lcdSetMode(ENTRY_MODE_INC_NO_SHIFT);

		/* Prevent the progress bar overflow */
		if (next_bar_pixel > PROGRESS_BAR_MAX_LOAD)
		{
			next_bar_pixel = PROGRESS_BAR_MAX_LOAD;
		}

		while (current_bar_pixel != next_bar_pixel)
		{
			/* Go to the next pixel. */
			current_bar_pixel++;
			current_cell_load++;
			/* Display the load of the current cell. */
			lcdPutc(current_cell_load);

			if (current_cell_load < FULL_LOAD)
			{/* Return the cursor to the current cell. */
				cursorShift(LEFT);
			}
			else
			{/* Go to the next cell. */
				current_cell_load = EMPTY_LOAD;
			}
		}
	 }
#if (USE_REGRESS_BAR)
	 else if (next_bar_pixel < current_bar_pixel)
	 {
		 /* Decrement LCD cursor */
		lcdSetMode(ENTRY_MODE_DEC_NO_SHIFT);

		do
		{
			if (EMPTY_LOAD == current_cell_load)
			{/* Go to the next cell. */
				cursorShift(LEFT);
				current_cell_load = FULL_LOAD;
			}
			/* Go to the next pixel. */
			current_bar_pixel--;
			current_cell_load--;
			/* Display the load of the current cell. */
			lcdPutc(current_cell_load);
			/* Return the cursor to the current cell. */
			cursorShift(RIGHT);
		}
		while (current_bar_pixel != next_bar_pixel);
	 }
#endif /* USE_REGRESS_BAR */
	 else
	 {
		 /* Nothing to do. */
	 }

	/* Restore the default entry mode. */
	lcdSetMode(DEFAULT_ENTRY_MODE);
	/* Return home. */
	lcdGoto(LCD_1st_LINE, 0u);
}

/*!	\brief	Clear the entire progress bar. */
void lcdClrBar(void)
{
	uint8_t i;
	/* Go to the last cell in the progress bar. */
	lcdGoto(PROGRESS_BAR_LINE, (PROGRESS_BAR_WIDTH - 1u));
	/* Set the decrement mode. */
	lcdSetMode(ENTRY_MODE_DEC_NO_SHIFT);

	for(i = 0u; i < PROGRESS_BAR_WIDTH; i++)
	{/* Display the "empty cell" symbol (i.e. clear the LCD cell). */
		lcdPutc(EMPTY_LOAD);
	}

	/* Reset the progress bar variables. */
	current_bar_pixel = 0u;
	current_cell_load = EMPTY_LOAD;

	/* Restore the default entry mode. */
	lcdSetMode(DEFAULT_ENTRY_MODE);
	/* Return home. */
	lcdGoto(LCD_1st_LINE, 0u);
}
#endif
/*!	\brief	Initialize the LCD.
 * 	\note	This library use the I2C interface. */
void lcdInit(void* config)
{
	i2cConfig = (lcdI2C_ConfigStruct*) config;
	if(i2cConfig->InitPeriph != NULL)
	{
		i2cConfig->InitPeriph();
	}
	sendInternal(0x03, 0);
	sendInternal(0x02, 0);
#ifdef USE_LCD2004
	sendInternal(0x03, 0);
	sendInternal(0x02, 0);
#endif /* USE_LCD2004 */
	lcdConfig(DEFAULT_DISPLAY_CONFIG);
	lcdSetMode(DEFAULT_VIEW_MODE);
	lcdSetMode(DEFAULT_ENTRY_MODE);
	lcdClrScr();
	lcdReturn();
	#if (USE_PROGRESS_BAR)
		lcdInitBar();
	#endif
}

void lcdBackLightOn(void)
{
	i2cConfig->bl_pin = (1 << 3);
	sendInternal(0x0Fu, 0);
}

void lcdBackLightOff(void)
{
	i2cConfig->bl_pin = (0 << 3);
	sendInternal(0x0Fu, 0);
}

//-------------------------------
/* END OF FILE */
//-------------------------------
