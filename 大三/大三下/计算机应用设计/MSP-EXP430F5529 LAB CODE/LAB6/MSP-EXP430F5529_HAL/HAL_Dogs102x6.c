/*******************************************************************************
 *
 *  HAL_Dogs102x6.c - Driver for the DOGS 102x6 display
 *
 *  Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @file       HAL_Dogs102x6.c
 * @addtogroup HAL_Dogs102x6
 * @{
 ******************************************************************************/
#include "msp430.h"
#include "HAL_Buttons.h"
#include "HAL_Dogs102x6.h"

// Macros
#ifndef abs
#    define abs(n)     (((n) < 0) ? -(n) : (n))
#endif

// For all commands, CD signal must = 0
#define SET_COLUMN_ADDRESS_MSB        0x10  //Set SRAM col. addr. before write, last 4 bits =
                                            // ca4-ca7
#define SET_COLUMN_ADDRESS_LSB        0x00  //Set SRAM col. addr. before write, last 4 bits =
                                            // ca0-ca3
#define SET_POWER_CONTROL             0x2F  //Set Power control - booster, regulator, and follower
                                            // on
#define SET_SCROLL_LINE               0x40  //Scroll image up by SL rows (SL = last 5 bits),
                                            // range:0-63
#define SET_PAGE_ADDRESS              0xB0  //Set SRAM page addr (pa = last 4 bits), range:0-8
#define SET_VLCD_RESISTOR_RATIO       0x27  //Set internal resistor ratio Rb/Ra to adjust contrast
#define SET_ELECTRONIC_VOLUME_MSB     0x81  //Set Electronic Volume "PM" to adjust contrast
#define SET_ELECTRONIC_VOLUME_LSB     0x0F  //Set Electronic Volume "PM" to adjust contrast (PM =
                                            // last 5 bits)
#define SET_ALL_PIXEL_ON              0xA4  //Disable all pixel on (last bit 1 to turn on all pixels
                                            // - does not affect memory)
#define SET_INVERSE_DISPLAY           0xA6  //Inverse display off (last bit 1 to invert display -
                                            // does not affect memory)
#define SET_DISPLAY_ENABLE            0xAF  //Enable display (exit sleep mode & restore power)
#define SET_SEG_DIRECTION             0xA1  //Mirror SEG (column) mapping (set bit0 to mirror
                                            // display)
#define SET_COM_DIRECTION             0xC8  //Mirror COM (row) mapping (set bit3 to mirror display)
#define SYSTEM_RESET                  0xE2  //Reset the system. Control regs reset, memory not
                                            // affected
#define NOP                           0xE3  //No operation
#define SET_LCD_BIAS_RATIO            0xA2  //Set voltage bias ratio (BR = bit0)
#define SET_CURSOR_UPDATE_MODE        0xE0  //Column address will increment with write operation
                                            // (but no wrap around)
#define RESET_CURSOR_UPDATE_MODE      0xEE  //Return cursor to column address from before cursor
                                            // update mode was set
#define SET_ADV_PROGRAM_CONTROL0_MSB  0xFA  //Set temp. compensation curve to -0.11%/C
#define SET_ADV_PROGRAM_CONTROL0_LSB  0x90

// Pins from MSP430 connected to LCD
#define CD              BIT6
#define CS              BIT4
#define RST             BIT7
#define BACKLT          BIT6
#define SPI_SIMO        BIT1
#define SPI_CLK         BIT3

// Ports
#define CD_RST_DIR      P5DIR
#define CD_RST_OUT      P5OUT
#define CS_BACKLT_DIR   P7DIR
#define CS_BACKLT_OUT   P7OUT
#define CS_BACKLT_SEL   P7SEL
#define SPI_SEL         P4SEL
#define SPI_DIR         P4DIR

