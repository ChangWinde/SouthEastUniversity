//***************************************************************************//
//                                                                           //
//                         MSP-EXP430F5529 LAB CODE                          //
//                                                                           //
//                            lab7 - ComplexLAB                              //
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
#include <stdlib.h>
#include <string.h>
#include "msp430.h"
#include "HAL_PMM.h"
#include "HAL_UCS.h"
#include "HAL_Buttons.h"
#include "HAL_Dogs102x6.h"
#include "HAL_Board.h"
#include "HAL_Menu.h"
#include "HAL_Wheel.h"
#include "MassStorage.h"
#include "mouse.h"
#include "descriptors.h"
#include "usb.h"                            // USB-specific functions
#include "UsbCdc.h"
#include "Demo_Cube.h"
#include "CTS_Layer.h"
#include "LaunchpadDef.h"
#include "Random.h"

char *strrev(char *);
char *itoa(int, char *, int);

static const char *const gameMenuText[] = {
    "=LAB7:ComplexLAB=",
    "1.Defender",
    "2.USB Mouse"
};

/***************************************************************************//**
 * @brief  The menu of lab7.
 * @param  none
 * @return none
 ******************************************************************************/

void lab7(void)
{
	   uint8_t selection = 0;

	    buttonsPressed = 0;

	    Dogs102x6_clearScreen();

	    Dogs102x6_stringDraw(7, 0, "*S1=Enter S2=Esc*", DOGS102x6_DRAW_NORMAL);
	    selection = Menu_active((char **)gameMenuText, 2);
	    if (buttonsPressed & BUTTON_S2);
	    else
	       switch (selection)
	        {
                case 1: LaunchpadDef(); break;    //飞船避障游戏实验程序
                case 2: Mouse(); break;           //USB鼠标实验程序
                default: break;
	        }
}

// Get a new random number via a fast linear congruence generator.
#define NEXT_RAND(x)            (((x) * 1664525) + 1013904223)

// This image has been created by ImageDog using '280px-Learning_Community.jpg'
static const uint8_t image[] =
{
    0x66,    // Image width in pixels
    0x07,    // Image height in rows (1 row = 8 pixels)
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xf9, 0xf9, 0xf1, 0xf4, 0xf2, 0xf7,
    0xf7, 0xf7, 0xf7, 0xf3, 0xfb, 0xfb, 0xfb, 0xf9, 0xfd, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff,
    0xfe, 0xfc, 0xfc, 0xf9, 0xfb, 0xfb, 0xf7, 0xf7, 0xe7, 0xef, 0xef, 0xcf, 0xdf, 0xdd, 0xdd, 0x9d,
    0xbd, 0xbb, 0xbb, 0x3b, 0x7b, 0x7b, 0x7b, 0x73, 0x77, 0x77, 0x77, 0x77, 0xf7, 0xf7, 0xf7, 0xf7,
    0xe7, 0xe7, 0xe7, 0xe7, 0x67, 0x77, 0x77, 0x77, 0x77, 0x77, 0x37, 0xb7, 0xb7, 0xf7, 0xfb, 0xfb,
    0xfa, 0xfa, 0xfe, 0xfe, 0xfe, 0x3e, 0x80, 0xee, 0xce, 0xce, 0xde, 0xde, 0x9e, 0x9e, 0x9e, 0xbe,
    0x3c, 0x3c, 0xbc, 0x3f, 0x3f, 0x3f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0x80, 0xbc, 0xbf, 0x9f, 0x9f, 0x9f, 0xdf, 0xdf, 0xcf, 0xef, 0xe7, 0xe7, 0xf3, 0xf8, 0xfd, 0xff,
    0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x7f, 0xbf, 0xbf, 0xbf, 0xbf, 0xdf, 0xdf, 0xdf,
    0xdd, 0xc9, 0xe4, 0xe6, 0xee, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xef, 0xef,
    0xef, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xef, 0xef, 0xef, 0xef, 0xef, 0xee, 0xee, 0xce, 0xde,
    0xdc, 0xdd, 0xbd, 0xf9, 0xfb, 0x7b, 0x77, 0x77, 0x7f, 0x7f, 0x7f, 0x7e, 0x00, 0x77, 0x73, 0x73,
    0x73, 0x7b, 0x79, 0x79, 0x79, 0x79, 0x78, 0x78, 0x79, 0xf9, 0xf9, 0xf9, 0xf9, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x73, 0xf7, 0xf7, 0xe7, 0xe7, 0xe7, 0xef, 0xcf, 0xcf,
    0x9f, 0x9f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x7f,
    0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
    0x7f, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xcf, 0x8f, 0x8f, 0xaf,
    0x2f, 0x6f, 0xef, 0xef, 0xef, 0xcf, 0xdf, 0xdf, 0xdf, 0xbf, 0xbf, 0x7f, 0x7f, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00
};

