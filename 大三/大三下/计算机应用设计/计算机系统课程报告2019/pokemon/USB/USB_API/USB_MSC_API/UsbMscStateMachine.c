// (c)2010 by Texas Instruments Incorporated, All Rights Reserved.
  /*----------------------------------------------------------------------------+
  |                                                                             |
  |                              Texas Instruments                              |
  |                                                                             |
  |                          MSP430 USB-Example (MSC Driver)                    |
  |                                                                             |
  +-----------------------------------------------------------------------------+
  |  Source: Msc_State_Machine.c, File Version 1.01                             |
  |  Description: This file contains the core function that handles the MSC SCSI|
  |                state machine.                                               |
  |  Author: Biju,MSP                                                           |
  |                                                                             |
  |  WHO          WHEN         WHAT                                             |
  |  ---          ----------   ------------------------------------------------ |
  |  MSP          2010/02/16   Created                                          |
  |  Biju,MSP     2010/07/15   CV Bug fix                                       |
  |  RSTO         2010/10/30   state machine rework                             |
  +----------------------------------------------------------------------------*/
/*File includes */
#include "../USB_Common/device.h"
#include "../USB_Common/types.h"
#include "../USB_Common/defMSP430USB.h"
#include "../USB_MSC_API/UsbMscScsi.h"
#include "../USB_MSC_API/UsbMsc.h"
#include "../USB_Common/usb.h"
#include <descriptors.h>
#include <string.h>

#ifdef _MSC_

/*Macros to indicate data direction */
#define DIRECTION_IN	0x80
#define DIRECTION_OUT	0x00

/*Flags to monitor data send/recv.Will be set in ISR accordingly */
extern BOOL bMscCbwReceived;    //Flag to indicate whether any CBW recieved from host
extern BOOL bMcsCommandSupported;  //Flag to know if its a supported command
extern BOOL bMscCbwFailed;

/* Variable that holds the MSC SCSI state */
static BOOL bMscSendCsw = FALSE;
extern BOOL isMSCConfigured;

/*Buffer pointers passed by application */
extern BYTE *xBufferAddr;
extern BYTE *yBufferAddr;
extern __no_init tEDB __data16 tInputEndPointDescriptorBlock[];

BYTE Scsi_Verify_CBW();

/*----------------------------------------------------------------------------+
  | Functions                                                                  |
+----------------------------------------------------------------------------*/
VOID Msc_ResetStateMachine(VOID)
{
    bMscSendCsw = FALSE;
    Scsi_Residue = 0;
}

//----------------------------------------------------------------------------
/*This is the core function called by application to handle the MSC SCSI state
  machine */
BYTE USBMSC_poll()
{
    BYTE edbIndex;
    BYTE ret;

    edbIndex = stUsbHandle[MSC0_INTFNUM].edb_Index;

    //check if currently transmitting data..
    if (MscReadControl.bReadProcessing == TRUE)
    {
        BYTE bGIE;
        bGIE  = (__get_SR_register() &GIE);     //save interrupt status
        // atomic operation - disable interrupts
        __disable_interrupt();                   // Disable global interrupts
        if ((MscReadControl.dwBytesToSendLeft == 0) &&
            (MscReadControl.lbaCount == 0))
        {
            //data is no more processing - clear flags..
            MscReadControl.bReadProcessing = FALSE;
            __bis_SR_register(bGIE);            //restore interrupt status
        }
        else
        {
            if (!(tInputEndPointDescriptorBlock[edbIndex].bEPCNF & EPCNF_STALL)) //if it is not stalled - contiune communication
            {
                USBIEPIFG |= 1<<(edbIndex+1);   //trigger IN interrupt to finish data tranmition
            }
            __bis_SR_register(bGIE);            //restore interrupt status
            return kUSBMSC_processBuffer;
        }
    }

    if(isMSCConfigured == FALSE)
    {
        return kUSBMSC_okToSleep;
    }

    if (!bMscSendCsw)
    {

        ret = kUSBMSC_processBuffer;
        if (bMscCbwReceived)
        {
            if (Scsi_Verify_CBW() == SUCCESS)
            {
                MscReadControl.bIsIdle = FALSE;
                // Successful reception of CBW
                // Parse the CBW opcode and invoke the right command handler function
                Scsi_Cmd_Parser(MSC0_INTFNUM);
                bMscSendCsw = TRUE;
            }
            bMscCbwReceived = FALSE; //CBW is performed!
        }
        else
        {
            if(!MscReadControl.bIsIdle)
            {
                return kUSBMSC_processBuffer;
            }
            else
            {
                return kUSBMSC_okToSleep;
            }
        }
        //check if any of out pipes has pending data and trigger interrupt

        if ((MscWriteControl.pCT1 != NULL)   &&
            ((*MscWriteControl.pCT1 & EPBCNT_NAK ) ||
             (*MscWriteControl.pCT2 & EPBCNT_NAK )))
        {
            USBOEPIFG |= 1<<(edbIndex+1);   //trigger OUT interrupt again
            return kUSBMSC_processBuffer;   //do not asleep, as data is coming in
                                            //and follow up data perform will be required.
        }
    }

    if (bMscSendCsw)
    {
        if (bMcsCommandSupported == TRUE)
        {
            // watiting till transport is finished!
            if ((MscWriteControl.bWriteProcessing == FALSE) &&
                (MscReadControl.bReadProcessing == FALSE) &&
                (MscReadControl.lbaCount == 0))
            {
                // Send CSW
                if(SUCCESS == Scsi_Send_CSW(MSC0_INTFNUM))
                {
                    MscReadControl.bIsIdle = TRUE;
                    bMscSendCsw = FALSE;
                    ret = kUSBMSC_okToSleep;
                }
            }
        }
    }
    return ret;
}

#endif // _MSC_
/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
