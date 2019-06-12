/*******************************************************************************
 *
 *  HAL_Buttons.c - Driver for the buttons
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
 * @file       HAL_Buttons.c
 * @addtogroup HAL_Buttons
 * @{
 ******************************************************************************/
#include "msp430.h"
#include "HAL_Board.h"
#include "HAL_Buttons.h"
#include "HAL_Cma3000.h"

#define BUTTON_PORT_DIR   PADIR
#define BUTTON_PORT_OUT   PAOUT
#define BUTTON_PORT_SEL   PASEL
#define BUTTON_PORT_REN   PAREN
#define BUTTON_PORT_IE    PAIE
#define BUTTON_PORT_IES   PAIES
#define BUTTON_PORT_IFG   PAIFG
#define BUTTON_PORT_IN    PAIN
#define BUTTON1_PIN       BIT7       //P1.7
#define BUTTON2_PIN       BIT2       //P2.2
#define BUTTON1_IFG       P1IFG      //P1.7
#define BUTTON2_IFG       P2IFG      //P1.7

volatile uint8_t buttonDebounce = 1;
volatile uint16_t buttonsPressed = 0;

// Forward declared functions
void Buttons_startWDT(void);

/***************************************************************************//**
 * @brief  Initialize ports for buttons as active low inputs
 * @param  buttonsMask   Use values defined in HAL_buttons.h for the buttons to
 *                       initialize
 * @return none
 ******************************************************************************/

void Buttons_init(uint16_t buttonsMask)
{
    BUTTON_PORT_OUT |= buttonsMask;  //buttons are active low
    BUTTON_PORT_REN |= buttonsMask;  //pullup resistor
    BUTTON_PORT_SEL &= ~buttonsMask;
}

/***************************************************************************//**
 * @brief  Enable button interrupts for selected buttons
 * @param  buttonsMask   Use values defined in HAL_buttons.h for the buttons to
 *                       enable
 * @return none
 ******************************************************************************/

void Buttons_interruptEnable(uint16_t buttonsMask)
{
    BUTTON_PORT_IES &= ~buttonsMask; //select falling edge trigger
    BUTTON_PORT_IFG &= ~buttonsMask; //clear flags
    BUTTON_PORT_IE |= buttonsMask;   //enable interrupts
}

/***************************************************************************//**
 * @brief  Disable button interrupts for selected buttons
 * @param  buttonsMask   Use values defined in HAL_buttons.h for the buttons to
 *                       disable
 * @return none
 ******************************************************************************/

void Buttons_interruptDisable(uint16_t buttonsMask)
{
    BUTTON_PORT_IE &= ~buttonsMask;
}

/***************************************************************************//**
 * @brief  Sets up the WDT as a button debouncer, only activated once a
 *         button interrupt has occurred.
 * @param  none
 * @return none
 ******************************************************************************/

void Buttons_startWDT()
{
    // WDT as 250ms interval counter
    SFRIFG1 &= ~WDTIFG;
    WDTCTL = WDTPW + WDTSSEL_1 + WDTTMSEL + WDTCNTCL + WDTIS_5;
    SFRIE1 |= WDTIE;
}

/***************************************************************************//**
 * @brief  Handles Watchdog Timer interrupts.
 *
 *         Global variables are used to determine the module triggering the
 *         interrupt, and therefore, how to handle it.
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    if (buttonDebounce == 2)
    {
        buttonDebounce = 1;

        SFRIFG1 &= ~WDTIFG;
        SFRIE1 &= ~WDTIE;
        WDTCTL = WDTPW + WDTHOLD;
    }
}

/***************************************************************************//**
 * @brief  Handles Port 2 interrupts - performs button debouncing and registers
 *         button presses.
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void)
{
    //
    // Context save interrupt flag before calling interrupt vector.
    // Reading interrupt vector generator will automatically clear IFG flag
    //
    buttonsPressed = PAIFG & BUTTON_ALL;

    switch (__even_in_range(P2IV, P2IV_P2IFG7))
    {
        // Vector  P2IV_NONE:  No Interrupt pending
        case  P2IV_NONE:
            break;

        // Vector  P2IV_P2IFG0:  P2IV P2IFG.0
        case  P2IV_P2IFG0:
            break;

        // Vector  P2IV_P2IFG1:  P2IV P2IFG.1
        case  P2IV_P2IFG1:
            break;

        // Vector  P2IV_P2IFG2:  P2IV P2IFG.2
        case  P2IV_P2IFG2:
            if (buttonDebounce == 1)
            {
                buttonDebounce = 2;
                Buttons_startWDT();
                __bic_SR_register_on_exit(LPM3_bits);
            }
            else if (buttonDebounce == 0)
            {
                __bic_SR_register_on_exit(LPM4_bits);
            }

            break;

        // Vector  P2IV_P2IFG3:  P2IV P2IFG.3
        case  P2IV_P2IFG3:
            break;

        // Vector  P2IV_P2IFG4:  P2IV P2IFG.4
        case  P2IV_P2IFG4:
            break;

        // Vector  P2IV_P2IFG5:  P2IV P2IFG.5
        case  P2IV_P2IFG5:
            break;

        // Vector  P2IV_P2IFG1:  P2IV P2IFG.6
        case  P2IV_P2IFG6:
            break;

        // Vector  P2IV_P2IFG7:  P2IV P2IFG.7
        case  P2IV_P2IFG7:
            break;

        // Default case
        default:
            break;
    }
}

/***************************************************************************//**
 * @brief  Handles Port 1 interrupts - performs button debouncing and registers
 *         button presses.
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
    //
    // Context save interrupt flag before calling interrupt vector.
    // Reading interrupt vector generator will automatically clear IFG flag
    //
    buttonsPressed = PAIFG & BUTTON_ALL;

    switch (__even_in_range(P1IV, P1IV_P1IFG7))
    {
        // Vector  P1IV_NONE:  No Interrupt pending
        case  P1IV_NONE:
            break;

        // Vector  P1IV_P1IFG0:  P1IV P1IFG.0
        case  P1IV_P1IFG0:
            break;

        // Vector  P1IV_P1IFG1:  P1IV P1IFG.1
        case  P1IV_P1IFG1:
            break;

        // Vector  P1IV_P1IFG2:  P1IV P1IFG.2
        case  P1IV_P1IFG2:
            break;

        // Vector  P1IV_P1IFG3:  P1IV P1IFG.3
        case  P1IV_P1IFG3:
            break;

        // Vector  P1IV_P1IFG4:  P1IV P1IFG.4
        case  P1IV_P1IFG4:
            break;

        // Vector  P1IV_P1IFG5:  P1IV P1IFG.5
        case  P1IV_P1IFG5:
            break;

        // Vector  P1IV_P1IFG1:  P1IV P1IFG.6
        case  P1IV_P1IFG6:
            break;

        // Vector  P1IV_P1IFG7:  P1IV P1IFG.7
        case  P1IV_P1IFG7:
            if (buttonDebounce == 1)
            {
                buttonDebounce = 2;
                Buttons_startWDT();
                __bic_SR_register_on_exit(LPM3_bits);
            }
            else if (buttonDebounce == 0)
            {
                __bic_SR_register_on_exit(LPM4_bits);
            }

            break;

        // Default case
        default:
            break;
    }
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