// Font lookup table
static const uint8_t FONT6x8[] = {
    /* 6x8 font, each line is a character each byte is a one pixel wide column
     * of that character. MSB is the top pixel of the column, LSB is the bottom
     * pixel of the column. 0 = pixel off. 1 = pixel on. */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // space
    0x00, 0x00, 0xFA, 0x00, 0x00, 0x00, // !
    0x00, 0xE0, 0x00, 0xE0, 0x00, 0x00, // "
    0x28, 0xFE, 0x28, 0xFE, 0x28, 0x00, // #
    0x24, 0x54, 0xFE, 0x54, 0x48, 0x00, // $
    0xC4, 0xC8, 0x10, 0x26, 0x46, 0x00, // %
    0x6C, 0x92, 0x6A, 0x04, 0x0A, 0x00, // &
    0x00, 0x10, 0xE0, 0xC0, 0x00, 0x00, // '
    0x00, 0x38, 0x44, 0x82, 0x00, 0x00, // (
    0x00, 0x82, 0x44, 0x38, 0x00, 0x00, // )
    0x54, 0x38, 0xFE, 0x38, 0x54, 0x00, // *
    0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, // +
    0x00, 0x02, 0x1C, 0x18, 0x00, 0x00, // ,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x00, // -
    0x00, 0x00, 0x06, 0x06, 0x00, 0x00, // .
    0x04, 0x08, 0x10, 0x20, 0x40, 0x00, // /
    //96 Bytes
    0x7C, 0x8A, 0x92, 0xA2, 0x7C, 0x00, // 0
    0x00, 0x42, 0xFE, 0x02, 0x00, 0x00, // 1
    0x42, 0x86, 0x8A, 0x92, 0x62, 0x00, // 2
    0x84, 0x82, 0x92, 0xB2, 0xCC, 0x00, // 3
    0x18, 0x28, 0x48, 0xFE, 0x08, 0x00, // 4
    0xE4, 0xA2, 0xA2, 0xA2, 0x9C, 0x00, // 5
    0x3C, 0x52, 0x92, 0x92, 0x0C, 0x00, // 6
    0x82, 0x84, 0x88, 0x90, 0xE0, 0x00, // 7
    0x6C, 0x92, 0x92, 0x92, 0x6C, 0x00, // 8
    0x60, 0x92, 0x92, 0x94, 0x78, 0x00, // 9
    0x00, 0x00, 0x28, 0x00, 0x00, 0x00, // :
    0x00, 0x00, 0x02, 0x2C, 0x00, 0x00, // ;
    0x00, 0x10, 0x28, 0x44, 0x82, 0x00, // <
    0x28, 0x28, 0x28, 0x28, 0x28, 0x00, // =
    0x00, 0x82, 0x44, 0x28, 0x10, 0x00, // >
    0x40, 0x80, 0x8A, 0x90, 0x60, 0x00, // ?
    //96*2 = 192 Bytes
    0x7C, 0x82, 0xBA, 0x9A, 0x72, 0x00, // @
    0x3E, 0x48, 0x88, 0x48, 0x3E, 0x00, // A
    0xFE, 0x92, 0x92, 0x92, 0x6C, 0x00, // B
    0x7C, 0x82, 0x82, 0x82, 0x44, 0x00, // C
    0xFE, 0x82, 0x82, 0x82, 0x7C, 0x00, // D
    0xFE, 0x92, 0x92, 0x92, 0x82, 0x00, // E
    0xFE, 0x90, 0x90, 0x90, 0x80, 0x00, // F
    0x7C, 0x82, 0x92, 0x92, 0x5E, 0x00, // G
    0xFE, 0x10, 0x10, 0x10, 0xFE, 0x00, // H
    0x00, 0x82, 0xFE, 0x82, 0x00, 0x00, // I
    0x04, 0x02, 0x82, 0xFC, 0x80, 0x00, // J
    0xFE, 0x10, 0x28, 0x44, 0x82, 0x00, // K
    0xFE, 0x02, 0x02, 0x02, 0x02, 0x00, // L
    0xFE, 0x40, 0x38, 0x40, 0xFE, 0x00, // M
    0xFE, 0x20, 0x10, 0x08, 0xFE, 0x00, // N
    0x7C, 0x82, 0x82, 0x82, 0x7C, 0x00, // O
    //96*3 = 288 Bytes
    0xFE, 0x90, 0x90, 0x90, 0x60, 0x00, // P
    0x7C, 0x82, 0x8A, 0x84, 0x7A, 0x00, // Q
    0xFE, 0x90, 0x98, 0x94, 0x62, 0x00, // R
    0x64, 0x92, 0x92, 0x92, 0x4C, 0x00, // S
    0x80, 0x80, 0xFE, 0x80, 0x80, 0x00, // T
    0xFC, 0x02, 0x02, 0x02, 0xFC, 0x00, // U
    0xF8, 0x04, 0x02, 0x04, 0xF8, 0x00, // V
    0xFC, 0x02, 0x1C, 0x02, 0xFC, 0x00, // W
    0xC6, 0x28, 0x10, 0x28, 0xC6, 0x00, // X
    0xC0, 0x20, 0x1E, 0x20, 0xC0, 0x00, // Y
    0x86, 0x8A, 0x92, 0xA2, 0xC2, 0x00, // Z
    0x00, 0xFE, 0x82, 0x82, 0x82, 0x00, // [
    0x40, 0x20, 0x10, 0x08, 0x04, 0x00, // '\'
    0x00, 0x82, 0x82, 0x82, 0xFE, 0x00, // ]
    0x20, 0x40, 0x80, 0x40, 0x20, 0x00, // ^
    0x01, 0x01, 0x01, 0x01, 0x01, 0x00, // _
    //96*4 = 384 Bytes
    0x00, 0xC0, 0xE0, 0x10, 0x00, 0x00, // `
    0x04, 0x2A, 0x2A, 0x2A, 0x1E, 0x00, // a
    0xFE, 0x14, 0x22, 0x22, 0x1C, 0x00, // b
    0x1C, 0x22, 0x22, 0x22, 0x14, 0x00, // c
    0x1C, 0x22, 0x22, 0x14, 0xFE, 0x00, // d
    0x1C, 0x2A, 0x2A, 0x2A, 0x18, 0x00, // e
    0x00, 0x10, 0x7E, 0x90, 0x40, 0x00, // f
    0x18, 0x25, 0x25, 0x25, 0x3E, 0x00, // g
    0xFE, 0x10, 0x20, 0x20, 0x1E, 0x00, // h
    0x00, 0x22, 0xBE, 0x02, 0x00, 0x00, // i
    0x00, 0x04, 0x02, 0x02, 0xBC, 0x00, // j
    0x00, 0xFE, 0x08, 0x14, 0x22, 0x00, // k
    0x00, 0x82, 0xFE, 0x02, 0x00, 0x00, // l
    0x3E, 0x20, 0x1E, 0x20, 0x1E, 0x00, // m
    0x3E, 0x10, 0x20, 0x20, 0x1E, 0x00, // n
    0x1C, 0x22, 0x22, 0x22, 0x1C, 0x00, // o
    //96*5 = 480 Bytes
    0x3F, 0x18, 0x24, 0x24, 0x18, 0x00, // p
    0x18, 0x24, 0x24, 0x18, 0x3F, 0x00, // q
    0x3E, 0x10, 0x20, 0x20, 0x10, 0x00, // r
    0x12, 0x2A, 0x2A, 0x2A, 0x24, 0x00, // s
    0x20, 0x20, 0xFC, 0x22, 0x24, 0x00, // t
    0x3C, 0x02, 0x02, 0x04, 0x3E, 0x00, // u
    0x38, 0x04, 0x02, 0x04, 0x38, 0x00, // v
    0x3C, 0x02, 0x0C, 0x02, 0x3C, 0x00, // w
    0x22, 0x14, 0x08, 0x14, 0x22, 0x00, // x
    0x32, 0x09, 0x09, 0x09, 0x3E, 0x00, // y
    0x22, 0x26, 0x2A, 0x32, 0x22, 0x00, // z
    0x00, 0x10, 0x6C, 0x82, 0x00, 0x00, // {
    0x00, 0x00, 0xEE, 0x00, 0x00, 0x00, // |
    0x00, 0x82, 0x6C, 0x10, 0x00, 0x00, // }
    0x40, 0x80, 0x40, 0x20, 0x40, 0x00, // ~
    0x00, 0x60, 0x90, 0x90, 0x60, 0x00, // degrees symbol
    //96*6 = 576 Bytes
    //Special Characters ** NON-ASCII **
    0x00, 0x00, 0xFE, 0x82, 0x82, 0x82, // [
    0x82, 0x82, 0x82, 0xFE, 0x00, 0x00  //  ]
};

