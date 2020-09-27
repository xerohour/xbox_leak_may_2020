////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define TRACE_VDR_CALLS 0
#define TRACE_VDR_CALLS_E 0

#include "library/common/prelude.h"
#include "library/common/tags.h"

#include "idllspec.h"
#include "vdgendrv.h"

#include "../lowlevel/ntkernel/genersys.h"

#include "library/common/vddebug.h"

#include "library/support/vdsup32/vdsup32.h"
#include "ddraw.h"

#define MAX_UNITS	100

#define PIP_SUPPORT	1
#define WIN_SUPPORT  1
#define OVERLAY_SUPPORT  0
#define RET_HOOK  0

#define USE_HOOK_PROCESS 1

#define WIDTHBYTES(bits)		(((bits) + 31) / 32 * 4)

///////////////////////////////////////////////////////////////////////////////
// Special Hooks
///////////////////////////////////////////////////////////////////////////////

class MPEGUnitsRefillHook : public MPEGStreamHook {
	private:
		HWND	hwnd;
		WORD	msg;

		friend Error CALLBACK MPEGUnitsRefillCall(MPEGStreamHook * me, MPEGStreamMsg& data);
		Error RefillRequest(void)
			{
			if (!pending)
				{
				pending = TRUE;
				::PostMessage(hwnd, WM_USER, msg, 0);
				}

			GNRAISE_OK;
			}
	public:
		BOOL	pending;

		MPEGUnitsRefillHook(HWND hwnd, WORD msg)
			: MPEGStreamHook(MPEGUnitsRefillCall)
			{this->hwnd = hwnd; this->msg = msg;pending = FALSE; VDR_RegisterCallback(this);}

		~MPEGUnitsRefillHook(void) {VDR_UnregisterCallback(this);}
	};

Error CALLBACK MPEGUnitsRefillCall(MPEGStreamHook * me, MPEGStreamMsg& msg)
	{
	return ((MPEGUnitsRefillHook *)me)->RefillRequest();
	}

class MPEGUnitsSignalHook : public MPEGSignalHook {
	private:
		HWND	hwnd;
		WORD	msg;
		friend Error CALLBACK MPEGUnitsSignalCall(MPEGSignalHook * me, MPEGSignalMsg& data);
		Error SignalRequest(DWORD pos)
			{
			::PostMessage(hwnd, WM_USER, msg, pos);

			GNRAISE_OK;
			}
	public:
		MPEGUnitsSignalHook(HWND hwnd, WORD msg)
			: MPEGSignalHook(MPEGUnitsSignalCall)
			{this->hwnd = hwnd; this->msg = msg; VDR_RegisterCallback(this);}

		~MPEGUnitsSignalHook(void) {VDR_UnregisterCallback(this);}
	};

Error CALLBACK MPEGUnitsSignalCall(MPEGSignalHook * me, MPEGSignalMsg& msg)
	{
	return ((MPEGUnitsSignalHook *)me)->SignalRequest(msg.position);
	}

class MPEGUnitsDoneHook : public MPEGSignalHook {
	private:
		HWND	hwnd;
		WORD	msg;
		friend Error CALLBACK MPEGUnitsDoneCall(MPEGSignalHook * me, MPEGSignalMsg& data);
		Error DoneRequest(void)
			{
			::PostMessage(hwnd, WM_USER, msg, 0);

			GNRAISE_OK;
			}
	public:
		MPEGUnitsDoneHook(HWND hwnd, WORD msg)
			: MPEGSignalHook(MPEGUnitsDoneCall)
			{this->hwnd = hwnd; this->msg = msg; VDR_RegisterCallback(this);}

		~MPEGUnitsDoneHook(void) {VDR_UnregisterCallback(this);}
	};

Error CALLBACK MPEGUnitsDoneCall(MPEGSignalHook * me, MPEGSignalMsg& msg)
	{
	return ((MPEGUnitsDoneHook *)me)->DoneRequest();
	}

///////////////////////////////////////////////////////////////////////////////
// Class definitions.
///////////////////////////////////////////////////////////////////////////////

typedef class IUnitSetClass
	{
	public:
		int		id;
		int		prev, next;
#if PIP_SUPPORT
		HWND		pipWindow;
		BOOL		pipEnabled;
#endif
		BOOL		hasMPEGDecoder;

		MPEGUnitsRefillHook		*	refillHook;
		MPEGUnitsSignalHook		*	signalHook;
		MPEGUnitsDoneHook			*	doneHook;

		VDRHandle handle;
		KUnitSet	kunits;
	} * IUnitSet;





///////////////////////////////////////////////////////////////////////////////
// Support functions.
///////////////////////////////////////////////////////////////////////////////

static DWORD staticRGBs[20] = {0x000000, 0x000080, 0x008000, 0x008080, 0x800000,
										 0x800080, 0x808000, 0xc0c0c0, 0xc0dcc0, 0xf0caa6,
										 0xf0fbff, 0xa4a0a0, 0x808080, 0x0000ff, 0x00ff00,
										 0x00ffff, 0xff0000, 0xff00ff, 0xffff00, 0xffffff};


static DWORD ConvertKeyColor (DWORD colorRGB, DWORD redBitMask, DWORD greenBitMask, DWORD blueBitMask)
	{
	DWORD newKeyColor;
	DWORD curMask;
	int i, redCount, greenCount, blueCount;

	newKeyColor = 0L;

	redCount		= 7;
	greenCount	= 15;
	blueCount	= 23;

	for (i = 31;  i >= 0;  i--)
		{
		curMask = 1L << i;

		if (redBitMask & curMask)
			{
			if (colorRGB & (1L << redCount))
				newKeyColor |= curMask;
			redCount--;
			}
		else if (greenBitMask & curMask)
			{
			if (colorRGB & (1L << greenCount))
				newKeyColor |= curMask;
			greenCount--;
			}
		else if (blueBitMask & curMask)
			{
			if (colorRGB & (1L << blueCount))
				newKeyColor |= curMask;
			blueCount--;
			}
		}

	return newKeyColor;
	}


DWORD ConvertKeyIndex (DWORD __far &colorIndex)
	{
	if (colorIndex > 9  &&  colorIndex < 246)
		colorIndex = 253;   // default to magenta if index is invalid

	if (colorIndex >= 246)
		return staticRGBs[colorIndex - 236];
	else
		return staticRGBs[colorIndex];
	}





///////////////////////////////////////////////////////////////////////////////
// VDRHandle implementation.
///////////////////////////////////////////////////////////////////////////////

class VDRHandleClass
	{
	public:
		Error Initialize (void);
		Error Free (void);

		~VDRHandleClass (void) {Free();}

	public:
		DWORD boardID;
		KernelHandle kHandle;

		// Direct Draw stuff
		DDAccess		ddAccess;
		BOOL			colorKeyed;
		BOOL			dmaOverlayEnabled;

		RECT			keyPaintRect;

		DWORD			keyColor;
		DWORD			keyIndex;
		DWORD			keyColorRGB;
		DWORD			keyColorIndex;

		BOOL			alwaysOverlay;
		DWORD			overlayPossible;
		BOOL			forceOverlay;

		WORD			screenWidth, screenHeight;

		DDSURFACEDESC	surface;
	};


Error VDRHandleClass::Initialize (void)
	{
	boardID = boardID;
	kHandle = NULL;
	ddAccess = NULL;

	// Set default Color Key color (magenta).
	keyColorRGB		= RGB(0xff, 0x00, 0xff);
	keyColorIndex	= 253;   // magenta in the system palette

	colorKeyed = FALSE;
#if OVERLAY_SUPPORT
	if (! VD32_OpenDDAccess (ddAccess, overlayPossible, &surface))
		{
		DP(__TEXT("Can't get DD access"));
		GNRAISE(GNR_OBJECT_NOT_FOUND);
		}
DP(__TEXT("overlayPossible %ld"), (long)overlayPossible);

   screenWidth = surface.dwWidth;
	screenHeight = surface.dwHeight;

	if (surface.ddpfPixelFormat.dwRBitMask)
		{
		keyColor = ConvertKeyColor (keyColorRGB,
		                            surface.ddpfPixelFormat.dwRBitMask,
		                            surface.ddpfPixelFormat.dwGBitMask,
		                            surface.ddpfPixelFormat.dwBBitMask);
		keyIndex = keyColorRGB;
		}
	else
		{
		keyIndex = ConvertKeyIndex (keyColorIndex);
		keyColor = keyColorIndex;
		}
#else
	overlayPossible = FALSE;
#endif

	GNRAISE_OK;
	}


Error VDRHandleClass::Free (void)
	{
	if (ddAccess)
		{
#if OVERLAY_SUPPORT
		VD32_CloseDDAccess (ddAccess);
		ddAccess = NULL;
#endif
		}
	GNRAISE_OK;
	}





///////////////////////////////////////////////////////////////////////////////
// Process instance variables.
///////////////////////////////////////////////////////////////////////////////

