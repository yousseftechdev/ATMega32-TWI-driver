/*********************************************************************************/
/* File:        TWI_program.c                                                    */
/* Author:      Youssef Mostafa                                                  */
/* Description: Low-Level Hardware Manipulation Source Code for ATmega32 TWI.    */
/*********************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "TWI_interface.h"

/* Private State Variables for the ISR */
static u8 *TWI_pDataBuffer;    // Pointer to the user's data array
static u8 TWI_u8DataSize;      // Total bytes to send/receive
static u8 TWI_u8DataCounter;   // Current byte index
static u8 TWI_u8SlaveAddress;  // Target slave address
static bool TWI_boolDirection; // true = Read, false = Write
static bool TWI_boolIsBusy;    // Flag to prevent overlapping transactions

/* Set interrupt handler */
ISR(TWI_vect)
{
    TWI_vIntHandler();
}

void TWI_vInit(u32 u32Freq, bool boolInterruptEnable)
{
    /* Set frequency */
    TWI_vSetFrequency(u32Freq);

    /* Enable TWI peripheral */
    TWCR |= (1 << TWEN);

    /* Check if user wants to enable interrupts */
    if (boolInterruptEnable)
    {
        TWI_vEnableInterrupt();
    }
}

void TWI_vSetFrequency(u32 u32Freq)
{
}

void TWI_vSetOwnSlaveAddress(u8 u8Address, bool boolGeneralCall)
{
    TWAR = (u8Address << 1) | boolGeneralCall;
}

void TWI_vAcknowledgeOwnAddress(bool boolAcknowledgeEnable)
{
    if (boolAcknowledgeEnable)
        TWCR |= (1 << TWEA);
    else
        TWCR &= ~(1 << TWEA);
}

void TWI_vEnableInterrupt(void)
{
    /* Set TWIE bit to enable TWI interrupts */
    TWCR |= (1 << TWIE);
}

void TWI_vWriteBitRateRegister(u8 u8BitRateByte)
{
    TWBR = u8BitRateByte;
}

void TWI_vWritePrescaler(u8 u8Prescaler)
{
    if (u8Prescaler <= 3)
    {
        TWSR = (TWSR & 0xFC) | u8Prescaler;
    }
    else
    {
        TWSR = (TWSR & 0xFC) | 3;
    }
}

void TWI_vStartTransmission(void)
{
    TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN);
}

bool TWI_bSendData(u8 u8Address, u8 *pData, u8 u8Size) {
    if (TWI_boolIsBusy) return false;

    /* Flag and var setup */
    TWI_u8SlaveAddress = u8Address;
    TWI_boolDirection  = TWI_WRITE; /* WRITE MODE */
    TWI_pDataBuffer    = pData;
    TWI_u8DataSize     = u8Size;
    TWI_u8DataCounter  = 0;
    TWI_boolIsBusy     = true;

    /* Send start bit to start transmitting */
    TWI_vStartTransmission();
    return true;
}

bool TWI_bReadData(u8 u8Address, u8 *pData, u8 u8Size) {
    if (TWI_boolIsBusy) return false;

    /* Flag and var setup */
    TWI_u8SlaveAddress = u8Address;
    TWI_boolDirection  = TWI_READ; /* READ MODE */
    TWI_pDataBuffer    = pData;
    TWI_u8DataSize     = u8Size;
    TWI_u8DataCounter  = 0;
    TWI_boolIsBusy     = true;

    /* Send start bit to start transmitting */
    TWI_vStartTransmission();
    return true;
}

void TWI_vEndTransmission(void)
{
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
}

void TWI_vSendSlaveCall(u8 u8Address, bool boolDirectionBit)
{
    TWDR = (u8Address << 1) | boolDirectionBit;
    TWCR = (1 << TWINT) | (1 << TWEN);
}

void TWI_vSendDataByte(u8 u8DataByte)
{
    TWDR = u8DataByte;
    TWCR = (1 << TWINT) | (1 << TWEN);
}

u8 TWI_u8ReadDataByte(void)
{
    return TWDR;
}

bool TWI_bIsBusy(void) {
    return TWI_boolIsBusy;
}

void TWI_vIntHandler(void)
{
    u8 status = TWSR & 0xF8;

    switch (status)
    {
    /* ************************************************** */
    /*             MASTER TRANSMITTER STATES              */
    /* ************************************************** */
    case 0x08: /* START TRANSMITTED */
    case 0x10: /* REPEATED START TRANSMITTED */
        TWI_vSendSlaveCall(TWI_u8SlaveAddress, TWI_boolDirection);
        break;

    case 0x18: /* SLA+W TRANSMITTED, ACK RECEIVED */
    case 0x28: /* DATA BYTE TRANSMITTED, ACK RECEIVED */
        if (TWI_u8DataCounter < TWI_u8DataSize)
        {
            TWI_vSendDataByte(TWI_pDataBuffer[TWI_u8DataCounter]);
            TWI_u8DataCounter++;
        }
        else
        {
            TWI_vEndTransmission();
            TWI_boolIsBusy = false;
        }
        break;

    /* ************************************************** */
    /*             MASTER RECEIVER STATES                 */
    /* ************************************************** */
    case 0x40: /* SLA+R TRANSMITTED, ACK RECEIVED */
        if (TWI_u8DataSize == 1)
        {
            TWI_vAcknowledgeOwnAddress(TWI_ACKNOWLEDGE_DISABLE);
        }
        else
        {
            TWI_vAcknowledgeOwnAddress(TWI_ACKNOWLEDGE_ENABLE);
        }
        TWCR = (1 << TWINT) | (1 << TWEN) | (TWCR & (1 << TWEA));
        break;

    case 0x50: /* DATA BYTE RECEIVED, ACK RETURNED */
        TWI_pDataBuffer[TWI_u8DataCounter] = TWI_u8ReadDataByte();
        TWI_u8DataCounter++;

        if (TWI_u8DataCounter == TWI_u8DataSize)
        {
            TWI_vEndTransmission();
            TWI_boolIsBusy = false;
        }
        else
        {
            if (TWI_u8DataCounter == TWI_u8DataSize - 1)
            {
                TWI_vAcknowledgeOwnAddress(TWI_ACKNOWLEDGE_DISABLE);
            }
            TWCR = (1 << TWINT) | (1 << TWEN) | (TWCR & (1 << TWEA));
        }

        /* ************************************************** */
        /*             ERROR / ARBITRATION STATES             */
        /* ************************************************** */
        case 0x38: // Arbitration lost
        case 0x48: // SLA+R/W transmitted, NACK received (Slave not responding)
        case 0x20: // SLA+W transmitted, NACK received
        case 0x30: // Data byte transmitted, NACK received
            // An error occurred. Abort and send STOP condition.
            TWI_vEndTransmission();
            TWI_boolIsBusy = false;
            break;

        default:
            // Unknown state. Just clear the flag to prevent hanging.
            TWCR = (1 << TWINT) | (1 << TWEN);
            break;
    }
}