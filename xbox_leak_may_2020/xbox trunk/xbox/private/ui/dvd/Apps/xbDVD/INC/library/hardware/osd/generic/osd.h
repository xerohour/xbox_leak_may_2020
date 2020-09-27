#ifndef OSD_H
#define OSD_H

#include "library\common\prelude.h"
#include "library\common\virtunit.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"

MKTAG(OSD_ENABLE,						OSD_UNIT,	0x00000001, 	BOOL)
MKTAG(OSD_VISIBLE,					OSD_UNIT,	0x00000002,		BOOL)

MKTAG(OSD_LEFT,						OSD_UNIT,	0x00000003,		WORD)
MKTAG(OSD_TOP,							OSD_UNIT,	0x00000004,		WORD)
MKTAG(OSD_WIDTH,						OSD_UNIT,	0x00000005,		WORD)
MKTAG(OSD_HEIGHT,						OSD_UNIT,	0x00000006,		WORD)

MKTAG(OSD_PALETTE_ENTRY,			OSD_UNIT,	0x00000007,		DWORD)
	// Palette Bits 0..7 Pen, 8..15 V, 16..23 U, 24..31 Y
	
MKTAG(OSD_DISPLAY_WIDTH,			OSD_UNIT, 0x00000008,		WORD)
MKTAG(OSD_DISPLAY_HEIGHT,			OSD_UNIT,	0x00000009,		WORD)

enum OSDCommand
	{
	OSDC_NONE,
	OSDC_FILL,
	OSDC_COPY,
	OSDC_XOR,
	OSDC_EXPAND,
	OSDC_UPDATE
	};

class OnScreenDisplay : public PhysicalUnit
	{
	friend class VirtualOnScreenDisplay;
	protected:
		virtual Error EnableOSD(BOOL enable) = 0;
		virtual Error ShowOSD(BOOL show) = 0;
		virtual Error SetPaletteEntry(int entry, WORD y, WORD u, WORD v) = 0;
		virtual Error UpdateRegion(WORD left, WORD top, WORD width, WORD height, FPTR data, WORD stride) = 0;
		virtual Error ClearDisplay(void) = 0;
		
		virtual WORD DisplayWidth(void) = 0;
		virtual WORD DisplayHeight(void) = 0;
	};

class VirtualOnScreenDisplay : public VirtualUnit
	{
	protected:
		OnScreenDisplay	*	device;
		
		WORD	left, top, width, height;
		WORD	stride;
		BYTE	__far *	map;
		BOOL	enable, visible;
		DWORD	palette[16];

      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);		
	public:
		VirtualOnScreenDisplay(OnScreenDisplay * device);
		~VirtualOnScreenDisplay(void);
		
		Error ExecuteCommand(OSDCommand com, WORD color, WORD x, WORD y, WORD w, WORD h, FPTR data, WORD dstride);
		
		Error Configure(TAG __far * tags);
	};

#endif
