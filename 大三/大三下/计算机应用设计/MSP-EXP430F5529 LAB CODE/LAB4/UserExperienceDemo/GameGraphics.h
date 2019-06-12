/*******************************************************************************
 *
 *  GameGraphics.h
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

#ifndef GAME_GRAPHICS_H_
#define GAME_GRAPHICS_H_

#define MAX_IMAGE     7


//*****************************************************************************
//
// A bitmap for the space ship.
//
//*****************************************************************************
const uint8_t Game_ship[] = {
    0x3F, 0x1F, 0x1F, 0x0F, 0x0F, 0x07, 0x07, 0x03, 0x03, 0x01
};

//*****************************************************************************
//
// A bitmap for mine type one.
//
//*****************************************************************************
const uint8_t Game_mine1[] = {
    0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18
};

//*****************************************************************************
//
// A bitmap for mine type two.
//
//*****************************************************************************
const uint8_t Game_mine2[] =
{
    0xC3, 0xC3, 0x3C, 0x3C, 0x3C, 0x3C, 0xC3, 0xC3
};

//*****************************************************************************
//
// A bitmap for the first stage of an explosion
//
//*****************************************************************************
const uint8_t Game_explosion1[] =
{
    0x28, 0x10, 0x28
};

//*****************************************************************************
//
// A bitmap for the second stage of an explosion
//
//*****************************************************************************
const uint8_t Game_explosion2[] =
{
    0x44, 0x28, 0x10, 0x28, 0x44
};

const uint8_t Game_explosion3[] =
{
    0x92, 0x54, 0x28, 0xD6, 0x28, 0x54, 0x92
};

const uint8_t Game_explosion4[] =
{
    0x10, 0x92, 0x54, 0x00, 0xD7, 0x00, 0x54, 0x92, 0x10,
};

//*****************************************************************************
//
// This array contains the sequence of explosion images, along with the width
// of each one.
//
//*****************************************************************************
const struct
{
    const uint8_t *image;
    unsigned int adjust;
    unsigned int width;
}

Game_explosion[4] =
{
    { Game_explosion1, 0, 3 },
    { Game_explosion2, 1, 5 },
    { Game_explosion3, 2, 7 },
    { Game_explosion4, 3, 9 }
};

#endif /*GAME_GRAPHICS_H_*/
