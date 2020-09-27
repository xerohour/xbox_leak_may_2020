//-----------------------------------------------------------------------------
// File: Installer.cpp
//
// Desc: 
//
// Hist: 04.18.01 - New for CliffG
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddscsi.h>
#include <stdio.h>
#include "xtl.h"
#include "ntos.h"
#include <scsi.h>
#include <stdio.h>
#include <xdbg.h>
#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }
#include "xboxverp.h"
#include "xconfig.h"
#include "xboxp.h"
#include "xdisk.h"
#include "XBFont.h"
#include "XBUtil.h"
#include "Resource.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
#ifdef E3
COBJECT_STRING CDrive = CONSTANT_OBJECT_STRING(OTEXT("\\??\\C:"));
COBJECT_STRING CHdPartition1 = CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Harddisk0\\partition1\\"));
#endif

COBJECT_STRING EDrive = CONSTANT_OBJECT_STRING(OTEXT("\\??\\E:"));
COBJECT_STRING EHdPartition1 = CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Harddisk0\\partition1\\DEVKIT"));

COBJECT_STRING TDrive = CONSTANT_OBJECT_STRING(OTEXT("\\??\\T:"));
COBJECT_STRING THdPartition1 = CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Harddisk0\\partition1\\TDATA"));

COBJECT_STRING UDrive = CONSTANT_OBJECT_STRING(OTEXT("\\??\\U:"));
COBJECT_STRING UHdPartition1 = CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Harddisk0\\partition1\\UDATA"));

// Threshold for analog buttons
#define XINPUT_ANALOGBUTTONTHRESHOLD 20

#ifdef E3
void SetLaunchTitle(void);
#endif

void DrawProgressBar(int);
BOOL EnumFiles(PCOSTR pcszSrcPath);
BOOL CopyFiles(PCOSTR pcszSrcPath, PCOSTR pcszDestPath);

_int64 g_cBytesToCopy = 0;
_int64 g_cBytesCopied = 0;
int g_cFilesToCopy = 0;
int g_cFilesCopied = 0;

bool g_bHasFiles = true;
bool g_bHasTData = true;
bool g_bHasUData = true;

static const WCHAR* g_RStrings[] =
	{
	// RS_WELCOME
	L"This program is about to install files\nonto your Xbox. Press any button to\nbegin copying files.",

	// RS_ERROR_INVALID_MEDIA
	L"This demo disk is not configured properly\nThe Xbox Installer could not find\nany files to copy.\n\nPress and gamepad button to reboot.\n",

	// RS_PREPARING_COPY
	L"Preparing to copy files...",

	// RS_WARN_INSUFFICIENT_DISKSPACE
	L"Your Xbox hard drive may not have\nsufficient disk space for this installation.\nIf you are upgrading an existing set\nof files, this install may succeed.\n\nPress any gamepad button to continue",

	// RS_ERROR_COPY_FAILED
	L"File copy has failed!\n\nThis can be caused by several problems:\n   1) insufficient hard disk space\n   2) corrupt files or media\n   3) invalid file timestamps\n\nPress any gamepad button to reboot",

	// RS_EJECTING_DVD
	L"Ejecting DVD Drive...",

	// RS_COPY_COMPLETE
	L"File copy is complete. Please remove\nthe disk and press any gamepad button\nto reboot the Xbox.",

	// RS_XBOX_VERSION
	L"Xbox version %hs",

	// RS_COPYING
	L"Copying files to the hard disk...\n\nCurrently copying:\n%S"
	};

