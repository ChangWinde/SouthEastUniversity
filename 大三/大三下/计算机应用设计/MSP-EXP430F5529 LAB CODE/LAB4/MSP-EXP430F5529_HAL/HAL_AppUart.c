/*******************************************************************************
 *
 *  HAL_AppUart.c - Code for controlling Application UART
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
 * @file       HAL_AppUart.c
 * @addtogroup HAL_AppUart
 * @{
 ******************************************************************************/
#include <stdint.h>
#include "msp430.h"
#include "HAL_UCS.h"
#include "HAL_AppUart.h"
#include "HAL_Dogs102x6.h"

/***************************************************************************//**
 * @brief   Initialize the Application UART
 * @param   None
 * @return  None
 ******************************************************************************/

void AppUart_init(void)
{
    SELECT_ACLK(SELA__XT1CLK);              // Source ACLK from LFXT1

    P4SEL = BIT5 + BIT4;                    // P4.4,5 = USCI_A1 TXD/RXD
    UCA1CTL1 |= UCSWRST;                    // **Put state machine in reset**
    UCA1CTL0 = 0x00;
    UCA1CTL1 = UCSSEL_1 + UCSWRST;          // Use ACLK, keep RESET
    UCA1BR0 = 0x03;                         // 32kHz/9600=3.41 (see User's Guide)
    UCA1BR1 = 0x00;                         //
    UCA1MCTL = UCBRS_3 + UCBRF_0;           // Modulation UCBRSx=3, UCBRFx=0
    UCA1CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
}

/***************************************************************************//**
 * @brief   Receive a character via Application UART
 * @param   None
 * @return  received character
 ******************************************************************************/

uint8_t AppUart_getChar(void)
{
    uint8_t receiveChar;

    while (!(UCA1IFG & UCRXIFG)) ;
    receiveChar = UCA1RXBUF;
    return receiveChar;
}

/***************************************************************************//**
 * @brief   Transmit a character via Application UART
 * @param   transmitChar  Character to be transmitted
 * @return  None
 ******************************************************************************/

void AppUart_putChar(uint8_t transmitChar)
{
    while (!(UCA1IFG & UCTXIFG)) ;
    UCA1TXBUF = transmitChar;
}

