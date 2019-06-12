/*******************************************************************************
 *
 *  HAL_Menu.c - Program for creating menus for the Experimenter's board
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
 * @file       HAL_Menu.c
 * @addtogroup HAL_Menu
 * @{
 ******************************************************************************/
#include "msp430.h"
#include "HAL_Dogs102x6.h"
#include "HAL_Buttons.h"
#include "HAL_Menu.h"
#include "HAL_Wheel.h"


/***************************************************************************//**
 * @brief   Set up a menu
 * @param   menuText  The text for the menu title and options.
 * @param   numItems  The number of menu items ( numItems <= 7 )
 * @return  Position of selection
 ******************************************************************************/

uint8_t Menu_active(char **menuText, uint8_t numItems)
{
    uint8_t i;
    uint8_t position = 0;
    uint8_t lastPosition = 9;

    if (numItems > 7){                                              // Screen is 8 lines tall = 1
                                                                    // title line + 7 items max
        numItems = 7;
    }
    Dogs102x6_clearImage(numItems + 1, 102, 0, 0);
    Dogs102x6_stringDraw(0, 0, menuText[0], DOGS102x6_DRAW_NORMAL); // Print the title

    buttonsPressed = 0;
    while (!buttonsPressed)                                         // Menu active until selection
                                                                    // is made
    {
        position = Wheel_getPosition();
        if (position > numItems){
            position = numItems;
        }
        else if (position == 0){
            position = 1;
        }
        if (position != lastPosition)                               // Update position if it is
                                                                    // changed
        {
            for (i = 1; i < numItems + 1; i++)                      // Display menu items
            {
                if (i != position){
                    Dogs102x6_stringDraw(i, 0, menuText[i], DOGS102x6_DRAW_NORMAL);
                }
                else {
                    // Highlight item at current position
                    Dogs102x6_stringDraw(i, 0, menuText[i], DOGS102x6_DRAW_INVERT);
                }
            }
            lastPosition = position;
        }
    }
    return position;
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