enum STR_INDEX
	{
    RS_WELCOME = 0,
	RS_ERROR_INVALID_MEDIA,
	RS_PREPARING_COPY,
	RS_WARN_INSUFFICIENT_DISKSPACE,
	RS_ERROR_COPY_FAILED,
	RS_EJECTING_DVD,
	RS_COPY_COMPLETE,
	RS_XBOX_VERSION,
	RS_COPYING
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
HRESULT HrInitGamepads(void)
	{

    XDEVICE_PREALLOC_TYPE deviceTypes[] =
		{
			{XDEVICE_TYPE_GAMEPAD, 4},
			{XDEVICE_TYPE_MEMORY_UNIT, 2}
		};

    // Initialize core peripheral port support
    XInitDevices(sizeof(deviceTypes) / sizeof(XDEVICE_PREALLOC_TYPE), deviceTypes);

    // Get a mask of all currently available devices
    DWORD dwDeviceMask = XGetDevices(XDEVICE_TYPE_GAMEPAD);

    // Open the devices
    for (DWORD i=0; i<XGetPortCount(); i++)
		{
		if (dwDeviceMask & (1 << i)) 
            g_hGamepads[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL);
        else
            g_hGamepads[i] = NULL;
		}

    return S_OK;
	}

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT HrInitD3D(void)
	{
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.BackBufferWidth        = 640;
    d3dpp.BackBufferHeight       = 480;
    d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount        = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

    g_pD3D = Direct3DCreate8( D3D_SDK_VERSION );
	if (!g_pD3D)
		{
		OutputDebugString("Error: Couldn't create Direct3D\n");
		return E_FAIL;
		}

    if (FAILED(g_pD3D->CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
		{
		OutputDebugString("Error: Couldn't create Direct3D device\n");
        return E_FAIL;
		}

    return S_OK;
	}

HRESULT HrLoadResources(void)
	{
	HRESULT hr;

    if (FAILED(hr = g_Font.Create(g_pd3dDevice, "Font.xpr")))
		{
        OutputDebugString("ERROR: Could not find Font.xpr\n");
        return hr;
		}

    if (FAILED(hr = g_xprResource.Create(g_pd3dDevice, "Resource.xpr", resource_NUM_RESOURCES)))
		{
        OutputDebugString("ERROR: Could not find Resource.xpr\n");
        return hr;
		}
    g_pBackgroundTexture = g_xprResource.GetTexture(resource_Background_OFFSET);

	return S_OK;
	}

//-----------------------------------------------------------------------------
// Name: DrawMessage()
// Desc: 
//-----------------------------------------------------------------------------
void DrawMessage(const WCHAR* str, int lProgress)
	{
    // First time around, allocate a vertex buffer
    static LPDIRECT3DVERTEXBUFFER8 g_pVB = NULL;
    
	if (g_pVB == NULL)
		{
        g_pd3dDevice->CreateVertexBuffer(4 * 5 * sizeof(FLOAT), D3DUSAGE_WRITEONLY, 0L, D3DPOOL_DEFAULT, &g_pVB);
        
		struct BACKGROUNDVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; };

        BACKGROUNDVERTEX *v;

        g_pVB->Lock(0, 0, (BYTE**)&v, 0);
        v[0].p = D3DXVECTOR4(  0,   0, 1.0f, 1.0f); v[0].tu = 0.0f; v[0].tv = 0.0f; 
        v[1].p = D3DXVECTOR4(640,   0, 1.0f, 1.0f); v[1].tu = 639.0f; v[1].tv = 0.0f; 
        v[2].p = D3DXVECTOR4(  0, 480, 1.0f, 1.0f); v[2].tu = 0.0f; v[2].tv = 479.0f; 
        v[3].p = D3DXVECTOR4(640, 480, 1.0f, 1.0f); v[3].tu = 639.0f; v[3].tv = 479.0f; 
        g_pVB->Unlock();
		}

    // Set states
    g_pd3dDevice->SetTexture(0, g_pBackgroundTexture);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_TEX1);
    g_pd3dDevice->SetStreamSource(0, g_pVB, 6 * sizeof(FLOAT));

    g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// Show version
	WCHAR szBuildString[80];

	wsprintfW(szBuildString, g_RStrings[RS_XBOX_VERSION], VER_PRODUCTVERSION_STR);
    g_Font.DrawText(190.0f, 356.0f, 0xFFFFFFA4, szBuildString);

    // Show text
    FLOAT fXStart = 115.0f;
    FLOAT fYStart = 135.0f;

    g_Font.DrawText(fXStart, fYStart, 0xFFFFFFA4, str);

	if (lProgress >= 0)
		DrawProgressBar(lProgress);

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}

void DrawProgressBar(int lProgress)
	{
    // First time around, allocate a vertex buffer
	static LPDIRECT3DVERTEXBUFFER8 g_pvbProgress = NULL;
	struct BACKGROUNDVERTEX { D3DXVECTOR4 p; DWORD color; };
    BACKGROUNDVERTEX *v;

	if (g_pvbProgress == NULL)
        g_pd3dDevice->CreateVertexBuffer(4 * 5 * sizeof(FLOAT), D3DUSAGE_WRITEONLY, 0L, D3DPOOL_DEFAULT, &g_pvbProgress);

    // Set states
    g_pd3dDevice->SetTexture(0, NULL);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// Draw the progress container first    
    g_pvbProgress->Lock(0, 0, (BYTE**)&v, 0);
    v[0].p = D3DXVECTOR4(211, 276, 1.0f, 1.0f); v[0].color = 0xFF73C034; 
    v[1].p = D3DXVECTOR4(428, 276, 1.0f, 1.0f); v[1].color = 0xFF73C034; 
    v[2].p = D3DXVECTOR4(211, 324, 1.0f, 1.0f); v[2].color = 0xFF73C034; 
    v[3].p = D3DXVECTOR4(428, 324, 1.0f, 1.0f); v[3].color = 0xFF73C034; 
    g_pvbProgress->Unlock();
	g_pd3dDevice->SetStreamSource(0, g_pvbProgress, sizeof(BACKGROUNDVERTEX));
    g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// Then, clear the inside of the progress container first    
    g_pvbProgress->Lock(0, 0, (BYTE**)&v, 0);
    v[0].p = D3DXVECTOR4(215, 280, 1.0f, 1.0f); v[0].color = 0xFF000000; 
    v[1].p = D3DXVECTOR4(424, 280, 1.0f, 1.0f); v[1].color = 0xFF000000; 
    v[2].p = D3DXVECTOR4(215, 320, 1.0f, 1.0f); v[2].color = 0xFF000000; 
    v[3].p = D3DXVECTOR4(424, 320, 1.0f, 1.0f); v[3].color = 0xFF000000; 
    g_pvbProgress->Unlock();
	g_pd3dDevice->SetStreamSource(0, g_pvbProgress, sizeof(BACKGROUNDVERTEX));
    g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// Finally, draw the actual progress bar
    g_pvbProgress->Lock(0, 0, (BYTE**)&v, 0);
    v[0].p = D3DXVECTOR4(217, 282, 1.0f, 1.0f); v[0].color = 0xFFFFFFA4; 
    v[1].p = D3DXVECTOR4((FLOAT)(217 + (205 * lProgress) / 100), 282, 1.0f, 1.0f); v[1].color = 0xFFFFFFA4; 
    v[2].p = D3DXVECTOR4(217, 318, 1.0f, 1.0f); v[2].color = 0xFFFFFFA4; 
    v[3].p = D3DXVECTOR4((FLOAT)(217 + (205 * lProgress) / 100), 318, 1.0f, 1.0f); v[3].color = 0xFFFFFFA4; 
    g_pvbProgress->Unlock();
	g_pd3dDevice->SetStreamSource(0, g_pvbProgress, sizeof(BACKGROUNDVERTEX));
    g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}

BOOL FIsAButtonDown(void)
	{
    static DWORD dwPads = 0;
    DWORD dwInsertions, dwRemovals;
    static HANDLE hPads[XGetPortCount()] = { 0 };
    int i;

    if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
		{
        dwPads |= dwInsertions;
        dwPads &= ~dwRemovals;

        for (i = 0; i < XGetPortCount(); i++)
			{
            if ((1 << i) & dwRemovals)
				{
                if (NULL != hPads[i])
					{
                    XInputClose(hPads[i]);
                    hPads[i] = NULL;
					}
				}

            if ((1 << i) & dwInsertions)
				{
                if (NULL == hPads[i])
                    hPads[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + i, XDEVICE_NO_SLOT, NULL);;
	            }
		    }
		}

    for (i = 0; i < XGetPortCount(); i++)
		{
        if ((1 << i) & dwPads)
			{
            if (NULL != hPads[i])
				{
                XINPUT_STATE State;

                if (ERROR_SUCCESS == XInputGetState(hPads[i], &State))
					{
                    if ((0 != ((XINPUT_GAMEPAD_START | XINPUT_GAMEPAD_BACK) & State.Gamepad.wButtons)) ||
                             (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                             (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                             (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                             (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                             (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                             (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > XINPUT_ANALOGBUTTONTHRESHOLD))
                        return TRUE;
	                }
		        }
			}
		}

	return FALSE;
	}

void WaitForAnyButton(void)
	{
	if (FIsAButtonDown())
		while (FIsAButtonDown());
	while (!FIsAButtonDown());
	}

//-----------------------------------------------------------------------------
// Name: EjectDVD()
// Desc: 
//-----------------------------------------------------------------------------
void EjectDVD(void)
	{
    HANDLE hDevice;
    hDevice = CreateFile(OTEXT("CdRom0:"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

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

        DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &PassThrough, sizeof(PassThrough), &SenseData, sizeof(SenseData), &cbBytes, NULL);
        CloseHandle(hDevice);
		}
    else
		{
        OutputDebugString("EjectDVD() could not open DVD device\n");
	    }
	}

BOOL EnumFiles(PCOSTR pcszSrcPath)
	{
    WIN32_FIND_DATA wfd;
    BOOL bRet = FALSE;
	int nSrcLen = ocslen(pcszSrcPath);
    HANDLE hFind = FindFirstFile(pcszSrcPath, &wfd);
	_int64 i64FileSize;

    if (INVALID_HANDLE_VALUE != hFind)
	    {
        OCHAR szSrcFile[MAX_PATH];
		ocscpy(szSrcFile, pcszSrcPath);
        do
			{
			i64FileSize = wfd.nFileSizeHigh;
			i64FileSize <<= 32;
			i64FileSize += wfd.nFileSizeLow;
			g_cBytesToCopy += i64FileSize;

            lstrcpynO(&(szSrcFile[nSrcLen - 1]), wfd.cFileName, ARRAYSIZE(szSrcFile) - (nSrcLen - 1));
                     
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
                int nFileLen = ocslen(wfd.cFileName);

                lstrcpynO(&(szSrcFile[nSrcLen + nFileLen - 1]), OTEXT("\\*"), ARRAYSIZE(szSrcFile) - (nSrcLen + nFileLen - 1));
                bRet = EnumFiles(szSrcFile);
                if (!bRet)
                    break;
				}
            else
	            {
				g_cFilesToCopy++;
				bRet = TRUE;
				}
			} while (FindNextFile(hFind, &wfd));
        
        FindClose(hFind);
		}
    else if (ERROR_FILE_NOT_FOUND == GetLastError())
	    {
        OutputDebugString("FindFirstFile error FILE_NOT_FOUND\n");
        bRet = TRUE;
	    }

    return bRet;
	}

//-----------------------------------------------------------------------------
// Name: CopyFiles()
// Desc: Copies files from a source path to a destination path. The source path
//       is a filter that can be passed to FindFirstFile() and is currently
//       required to end in "<\\*>".  The destination path should end with a 
//       trailing backslash ('\')
//-----------------------------------------------------------------------------
WCHAR g_szCopyText[512];

BOOL CopyFiles(PCOSTR pcszSrcPath, PCOSTR pcszDestPath)
	{
    WIN32_FIND_DATA wfd;
    BOOL bRet = FALSE;
    int nSrcLen = ocslen(pcszSrcPath);
    int nDestLen = ocslen(pcszDestPath);
    HANDLE hFind = FindFirstFile(pcszSrcPath, &wfd);
    _int64 i64FileSize;

    if (INVALID_HANDLE_VALUE != hFind)
		{
        OCHAR szSrcFile[MAX_PATH], szDestFile[MAX_PATH];
        ocscpy(szSrcFile, pcszSrcPath);
        ocscpy(szDestFile, pcszDestPath);

        do
			{
            lstrcpynO(&(szSrcFile[nSrcLen - 1]), wfd.cFileName, ARRAYSIZE(szSrcFile) - (nSrcLen - 1));
            lstrcpynO(&(szDestFile[nDestLen]), wfd.cFileName, ARRAYSIZE(szDestFile) - nDestLen);
                     
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
                int nFileLen = ocslen(wfd.cFileName);
                
                bRet = CreateDirectory(szDestFile, NULL);
				HRESULT hr = GetLastError();
                if ((bRet == 0) && (ERROR_ALREADY_EXISTS != hr))
                    break;

                lstrcpynO(&(szSrcFile[nSrcLen + nFileLen - 1]), OTEXT("\\*"), ARRAYSIZE(szSrcFile) - (nSrcLen + nFileLen - 1));
                lstrcpynO(&(szDestFile[nDestLen + nFileLen]), OTEXT("\\"), ARRAYSIZE(szDestFile) - (nDestLen + nFileLen));

                bRet = CopyFiles(szSrcFile, szDestFile);
                if (!bRet)
                    break;
	            }
            else
		        {
				int PercentageDone;

				// Bytes completed
				PercentageDone = (int)(((double)g_cBytesCopied / (double)g_cBytesToCopy) * 100.0);
				
				// Files completed
				//PercentageDone = (int)(((float)g_cFilesCopied / (float)g_cFilesToCopy) * 100.0f)

                DrawMessage(g_szCopyText, PercentageDone);
                bRet = CopyFile(szSrcFile, szDestFile, FALSE);
				g_cFilesCopied++;

				i64FileSize = wfd.nFileSizeHigh;
				i64FileSize <<= 32;
				i64FileSize += wfd.nFileSizeLow;
				g_cBytesCopied += i64FileSize;
                
				//
                // Files coming off of CD/DVD end up read-only, so clear
                // all of the attributes.. (otherwise dosnet won't work)
                //
                SetFileAttributes(szDestFile, FILE_ATTRIBUTE_NORMAL);
                if (!bRet)
                    break;

				wsprintfW(g_szCopyText, g_RStrings[RS_COPYING], wfd.cFileName);
	            }
			} while (FindNextFile(hFind, &wfd));
			FindClose(hFind);
		}
    else if (ERROR_FILE_NOT_FOUND == GetLastError())
		{
        OutputDebugString("FindFirstFile error FILE_NOT_FOUND\n");
        bRet = TRUE;
	    }
	
    return bRet;
	}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
void __cdecl main()
	{
    WIN32_FIND_DATA wfd;
	HANDLE hFind;
    
	// Initialize Direct3D & Backround Image
    if (FAILED(HrInitD3D()))
        return;
    
	if (FAILED(HrLoadResources()))
		return;

    // Initialize gamepads
    if (FAILED(HrInitGamepads()))
        return;

	hFind = FindFirstFile(OTEXT("D:\\Files"), &wfd);
	if (INVALID_HANDLE_VALUE == hFind)
		{
		OutputDebugString("Info: No E: files\n");
		g_bHasFiles = false;
		}
	else
		{
		OutputDebugString("Info: Has E: files\n");
		FindClose(hFind);
		}

	// ensure that the disc has a "TDATA" directory
	hFind = FindFirstFile(OTEXT("D:\\TDATA"), &wfd);
	if (INVALID_HANDLE_VALUE == hFind)
		{
		OutputDebugString("Info: No T: files\n");
		g_bHasTData = false;
		}
	else
		{
		OutputDebugString("Info: Has T: files\n");
		FindClose(hFind);
		}

	// ensure that the disc has a "UDATA" directory
	hFind = FindFirstFile(OTEXT("D:\\UDATA"), &wfd);
	if (INVALID_HANDLE_VALUE == hFind)
		{
		OutputDebugString("Info: No U: files\n");
		g_bHasUData = false;
		}
	else
		{
		OutputDebugString("Info: Has U: files\n");
		FindClose(hFind);
		}

	// make sure that the disc has something to copy
	if (!g_bHasFiles && !g_bHasTData && !g_bHasUData)
		{
		DrawMessage(g_RStrings[RS_ERROR_INVALID_MEDIA], -1);
		OutputDebugString("Error: no files on the disc to copy\n");
		WaitForAnyButton();
		XLaunchNewImage(NULL, NULL);
		}

#ifdef E3
	// Mount the C: drive
    if (!NT_SUCCESS(IoCreateSymbolicLink((POBJECT_STRING)&CDrive, (POBJECT_STRING)&CHdPartition1)))
		{
        OutputDebugString("Error: Could not map a drive letter to partition1\n");
        return;
		}
#endif

    // Mount the E: drive
	if (g_bHasFiles)
		{
		OutputDebugString("Mounting the E: drive\n");
		if (!NT_SUCCESS(IoCreateSymbolicLink((POBJECT_STRING)&EDrive, (POBJECT_STRING)&EHdPartition1)))
			{
			OutputDebugString("ERROR: Could not map a drive letter to partition1\n");
			return;
			}
		}

    // Mount the T: drive
    if (g_bHasTData)
		{
		OutputDebugString("Mounting the T: drive\n");
		if (!NT_SUCCESS(IoCreateSymbolicLink((POBJECT_STRING)&TDrive, (POBJECT_STRING)&THdPartition1)))
			{
			OutputDebugString("ERROR: Could not map a drive letter to partition1\n");
			return;
			}
		}

	// Mount the U: drive
    if (g_bHasUData)
		{
		OutputDebugString("Mounting the U: drive\n");
		if (!NT_SUCCESS(IoCreateSymbolicLink((POBJECT_STRING)&UDrive, (POBJECT_STRING)&UHdPartition1)))
			{
			OutputDebugString("ERROR: Could not map a drive letter to partition1\n");
			return;
			}
		}

    // Put up the initial screen
	OutputDebugString("Displaying the welcome screen\n");
    DrawMessage(g_RStrings[RS_WELCOME], -1);
	WaitForAnyButton();

	// enumerate all of the files on the disc to get an accurate progress counter
	OutputDebugString("Enumerating files on the disc\n");
	DrawMessage(g_RStrings[RS_PREPARING_COPY], 0);
	if (g_bHasFiles)
		EnumFiles(OTEXT("D:\\Files\\*"));
	if (g_bHasTData)
		EnumFiles(OTEXT("D:\\TDATA\\*"));
	if (g_bHasUData)
		EnumFiles(OTEXT("D:\\UDATA\\*"));

	// Determine the free space on the hard disk. If there isn't enough, warn.
	if (g_bHasFiles)
		{
		_int64 i64BytesFree;
		GetDiskFreeSpaceEx("E:\\", NULL, NULL, (ULARGE_INTEGER *)&i64BytesFree);
		if (i64BytesFree < g_cBytesToCopy)
			{
			DrawMessage(g_RStrings[RS_WARN_INSUFFICIENT_DISKSPACE], -1);
			WaitForAnyButton();
			}

		// Perform the file copy from the disc to the E:\ drive
		if (!CopyFiles(OTEXT("D:\\Files\\*"), OTEXT("E:\\")))
			{
			OutputDebugString("Critical error: file copy has failed");
			DrawMessage(g_RStrings[RS_ERROR_COPY_FAILED], -1);
			EjectDVD();
			WaitForAnyButton();
			XLaunchNewImage(NULL, NULL);
			}
		}

	if (g_bHasTData)
		{
		if (!CopyFiles(OTEXT("D:\\TDATA\\*"), OTEXT("T:\\")))
			{
			OutputDebugString("Critical error: file copy has failed");
			DrawMessage(g_RStrings[RS_ERROR_COPY_FAILED], -1);
			EjectDVD();
			WaitForAnyButton();
			XLaunchNewImage(NULL, NULL);
			}
		}

	if (g_bHasUData)
		{
		if (!CopyFiles(OTEXT("D:\\UDATA\\*"), OTEXT("U:\\")))
			{
			OutputDebugString("Critical error: file copy has failed");
			DrawMessage(g_RStrings[RS_ERROR_COPY_FAILED], -1);
			EjectDVD();
			WaitForAnyButton();
			XLaunchNewImage(NULL, NULL);
			}
		}

#ifdef E3
	SetLaunchTitle();
#endif

	// Eject the DVD drive
	DrawMessage(g_RStrings[RS_EJECTING_DVD], -1);
    EjectDVD();
	DrawMessage(g_RStrings[RS_COPY_COMPLETE], -1);
	WaitForAnyButton();

    // reboot the machine
	XLaunchNewImage(NULL, NULL);
	}

#ifdef E3

void SetLaunchTitle(void)
	{
	char szBuffer[MAX_PATH], szLaunch[360], *pch;
	FILE *fp;

	if (!(fp = fopen("D:\\LAUNCH.TXT", "rt")))
		return;
	fgets(szBuffer, MAX_PATH, fp);
	fclose(fp);

	// skip the initial XE:\, E:\ or \ symbols
	pch = szBuffer;
	if (('X' == toupper(*pch)) && ('E' == toupper(*(pch+1))) && (':' == *(pch+2)) && ('\\' == *(pch+3)))
		pch += 4;
	else if (('E' == toupper(*pch)) && (':' == *(pch+1)) && ('\\' == *(pch+2)))
		pch += 3;
	else if ('\\' == *pch)
		pch++;

	ZeroMemory(szLaunch, 360);
	sprintf(szLaunch, "\\Device\\Harddisk0\\partition1\\DEVKIT\\%s", pch);

	// replace the final '\' with a ';'
	pch = &(szLaunch[strlen(szLaunch) - 1]);
	while (*pch != '\\')
		*pch--;
	*pch = ';';

	if (!(fp = fopen("C:\\dashboard.xbx", "wb")))
		return;
	fwrite(szLaunch, 360, 1, fp);
	fclose(fp);
	}

#endif
