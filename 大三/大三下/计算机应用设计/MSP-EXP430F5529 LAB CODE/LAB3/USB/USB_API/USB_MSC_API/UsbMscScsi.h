// (c)2010 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
  |                                                                             |
  |                              Texas Instruments                              |
  |                                                                             |
  |                          MSP430 USB-Example (MSC Driver)                    |
  |                                                                             |
  +-----------------------------------------------------------------------------+
  |  Source: Msc_Scsi.h, File Version 1.02                                      |
  |  Description: This file contains all the structure,function declarations    |
  |               used by stack                                                 |
  |  Author: Biju,MSP                                                           |
  |                                                                             |
  |  WHO          WHEN         WHAT                                             |
  |  ---          ----------   ------------------------------------------------ |
  |  MSP          2010/02/16   Created                                          |
  |  Biju,MSP     2010/07/15   CV bug fix                                       |
  +----------------------------------------------------------------------------*/
#ifndef _UMSC_SCSI_H_
#define _UMSC_SCSI_H_

#include <descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*Macros for CBW, CSW signatures */
#define CBW_SIGNATURE 0x43425355u
#define CSW_SIGNATURE 0x53425355u

/*CBW, CSW length in bytes */
#define CBW_LENGTH   31
#define CSW_LENGTH   13

/*SCSI Commands - Mandatory only implemented */
#define SCSI_TEST_UNIT_READY            0x00
#define SCSI_REQUEST_SENSE 	        0x03
#define SCSI_INQUIRY		        0x12
#define SCSI_MODE_SENSE_6	        0x1A
#define SCSI_MODE_SENSE_10	        0x5A
#define SCSI_READ_CAPACITY_10           0x25
#define SCSI_READ_10		        0x28
#define SCSI_WRITE_10		        0x2A
#define SCSI_READ_FORMAT_CAPACITIES     0x23
#define SCSI_MODE_SELECT_6              0x15
#define SCSI_MODE_SELECT_10             0x55
#define PREVENT_ALLW_MDM                0x1E
#define START_STOP_UNIT                 0x1B
#define SCSI_REPORT_LUNS                0xA0
#define SCSI_VERIFY                     0x2F

/*SCSI Status codes. Used in CSW response */
#define SCSI_PASSED 		  0
#define SCSI_FAILED 		  1
#define SCSI_PHASE_ERROR 	  2
#define SCSI_READWRITE_FAIL       2

#define kUSBMSC_RWSuccess           0
#define kUSBMSC_RWNotReady          1
#define kUSBMSC_RWIllegalReq        2
#define kUSBMSC_RWUnitAttn          3
#define kUSBMSC_RWLbaOutOfRange     4
#define kUSBMSC_RWMedNotPresent     5
#define kUSBMSC_RWDevWriteFault     6
#define kUSBMSC_RWUnrecoveredRead   7
#define kUSBMSC_RWWriteProtected    8


  /* Macros to indicate READ or WRITE operation */
#define kUSBMSC_READ 1
#define kUSBMSC_WRITE 2

#define kUSBMSC_MEDIA_PRESENT 0x81
#define kUSBMSC_MEDIA_NOT_PRESENT 0x82

#define kUSBMSC_WRITE_PROTECTED 0x00

/* Defines for MSC SCSI State-Machine */
#define MSC_READY 			        0x00
#define MSC_COMMAND_TRANSPORT 		0x01
#define MSC_DATA_IN			        0x02
#define MSC_DATA_OUT		        0x03
#define MSC_STATUS_TRANSPORT		0x04
#define MSC_DATA			        0x05
#define MSC_WAIT4RESET              0x06

/*Lengths of SCSI commands(in bytes) */
#define SCSI_SCSI_INQUIRY_CMD_LEN            36
#define SCSI_READ_CAPACITY_CMD_LEN           8
#define SCSI_MODE_SENSE_6_CMD_LEN            4
#define SCSI_MODE_SENSE_10_CMD_LEN           8
#define SCSI_REQ_SENSE_CMD_LEN               18
#define SCSI_READ_FORMAT_CAPACITY_CMD_LEN    12
#define SCSI_REPORT_LUNS_CMD_LEN             16
/*----------------------------------------------------------------------------+
| Type defines and structures                                                 |
+----------------------------------------------------------------------------*/
/*CBW Structure */
typedef struct _CBW
{
    DWORD dCBWSignature;
    DWORD dCBWTag;
    DWORD dCBWDataTransferLength;
    BYTE  bmCBWFlags;
    BYTE  bCBWLUN;
    BYTE  bCBWCBLength;
    BYTE  CBWCB[16];
} CBW, *pCBW;

/*CSW structure */
typedef struct _CSW
{
    DWORD dCSWSignature;
    DWORD dCSWTag;
    DWORD dCSWDataResidue;
    BYTE  bCSWStatus;
} CSW, *pCSW;

/*Request Response union(Required for Request sense command) */
typedef struct
{
    BYTE ResponseCode:7;
    BYTE VALID:1;
    BYTE Obsolete;
    BYTE SenseKey:4;
    BYTE Resv:1;
    BYTE ILI:1;
    BYTE EOM:1;
    BYTE FILEMARK:1;
    BYTE Information[4];
    BYTE AddSenseLen;
    BYTE CmdSpecificInfo[4];
    BYTE ASC;
    BYTE ASCQ;
    BYTE FRUC;
    BYTE SenseKeySpecific[3];
    BYTE padding[14];	/* padding to cover case where host requests 24 bytes of sense data */

} REQUEST_SENSE_RESPONSE;

