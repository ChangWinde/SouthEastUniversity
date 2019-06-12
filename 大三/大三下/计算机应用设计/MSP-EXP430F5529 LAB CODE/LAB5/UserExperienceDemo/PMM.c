/*******************************************************************************
 *
 *  PMM.c - Demonstrates the different VCore and MCLK settings and their effect
 *          on current consumption.
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
 * @file       PMM.c
 * @addtogroup PMM
 * @{
 ******************************************************************************/
#include <stdint.h>
#include "msp430.h"
#include "HAL_PMM.h"
#include "HAL_UCS.h"
#include "HAL_Board.h"
#include "HAL_Buttons.h"
#include "HAL_Dogs102x6.h"
#include "HAL_Wheel.h"
#include "PMM.h"
#include "5xx_ACTIVE_test.h"
#include "CTS_Layer.h"


static const char *const menuTextCol1[] = {
    "Vcore ",
    "1.40V ",
    "1.60V ",
    "1.80V ",
    "1.90V "
};

static const char *const menuTextCol2[] = {
    "MCLK ",
    " 1MHz ",
    " 4MHz ",
    " 8MHz ",
    "12MHz ",
    "16MHz ",
    "20MHz ",
    "25MHz "
};

static const uint8_t MAX_MCLK_ALLOWED[] = {3, 5, 6, 7};
uint8_t leftPosition = 0, rightPosition = 7;

// Forward declared functions
void SetValues(void);
void MeasurementMode(void);

/***************************************************************************//**
 * @brief  Executes the "PMM-MCLK" menu option in the User Experience
 *         example code. This menu option allows one to change the frequency
 *         of operation for the MSP430 and the VCore setting.
 * @param  none
 * @return none
 ******************************************************************************/

void MenuPMMMCLK(void)
{
    uint8_t leftCol = 1;
    uint8_t quit = 0;
    uint16_t i;

    struct Element *keysPressed;

    uint8_t currentMaxMCLK = 7;
    uint8_t lastLeftPosition = 9, lastRightPosition = 9;

    Dogs102x6_clearScreen();

    //print the left title
    Dogs102x6_stringDraw(0, 0, (char *)menuTextCol1[0], DOGS102x6_DRAW_NORMAL);

    //print the right title
    Dogs102x6_stringDraw(0, 51, (char *)menuTextCol2[0], DOGS102x6_DRAW_NORMAL);

    buttonsPressed = 0;

    TI_CAPT_Init_Baseline(&slider);

    while (!quit)
    {
        buttonsPressed = 0;
        TI_CAPT_Init_Baseline(&slider);

        //menu active until selection is made
        while (!buttonsPressed)
        {
            keysPressed = (struct Element *)TI_CAPT_Buttons(&slider);
            if (keysPressed == (struct Element *)&element0)
                leftCol = 1;
            else if (keysPressed == (struct Element *)&element1)
                leftCol = 0;

            //left column is active
            if (leftCol)
            {
                Board_ledOn(LED4);
                Board_ledOff(LED5);
                leftPosition = Wheel_getPosition();
                if (leftPosition > 4)
                    leftPosition = 4;
                if (leftPosition < 1)
                    leftPosition = 1;
                //update position if it is changed
                if (leftPosition != lastLeftPosition)
                {
                    //display left column menu items
                    for (i = 1; i < 5; i++)
                    {
                        if (i != leftPosition)
                            Dogs102x6_stringDraw(i, 0, (char *)menuTextCol1[i],
                                                 DOGS102x6_DRAW_NORMAL);
                        else
                            //highlight item at current position
                            Dogs102x6_stringDraw(i, 0, (char *)menuTextCol1[i],
                                                 DOGS102x6_DRAW_INVERT);
                    }
                    currentMaxMCLK = MAX_MCLK_ALLOWED[leftPosition - 1];
                    if (rightPosition > currentMaxMCLK)
                        rightPosition = currentMaxMCLK;

                    //Only display MCLK values that are allowed for this VCore setting
                    for (i = 1; i < 8; i++)
                    {
                        if (i == rightPosition)
                            Dogs102x6_stringDraw(i, 51, (char *)menuTextCol2[i],
                                                 DOGS102x6_DRAW_INVERT);                    //highlight
                                                                                            // item
                                                                                            // at
                                                                                            //
                                                                                            // current
                                                                                            //
                                                                                            // position
                        else if (i <= currentMaxMCLK)
                            Dogs102x6_stringDraw(i, 51, (char *)menuTextCol2[i],
                                                 DOGS102x6_DRAW_NORMAL);
                        else
                            Dogs102x6_stringDraw(i, 51, "       ", DOGS102x6_DRAW_NORMAL);  //Blank
                                                                                            // lines
                                                                                            // for
                                                                                            //
                                                                                            // unavailable
                                                                                            // MCLKs
                    }
                    lastLeftPosition = leftPosition;
                    lastRightPosition = rightPosition;
                }
            }
            else //right column is active
            {
                Board_ledOn(LED5);
                Board_ledOff(LED4);
                rightPosition = Wheel_getPosition();
                if (rightPosition > currentMaxMCLK)
                    rightPosition = currentMaxMCLK;
                if (rightPosition < 1)
                    rightPosition = 1;
                //update position if it is changed
                if (rightPosition != lastRightPosition)
                {
                    //display menu items
                    for (i = 1; i <= currentMaxMCLK; i++)
                    {
                        if (i != rightPosition)
                            Dogs102x6_stringDraw(i, 51, (char *)menuTextCol2[i],
                                                 DOGS102x6_DRAW_NORMAL);
                        else
                            //highlight item at current position
                            Dogs102x6_stringDraw(i, 51, (char *)menuTextCol2[i],
                                                 DOGS102x6_DRAW_INVERT);
                    }
                    lastRightPosition = rightPosition;
                }
            }
        }
        if (buttonsPressed & BUTTON_S1)
        {
            buttonsPressed = 0;
            Board_ledOff(LED_ALL);
            //set values to selected VCore and clock for measurement
            SetValues();
            //optimized configuration for measuring current consumption
            MeasurementMode();
            //return to normal VCore setting
            SetVCore(3);
            //reinitialize cap touch pads
            TI_CAPT_Init_Baseline(&slider);

            //reprint the menu

            //print the left title
            Dogs102x6_stringDraw(0, 0, (char *)menuTextCol1[0], DOGS102x6_DRAW_NORMAL);

            //print the right title
            Dogs102x6_stringDraw(0, 51, (char *)menuTextCol2[0], DOGS102x6_DRAW_NORMAL);

            //set to an unreachable value so that menu will be refreshed
            lastLeftPosition = 9;

            //set to an unreachable value so that menu will be refreshed
            lastRightPosition = 9;
            leftCol = 1;
        }
        if (buttonsPressed & BUTTON_S2)
        {
            quit = 1;
        }
    }
    Board_ledOff(LED5 | LED4 | LED3 | LED2);
    Dogs102x6_clearScreen();
    SFRIE1 &= ~OFIE;

    //Set system clock to max (25MHz)
    Init_FLL_Settle(25000, 762);
    SFRIE1 |= OFIE;
}

