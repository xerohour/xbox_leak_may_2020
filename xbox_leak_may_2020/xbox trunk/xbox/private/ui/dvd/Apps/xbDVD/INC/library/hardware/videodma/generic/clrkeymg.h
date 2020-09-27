// FILE:      library\hardware\videodma\generic\clrkeymg.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1997 Viona Development GmbH. All Rights Reserved.
// CREATED:   12.08.97
//
// PURPOSE:   Direct Draw Color Key Management Class
//
// HISTORY:

#ifndef CLRKEYMG_H
#define CLRKEYMG_H

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "ddraw.h"
#include "library\support\VDSup32\VDSup32.h"

class DDColorkeyManager
	{
	protected:
		DWORD	keyIndex;
		DWORD	keyColor;
		DWORD	keyColorRGB;
		DWORD	keyColorIndex;

		BOOL	GetDDInfo(void);
		DWORD ConvertKeyIndex(DWORD __far & colorIndex);
		DWORD ConvertKeyColor(DWORD colorRGB);
	public:
		DDSURFACEDESC	ddSfcDesc;
		
		BOOL	ddAvailable;
		
		DDColorkeyManager(void);

		BOOL SetDDColorkeyColor(DWORD color);
		BOOL SetDDColorkeyIndex(DWORD index);
		
		Error DrawColorkey(HWND win);
		Error DrawBlack(HWND win);
	};

#endif // ONLY_EXTERNAL_VISIBLE

#endif
	