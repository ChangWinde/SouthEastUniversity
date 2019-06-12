// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbCdc.c, File Version 1.01 2009/12/03                             |
|  Author: RSTO                                                               |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2008/09/03   born                                             |
|  RSTO         2008/09/19   Changed USBCDC_sendData to send more then 64bytes|
|  RSTO         2008/12/23   enhancements of CDC API                          |
|  RSTO         2008/05/19   updated USBCDC_intfStatus()                      |
|  RSTO         2009/05/26   added USBCDC_bytesInUSBBuffer()                  |
|  RSTO         2009/05/28   changed USBCDC_sendData()                        |
|  RSTO         2009/07/17   updated USBCDC_bytesInUSBBuffer()                |
|  RSTO         2009/10/21   move __disable_interrupt() before                |
|                            checking for suspend                             |
|  MSP,Biju     2009/12/28   Fix for the bug "Download speed is slow"         |
+----------------------------------------------------------------------------*/
#include <descriptors.h>

#ifdef _CDC_


#include "../USB_Common/device.h"
#include "../USB_Common/types.h"              // Basic Type declarations
#include "../USB_Common/defMSP430USB.h"
#include "../USB_Common/usb.h"            // USB-specific Data Structures
#include "../USB_CDC_API/UsbCdc.h"

#include <string.h>


static ULONG lBaudrate = 0;
static BYTE bDataBits = 8;
static BYTE bStopBits = 0;
static BYTE bParity = 0;

// Local Macros
#define INTFNUM_OFFSET(X)   (X - CDC0_INTFNUM)  // Get the CDC offset

static struct _CdcWrite
{
    WORD nCdcBytesToSend;        // holds counter of bytes to be sent
    WORD nCdcBytesToSendLeft;    // holds counter how many bytes is still to be sent
    const BYTE* pUsbBufferToSend;   // holds the buffer with data to be sent
    BYTE bCurrentBufferXY;       // is 0 if current buffer to write data is X, or 1 if current buffer is Y
	BYTE bZeroPacketSent; // = FALSE;
    BYTE last_ByteSend;
} CdcWriteCtrl[CDC_NUM_INTERFACES];

static struct _CdcRead
{
    BYTE *pUserBuffer;     // holds the current position of user's receiving buffer. If NULL- no receiving operation started
    BYTE *pCurrentEpPos;   // current positon to read of received data from curent EP
    WORD nBytesToReceive;  // holds how many bytes was requested by receiveData() to receive
    WORD nBytesToReceiveLeft;        // holds how many bytes is still requested by receiveData() to receive
    BYTE * pCT1;           // holds current EPBCTxx register
    BYTE * pCT2;           // holds next EPBCTxx register
    BYTE * pEP2;           // holds addr of the next EP buffer
    BYTE nBytesInEp;       // how many received bytes still available in current EP
    BYTE bCurrentBufferXY; // indicates which buffer is used by host to transmit data via OUT endpoint3
} CdcReadCtrl[CDC_NUM_INTERFACES];

extern WORD wUsbEventMask;

//function pointers
extern VOID *(*USB_TX_memcpy)(VOID * dest, const VOID * source, size_t count);
extern VOID *(*USB_RX_memcpy)(VOID * dest, const VOID * source, size_t count);


/*----------------------------------------------------------------------------+
| Global Variables                                                            |
+----------------------------------------------------------------------------*/

extern __no_init tEDB __data16 tInputEndPointDescriptorBlock[];
extern __no_init tEDB __data16 tOutputEndPointDescriptorBlock[];


VOID CdcResetData()
{

    // indicates which buffer is used by host to transmit data via OUT endpoint3 - X buffer is first
    //CdcReadCtrl[intfIndex].bCurrentBufferXY = X_BUFFER;

    memset(&CdcWriteCtrl, 0, sizeof(CdcWriteCtrl));
    memset(&CdcReadCtrl, 0, sizeof(CdcReadCtrl));

}

