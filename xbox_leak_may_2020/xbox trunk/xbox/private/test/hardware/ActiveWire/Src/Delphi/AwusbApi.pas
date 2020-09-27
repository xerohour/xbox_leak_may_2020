{ AwusbApi.pas : Delphi 5 unit file }
{ October 10, 1999  Timothy T. Orling <ttorling@ucdavis.edu>, TTO Engineering }
{ Translated from AwusbApi.h for Visual C++ }
{ Windows.pas used for guidance in translation }

unit AwusbApi;

interface

uses    Windows;

type
PAwusbIntelHex = ^TAwusbIntelHex;
_AWUSB_INTELHEX_T = record
        count: BYTE;
        addr: WORD;
        typeof: BYTE;
        data: array[0..255] of BYTE;
        crc: BYTE;
end;
TAwusbIntelHex = _AWUSB_INTELHEX_T;
AWUSB_INTELHEX = _AWUSB_INTELHEX_T;

const AWUSB_MAX_NUMPIPE = 32;

type
PAwusbPipeInfo = ^TAwusbPipeInfo;

_AWUSB_PIPEINFO_T = record
        numpipe: BYTE;
        typeof: array[0..AWUSB_MAX_NUMPIPE] of BYTE;
        endpoint: array[0..AWUSB_MAX_NUMPIPE] of BYTE;
        dir: array[0..AWUSB_MAX_NUMPIPE] of BYTE;
        size: array[0..AWUSB_MAX_NUMPIPE] of WORD;
end;
TAwusbPipeInfo = _AWUSB_PIPEINFO_T;
AWUSB_PIPEINFO = _AWUSB_PIPEINFO_T;

{ Function interfaces }
{ High level Application interface }
{ cdecl means that the DLL was written in C or C++ }
function AwusbOutPort(PData: PBYTE; count: DWORD): DWORD; cdecl;
function AwusbInPort(PData: PBYTE; count: DWORD): DWORD; cdecl;
function AwusbEnablePort(PData: PBYTE; count: DWORD): DWORD; cdecl;

{ Low level USB interface }
function AwusbOpen(devnum: DWORD): DWORD;cdecl;
function AwusbClose(): DWORD;cdecl;
{ WARNING: none of the following functions have been tested in Delphi except AwusbErrorMessage }
function AwusbSetInterface(intfc: DWORD; altset: DWORD): DWORD; cdecl;
function AwusbRead(pipenum: DWORD; buf: PBYTE; count: DWORD): DWORD; cdecl;
function AwusbWrite(pipenum: DWORD; buf: PBYTE; count: DWORD): DWORD; cdecl;
function AwusbCancelIo(pipenum: DWORD): DWORD; cdecl;
function AwusbGetPipeInfo(pipeinfo: PAwusbPipeInfo): DWORD; cdecl;
function AwusbResetPipe(pipenum: DWORD): DWORD; cdecl;
function AwusbDownload(hexrec: TAwusbIntelHex): DWORD; cdecl;
function AwusbReadIntelHexLine(buf: PChar; hexrec: PAwusbIntelHex): DWORD; cdecl;
function AwusbDownloadIntelHexfile(filename: PChar): DWORD; cdecl;
function Awusb8051Reset(hold: BOOL): DWORD; cdecl;

{ Error Handling }
function AwusbErrorMessage(errcode: DWORD): PChar; cdecl;

{ Error Code }
const
    AWUSB_OK = 0;                       { success }
	AWUSB_ERROR_FATAL = 1;			    { Fatal error, cannot continue }
	AWUSB_ERROR_SYSTEM = 2;		    	{ System error, use WIN32 GetLastError() for further error code }
	AWUSB_ERROR_MEMORY = 3;			    { Run out of memory }
	
	AWUSB_ERROR_FILEOPEN = 4;		    { File open failure }
	AWUSB_ERROR_FILEWRITE = 5;		    { File write failure }
	AWUSB_ERROR_FILEREAD = 6;		    { File read failure }
	AWUSB_ERROR_FILECLOSE = 7;		    { File close failure }
	AWUSB_ERROR_FILEFORMAT = 8;		    { File format error }
	AWUSB_ERROR_FILECHECKSUM = 9;	    { File checksum error }
	AWUSB_ERROR_FILEEOF = 10;		    { Unexpected end of file }

	AWUSB_ERROR_HARDWARE = 11;		    { Hardware error, such as the device unplugged }

	AWUSB_ERROR_SOFTWARE = 12;		    { Software error, possibly a bug... }
	AWUSB_ERROR_NOTIMPLEMENTED = 13;	{ Not implemented yet... }
	AWUSB_ERROR_NOSUPPORT = 14;		    { Not supported }

	AWUSB_ERROR_USBNOTOPEN = 15;		{ Not opened yet }
	AWUSB_ERROR_USBNOTCLOSE = 16;	    { Not closed yet }

	AWUSB_ERROR_USBBADPIPE = 17;		{ Bad USB pipe }
	AWUSB_ERROR_USBBADOPCODE = 18;	    { Bad USB Command/Status Opcode }
	AWUSB_ERROR_USBZEROPACKET = 19;	    { Zero length USB data packet }
	AWUSB_ERROR_USBSHORTPACKET = 20;	{ Short USB data packet }
	AWUSB_ERROR_USBLONGPACKET = 21;	    { Longer USB data packet }

	AWUSB_ERROR_TIMEOUT = 22;		    { Time out, may or may not a problem... }
	AWUSB_ERROR_TRYAGAIN = 23;		    { Don't get too serious, try again, may work next time }
	AWUSB_ERROR_UNKNOWN = 24;		    { Unknown... Truth is out there...   :)  }

implementation
{ Function implemetation }
{ High level Application interface }
function AwusbOutPort(PData: PBYTE; count: DWORD): DWORD; cdecl; external 'awusb.dll';
function AwusbInPort(PData: PBYTE; count: DWORD): DWORD; cdecl; external 'awusb.dll';
function AwusbEnablePort(PData: PBYTE; count: DWORD): DWORD; cdecl; external 'awusb.dll';

{ Low level USB interface }
function AwusbOpen(devnum: DWORD): DWORD; cdecl; external 'awusb.dll';
function AwusbClose(): DWORD; cdecl; external 'awusb.dll';
function AwusbSetInterface(intfc: DWORD; altset: DWORD): DWORD; cdecl; external 'awusb.dll';
function AwusbRead(pipenum: DWORD; buf: PBYTE; count: DWORD): DWORD; cdecl; external 'awusb.dll';
function AwusbWrite(pipenum: DWORD; buf: PBYTE; count: DWORD): DWORD; cdecl; external 'awusb.dll';
function AwusbCancelIo(pipenum: DWORD): DWORD; cdecl; external 'awusb.dll';
function AwusbGetPipeInfo(pipeinfo: PAwusbPipeInfo): DWORD; cdecl; external 'awusb.dll';
function AwusbResetPipe(pipenum: DWORD): DWORD; cdecl; external 'awusb.dll';
function AwusbDownload(hexrec: TAwusbIntelHex): DWORD; cdecl; external 'awusb.dll';
function AwusbReadIntelHexLine(buf: PChar; hexrec: PAwusbIntelHex): DWORD; cdecl; external 'awusb.dll';
function AwusbDownloadIntelHexfile(filename: PChar): DWORD; cdecl; external 'awusb.dll';
function Awusb8051Reset(hold: BOOL): DWORD; cdecl; external 'awusb.dll';

{ Error Handling }
function AwusbErrorMessage(errcode: DWORD): PChar; cdecl; external 'awusb.dll';

end.
