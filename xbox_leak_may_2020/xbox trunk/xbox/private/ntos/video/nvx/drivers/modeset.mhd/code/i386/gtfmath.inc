
option expr32
option casemap:none

; Begin of file temp.h
ULONG		TYPEDEF		DWORD

tagGTFIN		STRUCT 2t
dwHPixels		ULONG		?
dwVLines		ULONG		?
dwMarginsRqd		ULONG		?
dwIntRqd		ULONG		?
dwIPParmType		ULONG		?
dwIPFreqRqd		ULONG		?
tagGTFIN		ENDS

GTFIN		TYPEDEF		tagGTFIN

LPGTFIN		TYPEDEF		PTR GTFIN

IPTYPE_VERTICAL_REFRESH_RATE		EQU		1t
IPTYPE_HORIZONTAL_REFRESH_RATE		EQU		2t
IPTYPE_PIXEL_CLOCK_RATE		EQU		3t
tagGTFOUT		STRUCT 2t
dwHTotalChars		ULONG		?
dwVTotalScans		ULONG		?
dwHActiveChars		ULONG		?
dwVActiveScans		ULONG		?
dwHBlankStartChar		ULONG		?
dwHFrontPorchChars		ULONG		?
dwHSyncChars		ULONG		?
dwHBackPorchChars		ULONG		?
dwHBlankEndChar		ULONG		?
dwVBlankStartScan		ULONG		?
dwVFrontPorchScans		ULONG		?
dwVSyncScans		ULONG		?
dwVBackPorchScans		ULONG		?
dwVBlankEndScan		ULONG		?
dwPixelClockIn10KHertz		ULONG		?
tagGTFOUT		ENDS

GTFOUT		TYPEDEF		tagGTFOUT

LPGTFOUT		TYPEDEF		PTR GTFOUT

; End of file temp.h