/*
Sends data over interface intfNum, of size size and starting at address data.
Returns: kUSBCDC_sendStarted
         kUSBCDC_sendComplete
         kUSBCDC_intfBusyError
*/
BYTE USBCDC_sendData(const BYTE* data, WORD size, BYTE intfNum)
{
    BYTE edbIndex;
    unsigned short bGIE;

    edbIndex= stUsbHandle[intfNum].edb_Index;

    if (size == 0)
    {
        return kUSBCDC_generalError;
    }

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status
    // atomic operation - disable interrupts
    __disable_interrupt();                   // Disable global interrupts

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        // data can not be read because of USB suspended
       __bis_SR_register(bGIE);   //restore interrupt status
        return kUSBCDC_busNotAvailable;
    }

    if (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft != 0)
    {
        // the USB still sends previous data, we have to wait
        __bis_SR_register(bGIE);   //restore interrupt status
        return kUSBCDC_intfBusyError;
    }

    //This function generate the USB interrupt. The data will be sent out from interrupt

    CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSend = size;
    CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft = size;
    CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].pUsbBufferToSend = data;

    //trigger Endpoint Interrupt - to start send operation
    USBIEPIFG |= 1<<(edbIndex+1);  //IEPIFGx;

    __bis_SR_register(bGIE);       //restore interrupt status

    return kUSBCDC_sendStarted;
}

#define EP_MAX_PACKET_SIZE_CDC      0x40

//this function is used only by USB interrupt
BOOL CdcToHostFromBuffer(BYTE intfNum)
{
    BYTE byte_count, nTmp2;
    BYTE * pEP1;
    BYTE * pEP2;
    BYTE * pCT1;
    BYTE * pCT2;
    BYTE bWakeUp = FALSE; //TRUE for wake up after interrupt
    BYTE edbIndex;

    edbIndex = stUsbHandle[intfNum].edb_Index;

    if (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft == 0)           // do we have somtething to send?
    {
        if (!CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bZeroPacketSent)               // zero packet was not yet sent
        {
            CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bZeroPacketSent = TRUE;

            if(CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].last_ByteSend == EP_MAX_PACKET_SIZE_CDC)
            {
                if (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY == X_BUFFER)
				{
                    tInputEndPointDescriptorBlock[edbIndex].bEPBCTX = 0;
				}
                else
				{
                    tInputEndPointDescriptorBlock[edbIndex].bEPBCTY = 0;
				}
                CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY = (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY+1)&0x01; //switch buffer
            }

            CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSend = 0;   // nothing to send

            //call event callback function
            if (wUsbEventMask & kUSB_sendCompletedEvent)
            {
                bWakeUp = USBCDC_handleSendCompleted(intfNum);
            }

        } // if (!bSentZeroPacket)

        return bWakeUp;
    }

    CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bZeroPacketSent = FALSE;    // zero packet will be not sent: we have data

    if (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY == X_BUFFER)
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
    byte_count = (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft > EP_MAX_PACKET_SIZE_CDC) ? EP_MAX_PACKET_SIZE_CDC : CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft;
    nTmp2 = *pCT1;

    if(nTmp2 & EPBCNT_NAK)
    {
        USB_TX_memcpy(pEP1, CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].pUsbBufferToSend, byte_count); // copy data into IEP3 X or Y buffer
        *pCT1 = byte_count;                      // Set counter for usb In-Transaction
        CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY = (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY+1)&0x01; //switch buffer
        CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft -= byte_count;
        CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].pUsbBufferToSend += byte_count;             // move buffer pointer
        CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].last_ByteSend = byte_count;

        //try to send data over second buffer
        nTmp2 = *pCT2;
        if ((CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft > 0) &&                                  // do we have more data to send?
            (nTmp2 & EPBCNT_NAK)) // if the second buffer is free?
        {
            // how many byte we can send over one endpoint buffer
            byte_count = (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft > EP_MAX_PACKET_SIZE_CDC) ? EP_MAX_PACKET_SIZE_CDC : CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft;

            USB_TX_memcpy(pEP2, CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].pUsbBufferToSend, byte_count); // copy data into IEP3 X or Y buffer
            *pCT2 = byte_count;                      // Set counter for usb In-Transaction
            CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY = (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY+1)&0x01; //switch buffer
            CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft -= byte_count;
            CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].pUsbBufferToSend += byte_count;            //move buffer pointer
            CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].last_ByteSend = byte_count;
        }
    }
    return bWakeUp;
}

/*
Aborts an active send operation on interface intfNum.
Returns the number of bytes that were sent prior to the abort, in size.
*/
BYTE USBCDC_abortSend(WORD* size, BYTE intfNum)
{
    unsigned short bGIE;

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status
    __disable_interrupt(); //disable interrupts - atomic operation

    *size = (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSend - CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft);
    CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSend = 0;
    CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft = 0;

    __bis_SR_register(bGIE); //restore interrupt status
    return kUSB_succeed;
}

