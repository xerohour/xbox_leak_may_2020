// FileStructs.h

#ifndef __FILESTRUCTS_H__
#define __FILESTRUCTS_H__

#include <afxtempl.h>

#define MakeID(a,b,c,d)  ( (LONG)(a)<<24L | (LONG)(b)<<16L | (c)<<8 | (d) )

#define ID_CHORDEXT		MakeID('s','C','E','X')
#define ID_STYLEREF		MakeID('s','S','R','F')
#define ID_PERSREF		MakeID('s','P','R','F')
#define ID_CHORDENTRY   MakeID('s','C','E','N')
#define ID_NEXTLIST     MakeID('s','N','X','L')
#define ID_PERSONALITY  MakeID('s','P','E','R')
#define ID_PERSONREF    MakeID('s','P','R','F')
#define ID_NEWSTYLE     MakeID('s','S','T','Y')
#define ID_CHORDPALETTE MakeID('s','C','P','L')
#define ID_TEMPLATE     MakeID('s','T','P','L')
#define ID_TEMPLATELIST MakeID('s','T','L','S')
#define ID_SIGNPOST     MakeID('s','S','N','P')
#define ID_INSTRUMENTS  MakeID('s','I','N','S')
#define ID_COMMAND      MakeID('s','C','M','D')
#define ID_GROOVENAME   MakeID('s','G','R','N')

// Large versions for editing:
#define ID_EPERSONALITY MakeID('s','C','M','P')
#define ID_ECHORDENTRY  MakeID('e','C','E','N')
#define ID_ENEXTLIST    MakeID('e','N','X','L')
#define ID_ESTYLE       MakeID('e','S','T','Y')
#define ID_EPERSONREF   MakeID('e','P','R','F')

#define ID_CHRD        0x43485244
#define ID_KBRD        0x4B425244

// Commands

#define PF_FILL 	0x0001		// Fill pattern
#define PF_START	0x0002		// May be starting pattern
#define PF_INTRO	0x0002
#define PF_WHOLE	0x0004		// Handles chords on measures
#define PF_HALF 	0x0008		// Chords every two beats
#define PF_QUARTER	0x0010		// Chords on beats
#define PF_BREAK	0x0020
#define PF_END		0x0040
#define PF_A		0x0080
#define PF_B		0x0100
#define PF_C		0x0200
#define PF_D		0x0400
#define PF_E		0x0800
#define PF_F		0x1000
#define PF_G		0x2000
#define PF_H		0x10000
#define PF_STOPNOW	0x4000
#define PF_INRIFF	0x8000
#define PF_BEATS    (PF_WHOLE | PF_HALF | PF_QUARTER)
#define PF_RIFF     (PF_INTRO | PF_BREAK | PF_FILL | PF_END)
#define PF_GROOVE   (PF_A | PF_B | PF_C | PF_D | PF_E | PF_F | PF_G | PF_H)

// Keys
#define KEY_1C		0x0000
#define KEY_1CS		0x0001
#define KEY_1Db		0x8001
#define KEY_1D		0x0002
#define KEY_1DS		0x0003
#define KEY_1Eb		0x8003
#define KEY_1E		0x0004
#define KEY_1F		0x0005
#define KEY_1FS		0x0006
#define KEY_1Gb		0x8006
#define KEY_1G		0x0007
#define KEY_1GS		0x0008
#define KEY_1Ab		0x8008
#define KEY_1A		0x0009
#define KEY_1AS		0x000A
#define KEY_1Bb		0x800A
#define KEY_1B		0x000B
#define KEY_2C		0x000C
#define KEY_2CS		0x000D
#define KEY_2Db		0x800D
#define KEY_2D		0x000E
#define KEY_2DS		0x000F
#define KEY_2Eb		0x800F
#define KEY_2E		0x0010
#define KEY_2F		0x0011
#define KEY_2FS		0x0012
#define KEY_2Gb		0x8012
#define KEY_2G		0x0013
#define KEY_2GS		0x0014
#define KEY_2Ab		0x8014
#define KEY_2A		0x0015
#define KEY_2AS		0x0016
#define KEY_2Bb		0x8016
#define KEY_2B		0x0017

#define KEYSTRING_1C		"1C"
#define KEYSTRING_1CS		"1C#"
#define KEYSTRING_1Db		"1Db"
#define KEYSTRING_1D		"1D"
#define KEYSTRING_1DS		"1D#"
#define KEYSTRING_1Eb		"1Eb"
#define KEYSTRING_1E		"1E"
#define KEYSTRING_1F		"1F"
#define KEYSTRING_1FS		"1F#"
#define KEYSTRING_1Gb		"1Gb"
#define KEYSTRING_1G		"1G"
#define KEYSTRING_1GS		"1G#"
#define KEYSTRING_1Ab		"1Ab"
#define KEYSTRING_1A		"1A"
#define KEYSTRING_1AS		"1A#"
#define KEYSTRING_1Bb		"1Bb"
#define KEYSTRING_1B		"1B"
#define KEYSTRING_2C		"2C"
#define KEYSTRING_2CS		"2C#"
#define KEYSTRING_2Db		"2Db"
#define KEYSTRING_2D		"2D"
#define KEYSTRING_2DS		"2D#"
#define KEYSTRING_2Eb		"2Eb"
#define KEYSTRING_2E		"2E"
#define KEYSTRING_2F		"2F"
#define KEYSTRING_2FS		"2F#"
#define KEYSTRING_2Gb		"2Gb"
#define KEYSTRING_2G		"2G"
#define KEYSTRING_2GS		"2G#"
#define KEYSTRING_2Ab		"2Ab"
#define KEYSTRING_2A		"2A"
#define KEYSTRING_2AS		"2A#"
#define KEYSTRING_2Bb		"2Bb"
#define KEYSTRING_2B		"2B"

class CCommandStruct {
public:
    CCommandStruct();
	~CCommandStruct();
	int		StructToString(char *, int);
	HRESULT	StringToStruct(char *, ULONG);
	int		GetSize();

    LONG            m_lTime;
    SHORT           m_nMeasure;
    DWORD           m_dwCommand;	// The command for this measure.
    DWORD           m_dwSignPost;	// The sign post for this measure.
};

class CommandList {
public:
	CommandList();
	~CommandList();
	void RemoveAll();
	void RemoveCommands();
	void RemoveSignPosts();

	CList <CCommandStruct *, CCommandStruct *> m_list;
};

class CTemplateStruct{
public:
	CTemplateStruct();
	~CTemplateStruct();
	int		StructToString(char *, int);
	HRESULT	StringToStruct(char *, ULONG);
	int		GetSize();

    char            m_szName[20];
    char            m_szType[20];
    SHORT           m_nMeasures;
	WORD			m_wActivityLevel;
	WORD			m_wKey;
	BOOL			m_bLoop;
};

CString KeyToStr(WORD wKey);
WORD	StrToKey(CString str);
BOOL	IsBelowFlat(WORD wKey);

#endif