HANDLE			kDriver;
HINSTANCE		hinst;
HANDLE			WorkerThread;
HANDLE			WorkerDriver;
PKWorkerQueue	WorkerQueue;
DWORD				WorkerID;

int				DriverOpenCnt;

HANDLE			clipMutex;

HANDLE	ServiceLock;
HANDLE	ServiceRequest;
HANDLE	ServiceDone;

///////////////////////////////////////////////////////////////////////////////
// Variables shared between processes.
///////////////////////////////////////////////////////////////////////////////

#pragma data_seg(".sdata")

IUnitSetClass	Units[MAX_UNITS]	= {0};
int				firstFree			= 0;
int				firstActive			= -1;
int				openCnt				= 0;

#if PIP_SUPPORT
int				activePIPCnt		= 0;
UINT				doneMsg				= 0;
HHOOK				sizeHook				= NULL;
HHOOK				doneHook				= NULL;
int				msgPending			= 0;
volatile HWND	lastMoved			= 0;
#endif

class VDRHandleClass DriverInstanceHandle;

volatile enum	{	DD_NONE,
						DD_TERMINATE,
						DD_INSTALLHOOK,
						DD_REMOVEHOOK} ServiceCommand = DD_NONE;

volatile BOOL	ServiceSuccess = FALSE;
volatile BOOL	ServiceActive = FALSE;
volatile long	ServiceProcesses = 0;

PROCESS_INFORMATION processInfo = {0};

#pragma data_seg()





///////////////////////////////////////////////////////////////////////////////
// IUnitSet implementation.
///////////////////////////////////////////////////////////////////////////////

inline UnitSet TU(IUnitSet units) {return (UnitSet)(units->id+0x40000000);}
inline IUnitSet TU(UnitSet units) {return Units + ((int)units-0x40000000);}


void InitUnits(void)
	{
	int i;

	for (i=0; i<MAX_UNITS; i++)
		{
		Units[i].next = i+1;
		Units[i].id   = i;
		}
	Units[MAX_UNITS-1].next = -1;
	firstFree = 0;
	}


IUnitSet AllocUnit (VDRHandle handle)
	{
	int i;

	if	(firstFree != -1)
		{
		i = firstFree;
		firstFree = Units[firstFree].next;

#if PIP_SUPPORT
		Units[i].pipWindow = NULL;
		Units[i].pipEnabled = FALSE;
#endif
		Units[i].hasMPEGDecoder = NULL;

		Units[i].refillHook	= NULL;
		Units[i].signalHook	= NULL;
		Units[i].doneHook		= NULL;

		Units[i].handle = handle;

		if (firstActive != -1)
			{
			Units[firstActive].prev = i;
			}
		Units[i].prev = -1;
		Units[i].next = firstActive;
		firstActive = i;

		return Units+i;
		}
	else
		return NULL;
	}


void FreeUnit (IUnitSet unit)
	{
	if (unit->prev != -1)
		Units[unit->prev].next = unit->next;
	else
		firstActive = unit->next;
	if (unit->next != -1)
		Units[unit->next].prev = unit->prev;

	unit->next = firstFree;
	firstFree = unit->id;
	}




///////////////////////////////////////////////////////////////////////////////
// Worker Queue
///////////////////////////////////////////////////////////////////////////////

void RegisterWorker(void)
	{
	INIT_IOCTL(REGISTER_WORKER);
	DO_IOCTL(REGISTER_WORKER);
	WorkerQueue = out.queue;
	}

void UnregisterWorker(void)
	{
	INIT_IOCTL(UNREGISTER_WORKER);
	in.queue = WorkerQueue;
	DO_IOCTL(UNREGISTER_WORKER);
	WorkerQueue = NULL;
	}

DWORD __stdcall WorkerRoutine(void * param)
	{
	HANDLE kDriver = CreateFile(KERNEL_DRIVER_SYMBOLIC_NAME,
								GENERIC_WRITE | GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);

	if (INVALID_HANDLE_VALUE == kDriver)
		{
		DWORD error = GetLastError();
		ExitThread(0);
		}

	for(;;)
		{
		if (!WorkerQueue)
			{
			DP(__TEXT("No worker queue any more!"));
			CloseHandle(kDriver);
			ExitThread(0);
			}

		INIT_IOCTL(GET_WORKER_WORK);
		in.queue = WorkerQueue;
		DO_IOCTL(GET_WORKER_WORK);

		if (!(out.me))
			{
			DP(__TEXT("DoneWork"));
			CloseHandle(kDriver);
			ExitThread(0);
			}
		else
			{
			DummyHook	*	p = (DummyHook *)(out.me);

			p->Call(out.data);
			}
		}

	return 0;
	}

void InitThreadContext(void)
	{
	DP(__TEXT("InitThreadContext+"));
	RegisterWorker();
	WorkerThread = CreateThread(NULL, 0, WorkerRoutine, NULL, 0, &WorkerID);
	SetThreadPriority(WorkerThread, THREAD_PRIORITY_HIGHEST);
	DP(__TEXT("InitThreadContext-"));
	}

void FiniThreadContext(void)
	{
	DP(__TEXT("FiniThreadContext+"));
	UnregisterWorker();
	if (WorkerThread)
		{
		WaitForSingleObject(WorkerThread, 0x7fffffff);
		CloseHandle(WorkerThread);
		WorkerThread = NULL;
		}
//	while (WorkerThread) ;
	DP(__TEXT("FiniThreadContext-"));
	}




///////////////////////////////////////////////////////////////////////////////
// PIP Support
///////////////////////////////////////////////////////////////////////////////

#if PIP_SUPPORT



// DirectDraw support

int			ddOpenCnt = 0;
HINSTANCE	ddInst;

HRESULT (WINAPI * xDirectDrawCreate) (GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);


BOOL OpenDDLib(void)
	{
	if (!ddOpenCnt)
		{
		ddInst = LoadLibrary(__TEXT("ddraw.dll"));
		if (ddInst != NULL)
			{
			ddOpenCnt = 1;
			(FARPROC &)xDirectDrawCreate = GetProcAddress(ddInst, "DirectDrawCreate");

			return TRUE;
			}
		else
			return FALSE;
		}
	else
		{
		ddOpenCnt++;
		return TRUE;
		}
	}


void CloseDDLib(void)
	{
	if (ddOpenCnt)
		{
		ddOpenCnt--;
		if (!ddOpenCnt)
			{
			FreeLibrary(ddInst);
			ddInst = NULL;
			}
		}
	}


BOOL GetDDPrimaryCaps (void)
	{
	HWND					hwnd;

	LPDIRECTDRAW			lpDD = NULL;
	LPDIRECTDRAWSURFACE	lpDDSPrimary = NULL;

	DDSURFACEDESC		ddsd;
	HRESULT				ddrval;

	BOOL					success = FALSE;

	if (! OpenDDLib())
		{
		DP(__TEXT("Can't open DDraw"));
		}
	else
		{
		hwnd = CreateWindowEx(NULL,
									 __TEXT("STATIC"),
									 __TEXT("DDIFWIN"),
									 WS_POPUP,
									 -20, -20, 10, 10,
									 NULL, NULL, hinst, NULL);

		if (! hwnd)
			{
			DP(__TEXT("CreateWindowEx failed"));
			}
		else
			{
			ddrval = xDirectDrawCreate (NULL, &lpDD, NULL);
			if (ddrval != DD_OK)
				{
				DP(__TEXT("xDirectDrawCreate failed"));
				}
			else
				{
				ddrval = lpDD->SetCooperativeLevel (hwnd, DDSCL_NORMAL);
				if (ddrval != DD_OK)
					{
					DP(__TEXT("SetCooperativeLevel failed"));
					}
				else
					{
					ddsd.dwSize = sizeof(DDSURFACEDESC);
					ddsd.dwFlags = DDSD_CAPS;
					ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
					ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
					if( ddrval != DD_OK )
						{
						DP(__TEXT("CreateSurface failed"));
						}
					else
						{
						DDSURFACEDESC desc;
						desc.dwSize = sizeof(desc);

						ddrval = lpDDSPrimary->Lock(NULL, &desc, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL);

						if( ddrval == DD_OK )
							{
							lpDDSPrimary->Unlock(&desc);
							if ((desc.dwFlags & (DDSD_PITCH | DDSD_PIXELFORMAT)) ==
								 (DDSD_PITCH | DDSD_PIXELFORMAT))
								{
								// Needed members are there.
								if (desc.ddpfPixelFormat.dwFlags & DDPF_RGB)
									{
									PIPFormat pixelFormat;
									success = TRUE;
									switch (desc.ddpfPixelFormat.dwRGBBitCount)
										{
										case 15:
											pixelFormat = PFMT_RGB_555;
											break;
										case 16:
										case DDBD_16:
											if (desc.ddpfPixelFormat.dwRBitMask == 0x7C00  &&
											    desc.ddpfPixelFormat.dwGBitMask == 0x03E0  &&
											    desc.ddpfPixelFormat.dwBBitMask == 0x001F)
												pixelFormat = PFMT_RGB_555;
											else
												pixelFormat = PFMT_RGB_565;
											break;
										case 24:
										case DDBD_24:
											pixelFormat = PFMT_RGB_888;
											break;
										case 32:
										case DDBD_32:
											pixelFormat = PFMT_RGB_888x;
											break;
										default:
											success = FALSE;
											break;
										}
									if (success)
										{
										INIT_IOCTL(SET_PIP_BASES);
										in.logicalBase = desc.lpSurface;
										in.bytesPerRow = desc.lPitch;
										in.pixelFormat = pixelFormat;
										DO_IOCTL(SET_PIP_BASES);
										if (IS_ERROR(out.error))
											success = FALSE;
										}
									}
								}
							}
						lpDDSPrimary->Release();
						}
					}
				lpDD->Release();
				}
			DestroyWindow( hwnd );
			}
		CloseDDLib();
		}
	return success;
	}





