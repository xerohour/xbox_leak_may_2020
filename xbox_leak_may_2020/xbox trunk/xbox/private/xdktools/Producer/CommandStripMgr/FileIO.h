#ifndef __COMMAND_FILEIO_H__
#define __COMMAND_FILEIO_H__



typedef struct CommandExt
{
	CommandExt() { memset( this, 0, sizeof(this) ); }
    CommandExt* pNext;
    long        lTime;		// Time, in clocks
	DWORD		dwDragDrop;
    BYTE        bCommand;	// Command type
    BYTE        bGrooveLevel;	// Command's groove level (1 - 100)
    BYTE        bGrooveRange;   // Groove range
    BYTE        bRepeatMode;    // Used to control selection of patterns with same groove level
} CommandExt;

typedef struct CommandMeasureInfo
{
	CommandExt*	pCommand;	// The command in this measure.
	DWORD		dwFlags;	// Currently, just whether the measure is selected.
} CommandMeasureInfo;

typedef struct ioCommand
{
    long    lTime;       // Time, in clocks.
    DWORD   dwCommand;    // Command type.
} ioCommand;

typedef struct PPGCommand
{
	long		lMeasure;
	DWORD		dwValid;
    BYTE        bCommand;	// Command type
    BYTE        bGrooveLevel;	// Command's groove level (1 - 100)
    BYTE        bGrooveRange;   // Groove range
    BYTE        bRepeatMode;    // Used to control selection of patterns with same groove level
} PPGCommand;

/* structure of dwDragDrop

	Bit	 0:			Drag select flag (command is part of drag select)
	Bit  1:			Drop cursor flag (command is drop cursor)
	Bits 2  - 11:	Not Used
	Bits 12 - 27	measure for drag/drop
	Bits 28 - 31	Not Used

*/

#define C_FILL      0x1		// Do a fill
#define C_INTRO     0x2		// Do an intro
#define C_BREAK     0x20	// Do a drum break
#define C_END       0x40	// End the song
#define C_MASK		0x7F
#define GROOVE_A	0x80
#define GROOVE_B	0x100
#define GROOVE_C	0x200
#define GROOVE_D	0x400
#define GROOVE_MASK	0x780

#define DRAG_SELECT	0x1	//	this is part of a dragged command list
#define DROP_CURSOR	0x2	//  this command is at the drop cursor point

#define MEASURE_MASK	0x0FFFF000	// position of measure info in dwCommand for persisted commands

#define VALID_COMMAND	0x1
#define VALID_GROOVE	0x2
#define VALID_GROOVERANGE	0x4
#define VALID_REPEATMODE	0x8

// Custom embellishment values
#define MIN_EMB_CUSTOM_ID	100
#define MAX_EMB_CUSTOM_ID	199

#define CMI_SELECTED	0x1	// The measure is selected.


long __inline GetMeasureFromCommand(DWORD dwDragDrop)
{
	long m = long(dwDragDrop) & MEASURE_MASK;
	m = (m >> 12) & 0xFFFF;
	return m;
}

void __inline SetMeasureInCommand(DWORD& dwDragDrop, long lMeasure)
{
	lMeasure = (lMeasure << 12) & MEASURE_MASK;
	dwDragDrop &= ~MEASURE_MASK;  // clear any previous value
	dwDragDrop |= lMeasure;
}

/*
class CCommandList
{
	CommandExt*	pHead;
public:
    CCommandItem *   GetHead() {return pHead};
	CCommandItem *	RemoveHead() { CommandExt* p = pHead; pHead = p->Next; }
	void		ClearList();
	void		ClearFlags(DWORD flag);
	void		InsertByAscendingTime(CCommandItem *pChord);	// *pCommand <= *(pCommand->next)
	HRESULT		Load( LPSTREAM pStream );
	HRESULT		Save( LPSTREAM pStream, BOOL bClearDirty );
};
*/

/*
// seeks to a 32-bit position in a stream.
HRESULT __inline StreamSeek( LPSTREAM pStream, long lSeekTo, DWORD dwOrigin )
{
	LARGE_INTEGER li;

	if( lSeekTo < 0 )
	{
		li.HighPart = -1;
	}
	else
	{
        li.HighPart = 0;
	}
	li.LowPart = lSeekTo;
	return pStream->Seek( li, dwOrigin, NULL );
}
*/




#endif