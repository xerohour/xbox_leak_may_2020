/**************************************************************************
				ActiveWire USB Sample Application

 Copyright (c) 1999   ActiveWire, Inc. http//www.ActiveWireInc.com 
                    - All rights reserved. 

 		Oct.12th, 1999  by Mato Hattori <Mato@ActiveWireInc.com>

 This file is made by ActiveWire, Inc. to use with ActiveWire USB board.
 
 Redistribution and use in source and/or binary forms, with or without 
 modification, are permitted.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 SUCH DAMAGE. 
**************************************************************************/

/*

  This example shows a simple C program to control ActiveWire USB board.
  This also demonstrate explicit call of DLL function.

  There are 2 ways to use DLL in any application,
	1. Statically load DLL (Link implicitly)
	2. Dynamically load DLL  (Link Explicitly)
  In general, 1. Statically load DLL by statically linking Awusb.lib file
  is easier and recommended when using Microsoft Visual C++.
  
  If Borland C++ or any other build environment is desired, the DLL must 
  be linked explicitly.
  Please refer to Win32 API GetProcAddress().

  Because explicit call of DLL function implies far memory pointer call, 
  extra caution must be paid for code optimization option when compiling.

*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "AwusbAPI.h"

// function type declaration to use with pointer declaration 
typedef DWORD	(CALLBACK * FARPROCP_AwusbOpen) (DWORD devnum);
typedef DWORD	(CALLBACK * FARPROCP_AwusbClose) ();
typedef DWORD	(CALLBACK * FARPROCP_AwusbEnablePort) (BYTE *data, DWORD count);
typedef DWORD	(CALLBACK * FARPROCP_AwusbOutPort) (BYTE *data, DWORD count);
typedef DWORD	(CALLBACK * FARPROCP_AwusbInPort) (BYTE *data, DWORD count);
typedef char *	(CALLBACK * FARPROCP_AwusbErrorMessage) (DWORD ret);

// function pointer declaration
FARPROCP_AwusbOpen			Dll_AwusbOpen;
FARPROCP_AwusbClose			Dll_AwusbClose;
FARPROCP_AwusbEnablePort	Dll_AwusbEnablePort;
FARPROCP_AwusbOutPort		Dll_AwusbOutPort;
FARPROCP_AwusbInPort		Dll_AwusbInPort;
FARPROCP_AwusbErrorMessage	Dll_AwusbErrorMessage;


void main (void)
{
	HINSTANCE	hDll;
	char	buf[256];
	int		devnum;
	DWORD	ret;
	int		key;
	unsigned int	data = 0, dir = 0;

	// load DLL
	hDll = LoadLibrary (AWUSB_DLL_NAME);
	if (hDll == NULL)
		printf ("ERROR : %s\n", Dll_AwusbErrorMessage (AWUSB_ERROR_SYSTEM));
	else
	{
		// initialize function pointer
		Dll_AwusbOpen =			(FARPROCP_AwusbOpen) GetProcAddress (hDll, "AwusbOpen");
		Dll_AwusbClose =		(FARPROCP_AwusbClose) GetProcAddress (hDll, "AwusbClose");
		Dll_AwusbEnablePort =	(FARPROCP_AwusbEnablePort) GetProcAddress (hDll, "AwusbEnablePort");
		Dll_AwusbOutPort =		(FARPROCP_AwusbOutPort) GetProcAddress (hDll, "AwusbOutPort");
		Dll_AwusbInPort =		(FARPROCP_AwusbInPort) GetProcAddress (hDll, "AwusbInPort");
		Dll_AwusbErrorMessage = (FARPROCP_AwusbErrorMessage) GetProcAddress (hDll, "AwusbErrorMessage");

		// now ready to call ActiveWire DLL functions

		// banner
		printf ("ActiveWire USB Sample Application\n");
		printf ("Copyright (C) 1999  ActiveWire, Inc. - All rights reserved.\n");
		printf ("\n");
		printf ("Please select Device Number : ");
		fgets (buf, sizeof(buf), stdin);
		sscanf (buf, "%d", &devnum);

		// open the device
		ret = Dll_AwusbOpen (devnum);
		if (ret != AWUSB_OK)				// if any error
			printf ("ERROR : %s\n", Dll_AwusbErrorMessage (ret));
		else
		{
			// successfully opened
			printf ("Device #%d successfully opened.\n", devnum);
			printf ("Press Space to toggle direction Input or Output.\n");
			printf ("Press 0-9a-f to toggle an Output bit.\n");
			printf ("Hit ESC to quit.\n");
			printf ("\n");
			
			while (1)
			{
				key = getch();
				
				if (key == 0x1b)	// if ESC key
				{
					Dll_AwusbClose ();		// close the device
					break;
				}

				switch (tolower (key))
				{
					case '0':	data ^= 0x0001;	break;
					case '1':	data ^= 0x0002;	break;
					case '2':	data ^= 0x0004;	break;
					case '3':	data ^= 0x0008;	break;
					case '4':	data ^= 0x0010;	break;
					case '5':	data ^= 0x0020;	break;
					case '6':	data ^= 0x0040;	break;
					case '7':	data ^= 0x0080;	break;
					case '8':	data ^= 0x0100;	break;
					case '9':	data ^= 0x0200;	break;
					case 'a':	data ^= 0x0400;	break;
					case 'b':	data ^= 0x0800;	break;
					case 'c':	data ^= 0x1000;	break;
					case 'd':	data ^= 0x2000;	break;
					case 'e':	data ^= 0x4000;	break;
					case 'f':	data ^= 0x8000;	break;
					case ' ':   dir ^= 0xffff;	break;
					default:	break;
				}

				ret = Dll_AwusbEnablePort ((BYTE*)&dir, 2);
				if (ret != AWUSB_OK)
					printf ("ERROR : %s\n", Dll_AwusbErrorMessage (ret));

				ret = Dll_AwusbOutPort ((BYTE*)&data, 2);
				if (ret != AWUSB_OK)
					printf ("ERROR : %s\n", Dll_AwusbErrorMessage (ret));

				ret = Dll_AwusbInPort ((BYTE*)&data, 2);
				if (ret != AWUSB_OK)
					printf ("ERROR : %s\n", Dll_AwusbErrorMessage (ret));
				
				printf ("data = 0x%04x\n", data);
			}
		}

		// clean up
		FreeLibrary (hDll);
	}
}