void SendWinDesc (IUnitSet units)
	{
	TAG tags[2];
	WinDesc wd;
	WinDescRect wr[32];
	WinDescRect *wp;
	RECT rect, mrect, hr;
	POINT pos;
	HDC hdc;
	HWND hwnd, hw2;
	HBRUSH hbr, hbrOld;
	DWORD style;
	BOOL childMode;
	int orgLeft, orgTop;
	int screenWidth, screenHeight, width, height;
	RECT fullRect;


	if (units->pipWindow == NULL  ||  ! ::IsWindowVisible (units->pipWindow))
		{
		tags[0] = SET_PIP_WINDOW_DESC(NULL);
		tags[1] = TAGDONE;
		}
	else
		{
		tags[0] = SET_PIP_WINDOW_DESC(&wd);
		tags[1] = TAGDONE;

		// Get the window main rect in screen coordinates.
		::GetClientRect (units->pipWindow, &rect);
		hdc = ::GetDC (units->pipWindow);
		::GetDCOrgEx (hdc, &pos);
		::ReleaseDC (units->pipWindow,hdc);
		orgLeft = pos.x;
		orgTop = pos.y;
		rect.left   += orgLeft;
		rect.top    += orgTop;
		rect.right  += orgLeft;
		rect.bottom += orgTop;

		DriverInstanceHandle.colorKeyed = FALSE;
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		INIT_IOCTL(IS_PIP_OVERLAY);
		in.units = units->kunits;
		in.targetWidth = width;
		in.targetHeight = height;
		DO_IOCTL(IS_PIP_OVERLAY);

		if (! IS_ERROR(out.error)  &&  out.maxDMAWidth > 0  &&
		    DriverInstanceHandle.overlayPossible  &&  DriverInstanceHandle.ddAccess)
			{
			DDCtrlIn ddIn;
			DDCtrlOut ddOut;

			ddIn.colorKey = DriverInstanceHandle.keyColor;
			ddIn.left = rect.left;
			ddIn.top = rect.top;
			ddIn.width = width;
			ddIn.height = height;
			ddIn.minDMAWidth = 16;
			ddIn.minDMAHeight = 16;
			ddIn.maxDMAWidth = out.maxDMAWidth;
			ddIn.maxDMAHeight = out.maxDMAHeight;
			ddIn.pipFormats = 0xffffffff;

#if OVERLAY_SUPPORT
			if (VD32_SetDDOverlay (DriverInstanceHandle.ddAccess, ddIn, ddOut))
				{
				INIT_IOCTL(SET_PIP_OFFSCREEN);
				in.units = units->kunits;
				in.logicalBase = (FPTR)ddOut.destAddress;
				in.stride = ddOut.stride;
				in.format = (PIPFormat)ddOut.pipFormat;
				in.left = ddOut.dmaLeft;
				in.top = ddOut.dmaTop;
				in.width = ddOut.dmaWidth;
				in.height = ddOut.dmaHeight;
				DO_IOCTL(SET_PIP_OFFSCREEN);

				if (IS_ERROR(out.error))
					{
					VD32_RemoveDDOverlay (DriverInstanceHandle.ddAccess);
					}
				else
					{
					// Overlay successful.
					DP(__TEXT("Installed overlay"));
					DriverInstanceHandle.colorKeyed = TRUE;
					}
				}
#endif
			}

//		if (! DriverInstanceHandle.colorKeyed)
			{
			// Use inlay.
			INIT_IOCTL(SET_PIP_OFFSCREEN);
			in.units = units->kunits;
			in.logicalBase = NULL;   // switch off overlay, use inlay instead
			DO_IOCTL(SET_PIP_OFFSCREEN);
//			DP(__TEXT("Installing inlay"));

			// Initialize the windows description and its clip list.
			wd.frame = rect;
			wd.clips = 1;
			wd.clip = wr;
			wr[0].include = TRUE;
			wr[0].pos = wd.frame;
			screenWidth = ::GetSystemMetrics (SM_CXSCREEN);
			screenHeight = ::GetSystemMetrics (SM_CYSCREEN);

			// See if the window is moved over the screen limitations and mask these parts.
			if (wr[0].pos.left < 0) wr[0].pos.left = 0;
			if (wr[0].pos.top  < 0) wr[0].pos.top = 0;
			if (wr[0].pos.right  > screenWidth ) wr[0].pos.right = screenWidth;
			if (wr[0].pos.bottom > screenHeight) wr[0].pos.bottom = screenHeight;


			// Clip against all child windows.
			for (hw2 = ::GetWindow (units->pipWindow, GW_CHILD);  hw2 != NULL;  hw2 = GetWindow (hw2, GW_HWNDNEXT))
				{
				style = ::GetWindowLong (hw2, GWL_STYLE);
				if (::IsWindowVisible (hw2)  &&  (!(style & WS_CHILD) || (style & WS_CLIPSIBLINGS)))
					{
					// Add rectangle to the clip list.
					::GetWindowRect (hw2, &hr);
					wp = wr + wd.clips;
					wd.clips++;
					wp->include = FALSE;
					wp->pos.left   = hr.left;
					wp->pos.top    = hr.top;
					wp->pos.right  = hr.right;
					wp->pos.bottom = hr.bottom;
					}
				}


			childMode = TRUE;
			hwnd = units->pipWindow;
			do {
				if (childMode)
					{
					// Clip into the surrounding window
					::GetClientRect (hwnd, &mrect);
					hdc = ::GetDC (hwnd);
					::GetDCOrgEx (hdc, &pos);
					::ReleaseDC (hwnd, hdc);
					mrect.left = pos.x;
					mrect.top  = pos.y;
					mrect.right  += mrect.left;
					mrect.bottom += mrect.top;

					if (rect.left < mrect.left)   // check left edge
						{
						wp = wr + wd.clips;
						wd.clips++;
						wp->include = FALSE;
						wp->pos.left   = rect.left;
						wp->pos.top    = rect.top;
						wp->pos.right  = mrect.left;
						wp->pos.bottom = rect.bottom;
						}
					if (rect.top < mrect.top)   // check top edge
						{
						wp = wr + wd.clips;
						wd.clips++;
						wp->include = FALSE;
						wp->pos.left   = rect.left;
						wp->pos.top    = rect.top;
						wp->pos.right  = rect.right;
						wp->pos.bottom = mrect.top;
						}
					if (rect.right > mrect.right)   // check right edge
						{
						wp = wr + wd.clips;
						wd.clips++;
						wp->include = FALSE;
						wp->pos.left   = mrect.right;
						wp->pos.top    = rect.top;
						wp->pos.right  = rect.right;
						wp->pos.bottom = rect.bottom;
						}
					if (rect.bottom > mrect.bottom)   // check bottom edge
						{
						wp = wr + wd.clips;
						wd.clips++;
						wp->include = FALSE;
						wp->pos.left   = rect.left;
						wp->pos.top    = mrect.bottom;
						wp->pos.right  = rect.right;
						wp->pos.bottom = rect.bottom;
						}
					} // if (childMode)

				// Now iterate through all siblings in front of us.
				hw2 = hwnd;
				while (hwnd = ::GetWindow (hwnd, GW_HWNDPREV))
					{
					style = ::GetWindowLong (hwnd, GWL_STYLE);
					if (::IsWindowVisible (hwnd)  &&  (!(style & WS_CHILD) || (style & WS_CLIPSIBLINGS)))
						{
						// Add rectangle to the clip list.
						::GetWindowRect (hwnd, &hr);
						wp = wr + wd.clips;
						wd.clips++;
						wp->include = FALSE;
						wp->pos.left   = hr.left;
						wp->pos.top    = hr.top;
						wp->pos.right  = hr.right;
						wp->pos.bottom = hr.bottom;
						}
					}

				// Check if we are still a child.
				if (! (::GetWindowLong (hw2, GWL_STYLE) & WS_CHILD))
					childMode = FALSE;

				// Advance to the next parent window level.
				hwnd = GetParent (hw2);

				} while (::GetWindowLong (hw2, GWL_STYLE) & WS_CHILD);
			}
		}


	INIT_IOCTL(CONFIGURE_UNITS);
	in.units = units->kunits;
	in.tags = tags;
	DO_IOCTL(CONFIGURE_UNITS);
	}