// This function copies data from OUT endpoint into user's buffer
// Arguments:
//    pEP - pointer to EP to copy from
//    pCT - pointer to pCT control reg
//
VOID CopyUsbToBuff(BYTE* pEP, BYTE* pCT, BYTE intfNum)
{
    BYTE nCount;

    // how many byte we can get from one endpoint buffer
    nCount = (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft > CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp) ? CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp : CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft;

    USB_RX_memcpy(CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer, pEP, nCount); // copy data from OEP3 X or Y buffer
    CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft -= nCount;
    CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer += nCount;          // move buffer pointer
                                                // to read rest of data next time from this place

    if (nCount == CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp)       // all bytes are copied from receive buffer?
    {
        //switch current buffer
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY = (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY+1) &0x01;

        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = 0;

        //clear NAK, EP ready to receive data
        *pCT = 0x00;
    }
    else
    {
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp -= nCount;
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = pEP + nCount;
    }
}

/*
Receives data over interface intfNum, of size size, into memory starting at address data.
Returns:
    kUSBCDC_receiveStarted  if the receiving process started.
    kUSBCDC_receiveCompleted  all requested date are received.
    kUSBCDC_receiveInProgress  previous receive opereation is in progress. The requested receive operation can be not started.
    kUSBCDC_generalError  error occurred.
*/
BYTE USBCDC_receiveData(BYTE* data, WORD size, BYTE intfNum)
{
    BYTE nTmp1;
    BYTE edbIndex;
    unsigned short bGIE;

    edbIndex=stUsbHandle[intfNum].edb_Index;

    if ((size == 0) ||                          // read size is 0
        (data == NULL))
    {
        return kUSBCDC_generalError;
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
        return kUSBCDC_busNotAvailable;
    }

    if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer != NULL)        // receive process already started
    {
        __bis_SR_register(bGIE); //restore interrupt status
        return kUSBCDC_intfBusyError;
    }

    CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceive = size;         // bytes to receive
    CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft = size;     // left bytes to receive
    CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = data;             // set user receive buffer

    //read rest of data from buffer, if any
    if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp > 0)
    {
        // copy data from pEP-endpoint into User's buffer
        CopyUsbToBuff(CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos, CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1, intfNum);

        if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft == 0)     // the Receive opereation is completed
        {
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;     // no more receiving pending
			if (wUsbEventMask & kUSB_receiveCompletedEvent)
            {
			    USBCDC_handleReceiveCompleted(intfNum);      // call event handler in interrupt context
			}
            __bis_SR_register(bGIE); //restore interrupt status
            return kUSBCDC_receiveCompleted;    // receive completed
        }

        // check other EP buffer for data - exchange pCT1 with pCT2
        if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 == &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX)
        {
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
        }
        else
        {
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
        }

        nTmp1 = *CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1;
        //try read data from second buffer
        if (nTmp1 & EPBCNT_NAK)                 // if the second buffer has received data?
        {
            nTmp1 = nTmp1 &0x7f;                // clear NAK bit
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1;     // holds how many valid bytes in the EP buffer
            CopyUsbToBuff(CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos, CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1, intfNum);
        }

        if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft == 0)     // the Receive opereation is completed
        {
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;     // no more receiving pending
			if (wUsbEventMask & kUSB_receiveCompletedEvent)
            {
			    USBCDC_handleReceiveCompleted(intfNum);      // call event handler in interrupt context
			}
            __bis_SR_register(bGIE); //restore interrupt status
            return kUSBCDC_receiveCompleted;    // receive completed
        }
    } //read rest of data from buffer, if any

    //read 'fresh' data, if available
    nTmp1 = 0;
    if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY == X_BUFFER)  //this is current buffer
    {
        if (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & EPBCNT_NAK) //this buffer has a valid data packet
        {
            //this is the active EP buffer
            //pEP1
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;

            //second EP buffer
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;
            nTmp1 = 1;    //indicate that data is available
        }
    }
    else
	{// Y_BUFFER
		if (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & EPBCNT_NAK)
		{
			//this is the active EP buffer
			CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
			CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;

			//second EP buffer
			CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
			CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;
			nTmp1 = 1;    //indicate that data is available
		}
	}
	
    if (nTmp1)
    {
        // how many byte we can get from one endpoint buffer
        nTmp1 = *CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1;
        while(nTmp1 == 0)
        {
            nTmp1 = *CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1;
        }

        if(nTmp1 & EPBCNT_NAK)
        {
            nTmp1 = nTmp1 &0x7f;            // clear NAK bit
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1; // holds how many valid bytes in the EP buffer

            CopyUsbToBuff(CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCurrentEpPos, CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1, intfNum);

            nTmp1 = *CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2;
            //try read data from second buffer
            if ((CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft > 0) &&       // do we have more data to send?
                (nTmp1 & EPBCNT_NAK))                 // if the second buffer has received data?
            {
                nTmp1 = nTmp1 &0x7f;                  // clear NAK bit
                CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1;       // holds how many valid bytes in the EP buffer
                CopyUsbToBuff(CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2, CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2, intfNum);
                CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2;
            }
        }
    }

    if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft == 0)     // the Receive opereation is completed
    {
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;           // no more receiving pending
		if (wUsbEventMask & kUSB_receiveCompletedEvent)
        {
            USBCDC_handleReceiveCompleted(intfNum);         // call event handler in interrupt context
	    }
        __bis_SR_register(bGIE); //restore interrupt status
        return kUSBCDC_receiveCompleted;
    }

    //interrupts enable
   __bis_SR_register(bGIE); //restore interrupt status
    return kUSBCDC_receiveStarted;
}


