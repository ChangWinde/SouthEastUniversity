/*******************************************************************************
 *
 *  Demo_Cube.c - Code for using displaying a rotating Cube on the LCD
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
 * @file       Demo_Cube.c
 * @addtogroup Demo_Cube
 * @{
 ******************************************************************************/


#include <stdio.h>
//#include <string.h>
#include <stdint.h>

// Custom Header
#include "msp430.h"
#include "hal_UCS.h"
#include "HAL_Buttons.h"
#include "HAL_Dogs102x6.h"
#include "HAL_Cma3000.h"
#include "HAL_Wheel.h"
#include "Random.h"

#define CUBE_SIZE (15)

#define XOFFSET DOGS102x6_X_SIZE / 2 - 1
#define YOFFSET DOGS102x6_Y_SIZE / 2 - 1

#define SPEED 1


// macro defintion
#define CONV_SFP32_TO_S16(n)    ((short)((n) >> 16))
#define MUL_SFP32(m, n)           (((m) >> 8) * ((n) >> 8))

typedef     short sfp16;              //1:7:8 fixed point
typedef     long sfp32;               //1:19:8 fixed point
typedef     unsigned short ufp16;     //8:8 fixed point
typedef     unsigned long ufp32;      //24:8 fixed point

#define FIXED      long
#define CUBE_MAX   (12)

typedef struct Tcube_data {
    unsigned int x0;
    unsigned int x1;
    unsigned int y0;
    unsigned int y1;
}Tcube_data;

struct Tcube_data cube_data[CUBE_MAX];
struct Tcube_data cube_data_last[CUBE_MAX];

// type struct defintion
typedef struct TVector3D
{
    FIXED x, y, z;
}TVector3D;


// Constant defintion
#define ONE 65535 * CUBE_SIZE

const TVector3D Cube3D[CUBE_MAX][2] =
{ {{-ONE, -ONE, -ONE}, { ONE, -ONE, -ONE}}, //  0        .-----2----.
  {{-ONE, -ONE, -ONE}, {-ONE, ONE, -ONE}},  //  1       /|         /|
  {{-ONE, ONE, -ONE}, { ONE, ONE, -ONE}},   //  2      9 |        A |
  {{ ONE, -ONE, -ONE}, { ONE, ONE, -ONE}},  //  3     /  |       /  |
  {{-ONE, -ONE, ONE}, { ONE, -ONE, ONE}},   //  4    .------6---.   3
  {{-ONE, -ONE, ONE}, {-ONE, ONE, ONE}},    //  5    |   |      |   |
  {{-ONE, ONE, ONE}, { ONE, ONE, ONE}},     //  6    |   1      7   |
  {{ ONE, -ONE, ONE}, { ONE, ONE, ONE}},    //  7    |   |      |   |
  {{-ONE, -ONE, ONE}, {-ONE, -ONE, -ONE}},  //  8    5   '----0-|---'
  {{-ONE, ONE, ONE}, {-ONE, ONE, -ONE}},    //  9    |  /       |  /
  {{ ONE, ONE, ONE}, { ONE, ONE, -ONE}},    //  A    | 8        | B
  {{ ONE, -ONE, ONE}, { ONE, -ONE, -ONE}} //  B    |/____4____|/
};

#define SIN(angle)  (angle < 180) ? FIXED_SIN[angle] : -FIXED_SIN[angle - 180]
#define COS(angle)  ((angle + 90) < 180) ? FIXED_SIN[(angle + 90)] : -FIXED_SIN[(angle + 90) - 180]