// A bitmap for the space ship.
const uint8_t Game_ship[] = {
    0x3F, 0x1F, 0x1F, 0x0F, 0x0F, 0x07, 0x07, 0x03, 0x03, 0x01
};

// A bitmap for mine type one.
const uint8_t Game_mine1[] = {
    0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18
};

// A bitmap for mine type two.
const uint8_t Game_mine2[] =
{
    0xC3, 0xC3, 0x3C, 0x3C, 0x3C, 0x3C, 0xC3, 0xC3
};

// A bitmap for the first stage of an explosion
const uint8_t Game_explosion1[] =
{
    0x28, 0x10, 0x28
};

// A bitmap for the second stage of an explosion
const uint8_t Game_explosion2[] =
{
    0x44, 0x28, 0x10, 0x28, 0x44
};

// A bitmap for the third stage of an explosion
const uint8_t Game_explosion3[] =
{
    0x92, 0x54, 0x28, 0xD6, 0x28, 0x54, 0x92
};

// A bitmap for the fourth stage of an explosion
const uint8_t Game_explosion4[] =
{
    0x10, 0x92, 0x54, 0x00, 0xD7, 0x00, 0x54, 0x92, 0x10,
};

// This array contains the sequence of explosion images, along with the width
// of each one.
const struct
{
    const uint8_t *image;
    uint8_t adjust;
    uint8_t width;
}

Game_explosion[4] =
{
    { Game_explosion1, 0, 3 },
    { Game_explosion2, 1, 5 },
    { Game_explosion3, 2, 7 },
    { Game_explosion4, 3, 9 }
};

// Storage for a local frame buffer.
uint8_t frame[818];         //8 rows, 102 columns on screen + first 2 bytes for height/width data

// Storage for the background image of the tunnel.  This is copied to the local
// frame buffer and then the other elements are overlaid upon it.
uint8_t background[8][102]; //8 rows, 102 columns on screen

// The offsets from the top and bottom scan lines of the display to the wall of
// the tunnel.  The first element is the offset from the top scan line and the
// second element is the offfset from the bottom scan line.
uint8_t offset[2];

// An array of mines currently visible on the display.  Up to five mines can be
// displayed, and each has three variables associated with it: the type (in
// index zero), the horizontal position (in index one), and the vertical
// position (in index two).  If all three variables are negative one, then that
// mine does not exist.
int8_t mines[5][3];

// The location of the missile, if it has been fired.  The first entry contains
// the horizontal position and the second entry contains the vertical position.
// If both are negative one, then the missile has not been fired.
int8_t missile[2];

// An array of explosions currently active on the display.  Up to five
// explosions can be displayed (the fifth being dedicated to the ship
// explosion), and each has three variables associated with it: the explosion
// step number (in index zero), the horizontal position (in index one), and the
// vertical position (in index two).  If the step number is negative one, then
// that explosion is not active.
int8_t explosions[5][3];

// The point accumulated during the game.  One point is added for each time the
// display is scrolled to the left (i.e. the ship travels one step through the
// tunnel) and twenty-five points are added for each obstacle that is shot.
uint32_t score;

// Forward declared functions
void UpdateBackground(uint8_t gap);
void DrawImage(const uint8_t *image, uint8_t x, uint8_t y, uint8_t width);
void UpdateMines(void);
uint8_t DrawShip(void);
void DrawExplosions(void);

