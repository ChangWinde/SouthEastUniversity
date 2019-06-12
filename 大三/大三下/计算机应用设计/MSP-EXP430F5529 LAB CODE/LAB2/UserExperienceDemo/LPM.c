/*******************************************************************************
 *
 *  LPM.c - Demonstrates the different low-power modes of the MSP430
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
 * @file       LPM.c
 * @addtogroup LPM
 * @{
 ******************************************************************************/
#include <stdint.h>
#include "msp430.h"
#include "HAL_UCS.h"
#include "HAL_Buttons.h"
#include "HAL_Board.h"
#include "HAL_Dogs102x6.h"
#include "HAL_Menu.h"
#include "HAL_Wheel.h"
#include "LPM.h"
#include "HAL_PMM.h"

#define LPM_MENU_MAX 6

static const char *const lpmMenuText[] = {
    "====Select LPM===",
    " 1. LPM0 - 1MHz ",
    " 2. LPM3 - REFO ",
    " 3. LPM3 - LFXT1 ",
    " 4. LPM3 - VLO ",
    " 5. LPM4 "
};

// Forward declared functions
void LowPowerMode0(void);
void LowPowerMode3(uint8_t aclkSource);
void LowPowerMode4(void);

void DisablePorts(void);

/***************************************************************************//**
 * @brief Executes the "LPM" menu option in the User Experience example code.
 *
 *        This menu option allows one to observe the effects of different
 *        Low-Power Modes. For accurate current measurements, place a
 *        multimeter at JP1 and begin measurement once the LCD display has
 *        shutdown. Expected values for different Low Power Modes can be found
 *        in the MSP430F5438A datasheet.
 * @param  none
 * @return none
 ******************************************************************************/

void MenuLPM(void)
{
    uint8_t quit = 0;
    uint8_t selection;

    Buttons_interruptEnable(BUTTON_ALL);

    buttonsPressed = 0;
    Dogs102x6_clearScreen();

    //unlock usb config regs
    USBKEYPID = 0x9628;
    USBPWRCTL &= ~(VUSBEN + SLDOEN);
    //lock usb config regs
    USBKEYPID = 0x0000;

    while (!quit)
    {
        selection = Menu_active((char **)lpmMenuText, 5);

        //S1 selects a menu option
        if (buttonsPressed & BUTTON_S1)
        {
            switch (selection)
            {
                case 1: LowPowerMode0(); break;
                case 2: LowPowerMode3(0); break; // LPM3 - REFO
                case 3: LowPowerMode3(1); break; // LPM3 - LFXT1
                case 4: LowPowerMode3(2); break; // LPM3 - VLO
                case 5: LowPowerMode4(); break;
                case 6: quit = 1; break;
                default: break;
            }
            buttonsPressed = 0;
        }

        if (buttonsPressed & BUTTON_S2)
        {
            quit = 1;
        }
    }

    Dogs102x6_clearScreen();

    //unlock usb config regs
    USBKEYPID = 0x9628;
    USBPWRCTL |= VUSBEN + SLDOEN;
    //lock usb config regs
    USBKEYPID = 0x0000;
}

/***************************************************************************//**
 * @brief Enters LPM0 until user presses a button.
 * @param  none
 * @return none
 ******************************************************************************/

