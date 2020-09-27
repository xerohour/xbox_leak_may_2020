//
// FILE:      library\hardware\videodma\generic\gfxconfg.h
// AUTHOR:    U. Sigmund, S. Herr
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   22.03.95
//
// PURPOSE:   Configuration class for graphic board format etc.
//
// HISTORY:
//
// Date			Author	Comment
// --------		------	-------
//	28.07.95		SH			addition of QueryGraphicConfig()

#ifndef GFXCONFG_H
#define GFXCONFG_H

#include <windows.h> 
#include <windowsx.h>
#include <mmsystem.h>
#include <msvideo.h>
#include <msviddrv.h>
#include <mmreg.h>
#include "dciddi.h"	// Use C++ corrected version!
#include <dciman.h>

#include "..\..\..\common\prelude.h"
#include "..\..\..\common\gnerrors.h"
#include "..\..\..\lowlevel\pci.h"
#include "..\..\video\generic\vidtypes.h"
#include "..\..\..\common\profiles.h"

#if VIONA_VERSION
#include "..\..\..\support\VDSup32\vdsup32.h"
#endif

#define TESTAREASIZE 0x800000		// 8 MB test area size!


enum OffsetFormat {OF_INVALID,
						 OF_DCI,
						 OF_PCI,
						 OF_VESA,
						 OF_DIRECTDRAW };

class GraphicConfig {
	protected:
		Profile		 * profile;				// Object reference for INI file configuration.
		
		struct DMADescriptor {					// Needed for access to virtual DMA services.
			DWORD			regionSize;
			DWORD			offset;
			WORD			selector;
			WORD			bufferID;
			DWORD			physical;
			} dds;
			
		HINSTANCE		instanceHandle;		// Instance handle of using DLL or application
		HWND				parentWindow;			// Handle of parent window for dialog boxes.
		
		PCIBus* 			mainBus;					// PCI bus object.
                                   	
		PCIDevice*		gfxBoard;				// Graphics board for which we examine the properties.

		DWORD				baseAddress;			// Necessary to store the physical base address for dialogs.
		DWORD				presetBaseAddress;
		DWORD				startOffset;			// Stores offset from the PCI or other physical start address
														// of frame buffer. May contain 0.
		OffsetFormat	offsetFormat;

		HBRUSH 			hbrGrey;

#if VIONA_VERSION
		DDSURFACEDESC	sfcDescr;
		BOOL				ddSuccess;
#endif		
		char				messageTitle[32];

		BOOL				retest;					// Flag for "Run compatibility test again".

		Error FindStartLocation(BOOL offsetAvailable);		// Tries to find the address of the framebuffer's first byte
		Error DetermineGfxBoard(List* gfxBoardList, PCIDevice* device);	// Determines, which board to take if there
																								// are more than one in the computer.
		Error DetermineOffsetViaGDI(DWORD physicalBase, DWORD __far & offset);	// Uses direct access to frame buffer to find
																								// offset to physical start address.
		BOOL MaskedPatternSearch(WORD _huge* destination,						// Pattern search routine with masked pattern
										  WORD * pattern,									// values.
										  WORD * mask,
										  DWORD startPos, DWORD bufferSize,
										  DWORD __far & endposition);
		Error FindStride(WORD _huge* destination,									// Used to determine stride by searching the
							 WORD * pattern,												// pattern in the next scan line after the
							 WORD * mask,													// pattern's first occurrence.
							 WORD bitsPerPixel,
							 DWORD position, WORD __far & stride);
      Error MakeWindow(POINT __far & point, HWND __far & window);							// Create test window for pattern search.
      Error ThrowAwayWindow(HWND __far & window);										// Delete test window.
		void  ProcessMessages();														// Processes test window messages.
		Error CheckAddressValidity(DWORD address,
											WORD* pattern,
											WORD* mask, BOOL& valid);
		BOOL 	DrawTest(void);
      Error	DrawWhiteAndBlackBoxes(void);											// Draws directly into the frame buffer mem.

		BOOL	RetrieveAndCompareProperties(void);									// Reads current properties from INI file and
																								// checks if they are still valid
		void	StoreProperties(void);

	public:
//		enum PixelFormat {RGB_888, RGB_888x, RGB_555, RGB_565, OTHER};
		
		BOOL				overlayPossible;		// Indicates, if overlay possible in current video mode.
		BOOL				flatModelPossible;	// Tells, if video mode/graphics card allows flat model.

      HDC					dciProvider;		// Device context of DCI Provider
		LPDCISURFACEINFO	surfaceInfo;		// Structure with information about primary display surface

		BOOL				buffWriteOnly;			// Indicates if buffer may be read or not

		WORD				xSize;					// Number of pixels per scan line
		WORD				ySize;					// Number of lines of display
		
		PIPFormat		pixelFormat;			// Arrangement of pixels in frame buffer memory
		WORD				bitsPerPixel;
		WORD				bytesPerPixel;
		WORD				bytesPerRow;

		DWORD				startLocation;			// Physical 32 bit address of frame buffer start. Contains 0 if
														// DMA access is not possible.
		DWORD				buffSize;				// Size of memory for frame buffer (in bytes)

		WORD				deviceID;				// Device ID of graphics board
		WORD				vendorID;				// Vendor ID of graphics board

      GraphicConfig(HINSTANCE instance, Profile* profile);	// Needs DLL or program instance handle
      ~GraphicConfig(void);
      
		Error	QueryGraphicConfig(void);		// Get metrics and physical address of current windows screen.
      Error Check15Or16(WORD __far & bistPerPixel);	// Determine true bit depth for 15/16 bit.
	
		int	ProcessDlgMessage(HWND hwnd, WPARAM message, WPARAM wParam, LONG lParam);
	};

#define GNR_NO_PCI_GFX_BOARD		MKERR(ERROR, GFC, GENERAL,	0x01)
#define GNR_PATTERN_NOT_FOUND		MKERR(ERROR, GFC, GENERAL, 0x02)
#define GNR_NO_TESTWINDOW			MKERR(ERROR, GFC, GENERAL, 0x03)
#define GNR_STRIDE_NOT_FOUND		MKERR(ERROR, GFC, GENERAL, 0x04)
#define GNR_MODE_NOT_SUITABLE		MKERR(ERROR, GFC, GENERAL, 0x05)

#endif
