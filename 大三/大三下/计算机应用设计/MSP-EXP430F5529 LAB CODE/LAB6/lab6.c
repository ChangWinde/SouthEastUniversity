//***************************************************************************//
//                                                                           //
//                         MSP-EXP430F5529 LAB CODE                          //
//                                                                           //
//                           lab6 - Power Tests                              //
//                                                                           //
//***************************************************************************//
/*******************************************************************************
 *
 *  Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
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

#include <stdint.h>
#include "msp430.h"
#include "HAL_Buttons.h"
#include "HAL_Dogs102x6.h"
#include "HAL_Menu.h"
#include "PMM.h"
#include "LPM.h"
#include "lab6.h"

static const char *const powerMenuText[] = {
    "===Power Tests===",
    "1. Active Mode ",
    "2. Low Power Mode"
};

/***************************************************************************//**
 * @brief  Executes the "Power Tests" menu option in the User Experience
 *         example code. This menu option allows one to select and perform
 *         active and low power mode tests.
 * @param  none
 * @return none
 ******************************************************************************/

void lab6(void)
{
    uint8_t selection = 0;

    buttonsPressed = 0;
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(0, 0, "LAB6:Power Tests", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(3, 0, " Press button S1 ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(4, 0, "  to start LAB6. ", DOGS102x6_DRAW_NORMAL);
    while (!(buttonsPressed & BUTTON_S1)) ;          //等待S1键按下

    char square[3] = {('~' + 2), ('~' + 3), '\0'};

    Dogs102x6_clearScreen();
    buttonsPressed = 0;
    while (!(buttonsPressed & BUTTON_S2))            //若S2键按下，退出程序
    {
        selection = Menu_active((char **)powerMenuText, 2);   //显示功耗测试菜单，并进行选择
        if (buttonsPressed & BUTTON_S2) break;
        buttonsPressed = 0;
        Dogs102x6_clearScreen();

        //进入活动模式功耗测试
        if (selection == 1)
        {
            Dogs102x6_stringDraw(0, 0, "Press Touch Pads ", DOGS102x6_DRAW_NORMAL);
            Dogs102x6_stringDraw(1, 0, "To Select Column ", DOGS102x6_DRAW_NORMAL);
            Dogs102x6_stringDraw(3, 0, "  Vcore & MCLK   ", DOGS102x6_DRAW_NORMAL);
            Dogs102x6_stringDraw(4, 0, "    X   &        ", DOGS102x6_DRAW_NORMAL);
            Dogs102x6_stringDraw(4, 67, square, DOGS102x6_DRAW_NORMAL);
            Dogs102x6_stringDraw(5, 0, "S1 selects choice", DOGS102x6_DRAW_NORMAL);
            Dogs102x6_stringDraw(7, 0, "   Press S1...   ", DOGS102x6_DRAW_INVERT);

            // Wait for button press
            while (!buttonsPressed)
            {
                // Wait in low power mode 3 until a button is pressed
                __bis_SR_register(LPM3_bits + GIE);
            }

            if (buttonsPressed & BUTTON_S1)
                MenuPMMMCLK();                //选择一种活动模式进行功耗测试
            Dogs102x6_clearScreen();

        }

        //进入低功耗模式功耗测试
        else if (selection == 2)
        {
            Dogs102x6_stringDraw(0, 0, "Select an LPM, ", DOGS102x6_DRAW_NORMAL);
            Dogs102x6_stringDraw(1, 0, "then press S1 to ", DOGS102x6_DRAW_NORMAL);
            Dogs102x6_stringDraw(2, 0, "enter LPM for ", DOGS102x6_DRAW_NORMAL);
            Dogs102x6_stringDraw(3, 0, "measurement. ", DOGS102x6_DRAW_NORMAL);
            Dogs102x6_stringDraw(6, 0, "  (Press S1 to   ", DOGS102x6_DRAW_INVERT);
            Dogs102x6_stringDraw(7, 0, "   continue.)    ", DOGS102x6_DRAW_INVERT);

            // Wait for button press
            while (!buttonsPressed)
            {
                // Wait in low power mode 3 until a button is pressed
                __bis_SR_register(LPM3_bits + GIE);
            }

            if (buttonsPressed & BUTTON_S1)
                MenuLPM();                      //选择一种低功耗模式进行功耗测试
            Dogs102x6_clearScreen();
        }
        buttonsPressed = 0;
    }
    Dogs102x6_clearScreen();
    buttonsPressed = 0;
}

/***************************************************************************//**
 * @brief  Handles UNMI interrupts
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR(void)
{
    switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG))
    {
        case SYSUNIV_NONE:
            __no_operation();
            break;
        case SYSUNIV_NMIIFG:
            __no_operation();
            break;
        case SYSUNIV_OFIFG:
            UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT2OFFG); // Clear OSC flaut Flags fault flags
            SFRIFG1 &= ~OFIFG;                          // Clear OFIFG fault flag
            break;
        case SYSUNIV_ACCVIFG:
            __no_operation();
            break;
        case SYSUNIV_BUSIFG:
            // If bus error occurred - the cleaning of flag and re-initializing of USB is required.
            SYSBERRIV = 0;                              // clear bus error flag
           // USB_disable();                              // Disable
    }
}
/***************************************************************************//**
 * @}
 ******************************************************************************/