void LowPowerMode0(void)
{
    uint8_t currentBacklight;
    uint8_t currentContrast;

    buttonsPressed = 0;
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(2, 12, "Entering LPM0 ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(3, 15, "Press button ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(4, 30, "to wake. ", DOGS102x6_DRAW_NORMAL);

    //Delay for user to read screen
    __delay_cycles(30000000);

    //save the current backlight setting
    currentBacklight = Dogs102x6_getBacklight();

    // Save current contrast setting
    currentContrast = Dogs102x6_getContrast();

    //turn off the Lcd and backlight to conserve power
    Dogs102x6_setBacklight(0);
    Dogs102x6_disable();

    Wheel_disable();
    DisablePorts();

    //disable oscillator fault before configuring FLL because of DCOFFG
    SFRIE1 &= ~OFIE;
    Init_FLL_Settle(1000, 31);
    SFRIE1 |= OFIE;

    do
    {
        // Enter LPM0
        __bis_SR_register(LPM0_bits + GIE);
        // For debugger only
        __no_operation();
    }
    // Exit on button press
    while (buttonsPressed == 0);

    // Disable oscillator fault before configuring FLL because of DCOFFG
    SFRIE1 &= ~OFIE;

    // Set system clock to max (25MHz)
    Init_FLL_Settle(25000, 762);
    SFRIE1 |= OFIE;

    // Re-enable wheel
    Wheel_enable();

    // Reinitialize LCD display
    Dogs102x6_init();
    Dogs102x6_setContrast(currentContrast);
    Dogs102x6_clearScreen();
    Dogs102x6_setBacklight(currentBacklight);
}

/***************************************************************************//**
 * @brief Enters LPM3 until user presses a button. Sources ACLK according to parameter aclkSource.
 * @param  aclkSource Desired source of ACLK. 0 = REFO, 1 = LFXT1, 2 = VLO
 * @return none
 ******************************************************************************/

void LowPowerMode3(uint8_t aclkSource)
{
    uint8_t currentBacklight;
    uint8_t currentContrast;

    buttonsPressed = 0;
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(2, 12, "Entering LPM3 ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(3, 15, "Press button ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(4, 30, "to wake. ", DOGS102x6_DRAW_NORMAL);

    //Delay for user to read screen
    __delay_cycles(30000000);

    //save the current backlight setting
    currentBacklight = Dogs102x6_getBacklight();

    // Save current contrast setting
    currentContrast = Dogs102x6_getContrast();

    //turn off the Lcd and backlight to conserve power
    Dogs102x6_setBacklight(0);
    Dogs102x6_disable();

    Wheel_disable();

    switch (aclkSource)
    {
        case 0:
            // Select REFO to source ACLK
            SELECT_ACLK(SELA__REFOCLK);
            break;
        case 1:
            // Select LFXT1 to source ACLK
            SELECT_ACLK(SELA__XT1CLK);
            break;
        case 2:
            // Select VLO to source ACLK
            SELECT_ACLK(SELA__VLOCLK);
            break;
        default:
            break;
    }

    SFRIE1 &= ~OFIE;
    DisablePorts();

    do
    {
        // Enter LPM3
        __bis_SR_register(LPM3_bits + GIE);
        // For debugger only
        __no_operation();
    }
    //Exit on button press
    while (buttonsPressed == 0);
    SFRIE1 |= OFIE;

    // Re-enable wheel
    Wheel_enable();

    // Reinitialize LCD display
    Dogs102x6_init();
    Dogs102x6_setContrast(currentContrast);
    Dogs102x6_clearScreen();
    Dogs102x6_setBacklight(currentBacklight);
}

/***************************************************************************//**
 * @brief Enters LPM4 until user presses a button.
 * @param  none
 * @return none
 ******************************************************************************/

void LowPowerMode4(void)
{
    uint8_t currentBacklight;
    uint8_t currentContrast;

    buttonsPressed = 0;
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(2, 12, "Entering LPM4 ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(3, 15, "Press button ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(4, 30, "to wake. ", DOGS102x6_DRAW_NORMAL);

    //Delay for user to read screen
    __delay_cycles(30000000);

    //save the current backlight setting
    currentBacklight = Dogs102x6_getBacklight();

    // Save current contrast setting
    currentContrast = Dogs102x6_getContrast();

    //turn off the Lcd and backlight to conserve power
    Dogs102x6_setBacklight(0);
    Dogs102x6_disable();

    Wheel_disable();

    UCSCTL8_L = 0;
    UCSCTL6_L |= SMCLKOFF;

    // disable oscillator fault before configuring FLL because of DCOFFG
    SFRIE1 &= ~OFIE;
    Init_FLL_Settle(8000, 244);
    SFRIE1 |= OFIE;

    SetVCore(2);
    SetVCore(1);
    SetVCore(0);
    DisablePorts();

    do
    {
        // Enter LPM4
        __bis_SR_register(LPM4_bits + GIE);
        // For debugger only
        __no_operation();
    }
    // Exit on button press
    while (buttonsPressed == 0);

    Board_init();
    // Use 32.768kHz XTAL as reference
    LFXT_Start(XT1DRIVE_0);
    SetVCore(1);
    SetVCore(2);
    SetVCore(3);

    // disable oscillator fault before configuring FLL because of DCOFFG
    SFRIE1 &= ~OFIE;
    // Set system clock to max (25MHz)
    Init_FLL_Settle(25000, 762);
    SFRIE1 |= OFIE;
    UCSCTL8_L = 0x0F;
    UCSCTL6_L &= ~SMCLKOFF;

    // Re-enable wheel
    Wheel_enable();

    // Reinitialize LCD display
    Dogs102x6_init();
    Dogs102x6_setContrast(currentContrast);
    Dogs102x6_clearScreen();
    Dogs102x6_setBacklight(currentBacklight);
}

//*****************************************************************************
// Set all ports to output
//*****************************************************************************
void DisablePorts(void)
{
    // Port Configuration
    P1OUT = 0x00; P2OUT = 0x00; P3OUT = 0x00; P4OUT = 0x00; P5OUT = 0x00;
    P6OUT = 0x00; P7OUT = 0x00; P8OUT = 0x00; PJOUT = 0x00;
    P1DIR = 0xFF; P2DIR = 0xFF; P3DIR = 0xFF; P4DIR = 0xFF; P5DIR = 0xFF;
    P6DIR = 0xFF; P7DIR = 0xFF; P8DIR = 0xFF; PJDIR = 0xFF;

    Buttons_init(BUTTON_ALL);
    Buttons_interruptEnable(BUTTON_ALL);
}

