
/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    i2clib.h

Abstract:

    This module contains routines that speak to access
	the i2c temp chip and the SMC.
	The module is compiled into a static lib.

Author:

    Seth Milman (sethmil) 12/22/00

Environment:

    Windows

Revision History:


  12/22/00		created			sethmil


--*/



#ifndef _I2CLIB_H__
#define _I2CLIB_H__


/////////////////////////////////////////////////////
// 
// include files
//

#ifdef _XBOX
	#include <xtl.h>
#else
	#include <windows.h>
#endif
//#include <stdio.h>
//#include "cali2c32.h"




 
////////////////////////////////////////////////////
// constants
//
// Note, I2C Error codes have bit 16 set.
////////////////////////////////////////////////////

static const int NOACK				= 1;
static const int ACK				= 0;
static const int I2CBOARD			= 0;
static const DWORD SMCERROR			= 0xB001;
static const DWORD SMCWRITEERROR	= 0xB002;
static const DWORD SMCREADERROR		= 0xB003;




static const enum Frequency
{
	FREQ90K = 0,
	FREQ45K = 1,
	FREQ10K = 2
};



//
// SMC read registers
//
static const enum SMCReg
{
	FWVER				= 0x01,
	RESET				= 0x02,
	TRAYSTATE			= 0x03,
	VMODE				= 0x04,
	FANOVERRIDE			= 0x05,
	REQFANSPEED			= 0x06,
	LEDOVERRIDE			= 0x07,
	LEDSTATES			= 0x08,
	CPUTEMP				= 0x09,
	AIRTEMP				= 0x0A,
	AUDIOCLAMP			= 0x0B,
	DVDTRAYOP			= 0x0C,
	OSRESUME			= 0x0D,
	ERRORCODE_W			= 0x0E,
	ERRORCODE_R			= 0x0F,
	READFANSPEED		= 0x10,
	SMICODE				= 0x11,
	RAMTESTRESULTS_W	= 0x12,
	RAMTYPE_W			= 0x13,
	RAMTESTRESULTS_R	= 0x14,
	RAMTYPE_R			= 0x15,
	LASTREGWRITTEN		= 0x16,
	LASTBYTEWRITTEN		= 0x17,
	FORCESMI			= 0x18,
	TRAYRESETOVERRIDE	= 0x19,
	OSREADY				= 0x1A

};


static const enum AMDReg
{
	AMDAIRTEMP = 0x00,
    AMDCPUTEMP = 0x01
};

//
// I2C Error Codes
//
// note SMC error codes have bit 16 set
//
static const UINT SETUPERROR		= 0x9000;
static const UINT SENDADDRESSERROR	= 0x8001;
static const UINT WRITEBYTEERROR	= 0x8004;
static const UINT SENDSTOPERROR		= 0x8002;
static const UINT READBYTEERROR		= 0x8005;
static const UINT RESTARTERROR		= 0x8003;
static const UINT RECOVERERROR		= 0x8006;

static const UINT SLAVEERROR			= 0xA001;
static const UINT MASTERERROR			= 0xA002;
static const UINT SLAVENOTRESPONDING	= 0xA003;
static const UINT I2CERROR				= 0xA004;




#define I2CERROR(Status) ((DWORD)(Status) & 0x8000)

