// AW.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include <awusbapi.h>
#include <stdio.h>
#include "aw.h"



static DWORD dwTlsIndex;		// address of shared memory


BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	LPVOID lpvData;
	//BOOL fIgnore;


    switch (ul_reason_for_call)
	{
		// The DLL is loading due to process 
        // initialization or a call to LoadLibrary. 
		case DLL_PROCESS_ATTACH:
			// allocate a tls index
			if(( dwTlsIndex = TlsAlloc()) == 0xFFFFFFFF)
			{
				return FALSE;
			}

			// no break.  initialize the index for 1st thread
		case DLL_THREAD_ATTACH:
			 
			/// Retrieve a data pointer for the current thread.
 
			lpvData = TlsGetValue(dwTlsIndex); 
 
			// If NULL, allocate memory for this thread.
 
			if (lpvData == NULL) { 
				lpvData = (LPVOID) LocalAlloc(LPTR, 256); 
				if (lpvData != NULL) 
					TlsSetValue(dwTlsIndex, lpvData); 
			} 
			break;
 
 

		case DLL_THREAD_DETACH:
			// Release the allocated memory for this thread.
 
            lpvData = TlsGetValue(dwTlsIndex); 
            if (lpvData != NULL) 
                LocalFree((HLOCAL) lpvData); 
 
            break; 
 
		case DLL_PROCESS_DETACH:
			// Release the allocated memory for this thread.
 
            lpvData = TlsGetValue(dwTlsIndex); 
            if (lpvData != NULL) 
                LocalFree((HLOCAL) lpvData); 
 
            // Release the TLS index.
 
            TlsFree(dwTlsIndex); 
            break; 
 
    }
    return TRUE;
	
}


/////////////////////////////////////////////////////////////////
// member functions for class CAWDevice
//

// default constructor
CAWDevice::CAWDevice()
{
	iDeviceNum = -1;
	dwErrCode = AW_OK;
}


// constructor with device num
// creates class and opens the device
CAWDevice::CAWDevice( DWORD devnum )
{
	dwErrCode = AwusbOpen( devnum );
	if( AW_OK != dwErrCode )
	{
		iDeviceNum = -1;
	}
	else
	{
		iDeviceNum = devnum;
	}
}


// destructor
CAWDevice::~CAWDevice()
{
	if( DeviceIsOpen() )
	{
		Close();
	}
}


// writes a word to the port
DWORD CAWDevice::OutPort( WORD data )
{
	BYTE d[2];
	d[0] = (BYTE)( data & 0x00FF );			// lower 8 bits
	d[1] = (BYTE)((data & 0xFF00) >>8);    // uper 8 bits

	dwErrCode = AwusbOutPort( d, 2 );
	return dwErrCode;
}


// reads a word from the port
//
DWORD CAWDevice::InPort( WORD* data )
{
	BYTE d[2];
	dwErrCode = AwusbInPort( d, 2 );

	*data  = (WORD)d[0];			// lower 8 bits
	*data |= ((WORD)d[1]) << 8;		// upper 8 bits

	return dwErrCode;
}

// enables the port for input/output
// for each bit:
//   1 = output
//   0 = input
DWORD CAWDevice::EnablePort( WORD data )
{
	BYTE d[2];
	d[0] = (BYTE)(data & 0x00FF);      // lower 8 bits
	d[1] = (BYTE)((data & 0xFF00)>>8); // upper 8 bits

	dwErrCode = AwusbEnablePort( d, 2 );
	return dwErrCode;
}


// sequentially writes bytes to the port
//  data is an array of BYTES
//  bytes with even array subscripts go to lower 8 bits
//  bytes with odd array sybscripts go to upper 8 bits
DWORD CAWDevice::OutPortEx( BYTE* data, DWORD count )
{
	dwErrCode = AwusbOutPort( data, count );
	return dwErrCode;
}


// sequentially reads bytes from the port
//  data is an array of BYTES
//  bytes with even array subscripts go to lower 8 bits
//  bytes with odd array sybscripts go to upper 8 bits
DWORD CAWDevice::InPortEx( BYTE* data, DWORD count )
{
	dwErrCode = AwusbInPort( data, count );
	return dwErrCode;
}


// sequentially enable output of the port
// for each bit, 1=output, 2 = input
//  data is an array of BYTES
//  bytes with even array subscripts go to lower 8 bits
//  bytes with odd array sybscripts go to upper 8 bits
DWORD CAWDevice::EnablePortEx( BYTE* data, DWORD count )
{
	dwErrCode = AwusbEnablePort( data, count );
	return dwErrCode;
}


