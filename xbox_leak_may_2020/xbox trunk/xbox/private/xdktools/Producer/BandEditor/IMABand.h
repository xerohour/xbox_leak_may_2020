///////////////////////////////////////////////////////////////////////////////////////////
//
//	IMABand.h
//	Data structures for IMA Band files
//	Created 3/20/98 Eric Whitcombe
//
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef __IMA_BAND_H__
#define __IMA_BAND_H__

typedef struct _rIMAInstrument
{
public:
	BYTE         m_bMSB;
	BYTE         m_bLSB;
	BYTE         m_bPatch;
	BOOL         m_fDLS;
	BYTE         m_bPan;
	BYTE         m_bVolume;
	signed char  m_chOctave;
} IMA_INSTRUMENT, *PIMAINSTRUMENT;

// IO Structs

#pragma pack(2)

#define FOURCC_BAND_FORM        mmioFOURCC('A','A','B','N')
#define FOURCC_BAND             mmioFOURCC('b','a','n','d')
#define IMA_DRUM_TRACK			(5)  // zero based track 6

typedef struct ioBand
{
    wchar_t wstrName[20]; // Band name
    BYTE    abPatch[16];  // GM
    BYTE    abVolume[16];
    BYTE    abPan[16];
    signed char achOctave[16];
    char    fDefault;    // This band is the style's default band
    char    chPad;
    WORD    awDLSBank[16];  // if GM bit set use abPatch
	                        // if GS bit set, use this plus abDLSPatch
	                        // else use both as a DLS
    BYTE    abDLSPatch[16];
    GUID    guidCollection;
    char    szCollection[32];           // this only needs to be single-wide chars
} ioIMABand;

#pragma pack()



#endif //__IMA_BAND_H__