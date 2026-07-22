/*********************************************************************************/
/* File:        LOGGER_interface.h                                               */
/* Author:      Embedded Systems Level 5                                         */
/* Description: UART Print Helpers - public API.                                 */
/*                                                                               */
/* This is a focused version of the LOGGER that contains only the two            */
/* printing helpers needed for this activity. It has no ADC dependency and no   */
/* command handling logic. It sits in the APP layer and only calls               */
/* UART_voidTransmitByte internally.                                             */
/*                                                                               */
/* ========================== AVAILABLE FUNCTIONS ============================== */
/*                                                                               */
/*   LOGGER_voidPrintNumber(value)   prints a decimal number, e.g. 255          */
/*   LOGGER_voidPrintBinary(byte)    prints 8 binary digits, e.g. 11001010      */
/*                                                                               */
/*********************************************************************************/

#ifndef LOGGER_INTERFACE_H_
#define LOGGER_INTERFACE_H_

typedef unsigned char       u8;
typedef unsigned short int  u16;

/* ============================================================================= */
/* PRINT HELPER PROTOTYPES                                                       */
/* ============================================================================= */

/**
 * @brief Prints a number in plain decimal, e.g. 255 prints as 255.
 * @param Copy_u16Value  The number to print (0 .. 65535).
 */
void LOGGER_voidPrintNumber(u16 Copy_u16Value);

/**
 * @brief Prints a byte as 8 binary digits MSB first, e.g. 0xAA prints as 10101010
 * @param Copy_u8Value  The byte to print.
 */
void LOGGER_voidPrintBinary(u8 Copy_u8Value);

#endif /* LOGGER_INTERFACE_H_ */