/*Read capacity union(Required for READ CAPACITY command)*/
typedef struct
{
    DWORD Last_LBA;
    BYTE Resv;
    BYTE Size_LBA[3];
} SCSI_READ_CAPACITY;

/*Structure internal to stack for holding LBA,buffer addr etc information*/
typedef struct
{
   // BYTE	intfNum;
    BYTE	lun;
    BYTE	operation;
    DWORD	lba;
    BYTE	lbCount;
    BYTE	*bufferAddr;
    BYTE	returnCode;
    BYTE	XorY;
}USBMSC_RWbuf_Info;

/*Structure exposed(shared) to application. Populated by stack */
struct LBAInfo
{
    BYTE	intfNum;
    BYTE	lun;
    DWORD   dLBA;
    int     iLBA_Count;
    BYTE    operation;
    BYTE	*bufferAddr;
    BYTE	returnCode;
    BYTE	XorY;
};

/*Media info structure */
struct USBMSC_mediaInfoStr
{
    DWORD   lastBlockLba;
    DWORD   bytesPerBlock;
    BYTE	mediaPresent;
    BYTE    mediaChanged;
    BYTE	writeProtected;
};

/*Lun entry Structures */
struct _LUN_entry_struct
{
   BYTE number;
   BYTE PDT;
   BYTE removable;
   char t10VID[8];
   char t10PID[16];
   char t10rev[4];
};

struct config_struct
{
    struct _LUN_entry_struct LUN[MSC_MAX_LUN_NUMBER];
};

struct _Report_Luns
{
    BYTE LunListLength[4];
    BYTE Reserved[4];
    BYTE LunList1[8];
};

struct _Scsi_Read_Capacity
{
    BYTE lLba[4];           // Last logical block address
    BYTE bLength[4];        // Block length, in this case 0x200 = 512 bytes for each Logical Block
};

//structure for controlling WRITE phase (HOST to MSP430)
struct _MscWriteControl
{
  DWORD dwBytesToReceiveLeft;        // holds how many bytes is still requested by WRITE operation:
                            // Host to MSP430.
    WORD wFreeBytesLeft;    // free bytes left in UserBuffer
    DWORD lba;              // holds the current LBA number. This is the first LBA in the UserBuffer
    BYTE *pUserBuffer;      // holds the current position of user's receiving buffer.
                            //If NULL- no receiving operation started
    WORD wCurrentByte;      // how many bytes in current LBA are received
    BYTE lbaCount;          // how many LBA we have received in current User Buffer
    BYTE * pCT1;            // holds current EPBCTxx register
    BYTE * pCT2;            // holds next EPBCTxx register
    BYTE * pEP2;            // holds addr of the next EP buffer
    BYTE bCurrentBufferXY;  // indicates which buffer is used by host to transmit data via OUT
    BYTE bWriteProcessing;  // indicated if the current state is DATA WRITE phase or CBW receiwing
};

//structure for controlling READ phase (MSP430 to HOST)
struct _MscReadControl
{
    DWORD dwBytesToSendLeft;// holds how many bytes is still requested by WRITE operation (Host to MSP430)
    BYTE *pUserBuffer;      // holds the current position of user's receiving buffer.
                            //If NULL- no receiving operation started
    DWORD lba;              // holds the current LBA number. This is the first LBA in the UserBuffer.
    BYTE * pCT1;            // holds current EPBCTxx register
    BYTE * pCT2;            // holds next EPBCTxx register
    BYTE * pEP2;            // holds addr of the next EP buffer
    BYTE lbaCount;          // how many LBA we have to send to Host
    BYTE bCurrentBufferXY;  // indicates which buffer is used by host to transmit data via OUT
    BYTE bReadProcessing;   // indicated if the current state is DATA READ phase or CSW sending
                            // initiated by McsDataSend()
    BYTE bIsIdle;           //
};

//structure for common control of MSC stack
struct _MscControl
{
    WORD wMscUserBufferSize;
    WORD lbaSize;           // limitid to WORD, but could be increased if required.
    BYTE lbaBufCapacity;    // how many LBAs (max) contains UserBuffer for read/write operation (>=1)
};


struct _CtrlLun
{
    BYTE bMediaPresent;
    BYTE bWriteProtected;
};
extern struct _MscWriteControl MscWriteControl;
extern struct _MscReadControl MscReadControl;
extern struct _MscControl MscControl;

/*----------------------------------------------------------------------------+
| Extern Variables                                                            |
+----------------------------------------------------------------------------*/
extern volatile DWORD Scsi_Residue;

extern  CBW cbw;
extern  CSW csw;
extern  REQUEST_SENSE_RESPONSE RequestSenseResponse;

/*----------------------------------------------------------------------------+
| Function Prototypes                                                         |
+----------------------------------------------------------------------------*/

/*SCSI Wrapper functions */
BYTE Scsi_Cmd_Parser(BYTE opcode);
BYTE Scsi_Send_CSW(BYTE intfNum);

/*Function to reset MSC SCSI state machine */
VOID Msc_ResetStateMachine(VOID);
VOID Msc_ResetFlags(VOID);
VOID Msc_ResetStruct(VOID);
VOID SET_RequestsenseNotReady(VOID);
VOID SET_RequestsenseMediaNotPresent(VOID);
VOID MscResetCtrlLun(VOID);

USBMSC_RWbuf_Info* USBMSC_fetchInfoStruct(VOID);
#ifdef __cplusplus
}
#endif
#endif // _MSC_SCSI_H_