//
// SMC Status Bits
//		See PICA documentation for the definition of these bits
//
//		PIN = Pending Inerrupt Not - used for polling for recieved data
//				Transmitting: =0 when data has been sent, 1 when sending
//				Recieving:	  =0 when data has been recieved, 1 other times
//      STS = used for stop condition detection
//				1 when externally generated stop detected
//				0 other times
//		BER = bus error bit
//				1 when misplaces stop or start detected
//				0 other times
//		LRB = last recieved bit - when in master mode, used to detect slave's ack
//				= value is equal to slave acknowledge
//		ADO = address 0 - when in slave mode, used to detect a call to addr 0 (General call address)
//				1 if addressed as general address (address 0)
//		AAS = addressed as slave - used to determine if the card has been addressed to its slave address
//				1 if addressed as slave
//				0 other times
//		LAB = lost arbitration - in multimaster mode, used to detect loss of arbitration.
//				1 if lost arbitration
//				0 other times
//		BB  = bus busy bit - =1 if bus is busy, = 0 if bus is free
//				0 if bus is busy
//				1 if bus is free
//
static const DWORD		PIN = 0x80;
static const DWORD		STS = 0x20;
static const DWORD		BER = 0x10;
static const DWORD		ADO = 0x08;
static const DWORD		LRB = 0x08;
static const DWORD		AAS = 0x04;
static const DWORD		LAB = 0x02;
static const DWORD		BB  = 0x01;


//
// SMC addresses
//
static const DWORD SMCRADDR = 0x21;
static const DWORD SMCWADDR = 0x20;

static const DWORD AMDRADDR = 0x99;
static const DWORD AMDWADDR = 0x98;


//*******************************************************************
// Other SMC/I2C consts and enums


#ifdef _XBOX
	extern "C"
		{
		DECLSPEC_IMPORT
		LONG
		WINAPI
		HalReadSMBusValue(
			IN UCHAR SlaveAddress,
			IN UCHAR CommandCode,
			IN BOOLEAN ReadWordValue,
			OUT ULONG *DataValue
			);

		DECLSPEC_IMPORT
		LONG
		WINAPI
		HalWriteSMBusValue(
			IN UCHAR SlaveAddress,
			IN UCHAR CommandCode,
			IN BOOLEAN WriteWordValue,
			IN ULONG DataValue
			);
		}


	void WriteSMC(unsigned char reg, unsigned char value)
		{
		HalWriteSMBusValue(SMCWADDR, reg, FALSE, value);
		}


	DWORD ReadSMC(unsigned char reg)
		{
		DWORD value = 0xCCCCCCCC;
		HalReadSMBusValue(SMCRADDR, reg, FALSE, &value);
		return value;
		}


	void WriteAMD( unsigned char reg, unsigned char value)
	{
		HalWriteSMBusValue( AMDWADDR, reg, FALSE, value );
	}


	DWORD ReadAMD( unsigned char reg )
	{
		DWORD value = 0xCCCCCCCC;
		HalReadSMBusValue(AMDRADDR, reg, FALSE, &value );
		return value;
	}
#endif



////////////////////////////////////////////////////////////////
//
// Generic I2C functions
//
////////////////////////////////////////////////////////////////



	/****************
		sets up the I2C board
		addr - board address - bits 7..1 are used to set the address
			bit 0 is used as the R/!W bit
		freq - board Frequency

		sets up addresses

		returns error code
	*/
	DWORD I2CSetup( const UINT addr, const Frequency f );



	/*
		writes an address byte out
		to the I2C Bus

		returns error code
	*/
	DWORD I2CSendAddress( const UINT addr, const UINT ACK );


	/* 
		writes a byte of data out to the 
		I2C Bus

		returns error code
	*/
	DWORD I2CWriteByte( const UINT DATA );


	/* 
		reads a byte of data out to the 
		I2C Bus

		returns the byte
	*/
	DWORD I2CReadByte( const UINT ack );
	

	/*
		sends a stop to the I2C Bus
		
		return error code
	*/
	DWORD I2CSendStop();


	/*
		Restarts the I2CBoard

		returns error code
	*/
	DWORD I2CRestart( const UINT addr , const UINT ack );

	/*
		gets status of the I2CBoard

		returns status
	*/
	DWORD I2CGetStatus(  );

	/*
		tries to recover the I2CBoard

		returns error code
	*/
	DWORD I2CRecover();


	/* 
		returns the last slave byte read
	*/
	DWORD I2CSlaveLastByte( );


	/*
		dll issue??
	*/
	DWORD I2CDLLIssue();
	
	/*
		writes bytes to SMC address addr.
		writes "LENGHT" bytes from the lpzData string.  
		lpzData is a null terminated string
	*/
	DWORD I2CWrite( const TCHAR* lpzData, const UINT LENGTH, const UINT ADDR );