/***************************************************************************//**
 * @brief Scroll the tunnel image one column to the left and add a new column of
 *        tunnel on the right side of the display.
 * @param  gap  The minimum allowed gap between the top and bottom of the tunnel
 * @return none
 ******************************************************************************/

void UpdateBackground(uint8_t gap)
{
    int8_t idx;
    uint8_t a;
    uint32_t count;

    for (idx = 0; idx < 5; idx++) // Loop through the array of mines.
    {
        if ((mines[idx][0] == -1) && (mines[idx][1] == -1) && (mines[idx][2] == -1))
        {
            continue;             // Skip this mine if it is disabled.
        }
        if (mines[idx][1] > 91)
        {
            break;                // Stop searching if this mine is near or on the right side of the
                                  // display.
        }
    }

    count = RandomNumber();

    // If the top part of the tunnel is not at the top of the display, then
    // move it up 18.75% of the time.
    if ((count < 0x30000000) && (offset[0] != 0))
    {
        offset[0]--;
    }

    // If the top part of the tunnel is not too close to the bottom part of the
    // tunnel, and there is no mine on the right side of the display or the top
    // part of the tunnel is far enough away from the mine, then move it down
    // 18.75% of the time.
    if ((count > 0xd0000000) && ((offset[0] + gap + offset[1]) < 64) &&
        ((idx == 5) || ((mines[idx][2] - offset[0]) > 1)))
    {
        offset[0]++;
    }

    count = NEXT_RAND(count);

    // If the bottom part of the tunnel is not too close to the top part of the
    // tunnel, and there is no mine on the right side of the display or the
    // bottom part of the tunnel is far enough away from the mine, then move it
    // up 18.75% of the time.
    if ((count < 0x30000000) && ((offset[0] + gap + offset[1]) < 64) &&
        ((idx == 5) || ((offset[1] - mines[idx][2]) > 5)))
    {
        offset[1]++;
    }

    // If the bottom part of the tunnel is not at the bottom of the display,
    // then move it down 18.75% of the time.
    if ((count > 0xd0000000) && (offset[1] != 0))
    {
        offset[1]--;
    }

    // Move the background image one column to the left.
    for (a = 0; a < 8; a++)
    {
        for (count = 0; count < 102; count++)
        {
            background[a][count] = background[a][count + 1];
        }
    }

    // Generate a new column on the right side of the background image.
    for (count = 0; count < 8; count++)
    {
        if (count < offset[0] / 8)
            background[count][101] = 0xFF;
        else if (count == offset[0] / 8)
            background[count][101] = 0xFF << (8 - offset[0] % 8);
        else if (count == 7 - offset[1] / 8)
            background[count][101] = 0xFF >> (8 - offset[1] % 8);
        else if (count > 7 - offset[1] / 8)
            background[count][101] = 0xFF;
        else background[count][101] = 0x00;
    }

    // Copy the background image to the local frame buffer.
    for (a = 0; a < 8; a++)
    {
        for (count = 0; count < 102; count++)
        {
            frame[2 + a * 102 + count] = background[a][count];
        }
    }
}

/***************************************************************************//**
 * @brief Draws a image on the local frame buffer.
 * @param *image  Pointer to the image to add to the buffer
 * @param x   The horizontal position of the image
 * @param y   The vertical position of the image
 * @param width   The width of the image
 * @return none
 ******************************************************************************/

void DrawImage(const uint8_t *image, uint8_t x, uint8_t y, uint8_t width)
{
    uint8_t idx;

    // Loop through the columns of this image.
    for (idx = 0; idx < width; idx++)
    {
        // See if this column is on the display.
        if (((x + idx) >= 0) && ((x + idx) < 102))
        {
            // Add this image to the row(s) of the local frame buffer.
            frame[2 + (y / 8) * 102 + x + idx] |= image[idx] >> (y % 8);
            frame[2 + (y / 8 + 1) * 102 + x + idx] |= image[idx] << (8 - y % 8);
        }
    }
}

/***************************************************************************//**
 * @brief  Update the mines in the tunnel.
 * @param  None
 * @return none
 ******************************************************************************/

