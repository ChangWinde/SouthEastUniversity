// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (HID Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbHid.c, File Version 1.00 2009/12/03                             |
|  Author: RSTO                                                               |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2009/02/20   ported from CdcHid                               |
|  RSTO         2009/05/19   updated USBHID_intfStatus()                      |
|  RSTO         2009/05/26   added USBHID_bytesInUSBBuffer()                  |
|  RSTO         2009/05/28   changed USBHID_sendData()                        |
|  RSTO         2009/06/09   updated USBHID_bytesInUSBBuffer()                |
|  MSP/Biju     2009/10/21   Changes for composite support                    |
|  RSTO         2009/10/21   move __disable_interrupt() before                |
|                            checking for suspend                             |
+----------------------------------------------------------------------------*/

#include "../USB_Common/device.h"
#include "../USB_Common/types.h"              // Basic Type declarations
#include "../USB_Common/defMSP430USB.h"
#include "../USB_Common/usb.h"            // USB-specific Data Structures
#include "UsbHid.h"
#include <descriptors.h>
#include <string.h>

#ifdef _HID_

//function pointers
extern VOID *(*USB_TX_memcpy)(VOID * dest, const VOID * source, size_t count);
extern VOID *(*USB_RX_memcpy)(VOID * dest, const VOID * source, size_t count);

// Local Macros
#define INTFNUM_OFFSET(X)   (X - HID0_INTFNUM)  // Get the HID offset

static struct _HidWrite
{
    WORD nHidBytesToSend;            // holds counter of bytes to be sent
    WORD nHidBytesToSendLeft;        // holds counter how many bytes is still to be sent
    const BYTE* pHidBufferToSend;    // holds the buffer with data to be sent
    BYTE bCurrentBufferXY;           // indicates which buffer is to use next for for write into IN OUT endpoint
} HidWriteCtrl[HID_NUM_INTERFACES];

static struct _HidRead
{
    BYTE *pUserBuffer;     // holds the current position of user's receiving buffer. If NULL- no receiving operation started
    BYTE *pCurrentEpPos;   // current positon to read of received data from curent EP
    WORD nBytesToReceive;  // holds how many bytes was requested by receiveData() to receive
    WORD nBytesToReceiveLeft;        // holds how many bytes is still requested by receiveData() to receive
    BYTE * pCT1;           // holds current EPBCTxx register
    BYTE * pCT2;           // holds next EPBCTxx register
    BYTE * pEP2;           // holds addr of the next EP buffer
    BYTE nBytesInEp;       // how many received bytes still available in current EP
    BYTE bCurrentBufferXY; // indicates which buffer is used by host to transmit data via OUT endpoint
} HidReadCtrl[HID_NUM_INTERFACES];

extern WORD wUsbEventMask;

/*----------------------------------------------------------------------------+
| Global Variables                                                            |
+----------------------------------------------------------------------------*/

extern __no_init tEDB __data16 tInputEndPointDescriptorBlock[];
extern __no_init tEDB __data16 tOutputEndPointDescriptorBlock[];


VOID HidCopyUsbToBuff(BYTE* pEP, BYTE* pCT, BYTE);

/*----------------------------------------------------------------------------+
| Functions' implementatin                                                    |
+----------------------------------------------------------------------------*/

//resets internal HID data structure
VOID HidResetData()
{
    // indicates which buffer is used by host to transmit data via OUT endpoint3 - X buffer is first
    //HidReadCtrl[intfIndex].bCurrentBufferXY = X_BUFFER;

    memset(&HidReadCtrl, 0, sizeof(HidReadCtrl));
    memset(&HidWriteCtrl, 0, sizeof(HidWriteCtrl));
}


/*
Sends a pre-built report reportData to the host.
  Returns:  kUSBHID_sendComplete
            kUSBHID_intfBusyError
            kUSBCDC_busNotAvailable
*/
BYTE USBHID_sendReport(const BYTE * reportData, BYTE intfNum)
{
    BYTE byte_count;
    BYTE * pEP1;
    BYTE * pCT1;

    BYTE edbIndex;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        return kUSBHID_busNotAvailable;
    }

    if (HidWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY == X_BUFFER)
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)stUsbHandle[intfNum].iep_X_Buffer;
        pCT1 = &tInputEndPointDescriptorBlock[edbIndex].bEPBCTX;
    }
    else
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)stUsbHandle[intfNum].iep_Y_Buffer;
        pCT1 = &tInputEndPointDescriptorBlock[edbIndex].bEPBCTY;
    }

    byte_count = USBHID_REPORT_LENGTH;  // we support only one length of report

    if(*pCT1 & EPBCNT_NAK)              // if this EP is empty
    {
        USB_TX_memcpy(pEP1, reportData, byte_count);  // copy data into IEP X or Y buffer
        *pCT1 = byte_count;                           // Set counter for usb In-Transaction
        HidWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY = (HidWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY+1)&0x01; //switch buffer
        return kUSBHID_sendComplete;
    }
    return kUSBHID_intfBusyError;
}