//this function is used only by USB interrupt.
//It fills user receiving buffer with received data
BOOL CdcToBufferFromHost(BYTE intfNum)
{
    BYTE * pEP1;
    BYTE nTmp1;
    BYTE bWakeUp = FALSE; // per default we do not wake up after interrupt

    BYTE edbIndex;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft == 0)       // do we have somtething to receive?
    {
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;             // no more receiving pending
        return bWakeUp;
    }

    // No data to receive...
    if (!((tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX |
           tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY)
           & 0x80))
    {
        return bWakeUp;
    }

    if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY == X_BUFFER)   //X is current buffer
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;

        //second EP buffer
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;
    }
    else
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)stUsbHandle[intfNum].oep_Y_Buffer;
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY;

        //second EP buffer
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2 = (BYTE*)stUsbHandle[intfNum].oep_X_Buffer;
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 = &tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX;
    }

    // how many byte we can get from one endpoint buffer
    nTmp1 = *CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1;

    if(nTmp1 & EPBCNT_NAK)
    {
        nTmp1 = nTmp1 &0x7f;            // clear NAK bit
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1; // holds how many valid bytes in the EP buffer

        CopyUsbToBuff(pEP1, CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1, intfNum);

        nTmp1 = *CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2;
        //try read data from second buffer
        if ((CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft > 0) &&       // do we have more data to send?
            (nTmp1 & EPBCNT_NAK))                 // if the second buffer has received data?
        {
            nTmp1 = nTmp1 &0x7f;                  // clear NAK bit
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = nTmp1;       // holds how many valid bytes in the EP buffer
            CopyUsbToBuff(CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pEP2, CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2, intfNum);
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT1 = CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2;
        }
    }

    if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft == 0)     // the Receive opereation is completed
    {
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;   // no more receiving pending
        if (wUsbEventMask & kUSB_receiveCompletedEvent)
        {
            bWakeUp = USBCDC_handleReceiveCompleted(intfNum);
        }

        if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp)       // Is not read data still available in the EP?
        {
            if (wUsbEventMask & kUSB_dataReceivedEvent)
            {
                bWakeUp = USBCDC_handleDataReceived(intfNum);
            }
        }
    }
    return bWakeUp;
}

// helper for USB interrupt handler
BOOL CdcIsReceiveInProgress(BYTE intfNum)
{
    return (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer != NULL);
}


/*
Aborts an active receive operation on interface intfNum.
  Returns the number of bytes that were received and transferred
  to the data location established for this receive operation.
*/
BYTE USBCDC_abortReceive(WORD* size, BYTE intfNum)
{
    //interrupts disable
    unsigned short bGIE;

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status
    // atomic operation - disable interrupts
    __disable_interrupt();                   // Disable global interrupts
	
    *size = 0; //set received bytes count to 0

    //is receive operation underway?
    if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer)
    {
        //how many bytes are already received?
        *size = CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceive - CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft;

        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = 0;
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer = NULL;
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft = 0;
    }

     //restore interrupt status
    __bis_SR_register(bGIE); //restore interrupt status
    return kUSB_succeed;
}