/////////////////////////////////////////////////////////////////////////////////////////////
//
// I2C Transaction Functions
//
/////////////////////////////////////////////////////////////////////////////////////////////
	/*
		uses Send Bytes SMC protocol to write an address
		and 1 byte of data

		Error Codes:
		returns SLAVENOTRESPONDING if the slave fails to respond
		returns SENDADDRESSERROR if I2CSendAddress returns an error.
		returns READBYTEERROR if read byte error occurred
		returns SENDSTOPERROR if send stop error occurred
	*/
	DWORD I2CSendByteTransaction( DWORD* dwDataArray, const DWORD dwArrayLength, 
							 const UINT uiAddr, const UINT uiData, const DWORD dwWaitCount );


	/*
		uses WriteByte protocol to write an
		address, command code, and data to a device
		on the SMBus.

		returns SLAVENOTRESPONDING if the slave fails to respond
		returns SENDADDRESSERROR if I2CSendAddress returns an error.
		returns WRITEBYTEERROR if write byte error occurred
		returns SENDSTOPERROR if send stop error occurred
	*/
	DWORD I2CWriteByteTransaction( DWORD* dwDataArray, 
								   const DWORD dwArrayLength, 
								   const UINT uiAddr, 
								   const UINT uiData1, 
								   const UINT uiData2,
								   const DWORD dwWaitCount );


	/*
		Master Mode
		Reads a byte from the slave using Receive Byte
		SMBus protocol

		returns SLAVENOTRESPONDING if the slave fails to respond
		returns SENDADDRESSERROR if I2CSendAddress returns an error.
		returns READBYTEERROR if read byte error occurred
		returns SENDSTOPERROR if send stop error occurred
	*/
	DWORD I2CReceiveByteTransaction( DWORD* dwDataArray,
								  const DWORD dwArrayLength,
								  const UINT uiAddr,
								  const DWORD dwWaitCount );



////////////////////////////////////////////////////////////////
//
// SMC Write functions
//
////////////////////////////////////////////////////////////////


	

	DWORD SMCWriteReg( SMCReg reg, DWORD data );

	/*
		write a reset to the SMC

		data: {0,0,0,0,0,0,0,assert_reset}
	*/

	DWORD SMCReset( DWORD data );



	/*
		write fan override data

		{0,0,0,0,0,0,0,Fan_override}
	*/
	DWORD SMCFanOverride( DWORD data );


	/*
		write fan speed to SMC

		data: RequestedFanSpeed7..0
	*/
	DWORD SMCRequestFanSpeed( DWORD data );


	/*
		writes LED override data

		data: {0,0,0,0,0,0,0,LED_override}
	*/
	DWORD SMCLEDOverride( DWORD data );


	/*
		writes Audio Clamp register

		data: {0,0,0,0,0,Audio_release_now, Audio_off}
	*/
	DWORD SMCAudioClamp( DWORD data );


	/*
		writes DVDTrayOp register

		data: {0,0,0,0,0,0,
				OS_DVD_open_request,
				OS_DVD_close_request}
	*/
	DWORD SMCDVDTrayOp( DWORD data );



	/*
		write OSResume register

		data: {0,0,0,0,0,
				OS_resume,
				OS_shutdown_in_progress,
				OS_shutdown_complete
				}
	*/
	DWORD SMCOSResume( DWORD data );


	/*
		writes ErrorCode register

		data: TBD
	*/
	DWORD SMCWriteErrorCode( DWORD data );