// Variables

// Store a copy of the lcd memory (8x102) = 816 bytes
// Since we cannot read from the lcd memory, this is a way to keep track of
// what is stored there Two additional byes are used for driver-
// internal purposes
uint8_t dogs102x6Memory[816 + 2];

uint8_t currentPage = 0, currentColumn = 0;

uint8_t backlight  = 8;
uint8_t contrast = 0x0F;

// Dog102-6 Initialization Commands
uint8_t Dogs102x6_initMacro[] = {
    SET_SCROLL_LINE,
    SET_SEG_DIRECTION,
    SET_COM_DIRECTION,
    SET_ALL_PIXEL_ON,
    SET_INVERSE_DISPLAY,
    SET_LCD_BIAS_RATIO,
    SET_POWER_CONTROL,
    SET_VLCD_RESISTOR_RATIO,
    SET_ELECTRONIC_VOLUME_MSB,
    SET_ELECTRONIC_VOLUME_LSB,
    SET_ADV_PROGRAM_CONTROL0_MSB,
    SET_ADV_PROGRAM_CONTROL0_LSB,
    SET_DISPLAY_ENABLE,
    SET_PAGE_ADDRESS,
    SET_COLUMN_ADDRESS_MSB,
    SET_COLUMN_ADDRESS_LSB
};

/***************************************************************************//**
 * @brief   Initialize LCD
 * @param   None
 * @return  None
 ******************************************************************************/

void Dogs102x6_init(void)
{
    // Port initialization for LCD operation
    CD_RST_DIR |= RST;
    // Reset is active low
    CD_RST_OUT &= RST;
    // Reset is active low
    CD_RST_OUT |= RST;
    // Chip select for LCD
    CS_BACKLT_DIR |= CS;
    // CS is active low
    CS_BACKLT_OUT &= ~CS;
    // Command/Data for LCD
    CD_RST_DIR |= CD;
    // CD Low for command
    CD_RST_OUT &= ~CD;

    // P4.1 option select SIMO
    SPI_SEL |= SPI_SIMO;
    SPI_DIR |= SPI_SIMO;
    // P4.3 option select CLK
    SPI_SEL |= SPI_CLK;
    SPI_DIR |= SPI_CLK;

    // Initialize USCI_B1 for SPI Master operation
    // Put state machine in reset
    UCB1CTL1 |= UCSWRST;
    //3-pin, 8-bit SPI master
    UCB1CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC;
    // Clock phase - data captured first edge, change second edge
    // MSB
    // Use SMCLK, keep RESET
    UCB1CTL1 = UCSSEL_2 + UCSWRST;
    UCB1BR0 = 0x02;
    UCB1BR1 = 0;
    // Release USCI state machine
    UCB1CTL1 &= ~UCSWRST;
    UCB1IFG &= ~UCRXIFG;

    Dogs102x6_writeCommand(Dogs102x6_initMacro, 13);

    // Deselect chip
    CS_BACKLT_OUT |= CS;

    dogs102x6Memory[0] = 102;
    dogs102x6Memory[1] = 8;
}

/***************************************************************************//**
 * @brief   Initialize Backlight
 * @param   None
 * @return  None
 ******************************************************************************/

void Dogs102x6_backlightInit(void)
{
    // Turn on Backlight
    CS_BACKLT_DIR |= BACKLT;
    CS_BACKLT_OUT |= BACKLT;
    // Uses PWM to control brightness
    CS_BACKLT_SEL |= BACKLT;

    // start at full brightness (8)
    TB0CCTL4 = OUTMOD_7;
    TB0CCR4 = TB0CCR0 >> 1;

    TB0CCR0 = 50;
    TB0CTL = TBSSEL_1 + MC_1;
}

/***************************************************************************//**
 * @brief  Set function for the backlight PWM's duty cycle
 *
 * @param  BackLightLevel The target backlight duty cycle - valued 0~11.
 *
 * @return none
 ******************************************************************************/

void Dogs102x6_setBacklight(uint8_t brightness)
{
    unsigned int dutyCycle = 0, i, dummy;

    if (brightness > 0)
    {
        TB0CCTL4 = OUTMOD_7;
        dummy = (TB0CCR0 >> 4);

        dutyCycle = 12;
        for (i = 0; i < brightness; i++)
            dutyCycle += dummy;

        TB0CCR4 = dutyCycle;

        //If the backlight was previously turned off, turn it on.
        if (!backlight)
            TB0CTL |= MC0;
    }
    else
    {
        TB0CCTL4 = 0;
        TB0CTL &= ~MC0;
    }
    backlight = brightness;
}

/***************************************************************************//**
 * @brief   Disable display
 * @param   None
 * @return  None
 ******************************************************************************/

