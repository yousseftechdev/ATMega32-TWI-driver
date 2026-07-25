/*********************************************************************************/
/* File:        TWI_program.c                                                    */
/* Author:      Youssef Mostafa                                                  */
/* Description: Low-Level Hardware Manipulation Source Code for ATmega32 TWI.    */
/*********************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include "TWI_interface.h"

/* Private State Variables for the ISR */
static volatile bool TWI_boolTransactionSuccess = false; /* true = tranaction successful, false = failed */
static volatile bool TWI_boolInterruptEnable = false;    /* true = Enable Interrupts, false = Disable Interrups */
static volatile u8 *TWI_pDataBuffer;                     /* Pointer to the user's data array */
static volatile u8 TWI_u8DataSize;                       /* Total bytes to send/receive */
static volatile u8 TWI_u8DataCounter;                    /* Current byte index */
static volatile u8 TWI_u8SlaveAddress;                   /* Target slave address */
static volatile bool TWI_boolDirection;                  /* true = Read, false = Write */
static volatile bool TWI_boolIsBusy;                     /* Flag to prevent overlapping transactions */

/* Set interrupt handler */
ISR(TWI_vect)
{
    TWI_vIntHandler();
}

void TWI_vInit(u32 u32Freq, bool boolInterruptEnable)
{
    /* Set frequency */
    if (u32Freq != TWI_NO_AUTO_FREQUENCY)
    {
        TWI_vSetFrequency(u32Freq);
    }

    /* Enable TWI peripheral */
    TWCR = (1 << TWEN);

    /* Check if user wants to enable interrupts */
    if (boolInterruptEnable)
    {
        TWI_vEnableInterrupt();
    }
}

void TWI_vSetFrequency(u32 u32Freq)
{
    u32 u32BestError = UINT32_MAX;
    u8 u8BestBr = 0;
    u8 u8BestPs = 0;
    u8 u8PsValue = 0;
    u32 u32Denominator = 0;
    u32 u32CalculatedFrequency = 0;
    u32 u32Error = 0;

    for (u8 u8Ps = 0; u8Ps <= 3; u8Ps++)
    {
        u8PsValue = 1UL << (2 * u8Ps); /* <-- Equals 4^TWPS */

        for (u8 u8Br = 0; u8Br <= 255; u8Br++)
        {
            u32Denominator = 16UL + (2UL * u8Br * u8PsValue);

            if (u32Denominator == 0)
                continue;

            u32CalculatedFrequency = F_CPU / u32Denominator;

            u32Error = (u32CalculatedFrequency > u32Freq) ? (u32CalculatedFrequency - u32Freq) : (u32Freq - u32CalculatedFrequency);

            if (u32Error < u32BestError)
            {
                u32BestError = u32Error;
                u8BestBr = u8Br;
                u8BestPs = u8Ps;

                /* If exact match stop looking */
                if (u32Error == 0)
                    break;
            }
        }

        if (u32BestError == 0)
            break;
    }

    TWI_vWriteBitRateRegister(u8BestBr);
    TWI_vWritePrescaler(u8BestPs);
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
    TWI_boolInterruptEnable = true;
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
    TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN) | (TWI_boolInterruptEnable ? (1 << TWIE) : 0);
}

bool TWI_bSendData(u8 u8Address, u8 *pData, u8 u8Size)
{
    if (TWI_boolIsBusy)
        return false;

    /* Flag and var setup */
    TWI_u8SlaveAddress = u8Address;
    TWI_boolDirection = TWI_WRITE; /* WRITE MODE */
    TWI_pDataBuffer = pData;
    TWI_u8DataSize = u8Size;
    TWI_u8DataCounter = 0;
    TWI_boolIsBusy = true;

    /* Send start bit to start transmitting */
    TWI_vStartTransmission();
    return true;
}

bool TWI_bReadData(u8 u8Address, u8 *pData, u8 u8Size)
{
    if (TWI_boolIsBusy)
        return false;

    /* Flag and var setup */
    TWI_u8SlaveAddress = u8Address;
    TWI_boolDirection = TWI_READ; /* READ MODE */
    TWI_pDataBuffer = pData;
    TWI_u8DataSize = u8Size;
    TWI_u8DataCounter = 0;
    TWI_boolIsBusy = true;

    /* Send start bit to start transmitting */
    TWI_vStartTransmission();
    return true;
}

void TWI_vEndTransmission(void)
{
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN) | (TWI_boolInterruptEnable ? (1 << TWIE) : 0);
}

void TWI_vSendSlaveCall(u8 u8Address, bool boolDirectionBit)
{
    TWDR = (u8Address << 1) | boolDirectionBit;
    TWCR = (1 << TWINT) | (1 << TWEN) | (TWI_boolInterruptEnable ? (1 << TWIE) : 0);
}

void TWI_vSendDataByte(u8 u8DataByte)
{
    TWDR = u8DataByte;
    TWCR = (1 << TWINT) | (1 << TWEN) | (TWI_boolInterruptEnable ? (1 << TWIE) : 0);
}

u8 TWI_u8ReadDataByte(void)
{
    return TWDR;
}

bool TWI_bIsBusy(void)
{
    return TWI_boolIsBusy;
}

bool TWI_boolGetTransactionStatus(void)
{
    return TWI_boolTransactionSuccess;
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
        TWI_boolTransactionSuccess = true;
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
        TWI_boolTransactionSuccess = true;
        if (TWI_u8DataSize == 1)
        {
            TWI_vAcknowledgeOwnAddress(TWI_ACKNOWLEDGE_DISABLE);
        }
        else
        {
            TWI_vAcknowledgeOwnAddress(TWI_ACKNOWLEDGE_ENABLE);
        }
        TWCR = (TWI_boolInterruptEnable ? (1 << TWIE) : 0) | (1 << TWINT) | (1 << TWEN) | (TWCR & (1 << TWEA));
        break;

    case 0x50: /* DATA BYTE RECEIVED, ACK RETURNED */
        TWI_boolTransactionSuccess = true;
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
            TWCR = (TWI_boolInterruptEnable ? (1 << TWIE) : 0) | (1 << TWINT) | (1 << TWEN) | (TWCR & (1 << TWEA));
        }
        break;

    /* ************************************************** */
    /*             ERROR / ARBITRATION STATES             */
    /* ************************************************** */
    case 0x38: // Arbitration lost
    case 0x48: // SLA+R/W transmitted, NACK received (Slave not responding)
    case 0x20: // SLA+W transmitted, NACK received
    case 0x30: // Data byte transmitted, NACK received
        // An error occurred. Abort and send STOP condition.
        TWI_boolTransactionSuccess = false;
        TWI_vEndTransmission();
        TWI_boolIsBusy = false;
        break;

    default:
        // Unknown state. Just clear the flag to prevent hanging.
        TWCR = (1 << TWINT) | (1 << TWEN) | (TWI_boolInterruptEnable ? (1 << TWIE) : 0);
        break;
    }
}