/*
This function rejects payload data that has been received from the host.
*/
BYTE USBCDC_rejectData(BYTE intfNum)
{
    BYTE edbIndex;
    unsigned short bGIE;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status

    // atomic operation - disable interrupts
    __disable_interrupt();               // Disable global interrupts

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if (bFunctionSuspended)
    {
       __bis_SR_register(bGIE); //restore interrupt status
        return kUSBCDC_busNotAvailable;
    }

    //Is receive operation underway?
    // - do not flush buffers if any operation still active.
    if (!CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer)
    {
        BYTE tmp1 = tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & EPBCNT_NAK;
        BYTE tmp2 = tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & EPBCNT_NAK;

        if (tmp1 ^ tmp2) // switch current buffer if any and only ONE of buffers is full
        {
            //switch current buffer
            CdcReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY = (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].bCurrentBufferXY+1) &0x01;
        }

        tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX = 0;  //flush buffer X
        tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY = 0;  //flush buffer Y
        CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp = 0;                     // indicates that no more data available in the EP
    }

    __bis_SR_register(bGIE); //restore interrupt status
    return kUSB_succeed;
}

/*
This function indicates the status of the itnerface intfNum.
  If a send operation is active for this interface,
  the function also returns the number of bytes that have been transmitted to the host.
  If a receiver operation is active for this interface, the function also returns
  the number of bytes that have been received from the host and are waiting at the assigned address.

returns kUSBCDC_waitingForSend (indicates that a call to USBCDC_SendData()
  has been made, for which data transfer has not been completed)

returns kUSBCDC_waitingForReceive (indicates that a receive operation
  has been initiated, but not all data has yet been received)

returns kUSBCDC_dataWaiting (indicates that data has been received
  from the host, waiting in the USB receive buffers)
*/
BYTE USBCDC_intfStatus(BYTE intfNum, WORD* bytesSent, WORD* bytesReceived)
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
    if (CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft != 0)
    {
        ret |= kUSBCDC_waitingForSend;
        *bytesSent = CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSend - CdcWriteCtrl[INTFNUM_OFFSET(intfNum)].nCdcBytesToSendLeft;
    }

    //Is receive operation underway?
    if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pUserBuffer != NULL)
    {
        ret |= kUSBCDC_waitingForReceive;
        *bytesReceived = CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceive - CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesToReceiveLeft;
    }
    else // receive operation not started
    {
        // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
        if (!bFunctionSuspended)
        {
            if((tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & EPBCNT_NAK)  | //any of buffers has a valid data packet
               (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & EPBCNT_NAK))
            {
                ret |= kUSBCDC_dataWaiting;
            }
        }
    }

    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        // if suspended or not enumerated - report no other tasks pending
        ret = kUSBCDC_busNotAvailable;
    }

     //restore interrupt status
    __bis_SR_register(bGIE); //restore interrupt status

    __no_operation();
    return ret;
}

/*
Returns how many bytes are in the buffer are received and ready to be read.
*/
BYTE USBCDC_bytesInUSBBuffer(BYTE intfNum)
{
    BYTE bTmp1 = 0;
    unsigned short bGIE;
    BYTE edbIndex;
    edbIndex = stUsbHandle[intfNum].edb_Index;

    bGIE  = (__get_SR_register() &GIE);  //save interrupt status
    // atomic operation - disable interrupts
    __disable_interrupt();               // Disable global interrupts

    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
       __bis_SR_register(bGIE); //restore interrupt status
        // if suspended or not enumerated - report 0 bytes available
        return 0;
    }

    if (CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp > 0)         // If a RX operation is underway, part of data may was read of the OEP buffer
    {
        bTmp1 = CdcReadCtrl[INTFNUM_OFFSET(intfNum)].nBytesInEp;
        if (*CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 & EPBCNT_NAK) // the next buffer has a valid data packet
        {
            bTmp1 += *CdcReadCtrl[INTFNUM_OFFSET(intfNum)].pCT2 & 0x7F;
        }
    }
    else
    {
        if (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & EPBCNT_NAK) //this buffer has a valid data packet
        {
            bTmp1 = tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX & 0x7F;
        }
        if (tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & EPBCNT_NAK) //this buffer has a valid data packet
        {
            bTmp1 += tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY & 0x7F;
        }
    }

    __bis_SR_register(bGIE); //restore interrupt status
    return bTmp1;
}


