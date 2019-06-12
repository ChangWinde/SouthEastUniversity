/*******************************************************************************
 *
 *  HAL_Cma3000.c - Code for using the CMA3000-D01 3-Axis Ultra Low Power
 *                  Accelerometer
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
 * @file       HAL_Cma3000.c
 * @addtogroup HAL_Cma3000
 * @{
 ******************************************************************************/
#include "msp430.h"
#include "HAL_UCS.h"
#include "HAL_Cma3000.h"

// CONSTANTS
#define MCLK                    25000000
#define TICKSPERUS              (MCLK / 1000000)

// PORT DEFINITIONS
#define ACCEL_INT_IN            P2IN
#define ACCEL_INT_OUT           P2OUT
#define ACCEL_INT_DIR           P2DIR
#define ACCEL_SCK_SEL           P2SEL
#define ACCEL_INT_IE            P2IE
#define ACCEL_INT_IES           P2IES
#define ACCEL_INT_IFG           P2IFG
#define ACCEL_INT_VECTOR        PORT2_VECTOR
#define ACCEL_OUT               P3OUT
#define ACCEL_DIR               P3DIR
#define ACCEL_SEL               P3SEL

// PIN DEFINITIONS
#define ACCEL_INT               BIT5
#define ACCEL_CS                BIT5
#define ACCEL_SIMO              BIT3
#define ACCEL_SOMI              BIT4
#define ACCEL_SCK               BIT7
#define ACCEL_PWR               BIT6

// ACCELEROMETER REGISTER DEFINITIONS
#define REVID                   0x01
#define CTRL                    0x02
#define MODE_400                0x04        // Measurement mode 400 Hz ODR
#define DOUTX                   0x06
#define DOUTY                   0x07
#define DOUTZ                   0x08
#define G_RANGE_2               0x80        // 2g range
#define I2C_DIS                 0x10        // I2C disabled

int8_t accelData;
int8_t RevID;
int8_t Cma3000_xAccel;
int8_t Cma3000_yAccel;
int8_t Cma3000_zAccel;

// Stores x-Offset
int8_t Cma3000_xAccel_offset;

// Stores y-Offset
int8_t Cma3000_yAccel_offset;

// Stores z-Offset
int8_t Cma3000_zAccel_offset;


/***************************************************************************//**
 * @brief  Configures the CMA3000-D01 3-Axis Ultra Low Power Accelerometer
 * @param  none
 * @return none
 ******************************************************************************/

void Cma3000_init(void)
{
    do
    {
        // Set P3.6 to output direction high
        ACCEL_OUT |= ACCEL_PWR;
        ACCEL_DIR |= ACCEL_PWR;

        // P3.3,4 option select
        ACCEL_SEL |= ACCEL_SIMO + ACCEL_SOMI;

        // P2.7 option select
        ACCEL_SCK_SEL |= ACCEL_SCK;

        ACCEL_INT_DIR &= ~ACCEL_INT;

        // Generate interrupt on Lo to Hi edge
        ACCEL_INT_IES &= ~ACCEL_INT;

        // Clear interrupt flag
        ACCEL_INT_IFG &= ~ACCEL_INT;

        // Unselect acceleration sensor
        ACCEL_OUT |= ACCEL_CS;
        ACCEL_DIR |= ACCEL_CS;

        // **Put state machine in reset**
        UCA0CTL1 |= UCSWRST;
        // 3-pin, 8-bit SPI master Clock polarity high, MSB
        UCA0CTL0 = UCMST + UCSYNC + UCCKPH + UCMSB;
        // Use SMCLK, keep RESET
        UCA0CTL1 = UCSWRST + UCSSEL_2;
        // /0x30
        UCA0BR0 = 0x30;
        // 0
        UCA0BR1 = 0;
        // No modulation
        UCA0MCTL = 0;
        // **Initialize USCI state machine**
        UCA0CTL1 &= ~UCSWRST;

        // Read REVID register
        RevID = Cma3000_readRegister(REVID);
        __delay_cycles(50 * TICKSPERUS);

        // Activate measurement mode: 2g/400Hz
        accelData = Cma3000_writeRegister(CTRL, G_RANGE_2 | I2C_DIS | MODE_400);

        // Settling time per DS = 10ms
        __delay_cycles(1000 * TICKSPERUS);

        // INT pin interrupt disabled
        ACCEL_INT_IE  &= ~ACCEL_INT;

        // Repeat till interrupt Flag is set to show sensor is working
    } while (!(ACCEL_INT_IN & ACCEL_INT));
}

/***************************************************************************//**
 * @brief  Disables the CMA3000-D01 3-Axis Ultra Low Power Accelerometer
 * @param  none
 * @return none
 ******************************************************************************/

void Cma3000_disable(void)
{
    // Set P3.6 to output direction low
    ACCEL_OUT &= ~ACCEL_PWR;

    // Disable P3.3,4 option select
    ACCEL_SEL &= ~(ACCEL_SIMO + ACCEL_SOMI);

    // Disable P2.7 option select
    ACCEL_SCK_SEL &= ~ACCEL_SCK;

    // Set CSn to low
    ACCEL_OUT &= ~ACCEL_CS;

    // INT pin interrupt disabled
    ACCEL_INT_IE  &= ~ACCEL_INT;

    // **Put state machine in reset**
    UCA0CTL1 |= UCSWRST;
}

