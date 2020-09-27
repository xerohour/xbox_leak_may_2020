// FILE:			library\hardware\panel\generic\panels.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1999 VIONA Development GmbH & Co. KG.  All Rights Reserved.
// CREATED:		02.08.1999
//
// PURPOSE:		Generic User Interface Panel Class
//
// HISTORY:

#ifndef PANELS_H
#define PANELS_H

#include "library\common\virtunit.h"
#include "library\common\tagunits.h"
#include "library\common\hooks.h"
#include "library\common\krnlsync.h"

// Panel Operations data type

#define MKPOPTYPE(name, id) \
	static const DWORD POPTYPEID_##name = id & 0xffff; 

struct __far POP {
	DWORD	id;
	DWORD	data;
	POP (DWORD _id, DWORD _data) { data = _data; id = _id; };
	POP () {}
#if !__EDG__  || __EDG_VERSION__ < 240
	~POP () {}  // Mainly workaround for a memory leak bug in the C++ frontend compiler (1.6.2 toolset).
#endif
	};

typedef POP * POPList;

#define POPDONE POP(0,0)

#define POP_SET	0x1000
#define POP_GET	0x2000
#define POP_QRY	0x4000

#define MKPOP(name, id, type)	\
	inline POP SET_POP_##name(type x) {return POP((id << 16) | POPTYPEID_##type | POP_SET, (DWORD)(x));}	\
	inline POP GET_POP_##name(type __far &x) {return POP((id << 16) | POPTYPEID_##type | POP_GET, (DWORD)(&x));}	\
	inline POP QRY_POP_##name(BOOL __far &x) {return POP((id << 16) | POPTYPEID_##type | POP_QRY, (DWORD)(&x));}	\
	inline POP INI_POP_##name(type x) {return POP((id << 16) | POPTYPEID_##type, (DWORD)(x));} \
	inline type VAL_POP_##name(POP pop) {return (type)(pop.data);}	\
	static const DWORD ID_POP_##name = id;

