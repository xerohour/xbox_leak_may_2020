/*************************************************************************

Copyright (c) 1998  Microsoft Corporation

Module Name:

	dectable_mp4x.hpp

Abstract:

	MP4S - MP43 Decoder Huffmane tables 

Author:

	Bruce Lin (blin@microsoft.com) June-1998

Revision History:

*************************************************************************/
#include "xplatform.h"
#include "typedef.hpp"
extern U8_WMV gDecodeCodeTableCBPY[128];
extern U8_WMV gDecodeCodeTableMCBPCintra_New [16];
extern U8_WMV gDecodeCodeTableMCBPCinter_New [256];

extern U8_WMV gDecodeCodeTableMCBPCintra[1024];
extern U8_WMV gDecodeCodeTableMCBPCinter[1024];

extern U8_WMV gDecodeCodeTableIntraDCy[256];
extern U8_WMV gDecodeCodeTableIntraDCc[512];

extern U8_WMV gDecodeCodeTableMV[16384];

extern U8_WMV gDecodeCodeTableIntraDCMPEG4y[4096];
extern U8_WMV gDecodeCodeTableIntraDCMPEG4c[8192];
