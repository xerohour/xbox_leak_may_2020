//-----------------------------------------------------------------------------
// File: Installer.cpp
//
// Desc: 
//
// Hist: 04.18.01 - New for CliffG
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <XBFont.h>
#include <XBUtil.h>
#include "Installer.h"
#include "Resource.h"

static const WCHAR* g_RStrings[] =

{	//    RS_BUILD_NUMBER_FORMAT
    L"Xbox version %hs",

	//    RS_INSERT_UDF    
    L"Your UDF CD-R/W or DVD Media\n"
	L"should have been placed in the drive\n"
	L"prior to running the Installer\n"
	L"\nPlease press any button to start copying\n",

	//    RS_NOMEDIA,
    L"The CDR/RW or DVD-R that you've inserted\n" 
    L"cannot be accessed, please reinsert or\n" 
    L"check the disc you've inserted and try again\n"
	L"Press any gamepad button to reboot.",

	//	  RS_CANT_CREATE_DIR
    L"Cannot create directory\n"
	L"Please Recover Machine and try again\n"
    L"Press any gamepad button to reboot.",

	//    RS_COPYING    
    L"Copying files to the hard disk...\n"
    L"(please wait until drive opens)\n",

	//    RS_DONECOPYING
    L"Done copying files. Remove media and press\n"
    L"any gamepad button to reboot.",

    //    RS_ERRORCOPY
    L"An error has occurred during file copy\n" 
    L"Either the source media is corrupt\n"
	L"It is not a UDFS filesystem\n"
    L"or hard disk is full\n"
	L"Press any button on the gamepad to reboot",

};

enum RECOVSTR_INDEX
{
    RS_BUILD_NUMBER_FORMAT = 0,
    
    RS_INSERT_UDF,
    
    RS_NOMEDIA,

	RS_CANT_CREATE_DIR,
    
    RS_COPYING,
    
    RS_DONECOPYING,
    
    RS_ERRORCOPY,
    
    RS_MAX
};


//-----------------------------------------------------------------------------
// Global rendering variables
//-----------------------------------------------------------------------------
LPDIRECT3D8        g_pD3D               = NULL;
LPDIRECT3DDEVICE8  g_pd3dDevice         = NULL;
LPDIRECT3DTEXTURE8 g_pBackgroundTexture = NULL;
CXBPackedResource  g_xprResource;
CXBFont            g_Font;

// Global instance of gamepad devices
HANDLE g_hGamepads[4];