void Dogs102x6_disable(void)
{
    uint8_t cmd[1] = { SYSTEM_RESET };

    Dogs102x6_writeCommand(cmd, 1);
    cmd[0] = SET_DISPLAY_ENABLE & 0xFE;
    Dogs102x6_writeCommand(cmd, 1);
}

/***************************************************************************//**
 * @brief   Sends commands to LCD via 3 wire SPI
 * @param   sCmd Pointer to the commands to be written to the LCD
 * @param   i Number of commands to be written to the LCD
 * @return  None
 ******************************************************************************/

void Dogs102x6_writeCommand(uint8_t *sCmd, uint8_t i)
{
    // Store current GIE state
    uint16_t gie = __get_SR_register() & GIE;

    // Make this operation atomic
    __disable_interrupt();

    // CS Low
    P7OUT &= ~CS;

    // CD Low
    P5OUT &= ~CD;
    while (i)
    {
        // USCI_B1 TX buffer ready?
        while (!(UCB1IFG & UCTXIFG)) ;

        // Transmit data
        UCB1TXBUF = *sCmd;

        // Increment the pointer on the array
        sCmd++;

        // Decrement the Byte counter
        i--;
    }

    // Wait for all TX/RX to finish
    while (UCB1STAT & UCBUSY) ;

    // Dummy read to empty RX buffer and clear any overrun conditions
    UCB1RXBUF;

    // CS High
    P7OUT |= CS;

    // Restore original GIE state
    __bis_SR_register(gie);
}

/***************************************************************************//**
 * @brief   Sends Data to LCD via 3 wire SPI
 * @param   sData Pointer to the Data to be written to the LCD
 * @param   i Number of data bytes to be written to the LCD
 * @return  None
 ******************************************************************************/

void Dogs102x6_writeData(uint8_t *sData, uint8_t i)
{
    // Store current GIE state
    uint16_t gie = __get_SR_register() & GIE;

    // Make this operation atomic
    __disable_interrupt();

    // CS Low
    P7OUT &= ~CS;
    //CD High
    P5OUT |= CD;

    while (i)
    {
        dogs102x6Memory[2 + (currentPage * 102) + currentColumn] = (uint8_t)*sData;
        currentColumn++;

        // Boundary check
        if (currentColumn > 101)
        {
            currentColumn = 101;
        }

        // USCI_B1 TX buffer ready?
        while (!(UCB1IFG & UCTXIFG)) ;

        // Transmit data and increment pointer
        UCB1TXBUF = *sData++;

        // Decrement the Byte counter
        i--;
    }

    // Wait for all TX/RX to finish
    while (UCB1STAT & UCBUSY) ;

    // Dummy read to empty RX buffer and clear any overrun conditions
    UCB1RXBUF;

    // CS High
    P7OUT |= CS;

    // Restore original GIE state
    __bis_SR_register(gie);
}

/***************************************************************************//**
 * @brief   Gets the current contrast level
 * @param   None
 * @return  Contrast level
 ******************************************************************************/

uint8_t Dogs102x6_getContrast(void)
{
    return contrast;
}

/***************************************************************************//**
 * @brief   Gets the current backlight level
 * @param   None
 * @return  Backlight level
 ******************************************************************************/

uint8_t Dogs102x6_getBacklight(void)
{
    return backlight;
}

/***************************************************************************//**
 * @brief   Sets Address of the LCD RAM memory
 *
 *          (0,0) is the upper left corner of screen.
 * @param   pa Page Address of the LCD RAM memory to be written (0 - 7)
 * @param   ca Column Address of the LCD RAM memory to be written (0 - 101)
 * @return  None
 ******************************************************************************/

void Dogs102x6_setAddress(uint8_t pa, uint8_t ca)
{
    uint8_t cmd[1];

    // Page boundary check
    if (pa > 7)
    {
        pa = 7;
    }

    // Column boundary check
    if (ca > 101)
    {
        ca = 101;
    }

    // Page Address Command = Page Address Initial Command + Page Address
    cmd[0] = SET_PAGE_ADDRESS + (7 - pa);
    uint8_t H = 0x00;
    uint8_t L = 0x00;
    uint8_t ColumnAddress[] = { SET_COLUMN_ADDRESS_MSB, SET_COLUMN_ADDRESS_LSB };

    currentPage = pa;
    currentColumn = ca;

    // Separate Command Address to low and high
    L = (ca & 0x0F);
    H = (ca & 0xF0);
    H = (H >> 4);
    // Column Address CommandLSB = Column Address Initial Command
    //                             + Column Address bits 0..3
    ColumnAddress[0] = SET_COLUMN_ADDRESS_LSB + L;
    // Column Address CommandMSB = Column Address Initial Command
    //                             + Column Address bits 4..7
    ColumnAddress[1] = SET_COLUMN_ADDRESS_MSB + H;

    // Set page address
    Dogs102x6_writeCommand(cmd, 1);
    // Set column address
    Dogs102x6_writeCommand(ColumnAddress, 2);
}

/***************************************************************************//**
 * @brief   Sets the contrast
 * @param   contrast Contrast level (0~31, where 31 is darkest setting)
 * @return  None
 ******************************************************************************/

void Dogs102x6_setContrast(uint8_t newContrast)
{
    uint8_t cmd[2];

    cmd[0] = SET_ELECTRONIC_VOLUME_MSB;

    //check if parameter is in range
    if (newContrast > 0x1F)
        cmd[1] = 0x1F;
    else
        cmd[1] = newContrast;

    contrast = cmd[1];

    Dogs102x6_writeCommand(cmd, 2);

    // Save new contrast to initMacro
    Dogs102x6_initMacro[8] = newContrast;
}

/***************************************************************************//**
 * @brief   Inverts the screen (pixels on/off) - does not affect LCD memory.
 * @param   None
 * @return  None
 ******************************************************************************/