static BOOL IsPIPWindow (HWND win)
	{
	int i;
	HWND hw;

	for (i = firstActive;  i != -1;  i = Units[i].next)
		{
		if (Units[i].pipEnabled)
			{
			hw = Units[i].pipWindow;

			while (hw != NULL)
				{
				if (hw == win) return TRUE;
				if (::GetWindowLong (hw, GWL_STYLE) & WS_CHILD)
					hw = GetParent(hw);
				else
					hw = NULL;
				}
			}
		}
	return FALSE;
	}





BOOL Affects (WINDOWPOS *hpos)
	{
	HWND hw, pw, aw;
	RECT rect;
	DWORD pos;
	HDC hdc;
	RECT hr, pr;
	POINT p;
	BOOL parentVisible;


	hw = hpos->hwnd;   // the window to be moved, sized, etc.

	::GetWindowRect (hw, &hr);
	if (::GetWindowLong (hw, GWL_STYLE) & WS_CHILD)
		{
//		DP(__TEXT("Is child"));
		p.x = hr.left;
		p.y = hr.top;
		pw = (HWND) ::GetWindowLong (hw, GWL_HWNDPARENT);
		::GetWindowRect (pw, &pr);
		::ScreenToClient (pw, &p);
		pr.left   = p.x;
		pr.top    = p.y;
		pr.right  = hr.right  - hr.left + pr.left;
		pr.bottom = hr.bottom - hr.top  + pr.top;

		parentVisible = TRUE;

		while (parentVisible  &&  pw)
			{
			parentVisible &= IsWindowVisible (pw);
			pw = (HWND)::GetWindowLong (pw, GWL_HWNDPARENT);
			}
		}
	else
		{
		// We are not a child.
		pr = hr;
		parentVisible = TRUE;
		}

	//DP(__TEXT("Flags %08lx"), hpos->flags);
/*
	DP(__TEXT("%x %x %08lx (%x) (%d,%d)-(%d,%d) (%d,%d)-(%d,%d)"), hw, pw, hpos->flags,
			      hpos->hwndInsertAfter,
			      hpos->x, hpos->y,hpos->cx, hpos->cy,
			      pr.left, pr.top, pr.right-pr.left, pr.bottom-pr.top);
  */

	if (! parentVisible)
		return FALSE;

	// If a window will show and is invisible, the PIP is affected.
	if ((SWP_SHOWWINDOW & hpos->flags)  &&  ! IsWindowVisible (hw))
		{
		//DP(__TEXT("win real show"));
		return TRUE;
		}
	if (0 && ! IsWindowVisible (hw))
		{
		//DP(__TEXT("win invisible"));
		return FALSE;
		}

	// If a window was really moved or sized, the PIP is affected.
	if (! (SWP_NOMOVE & hpos->flags)  &&  (hpos->x != pr.left  ||  hpos->y != pr.top))
		{
		//DP(__TEXT("win real move"));
		return TRUE;
		}
	if (! (SWP_NOSIZE & hpos->flags)  &&  (hpos->cx != pr.right-pr.left  ||  hpos->cy != pr.bottom-pr.top))
		{
		//DP(__TEXT("win real size"));
		return TRUE;
		}

	// If a window will hide and is visible, the PIP is affected.
	if ((SWP_HIDEWINDOW & hpos->flags)  &&  IsWindowVisible (hw))
		{
		//DP(__TEXT("win real hide"));
		return TRUE;
		}

/*	HWND tw = hw;
	while(tw)
		{
		DP(__TEXT("ZOrder %08lx"), tw);
		tw = ::GetWindow(tw, GW_HWNDPREV);
		}
*/
	// If the Z order is the same, the PIP is not affected.
	if (SWP_NOZORDER & hpos->flags)
		return FALSE;

	// Now, the Z order decides.
	// If all windows between hw and hpos->hwndInsertAfter are
	// invisible, we need not do anything. Else the PIP is affected.
	BOOL pipMoved = IsPIPWindow(hw);

	parentVisible = FALSE;
	HWND prev = ::GetWindow (hw, GW_HWNDPREV);
	while (prev != NULL  &&  prev != hpos->hwndInsertAfter)
		{
		if (IsWindowVisible (prev))
			{
			if (pipMoved)
				{
				//DP(__TEXT("  pipmoved %08lx visible"), prev);
				parentVisible = TRUE;
				}
			else if (IsPIPWindow(prev))
				{
				//DP(__TEXT("  ispip %08lx visible"), prev);
				parentVisible = TRUE;
				}
//			else
				//DP(__TEXT("neither pip %08lx visible"), prev);
			}
//		else
			//DP(__TEXT("  win %08lx not visible"), prev);
		prev = ::GetWindow (prev, GW_HWNDPREV);
		}
	if (prev == hpos->hwndInsertAfter)
		{
		return parentVisible;
		}

	// The hwndInsertAfter window is not in front of us, so it
	// must be behind us. Again, decide on visibility.
	//DP(__TEXT("  got to NULL"));
	parentVisible = FALSE;
	prev = ::GetWindow (hw, GW_HWNDNEXT);
	while (prev != NULL  &&  prev != hpos->hwndInsertAfter)
		{
		if (IsWindowVisible (prev))
			{
//			DP(__TEXT("  win %08lx visible"), prev);
			if (pipMoved)
				{
				//DP(__TEXT("  pipmoved %08lx visible"), prev);
				parentVisible = TRUE;
				}
			else if (IsPIPWindow(prev))
				{
				//DP(__TEXT("  ispip %08lx visible"), prev);
				parentVisible = TRUE;
				}
//			else
//				//DP(__TEXT("neither pip %08lx visible"), prev);
			}
//		else
			//DP(__TEXT("  win %08lx not visible"), prev);
		prev = ::GetWindow (prev, GW_HWNDNEXT);
		}
	if (prev == hpos->hwndInsertAfter)
		{
		if (IsWindowVisible (hpos->hwndInsertAfter)  &&  IsPIPWindow(hpos->hwndInsertAfter))
			{
			//DP(__TEXT("  after %08lx visible"), prev);
			return TRUE;
			}
		return parentVisible;
		}
	//DP(__TEXT("  win already bottom2"));
	return FALSE;
	}



LRESULT CALLBACK CallWndProc (int code, WPARAM wParam, LPARAM lParam)
	{
	int i;

	if (code >= 0)
		{
#if RET_HOOK
		CWPRETSTRUCT *msg = (CWPRETSTRUCT *)lParam;
#else
		CWPSTRUCT *msg = (CWPSTRUCT *)lParam;
#endif

		if (msg->message == WM_WINDOWPOSCHANGING)
			{
			LPWINDOWPOS pos = (LPWINDOWPOS)(msg->lParam);

			//DP(__TEXT("CallWndProc: Pos changing"));

			WaitForSingleObject (clipMutex, 0x7fffffff);

			if (Affects (pos) /*&& !lastMoved*/)
				{
				//DP(__TEXT("lastMoved is %08lx"), msg->hwnd);
				lastMoved = msg->hwnd;

//				DP(__TEXT("CallWndProc: check start"));
#if OVERLAY_SUPPORT
				if (DriverInstanceHandle.colorKeyed)
					{
					//DP(__TEXT("removing overlay"));
					VD32_RemoveDDOverlay (DriverInstanceHandle.ddAccess);
					DriverInstanceHandle.colorKeyed = FALSE;
					}
#endif
//				DP(__TEXT("CallWndProc: check end"));

DP(__TEXT("P-"));

				// Switch off all enabled PIPs.
				for (i = firstActive;  i != -1;  i = Units[i].next)
					{
					if (Units[i].pipEnabled)
						{
						/*
						if (!msgPending)
							{
							msgPending = TRUE;
							PostMessage(Units[i].pipWindow, doneMsg, 0, 0);
							}
						*/
//DP(__TEXT("CallWndProc: pip off"));
						INIT_IOCTL(ENABLE_PIP);
						in.units = Units[i].kunits;
						in.enable = FALSE;
						DO_IOCTL(ENABLE_PIP);
						}
					}

				}
			else if (lastMoved != NULL)
				{
				//DP(__TEXT("Hey last moved is set"));
				}
			ReleaseMutex (clipMutex);
			}
		else if (msg->message == WM_WINDOWPOSCHANGED  &&  lastMoved) //msg->hwnd == lastMoved)
			{
			//DP(__TEXT("CallWndProc: Pos changed"));

			WaitForSingleObject (clipMutex, 0x7fffffff);

			msgPending = FALSE;

DP(__TEXT("P+"));

			for (i = firstActive;  i != -1;  i = Units[i].next)
				{
				if (Units[i].pipEnabled)
					{
					SendWinDesc (Units + i);

//DP(__TEXT("CallWndProc: pip on"));
					INIT_IOCTL(ENABLE_PIP);
					in.units = Units[i].kunits;
					in.enable = TRUE;
					DO_IOCTL(ENABLE_PIP);

					VDR_UpdatePIP (TU(Units + i));
					}
				}

			//DP(__TEXT("lastMoved is NULL"));
			lastMoved = NULL;

			ReleaseMutex (clipMutex);
			}
		}

	return CallNextHookEx (sizeHook, code, wParam, lParam);
	}





