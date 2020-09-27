#ifndef __DLSRIFFID_H__
#define __DLSRIFFID_H__

// DLSRiffId.h : header file
//

#define MAKETAG( ch0, ch1, ch2, ch3 )                                \
                ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

#define RIFF_TAG						MAKETAG('R','I','F','F')
#define LIST_TAG						MAKETAG('L','I','S','T')

#define ID_DLS_FOLDER					MAKETAG('F','D','L','S')
#define ID_COLLECTION_NODE				MAKETAG('N','C','o','l')
#define ID_INSTRUMENT_FOLDER			MAKETAG('F','I','n','s')
#define ID_INSTRUMENT_NODE				MAKETAG('N','I','n','s')
#define ID_ARTICULATION_NODE			MAKETAG('N','A','r','t')
#define ID_REGION_FOLDER				MAKETAG('F','R','e','g')
#define ID_REGION_NODE					MAKETAG('N','R','e','g')
#define ID_VARCHOICES_FOLDER			MAKETAG('F','V','a','r')
#define ID_WAVE_FOLDER					MAKETAG('F','W','a','v')
#define ID_WAVE_NODE					MAKETAG('N','W','a','v')

#define DLS_REGION_EXTRA_CHUNK			mmioFOURCC('d','m','p','r')
#endif // __DLSRIFFID_H__