/*
Receives report reportData from the host.
Return:     kUSBHID_receiveCompleted
            kUSBHID_generalError
            kUSBCDC_busNotAvailable
*/
BYTE USBHID_receiveReport(BYTE * reportData, BYTE intfNum)
{
    BYTE ret = kUSBHID_generalError;
    BYTE nTmp1 = 0;

    BYTE edbIndex;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        return kUSBHID_busNotAvailable;
    }

    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY == X_BUFFER)  //this is current buffer
    {
        if (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & EPBCNT_NAK) //this buffer has a valid data packet
        {
            //this is the active EP buffer
            //pEP1
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;

            //second EP buffer
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;
            nTmp1 = 1;    //indicate that data is available
        }
    }
    else // Y_BUFFER
	{
		if (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & EPBCNT_NAK)
		{
			//this is the active EP buffer
			HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
			HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;

			//second EP buffer
			HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
			HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;
			nTmp1 = 1;    //indicate that data is available
		}
	}

    if (nTmp1)
    {
        // how many byte we can get from one endpoint buffer
        nTmp1 = *HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1;

        if(nTmp1 & EPBCNT_NAK)
        {
            nTmp1 = nTmp1 &0x7f;            // clear NAK bit
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1; // holds how many valid bytes in the EP buffer

            USB_RX_memcpy(reportData, HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos, nTmp1);
            //memcpy(reportData, HidReadCtrl.pEP1, nTmp1);
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY = (HidReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY+1) &0x01;
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = 0;
            *HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = 0;          // clear NAK, EP ready to receive data

            ret = kUSBHID_receiveCompleted;
        }
    }
    return ret;
}


/*
Sends data over interface intfNum, of size size and starting at address data.
Returns: kUSBHID_sendStarted
         kUSBHID_sendComplete
         kUSBHID_intBusyError
*/
BYTE USBHID_sendData(const BYTE* data, WORD size, BYTE intfNum)
{
    unsigned short bGIE;
    BYTE edbIndex;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    if (size == 0)
    {
        return kUSBHID_generalError;
    }

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status

    // atomic operation - disable interrupts
    __disable_interrupt();               // Disable global interrupts

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        // data can not be read because of USB suspended
        __bis_SR_register(bGIE); //restore interrupt status
        return kUSBHID_busNotAvailable;
    }

    if (HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft != 0)
    {
        // the USB still sends previous data, we have to wait
        __bis_SR_register(bGIE); //restore interrupt status
        return kUSBHID_intfBusyError;
    }

    //This function generate the USB interrupt. The data will be sent out from interrupt

    HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSend = size;
    HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft = size;
    HidWriteCtrl[INTFNUM_OFFSET(intfNum)].pHidBufferToSend = data;

    //trigger Endpoint Interrupt - to start send operation
    USBIEPIFG |= 1<<(edbIndex+1);   //IEPIFGx;

    __bis_SR_register(bGIE); //restore interrupt status

    return kUSBHID_sendStarted;
}