LRESULT CALLBACK GetMsgProc (int code, WPARAM wParam, LPARAM lParam)
	{
	int i;

	if (code >= 0)
		{
		MSG *msg = (MSG *)lParam;

		if (msg->message == doneMsg)
			{
			WaitForSingleObject (clipMutex, 0x7fffffff);

			msgPending = FALSE;

			for (i = firstActive;  i != -1;  i = Units[i].next)
				{
				if (Units[i].pipEnabled)
					{
					SendWinDesc (Units + i);

					INIT_IOCTL(ENABLE_PIP);
					in.units = Units[i].kunits;
					in.enable = TRUE;
					DO_IOCTL(ENABLE_PIP);
					}
				}

			ReleaseMutex (clipMutex);
			}
		}

	return CallNextHookEx (doneHook, code, wParam, lParam);
	}



void VDR_ServiceProcess (void)
	{
	BOOL terminate = FALSE;

	DP(__TEXT("Entering PIP Service Process"));
	while (! terminate)
		{
		WaitForSingleObject (ServiceRequest, INFINITE);
		switch (ServiceCommand)
			{
			case DD_INSTALLHOOK:
				DP(__TEXT("Installing PIP hook"));
#if RET_HOOK
				sizeHook = SetWindowsHookEx(WH_CALLWNDPROCRET,
													 (HOOKPROC)CallWndProc, hinst,
													 NULL);
#else
				sizeHook = SetWindowsHookEx(WH_CALLWNDPROC,
													 (HOOKPROC)CallWndProc, hinst,
													 NULL);
#endif
				break;
			case DD_REMOVEHOOK:
				DP(__TEXT("Removing PIP hook"));
				if (sizeHook) UnhookWindowsHookEx(sizeHook);
				sizeHook = NULL;
				break;
			case DD_TERMINATE:
				terminate = TRUE;
				break;
			case DD_NONE:
				break;
			default:
				//DP(__TEXT("Invalid command %ld"), ServiceCommand);
				break;
			}
		ServiceCommand = DD_NONE;
		SetEvent (ServiceDone);
		}

	// Leaving the loop means to terminate.
	DP(__TEXT("Terminate PIP Service Process"));
	}


#if USE_HOOK_PROCESS

int CreateServiceProcess (void)
	{
	if (ServiceProcesses == 0  &&  ! ServiceActive)
		{
		STARTUPINFO startupInfo =	{
											sizeof(STARTUPINFO),
											NULL, NULL, NULL,
											0, 0, 0, 0,
											0, 0,
											0,
											STARTF_FORCEOFFFEEDBACK,
											0,
											0,
											NULL, NULL, NULL, NULL
											};
DP(__TEXT("Creating Hook process\n\n"));
		ServiceActive = TRUE;
		ServiceCommand = DD_NONE;
		if (! CreateProcess (NULL,
									__TEXT("VDCMSERV"),
									NULL, NULL,
									FALSE,
									CREATE_NEW_PROCESS_GROUP | NORMAL_PRIORITY_CLASS,
									NULL,
									NULL,
									&startupInfo,
									&processInfo))
			{
			DP(__TEXT("  can't create hook process"));
			ServiceActive = FALSE;
			return FALSE;
			}
		}
//	else
		ServiceProcesses++;
	return TRUE;
	}


void FinishServiceProcess (void)
	{
//	if (ServiceProcesses > 0)
		ServiceProcesses--;
	if (ServiceProcesses <= 0  &&  ServiceActive)
		{
DP(__TEXT("Deleting hook process\n\n"));
		WaitForSingleObject (ServiceLock, INFINITE);
		ServiceCommand = DD_TERMINATE;
		ServiceActive = FALSE;
		SetEvent (ServiceRequest);
		WaitForSingleObject (ServiceDone, INFINITE);
		SetEvent (ServiceLock);
		WaitForSingleObject (processInfo.hProcess, 2000);
		CloseHandle (processInfo.hProcess);
		CloseHandle (processInfo.hThread);
		}
	}

#endif

#endif	// PIP_SUPPORT



///////////////////////////////////////////////////////////////////////////////
// Exported DLL Functions
///////////////////////////////////////////////////////////////////////////////


DllEXPORT	void	WINAPI VDR_RegisterCallback(GenericHook * hook)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_RegisterCallback+"));
#endif

	if (!WorkerThread) InitThreadContext();
	hook->Register(WorkerQueue);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_RegisterCallback-"));
#endif
	}

DllEXPORT	void	WINAPI VDR_UnregisterCallback(GenericHook * hook)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_UnregisterCallback+"));
#endif

	if (WorkerThread && WorkerQueue)
		{
		INIT_IOCTL(UNREGISTER_CALLBACK);

		in.queue = WorkerQueue;
		in.hook	= hook;

		DO_IOCTL(UNREGISTER_CALLBACK);
		}

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_UnregisterCallback-"));
#endif
	}


DllEXPORT Error WINAPI VDR_OpenDriver (TCHAR __far *name, DWORD boardID, VDRHandle __far &handle)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_OpenDriver+"));
#endif

	if (DriverOpenCnt == 0)
		{
		if (!WorkerThread) InitThreadContext();

		GNREASSERT(DriverInstanceHandle.Initialize());
//		GetDDPrimaryCaps ();
		}

	DriverOpenCnt++;

	handle = &DriverInstanceHandle;

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_OpenDriver-"));
#endif

	GNRAISE_OK;
	}


DllEXPORT Error WINAPI VDR_CloseDriver (VDRHandle handle)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_CloseDriver+"));
#endif

	if (DriverOpenCnt)
		{
		if (handle == &DriverInstanceHandle)
			{
			DriverOpenCnt--;

/*			INIT_IOCTL(CLOSE_DRIVER);
			in.handle = driver->handle;
			DO_IOCTL(CLOSE_DRIVER);

			if (IS_ERROR(out.error))
				GNRAISE(out.error);
*/

			if (DriverOpenCnt == 0)
				{
				if (WorkerThread) FiniThreadContext();
				DriverInstanceHandle.Free ();
				}
			}
		else
			{
			DP(__TEXT("VDR_CLoseDriver: invalid handle!"));
			GNRAISE(GNR_OBJECT_NOT_FOUND);
			}
		}

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_CloseDriver-"));
#endif

	GNRAISE_OK;
	}

DllEXPORT Error WINAPI VDR_ReconfigureDriver(VDRHandle handle)
	{
	INIT_IOCTL(RECONFIGURE_DRIVER);
	DO_IOCTL(RECONFIGURE_DRIVER);

	return out.error;
	}

DllEXPORT DWORD WINAPI VDR_AvailUnits (VDRHandle handle)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_AvailUnits+"));
#endif

	INIT_IOCTL(AVAIL_UNITS);
//	in.handle = driver->handle;
	DO_IOCTL(AVAIL_UNITS);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_AvailUnits-"));
#endif

	return out.units;
	}


DllEXPORT Error WINAPI VDR_OpenUnits (VDRHandle handle, DWORD requnits, UnitSet __far &units)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_OpenUnits+"));
#endif

	IUnitSet	iunits;


#if PIP_SUPPORT
	if (requnits & PIP_UNIT)
		{
		GetDDPrimaryCaps ();
		}
#endif

	INIT_IOCTL(ALLOCATE_UNITS);

	units = NULL;
	in.units = requnits;

	DO_IOCTL(ALLOCATE_UNITS);

	if (!IS_ERROR(out.error))
		{
		if (!openCnt)
			{
			InitUnits();
			}
		openCnt++;

		if (!WorkerThread) InitThreadContext();

		iunits = AllocUnit(handle);
		units = TU(iunits);
		iunits->kunits = out.units;

		if (requnits & MPEG_DECODER_UNIT)
			iunits->hasMPEGDecoder = TRUE;
		}

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_OpenUnits-"));
#endif

	GNRAISE(out.error);
	}

DllEXPORT	Error WINAPI VDR_OpenSubUnits(UnitSet parent, DWORD requnits, UnitSet __far &units)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_OpenSubUnits+"));
#endif

	IUnitSet	iunits;

	INIT_IOCTL(ALLOC_SUB_UNITS);

	units = NULL;
	in.parent = TU(parent)->kunits;
	in.units = requnits;

	DO_IOCTL(ALLOC_SUB_UNITS);

	if (!IS_ERROR(out.error))
		{
		iunits = AllocUnit(TU(parent)->handle);
		units = TU(iunits);
		iunits->kunits = out.units;

		if (requnits & MPEG_DECODER_UNIT)
			iunits->hasMPEGDecoder = TRUE;

		openCnt++;
		}

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_OpenSubUnits-"));
#endif

	GNRAISE(out.error);
	}

