/*********************************************************************************/
/* File:        UART_interface.h                                                 */
/* Author:      Embedded Systems Level 5                                         */
/* Description: Asynchronous USART (UART) MCAL Driver Public API                 */
/*********************************************************************************/

#ifndef UART_INTERFACE_H_
#define UART_INTERFACE_H_

/* Standard Data Type Definitions (Matching System Architecture) */
typedef unsigned char       u8;
typedef unsigned short int  u16;
typedef unsigned long int   u32;

/* Common Baud Rate Selections (passed straight into UART_voidInit) */
/* The driver converts these into the correct UBRR factor for your F_CPU. */
#define UART_BAUD_2400          2400UL
#define UART_BAUD_4800          4800UL
#define UART_BAUD_9600          9600UL    /* The usual default for terminals */
#define UART_BAUD_19200         19200UL
#define UART_BAUD_38400         38400UL
#define UART_BAUD_57600         57600UL
#define UART_BAUD_115200        115200UL

/* Parity Mode Selection (maps to UPM1:0 inside UCSRC) */
#define UART_PARITY_NONE        0   /* No parity bit appended (most common) */
#define UART_PARITY_EVEN        2   /* Hardware appends and checks Even parity */
#define UART_PARITY_ODD         3   /* Hardware appends and checks Odd parity  */

/* Stop Bit Selection (maps to USBS inside UCSRC) */
#define UART_STOP_ONE           0   /* One stop bit (maximum throughput) */
#define UART_STOP_TWO           1   /* Two stop bits (extra timing cushion) */

/* ============================================================================= */
/* PUBLIC API FUNCTION PROTOTYPES                                                */
/* ============================================================================= */

/**
 * @brief Powers on the serial port and sets speed, parity and stop bits.
 * @param Copy_u32BaudRate   Options: UART_BAUD_9600, UART_BAUD_115200, etc.
 * @param Copy_u8Parity      Options: UART_PARITY_NONE, UART_PARITY_EVEN, UART_PARITY_ODD
 * @param Copy_u8StopBits    Options: UART_STOP_ONE, UART_STOP_TWO
 * @note  Frame size is fixed at the industry standard 8 data bits.
 * @note  Call this once at the start of your program.
 */
void UART_voidInit(u32 Copy_u32BaudRate, u8 Copy_u8Parity, u8 Copy_u8StopBits);

/**
 * @brief Sends one byte and waits until the transmit buffer is ready (the simple way).
 * @param Copy_u8Data  The byte to send out on the Tx pin (PD1).
 */
void UART_voidTransmitByte(u8 Copy_u8Data);

/**
 * @brief Waits for one byte to arrive and hands it back (blocking).
 * @return The byte received on the Rx pin (PD0).
 */
u8 UART_u8ReceiveByte(void);

/**
 * @brief Sends a whole text string, one character at a time.
 * @param Copy_pu8String  A null-terminated string, for example "Hello".
 */
void UART_voidTransmitString(const u8 *Copy_pu8String);

/**
 * @brief Turns on background reception so the CPU does not have to wait.
 * @note  When a byte arrives, your callback function runs automatically.
 */
void UART_voidReceiveAsync(void);

/**
 * @brief Registers the function that runs when a background byte is received.
 * @param Copy_pvCallBackFunc Pointer to your function (takes no inputs, returns nothing).
 */
void UART_voidSetCallBack(void (*Copy_pvCallBackFunc)(void));

/**
 * @brief Gives back the most recent byte captured by the background receiver.
 * @return The last byte received through the RX-complete interrupt.
 */
u8 UART_u8GetLastReceived(void);

#endif /* UART_INTERFACE_H_ */