//this function is used only by USB interrupt
BOOL HidToHostFromBuffer(BYTE intfNum)
{
    BYTE byte_count, nTmp2;
    BYTE * pEP1;
    BYTE * pEP2;
    BYTE * pCT1;
    BYTE * pCT2;
    BYTE bWakeUp = FALSE; // per default we do not wake up after interrupt

    BYTE edbIndex;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    if (HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft == 0)           // do we have somtething to send?
    {

        HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSend = 0;

        //call event callback function
        if (wUsbEventMask & kUSB_sendCompletedEvent)
        {
            bWakeUp = USBHID_handleSendCompleted(intfNum);
        }
        return bWakeUp;
    }

    if(!(tInputEndPointDescriptorBlock[edbIndex].bEPCNF & EPCNF_TOGGLE))
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)stUsbHandle[intfNum].iep_X_Buffer;
        pCT1 = &tInputEndPointDescriptorBlock[edbIndex].bEPBCTX;

        //second EP buffer
        pEP2 = (BYTE*)stUsbHandle[intfNum].iep_Y_Buffer;
        pCT2 = &tInputEndPointDescriptorBlock[edbIndex].bEPBCTY;
    }
    else
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)stUsbHandle[intfNum].iep_Y_Buffer;
        pCT1 = &tInputEndPointDescriptorBlock[edbIndex].bEPBCTY;

        //second EP buffer
        pEP2 = (BYTE*)stUsbHandle[intfNum].iep_X_Buffer;
        pCT2 = &tInputEndPointDescriptorBlock[edbIndex].bEPBCTX;
    }

    // how many byte we can send over one endpoint buffer
    //  2 bytes a reserved: [0] - HID Report Descriptor, [1] - count of valid bytes
    byte_count = (HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft > EP_MAX_PACKET_SIZE-2) ? EP_MAX_PACKET_SIZE-2 : HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft;
    nTmp2 = *pCT1;

    if(nTmp2 & EPBCNT_NAK)
    {
        USB_TX_memcpy(pEP1+2, HidWriteCtrl[INTFNUM_OFFSET(intfNum)].pHidBufferToSend, byte_count);  // copy data into IEP3 X or Y buffer
        pEP1[0] = 0x3F;                                     // set HID report descriptor: 0x3F
        pEP1[1] = byte_count;                               // set HID report descriptor

        // 64 bytes will be send: we use only one HID report descriptor
        *pCT1 = 0x40;                                       // Set counter for usb In-Transaction

        HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft -= byte_count;
        HidWriteCtrl[INTFNUM_OFFSET(intfNum)].pHidBufferToSend += byte_count;        // move buffer pointer

        //try to send data over second buffer
        nTmp2 = *pCT2;
        if ((HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft > 0) &&       // do we have more data to send?
            (nTmp2 & EPBCNT_NAK)) // if the second buffer is free?
        {
            // how many byte we can send over one endpoint buffer
            byte_count = (HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft > EP_MAX_PACKET_SIZE-2) ? EP_MAX_PACKET_SIZE-2 : HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft;

            USB_TX_memcpy(pEP2+2, HidWriteCtrl[INTFNUM_OFFSET(intfNum)].pHidBufferToSend, byte_count); // copy data into IEP3 X or Y buffer
            pEP2[0] = 0x3F;                                 // set HID report descriptor: 0x3F
            pEP2[1] = byte_count;                           // set byte count of valid data

            // 64 bytes will be send: we use only one HID report descriptor
            *pCT2 = 0x40;                                   // Set counter for usb In-Transaction

            HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft -= byte_count;
            HidWriteCtrl[INTFNUM_OFFSET(intfNum)].pHidBufferToSend += byte_count;    // move buffer pointer
        }
    }
    return bWakeUp;
}

/*
Aborts an active send operation on interface intfNum.
Returns the number of bytes that were sent prior to the abort, in size.
*/
BYTE USBHID_abortSend(WORD* size, BYTE intfNum)
{
    unsigned short bGIE;
    bGIE  = (__get_SR_register() &GIE);  //save interrupt status

    __disable_interrupt(); //disable interrupts - atomic operation

    *size = (HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSend - HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft);
    HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSend = 0;
    HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft = 0;

    __bis_SR_register(bGIE); //restore interrupt status
    return kUSB_succeed;
}

// This function copies data from OUT endpoint into user's buffer
// Arguments:
//    pEP - pointer to EP to copy from
//    pCT - pointer to pCT control reg
//
VOID HidCopyUsbToBuff(BYTE* pEP, BYTE* pCT,BYTE intfNum)
{
    BYTE nCount;

    // how many byte we can get from one endpoint buffer
    nCount = (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft > HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp) ? HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp : HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft;

    USB_RX_memcpy(HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer, pEP, nCount); // copy data from OEPx X or Y buffer
    HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft -= nCount;
    HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer += nCount;          // move buffer pointer
                                                // to read rest of data next time from this place

    if (nCount == HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp)       // all bytes are copied from receive buffer?
    {
        //switch current buffer
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY = (HidReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY+1) &0x01;

        HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = 0;

        //clear NAK, EP ready to receive data
        *pCT = 0;
    }
    else
    {
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp -= nCount;
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = pEP + nCount;
    }
}