void UpdateMines(void)
{
    uint8_t count, max;
    uint32_t idx;

    // The maximum horizontal position of any mine found.
    max = 0;

    // Loop through the five possible mines.
    for (count = 0; count < 5; count++)
    {
        if ((mines[count][0] == -1) && (mines[count][1] == -1) && (mines[count][2] == -1))
        {
            // Skip this mine if it does not exist.
            continue;
        }

        // Move the mine one step to the left
        mines[count][1]--;

        // If the mine is too far off the left edge of the display then disable it.
        if (mines[count][1] == -8)
        {
            mines[count][0] = -1;
            mines[count][1] = -1;
            mines[count][2] = -1;
            continue;
        }

        // See if this mine is furthest to the right so far.
        if ((mines[count][1] + 9) > max)
        {
            // Set the new maximal mine position.
            max = mines[count][1] + 9;
        }

        // See which type of mine this is.
        if (mines[count][0] == 0)
        {
            // Draw mine type one
            DrawImage(Game_mine1, mines[count][1], mines[count][2], 6);
        }
        else
        {
            // Draw mine type twos
            DrawImage(Game_mine2, mines[count][1], mines[count][2], 8);
        }
    }

    // If there are mines too close to the right side of the display then do not
    // place any new mines.
    if (max > 85)
    {
        return;
    }

    // Only place new mines occasionally.
    idx = RandomNumber();
    if (idx >= 0x0c000000)
    {
        return;
    }

    // Try to find an unused mine entry.
    for (count = 0; count < 5; count++)
    {
        if ((mines[count][0] == -1) && (mines[count][1] == -1) && (mines[count][2] == -1))
        {
            break;
        }
    }
    if (count == 5)
    {
        // If all five mines are already in use, then a new mine can not be
        // added.
        return;
    }

    // Choose a random mine type.
    idx = NEXT_RAND(idx);
    mines[count][0] = idx >> 31;
    // The mine starts at the right edge of the display.
    mines[count][1] = 94;
    // Choose a random vertical position
    idx = NEXT_RAND(idx);
    mines[count][2] = offset[0] + idx % (64 - offset[0] - offset[1]);

    if (mines[count][0] == 0)
    {
        // Draw mine type one on the local frame buffer.
        DrawImage(Game_mine1, mines[count][1], mines[count][2], 6);
    }
    else
    {
        // Draw mine type two on the local frame buffer.
        DrawImage(Game_mine2, mines[count][1], mines[count][2], 8);
    }
}

/***************************************************************************//**
 * @brief  Move the missile further to the right, checking for impacts.
 * @param  fire The status of the fire button (0 = not pressed, 1 = pressed)
 * @param  dead The status of the ship (0 = alive, 1 = dead)
 * @return none
 ******************************************************************************/