/***************************************************************************//**
 * @brief  Sets the values for Vcore and MCLK
 * @param  none
 * @return none
 ******************************************************************************/

void SetValues(void)
{
    // disable oscillator fault before configuring FLL because of DCOFFG
    SFRIE1 &= ~OFIE;

    // Set CPU to lowest frequency first before setting PMM.
    // Then ultimately set the final frequency.
    Init_FLL_Settle(1000, 31);

    // Set PMM module
    switch (leftPosition)
    {
        case 1:
            //Set to 1.40V
            SetVCore(0);
            break;

        case 2:
            //Set to 1.60V
            SetVCore(1);
            break;

        case 3:
            //Set to 1.80V
            SetVCore(2);
            break;

        case 4:
            //Set to 1.90V
            SetVCore(3);
            break;
    }

    // Set CPU frequency
    switch (rightPosition)
    {
        case 1:
            //Set to 1MHz
            Init_FLL_Settle(1000, 31);
            break;
        case 2:
            //Set to 4MHz
            Init_FLL_Settle(4000, 122);
            break;
        case 3:
            //Set to 8MHz
            Init_FLL_Settle(8000, 244);
            break;
        case 4:
            //Set to 12MHz
            Init_FLL_Settle(12000, 366);
            break;
        case 5:
            //Set to 16MHz
            Init_FLL_Settle(16000, 488);
            break;
        case 6:
            //Set to 20MHz
            Init_FLL_Settle(20000, 610);
            break;
        case 7:
            //Set to 25MHz
            Init_FLL_Settle(25000, 762);
            break;
    }

    SFRIE1 |= OFIE;
}

/***************************************************************************//**
 * @brief  Turns off screen for current measurement.
 * @param  none
 * @return none
 ******************************************************************************/

void MeasurementMode(void)
{
    uint8_t currentBacklight;
    uint16_t i, j, delay;

    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(2, 21, "Screen off ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(3, 15, "Press button ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(4, 30, "to wake. ", DOGS102x6_DRAW_NORMAL);
    //Delay for user to read screen

    switch (rightPosition)
    {
        case 1:     __delay_cycles(2000000);  delay = 2000;  break;
        case 2:     __delay_cycles(8000000);  delay = 8000;  break;
        case 3:     __delay_cycles(16000000); delay = 16000; break;
        case 4:     __delay_cycles(24000000); delay = 24000; break;
        case 5:     __delay_cycles(32000000); delay = 32000; break;
        case 6:     __delay_cycles(40000000); delay = 40000; break;
        case 7:     __delay_cycles(50000000); delay = 50000; break;
        default: delay = 2000; break;
    }
    //turn off the Lcd to conserve power
    currentBacklight = Dogs102x6_getBacklight(); //save the current backlight setting
    Dogs102x6_setBacklight(0);
    Dogs102x6_clearScreen();
    Dogs102x6_disable();
    Wheel_disable();
    SetValues();

    while (!buttonsPressed){                     //wait for button push
        for (i = 0; i < delay; i++)              //delay between LED_2 pulses
            for (j = 0; j < 10; j++)
                ACTIVE_MODE_TEST();              //Emulate a real application instead of continuous
                                                 // jumps

        for (i = 0; i < (delay >> 4); i++)       //delay so LED2 pulse is visible
            ACTIVE_MODE_TEST();                  //Emulate a real application instead of continuous
                                                 // jumps
    }

    Wheel_enable();

    buttonsPressed = 0;                          //clear button press
    Dogs102x6_init();                            //turn screen back on
    Dogs102x6_setContrast(Dogs102x6_getContrast());
    Dogs102x6_setBacklight(currentBacklight);
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