DllEXPORT	Error WINAPI VDR_CloseUnits(UnitSet units)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_CloseUnits+"));
#endif
//DP(__TEXT("VDR_CloseUnits %lx"), TU(units)->id);

	if (TU(units)->pipEnabled)
		VDR_EnablePIP (units, FALSE);

	INIT_IOCTL(FREE_UNITS);

	in.units = TU(units)->kunits;

	DO_IOCTL(FREE_UNITS);

	FreeUnit(TU(units));

	openCnt--;
	if (!openCnt)
		{
		}

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_CloseUnits-"));
#endif

	GNRAISE(out.error);
	}

#pragma optimize("", off)
DllEXPORT	Error WINAPI VDR_ConfigureUnits(UnitSet units, TAG __far * tags)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_ConfigureUnits+"));
#endif
//DP(__TEXT("VDR_ConfigureUnits+"));

	BOOL changed = FALSE;

	INIT_IOCTL(CONFIGURE_UNITS);

	in.units = TU(units)->kunits;
	in.tags = tags;

#if PIP_SUPPORT
	PARSE_TAGS_START(tags);
		GETSETC(PIP_WINDOW, TU(units)->pipWindow);
	PARSE_TAGS_END;
#endif

	DO_IOCTL(CONFIGURE_UNITS);

#if PIP_SUPPORT
	{
	BOOL needUpdate = FALSE;
	PARSE_TAGS_START(tags);
		case CSET_PIP_DEST_LEFT:
		case CSET_PIP_DEST_TOP:
		case CSET_PIP_DEST_WIDTH:
		case CSET_PIP_DEST_HEIGHT:
			needUpdate = TRUE;
			break;
	PARSE_TAGS_END;
	if (needUpdate)
		VDR_UpdatePIP (units);
	}

	WaitForSingleObject (clipMutex, 0x7fffffff);

	if (changed && TU(units)->pipEnabled)
		SendWinDesc(TU(units));

	ReleaseMutex (clipMutex);
#endif

	Sleep(1);


//DP(__TEXT("VDR_ConfigureUnits-"));

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_ConfigureUnits-"));
#endif

	GNRAISE(out.error);
	}

#pragma optimize("", on)


DllEXPORT	Error WINAPI VDR_LockUnits(UnitSet units)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_LockUnits+"));
#endif

	INIT_IOCTL(LOCK_UNITS);

	in.units = TU(units)->kunits;

	DO_IOCTL(LOCK_UNITS);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_LockUnits-"));
#endif

	GNRAISE(out.error);
	}

DllEXPORT	Error WINAPI VDR_UnlockUnits(UnitSet units)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_UnlockUnits+"));
#endif

	INIT_IOCTL(UNLOCK_UNITS);

	in.units = TU(units)->kunits;

	DO_IOCTL(UNLOCK_UNITS);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_UnlockUnits-"));
#endif

	GNRAISE(out.error);
	}


DllEXPORT	Error WINAPI VDR_ActivateUnits(UnitSet units)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_ActivateUnits+"));
#endif

	INIT_IOCTL(ACTIVATE_UNITS);

	in.units = TU(units)->kunits;

	DO_IOCTL(ACTIVATE_UNITS);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_ActivateUnits-"));
#endif

	GNRAISE(out.error);
	}


DllEXPORT	Error WINAPI VDR_PassivateUnits(UnitSet units)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_PassivateUnits+"));
#endif

	INIT_IOCTL(PASSIVATE_UNITS);

	in.units = TU(units)->kunits;

	DO_IOCTL(PASSIVATE_UNITS);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_PassivateUnits-"));
#endif

	GNRAISE(out.error);
	}



DllEXPORT	Error WINAPI VDR_EnablePIP(UnitSet units, BOOL enable)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_EnablePIP+"));
#endif

#if PIP_SUPPORT
	BOOL switchOff = (! enable  &&  TU(units)->pipEnabled);

	DP(__TEXT("VDR_EnablePIP: pip enable %d, %lx"), enable, TU(units)->id);

	if (lastMoved != NULL)
		{
		DP(__TEXT("  enable delayed (lastMoved)"));
		// Try again for 2 seconds, then ignore it.
		for (int i = 0;  i < 25*2  &&  lastMoved != NULL;  i++)
			Sleep (40);

		if (lastMoved != NULL)
			DP(__TEXT("  still not NULL"));
		}

	if (enable  &&  ! TU(units)->pipEnabled)
		{
		// A new PIP is enabled. Install hooks on the first one.
		if (activePIPCnt == 0)
			{
#if WIN_SUPPORT
			doneMsg = RegisterWindowMessage(__TEXT("Done Resizing"));
#if USE_HOOK_PROCESS
			CreateServiceProcess ();
			if (ServiceActive)
				{
				DP(__TEXT("  invoking pip install"));
				WaitForSingleObject (ServiceLock, INFINITE);
				ServiceCommand = DD_INSTALLHOOK;
				SetEvent (ServiceRequest);
				WaitForSingleObject (ServiceDone, INFINITE);
				SetEvent (ServiceLock);
				DP(__TEXT("  invoking done"));
				}
#else
#if RET_HOOK
			sizeHook = SetWindowsHookEx(WH_CALLWNDPROCRET,
												 (HOOKPROC)CallWndProc, hinst,
												 NULL);
#else
			sizeHook = SetWindowsHookEx(WH_CALLWNDPROC,
												 (HOOKPROC)CallWndProc, hinst,
												 NULL);
#endif
#endif	// USE_HOOK_PROCESS
#endif
			}
		activePIPCnt++;
		}

	WaitForSingleObject (clipMutex, 0x7fffffff);

	INIT_IOCTL(ENABLE_PIP);

//	if (enable && ! TU(units)->pipEnabled)
	if (enable || TU(units)->pipEnabled)
		{
		SendWinDesc(TU(units));
		}

	TU(units)->pipEnabled = enable;

	in.units = TU(units)->kunits;
	in.enable = enable;

	DO_IOCTL(ENABLE_PIP);

	ReleaseMutex (clipMutex);

	if (switchOff)
		{
		activePIPCnt--;
		DP(__TEXT("  activePIPCnt Off %ld"), activePIPCnt);
		if (activePIPCnt == 0)
			{
#if USE_HOOK_PROCESS
			if (ServiceActive)
				{
				DP(__TEXT("  invoking pip remove"));
				WaitForSingleObject (ServiceLock, INFINITE);
				ServiceCommand = DD_REMOVEHOOK;
				SetEvent (ServiceRequest);
				WaitForSingleObject (ServiceDone, INFINITE);
				SetEvent (ServiceLock);
				DP(__TEXT("  invoking done"));
				}
			FinishServiceProcess ();
#else
//			if (doneHook) UnhookWindowsHookEx(doneHook);
			if (sizeHook) UnhookWindowsHookEx(sizeHook);
//			doneHook = NULL;
			sizeHook = NULL;
#endif	// USE_HOOK_PROCESS
			}
		}

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_EnablePIP-"));
#endif

	GNRAISE(out.error);
#else

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_EnablePIP-"));
#endif

	GNRAISE_OK;
#endif
	}


Error InternalConfigureUnits(UnitSet units, TAG __far * tags)
	{
	INIT_IOCTL(CONFIGURE_UNITS);

	in.units = TU(units)->kunits;
	in.tags = tags;

	DO_IOCTL(CONFIGURE_UNITS);

	GNRAISE(out.error);
	}

Error InternalConfigureUnitsTags (UnitSet units, TAG tags, ...)
	{
	return InternalConfigureUnits (units, &tags);
	}