/*
Receives data over interface intfNum, of size size, into memory starting at address data.
Returns:
    kUSBHID_receiveStarted  if the receiving process started.
    kUSBHID_receiveCompleted  all requested date are received.
    kUSBHID_receiveInProgress  previous receive opereation is in progress. The requested receive operation can be not started.
    kUSBHID_generalError  error occurred.
*/
BYTE USBHID_receiveData(BYTE* data, WORD size, BYTE intfNum)
{
    BYTE nTmp1;
    unsigned short bGIE;
    BYTE edbIndex;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    if ((size == 0) ||                          // read size is 0
        (data == NULL))
    {
        return kUSBHID_generalError;
    }

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status

    // atomic operation - disable interrupts
    __disable_interrupt();               // Disable global interrupts

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
         __bis_SR_register(bGIE); //restore interrupt status
        return kUSBHID_busNotAvailable;
    }

    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer != NULL)        // receive process already started
    {
        __bis_SR_register(bGIE); //restore interrupt status
        return kUSBHID_receiveInProgress;
    }

    HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceive = size;         // bytes to receive
    HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft = size;     // left bytes to receive
    HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = data;             // set user receive buffer

    //read rest of data from buffer, if any
    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp > 0)
    {
        // copy data from pEP-endpoint into User's buffer
        HidCopyUsbToBuff(HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos, HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1,intfNum);

        if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft == 0)     // the Receive opereation is completed
        {
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;     // no more receiving pending
            USBHID_handleReceiveCompleted(intfNum);      // call event handler in interrupt context
            __bis_SR_register(bGIE); //restore interrupt status
            return kUSBHID_receiveCompleted;    // receive completed
        }

        // check other EP buffer for data - exchange pCT1 with pCT2
        if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 == &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX)
        {
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
        }
        else
        {
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
        }
        nTmp1 = *HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1;
        //try read data from second buffer
        if (nTmp1 & EPBCNT_NAK)                 // if the second buffer has received data?
        {
            nTmp1 = nTmp1 &0x7f;                // clear NAK bit
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = *(HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos+1); // holds how many valid bytes in the EP buffer
            if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp > nTmp1-2)
            {
                HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1-2;
            }
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos += 2;           // here starts user data
            HidCopyUsbToBuff(HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos, HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1,intfNum);
        }

        if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft == 0)     // the Receive opereation is completed
        {
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;     // no more receiving pending
            USBHID_handleReceiveCompleted(intfNum);   // call event handler in interrupt context
            __bis_SR_register(bGIE); //restore interrupt status
            return kUSBHID_receiveCompleted;    // receive completed
        }
    } //read rest of data from buffer, if any

    //read 'fresh' data, if available
    nTmp1 = 0;
    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY == X_BUFFER)  //this is current buffer
    {
        if (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & EPBCNT_NAK) //this buffer has a valid data packet
        {
            //this is the active EP buffer
            //pEP1
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;

            //second EP buffer
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;
            nTmp1 = 1;    //indicate that data is available
        }
    }
    else // Y_BUFFER
	{
		if (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & EPBCNT_NAK)
		{
			//this is the active EP buffer
			HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
			HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;

			//second EP buffer
			HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
			HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;
			nTmp1 = 1;    //indicate that data is available
		}
	}

    if (nTmp1)
    {
        // how many byte we can get from one endpoint buffer
        nTmp1 = *HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1;

        if(nTmp1 & EPBCNT_NAK)
        {
            nTmp1 = nTmp1 &0x7f;                      // clear NAK bit
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = *(HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos+1); // holds how many valid bytes in the EP buffer
            if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp > nTmp1-2)
            {
                HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1-2;
            }
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos += 2;           // here starts user data
            HidCopyUsbToBuff(HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos, HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1,intfNum);

            nTmp1 = *HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2;
            //try read data from second buffer
            if ((HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft > 0) &&       // do we have more data to receive?
                (nTmp1 & EPBCNT_NAK))                 // if the second buffer has received data?
            {
                nTmp1 = nTmp1 &0x7f;                  // clear NAK bit
                HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = *(HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2+1); // holds how many valid bytes in the EP buffer
                if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp > nTmp1-2)
                {
                    HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1-2;
                }
                HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 += 2;             	  // here starts user data
                HidCopyUsbToBuff(HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2, HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2,intfNum);
                HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2;
            }
        }
    }

    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft == 0)     // the Receive opereation is completed
    {
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;           // no more receiving pending
        USBHID_handleReceiveCompleted(intfNum);            // call event handler in interrupt context
        __bis_SR_register(bGIE); //restore interrupt status
        return kUSBHID_receiveCompleted;
    }

    //interrupts enable
    __bis_SR_register(bGIE); //restore interrupt status
    return kUSBHID_receiveStarted;
}

