#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>
#include <configmg.h>
#include <vcomm.h>
#include <nvtypes.h>
#include <osdbg.h>
#include <serial.h>

#ifdef DEBUG

//
// Baud Rate Stuff
//
NvU32 dbgBaudRateTable[] = {
   1200, 96,
   2400, 48,
   3600, 32,
   4800, 24,
   7200, 16,
   9600, 12,
   19200, 6,
   38400, 3,
   57600, 2,
   115200, 1,
   230400, 32770,
   460800, 32769,
};

//
// PIO Offset
//
#define DBG_USE_COM_PORT_1 (0)
#define DBG_USE_COM_PORT_2 (1)
NvU16 dbgComPortTable[]={DBG_COM1_PORT, DBG_COM2_PORT};

DBGPORT DbgPort = {
   DBG_OBJECT_SERIAL, DBG_USE_COM_PORT_1, 115200,
   };

//
// Simple BaudRate Lookup Table Function
//
NvU16 dbgBaudRate(NvU32 uBaudRate)
{
   NvS32 i;
   NvU16 uReturn = 0x0c;
   
   for (i=0; i<sizeof(dbgBaudRateTable)/sizeof(NvU32); i+=2)
      {
      if (uBaudRate == dbgBaudRateTable[i])
         {
         uReturn = (NvU16)dbgBaudRateTable[i+1];
         break;
         }
      }

   return uReturn;
}


//
// Simple Init Serial Function
//
NvU32 nFirst=TRUE;
NvU16 dbgComPort = DBG_COM1_PORT;
NvU16 uBaudRate;
//
//  This is a cheap hack to fix a problem with hibernation
//  when we go into hibernation Serenum is called to reenum
//  the serial port and change it's baud rate
//  We check for this condition here
//
void ChkBaudRate(void)
{
   NvU16 ChkRate;

   //Make sure that we have the right baud rate
   // sometimes on returns from hibernation Serenum resets us to 9600 baud.. yuch
   // this is a quick check
   if (0x03 != (INB((NvU16)(dbgComPort+LCR_OFFSET))))
      {
      _asm {int 03};
		dbgInitSerial(&DbgPort);
      }
   else
      {

      // this is a long slow check but a bit more intensive
      // make sure baud rate is right
      OUTB((NvU16)(dbgComPort+LCR_OFFSET), 0x83);
      ChkRate = INB((NvU16)(dbgComPort+DHI_OFFSET));
      ChkRate = (ChkRate<<8) | INB((NvU16)(dbgComPort+DLOW_OFFSET));
      if (uBaudRate != ChkRate)
         {
         _asm {int 03};
   		dbgInitSerial(&DbgPort);
         }
      OUTB((NvU16)(dbgComPort+LCR_OFFSET), 0x03);
      }

}

VOID dbgInitSerial(PDBGPORT pDbgPort)
{
   WIN32DCB DCB;
   DWORD ulLength;
   extern DEVNODE dbgDevNode;

   uBaudRate = dbgBaudRate(pDbgPort->uTag.Serial.uBaud);

   // First Time do an enable
   if (nFirst)
      {
      dbgEnableDevice(pDbgPort->uTag.Serial.uPort);
      nFirst=FALSE;
      }

   dbgComPort = dbgComPortTable[pDbgPort->uTag.Serial.uPort];

   //
   //  Default is 8,n,1
   //
   OUTB((NvU16)(dbgComPort+ICR_OFFSET), 0x0);
   OUTB((NvU16)(dbgComPort+FIFO_OFFSET), 0x07);
   OUTB((NvU16)(dbgComPort+LCR_OFFSET), 0x83);
   OUTB((NvU16)(dbgComPort+DLOW_OFFSET), (NvU8)(uBaudRate & 0xFF));
   OUTB((NvU16)(dbgComPort+DHI_OFFSET), (NvU8)(uBaudRate>>8));
   OUTB((NvU16)(dbgComPort+LCR_OFFSET), 0x03);
   OUTB((NvU16)(dbgComPort+MCR_OFFSET), 0x0);
   OUTB((NvU16)(dbgComPort+SPARE_OFFSET), 0xAA);

   //
   // hack#9
   // we are in a fight with VCOMM for control of the serial port
   // when we input the baud, etc, they will try and change it.
   // we need to fix this here
   //
   ulLength = sizeof(DCB);
   if (CR_SUCCESS == CMxx_Read_Registry_Value(dbgDevNode, NULL, "DCB", REG_BINARY, &DCB, &ulLength, CM_REGISTRY_SOFTWARE))
      {
      // Ditto 8,n,1
      DCB.BaudRate = pDbgPort->uTag.Serial.uBaud;
      DCB.fFlags &= ~(DBG_FFLAGS_PARITY);
      DCB.ByteSize = 8;
      DCB.Parity = 0x0;
      DCB.StopBits = 0x0;
      CMxx_Write_Registry_Value(dbgDevNode, NULL, "DCB", REG_BINARY, &DCB, ulLength, CM_REGISTRY_SOFTWARE);
      } 

}


//
// Simple Poll Method to transmit string
// This is simple so the message is not lost if IRQ were used
//
VOID dbgDisplaySerial(PSTR pStr)
{
   NvU32 dbgCurrentState;
   NvU32 ulStateChange;
   extern NvU32 IsConfig98;
   extern NvU32 dbgPowerSysState;
   extern DEVNODE dbgDevNode;


   // If we are in a low power mode then turn the serial port on
   ulStateChange = FALSE;
   if (dbgPowerSysState)
      {
      if (IsConfig98)
         {
         if (CR_SUCCESS == CMxx_Get_DevNode_PowerState(dbgDevNode, &dbgCurrentState, 0x0))
            {
            if (CM_POWERSTATE_D0 != dbgCurrentState)
               {
               ulStateChange = TRUE;
               // Power On
               CMxx_Set_DevNode_PowerState(dbgDevNode, CM_POWERSTATE_D0, 0x0);
               // Initialize
               }
            dbgInitSerial(&DbgPort);
            }
         }
      }

   // Check to make sure baudrate is right
   ChkBaudRate();

	for (;*pStr!='\0'; pStr++)
      {
      while (0x20 != (INB((NvU16)(dbgComPort+LSR_OFFSET))&0x20))
         ;
      OUTB((NvU16)(dbgComPort+DATA_OFFSET), *pStr);
      if (0x0A == *pStr)
          {
          while (0x20 != (INB((NvU16)(dbgComPort+LSR_OFFSET))&0x20))
            ;
          OUTB((NvU16)(dbgComPort+DATA_OFFSET), 0x0D);
          }
      }

   // Turn if off if we are not going to D0
   if (dbgPowerSysState)
      {
      if (IsConfig98)
         {
         if (1 == dbgPowerSysState)
            {
            if (ulStateChange)
               CMxx_Set_DevNode_PowerState(dbgDevNode, dbgCurrentState, 0x0);
            }
         else
            {
            dbgPowerSysState = 0x0;
            dbgEnableDevice2(&DbgPort);
            }
         }
      }

}

#endif