/***************************************************************************//**
 * @brief  Reads data from the accelerometer
 * @param  None
 * @return None
 ******************************************************************************/

void Cma3000_readAccel(void)
{
    // Read DOUTX register
    Cma3000_xAccel = Cma3000_readRegister(DOUTX);
    __delay_cycles(50 * TICKSPERUS);

    // Read DOUTY register
    Cma3000_yAccel = Cma3000_readRegister(DOUTY);
    __delay_cycles(50 * TICKSPERUS);

    // Read DOUTZ register
    Cma3000_zAccel = Cma3000_readRegister(DOUTZ);
}

/***************************************************************************//**
 * @brief  Sets accelerometer offset.
 * @param  xAccel_offset  x-axis offset
 * @param  yAccel_offset  y-axis offset
 * @param  zAccel_offset  z-axis offset
 * @return None
 ******************************************************************************/

void Cma3000_setAccel_offset(int8_t xAccel_offset,
                             int8_t yAccel_offset,
                             int8_t zAccel_offset)
{
    // Store x-Offset
    Cma3000_xAccel_offset = xAccel_offset;

    // Store y-Offset
    Cma3000_yAccel_offset = yAccel_offset;

    // Store z-Offset
    Cma3000_zAccel_offset = zAccel_offset;
}

/***************************************************************************//**
 * @brief  Reads data from the accelerometer with removed offset
 * @param  None
 * @return None
 ******************************************************************************/

void Cma3000_readAccel_offset(void)
{
    // Read current accelerometer value
    Cma3000_readAccel();

    // remove offset
    Cma3000_xAccel -= Cma3000_xAccel_offset;

    // remove offset
    Cma3000_yAccel -= Cma3000_yAccel_offset;

    // remove offset
    Cma3000_zAccel -= Cma3000_zAccel_offset;
}

/***************************************************************************//**
 *
 * @brief  Reads data from the accelerometer
 * @param  Address  Address of register
 * @return Register contents
 ******************************************************************************/

int8_t Cma3000_readRegister(uint8_t Address)
{
    uint8_t Result;

    // Address to be shifted left by 2 and RW bit to be reset
    Address <<= 2;

    // Select acceleration sensor
    ACCEL_OUT &= ~ACCEL_CS;

    // Read RX buffer just to clear interrupt flag
    Result = UCA0RXBUF;

    // Wait until ready to write
    while (!(UCA0IFG & UCTXIFG)) ;

    // Write address to TX buffer
    UCA0TXBUF = Address;

    // Wait until new data was written into RX buffer
    while (!(UCA0IFG & UCRXIFG)) ;

    // Read RX buffer just to clear interrupt flag
    Result = UCA0RXBUF;

    // Wait until ready to write
    while (!(UCA0IFG & UCTXIFG)) ;

    // Write dummy data to TX buffer
    UCA0TXBUF = 0;

    // Wait until new data was written into RX buffer
    while (!(UCA0IFG & UCRXIFG)) ;

    // Read RX buffer
    Result = UCA0RXBUF;

    // Wait until USCI_A0 state machine is no longer busy
    while (UCA0STAT & UCBUSY) ;

    // Deselect acceleration sensor
    ACCEL_OUT |= ACCEL_CS;

    // Return new data from RX buffer
    return Result;
}

/***************************************************************************//**
 * @brief  Writes data to the accelerometer
 * @param  Address  Address of register
 * @param  accelData     Data to be written to the accelerometer
 * @return  Received data
 ******************************************************************************/

int8_t Cma3000_writeRegister(uint8_t Address, int8_t accelData)
{
    uint8_t Result;

    // Address to be shifted left by 2
    Address <<= 2;

    // RW bit to be set
    Address |= 2;

    // Select acceleration sensor
    ACCEL_OUT &= ~ACCEL_CS;

    // Read RX buffer just to clear interrupt flag
    Result = UCA0RXBUF;

    // Wait until ready to write
    while (!(UCA0IFG & UCTXIFG)) ;

    // Write address to TX buffer
    UCA0TXBUF = Address;

    // Wait until new data was written into RX buffer
    while (!(UCA0IFG & UCRXIFG)) ;

    // Read RX buffer just to clear interrupt flag
    Result = UCA0RXBUF;

    // Wait until ready to write
    while (!(UCA0IFG & UCTXIFG)) ;

    // Write data to TX buffer
    UCA0TXBUF = accelData;

    // Wait until new data was written into RX buffer
    while (!(UCA0IFG & UCRXIFG)) ;

    // Read RX buffer
    Result = UCA0RXBUF;

    // Wait until USCI_A0 state machine is no longer busy
    while (UCA0STAT & UCBUSY) ;

    // Deselect acceleration sensor
    ACCEL_OUT |= ACCEL_CS;

    return Result;
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