DllEXPORT	Error WINAPI VDR_UpdatePIP(UnitSet units)
	{
	HDC		dstDC;
	HBRUSH	hbrOld, hbr;
	RECT		rect;
	POINT		org;
	int		srcWidth, srcHeight;
	DWORD		dwLen;
	DWORD		dwBytesPerLine;
	Error		error = GNR_OK;

	BITMAPINFOHEADER		bi;
	LPSTR						lpDIBBits;   // pointer to DIB bits
	LPBITMAPINFOHEADER	dib;

#if TRACE_VDR_CALLS
	//DP(__TEXT("VDR_UpdatePIP+"));
#endif
//DP(__TEXT("VDR_UpdatePIP %lx"), TU(units)->id);

	if (TU(units)->id >= MAX_UNITS)
		DP(__TEXT("id > MAX_UNITS!"));

#if PIP_SUPPORT

//DP(__TEXT("UpdatePIP"));
	WaitForSingleObject (clipMutex, 0x7fffffff);

	// Get source size of PIP

	error = InternalConfigureUnitsTags(units, GET_PIP_SOURCE_WIDTH(srcWidth),
															GET_PIP_SOURCE_HEIGHT(srcHeight),
															TAGDONE);
	if (!IS_ERROR(error))
		{
		if (srcWidth != 0 && srcHeight != 0)
			{
			WORD wBitCount = 24;
			DWORD dwWidth = (DWORD) (srcWidth / 2);
			DWORD dwHeight = (DWORD) (srcHeight / 2);

			bi.biSize = sizeof(BITMAPINFOHEADER);
			bi.biWidth = dwWidth;         // fill in width from parameter
			bi.biHeight = dwHeight;       // fill in height from parameter
			bi.biPlanes = 1;          		// must be 1
			bi.biBitCount = wBitCount;		// we only work with 24 bits
			bi.biCompression = 0L;			// uncompressed bitmap, BI_RGB
			bi.biSizeImage = 0;				// 0's here mean "default"
			bi.biXPelsPerMeter = 0;
			bi.biYPelsPerMeter = 0;
			bi.biClrUsed = 0;
			bi.biClrImportant = 0;

			dwBytesPerLine = WIDTHBYTES(wBitCount * dwWidth);
			dwLen = bi.biSize + (dwBytesPerLine * dwHeight);

			// Allocate memory for DIB with bitmap info header
			dib = (LPBITMAPINFOHEADER) malloc(dwLen);
			}

		if (dib)
			{
			*dib = bi;

			INIT_IOCTL(UPDATE_PIP);
			in.units = TU(units)->kunits;
			in.dib	= dib;
			DO_IOCTL(UPDATE_PIP);

			if (!IS_ERROR(out.error)  &&  TU(units)->pipWindow)
				{
//				DP(__TEXT("Before out.dib check"));
				::GetClientRect (TU(units)->pipWindow, &rect);

				if (out.dib)
					{
//					DP(__TEXT("restoring DIB"));
					dstDC = GetDC(TU(units)->pipWindow);

					lpDIBBits = (LPSTR)(((BYTE __far *)dib)+dib->biSize);

					::SetStretchBltMode (dstDC, COLORONCOLOR);		// Set stretch mode for color

					::StretchDIBits (dstDC, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
										 0, 0, dib->biWidth, dib->biHeight, lpDIBBits, (LPBITMAPINFO)dib, DIB_RGB_COLORS, SRCCOPY);

					ReleaseDC (TU(units)->pipWindow, dstDC);
					}
				else
					{
//					DP(__TEXT("Clearing area"));
					dstDC = GetDC (TU(units)->pipWindow);
					hbr = ::CreateSolidBrush (RGB(0,0,0));
					hbrOld = (HBRUSH) ::SelectObject (dstDC, hbr);
		//DP(__TEXT("client pos %ld, %ld"), (long)rect.left, (long)rect.top);
		//DP(__TEXT("client size %ld, %ld"), (long)(rect.right-rect.left), (long)(rect.bottom-rect.top));

					::GetDCOrgEx (dstDC, &org);
		//DP(__TEXT("DC org %ld, %ld"), (long)org.x, (long)org.y);
					if (out.destWidth	== 0  ||  out.destHeight == 0)
						::Rectangle (dstDC, rect.left, rect.top, rect.right, rect.bottom);
					else
						{
						::Rectangle (dstDC, rect.left, rect.top, rect.right, out.destTop-org.y);
						::Rectangle (dstDC, rect.left, out.destTop-org.y+out.destHeight, rect.right, rect.bottom);
						::Rectangle (dstDC, rect.left, out.destTop-org.y, out.destLeft-org.x, out.destTop-org.y+out.destHeight);
						::Rectangle (dstDC, out.destLeft-org.x+out.destWidth, out.destTop-org.y, rect.right, out.destTop-org.y+out.destHeight);
						}

					::SelectObject (dstDC, hbrOld);
					::DeleteObject (hbr);
					ReleaseDC (TU(units)->pipWindow, dstDC);
					}

				error = GNR_OK;
				}
			else
				{
				DP(__TEXT("Error in UPDATE_PIP ioctl"));
				error = out.error;
				}

			free(dib);
			}
		else
			{
			DP(__TEXT("########## no DIB!"));
			error = GNR_NOT_ENOUGH_MEMORY;
			}
		}

	ReleaseMutex (clipMutex);

#endif

#if TRACE_VDR_CALLS_E
	//DP(__TEXT("VDR_UpdatePIP-"));
#endif

	GNRAISE(error);
	}


DllEXPORT	Error WINAPI VDR_OptimizeBuffer(UnitSet units, int __far & minPixVal)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_OptimizeBuffer"));
#endif

	GNRAISE_OK;
	}


DllEXPORT	Error WINAPI VDR_GrabFrame(UnitSet units, FPTR base,
                            WORD width, WORD height,
                            WORD stride,
                            GrabFormat fmt)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_GrabFrame+"));
#endif

	INIT_IOCTL(GRAB_FRAME);

	in.units = TU(units)->kunits;
	in.base = base;
	in.width = width;
	in.height = height;
	in.stride = stride;
	in.fmt = fmt;

	DO_IOCTL(GRAB_FRAME);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_GrabFrame-"));
#endif

	GNRAISE(out.error);
	}



DllEXPORT	DWORD WINAPI VDR_SendMPEGData(UnitSet units, HPTR data, DWORD size)
	{
#if TRACE_VDR_CALLS
//DP(__TEXT("VDR_SendMPEGData+"));
#endif

	INIT_IOCTL(SEND_MPEG_DATA);

	in.units = TU(units)->kunits;
	in.data = data;
	in.size = size;

	DO_IOCTL(SEND_MPEG_DATA);

#if TRACE_VDR_CALLS_E
//DP(__TEXT("VDR_SendMPEGData-"));
#endif

	return out.done;
	}

DllEXPORT	void WINAPI VDR_CompleteMPEGData(UnitSet units)
	{
#if TRACE_VDR_CALLS
//DP(__TEXT("VDR_CompleteMPEGData+"));
#endif

	INIT_IOCTL(COMPLETE_MPEG_DATA);

	in.units = TU(units)->kunits;

	DO_IOCTL(COMPLETE_MPEG_DATA);

#if TRACE_VDR_CALLS_E
//DP(__TEXT("VDR_CompleteMPEGData-"));
#endif
	}


DllEXPORT	Error WINAPI VDR_SendMPEGCommand(UnitSet units, MPEGCommand com, long param, DWORD __far &tag)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_SendMPEGCommand+, %x"), com);
#endif

	INIT_IOCTL(SEND_MPEG_COMMAND);

	in.units = TU(units)->kunits;
	in.com = com;
	in.param = param;

	DO_IOCTL(SEND_MPEG_COMMAND);

	tag = out.tag;

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_SendMPEGCommand-"));
#endif

	GNRAISE(out.error);
	}

DllEXPORT	Error WINAPI VDR_DoMPEGCommand(UnitSet units, MPEGCommand com, long param)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_DoMPEGCommand+"));
#endif

	INIT_IOCTL(DO_MPEG_COMMAND);

	in.units = TU(units)->kunits;
	in.com = com;
	in.param = param;

	DO_IOCTL(DO_MPEG_COMMAND);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_DoMPEGCommand-"));
#endif

	GNRAISE(out.error);
	}

DllEXPORT	Error WINAPI VDR_CompleteMPEGCommand(UnitSet units, DWORD tag)
	{
#if TRACE_VDR_CALLS
//DP(__TEXT("VDR_CompleteMPEGCommand+"));
#endif

	INIT_IOCTL(COMPLETE_MPEG_COMMAND);

	in.units = TU(units)->kunits;
	in.tag = tag;

	DO_IOCTL(COMPLETE_MPEG_COMMAND);

#if TRACE_VDR_CALLS_E
//DP(__TEXT("VDR_CompleteMPEGCommand-"));
#endif

	GNRAISE(out.error);
	}

DllEXPORT BOOL	WINAPI VDR_MPEGCommandPending (UnitSet units, DWORD tag)
	{
#if TRACE_VDR_CALLS
//DP(__TEXT("VDR_MPEGCommandPending+"));
#endif

	INIT_IOCTL(MPEG_COMMAND_PENDING);

	in.units = TU(units)->kunits;
	in.tag = tag;

	DO_IOCTL(MPEG_COMMAND_PENDING);

#if TRACE_VDR_CALLS_E
//DP(__TEXT("VDR_MPEGCommandPending-"));
#endif

	return out.status;
	}

DllEXPORT	DWORD WINAPI VDR_CurrentMPEGLocation(UnitSet units)
	{
#if TRACE_VDR_CALLS
//DP(__TEXT("VDR_CurrentMPEGLocation+"));
#endif

	INIT_IOCTL(CURRENT_MPEG_LOCATION);

	in.units = TU(units)->kunits;

	DO_IOCTL(CURRENT_MPEG_LOCATION);

#if TRACE_VDR_CALLS_E
//DP(__TEXT("VDR_CurrentMPEGLocation-"));
#endif

	return out.pos;
	}