const FIXED FIXED_SIN[270] =
{
    0, 1143, 2287, 3429, 4571, 5711, 6850, 7986, 9120, 10252,
    11380, 12504, 13625, 14742, 15854, 16961, 18064, 19160, 20251, 21336,
    22414, 23486, 24550, 25606, 26655, 27696, 28729, 29752, 30767, 31772,
    32768, 33753, 34728, 35693, 36647, 37589, 38521, 39440, 40347, 41243,
    42125, 42995, 43852, 44695, 45525, 46340, 47142, 47930, 48702, 49460,
    50203, 50931, 51643, 52339, 53019, 53683, 54331, 54963, 55577, 56175,
    56755, 57319, 57864, 58393, 58903, 59395, 59870, 60326, 60763, 61183,
    61583, 61965, 62328, 62672, 62997, 63302, 63589, 63856, 64103, 64331,
    64540, 64729, 64898, 65047, 65176, 65286, 65376, 65446, 65496, 65526,
    65536, 65526, 65496, 65446, 65376, 65286, 65176, 65047, 64898, 64729,
    64540, 64331, 64103, 63856, 63589, 63302, 62997, 62672, 62328, 61965,
    61583, 61183, 60763, 60326, 59870, 59395, 58903, 58393, 57864, 57319,
    56755, 56175, 55577, 54963, 54331, 53683, 53019, 52339, 51643, 50931,
    50203, 49460, 48702, 47930, 47142, 46340, 45525, 44695, 43852, 42995,
    42125, 41243, 40348, 39440, 38521, 37589, 36647, 35693, 34728, 33753,
    32768, 31772, 30767, 29752, 28729, 27696, 26655, 25606, 24550, 23486,
    22414, 21336, 20251, 19160, 18064, 16961, 15854, 14742, 13625, 12504,
    11380, 10252, 9120, 7986, 6850, 5711, 4571, 3429, 2287, 1143,

    0, -1143, -2287, -3429, -4571, -5711, -6850, -7986, -9120, -10252,
    -11380, -12504, -13625, -14742, -15854, -16961, -18064, -19160, -20251, -21336,
    -22414, -23486, -24550, -25606, -26655, -27696, -28729, -29752, -30767, -31772,
    -32768, -33753, -34728, -35693, -36647, -37589, -38521, -39440, -40347, -41243,
    -42125, -42995, -43852, -44695, -45525, -46340, -47142, -47930, -48702, -49460,
    -50203, -50931, -51643, -52339, -53019, -53683, -54331, -54963, -55577, -56175,
    -56755, -57319, -57864, -58392, -58903, -59395, -59870, -60326, -60763, -61183,
    -61583, -61965, -62328, -62672, -62997, -63302, -63589, -63856, -64103, -64331,
    -64540, -64729, -64898, -65047, -65176, -65286, -65376, -65446, -65496, -65526,
};




// global variables
TVector3D g_RotatedCube3D[CUBE_MAX][2]; // This will hold the rotated points of the cube


// Prototypes
void RotateCube(int16_t x, int16_t y, int16_t z);
void DrawCube(char cubetype);
void Rotate3DPointXYZ(TVector3D *pPointToRotate, TVector3D *pRotatedPoint, int AngleX, int AngleY,
                      int AngleZ);
void Demo_Cube1(char cubetype);

