/*******************************************************************************
 *
 *  MassStorage.c - Uses the USB MSC stack
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

#include <stdint.h>
#include <string.h>
#include "msp430.h"
#include "HAL_PMM.h"
#include "HAL_UCS.h"
#include "HAL_Board.h"
#include "HAL_Buttons.h"
#include "HAL_Dogs102x6.h"
#include "mmc.h"
#include "diskio.h"                         // Low level disk interface module from the FatFs
                                            // package
#include "device.h"
#include "types.h"                          // Basic Type declarations
#include "descriptors.h"
#include "usb.h"                            // USB-specific functions
#include "USB_MSC_API\UsbMscScsi.h"
#include "USB_MSC_API\UsbMsc.h"
#include "USB_MSC_API\UsbMscStateMachine.h"
#include "UsbMscUser.h"
#include "HAL_FLASH.h"
#include "MassStorageCommon.h"

extern uint8_t bDetectCard;

BYTE checkInsertionRemoval(void);

// The number of bytes per block.  In FAT, this is 512
const WORD BYTES_PER_BLOCK = 512;

// Data-exchange buffer between the API and the application.  The application
// allocates it, and then registers it with the API.  Later, the API will pass
// it back to the application when it needs the application to "process" it
// (exchange data with the media).
__no_init BYTE RW_dataBuf[512 * 2];

// The data block of the storage volume, held in RAM
extern BYTE Data[];

// The index of the next available byte to be written within Data[]
WORD DataCnt = 0;

// Holds an incrementing character to be written into each new data log entry
BYTE logCnt = 0x30;

// The API allocates an instance of structure type USBMSC_RWbuf_Info to hold all
// information describing buffers to be processed.  It is a shared resource
// between the API and application.  It will be initialized by calling
// USBMSC_fetchInfoStruct().
USBMSC_RWbuf_Info  *RWbuf_info;

// Data-exchange buffer between the API and the application.  The application
// allocates it, and then registers it with the API.  Later, the API will pass
// it back to the application when it needs the application to "process" it
// (exchange data with the media).
uint8_t RWbuf[512];

// The application must tell the API about the media.  This information is
// conveyed in a call to USBMSC_updateMediaInfo(), passing an instance of
// USBMSC_RWbuf_Info.  Since SD-cards are removable, this might need to occur
// at any time, so this is made global.
struct USBMSC_mediaInfoStr mediaInfo;

void msc_Init(void)
{
    // SD-cards must go through a setup sequence after powerup.  This FatFs
    // call does this.
    disk_initialize(0);

    // The API maintains an instance of the USBMSC_RWbuf_Info structure.  If
    // double-buffering were used, it would maintain one for both the X and
    // Y side.  (This version of the API only supports single-buffering,
    // so only one structure is maintained.)  This is a shared resource between
    // the API and application; the application must request the pointers.
    RWbuf_info = USBMSC_fetchInfoStruct();

    // LUN0
    if (detectCard())
        mediaInfo.mediaPresent = kUSBMSC_MEDIA_PRESENT;
    else mediaInfo.mediaPresent = kUSBMSC_MEDIA_NOT_PRESENT;
    mediaInfo.mediaChanged = 0x00;
    mediaInfo.writeProtected = 0x00;
    // Returns the number of blocks (sectors) in the media.
    disk_ioctl(0, GET_SECTOR_COUNT, &mediaInfo.lastBlockLba);
    // Block size will always be 512
    mediaInfo.bytesPerBlock = BYTES_PER_BLOCK;
    USBMSC_updateMediaInfo(0, &mediaInfo);
    
        // The data interchange buffer (used when handling SCSI READ/WRITE) is
    // declared by the application, and registered with the API using this
    // function.  This allows it to be assigned dynamically, giving
    // the application more control over memory management.
    USBMSC_registerBufInfo(0, &RW_dataBuf[0], NULL, sizeof(RW_dataBuf));
}

void msc_Loop(void)
{
    // Call USBMSC_poll() to initiate handling of any received SCSI commands.
    // Disable interrupts during this function, to avoid conflicts arising from
    // SCSI commands being received from the host AFTER decision to enter LPM is
    // made, but BEFORE it's actually entered (in other words, avoid sleeping
    // accidentally).
    __disable_interrupt();
    if ((USBMSC_poll() == kUSBMSC_okToSleep) && (!bDetectCard))
    {
        // Enable interrupts atomically with LPM0 entry
        __bis_SR_register(LPM0_bits + GIE);
    }
    __enable_interrupt();

    // If the API needs the application to process a buffer, it will keep the
    // CPU awake by returning kUSBMSC_processBuffer from USBMSC_poll().
    // The application should then check the 'operation' field of all defined
    // USBMSC_RWbuf_Info structure instances.  If any of them is non-null, then
    // an operation needs to be processed.  A value of kUSBMSC_READ indicates
    // the API is waiting for the application to fetch data from the storage
    // volume, in response to a SCSI READ command from the USB host.  After the
    // application does this, it must indicate whether the operation succeeded,
    // and then close the buffer operation by calling USBMSC_bufferProcessed().
    while (RWbuf_info->operation == kUSBMSC_READ){
        read_LUN0();
    }

    // Everything in this section is analogous to READs.  Reference the comments
    // above.
    while (RWbuf_info->operation == kUSBMSC_WRITE){
        write_LUN0();
    }

    // Every second, the Timer_A ISR sets this flag.  The checking can't be done
    // from within the timer ISR, because the checking enables interrupts, and
    // this is not a recommended practice due to the risk of nested interrupts.
    if (bDetectCard){
        checkInsertionRemoval();
        // Clear the flag, until the next timer ISR
        bDetectCard = 0x00;
    }
}

void ClockUSB(void)
{
    if (USB_PLL_XT == 2)
    {
        // Enable the XT2 pins.
        // Without this, the xtal pins default to being I/O's.
        P5SEL |= 0x0C;

        // Use the REFO oscillator to source the FLL and ACLK
        UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (SELREF__REFOCLK);
        UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);

        // MCLK will be driven by the FLL (not by XT2), referenced to the REFO
        SFRIE1 &= ~OFIE;
        // Start the FLL, at the freq indicated by the config
        // constant USB_MCLK_FREQ
        Init_FLL_Settle(USB_MCLK_FREQ / 1000, USB_MCLK_FREQ / 32768);

        // Start the "USB crystal"
        XT2_Start(XT2DRIVE_0);
        SFRIE1 |= OFIE;
    }
    else
    {
        // Enable the XT1 pins.
        // Without this, the xtal pins default to being I/O's.
        P5SEL |= 0x10;

        // Use the REFO oscillator to source the FLL and ACLK
        UCSCTL3 = SELREF__REFOCLK;
        UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);

        SFRIE1 &= ~OFIE;
        // MCLK will be driven by the FLL (not by XT2), referenced to the REFO
        // set FLL (DCOCLK)
        Init_FLL_Settle(USB_MCLK_FREQ / 1000, USB_MCLK_FREQ / 32768);

        // Start the "USB crystal"
        XT1_Start(XT1DRIVE_0);
        SFRIE1 |= OFIE;
    }
}

// This function checks for insertion/removal of the card.  If either is
// detected, it informs the API by calling USBMSC_updateMediaInfo().
// Whether it detects it or not, it returns non-zero if the card
// is present, or zero if not present
BYTE checkInsertionRemoval(void)
{
    // Check card status -- there or not?
    BYTE newCardStatus = detectCard();

    if ((newCardStatus) && (mediaInfo.mediaPresent == kUSBMSC_MEDIA_NOT_PRESENT))
    {
        // An insertion has been detected -- inform the API
        mediaInfo.mediaPresent = kUSBMSC_MEDIA_PRESENT;
        mediaInfo.mediaChanged = 0x01;
        // Get the size of this new medium
        DRESULT SDCard_result = disk_ioctl(0,
                                           GET_SECTOR_COUNT,
                                           &mediaInfo.lastBlockLba);
        USBMSC_updateMediaInfo(0, &mediaInfo);
    }

    if ((!newCardStatus) && (mediaInfo.mediaPresent == kUSBMSC_MEDIA_PRESENT))
    {
        // A removal has been detected -- inform the API
        mediaInfo.mediaPresent = kUSBMSC_MEDIA_NOT_PRESENT;
        mediaInfo.mediaChanged = 0x01;
        USBMSC_updateMediaInfo(0, &mediaInfo);
    }

    return newCardStatus;
}

void read_LUN0(void)
{
    // A READ operation is underway, and the app has been requested to access
    // the medium.  So, call file system to read to do so.  Note this is a low
    // level FatFs call -- we are not attempting to open a file ourselves.
    // The host is in control of this access, we're just carrying it out.
    DRESULT dresult = disk_read(0,                      // Physical drive number (0)
                                RWbuf_info->bufferAddr, // Pointer to the user buffer
                                RWbuf_info->lba,        // First LBA of this buffer operation
                                RWbuf_info->lbCount);   // The number of blocks being requested as

    // part of this operation

    // The result of the file system call needs to be communicated to the host.
    // Different file system software uses different return codes, but they all
    // communicate the same types of results.  This code ultimately gets passed
    // to the host app that issued the command to read (or if the user did it
    // the host OS, perhaps in a dialog box).
    switch (dresult)
    {
        case RES_OK:
            RWbuf_info->returnCode = kUSBMSC_RWSuccess;
            break;
        case RES_ERROR:                                 // In FatFs, this result suggests the medium
                                                        // may have been removed recently.
            if (!checkInsertionRemoval())               // This application function checks for the
                                                        // SD-card, and if missing, calls
                                                        // USBMSC_updateMediaInfo() to inform the
                                                        // API
                RWbuf_info->returnCode = kUSBMSC_RWMedNotPresent;
            break;
        case RES_NOTRDY:
            RWbuf_info->returnCode = kUSBMSC_RWNotReady;
            break;
        case RES_PARERR:
            RWbuf_info->returnCode = kUSBMSC_RWLbaOutOfRange;
            break;
    }

    USBMSC_bufferProcessed();
}

void write_LUN0(void)
{
    DRESULT dresult = disk_write(0,                      // Physical drive number (0)
                                 RWbuf_info->bufferAddr, // Pointer to the user buffer
                                 RWbuf_info->lba,        // First LBA of this buffer operation
                                 RWbuf_info->lbCount);   // The number of blocks being requested as

    // part of this operation

    switch (dresult)
    {
        case RES_OK:
            RWbuf_info->returnCode = kUSBMSC_RWSuccess;
            break;
        case RES_ERROR:
            if (!checkInsertionRemoval())
                RWbuf_info->returnCode = kUSBMSC_RWMedNotPresent;
            break;
        case RES_NOTRDY:
            RWbuf_info->returnCode = kUSBMSC_RWNotReady;
            break;
        case RES_PARERR:
            RWbuf_info->returnCode = kUSBMSC_RWLbaOutOfRange;
            break;
        default:
            RWbuf_info->returnCode = kUSBMSC_RWNotReady;
            break;
    }
    USBMSC_bufferProcessed();
}

