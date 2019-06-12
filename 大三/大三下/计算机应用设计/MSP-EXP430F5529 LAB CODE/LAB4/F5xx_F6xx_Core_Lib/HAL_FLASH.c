/*******************************************************************************
 *
 * HAL_FLASH.c
 * Flash Library for flash memory controller of MSP430F5xx/6xx family
 * 
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
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
 * Created: Version 1.0 11/24/2009
 * Updated: Version 2.0 01/18/2011
 *  
 ******************************************************************************/
 
#include "msp430.h"
#include "HAL_FLASH.h"

void Flash_SegmentErase(uint16_t *Flash_ptr)
{
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash seg
  while (FCTL3 & BUSY);                     // test busy
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

uint8_t Flash_EraseCheck(uint16_t *Flash_ptr, uint16_t len)
{
  uint16_t i;
  
  for (i = 0; i < len; i++) {               // was erasing successfull?
    if (*(Flash_ptr + i) != 0xFF) {
      return FLASH_STATUS_ERROR;
    }
  }

  return FLASH_STATUS_OK;
}

void FlashWrite_8(uint8_t *Data_ptr, uint8_t *Flash_ptr, uint16_t count)
{
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+WRT;                        // Enable byte/word write mode
  
  while (count > 0) {
    while (FCTL3 & BUSY);                   // test busy
    *Flash_ptr++ = *Data_ptr++;             // Write to Flash
    count--;
  }

  FCTL1 = FWKEY;                            // Clear write bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

void FlashWrite_16(uint16_t *Data_ptr, uint16_t *Flash_ptr, uint16_t count)
{
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+WRT;                        // Enable byte/word write mode
  
  while (count > 0) {
    while (FCTL3 & BUSY);                   // test busy
    *Flash_ptr++ = *Data_ptr++;             // Write to Flash
    count--;
  }

  FCTL1 = FWKEY;                            // Clear Erase bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

void FlashWrite_32(uint32_t *Data_ptr, uint32_t *Flash_ptr, uint16_t count)
{
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + BLKWRT;                   // Enable long-word write
  
  while (count > 0) {
    while (FCTL3 & BUSY);                   // test busy
    *Flash_ptr++ = *Data_ptr++;             // Write to Flash
    count--;
  }

  FCTL1 = FWKEY;                            // Clear Erase bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

void FlashMemoryFill_32(uint32_t value, uint32_t *Flash_ptr, uint16_t count)
{
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + BLKWRT;                   // Enable long-word write

  while (count > 0) {
    while (FCTL3 & BUSY);                   // test busy
    *Flash_ptr++ = value;                   // Write to Flash
    count--;
  }

  FCTL1 = FWKEY;                            // Clear Erase bit
  FCTL3 = FWKEY + LOCK;                      // Set LOCK bit
}