void Demo_Cube(void)
{
    unsigned int mode = 0;

    memset(cube_data_last, 0, sizeof(cube_data_last));

    Cma3000_init();                       // Initialize accelerometer


    // Write text on LCD
    buttonsPressed = 0;
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(0, 0, "Press S1 to swich", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(1, 0, "mode.            ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(2, 0, "Press button S2  ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(3, 0, "to exit.         ", DOGS102x6_DRAW_NORMAL);
    Dogs102x6_stringDraw(6, 0, "Press button S1  ", DOGS102x6_DRAW_INVERT);
    Dogs102x6_stringDraw(7, 0, "to start.        ", DOGS102x6_DRAW_INVERT);
    while (!(buttonsPressed & BUTTON_S1)) ;


    buttonsPressed = 0;
    Dogs102x6_clearScreen();
    Dogs102x6_refresh(DOGS102x6_DRAW_ON_REFRESH);

    while (!(buttonsPressed & BUTTON_S2))
    {
        if (buttonsPressed & BUTTON_S1){
            mode++;
            if (mode > 2) mode = 0;
            buttonsPressed = 0;
        }
        Demo_Cube1(mode);
    }
    Dogs102x6_clearScreen();
    Dogs102x6_refresh(DOGS102x6_DRAW_IMMEDIATE);
    buttonsPressed = 0;
}

void Demo_Cube1(char cubetype)
{
    if (cubetype & 0x02){
        // Get the acceleration in x- and y-direction and call event function
        // read values out of accelerometer with removed offset
        Cma3000_readAccel_offset();
        RotateCube((Cma3000_xAccel + 1) >> 3, (Cma3000_yAccel + 1) >> 3, (Cma3000_zAccel + 1) >> 3);
    } else {
        RotateCube(SPEED, SPEED, SPEED);
    }
    DrawCube(cubetype & 0x01);
}

/*****************************************************************************************
 *
 * DrawCube
 *
 * This draws the rotated 3d cube.
 *
 ****************************************************************************************/

void DrawCube(char cubetype)
{
    int i;
    int x0, y0, x1, y1;
    static char lastLineCount = 0;
    char lineCount = 0;

    ////////////////////////////////
    //find out the cube edges to hide
    //find the farthest z-point of the cube
    FIXED zMin = g_RotatedCube3D[0][0].z;

    if (cubetype)
    {
        for (i = 0; i < CUBE_MAX; i++)
        {
            if (g_RotatedCube3D[i][0].z < zMin) zMin = g_RotatedCube3D[i][0].z;
            if (g_RotatedCube3D[i][1].z < zMin) zMin = g_RotatedCube3D[i][1].z;
        }
    }
    for (i = 0; i < CUBE_MAX; i++)
    {
        //if the line includes the farthest point? - so skip it!
        if (cubetype)
            if ((zMin == g_RotatedCube3D[i][0].z) || (zMin == g_RotatedCube3D[i][1].z))
            {
                continue;
            }

        // Get rotated coordinate for start of line
        x0 = CONV_SFP32_TO_S16((g_RotatedCube3D[i][0].x)) + XOFFSET;
        y0 = CONV_SFP32_TO_S16((g_RotatedCube3D[i][0].y)) + YOFFSET;

        // Get rotated coordinate for end of line
        x1 = CONV_SFP32_TO_S16((g_RotatedCube3D[i][1].x)) + XOFFSET;
        y1 = CONV_SFP32_TO_S16((g_RotatedCube3D[i][1].y)) + YOFFSET;

        // make sure x0, x1 isnt greater than LCD_X_SIZE
        // or less than 0.
        if (x0 >= DOGS102x6_X_SIZE) x0 = DOGS102x6_X_SIZE - 1;
        if (x0 < 1) x0 = 1;
        if (x1 >= DOGS102x6_X_SIZE) x1 = DOGS102x6_X_SIZE - 1;
        if (x1 < 1) x1 = 1;

        // make sure y0, y1 isnt greater than LCD_Y_SIZE
        // or less than 0.
        if (y0 >= DOGS102x6_Y_SIZE) y0 = DOGS102x6_Y_SIZE - 1;
        if (y0 < 1) y0 = 1;
        if (y1 >= DOGS102x6_Y_SIZE) y1 = DOGS102x6_Y_SIZE - 1;
        if (y1 < 1) y1 = 1;
        cube_data[lineCount].x0 = x0;
        cube_data[lineCount].x1 = x1;
        cube_data[lineCount].y0 = y0;
        cube_data[lineCount].y1 = y1;

        lineCount++;
    }

    Dogs102x6_clearScreen();

    for (i = 0; i < lineCount; i++)
        Dogs102x6_lineDraw(cube_data[i].x0, cube_data[i].y0, cube_data[i].x1, cube_data[i].y1,
                           DOGS102x6_DRAW_NORMAL);

    Dogs102x6_horizontalLineDraw(0, DOGS102x6_X_SIZE,                0, DOGS102x6_DRAW_NORMAL);
    Dogs102x6_horizontalLineDraw(0, DOGS102x6_X_SIZE, DOGS102x6_Y_SIZE, DOGS102x6_DRAW_NORMAL);
    Dogs102x6_verticalLineDraw(0, DOGS102x6_Y_SIZE,                0, DOGS102x6_DRAW_NORMAL);
    Dogs102x6_verticalLineDraw(0, DOGS102x6_Y_SIZE, DOGS102x6_X_SIZE, DOGS102x6_DRAW_NORMAL);
    Dogs102x6_refresh(DOGS102x6_DRAW_ON_REFRESH);
}

/*****************************************************************************************
 *
 * RotateCube
 *
 * Rotate the 3d cube
 *
 ****************************************************************************************/

void RotateCube(int16_t x, int16_t y, int16_t z)
{

    int i;
    static int16_t AngleX = 90, AngleY = 45, AngleZ = 180;

    // Change angles
    if ((AngleX += x) > 359) AngleX = 0;
    if ((AngleY += y) > 359) AngleY = 0;
    if ((AngleZ += z) > 359) AngleZ = 0;

    if ((AngleX) < 0) AngleX = 360 + AngleX;  // Value is negative, so we need to add
    if ((AngleY) < 0) AngleY = 360 + AngleY;
    if ((AngleZ) < 0) AngleZ = 360 + AngleZ;

    for (i = 0; i < CUBE_MAX; i++)
    {
        Rotate3DPointXYZ((TVector3D*)&Cube3D[i][0], &g_RotatedCube3D[i][0], AngleX, AngleY, AngleZ);
        Rotate3DPointXYZ((TVector3D*)&Cube3D[i][1], &g_RotatedCube3D[i][1], AngleX, AngleY, AngleZ);
    }
}

/*****************************************************************************************
 *
 * Rotate3DPointXYZ
 *
 * Rotate a 3d point and returns the rotated 3d point.
 *
 *    pPointToRotate    :     3d point to rotate
 *    AngleX            :     Angle in degrees to rotate the point around x axis
 *    AngleY            :     Angle in degrees to rotate the point around y axis
 *    AngleZ            :     Angle in degrees to rotate the point around z axis
 *
 * Returns:
 *    The rotated point
 *
 ****************************************************************************************/

void Rotate3DPointXYZ(TVector3D *pPointToRotate, TVector3D *pRotatedPoint, int16_t AngleX,
                      int16_t AngleY,
                      int16_t AngleZ)
{
    FIXED xtemp, ytemp;
    TVector3D *pPt = pRotatedPoint; // pointer to our rotated point

    *pRotatedPoint = *pPointToRotate;

    // Rotate Z //
    xtemp  = MUL_SFP32(pPt->x, COS(AngleZ)) - MUL_SFP32(pPt->y, SIN(AngleZ));
    pPt->y = MUL_SFP32(pPt->x, SIN(AngleZ)) + MUL_SFP32(pPt->y, COS(AngleZ));
    pPt->x = xtemp;

    // Rotate X //
    xtemp  = MUL_SFP32(pPt->x, COS(AngleX)) - MUL_SFP32(pPt->z, SIN(AngleX));
    pPt->z = MUL_SFP32(pPt->x, SIN(AngleX)) + MUL_SFP32(pPt->z, COS(AngleX));
    pPt->x = xtemp;

    //   // Rotate Y //
    ytemp  = MUL_SFP32(pPt->y, COS(AngleY)) - MUL_SFP32(pPt->z, SIN(AngleY));
    pPt->z = MUL_SFP32(pPt->y, SIN(AngleY)) + MUL_SFP32(pPt->z, COS(AngleY));
    pPt->y = ytemp;
}