//this function is used only by USB interrupt.
//It fills user receiving buffer with received data
BOOL HidToBufferFromHost(BYTE intfNum)
{
    BYTE * pEP1;
    BYTE nTmp1;
    BYTE bWakeUp = FALSE; // per default we do not wake up after interrupt

    BYTE edbIndex;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft == 0)       // do we have somtething to receive?
    {
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;             // no more receiving pending
        return bWakeUp;
    }

    // No data to receive...
    if (!((tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX |
           tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY)
           & 0x80))
    {
        return bWakeUp;
    }

    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY == X_BUFFER)   //X is current buffer
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;

        //second EP buffer
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;
    }
    else
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;

        //second EP buffer
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;
    }

    // how many byte we can get from one endpoint buffer
    nTmp1 = *HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1;

    if(nTmp1 & EPBCNT_NAK)
    {
        nTmp1 = nTmp1 &0x7f;                // clear NAK bit
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = *(pEP1+1); // holds how many valid bytes in the EP buffer
        if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp > nTmp1-2)
        {
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1-2;
        }
        pEP1 += 2;                      // here starts user data
        HidCopyUsbToBuff(pEP1, HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1,intfNum);

        nTmp1 = *HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2;
        //try read data from second buffer
        if ((HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft > 0) &&       // do we have more data to send?
            (nTmp1 & EPBCNT_NAK))                   // if the second buffer has received data?
        {
            nTmp1 = nTmp1 &0x7f;                    // clear NAK bit
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = *(pEP1+1);     // holds how many valid bytes in the EP buffer
            if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp > nTmp1-2)
            {
                HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1-2;
            }
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 += 2;                              // here starts user data
            HidCopyUsbToBuff(HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2, HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2,intfNum);
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2;
        }
    }

    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft == 0)       // the Receive opereation is completed
    {
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;             // no more receiving pending
        if (wUsbEventMask & kUSB_receiveCompletedEvent)
        {
            bWakeUp = USBHID_handleReceiveCompleted(intfNum);
        }

        if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp)                 // Is not read data still available in the EP?
        {
            if (wUsbEventMask & kUSB_dataReceivedEvent)
            {
                bWakeUp = USBHID_handleDataReceived(intfNum);
            }
        }
    }
    return bWakeUp;
}

// helper for USB interrupt handler
BOOL HidIsReceiveInProgress(BYTE intfNum)
{
    return (HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer != NULL);
}


/*
Aborts an active receive operation on interface intfNum.
  Returns the number of bytes that were received and transferred
  to the data location established for this receive operation.
*/
BYTE USBHID_abortReceive(WORD* size, BYTE intfNum)
{
    unsigned short bGIE;

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status
    __disable_interrupt(); //disable interrupts - atomic operation

    *size = 0;    //set received bytes count to 0

    //is receive operation underway?
    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer)
    {
        //how many bytes are already received?
        *size = HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceive - HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft;

        HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = 0;
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft = 0;
    }

     //restore interrupt status
    __bis_SR_register(bGIE); //restore interrupt status
    return kUSB_succeed;
}

/*
This function rejects payload data that has been received from the host.
*/
BYTE USBHID_rejectData(BYTE intfNum)
{
    unsigned short bGIE;
    BYTE edbIndex;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status

    //interrupts disable
    __disable_interrupt();

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if (bFunctionSuspended)
    {
        __bis_SR_register(bGIE); //restore interrupt status
        return kUSBHID_busNotAvailable;
    }

    //Is receive operation underway?
    // - do not flush buffers if any operation still active.
    if (!HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer)
    {
        BYTE tmp1 = tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & EPBCNT_NAK;
        BYTE tmp2 = tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & EPBCNT_NAK;

        if (tmp1 ^ tmp2) // switch current buffer if any and only ONE of the buffers is full
        {
            //switch current buffer
            HidReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY = (HidReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY+1) &0x01;
        }

        tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX = 0;  //flush buffer X
        tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY = 0;  //flush buffer Y
        HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = 0;                     // indicates that no more data available in the EP
    }

    __bis_SR_register(bGIE); //restore interrupt status
    return kUSB_succeed;
}