//----------------------------------------------------------------------------
//  Line Coding Structure
//  dwDTERate     | 4 | Data terminal rate, in bits per second
//  bCharFormat   | 1 | Stop bits, 0 = 1 Stop bit, 1 = 1,5 Stop bits, 2 = 2 Stop bits
//  bParityType   | 1 | Parity, 0 = None, 1 = Odd, 2 = Even, 3= Mark, 4 = Space
//  bDataBits     | 1 | Data bits (5,6,7,8,16)
//----------------------------------------------------------------------------
VOID usbGetLineCoding0(VOID)
{
    abUsbRequestReturnData[6] = bDataBits;               // Data bits = 8
    abUsbRequestReturnData[5] = bParity;                 // No Parity
    abUsbRequestReturnData[4] = bStopBits;               // Stop bits = 1

    abUsbRequestReturnData[3] = lBaudrate >> 24;
    abUsbRequestReturnData[2] = lBaudrate >> 16;
    abUsbRequestReturnData[1] = lBaudrate >> 8;
    abUsbRequestReturnData[0] = lBaudrate;

    wBytesRemainingOnIEP0 = 0x07;                   // amount of data to be send over EP0 to host
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);  // send data to host
}


//----------------------------------------------------------------------------
VOID usbGetLineCoding1(VOID)
{
    abUsbRequestReturnData[6] = bDataBits;               // Data bits = 8
    abUsbRequestReturnData[5] = bParity;                 // No Parity
    abUsbRequestReturnData[4] = bStopBits;               // Stop bits = 1

    abUsbRequestReturnData[3] = lBaudrate >> 24;
    abUsbRequestReturnData[2] = lBaudrate >> 16;
    abUsbRequestReturnData[1] = lBaudrate >> 8;
    abUsbRequestReturnData[0] = lBaudrate;

    wBytesRemainingOnIEP0 = 0x07;                   // amount of data to be send over EP0 to host
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);  // send data to host
}

//----------------------------------------------------------------------------
VOID usbGetLineCoding2(VOID)
{
    abUsbRequestReturnData[6] = bDataBits;               // Data bits = 8
    abUsbRequestReturnData[5] = bParity;                 // No Parity
    abUsbRequestReturnData[4] = bStopBits;               // Stop bits = 1

    abUsbRequestReturnData[3] = lBaudrate >> 24;
    abUsbRequestReturnData[2] = lBaudrate >> 16;
    abUsbRequestReturnData[1] = lBaudrate >> 8;
    abUsbRequestReturnData[0] = lBaudrate;

    wBytesRemainingOnIEP0 = 0x07;                   // amount of data to be send over EP0 to host
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);  // send data to host
}
//----------------------------------------------------------------------------

VOID usbSetLineCoding0(VOID)
{
    usbReceiveDataPacketOnEP0((PBYTE) &abUsbRequestIncomingData);     // receive data over EP0 from Host
}

//----------------------------------------------------------------------------

VOID usbSetLineCoding1(VOID)
{
    usbReceiveDataPacketOnEP0((PBYTE) &abUsbRequestIncomingData);     // receive data over EP0 from Host
}

//----------------------------------------------------------------------------

VOID usbSetLineCoding2(VOID)
{
    usbReceiveDataPacketOnEP0((PBYTE) &abUsbRequestIncomingData);     // receive data over EP0 from Host
}

//----------------------------------------------------------------------------

VOID usbSetControlLineState(VOID)
{
    usbSendZeroLengthPacketOnIEP0();    // Send ZLP for status stage
}

//----------------------------------------------------------------------------

VOID Handler_SetLineCoding0(VOID)
{
    // Baudrate Settings

    lBaudrate = (ULONG)abUsbRequestIncomingData[3] << 24 | (ULONG)abUsbRequestIncomingData[2]<<16 |
      (ULONG)abUsbRequestIncomingData[1]<<8 | abUsbRequestIncomingData[0];
}

//----------------------------------------------------------------------------

VOID Handler_SetLineCoding1(VOID)
{
    // Baudrate Settings

    lBaudrate = (ULONG)abUsbRequestIncomingData[3] << 24 | (ULONG)abUsbRequestIncomingData[2]<<16 |
      (ULONG)abUsbRequestIncomingData[1]<<8 | abUsbRequestIncomingData[0];
}

//----------------------------------------------------------------------------

VOID Handler_SetLineCoding2(VOID)
{
    // Baudrate Settings

    lBaudrate = (ULONG)abUsbRequestIncomingData[3] << 24 | (ULONG)abUsbRequestIncomingData[2]<<16 |
      (ULONG)abUsbRequestIncomingData[1]<<8 | abUsbRequestIncomingData[0];
}
#endif //ifdef _CDC_

/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
