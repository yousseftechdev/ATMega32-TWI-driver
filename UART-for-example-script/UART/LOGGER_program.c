/*********************************************************************************/
/* File:        LOGGER_program.c                                                 */
/* Author:      Embedded Systems Level 5                                         */
/* Description: UART Print Helpers - implementation.                             */
/*                                                                               */
/* This is a focused version of the LOGGER that contains only the two            */
/* printing helpers needed for this activity. All ADC, GPIO, and streaming       */
/* logic has been removed. The only dependency is UART_voidTransmitByte          */
/* from the UART MCAL driver.                                                    */
/*                                                                               */
/*********************************************************************************/

#include "UART_interface.h"
#include "LOGGER_interface.h"

/* ============================================================================= */
/* PRINT HELPERS                                                                 */
/* ============================================================================= */

/* Print a number in plain decimal, e.g. 255 prints as 255. */
void LOGGER_voidPrintNumber(u16 Copy_u16Value)
{
	u8 Local_au8Digits[5];
	u8 Local_u8Count = 0;

	if (Copy_u16Value == 0)
	{
		UART_voidTransmitByte('0');
		return;
	}

	/* Extract digits in reverse order */
	while (Copy_u16Value > 0)
	{
		Local_au8Digits[Local_u8Count] = (u8)('0' + (Copy_u16Value % 10));
		Copy_u16Value /= 10;
		Local_u8Count++;
	}

	/* Send digits in correct order (most significant first) */
	while (Local_u8Count > 0)
	{
		Local_u8Count--;
		UART_voidTransmitByte(Local_au8Digits[Local_u8Count]);
	}
}

/* Print a byte as 8 binary digits MSB first, e.g. 0xAA prints as 10101010 */
void LOGGER_voidPrintBinary(u8 Copy_u8Value)
{
	u8 Local_u8Bit;

	for (Local_u8Bit = 0; Local_u8Bit < 8; Local_u8Bit++)
	{
		UART_voidTransmitByte((Copy_u8Value & 0x80) ? '1' : '0');
		Copy_u8Value <<= 1;
	}
}