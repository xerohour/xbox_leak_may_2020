// AWUSBAPI.H : header file
//

#if !defined(AFX_ACTIVEWIREUSB_H__C5F11600_F0F9_11D2_AC25_00104B306BEE__INCLUDED_)
#define AFX_ACTIVEWIREUSB_H__C5F11600_F0F9_11D2_AC25_00104B306BEE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define	DEVROOTNAME		"Awusb-"
#define AWUSB_DLL_NAME	"awusb.dll"

#ifdef AWUSB_DLL_EXPORT
#define __AWUSB_DLL		__declspec(dllexport) 
#else
#define	__AWUSB_DLL		__declspec(dllimport) 
#endif

// typedef
typedef struct _AWUSB_INTELHEX_T
{
	BYTE	count;
	WORD	addr;
	BYTE	type;
	BYTE	data[256];
	BYTE	crc;
} 
AWUSB_INTELHEX;

#define AWUSB_MAX_NUMPIPE	32
typedef struct _AWUSB_PIPEINFO_T
{
	BYTE	numpipe;
	BYTE	type[AWUSB_MAX_NUMPIPE];
	BYTE	endpoint[AWUSB_MAX_NUMPIPE];
	BYTE	dir[AWUSB_MAX_NUMPIPE];
	WORD	size[AWUSB_MAX_NUMPIPE];
} 
AWUSB_PIPEINFO;


// Function prototype
// High level Application interface
__AWUSB_DLL	DWORD	AwusbOutPort (BYTE *data, DWORD count);
__AWUSB_DLL	DWORD	AwusbInPort (BYTE *data, DWORD count);
__AWUSB_DLL	DWORD	AwusbEnablePort (BYTE *data, DWORD count);

// Low level USB interface
__AWUSB_DLL	DWORD	AwusbOpen (DWORD devnum);
__AWUSB_DLL	DWORD	AwusbClose ();
__AWUSB_DLL	DWORD	AwusbSetInterface (DWORD intfc, DWORD altset);
__AWUSB_DLL	DWORD	AwusbRead (DWORD pipenum, BYTE *buf, DWORD count);
__AWUSB_DLL	DWORD	AwusbWrite (DWORD pipenum, BYTE *buf, DWORD count);
__AWUSB_DLL	DWORD	AwusbCancelIo (DWORD pipenum);
__AWUSB_DLL	DWORD	AwusbGetPipeInfo (AWUSB_PIPEINFO *pipeinfo);
__AWUSB_DLL	DWORD	AwusbResetPipe (DWORD pipenum);
__AWUSB_DLL	DWORD	AwusbDownload (AWUSB_INTELHEX hexrec);
__AWUSB_DLL	DWORD	AwusbReadIntelHexLine (char *buf, AWUSB_INTELHEX *hexrec);
__AWUSB_DLL	DWORD	AwusbDownloadIntelHexFile (char *filename);
__AWUSB_DLL	DWORD	Awusb8051Reset (BOOL hold);

// Error handling
__AWUSB_DLL	char *	AwusbErrorMessage (DWORD errcode);


// Error Code
enum AWUSB_ERROR_CODE_T {
	AWUSB_OK	= 0,			// success
	AWUSB_ERROR_FATAL,			// Fatal error, cannot continue
	AWUSB_ERROR_SYSTEM,			// System error, use WIN32 GetLastError() for further error code
	AWUSB_ERROR_MEMORY,			// Run out of memory
	
	AWUSB_ERROR_FILEOPEN,		// File open failure
	AWUSB_ERROR_FILEWRITE,		// File write failure
	AWUSB_ERROR_FILEREAD,		// File read failure
	AWUSB_ERROR_FILECLOSE,		// File close failure
	AWUSB_ERROR_FILEFORMAT,		// File format error
	AWUSB_ERROR_FILECHECKSUM,	// File checksum error
	AWUSB_ERROR_FILEEOF,		// Unexpected end of file 

	AWUSB_ERROR_HARDWARE,		// Hardware error, such as the device unplugged

	AWUSB_ERROR_SOFTWARE,		// Software error, possibly a bug...
	AWUSB_ERROR_NOTIMPLEMENTED,	// Not implemented yet...
	AWUSB_ERROR_NOSUPPORT,		// Not supported

	AWUSB_ERROR_USBNOTOPEN,		// Not opend yet
	AWUSB_ERROR_USBNOTCLOSE,	// Not closed yet

	AWUSB_ERROR_USBBADPIPE,		// Bad USB pipe
	AWUSB_ERROR_USBBADOPCODE,	// Bad USB Command/Status Opcode
	AWUSB_ERROR_USBZEROPACKET,	// Zero length USB data packet
	AWUSB_ERROR_USBSHORTPACKET,	// Short USB data packet
	AWUSB_ERROR_USBLONGPACKET,	// Longer USB data packet

	AWUSB_ERROR_TIMEOUT,		// Time out, may or may not a problem...
	AWUSB_ERROR_TRYAGAIN,		// Don't get too serious, try again, may work next time
	AWUSB_ERROR_UNKNOWN,		// Unknown... Truth is out there...   :)
};

#endif // !defined(AFX_ACTIVEWIREUSB_H__C5F11600_F0F9_11D2_AC25_00104B306BEE__INCLUDED_)