//-----------------------------------------------------------------------------
// Name: InitGamepads()
// Desc: Creates the gamepad devices
//-----------------------------------------------------------------------------
HRESULT InitGamepads()
{
    XDEVICE_PREALLOC_TYPE deviceTypes[] =
    {
        { XDEVICE_TYPE_GAMEPAD,     4 },
        { XDEVICE_TYPE_MEMORY_UNIT, 2 }
    };

    // Initialize core peripheral port support
    XInitDevices( sizeof(deviceTypes) / sizeof(XDEVICE_PREALLOC_TYPE),
                  deviceTypes );

    // Get a mask of all currently available devices
    DWORD dwDeviceMask = XGetDevices( XDEVICE_TYPE_GAMEPAD );

    // Open the devices
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        if( dwDeviceMask & (1<<i) ) 
        {
            // Get a handle to the device
            g_hGamepads[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, 
                                         XDEVICE_NO_SLOT, NULL );
        }
        else
            g_hGamepads[i] = NULL;
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D()
{
    // Create the D3D object, which is used to create the D3DDevice.
    g_pD3D = Direct3DCreate8( D3D_SDK_VERSION );

    // Set up the structure used to create the D3DDevice.
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    
    // Set fullscreen 640x480x32 mode
    d3dpp.BackBufferWidth        = 640;
    d3dpp.BackBufferHeight       = 480;
    d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;

    // Create one backbuffer and a zbuffer
    d3dpp.BackBufferCount        = 1;


    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // Set up how the backbuffer is "presented" to the frontbuffer each frame
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

    // Create the Direct3D device. Hardware vertex processing is specified 
    // since all vertex processing takes place on Xbox hardware.
    if( FAILED( g_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
        return E_FAIL;

    // Create the font
    if( FAILED( g_Font.Create( g_pd3dDevice, "Font.xpr" ) ) )
    {
        OutputDebugString( _T("ERROR: Could not find Font.xpr\n") );
        return E_FAIL;
    }

    // Create the resource to get the background texture
    if( FAILED( g_xprResource.Create( g_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES ) ) )
    {
        OutputDebugString( _T("ERROR: Could not find Resource.xpr\n") );
        return E_FAIL;
    }

    g_pBackgroundTexture = g_xprResource.GetTexture( resource_Background_OFFSET );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawMessage()
// Desc: 
//-----------------------------------------------------------------------------
VOID DrawMessage( const WCHAR* str, VOID*, int )
{
    // First time around, allocate a vertex buffer
    static LPDIRECT3DVERTEXBUFFER8 g_pVB  = NULL;
    if( g_pVB == NULL )
    {
        g_pd3dDevice->CreateVertexBuffer( 4*5*sizeof(FLOAT), D3DUSAGE_WRITEONLY, 
                                          0L, D3DPOOL_DEFAULT, &g_pVB );
        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; };
        BACKGROUNDVERTEX* v;
        g_pVB->Lock( 0, 0, (BYTE**)&v, 0L );
        v[0].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 1.0f, 1.0f );  v[0].tu =   0; v[0].tv =   0;
        v[1].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 1.0f, 1.0f );  v[1].tu = 640; v[1].tv =   0;
        v[2].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 1.0f, 1.0f );  v[2].tu =   0; v[2].tv = 480;
        v[3].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 1.0f, 1.0f );  v[3].tu = 640; v[3].tv = 480;
        g_pVB->Unlock();
    }

    // Set states
    g_pd3dDevice->SetTexture( 0, g_pBackgroundTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->SetStreamSource( 0, g_pVB, 6*sizeof(FLOAT) );

    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Show title
    g_Font.DrawText( 135.0f, 80.0f, 0xffffffff, L"Installer" );

    // Show text
    FLOAT fXStart = 115.0f;
    FLOAT fYStart = 135.0f;

    g_Font.DrawText( fXStart, fYStart, 0xffffffff, str );

    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

//-----------------------------------------------------------------------------
// Name: GamepadButtonPressed()
// Desc: Processes input from the gamepads
//-----------------------------------------------------------------------------
BOOL GamepadButtonPressed()
{
    // Get status about gamepad insertions and removals. Note that, in order to
    // not miss devices, we will check for removed device BEFORE checking for
    // insertions
    DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals );

    // Loop through all gamepads
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        // Handle removed devices.
        if( dwRemovals & (1<<i) ) 
        {
            XInputClose( g_hGamepads[i] );
            g_hGamepads[i] = NULL;
        }

        // Handle inserted devices
        if( dwInsertions & (1<<i) ) 
        {
            g_hGamepads[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, 
                                         XDEVICE_NO_SLOT, NULL );
        }

        // If we have a valid device, poll it's state and track button changes
        if( g_hGamepads[i] )
        {
            // Read the input state
            XINPUT_STATE xiState;
            XInputGetState( g_hGamepads[i], &xiState );

            // Check for a button down
            if( xiState.Gamepad.wButtons ) 
			{
				// Return true once the buttons are let up
				while(TRUE)
				{
		            XInputGetState( g_hGamepads[i], &xiState );
					if( 0 == xiState.Gamepad.wButtons )
						return TRUE;
				}
			}

            // Check for an analog button down
            for( DWORD i=0; i<8; i++ )
			{
				if( xiState.Gamepad.bAnalogButtons[i] > XINPUT_ANALOGBUTTONTHRESHOLD )
				{
					// Return true once the buttons are let up
					while(TRUE)
					{
						XInputGetState( g_hGamepads[i], &xiState );
						if( xiState.Gamepad.bAnalogButtons[i] <= XINPUT_ANALOGBUTTONTHRESHOLD )
							return TRUE;
					}
				}
			}
        }
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
void __cdecl main()
{
    // Initialize Direct3D & Backround Image
    if( FAILED( InitD3D() ) )
        return;
    
    // Initialize gamepads
    if( FAILED( InitGamepads() ) )
        return;

    // Give the main partition a drive letter C:
    if( !NT_SUCCESS( IoCreateSymbolicLink( (POBJECT_STRING)&CDrive, 
                                           (POBJECT_STRING)&HdPartition1 ) ) )
    {
        OutputDebugString( _T("ERROR: Could not map a drive letter to partition1\n") );
        return;
    }

    // Give the DVD Drive a drive letter V:
    if( !NT_SUCCESS( IoCreateSymbolicLink( (POBJECT_STRING)&VDrive, 
                                           (POBJECT_STRING)&DVDDevice ) ) )
    {
        OutputDebugString( _T("ERROR: Could not map a drive letter to the DVD Drive\n") );
        return;
    }

    // Put up the initial screen
    DrawMessage( g_RStrings[RS_INSERT_UDF], NULL, 5 );


	//We cannot eject a DVD as the filesystem will have lost it's mounted drive...
    //EjectDVD();  

    // Wait for a button to be pressed
    while( FALSE == GamepadButtonPressed() );

	//Create the subdirectory

	if ( !CreateDirectory( (LPCSTR)"C:\\DEVKIT\\SAMPLES\\INSTALLED" ,NULL) )
	{
		OutputDebugString("Could not CreateDirectory \n");
		//Need to output a message that we didn't work
		DrawMessage( g_RStrings[RS_CANT_CREATE_DIR], NULL, 5 );
	    // Wait for a button to be pressed
		while( FALSE == GamepadButtonPressed() );
		//reboot
		XLaunchNewImage( NULL, NULL );
	}

    // Put up a screen saying we are copying
    DrawMessage( g_RStrings[RS_COPYING], NULL, 5 );

    // Now do the file copy from the DVD to the HD
    if( !CopyFiles( OTEXT("V:\\*"), OTEXT("C:\\DEVKIT\\SAMPLES\\INSTALLED\\"), TRUE ) )
    {
        OutputDebugString( _T("Could not copy files to the hard disk \n") );
		//Need to output a message that we didn't work and reboot!
		DrawMessage( g_RStrings[RS_ERRORCOPY], NULL, 5 );
	    // Wait for a button to be pressed
		while( FALSE == GamepadButtonPressed() );

		// We should delete the directory we created since this didn't work

		if ( !RemoveDirectory( (LPCSTR)"C:\\DEVKIT\\SAMPLES\\INSTALLED") )
		{
			// We should never get here since we just created this directory above!

			OutputDebugString("Could not CreateDirectory \n");
			//Need to output a message that we didn't work
			DrawMessage( g_RStrings[RS_CANT_CREATE_DIR], NULL, 5 );
			// Wait for a button to be pressed
			while( FALSE == GamepadButtonPressed() );
			//reboot
			XLaunchNewImage( NULL, NULL );
		}
		//reboot
		XLaunchNewImage( NULL, NULL );
    }

    // Put up a screen saying we are done copying
    DrawMessage( g_RStrings[RS_DONECOPYING], NULL, 5 );

    EjectDVD();

    // Wait for a button to be pressed
    while( FALSE == GamepadButtonPressed() );

    // Reboot the machine
    XLaunchNewImage( NULL, NULL );
}

//-----------------------------------------------------------------------------
// Name: EjectDVD()
// Desc: 
//-----------------------------------------------------------------------------
VOID EjectDVD()
{
    HANDLE hDevice;
    hDevice = CreateFile(OTEXT("CdRom0:"),
                         GENERIC_READ,
                         FILE_SHARE_READ,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);

    if (INVALID_HANDLE_VALUE != hDevice)
    {
        SCSI_PASS_THROUGH_DIRECT PassThrough;
        DWORD cbBytes;
        PCDB Cdb;
        SENSE_DATA SenseData;

        RtlZeroMemory(&PassThrough, sizeof(PassThrough));

        PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
        PassThrough.DataIn = SCSI_IOCTL_DATA_IN;

        Cdb = (PCDB)&PassThrough.Cdb;

        Cdb->START_STOP.OperationCode = SCSIOP_START_STOP_UNIT;
        Cdb->START_STOP.LoadEject = 1;
        Cdb->START_STOP.Start = 0;

        DeviceIoControl(hDevice,
                        IOCTL_SCSI_PASS_THROUGH_DIRECT,
                        &PassThrough,
                        sizeof(PassThrough),
                        &SenseData,
                        sizeof(SenseData),
                        &cbBytes,
                        NULL);

        CloseHandle(hDevice);
    }
    else
    {
        OutputDebugString( _T("EjectDVD() could not open DVD device\n") );
    }
}

//-----------------------------------------------------------------------------
// Name: CopyFiles()
// Desc: Copies files from a source path to a destination path. The source path
//       is a filter that can be passed to FindFirstFile() and is currently
//       required to end in "<\\*>".  The destination path should end with a 
//       trailing backslash ('\')
//-----------------------------------------------------------------------------
BOOL CopyFiles( IN PCOSTR pcszSrcPath, IN PCOSTR pcszDestPath,
                IN BOOL fRootDir )
{
    WIN32_FIND_DATA wfd;
    BOOL bRet = FALSE;
    int nSrcLen = ocslen(pcszSrcPath);
    int nDestLen = ocslen(pcszDestPath);
    HANDLE hFind = FindFirstFile(pcszSrcPath, &wfd);
    
    if (INVALID_HANDLE_VALUE != hFind)
    {
        OCHAR szSrcFile[MAX_PATH], szDestFile[MAX_PATH];
        ocscpy(szSrcFile, pcszSrcPath);
        ocscpy(szDestFile, pcszDestPath);

        do
        {
            BOOL fCopyThis = TRUE;
            
            if (fRootDir)
            {
                int i;
                for (i = 0; i < ARRAYSIZE(g_RootDirExceptions); i++)
                {
                    if (0 == _ocsicmp(wfd.cFileName, g_RootDirExceptions[i]))
                    {
                        fCopyThis = FALSE;
                        break;
                    }
                }
            }

            if (!fCopyThis)
            {
                continue;
            }
            
            lstrcpynO(&(szSrcFile[nSrcLen - 1]),
                      wfd.cFileName,
                      ARRAYSIZE(szSrcFile) - (nSrcLen - 1));
            lstrcpynO(&(szDestFile[nDestLen]),
                      wfd.cFileName,
                      ARRAYSIZE(szDestFile) - nDestLen);
                     
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                int nFileLen = ocslen(wfd.cFileName);
                
                bRet = CreateDirectory(szDestFile, NULL);

                if (!bRet)
                {
                    break;
                }

                lstrcpynO(&(szSrcFile[nSrcLen + nFileLen - 1]),
                          OTEXT("\\*"),
                          ARRAYSIZE(szSrcFile) - (nSrcLen + nFileLen - 1));

                lstrcpynO(&(szDestFile[nDestLen + nFileLen]),
                          OTEXT("\\"),
                          ARRAYSIZE(szDestFile) - (nDestLen + nFileLen));

                bRet = CopyFiles(szSrcFile, szDestFile, FALSE);

                if (!bRet)
                {
                    break;
                }
            }
            else
            {
                bRet = CopyFile(szSrcFile, szDestFile, FALSE);

                //
                // Files coming off of CD/DVD end up read-only, so clear
                // all of the attributes.. (otherwise dosnet won't work)
                //
                
                SetFileAttributes(szDestFile, FILE_ATTRIBUTE_NORMAL);

                if (!bRet)
                {
                    break;
                }

                DrawMessage( g_RStrings[RS_COPYING],
                            NULL,
                            5 + min((++g_FilesCopied * 90) / APPROX_FILE_COUNT, 90));
            }
            
        } while (FindNextFile(hFind, &wfd));
        
        FindClose(hFind);
    }
    else if (ERROR_FILE_NOT_FOUND == GetLastError())
    {
        OutputDebugString( _T("FindFirstFile error FILE_NOT_FOUND\n") );

        bRet = TRUE;
    }
    

    else if ( ERROR_INVALID_PARAMETER == GetLastError())
    {
        OutputDebugString( _T("FindFirstFile error INVALID_PARAMETER\n") );

		//We need to bail here.  Incorrect Media or an Unmounted Filesystem

//BUG BUG RS_ERRORCOPY
		DrawMessage( g_RStrings[RS_ERRORCOPY], NULL, 5 );
	    // Wait for a button to be pressed
		while( FALSE == GamepadButtonPressed() );
		//reboot
		XLaunchNewImage( NULL, NULL );
//BUG BUG OVER



        bRet = TRUE;
    }

    return bRet;
}



