#ifndef __WAVECOMPRESSIONMANAGER_H__
#define __WAVECOMPRESSIONMANAGER_H__

// WaveCompressionManager.h : header file
//

#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>

class CWave;


//================================================================
// Borrowed definitions from MSAudio team
//================================================================
#define VOXWARE_METAVOICE_FORMAT_TAG                116
#define VOXWARE_METASOUND_FORMAT_TAG                117
#define IsVoxwareCodec(dwFormatTag) ( (VOXWARE_METASOUND_FORMAT_TAG == dwFormatTag) || (VOXWARE_METAVOICE_FORMAT_TAG == dwFormatTag) )

#define  SCODE_FROM_ACM( x ) (x ? ((HRESULT) (((x) & 0x0000FFFF) | (FACILITY_ACM << 16) | 0xC0000000)) : 0 )

#define MSAUDIO_FORMAT_TAG	352	// V1
#define WMAUDIO_FORMAT_TAG	353	// V2

#ifdef DMP_XBOX
#define XBOXADPCM_FORMAT_TAG 105
extern void CreateXboxAdpcmFormat( WORD nChannels, DWORD nSamplesPerSec, LPIMAADPCMWAVEFORMAT pwfx );
#endif // DMP_XBOX

// Keys required for MS Audio 4.0 CODEC
// Both Codecs use the same keys
#define MSAUDIO_KEY "F6DC9830-BC79-11d2-A9D0-006097926036"
#define MSAUDIO_DEC_KEY "1A0F78F0-EC8A-11d2-BBBE-006008320064"
#define MSAUDIOWAVEFORMATEX_EXTRA_BYTES	4

#pragma pack(1)
// Special format for V1
typedef struct msaudio1waveformat_tag {
    WAVEFORMATEX wfx;
    WORD         wSamplesPerBlock; // only counting "new" samples "= half of what will be used due to overlapping
    WORD         wEncodeOptions;
} MSAUDIO1WAVEFORMAT;

typedef MSAUDIO1WAVEFORMAT FAR  *LPMSAUDIO1WAVEFORMAT;


// Special format for V2
typedef struct wmaudio2waveformat_tag {
    WAVEFORMATEX wfx;
    DWORD        dwSamplesPerBlock; // only counting "new" samples "= half of what will be used due to overlapping
    WORD         wEncodeOptions;
    DWORD        dwSuperBlockAlign; // the big size...  should be multiples of wfx.nBlockAlign.
} WMAUDIO2WAVEFORMAT;

typedef WMAUDIO2WAVEFORMAT FAR  *LPWMAUDIO2WAVEFORMAT;

#pragma pack()

//=================================================================


// Keeps the information about a driver
typedef struct _FORMAT_TAG_INF0
{
	CString			sDriverName;
	HACMDRIVERID	hadid;
	DWORD			dwFormatTag;

} FORMAT_TAG_INF0;

// Keeps the information about a supported 
// format for a particular driver.
typedef struct _SUPPORTED_FORMAT_INFO
{
	CString			sSupportedFormatName;
	WAVEFORMATEX*	pWaveFormatEx;

} SUPPORTED_FORMAT_INFO;
	
class CWaveCompressionManager
{
	friend class CWaveCompressionPropPage;

//Construction
public:
	CWaveCompressionManager();
	~CWaveCompressionManager();

// Implementation
	BOOL GetSupportedDrivers();
	BOOL GetSupportedFormats(WORD wFormatTag, CWave *pWave);

	/* fills m_arrSupportedFormats with formats supported by given driver, and for given wave */
	void GetSupportedFormats(const FORMAT_TAG_INF0* pFormatInfo, CWave *pWave);

private:
	void ClearDriverCollection();
	void ClearFormatCollection();
	
	static MMRESULT EnumFormatTags(HACMDRIVER *phad, DWORD dwInstance);
	
	static BOOL CALLBACK acmDriverEnumCallback(HACMDRIVERID hadid,
									DWORD dwInstance,
									DWORD fdwSupport);

	static BOOL CALLBACK acmFormatTagEnumCallback(HACMDRIVERID hadid,           
									   LPACMFORMATTAGDETAILS paftd,  
									   DWORD dwInstance,             
									   DWORD fdwSupport);

	static BOOL CALLBACK acmFormatEnumCallback(HACMDRIVERID hadid,           
									LPACMFORMATDETAILS pafd,  
									DWORD dwInstance,             
									DWORD fdwSupport);

//Attributes
public:
	HACMDRIVERID								m_hMSAudioV1BetaDriverID;	// Keep the driver ID for the Beta V1 codec...we need to use 
																			// this specifically when opening a stream for a conversion 
																			// using this codec as the new V2 lists this codec as well...

private:
	bool										m_bGotDrivers;				// m_arrFormatTags and m_arrPCMFormatTags have been filled
	CTypedPtrArray<CPtrArray, FORMAT_TAG_INF0*>	m_arrFormatTags;			// Keeps all the registred drivers in FORMAT_TAG_INFO objects 
	CTypedPtrArray<CPtrArray, FORMAT_TAG_INF0*>	m_arrPCMFormatTags;			// Keeps the drivers that can handle PCM formats
	CTypedPtrArray<CPtrArray, SUPPORTED_FORMAT_INFO*>	m_arrSupportedFormats;		// Collects the supported Wave-Audio formats for a Format Tag
};

#endif // __WAVECOMPRESSIONMANAGER_H__