// opens an AW device.
// if a device is already open, returns an error
// code 
DWORD CAWDevice::Open( DWORD devnum )
{
	// check if a device is open
	if( DeviceIsOpen() )
	{
		return AW_ERROR_USBNOTCLOSE;	// not closed yet
	}

	// now open the desired device
	dwErrCode = AwusbOpen( devnum );
	
	if( AW_OK == dwErrCode )
	{
		iDeviceNum = devnum;
	}

	return dwErrCode;
}


// closes the device
// if no device is open, this function returns an error
DWORD CAWDevice::Close()
{
	// check for a valid device
	if( !DeviceIsOpen() )
	{
		// device not opened
		return AW_ERROR_USBNOTOPEN;		// no open device
	}

	// otherwise, close the device
	dwErrCode = AwusbClose();

	// check for error
	if( AW_OK == dwErrCode )
	{
		iDeviceNum = -1;
	}
	return dwErrCode;
}

// returns the last error code
DWORD CAWDevice::GetLastError()
{
	return dwErrCode;
}


// sets the last error code
VOID CAWDevice::SetLastError( DWORD errcode )
{
	dwErrCode = errcode;
}


// returns a string represenation of the error code
char* CAWDevice::ErrorMessage(DWORD errcode)
{
	return AwusbErrorMessage( errcode );
}

// returns a string representation fo the last error code
char* CAWDevice::ErrorMessage()
{
	return AwusbErrorMessage( dwErrCode );
}


// returns true if a device is open
BOOL CAWDevice::DeviceIsOpen()
{
	return ( iDeviceNum >= 0 );
}











	





// writes a word to the port
//
AW_API DWORD AWOutPort( WORD data )
{
	BYTE	d[2];
	
	// break the word into upper and lower bits
	// we need to to this because of the way the ActiveWire USB API works.
	// see the ActiveWire USB SW reference for more information
	d[0] = (BYTE) ( data & 0x00FF );		// lower 8 bits
	d[1] = (BYTE) ( (data & 0xFF00) >> 8 );	// upper 8 bits

	return AwusbOutPort( d, 2 );
}



/* writes count bytes to the ports
 array elements with even subscripts are written to the lower
 8 bits of the port, elements with odd subscripts are written to
 the upper 8 bits of the port.  If count > 2, bytes are written consecutively to
 the port
*/
AW_API DWORD AWOutPortEx( BYTE* data, DWORD count )
{
	return AwusbOutPort( data, count );
}



// reads a word from the port
AW_API DWORD AWInPort ( WORD *data )
{
	DWORD	dwResult;
	BYTE	d[2];
	
	// get the input
	dwResult =  AwusbInPort( d, 2 );

	// pass the results back through the data param
	*data = (WORD)d[0];		// lower 8 bytes
	*data |= ((WORD)d[1]) << 8;

	return dwResult;

}



/* writes count bytes from the ports
 array elements with even subscripts are read from the lower
 8 bits of the port, elements with odd subscripts are read from
 the upper 8 bits of the port.  If count > 2, bytes are read consecutively from
 the port
*/
AW_API DWORD AWInPortEx ( BYTE* data, DWORD count )
{
	return AwusbInPort( data, count );
}


/* enables input or output of the port
	for each bit:
		write a 1 to make the bit an output
		write a 0 to make the bit an input
*/
AW_API DWORD AWEnablePort ( WORD data )
{
	BYTE	d[2];
	
	// break the word into upper and lower bits
	// we need to to this because of the way the ActiveWire USB API works.
	// see the ActiveWire USB SW reference for more information
	d[0] = (BYTE) ( data & 0x00FF );		// lower 8 bits
	d[1] = (BYTE) ( (data & 0xFF00) >> 8 );	// upper 8 bits

	return AwusbEnablePort( d, 2 );
}



/* enables input or output of the port
	for each bit:
		write a 1 to make the bit an output
		write a 0 to make the bit an input
	array elements with even subscripts enable the lower
	 8 bits of the port, elements with odd subscripts enable
	 the upper 8 bits of the port.  If count > 2, enable data is written
	 consecutively to the por
*/	
AW_API DWORD AWEnablePortEx ( BYTE* data, DWORD count )
{
	return AwusbEnablePort( data, count );
}


/*
	opens the USB device.  
	devnum is the device number
	0 is the 1st device attached, 1 is the second, etc

	each thread can open one device at a time.
*/
AW_API DWORD AWOpen ( WORD devnum )
{
	return AwusbOpen( devnum );
}


/*
	closes the device that is currently opened by the thread
*/
AW_API DWORD AWClose (  )
{
	return AwusbClose();
}



/*
	returns error string
*/
AW_API char * AWErrorMessage( DWORD errcode )
{
	return AwusbErrorMessage( errcode );
}




