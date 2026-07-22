/*********************************************************************************/
/* File:        UART_program.c                                                   */
/* Author:      Embedded Systems Level 5                                         */
/* Description: Low-Level Hardware Manipulation Source Code for ATmega32 USART.  */
/* Handles baud generation, the URSEL shared-address steering, frame setup,      */
/* blocking transmit/receive and the background RX-complete ISR.                 */
/*********************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART_interface.h"

/* Null Definition for defensive callback initialization guards */
#define NULL    ((void*)0)

/* Private storage for the registered upper-layer receive-complete callback */
static void (*UART_pvCallBackNotification)(void) = NULL;

/* Private holding cell for the latest byte captured by the background ISR */
static volatile u8 UART_u8LastReceived = 0;

/* ============================================================================= */
/* IMPLEMENTATIONS: PERIPHERAL CONFIGURATION AND CONTROL FUNCTIONS               */
/* ============================================================================= */

void UART_voidInit(u32 Copy_u32BaudRate, u8 Copy_u8Parity, u8 Copy_u8StopBits)
{
    u16 Local_u16UbrrValue = 0;

    /* Work out the baud factor from the datasheet formula: */
    /* UBRR = ( F_CPU / (16 * BaudRate) ) - 1                */
    Local_u16UbrrValue = (u16)((F_CPU / (16UL * Copy_u32BaudRate)) - 1UL);

    /* Load the low 8 bits of the factor. UBRRL has its own address, no steering. */
    UBRRL = (u8)(Local_u16UbrrValue);

    /* Load the high 4 bits. UBRRH shares an address with UCSRC, so Bit 7 (URSEL) */
    /* must stay 0 here to steer the write into the baud generator, not UCSRC.    */
    UBRRH = (u8)((Local_u16UbrrValue >> 8) & 0x0F);

    /* Build the frame format in UCSRC. Because UCSRC shares the address with     */
    /* UBRRH, Bit 7 (URSEL) must be 1 so the write lands in UCSRC.                 */
    /* UMSEL = 0 keeps us asynchronous. UCSZ1:0 = 11 sets the 8-bit data payload.  */
    UCSRC = (1 << URSEL)
          | (1 << UCSZ1) | (1 << UCSZ0)
          | (u8)((Copy_u8Parity & 0x03) << UPM0)
          | (u8)((Copy_u8StopBits & 0x01) << USBS);

    /* The third character-size bit, UCSZ2, lives in UCSRB and stays 0 for 8 bits. */
    /* Power on both the receiver (RXEN) and the transmitter (TXEN). This step     */
    /* takes over pins PD0 and PD1 for serial use.                                 */
    UCSRB = (1 << RXEN) | (1 << TXEN);
}


void UART_voidTransmitByte(u8 Copy_u8Data)
{
    /* Wait until the transmit buffer is empty. UDRE goes high when it is ready. */
    while ((UCSRA & (1 << UDRE)) == 0)
    {
        /* just waiting for the buffer to clear */
    }

    /* Drop the byte into UDR. Writing UDR routes it into the transmit buffer. */
    UDR = Copy_u8Data;
}


u8 UART_u8ReceiveByte(void)
{
    /* Wait until a full byte has arrived. RXC goes high when one is ready. */
    while ((UCSRA & (1 << RXC)) == 0)
    {
        /* just waiting for an incoming byte */
    }

    /* Reading UDR fetches the byte from the receive buffer. */
    return UDR;
}


void UART_voidTransmitString(const u8 *Copy_pu8String)
{
    u16 Local_u16Index = 0;

    if (Copy_pu8String != NULL)
    {
        /* Walk the string until we hit the terminating null character. */
        while (Copy_pu8String[Local_u16Index] != '\0')
        {
            UART_voidTransmitByte(Copy_pu8String[Local_u16Index]);
            Local_u16Index++;
        }
    }
}


void UART_voidReceiveAsync(void)
{
    /* Turn on the receive-complete interrupt so the CPU does not have to poll. */
    /* Each arriving byte now fires the ISR and refreshes the stored value.     */
    UCSRB |= (1 << RXCIE);
}


void UART_voidSetCallBack(void (*Copy_pvCallBackFunc)(void))
{
    if (Copy_pvCallBackFunc != NULL)
    {
        UART_pvCallBackNotification = Copy_pvCallBackFunc;
    }
}


u8 UART_u8GetLastReceived(void)
{
    return UART_u8LastReceived;
}

/* ============================================================================= */
/* INTERRUPT SERVICE ROUTINE (ISR) - USART RECEIVE COMPLETE VECTOR               */
/* ============================================================================= */

/**
 * @brief Runs automatically when a background byte finishes arriving.
 * @note  Stores the new byte and calls the user callback if one was set.
 */
ISR(USART_RXC_vect)
{
    /* Reading UDR both grabs the new byte and clears the RXC flag. */
    UART_u8LastReceived = UDR;

    /* Tell the application a fresh byte is ready, if a callback was set. */
    if (UART_pvCallBackNotification != NULL)
    {
        UART_pvCallBackNotification();
    }
}
