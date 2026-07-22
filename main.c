#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "TWI_interface.h"
#include "UART_interface.h"
#include "LOGGER_interface.h"

#define F_CPU 16000000UL

#define LOG(str) UART_voidTransmitString((const u8 *)(str))

int main(void)
{
    UART_voidInit(UART_BAUD_9600, UART_PARITY_NONE, UART_STOP_ONE);
    TWI_vInit(400000, TWI_ENABLE_INTERRUPT);
    sei();
    _delay_ms(5);
    LOG("\r\n--- Starting I2C Bus Scan ---\r\n");
    _delay_ms(5);
    u8 found_count = 0;
    u8 dummy_data = 0;
    for (u8 address = 0x08; address < 0x78; address++)
    {
        while (TWI_bIsBusy());
        TWI_bSendData(address, &dummy_data, 0);
        while (TWI_bIsBusy());
        if (TWI_boolGetTransactionStatus())
        {
            LOGGER_voidPrintNumber(address);
            LOG("... ");
            LOG("DEVICE FOUND!\r\n");
            found_count++;
        }
        _delay_ms(5);
    }
    LOG("--- Scan Complete. Found ");
    LOGGER_voidPrintNumber(found_count);
    LOG(" device(s). ---\r\n");
}