void Dogs102x6_setInverseDisplay(void)
{
    uint8_t cmd[] = {SET_INVERSE_DISPLAY + 0x01};

    Dogs102x6_writeCommand(cmd, 1);
}

/***************************************************************************//**
 * @brief   Uninverts the screen (pixels on/off) - does not affect LCD memory.
 * @param   None
 * @return  None
 ******************************************************************************/

void Dogs102x6_clearInverseDisplay(void)
{
    uint8_t cmd[] = {SET_INVERSE_DISPLAY};

    Dogs102x6_writeCommand(cmd, 1);
}

/***************************************************************************//**
 * @brief   Scrolls image down a number of lines. The scrolling wraps around the screen.
 * @param   lines number of lines to scroll (0~63)
 * @return  None
 ******************************************************************************/

void Dogs102x6_scrollLine(uint8_t lines)
{
    uint8_t cmd[] = {SET_SCROLL_LINE};

    //check if parameter is in range
    if (lines > 0x1F)
    {
        cmd[0] |= 0x1F;
    }
    else
    {
        cmd[0] |= lines;
    }

    Dogs102x6_writeCommand(cmd, 1);
}

/***************************************************************************//**
 * @brief   Sets all Pixels on
 * @param   None
 * @return  None
 ******************************************************************************/

void Dogs102x6_setAllPixelsOn(void)
{
    uint8_t cmd[] = {SET_ALL_PIXEL_ON + 0x01};

    Dogs102x6_writeCommand(cmd, 1);
}

/***************************************************************************//**
 * @brief   Returns pixels to normal functioning
 * @param   None
 * @return  None
 ******************************************************************************/

void Dogs102x6_clearAllPixelsOn(void)
{
    uint8_t cmd[] = {SET_ALL_PIXEL_ON};

    Dogs102x6_writeCommand(cmd, 1);
}

/****************************DRAWING FUNCTIONS*********************************/

/***************************************************************************//**
 * @brief   Sets All pixels Off (in memory as well)
 * @param   None
 * @return  None
 ******************************************************************************/

void Dogs102x6_clearScreen(void)
{
    uint8_t LcdData[] = {0x00};
    uint8_t p, c;

    // 8 total pages in LCD controller memory
    for (p = 0; p < 8; p++)
    {
        Dogs102x6_setAddress(p, 0);
        // 102 total columns in LCD controller memory
        for (c = 0; c < 102; c++)
        {
            Dogs102x6_writeData(LcdData, 1);
        }
    }
}

/***************************************************************************//**
 * @brief   Writes a character from FONT6x8[] array to the LCD at (row,col).
 *
 *          (0,0) is the upper left corner of screen.
 * @param   row Page Address (0 - 7) (there are 8 pages on the screen, each is 8 pixels tall)
 * @param   col Column Address (there are 102 columns on the screen)
 * @param   f Pointer to the character to be written to the LCD
 * @param   style The style of the text
 *                - NORMAL = 0 = dark letters with light background
 *                - INVERT = 1 = light letters with dark background
 * @return  None
 ******************************************************************************/

void Dogs102x6_charDraw(uint8_t row, uint8_t col, uint16_t f, uint8_t style)
{
    // Each Character consists of 6 Columns on 1 Page
    // Each Page presents 8 pixels vertically (top = MSB)
    uint8_t b;
    uint16_t h;
    uint8_t inverted_char[6];

    // Row boundary check
    if (row > 7)
    {
        row = 7;
    }

    // Column boundary check
    if (col > 101)
    {
        col = 101;
    }

    // handle characters not in our table
    if (f < 32 || f > 129)
    {
        // replace the invalid character with a '.'
        f = '.';
    }

    // subtract 32 because FONT6x8[0] is "space" which is ascii 32,
    // multiply by 6 because each character is columns wide
    h = (f - 32) * 6;

    Dogs102x6_setAddress(row, col);
    if (style == DOGS102x6_DRAW_NORMAL)
    {
        // write character
        Dogs102x6_writeData((uint8_t *)FONT6x8 + h, 6);
    }
    else
    {
        for (b = 0; b < 6; b++)
        {
            // invert the character
            inverted_char[b] = FONT6x8[h + b] ^ 0xFF;
        }
        // write inverted character
        Dogs102x6_writeData(inverted_char, 6);
    }
}

/***************************************************************************//**
 * @brief   Writes a character from FONT6x8[] array to the LCD at (x,y).
 *
 *          (0,0) is the upper left corner of screen.
 * @param   x Horizontal coordinate (0 - 102)
 * @param   y Vertical coordinate (0 - 63)
 * @param   f Pointer to the character to be written to the LCD
 * @param   style The style of the text
 *                - NORMAL = 0 = dark letters with light background
 *                - INVERT = 1 = light letters with dark background
 * @return  None
 ******************************************************************************/

void Dogs102x6_charDrawXY(uint8_t x, uint8_t y, uint16_t f, uint8_t style)
{
    // Each Character consists of 6 Columns 8 pixels tall
    uint8_t b, row;
    uint16_t h;
    uint8_t desired_char[12];

    // make sure we won't be writing off the screen
    if (x >= 102)
    {
        x = 101;
    }
    if (y >= 64)
    {
        y = 63;
    }

    // handle characters not in our table
    if (f < 32 || f > 129)
    {
        // replace the invalid character with a '.'
        f = '.';
    }

    // subtract 32 because FONT6x8[0] is "space" which is ascii 32,
    // multiply by 6 because each character is 6 columns wide
    h = (f - 32) * 6;

    // Check if there is a remainder
    row = y / 8;

    if (style == DOGS102x6_DRAW_NORMAL)
    {
        for (b = 0; b < 6; b++)
        {
            desired_char[b] =
                (FONT6x8[h + b] >> (y % 8)) | dogs102x6Memory[2 + (row * 102) + x + b];
            desired_char[b + 6] =
                FONT6x8[h + b] << (8 - y % 8) | dogs102x6Memory[2 + ((row + 1) * 102) + x + b];
        }
    }
    else
    {
        for (b = 0; b < 6; b++)
        {
            desired_char[b] = (FONT6x8[h + b] ^ 0xFF) >> (y % 8);
            desired_char[b + 6] = (FONT6x8[h + b] ^ 0xFF) << (8 - y % 8);
        }
    }
    Dogs102x6_setAddress(row, x);
    // write first line of character
    Dogs102x6_writeData(desired_char, 6);
    Dogs102x6_setAddress(row + 1, x);
    // write second line of character
    Dogs102x6_writeData(desired_char + 6, 6);
}

