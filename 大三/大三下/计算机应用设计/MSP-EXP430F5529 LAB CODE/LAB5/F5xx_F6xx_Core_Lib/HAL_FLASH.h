/*******************************************************************************
 *
 * HAL_FLASH.h
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

#ifndef HAL_FLASH_H
#define HAL_FLASH_H

#include <stdint.h>

//******************************************************************************
// Defines
//******************************************************************************

#define FLASH_STATUS_OK     0
#define FLASH_STATUS_ERROR  1

/*******************************************************************************
 * \brief   Erase a single segment of the flash memory
 *
 * \param *Flash_ptr    Pointer into the flash segment to erase
 ******************************************************************************/
extern void Flash_SegmentErase(uint16_t *Flash_ptr);

/*******************************************************************************
 * \brief   Erase Check of the flash memory
 *
 * \param *Flash_ptr    Pointer into the flash segment to erase
 * \param len           give the len in word
 ******************************************************************************/
extern uint8_t Flash_EraseCheck(uint16_t *Flash_ptr, uint16_t len);

/*******************************************************************************
 * \brief   Write data into the flash memory (Byte format)
 *
 * \param *Data_ptr     Pointer to the Data to write
 * \param *Flash_ptr    Pointer into the flash to write data to
 * \param count         number of data to write
 ******************************************************************************/
extern void FlashWrite_8(uint8_t *Data_ptr, uint8_t *Flash_ptr, uint16_t count);

/*******************************************************************************
 * \brief   Write data into the flash memory (Word format)
 *
 * \param *Data_ptr     Pointer to the Data to write
 * \param *Flash_ptr    Pointer into the flash to write data to
 * \param count         number of data to write
 ******************************************************************************/
extern void FlashWrite_16(uint16_t *Data_ptr, uint16_t *Flash_ptr, uint16_t count);

/*******************************************************************************
 * \brief   Write data into the flash memory (Long format)
 *
 * \param *Data_ptr     Pointer to the Data to write
 * \param *Flash_ptr    Pointer into the flash to write data to
 * \param count         number of data to write
 ******************************************************************************/
extern void FlashWrite_32(uint32_t *Data_ptr, uint32_t *Flash_ptr, uint16_t count);

/*******************************************************************************
 * \brief   Fill data into the flash memory (Long format)
 *
 * \param value         Pointer to the Data to write
 * \param *Flash_ptr    pointer into the flash to write data to
 * \param count         number of data to write (= byte * 4)
 ******************************************************************************/
extern void FlashMemoryFill_32(uint32_t value, uint32_t *Flash_ptr, uint16_t count);

#endif /* HAL_FLASH_H */