DllEXPORT DWORD WINAPI VDR_CurrentMPEGTransferLocation (UnitSet units)
	{
#if TRACE_VDR_CALLS
//DP(__TEXT("VDR_CurrentMPEGTransferLocation+"));
#endif

	INIT_IOCTL(CURRENT_MPEG_TRANSFER_LOCATION);

	in.units = TU(units)->kunits;

	DO_IOCTL(CURRENT_MPEG_TRANSFER_LOCATION);

#if TRACE_VDR_CALLS_E
//DP(__TEXT("VDR_CurrentMPEGTransferLocation-"));
#endif

	return out.pos;
	}

DllEXPORT	MPEGState WINAPI VDR_CurrentMPEGState(UnitSet units)
	{
#if TRACE_VDR_CALLS
//DP(__TEXT("VDR_CurrentMPEGState+"));
#endif

	INIT_IOCTL(CURRENT_MPEG_STATE);

	in.units = TU(units)->kunits;

	DO_IOCTL(CURRENT_MPEG_STATE);

#if TRACE_VDR_CALLS_E
//DP(__TEXT("VDR_CurrentMPEGState-"));
#endif

	return out.state;
	}

DllEXPORT Error WINAPI VDR_InstallMPEGWinHooks (UnitSet units, HWND hwnd, WORD refillMsg, WORD signalMsg, WORD doneMsg)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_InstallMPEGWinHooks+"));
#endif

	if (TU(units) && TU(units)->hasMPEGDecoder)
		{
		TU(units)->refillHook = new MPEGUnitsRefillHook(hwnd, refillMsg);
		TU(units)->signalHook = new MPEGUnitsSignalHook(hwnd, signalMsg);
		TU(units)->doneHook = new MPEGUnitsDoneHook(hwnd, doneMsg);

		INIT_IOCTL(INSTALL_MPEG_WIN_HOOKS);

		in.units = TU(units)->kunits;
		in.signalHook	= TU(units)->signalHook;
		in.doneHook		= TU(units)->doneHook;
		in.refillHook	= TU(units)->refillHook;

		DO_IOCTL(INSTALL_MPEG_WIN_HOOKS);

		GNRAISE(out.error);
		}

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_InstallMPEGWinHooks-"));
#endif

	GNRAISE_OK;
	}

DllEXPORT Error WINAPI VDR_RemoveMPEGWinHooks (UnitSet units)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_RemoveMPEGWinHooks+"));
#endif

	if (TU(units) && TU(units)->hasMPEGDecoder)
		{
		INIT_IOCTL(INSTALL_MPEG_WIN_HOOKS);

		in.units = TU(units)->kunits;
		in.signalHook	= NULL;
		in.doneHook		= NULL;
		in.refillHook	= NULL;

		DO_IOCTL(INSTALL_MPEG_WIN_HOOKS);

		delete TU(units)->refillHook;
		delete TU(units)->signalHook;
		delete TU(units)->doneHook;

		TU(units)->refillHook = NULL;
		TU(units)->signalHook = NULL;
		TU(units)->doneHook = NULL;
		}

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_RemoveMPEGWinHooks-"));
#endif

	GNRAISE_OK;
	}

DllEXPORT Error WINAPI VDR_CompleteMPEGRefillMessage (UnitSet units)
	{
#if TRACE_VDR_CALLS
//DP(__TEXT("VDR_CompleteMPEGRefillMessage+"));
#endif

	if (TU(units) && TU(units)->hasMPEGDecoder && TU(units)->refillHook)
		TU(units)->refillHook->pending = FALSE;

#if TRACE_VDR_CALLS_E
//DP(__TEXT("VDR_CompleteMPEGRefillMessage-"));
#endif

	GNRAISE_OK;
	}

DllEXPORT Error WINAPI VDR_DoAuthenticationCommand (UnitSet units, MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE * key)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_DoAuthenticationCommand+"));
#endif

	INIT_IOCTL(DO_AUTHENTICATION_COMMAND);

	in.units = TU(units)->kunits;
	in.com = com;
	in.sector = sector;
	in.key = key;

	DO_IOCTL(DO_AUTHENTICATION_COMMAND);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_DoAuthenticationCommand-"));
#endif

	GNRAISE(out.error);
	}

DllEXPORT BOOL WINAPI VDR_CheckRegionCodeValid (UnitSet units, BYTE regionSet)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_CheckRegionCodeValid+"));
#endif

	INIT_IOCTL(CHECK_REGIONCODE_VALID);

	in.units = TU(units)->kunits;
	in.regionSet = regionSet;

	DO_IOCTL(CHECK_REGIONCODE_VALID);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_CheckRegionCodeValid-"));
#endif

	return out.status;
	}



DllEXPORT Error WINAPI VDR_SendAudioNotification(UnitSet units, DWORD senderID, DWORD type, DWORD msg, DWORD data)
	{
#if TRACE_VDR_CALLS
DP(__TEXT("VDR_SendAudioNotification+"));
#endif

	INIT_IOCTL(SEND_AUDIO_NOTIFICATION)

	in.units = TU(units)->kunits;
	in.senderID = senderID;
	in.msg = msg;
	in.type = type;
	in.data = data;

	DO_IOCTL(SEND_AUDIO_NOTIFICATION);

#if TRACE_VDR_CALLS_E
DP(__TEXT("VDR_SendAudioNotification-"));
#endif

	GNRAISE(out.error);
	}


///////////////////////////////////////////////////////////////////////////////
// Driver/DLL Initialization
///////////////////////////////////////////////////////////////////////////////


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
	{
	HKEY key;

	switch(fdwReason)
		{
		case DLL_PROCESS_ATTACH:
			DP(__TEXT("Process Attach+"));
			hinst = hinstDLL;

			if (ERROR_SUCCESS == RegCreateKey(HKEY_LOCAL_MACHINE,
				                               EVENT_LOG_REG_LOCATION,
														 &key))
				{
				const TCHAR buffer[] = EVENT_LOG_REG_DRIVER;
				const DWORD seven = 7;

				RegSetValueEx(key, __TEXT("EventMessageFile"), 0, REG_SZ,
					           (BYTE *)buffer, sizeof(buffer));
				RegSetValueEx(key, __TEXT("TypesSupported"), 0, REG_DWORD,
					           (BYTE *)&seven, 4);
				RegCloseKey(key);
				}

			kDriver = CreateFile(KERNEL_DRIVER_SYMBOLIC_NAME,
										GENERIC_READ | GENERIC_WRITE,
										FILE_SHARE_READ,
										NULL,
										OPEN_EXISTING,
										FILE_ATTRIBUTE_NORMAL,
										NULL);

			if (kDriver == INVALID_HANDLE_VALUE || kDriver == NULL)
				{
				DWORD error = GetLastError();
				return FALSE;
				}

			clipMutex = CreateMutex(NULL, FALSE, __TEXT("VDRPIPMUTEX"));
//			dummyWindow = CreateWindow(__TEXT("STATIC"), __TEXT("Fake move"), WS_VISIBLE, 0, 0, 10, 10, NULL, NULL, hinstDLL, NULL);

#if USE_HOOK_PROCESS
			SECURITY_ATTRIBUTES sa;
			PSECURITY_DESCRIPTOR psd;
			BOOL success;
			success = FALSE;
			psd = (PSECURITY_DESCRIPTOR) LocalAlloc (LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
			if (psd == NULL)
				{
				DP(__TEXT("alloc psd failed"));
				}
			else
				{
				if (! InitializeSecurityDescriptor (psd, SECURITY_DESCRIPTOR_REVISION))
					{
					DP(__TEXT("InitializeSecurityDescriptor failed"));
					}
				else
					{
					// An empty ACL means full access.
					if (! SetSecurityDescriptorDacl (psd, TRUE, (PACL)NULL, FALSE))
						{
						DP(__TEXT("SetSecurityDescriptorDacl failed"));
						}
					else
						{
						sa.nLength = sizeof (SECURITY_ATTRIBUTES);
						sa.lpSecurityDescriptor = psd;
						sa.bInheritHandle = TRUE;
						ServiceLock		= CreateEvent(&sa, FALSE, TRUE, __TEXT("VDCMSERV.SERVICELOCK"));
						ServiceRequest	= CreateEvent(&sa, FALSE, FALSE, __TEXT("VDCMSERV.SERVICEREQUEST"));
						ServiceDone		= CreateEvent(&sa, FALSE, FALSE, __TEXT("VDCMSERV.SERVICEDONE"));
						if (! ServiceLock)
							{
							DP(__TEXT("No ServiceLock created!"));
							}
						}
					}
				if (psd)
					LocalFree (psd);
				}
#endif

			DP(__TEXT("Process Attach-"));
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			DP(__TEXT("Process Detach+"));
			if (WorkerThread) FiniThreadContext();
			DP(__TEXT("Closing process handle"));
			CloseHandle(kDriver);
			if (clipMutex) CloseHandle(clipMutex);

#if USE_HOOK_PROCESS
			if (ServiceLock)		CloseHandle(ServiceLock);
			if (ServiceRequest)	CloseHandle(ServiceRequest);
			if (ServiceDone)		CloseHandle(ServiceDone);
#endif

			DP(__TEXT("Process Detach-"));
			break;
		}

   return TRUE;
	}
