/*******************************************************************************
 *
 *  random.c - Random number generator utilizing MD4 hash function of
 *             environmental noise captured via the the potentiometer.
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

#include "Random.h"

//*****************************************************************************
//
// The pool of entropy that has been collected.
//
//*****************************************************************************
static unsigned long g_pulEntropy[16];

//*****************************************************************************
//
// The index of the next byte to be added to the entropy pool.
//
//*****************************************************************************
static unsigned long g_ulIndex = 0;

//*****************************************************************************
//
// Add entropy to the pool.
//
//*****************************************************************************
void
RandomAddEntropy(unsigned long ulEntropy)
{
    //
    // Add this byte to the entropy pool.
    //
    ((unsigned char *)g_pulEntropy)[g_ulIndex] = ulEntropy & 0xff;

    //
    // Increment to the next byte of the entropy pool.
    //
    g_ulIndex = (g_ulIndex + 1) & 63;
}

//*****************************************************************************
//
// Generate a random number by running a MD4 hash on the entropy pool.  Note
// that the entropy pool may change from beneath us, but for the purposes of
// generating random numbers that is not a concern.  Also, the MD4 hash was
// broken long ago, but since it is being used to generate random numbers
// instead of providing security this is not a concern.
//
//*****************************************************************************
unsigned long
RandomNumber(void)
{
    unsigned long ulA, ulB, ulC, ulD, ulTemp, ulIdx;

    //
    // Initialize the digest.
    //
    ulA = 0x67452301;
    ulB = 0xefcdab89;
    ulC = 0x98badcfe;
    ulD = 0x10325476;

    //
    // Perform the first round of operations.
    //
#define F(a, b, c, d, k, s)                                 \
    {                                                       \
        ulTemp = a + (d ^ (b & (c ^ d))) + g_pulEntropy[k]; \
        a = (ulTemp << s) | (ulTemp >> (32 - s));           \
    }
    for (ulIdx = 0; ulIdx < 16; ulIdx += 4)
    {
        F(ulA, ulB, ulC, ulD, ulIdx + 0, 3);
        F(ulD, ulA, ulB, ulC, ulIdx + 1, 7);
        F(ulC, ulD, ulA, ulB, ulIdx + 2, 11);
        F(ulB, ulC, ulD, ulA, ulIdx + 3, 19);
    }

    //
    // Perform the second round of operations.
    //
#define G(a, b, c, d, k, s)                                            \
    {                                                                  \
        ulTemp = a + ((b & c) | (b & d) | (c & d)) + g_pulEntropy[k] + \
            0x5a827999;                                           \
        a = (ulTemp << s) | (ulTemp >> (32 - s));                      \
    }
    for (ulIdx = 0; ulIdx < 4; ulIdx++)
    {
        G(ulA, ulB, ulC, ulD, ulIdx + 0, 3);
        G(ulD, ulA, ulB, ulC, ulIdx + 4, 5);
        G(ulC, ulD, ulA, ulB, ulIdx + 8, 9);
        G(ulB, ulC, ulD, ulA, ulIdx + 12, 13);
    }

    //
    // Perform the third round of operations.
    //
#define H(a, b, c, d, k, s)                                      \
    {                                                            \
        ulTemp = a + (b ^ c ^ d) + g_pulEntropy[k] + 0x6ed9eba1; \
        a = (ulTemp << s) | (ulTemp >> (32 - s));                \
    }
    for (ulIdx = 0; ulIdx < 4; ulIdx += 2)
    {
        H(ulA, ulB, ulC, ulD, ulIdx + 0, 3);
        H(ulD, ulA, ulB, ulC, ulIdx + 8, 9);
        H(ulC, ulD, ulA, ulB, ulIdx + 4, 11);
        H(ulB, ulC, ulD, ulA, ulIdx + 12, 15);

        if (ulIdx == 2)
        {
            ulIdx -= 3;
        }
    }

    //
    // Return the first word of the resulting digest as the random number.
    //
    return (ulA + 0x67452301);
}