/***************************************************************************//**
 * @brief   Writes a String to the LCD at (row,col).
 *
 *          (0,0) is the upper left corner of screen.
 * @param   row Page Address (there are 8 pages on the screen, each is 8 pixels
 *          tall) (0 - 7)
 * @param   col Column Address (there are 102 columns on the screen) (0 - 101)
 * @param   word[] Pointer to the String to be written to the LCD
 * @param   style The style of the text
 *                - NORMAL = 0 = dark letters with light background
 *                - INVERT = 1 = light letters with dark background
 * @return  None
 ******************************************************************************/

void Dogs102x6_stringDraw(uint8_t row, uint8_t col, char *word, uint8_t style)
{
    // Each Character consists of 6 Columns on 1 Page
    // Each Page presents 8 pixels vertically (top = MSB)
    uint8_t a = 0;

    // Row boundary check
    if (row > 7)
    {
        row = 7;
    }

    // Column boundary check
    if (col > 101)
    {
        col = 101;
    }

    while (word[a] != 0)
    {
        // check for line feed '/n'
        if (word[a] != 0x0A)
        {
            //check for carriage return '/r' (ignore if found)
            if (word[a] != 0x0D)
            {
                //Draw a character
                Dogs102x6_charDraw(row, col, word[a], style);

                //Update location
                col += 6;

                //Text wrapping
                if (col >= 102)
                {
                    col = 0;
                    if (row < 7)
                        row++;
                    else
                        row = 0;
                }
            }
        }
        // handle line feed character
        else
        {
            if (row < 7)
                row++;
            else
                row = 0;
            col = 0;
        }
        a++;
    }
}

/***************************************************************************//**
 * @brief   Writes a String to the LCD at (x,y).
 *
 *          (0,0) is the upper left corner of screen.
 * @param   x Horizontal coordinate
 * @param   y Vertical coordinate
 * @param   word[] Pointer to the String to be written to the LCD
 * @param   style The style of the text
 *                - NORMAL = 0 = dark letters with light background
 *                - INVERT = 1 = light letters with dark background
 * @return  None
 ******************************************************************************/

void Dogs102x6_stringDrawXY(uint8_t x, uint8_t y, char *word, uint8_t style)
{
    /* Each Character consists of 6 Columns on 1 Page
     * Each Page presents 8 pixels vertically (top = MSB)*/
    uint8_t a = 0;

    while (word[a] != 0)
    {
        // Draw a character
        Dogs102x6_charDrawXY(x, y, word[a], style);

        // Update location
        x += 6;

        // Text wrapping
        if (x >= 102)
        {
            x = 0;
            if (y + 8 < 64)
                y += 8;
            else
                y = 0;
        }
        a++;
    }
}

/***************************************************************************//**
 * @brief   Clears one row/page (in memory as well).
 *
 *          Row 0 is at the top of the screen.
 * @param   row The row to be cleared (0~7)
 * @return  None
 ******************************************************************************/

void Dogs102x6_clearRow(uint8_t row)
{
    uint8_t cmd[] = {0};
    uint8_t a = 0;

    // Check row boundary
    if (row > 7)
    {
        row = 7;
    }

    Dogs102x6_setAddress(row, 0);
    for (a = 0; a < 102; a++)
    {
        Dogs102x6_writeData(cmd, 1);
        dogs102x6Memory[2 + (row * 102) + a] = 0x00;
    }
}

/***************************************************************************//**
 * @brief  Draws a pixel at (x,y).
 *
 *          (0,0) is the upper left corner of screen.
 * @param  x x-coordinate of the point
 * @param  y y-coordinate of the point
 * @param   style The style of the pixel
 *                - NORMAL = 0 = dark pixel
 *                - INVERT = 1 = inverts pixel
 * @return None
 ******************************************************************************/

void Dogs102x6_pixelDraw(uint8_t x, uint8_t y, uint8_t style)
{
    uint8_t p, temp;

    //make sure we won't be writing off the screen
    if (x > 101)
    {
        x = 101;
    }

    if (y > 63)
    {
        y = 63;
    }

    //determine the page
    p = y / 8;

    //determine height of pixel within the page
    temp = 0x80 >> (y % 8);

    //update our array
    if (style == DOGS102x6_DRAW_NORMAL)
        dogs102x6Memory[2 + (p * 102) + x] |= temp;
    else
        dogs102x6Memory[2 + (p * 102) + x] &= ~temp;

    Dogs102x6_setAddress(p, x);

    //draw pixel
    Dogs102x6_writeData(dogs102x6Memory + (2 + (p * 102) + x), 1);
}

/***************************************************************************//**
 * @brief   Draws a horizontal line on the LCD.
 *
 *          (0,0) is the upper left corner of screen.
 * @param   x1 The horizontal line starting pixel coordinate (0~101)
 * @param   x2 The horizontal line ending pixel coordinate (0~101)
 * @param   y  The height of the line (pixel row, not page) (0~63)
 * @param   style The style of the line
 *                - NORMAL = 0 = dark line, overwrites
 *                - INVERT = 1 = inverts color on line
 * @return  None
 ******************************************************************************/