/*
This function indicates the status of the interface intfNum.
  If a send operation is active for this interface,
  the function also returns the number of bytes that have been transmitted to the host.
  If a receiver operation is active for this interface, the function also returns
  the number of bytes that have been received from the host and are waiting at the assigned address.

returns kUSBHID_waitingForSend (indicates that a call to USBHID_SendData()
  has been made, for which data transfer has not been completed)

returns kUSBHID_waitingForReceive (indicates that a receive operation
  has been initiated, but not all data has yet been received)

returns kUSBHID_dataWaiting (indicates that data has been received
  from the host, waiting in the USB receive buffers)
*/
BYTE USBHID_intfStatus(BYTE intfNum, WORD* bytesSent, WORD* bytesReceived)
{
    BYTE ret = 0;
	unsigned short bGIE;
		
    *bytesSent = 0;
    *bytesReceived = 0;

    BYTE edbIndex;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status
    __disable_interrupt(); //disable interrupts - atomic operation

    // Is send operation underway?
    if (HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft != 0)
    {
        ret |= kUSBHID_waitingForSend;
        *bytesSent = HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSend - HidWriteCtrl[INTFNUM_OFFSET(intfNum)].nHidBytesToSendLeft;
    }

    //Is receive operation underway?
    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer != NULL)
    {
        ret |= kUSBHID_waitingForReceive;
        *bytesReceived = HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceive - HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft;
    }
    else // not receive operation started
    {
        // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
        if (!bFunctionSuspended)
        {
            if((tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & EPBCNT_NAK)  | //any of buffers has a valid data packet
               (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & EPBCNT_NAK))
            {
                ret |= kUSBHID_dataWaiting;
            }
        }
    }

    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        // if suspended or not enumerated  - report no other tasks pending
        ret = kUSBHID_busNotAvailable;
    }

     //restore interrupt status
    __bis_SR_register(bGIE); //restore interrupt status

    return ret;
}

/*
Returns how many bytes are in the buffer are received and ready to be read.
*/
BYTE USBHID_bytesInUSBBuffer(BYTE intfNum)
{
    BYTE bTmp1 = 0;
    BYTE bTmp2;

    BYTE edbIndex;
    unsigned short bGIE;

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status

    edbIndex = stUsbHandle[intfNum].edb_Index;

    //interrupts disable
    __disable_interrupt();

    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        // if suspended or not enumerated - report 0 bytes available
       __bis_SR_register(bGIE); //restore interrupt status
        return 0;
    }

    if (HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp > 0)         // If a RX operation is underway, part of data may was read of the OEP buffer
    {
        bTmp1 = HidReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp;
        if (*HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 & EPBCNT_NAK) // the next buffer has a valid data packet
        {
            bTmp2 = *(HidReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2+1);  // holds how many valid bytes in the EP buffer
            if (bTmp2 > (*HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 & 0x7F) -2) // check if all data received correctly
            {
                bTmp1 += (*HidReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 & 0x7F) -2;
            }
            else
            {
                bTmp1 += bTmp2;
            }
        }
    }
    else
    {
        if (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & EPBCNT_NAK) //this buffer has a valid data packet
        {
            bTmp2 = tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & 0x7F;
            bTmp1 = *((BYTE*)stUsbHandle[intfNum].oep_X_Buffer+1);
            if (bTmp2-2 < bTmp1)	// check if the count (second byte) is valid
            {
                bTmp1 = bTmp2 - 2;
            }
        }
        if (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & EPBCNT_NAK) //this buffer has a valid data packet
        {
            bTmp2 = tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & 0x7F;
            if (bTmp2-2 > *((BYTE*)stUsbHandle[intfNum].oep_Y_Buffer+1)) // check if the count (second byte) is valid
            {
                bTmp1 += *((BYTE*)stUsbHandle[intfNum].oep_Y_Buffer+1);
            }
            else
            {
                bTmp1 += bTmp2 - 2;
            }
        }
    }

    //interrupts enable
   __bis_SR_register(bGIE); //restore interrupt status
    return bTmp1;
}

#endif //ifdef _HID_

/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