void UpdateMissile(uint8_t fire, uint8_t dead)
{
    uint8_t bit, x;
    uint16_t pos;

    //Set the x position to zero to indicate that no impact has been detected.
    x = 0;

    // See if a missile is currently in flight.
    if ((missile[0] == -1) && (missile[1] == -1))
    {
        // No missile is in flight, so see if one should be fired.
        if (fire && !dead)
        {
            // Set the horizontal position of a newly fired missile
            missile[0] = 15;

            //Set the vertical position by the position of the ship
            pos = Wheel_getValue();
            if (pos > 0x824)
            {
                //Scale pos for size of screen. pos range is (0~63)
                pos = (0xFFF - pos) / 32;
            }
            else
            {
                pos = pos / 32;
            }

            if (pos > 56)
            {
                // Don't let ship go off bottom of screen.
                pos = 56;
            }

            missile[1] = pos + 8;
        }
        else
        {
            // do nothing because no missile fired or in flight
            return;
        }
    }

    // Move the missile to the right.
    missile[0] += 2;

    // See if the missile has moved off the display.
    if (missile[0] >= 101)
    {
        // The missile is no longer on the display, so remove it
        missile[0] = -1;
        missile[1] = -1;
        // Return without doing anything else.
        return;
    }

    // Compute the bit that contains the missile.
    bit = 0x80 >> (missile[1] % 8);

    // Draw the left most column of the missile and check for an impact.
    frame[2 + (missile[1] / 8) * 102 + (missile[0] + 0)] ^= bit;
    if ((frame[2 + (missile[1] / 8) * 102 + (missile[0] + 0)] & bit) != bit)
    {
        frame[2 + (missile[1] / 8) * 102 + (missile[0] + 0)] |= bit;
        //impact location
        x = missile[0];
    }

    // Draw the middle column of the missile and check for an impact.
    frame[2 + (missile[1] / 8) * 102 + (missile[0] + 1)] ^= bit;
    if ((frame[2 + (missile[1] / 8) * 102 + (missile[0] + 1)] & bit) != bit)
    {
        frame[2 + (missile[1] / 8) * 102 + (missile[0] + 1)] |= bit;
        if (x == 0)
        {
            x = missile[0] + 1;
        }
    }

    // Draw the right column of the missile and check for an impact.  The
    // right column may be off the display, so bypass the check in that
    // case.
    if (missile[0] <= 99)
    {
        frame[2 + (missile[1] / 8) * 102 + (missile[0] + 2)] ^= bit;
        if ((frame[2 + (missile[1] / 8) * 102 + (missile[0] + 2)] & bit) != bit)
        {
            frame[2 + (missile[1] / 8) * 102 + (missile[0] + 2)] |= bit;
            if (x == 0)
            {
                x = missile[0] + 2;
            }
        }
    }

    // See if the missile hit something.
    if (x != 0)
    {
        // Loop through the mines.
        for (bit = 0; bit < 5; bit++)
        {
            // See if the missile hit this mine.
            if ((mines[bit][0] != -1) && (mines[bit][1] <= x) &&
                ((mines[bit][1] + mines[bit][0] + 7) >= x) &&
                (mines[bit][2] <= missile[1]) &&
                ((mines[bit][2] + mines[bit][0] + 7) >= missile[1]))
            {
                // This mine was struck, so remove it from the display.
                mines[bit][0] = -1;
                mines[bit][1] = -1;
                mines[bit][2] = -1;

                if (!dead)
                {
                    score += 25;
                }

                // Stop looking through the mines.
                break;
            }
        }

        // Find an empty entry in the explosion list.
        for (bit = 0; bit < 4; bit++)
        {
            if (explosions[bit][0] == -1)
            {
                break;
            }
        }

        // See if an empty entry was found.
        if (bit != 4)
        {
            // Start an explosion at the point of impact.
            explosions[bit][0] = 0;
            explosions[bit][1] = x;
            explosions[bit][2] = missile[1] - 1;
        }

        // Remove the missile from the display.
        missile[0] = -1;
        missile[1] = -1;
    }
}

/***************************************************************************//**
 * @brief  Draw the ship in its current position and check for impacts.
 * @param  None
 * @return The status of the ship (0 = alive, 1 = dead)
 ******************************************************************************/

uint8_t DrawShip(void)
{
    uint16_t pos;
    uint8_t count, bits;
    uint8_t boom;

    // Convert the wheel position to the position of the ship on the display.
    pos = Wheel_getValue();

    if (pos > 0x824)
    {
        // Scale pos for size of screen. pos range is (0~63)
        pos = (0xFFF - pos) / 32;
    }
    else
    {
        pos = pos / 32;
    }


    if (pos > 56)
    {
        // Don't let ship go off bottom of screen.
        pos = 56;
    }

    // Assume no collisions until one is found.
    boom = 0;

    // Loop through the ten columns of the ship image.
    for (count = 0; count < 10; count++)
    {
        // Find the scan lines in the first row of the ship
        bits = Game_ship[count] >> pos % 8;

        // Set the scan lines in the local frame buffer and check for an impact.
        frame[2 + (pos / 8) * 102 + (count + 5)] ^= bits;
        if ((frame[2 + (pos / 8) * 102 + (count + 5)] & bits) != bits)
        {
            // The ship hit something, so don't draw it.
            frame[2 + (pos / 8) * 102 + (count + 5)] ^= bits;
            boom = 1;
        }

        // Get the scan lines that should be set for the second row of the ship.
        bits = Game_ship[count] << (8 - pos % 8);
        if (bits)
        {
            // Set the scan lines in the local frame buffer and check for an impact.
            frame[2 + (pos / 8 + 1) * 102 + (count + 5)] ^= bits;
            if ((frame[2 + (pos / 8 + 1) * 102 + (count + 5)] & bits) != bits)
            {
                //The ship hit something, so don't draw it.
                frame[2 + (pos / 8 + 1) * 102 + (count + 5)] ^= bits;
                boom = 1;
            }
        }
    }

    // See if an impact occurred.
    if (boom)
    {
        // Start the ship explosion.
        explosions[4][0] = 0;
        explosions[4][1] = 4;
        explosions[4][2] = pos + 3;

        // Indicate that the ship has exploded.
        return (1);
    }
    else
    {
        // The ship survived, so increment the score by one.
        score++;

        // Indicate that the ship is still alive.
        return (0);
    }
}