////////////////////////////////////////////////////////////////
//
// I2C/SMC Read functions
//
////////////////////////////////////////////////////////////////


	/* 
		I2C read function
		reads "length" bytes from the SMC

		returns -1 if failed
	*/
	INT I2CRead( TCHAR* lpData, const UINT LENGTH, const UINT ADDR, const UINT REG );

	
	/* 
		SMC read function
		reads "numbytes" bytes from the SMC

		returns -1 if failed
	*/
	INT SMCRead( TCHAR* lpData, const UINT LENGTH, const SMCReg REG );


	/* 
		SMC read byte function
		reads 1 byte from the SMC

		returns the byte read from the SMC
		returns -1 if failed
	*/
	INT SMCReadByte(  );


	/*
	//
	// DWORD SMCGetFWVersion( TCHAR* lpsFWVer, DWORD addr )
	//
	// parameters:
	//
	//  lpsFWVer - pointer to string that will contain
	//		the firmware version
	//  addr - address of the SMC
	//
	*/
	DWORD SMCGetFWVersion( TCHAR* lpsFWVer );


	/*
		reads tray state
		passes Tray State back as a LPS parameter
	*/
	DWORD SMCGetTrayState( TCHAR* lpsTrayState );


	/*
		reads video mode
		passes video mode as a parameter
	*/
	DWORD SMCGetVMode( TCHAR* lpsVMode );


	/*
		reads CPU temp
		returns CPU temp as a parameter
	*/
	DWORD SMCGetCPUTemp( TCHAR* lpsTemp );



	/* 
		reads Air Temp
		returns Air Temp as a parameter
	*/
	DWORD SMCGetAirTemp( TCHAR* lpsTemp );



	/* 
		reads ErrorCode Register

		returns ErrorCode as a parameter
	*/
	DWORD SMCGetErrorCode( TCHAR* lpsErrorCode );



///////////////////////////////////////////////////////////
//
// I2C Status Functions
//
//////////////////////////////////////////////////////////

	/*
		Name: I2CDataReady
		Desc: gets status of bus and returns TRUE if there is slave
				data ready to read
	*/
	BOOL	I2CDataReady();


	/*
		Name: I2CExternalStopDetected()
		Desc: When in slave mode, returns TRUE if a STOP was detected
				from the master
	*/
	BOOL	I2CExternalStopDetected();


	/* 
		Name: I2CBusError()
		Desc: Returns TRUE if the bus error status bit is set
	*/
	BOOL	I2CBusError();


	/*
		Name: I2CSlaveAck()
		Desc: Returns TRUE if an ACK was recieved from the slave
	*/
	BOOL	I2CSlaveAck();

	/*
		Name: I2CGeneralCallAddress
		Desc: returns TRUE if the slave address recieved was the 
				General Call Address
	*/
	BOOL	I2CGeneralCallAddress();


	/* 
		Name: I2CAddressedAsSlave
		Desc: Returns TRUE if the test card is acting as a slave
	*/
	BOOL	I2CAddressedAsSlave();


	/* 
		Name: I2CLostArbitration()
		Desc: In multimaster mode, returns TRUE if lost bus arbitration
	*/
	BOOL	I2CLostArbitration();


	/* 
		Name: I2CBusBusy()
		Desc: Returns TRUE if the I2C Bus is busy
	*/
	BOOL	I2CBusBusy();
	


//******************************************************************************
//
// SMC Functions
//
//******************************************************************************



//**********************************************************************************
// this function acts as a slave for transactions where the SMC is the master
//
// returns the number of bytes read from the bus
// dwDataArray is a pointer to a DWORD array.
// dwArrayLength is the lengto of the data array
// dwTemp is the temperature that the function will write to the SMC
// dwWaitCount is a counter.  The counter is used to determine the length of time
//			the function will wait before writing a byte.
//
// requires that the PIC93 card has been initialized with I2CSetup()
//
// returns READBYTEERROR if a read byte error occurred.
// returns WRITEBYTERROR if a write byte error occurred.
// *********************************************************************************


DWORD SMCSlaveTransaction( DWORD* dwDataArray, const DWORD dwArrayLength, 
						  const DWORD dwTemp, const DWORD dwWaitCount );


#endif // _I2CLIB_H__