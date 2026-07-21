#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "TWI_interface.h"
#include "UART_interface.h"

/* ============================================================================= */
/* HELPER FUNCTIONS FOR UART PRINTING                                            */
/* ============================================================================= */

#define LOG(str) UART_voidTransmitString((const u8 *) (str))

// Prints a byte as a 2-character Hex string (e.g., 0x1A)
void UART_PrintHex(u8 value) {
    char hex_chars[] = "0123456789ABCDEF";
    UART_voidTransmitByte(hex_chars[(value >> 4) & 0x0F]);
    UART_voidTransmitByte(hex_chars[value & 0x0F]);
}

// Prints a byte as a Decimal number (e.g., 42)
void UART_PrintDecimal(u8 value) {
    if (value >= 100) UART_voidTransmitByte('0' + (value / 100));
    if (value >= 10)  UART_voidTransmitByte('0' + ((value / 10) % 10));
    UART_voidTransmitByte('0' + (value % 10));
}

/* ============================================================================= */
/* MAIN PROGRAM                                                                  */
/* ============================================================================= */

int main(void) {
    // 1. Initialize UART at 9600 baud, no parity, 1 stop bit
    // (Make sure F_CPU is defined in your compiler settings, e.g., -DF_CPU=8000000UL)
    UART_voidInit(UART_BAUD_9600, UART_PARITY_NONE, UART_STOP_ONE);
    
    // 2. Initialize TWI at 100kHz, with interrupts enabled
    TWI_vInit(100000, TWI_ENABLE_INTERRUPT);
    
    // 3. Enable global interrupts so the TWI ISR can run
    sei();
    
    LOG("\r\n--- Starting I2C Bus Scan ---\r\n");
    
    u8 found_count = 0;
    u8 dummy_data = 0; // Required to pass a valid pointer, even if size is 0
    
    // 4. Scan all valid 7-bit I2C addresses (0x08 to 0x77)
    for (u8 address = 0x08; address < 0x78; address++) {
        
        // Wait for the bus to be completely free from the previous probe
        while (TWI_bIsBusy()); 
        
        // Start a 0-byte write transaction to probe the address
        TWI_bSendData(address, &dummy_data, 0);
        
        // Wait for the ISR to finish the transaction (Send ACK/NACK and STOP)
        while (TWI_bIsBusy());
        
        // Check the status returned by the ISR
        if (TWI_u8GetTransactionStatus()) {
            // 0x18 means SLA+W transmitted, ACK received -> Device is present!
            LOG("I2C Device found at address: 0x");
            UART_PrintHex(address);
            LOG("\r\n");
            found_count++;
        }
    }
    
    LOG("--- Scan Complete. Found ");
    UART_PrintDecimal(found_count);
    LOG(" device(s). ---\r\n");

    // Infinite loop
    while (1) {
        // Your application code here
    }
}