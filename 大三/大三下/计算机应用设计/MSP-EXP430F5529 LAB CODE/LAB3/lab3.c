//***************************************************************************//
//                                                                           //
//                         MSP-EXP430F5529 LAB CODE                          //
//                                                                           //
//                             lab3 - AccelDemo                              //
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
#include "HAL_PMM.h"
#include "HAL_UCS.h"
#include "HAL_Board.h"
#include "HAL_Buttons.h"
#include "HAL_Cma3000.h"
#include "HAL_Dogs102x6.h"
#include "HAL_Menu.h"
#include "HAL_Wheel.h"
#include "LaunchpadDef.h"
#include "Clock.h"
#include "LPM.h"
#include "MassStorage.h"
#include "Mouse.h"
#include "PMM.h"
#include "SdCard.h"
#include "lab3.h"
#include "puzzle.h"
#include "HAL_Wheel.h"
#include "Cube.h"

static const char *const accelMenuText[] = {
    "=LAB3: AccelDemo=",
    "1. Calibrte Accel",
    "2. Demo Cube ",
    "3. Tilt Puzzle",
};

void CalibrateAccel(void);

/***************************************************************************//**
 * @brief  The menu of lab3.
 * @param  none
 * @return none
 ******************************************************************************/

void lab3(void)
{
	   uint8_t selection = 0;

	    buttonsPressed = 0;

	    Dogs102x6_clearScreen();
	    while (!(buttonsPressed & BUTTON_S2))
	    {
	        Dogs102x6_stringDraw(7, 0, "*S1=Enter S2=Esc*", DOGS102x6_DRAW_NORMAL);
	        selection = Menu_active((char **)accelMenuText, 3);
	        if (buttonsPressed & BUTTON_S2) break;
	        switch (selection)
	        {
                case 1: CalibrateAccel(); break;      //加速度计校准实验程序
                case 2: Cube(); break;                //动态立方体演示实验程序
                case 3: StartPuzzle(); break;         //数字拼图游戏实验程序
	            default: break;
	        }
	    }
	    Dogs102x6_clearScreen();
}

/***************************************************************************//**
 * @brief  Calibrates the Accelerometer
 * @param  none
 * @return none
 ******************************************************************************/

void CalibrateAccel(void)
{
    uint8_t i;

    uint16_t Cma3000_xAccel_offset;
    uint16_t Cma3000_yAccel_offset;
    uint16_t Cma3000_zAccel_offset;

    static const char *const AccelCalibrateText[] = {
        "**    Accel    **",
        "*  Calibration  *",
        "",
        " Place Board on  ",
        "  Flat Surface   ",
        "",
        "",
        "    Press S1     "
    };

    buttonsPressed = 0;
    Dogs102x6_clearScreen();

    // Initialize accelerometer
    Cma3000_init();           //加速度计初始化

    for (i = 0; i < 8; i++){
        Dogs102x6_stringDraw(i, 0, (char *)AccelCalibrateText[i], DOGS102x6_DRAW_NORMAL);//显示说明：请将开发板放置在水平表面，之后按下S1
    }

    while (!buttonsPressed)
    {
        // Wait in low power mode 3 until a button is pressed
        __bis_SR_register(LPM3_bits + GIE);     //进入低功耗模式3，等待按键按下，唤醒CPU
    }
    //读取加速度计偏移量
    Cma3000_xAccel_offset = Cma3000_readRegister(DOUTX); // Read to get x-Offset
    Cma3000_yAccel_offset = Cma3000_readRegister(DOUTY); // Read to get y-Offset
    Cma3000_zAccel_offset = Cma3000_readRegister(DOUTZ); // Read to get z-Offset

    // 设置加速度计偏移量
    Cma3000_setAccel_offset(Cma3000_xAccel_offset,
                            Cma3000_yAccel_offset,
                            Cma3000_zAccel_offset);

    // 将加速度计偏移量写入FLASH
    WriteFlashSettings(Cma3000_xAccel_offset, accelXcalibrationAddress);
    WriteFlashSettings(Cma3000_yAccel_offset, accelYcalibrationAddress);
    WriteFlashSettings(Cma3000_zAccel_offset, accelZcalibrationAddress);

    buttonsPressed = 0;
    Dogs102x6_clearScreen();

    // 禁用加速度计
    Cma3000_disable();
}

/***************************************************************************//**
 * @brief  Writes the settings to flash
 * @param  Data to be written, and offset
 * @return none
 ******************************************************************************/

void WriteFlashSettings(uint16_t Data, uint16_t Address)
{
    uint16_t * Flash_ptr;                   // Initialize Flash pointer
    uint16_t Flash_Contents[16];            // Store Contents of Flash before Programming
    uint8_t i;

    Flash_ptr = (uint16_t *)(0x1880);       // Info C

    for (i = 0; i < 16; i++)                // We know that we will only use > 8 variables in Info C
    {
        Flash_Contents[i] = *Flash_ptr;     // Read a word from flash
        if (Flash_ptr == (uint16_t *)Address)
            Flash_Contents[i] = Data;
        *Flash_ptr++;
    }


    Flash_ptr = (uint16_t *)(0x1880);       // Info C
    FCTL3 = FWKEY;                          // Clear Lock bit
    FCTL1 = FWKEY + ERASE;                  // Set Erase bit
    *Flash_ptr = 0;                         // Dummy write to erase Flash seg
    FCTL1 = FWKEY + WRT;                    // Set WRT bit for write operation

    for (i = 0; i < 16; i++)
    {
        *Flash_ptr++ = Flash_Contents[i];   // Write a word to flash
    }

    FCTL1 = FWKEY;                          // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                   // Set LOCK bit
}