/***************************************************************************//**
 * @brief  Draws any active explosions.
 * @param  None
 * @return none
 ******************************************************************************/

void DrawExplosions(void)
{
    uint8_t count, idx;

    // Loop through the explosion list.
    for (count = 0; count < 5; count++)
    {
        if (explosions[count][0] == -1)
        {
            // Skip this entry if it is not in use.
            continue;
        }

        // Get the index to the explosion step to display.
        idx = explosions[count][0] >> 2;

        // For all except the last explosion (i.e. the ship explosion), move
        // the explosion to the left to match the movement of the tunnel.
        if (count != 4)
        {
            explosions[count][1]--;
        }

        // Draw the explosion image into the local frame buffer.
        DrawImage(Game_explosion[idx].image,
                  explosions[count][1] - Game_explosion[idx].adjust,
                  explosions[count][2] - Game_explosion[idx].adjust,
                  Game_explosion[idx].width);

        // Increment the explosion type counter.
        explosions[count][0]++;

        if (explosions[count][0] == 16)
        {
            // If the explosion has completed, then remove it from the list.
            explosions[count][0] = -1;
        }
    }
}

/***************************************************************************//**
 * @brief  Main function that plays the game.
 * @param  None
 * @return none
 ******************************************************************************/

void LaunchpadDef(void)
{
    uint32_t idx, a;
    uint8_t dead, timeout;
    uint8_t fire;
    uint8_t restart = 0;
    char scoreString[6];

    buttonsPressed = 0;
    __enable_interrupt();
    Dogs102x6_clearScreen();
    Dogs102x6_imageDraw(image, 0, 0);                      //绘制飞船图案
    Dogs102x6_stringDraw(7, 0, "LaunchpadDefender", 0);

    //等待按键被按下
    while (!buttonsPressed)
    {
        // Wait in low power mode 3 until a button is pressed
        __bis_SR_register(LPM3_bits + GIE);
    }

    if (buttonsPressed & BUTTON_S1)                       //若按下的为S1键，则执行段内程序；若按下的为S2键，则退出
    {
        buttonsPressed = 0;
        while (1)                                         //游戏程序在该段内
        {
            restart = 0;


            // 初始化隧道底部和顶部显示
            offset[0] = 10;
            offset[1] = 10;

            // 关闭所有地雷
            for (idx = 0; idx < 5; idx++)
            {
                mines[idx][0] = -1;
                mines[idx][1] = -1;
                mines[idx][2] = -1;
            }

            // 关闭导弹发射
            missile[0] = -1;
            missile[1] = -1;

            // 关闭所有爆炸
            for (idx = 0; idx < 5; idx++)
            {
                explosions[idx][0] = -1;
            }

            // 设置得分为0
            score = 0;

            // 激活飞船
            dead = 0;
            timeout = 0;

            Dogs102x6_clearScreen();

            // 清除隧道图案缓冲区
            for (a = 0; a < 8; a++)
            {
                for (idx = 0; idx < 102; idx++)
                {
                    background[a][idx] = 0;
                }
            }

            frame[0] = 102;
            frame[1] = 8;

            // Update the tunnel background, making sure that there are at least
            // thirteen scan lines between the top and bottom walls (providing
            // room for the "Press Button To Play" text).
            UpdateBackground(40);                       //更新隧道图案缓冲区，每次调用该函数，使隧道图案向右滚动一列
            Dogs102x6_imageDraw((uint8_t*)frame, 0, 0); //更新画面，进行显示
            buttonsPressed = 0;

            while (!restart)
            {
                // Update the tunnel.  The tunnel gets smaller as the score goes
                // up.
                UpdateBackground(30 - (score / 500));  //更新隧道图案缓冲区，每次调用该函数，使隧道图案向右滚动一列
                UpdateMines();                         //更新隧道地雷
                if (buttonsPressed) fire = 1;          //如果有按键按下，发射导弹
                else fire = 0;
                buttonsPressed = 0;
                if (dead)                              //如果飞船死亡，进行超时累加，为绘制飞船死亡爆炸提供时间
                {
                    timeout++;
                }
                if (!dead)                             //若飞船没有死亡
                {
                    dead = DrawShip();                 //绘制飞船图案，并返回飞船死亡状态
                    UpdateMissile(fire, dead);         //更新导弹
                }
                DrawExplosions();                      //绘制爆炸图案
                RandomAddEntropy(score);               //存储得分

                Dogs102x6_imageDraw((uint8_t*)frame, 0, 0);     //更新画面，进行显示。每次只调用该函数时，进行显示图案的更改

                if (timeout == 32)                     //若超时计数器达到32，则表示飞船死亡爆炸已绘制完成，进行显示说明及得分
                {
                    Dogs102x6_setInverseDisplay();
                    __delay_cycles(6000000);
                    Dogs102x6_clearInverseDisplay();
                    __delay_cycles(6000000);
                    Dogs102x6_setInverseDisplay();
                    __delay_cycles(6000000);
                    Dogs102x6_clearInverseDisplay();

                    scoreString[0] = '0' + ((score / 10000) % 10);
                    scoreString[1] = '0' + ((score / 1000) % 10);
                    scoreString[2] = '0' + ((score / 100) % 10);
                    scoreString[3] = '0' + ((score / 10) % 10);
                    scoreString[4] = '0' + (score % 10);
                    scoreString[5] = '\0';

                    Dogs102x6_clearRow(5);
                    Dogs102x6_clearRow(4);
                    Dogs102x6_clearRow(3);

                    Dogs102x6_stringDraw(2, 0, " Score:          ", DOGS102x6_DRAW_INVERT);
                    Dogs102x6_stringDraw(2, 48, scoreString, DOGS102x6_DRAW_INVERT);
                    Dogs102x6_stringDraw(3, 0, " S2: QUIT        ", DOGS102x6_DRAW_INVERT);
                    Dogs102x6_stringDraw(4, 0, " S1: PLAY AGAIN  ", DOGS102x6_DRAW_INVERT);

                    //wait for button press
                    while (!buttonsPressed)
                    {
                        // Wait in low power mode 3 until a button is pressed
                        __bis_SR_register(LPM3_bits + GIE);
                    }

                    restart = 1;                           //退出游戏更新内循环
                }

                //调整游戏速度：分数越高，游戏运行速度越快
                if (score < 500)
                    __delay_cycles(500000);
                else if (score < 1000)
                    __delay_cycles(400000);
                else if (score < 2000)
                    __delay_cycles(300000);
                else if (score < 3000)
                    __delay_cycles(200000);
                else
                    __delay_cycles(100000);

            }
            if (buttonsPressed & BUTTON_S2)               //若S2按键按下，则退出游戏；若按下S1键，则继续下一轮游戏
            {
                Dogs102x6_clearScreen();
                buttonsPressed = 0;
                break;
            }
        }
    }
    else
    {
        buttonsPressed = 0;
        Dogs102x6_clearScreen();
    }
}

//*****************************************************************************

char *strrev(char *str)
{
    char *p1, *p2;

    if (!str || !*str)
        return str;

    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }

    return str;
}

char *itoa(int n, char *s, int b)
{
    static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i = 0, sign;

    if ((sign = n) < 0)
        n = -n;

    do
    {
        s[i++] = digits[n % b];
    } while ((n /= b) > 0);

    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';

    return strrev(s);
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
            USB_disable();                              // Disable
    }
}

