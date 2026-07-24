#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "TWI_interface.h"

/* SSD1306 I2C Address 
 * 0x3C if the D/C# (SA0) pin is pulled LOW (Most common)
 * 0x3D if the D/C# (SA0) pin is pulled HIGH */
#define SSD1306_ADDR    0x3C

/**
 * @brief Sends a single command byte to the SSD1306.
 * @param cmd The command to send.
 */
void SSD1306_WriteCommand(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd}; // 0x00 is the Control Byte for Commands
    while(TWI_bIsBusy());            // Wait for previous transaction to finish
    TWI_bSendData(SSD1306_ADDR, buffer, 2); // Kick off transmission
    while(TWI_bIsBusy());            // Wait for ISR to complete this transaction
}

/**
 * @brief Initializes the SSD1306 for a 128x64 display.
 */
void SSD1306_Init(void) {
    SSD1306_WriteCommand(0xAE); // Display OFF
    
    SSD1306_WriteCommand(0xD5); // Set Display Clock Divide Ratio
    SSD1306_WriteCommand(0x80); // Default ratio
    
    SSD1306_WriteCommand(0xA8); // Set Multiplex Ratio
    SSD1306_WriteCommand(0x3F); // 1/64 duty (for 64px height)
    
    SSD1306_WriteCommand(0xD3); // Set Display Offset
    SSD1306_WriteCommand(0x00); // No offset
    
    SSD1306_WriteCommand(0x40); // Set Display Start Line to 0
    
    SSD1306_WriteCommand(0x8D); // Charge Pump Setting
    SSD1306_WriteCommand(0x14); // Enable Charge Pump (CRITICAL for brightness)
    
    SSD1306_WriteCommand(0x20); // Set Memory Addressing Mode
    SSD1306_WriteCommand(0x00); // 0x00 = Horizontal Addressing Mode
    
    SSD1306_WriteCommand(0xA1); // Segment Re-map (Column 127 mapped to SEG0)
    SSD1306_WriteCommand(0xC8); // COM Output Scan Direction (Remapped)
    // Note: If your screen is upside down or mirrored, toggle A1<->A0 and C8<->C0
    
    SSD1306_WriteCommand(0xDA); // Set COM Pins Hardware Configuration
    SSD1306_WriteCommand(0x12); // Alternative COM pin config (for 128x64)
    
    SSD1306_WriteCommand(0x81); // Set Contrast Control
    SSD1306_WriteCommand(0xCF); // Medium-High contrast
    
    SSD1306_WriteCommand(0xD9); // Set Pre-charge Period
    SSD1306_WriteCommand(0xF1); 
    
    SSD1306_WriteCommand(0xDB); // Set VCOMH Deselect Level
    SSD1306_WriteCommand(0x40); 
    
    SSD1306_WriteCommand(0xA4); // Entire Display ON (Resume to RAM content)
    SSD1306_WriteCommand(0xA6); // Normal Display (Not Inverse)
    
    SSD1306_WriteCommand(0xAF); // Display ON
}

/**
 * @brief Fills the entire screen with ON pixels.
 */
void SSD1306_FillScreen(void) {
    // Buffer size: 1 byte Control Byte + 128 bytes of Data (1 full page width)
    uint8_t buffer[129]; 
    
    buffer[0] = 0x40; // Control Byte for Data
    for(uint8_t i = 1; i < 129; i++) {
        buffer[i] = 0xFF; // All 8 vertical pixels in this column are ON
    }
    
    // Set Column Address bounds (0 to 127)
    SSD1306_WriteCommand(0x21); 
    SSD1306_WriteCommand(0x00); // Start column
    SSD1306_WriteCommand(0x7F); // End column
    
    // Set Page Address bounds (0 to 7)
    SSD1306_WriteCommand(0x22); 
    SSD1306_WriteCommand(0x00); // Start page
    SSD1306_WriteCommand(0x07); // End page
    
    // Send 8 pages of data (128 bytes per page = 1024 bytes total)
    for(uint8_t page = 0; page < 8; page++) {
        while(TWI_bIsBusy());
        TWI_bSendData(SSD1306_ADDR, buffer, 129);
        while(TWI_bIsBusy());
    }
}

int main(void) {
    // 1. Initialize TWI at 400kHz (Fast Mode) with Interrupts Enabled
    TWI_vInit(400000, TWI_ENABLE_INTERRUPT);
    
    // 2. Enable Global Interrupts (Required for your TWI ISR to fire)
    sei();
    
    // 3. Initialize the OLED Controller
    SSD1306_Init();
    
    // 4. Turn all pixels ON
    SSD1306_FillScreen();
    
    // Infinite loop
    while(1) {
        // The screen will stay fully lit. 
        // You can add delays or other logic here.
    }
    
    return 0;
}