#define EXECUTE_POP_START \
	switch (pop.id >> 16) {

#define EXECUTE_POP_END }


// Panel specific data types

enum PanelSegment
	{
	PSG_NONE,
	PSG_SET,
	PSG_CLR,
	PSG_STEP
	};

enum PanelNumType
	{
	PNUM_NONE,
	PNUM_DEC,
	PNUM_DEC_SIMPLE_LEFT,
	PNUM_HEX,
	PNUM_HEX_SIMPLE_LEFT,
	PNUM_BIN
	};

enum PanelNumericInfo
	{
	PNI_NONE					= 0,
	PNI_TIME					= 1,
	PNI_ANGLE				= 2,
	PNI_TITLE				= 4,
	PNI_CHAPTER				= 8,
	PNI_NUM_ANGLES			= 16,
	PNI_NUM_TITLES			= 32,
	PNI_NUM_CHAPTERS		= 64,
	PNI_PBC_ENTRY			= 128,
	PNI_PBC_NUM_ENTRIES	= 256
	};

enum PanelDiskType
	{
	PDT_NONE,
	PDT_VIDEO_DVD,
	PDT_AUDIO_DVD,
	PDT_VIDEO_CD,
	PDT_AUDIO_CD,
	PDT_SUPER_VIDEO_CD,
	PDT_SUPER_AUDIO_CD
	};

enum PanelAudioType
	{
	PAT_NONE,
	PAT_AC3,
	PAT_MPEG1,
	PAT_MPEG2,
	PAT_LPCM,
	PAT_DTS,
	PAT_SDDS,
	PAT_PROLOGIC,
	PAT_SRS
	};

enum PanelState
	{
	PST_STOPPED,
	PST_PAUSED,
	PST_STILL,
	PST_PLAYING,
	PST_BUSY,
	PST_INIT,
	PST_SCANNING_FORWARD_FAST,
	PST_SCANNING_FORWARD_SMOOTH,
	PST_SCANNING_BACKWARD_FAST,
	PST_SCANNING_BACKWARD_SMOOTH,
	PST_INACTIVE,
	PST_REVERSEPLAY,
	PST_TRICKPLAY,
	PST_REVERSESCAN,
	PST_STOPPED_FOR_RESUME
	};

enum PanelTimeMode
	{
	PTM_INACTIVE,
	PTM_TOTAL,
	PTM_TOTAL_ELAPSED,
	PTM_TOTAL_REMAIN,
	PTM_SINGLE_ELAPSED,
	PTM_SINGLE_REMAIN,
	PTM_SINGLE
	};

enum PanelTrayStatus
	{
	PTS_UNKNOWN,
	PTS_INSERTED,
	PTS_REMOVED,
	PTS_INSERTING,
	PTS_REMOVING
	};

// General type defs

typedef char * STRING8;		// String with max. size of 8 characters
typedef DWORD	SEGMENT4;	// 4 segments, each one byte of the DWORD. MSB is first segment from the left.
typedef DWORD	TIME;			// Time representation
typedef DWORD	DUMMY; 		// Dummy type if not parameter is used

// Panel Operation data types
MKPOPTYPE(int,					0x01)
MKPOPTYPE(char,	 			0x02)
MKPOPTYPE(BYTE,				0x03)
MKPOPTYPE(WORD,				0x04)
MKPOPTYPE(DWORD,				0x05)
MKPOPTYPE(BOOL,				0x06)
MKPOPTYPE(DUMMY,				0x07)
MKPOPTYPE(TIME,				0x08)
MKPOPTYPE(STRING8,			0x09)
MKPOPTYPE(SEGMENT4,			0x0a)
MKPOPTYPE(PanelSegment,		0x0b)
MKPOPTYPE(PanelNumType,		0x0c)
MKPOPTYPE(PanelNumericInfo,0x0d)
MKPOPTYPE(PanelDiskType, 	0x0e)
MKPOPTYPE(PanelAudioType,	0x0f)
MKPOPTYPE(PanelState,		0x10)
MKPOPTYPE(PanelTimeMode,	0x11)
MKPOPTYPE(PanelTrayStatus, 0x12)

// Definition of Panel Operations
MKPOP(TEST_SEGMENTS,			0x0,		PanelSegment)
MKPOP(TEST_NUMBER_TYPE,		0x1,		PanelNumType)
MKPOP(TEST_NUMBER,			0x2,		int)
MKPOP(TEST_RESULT,			0x3,		BOOL)

MKPOP(RESET,		 			0x010,	DUMMY)

MKPOP(POWER,					0x020,	BOOL)

MKPOP(BRIGHTNESS,				0x030,	DWORD)

MKPOP(ENABLE_PIE, 	 		0x040,	BOOL)

MKPOP(ENABLE,					0x048,	PanelNumericInfo)
MKPOP(DISABLE,					0x049,	PanelNumericInfo)
MKPOP(EDIT_ENTER,				0x04a,	PanelNumericInfo)
MKPOP(EDIT_LEAVE,	 			0x04b,	PanelNumericInfo)
MKPOP(EDIT_WRITE_DIGIT,		0x04c,	int)
MKPOP(EDIT_WRITE_CHAR,		0x04d,	char)
MKPOP(EDIT_WRITE_STRING, 	0x04e,	STRING8)

MKPOP(DISK_TYPE,	 			0x050,	PanelDiskType)
MKPOP(REGION,					0x051,	DWORD)

MKPOP(AUDIO_TYPE,	 			0x060,	PanelAudioType)

MKPOP(PANEL_STATE,			0x070,	PanelState)

MKPOP(TRAY,						0x080,	PanelTrayStatus)

MKPOP(TIME_MODE,		  	  	0x090,	PanelTimeMode)
MKPOP(TIME,				  	  	0x092,	DWORD)

MKPOP(TITLE,					0x100,	int)
MKPOP(NUM_TITLES,				0x101,	int)
MKPOP(CHAPTER,					0x102,	int)
MKPOP(NUM_CHAPTERS,	 		0x103,	int)
MKPOP(ANGLE,					0x104,	int)
MKPOP(NUM_ANGLES,				0x105,	int)
MKPOP(PBC_ENTRY,				0x106,	int)
MKPOP(PBC_NUM_ENTRIES, 		0x107,	int)

MKPOP(AUDIO_CHANNELS,  		0x110,	int)

MKPOP(SPEED,			  		0x121,	int)

MKPOP(LOCKED, 					0x130,	BOOL)
MKPOP(ZOOM, 					0x131,	int)
MKPOP(LAST, 					0x132,	int)
MKPOP(PLAYBACK_CONTROL,		0x133,	BOOL)

MKPOP(REPEAT, 					0x140,	BOOL)
MKPOP(REPEAT_START,			0x141,	BOOL)
MKPOP(REPEAT_END,				0x142,	BOOL)

MKPOP(ADVANCE_FRAME_BY,		0x150,	BOOL)

//....



// Definition of keyboard input hook

struct PanelKeyboardMsg { 
	DWORD		ceMsg;
	};

MKHOOK(PanelKeyboard,   PanelKeyboardMsg)

typedef PanelKeyboardHook	__far *	PanelKeyboardHookPtr;


// Tags for Panel Classes

MKTAG(PANEL_KEYBOARD_HOOK, PANEL_UNIT, 0x0001, PanelKeyboardHookPtr)

//...



#ifndef ONLY_EXTERNAL_VISIBLE

class Panel : public PhysicalUnit {
	friend class VirtualPanel;
	private:
		POPList CreatePOPListFromPtr(int __far & size, POP __far * pops);
	protected:
		Error SignalKeyHook(DWORD ceMsg);
		POPList CreatePOPList(int __far & size, POP pop, ...);
	public:
		virtual POPList GetInitPOPList(int __far & size) = 0;
		
		virtual Error StartQueueOperations(void) = 0;
		virtual Error QueueOperation(POP * pop) = 0;
		virtual Error EndQueueOperations(DWORD __far &tag) = 0;

		virtual Error FlushOperations(void) = 0;

		virtual Error CompleteOperations(DWORD tag) = 0;
		virtual BOOL  OperationsPending(DWORD tag) = 0;
	};


class VirtualPanel : public VirtualUnit {
	friend class Panel;
	private:
		Panel			*	panel;
	protected:
		POPList			popList;
		int				plSize;		// Number of elements in list for faster search

		PanelKeyboardHookHandle	keyHook;

		Error Preempt(VirtualUnit * previous);
	public:
		VirtualPanel(Panel * panel);
		virtual ~VirtualPanel(void);

		Error Configure(TAG __far * tags);

		virtual Error DoOperations(POP __far * pops);
		Error __cdecl DoOperationsOps(POP pop, ...);
		virtual Error SendOperations(DWORD __far &tag, POP __far * pops);
		Error __cdecl SendOperationsOps(DWORD __far &tag, POP pop, ...);
		virtual Error CompleteOperations(DWORD tag);
		virtual BOOL  OperationsPending(DWORD tag);
	};


class PollingPanel : public Panel {
	private:
		BOOL	endTask;
		DWORD	bufferSize;
		DWORD	pollInterval;
		DWORD	lastOp;
		DWORD	curOp;
		DWORD preCeMsg, failedCeMsg;
		POPList * opBuff;
		friend void PollingPanel_HandlerTask(void *panel);
		virtual void PanelTaskProcedure(void);
		
		VDTimedSemaphore	taskSema;
		VDSemaphore			completeSema;

		task_t *	panelTask;	// This ST specific stuff could be avoided by creating a 
									// KernelTask class!
		clock_t nextTime;
	protected:
		virtual void ExecuteOperation(POP pop) = 0;
		virtual BOOL PollCEMessage(DWORD & ceMsg) {return FALSE;}
	public:
		PollingPanel(DWORD bufferSize, DWORD pollInterval, int taskPriority, DWORD stackSize = 4*1000);	// pollInterval in us
		virtual ~PollingPanel(void);

		Error StartQueueOperations(void);
		Error QueueOperation(POP * pop);
		Error EndQueueOperations(DWORD __far &tag);

		Error FlushOperations(void);

		Error CompleteOperations(DWORD tag);
		BOOL  OperationsPending(DWORD tag);
	};


#endif

#endif