void Dogs102x6_horizontalLineDraw(uint8_t x1, uint8_t x2, uint8_t y, uint8_t style)
{
    uint8_t temp = 0, p, a;

    //make sure we won't be writing off the screen
    if (x1 > 101)
    {
        x1 = 101;
    }

    if (x2 > 101)
    {
        x2 = 101;
    }

    if (y > 63)
    {
        y = 63;
    }

    //swap coordinates if x1 is past x2
    if (x1 > x2)
    {
        temp = x1;
        x1 = x2;
        x2 = temp;
    }

    //determine the page
    p = y / 8;
    //determine height of line within the page
    temp = 0x80 >> (y % 8);

    a = x1;
    while (a <= x2)
    {
        if (style == DOGS102x6_DRAW_NORMAL)
        {
            //draws a dark line through everything
            dogs102x6Memory[2 + (p * 102) + a] |= temp;
        }
        else
        {
            //draws a light line through everything
            dogs102x6Memory[2 + (p * 102) + a] &= ~temp;
        }

        a++;
    }

    Dogs102x6_setAddress(p, x1);

    //draw the line
    Dogs102x6_writeData(dogs102x6Memory + (2 + (p * 102) + x1), x2 - x1 + 1);
}

/***************************************************************************//**
 * @brief   Draws a vertical line on the LCD.
 *
 *          (0,0) is the upper left corner of screen.
 * @param   y1 The vertical line starting pixel coordinate (0~63)
 * @param   y2 The vertical line ending pixel coordinate (0~63)
 * @param   x  The column of line (0~101)
 * @param   style The style of the line
 *                - NORMAL = 0 = dark line, overwrites
 *                - INVERT = 1 = inverts color on line
 * @return  None
 ******************************************************************************/

void Dogs102x6_verticalLineDraw(uint8_t y1, uint8_t y2, uint8_t x, uint8_t style)
{
    uint8_t temp1 = 0, temp2 = 0, p1, p2, a;

    //make sure we won't be writing off the screen
    if (y1 > 63)
    {
        y1 = 63;
    }

    if (y2 > 63)
    {
        y2 = 63;
    }

    if (x > 101)
    {
        x = 101;
    }

    //swap coordinates if y1 is past y2
    if (y1 > y2)
    {
        temp1 = y1;
        y1 = y2;
        y2 = temp1;
    }

    //determine the first page of line
    p1 = y1 / 8;
    //determine the last page of line
    p2 = y2 / 8;

    //mask for last page of line
    temp2 = 8 - (y2 % 8);
    temp2--;
    temp2 = 0xFF << temp2;

    //mask for first page of line
    // Length that spans across multiple pages
    if (p1 != p2)
    {
        // Check if there is an offset
        if (y1 > 0)
        {
            temp1 = 0xFF00 >> (y1 % 8);
            temp1 = temp1 ^ 0xFF;
        }
        // If there is no offset, set all bits for lower mask
        else
        {
            temp1 = 0xFF;
        }
    }
    // Short length that only spans one page
    else
    {
        // Clear lower page bits
        temp1 = 0;

        // mask off y1 from top page bits
        a = y1 - (p1 * 8);
        a = 0xFF00 >> a;
        temp2 = temp2 ^ a;
    }

    // Prepare for first page of line
    if (style == DOGS102x6_DRAW_NORMAL)
    {
        dogs102x6Memory[2 + (p1 * 102) + x] |= temp1;
    }
    else
    {
        dogs102x6Memory[2 + (p1 * 102) + x] &= ~temp1;
    }

    // Set starting address for first page
    Dogs102x6_setAddress(p1, x);
    //draw first page of line
    Dogs102x6_writeData(dogs102x6Memory + (2 + (p1 * 102) + x), 1);

    a = p1 + 1;
    while (a < p2)
    {
        if (style == DOGS102x6_DRAW_NORMAL)
        {
            //draws a dark line through everything
            dogs102x6Memory[2 + (a * 102) + x] = 0xFF;
        }
        else
        {
            //draws a light line through everything
            dogs102x6Memory[2 + (a * 102) + x] &= 0x00;
        }
        Dogs102x6_setAddress(a, x);
        // draw middle of line
        Dogs102x6_writeData(dogs102x6Memory + (2 + (a * 102) + x), 1);
        a++;
    }

    // Prepare for last page of line
    if (style == DOGS102x6_DRAW_NORMAL)
    {
        dogs102x6Memory[2 + (p2 * 102) + x] |= temp2;
    }
    else
    {
        dogs102x6Memory[2 + (p2 * 102) + x] &= ~temp2;
    }

    Dogs102x6_setAddress(p2, x);
    //draw last page of line
    Dogs102x6_writeData(dogs102x6Memory + (2 + (p2 * 102) + x), 1);
}

/***************************************************************************//**
 * @brief  Draws a line from (x1,y1) to (x2,y2).
 *
 *         Uses Bresenham's line algorithm.
 *         (0,0) is the upper left corner of screen.
 * @param  x1   x-coordinate of the first point
 * @param  y    y-coordinate of the first point
 * @param  x2   x-coordinate of the second point
 * @param  y2   y-coordinate of the second point
 * @param   style The style of the line
 *                - NORMAL = 0 = dark line, overwrites
 *                - INVERT = 1 = inverts color on line
 * @return None
 ******************************************************************************/

