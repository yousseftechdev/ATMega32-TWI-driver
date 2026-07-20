/*********************************************************************************/
/* File:        TWI_interface.h                                                  */
/* Author:      Youssef Mostafa                                                  */
/* Description: Two-write interface (TWI) MCAL Driver Public API                 */
/*********************************************************************************/

#ifndef TWI_INTERFACE_H_
#define TWI_INTERFACE_H_

/* Standard Data Type Definitions (Matching System Architecture) */
typedef unsigned char       u8;
typedef unsigned short int  u16;
typedef unsigned long int   u32;

/* Clock Prescaler Configurations (Hardware Divider for the TWI Bit Rate Generator) */
#define TWI_PRESCALER_1        0b00   /* Divide system clock by 1   */
#define TWI_PRESCALER_4        0b01   /* Divide system clock by 4   */
#define TWI_PRESCALER_16       0b10   /* Divide system clock by 16  */
#define TWI_PRESCALER_64       0b11   /* Divide system clock by 64  */

/* ============================================================================= */
/* FUNCTION PROTOTYPES                                                           */
/* ============================================================================= */

void TWI_vInit();

#endif /* TWI_INTERFACE_H_ */