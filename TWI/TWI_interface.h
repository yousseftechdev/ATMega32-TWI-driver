/*********************************************************************************/
/* File:        TWI_interface.h                                                  */
/* Author:      Youssef Mostafa                                                  */
/* Description: Two-write interface (TWI) MCAL Driver Public API                 */
/*********************************************************************************/

#ifndef TWI_INTERFACE_H_
#define TWI_INTERFACE_H_

/* Include stdbool to use bool types */
#include <stdbool.h>

/* Standard Data Type Definitions (Matching System Architecture) */
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long int u32;

/* Clock Prescaler Configurations (Hardware Divider for the TWI Bit Rate Generator) */
#define TWI_PRESCALER_1 0  /* Divide system clock by 1   */
#define TWI_PRESCALER_4 1  /* Divide system clock by 4   */
#define TWI_PRESCALER_16 2 /* Divide system clock by 16  */
#define TWI_PRESCALER_64 3 /* Divide system clock by 64  */

/* Enable/Disable Interrupt on TWINT flag set */
#define TWI_ENABLE_INTERRUPT true
#define TWI_DISABLE_INTERRUPT false

/* Read/Write bits */
#define TWI_READ true
#define TWI_WRITE false

/* Auto stop */
#define TWI_AUTO_STOP true
#define TWI_MANUAL_STOP false

/* Acknowledge enable bit */
#define TWI_ACKNOWLEDGE_ENABLE 1
#define TWI_ACKNOWLEDGE_DISABLE 0

/* Disable auto frequency calculation alogrithm flag, pass as first argument of TWI_vInit()
   to disable auto Bit Rate Generator byte and prescaler calculation algorithm */
#define TWI_NO_AUTO_FREQUENCY 0

/* ============================================================================= */
/* FUNCTION PROTOTYPES                                                           */
/* ============================================================================= */

/**
 * @brief                     Powers on the TWI module, sets SCL frequency, enables TWI interrupt.
 * @param u32Freq             Desired SCL frequency
 * @param boolInterruptEnable Whether to enable interrupts on TWINT flag set
 */
void TWI_vInit(u32 u32Freq, bool boolInterruptEnable);

/**
 * @brief         Sets frequency, runs in initialization.
 * @param u32Freq Desired SCL frequency
 * @note          Don't run this on its own, it's a helper function for in-driver logic.
 */
void TWI_vSetFrequency(u32 u32Freq);

/**
* @brief Enables TWI event based interrupts.
* @note  Runs automatically in initialization function
         when true is passed in boolInterruptEnable, no need to run manually.
*/
void TWI_vEnableInterrupt();

/**
* @brief                Enables/disables auto-stop feature.
* @param bAutoStopState Input TWI_AUTO_STOP to enable, TWI_MANUAL_STOP to disable
* @note                 If auto-stop disabled the developer has to call EndTransmission() manually
*/
void TWI_vSetAutoStop(bool bAutoStopState);

/**
 * @brief               Sets the bit rate generator bits in TWBR.
 * @param u8BitRateByte The 8 bits to be written to the TWBR register
 */
void TWI_vWriteBitRateRegister(u8 u8BitRateByte);

/**
 * @brief             Writes the prescaler bits in TWSR.
 * @param u8Prescaler The prescaler value
 */
void TWI_vWritePrescaler(u8 u8Prescaler);

/**
 * @brief                 Writes the ATMega's own slave address to TWAR.
 * @param u8Address       The desired address
 * @param boolGeneralCall General call enable
 */
void TWI_vSetOwnSlaveAddress(u8 u8Address, bool boolGeneralCall);

/**
 * @brief                       Allows the ATMega to to toggle whether to
 *                              send acknowledge bit when own address is called.
 * @param boolAcknowledgeEnable Decide whether to send acknowledge pulses when address is detected
 */
void TWI_vAcknowledgeOwnAddress(bool boolAcknowledgeEnable);

/**
 * @brief Sends a start bit.
 */
void TWI_vStartTransmission(void);

/**
 * @brief Sends a stop bit.
 */
void TWI_vEndTransmission(void);

/**
 * @brief               Sends SLA+R/W (Slave address and write bit) call.
 * @param u8Address     7-bit address for desired slave
 * @param boolDirection Bit which decides whether the master wants to read or write to the slave
 */
void TWI_vSendSlaveCall(u8 u8Address, bool boolDirectionBit);

/**
 * @brief            Writes byte to TWDR then sends it by setting TWINT.
 * @param u8DataByte Byte to be written and sent
 */
void TWI_vSendDataByte(u8 u8DataByte);

/**
 * @brief  Reads recieved byte from TWDR.
 * @return Recieved byte from TWDR
 */
u8 TWI_u8ReadDataByte(void);

/**
 * @brief           Actually kicks off the transmission and sets all the necessary flags at the beginning and puts the slave in wriite mode.
 * @param u8Address Target slave address
 * @param pData     Array pointer for the bytes to be transmitted
 * @param u8Size    Number of bytes to send
 * @return true if busy, false if idle
 */
bool TWI_bSendData(u8 u8Address, u8 *pData, u8 u8Size);

/**
 * @brief           Wrapper for TWI_bSendData, is able to send one byte without the need to pass it as an array or define the size.
 * @param u8Address Target slave address
 * @param u8Data    Byte to be sent
 * @return true if busy, false if idle
 */
bool TWI_bSendDataShot(u8 u8Address, u8 u8Data);

/**
 * @brief           Actually kicks off the transmission and sets all the necessary flags at the beginning and puts the slave in read mode.
 * @param u8Address Target slave address
 * @param pData     Array pointer for the bytes to be transmitted
 * @param u8Size    Number of bytes to send
 * @return true if busy, false if idle
 */
bool TWI_bReadData(u8 u8Address, u8 *pData, u8 u8Size);

/**
 * @brief Checks if the I2C bus is currently processing a transaction.
 * @return true if busy, false if idle
 */
bool TWI_bIsBusy(void);

/**
 * @brief Returns the status code of the last completed transaction.
 * @return TWSR status code (e.g., 0x18 for ACK, 0x20 for NACK)
 */
bool TWI_boolGetTransactionStatus(void);

/**
 * @brief Runs on TWINT ISR trigger, handles all TWI events, checks TWSR status register to decide next action.
 * @note  You can run this function manually if interrupts are disabled.
 */
void TWI_vIntHandler(void);

#endif /* TWI_INTERFACE_H_ */