void Dogs102x6_lineDraw(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t style)
{
    int8_t x, y, deltay, deltax, d;
    int8_t x_dir, y_dir;

    //make sure we won't be writing off the screen
    if (y1 > 63)
    {
        y1 = 63;
    }

    if (y2 > 63)
    {
        y2 = 63;
    }

    if (x1 > 101)
    {
        x1 = 101;
    }

    if (x2 > 101)
    {
        x2 = 101;
    }

    // Check if vertical line. Use more efficient vertical line function
    if (x1 == x2)
    {
        Dogs102x6_verticalLineDraw(y1, y2, x1, style);
    }
    // Check if horizontal line. Use more efficient horizontal line function
    else if (y1 == y2)
    {
        Dogs102x6_horizontalLineDraw(x1, x2, y1, style);
    }
    // else it is a diagonal line
    else
    {
        if (x1 > x2)
        {
            x_dir = -1;
        }
        else
        {
            x_dir = 1;
        }

        if (y1 > y2)
        {
            y_dir = -1;
        }
        else
        {
            y_dir = 1;
        }

        x = x1;
        y = y1;
        deltay = abs(y2 - y1);
        deltax = abs(x2 - x1);

        if (deltax >= deltay)
        {
            d = (deltay << 1) - deltax;
            while (x != x2)
            {
                Dogs102x6_pixelDraw(x, y,  style);
                if (d < 0)
                    d += (deltay << 1);
                else
                {
                    d += ((deltay - deltax) << 1);
                    y += y_dir;
                }
                x += x_dir;
            }
        }
        else
        {
            d = (deltax << 1) - deltay;
            while (y != y2)
            {
                Dogs102x6_pixelDraw(x, y, style);
                if (d < 0)
                    d += (deltax << 1);
                else
                {
                    d += ((deltax - deltay) << 1);
                    x += x_dir;
                }
                y += y_dir;
            }
        }
    }
}

/***************************************************************************//**
 * @brief   Draw a circle of Radius with center at (x,y).
 *
 *          Uses Bresenham's circle algorithm.
 *          (0,0) is the upper left corner of screen.
 * @param   x   x-coordinate of the circle's center point
 * @param   y   y-coordinate of the circle's center point
 * @param   radius  Radius of the circle
 * @param   style The style of the circle
 *                - NORMAL = 0 = dark line, overwrites
 *                - INVERT = 1 = inverts color on line
 * @return  None
 ******************************************************************************/

void Dogs102x6_circleDraw(uint8_t x, uint8_t y, uint8_t radius, uint8_t style)
{
    int8_t xx, yy, ddF_x, ddF_y, f;

    ddF_x = 0;
    ddF_y = -(2 * radius);
    f = 1 - radius;

    xx = 0;
    yy = radius;
    Dogs102x6_pixelDraw(x + xx, y + yy, style);
    Dogs102x6_pixelDraw(x + xx, y - yy, style);
    Dogs102x6_pixelDraw(x - xx, y + yy, style);
    Dogs102x6_pixelDraw(x - xx, y - yy, style);
    Dogs102x6_pixelDraw(x + yy, y + xx, style);
    Dogs102x6_pixelDraw(x + yy, y - xx, style);
    Dogs102x6_pixelDraw(x - yy, y + xx, style);
    Dogs102x6_pixelDraw(x - yy, y - xx, style);
    while (xx < yy)
    {
        if (f >= 0)
        {
            yy--;
            ddF_y += 2;
            f += ddF_y;
        }
        xx++;
        ddF_x += 2;
        f += ddF_x + 1;
        Dogs102x6_pixelDraw(x + xx, y + yy, style);
        Dogs102x6_pixelDraw(x + xx, y - yy, style);
        Dogs102x6_pixelDraw(x - xx, y + yy, style);
        Dogs102x6_pixelDraw(x - xx, y - yy, style);
        Dogs102x6_pixelDraw(x + yy, y + xx, style);
        Dogs102x6_pixelDraw(x + yy, y - xx, style);
        Dogs102x6_pixelDraw(x - yy, y + xx, style);
        Dogs102x6_pixelDraw(x - yy, y - xx, style);
    }
}

/***************************************************************************//**
 * @brief   Loads an image of size = height * width, starting at (row,col).
 *          The first two bytes of the image should contain the width in pixels
 *          and the height in rows (height in rows = height in pixels/8)
 *
 *          (0,0) is the upper left corner of screen.
 * @param   image[] The image to be loaded.
 * @param   height  The number of rows in the image. Size = Rows * Columns.
 * @param   width   The number of columns in the image. Size = Rows * Columns.
 * @param   row     row of the image's starting location
 * @param   col     column of the image's starting location
 * @return None
 ******************************************************************************/

void Dogs102x6_imageDraw(const uint8_t IMAGE[], uint8_t row, uint8_t col)
{
    // height in rows (row = 8 pixels), width in columns
    uint8_t a, height, width;

    width = IMAGE[0];
    height = IMAGE[1];
    for (a = 0; a < height; a++)
    {
        Dogs102x6_setAddress(row + a, col);
        // Draw a row of the image
        Dogs102x6_writeData((uint8_t*)IMAGE + 2 + a * width, width);
    }
}

/***************************************************************************//**
 * @brief   Clears an area of size = height * width, starting at (row,col).
 *
 *          (0,0) is the upper left corner of screen.
 * @param   height  The number of rows in the image. Size = Rows * Columns.
 * @param   width   The number of columns in the image. Size = Rows * Columns.
 * @param   row row of the image's starting location
 * @param   col column of the image's starting location
 * @return None
 ******************************************************************************/

void Dogs102x6_clearImage(uint8_t height, uint8_t width, uint8_t row, uint8_t col)
{
    uint8_t a, b;
    uint8_t cmd[] = {0x00};

    for (a = 0; a < height; a++)
    {
        Dogs102x6_setAddress(row + a, col);
        for (b = 0; b < width; b++)
        {
            // clear a byte
            Dogs102x6_writeData(cmd, 1);
        }
    }
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
