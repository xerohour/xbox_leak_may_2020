#include <xtl.h>

#define AC3_SUPREXECBASE        0x000
#define AC3_LOADERBASE          0x180
#define AC3_LOADERTABLEBASE     0x100  
#define AC3_DOLBYBASE           0x300

#define AC3_MAX_PROGRAMS        6

#define AC3_HEAPLENGTH            8192  // length in words

static ULONG AC3SuperExec[] = 
{
#include "AC3SuperExec.h"
};

static ULONG AC3Loader[] =
{
#include "AC3Loader.h"
};

static ULONG AC3Dolby1[] =
{
#include "AC3Dolby1.h"
};

static ULONG AC3Dolby2[] =
{
#include "AC3Dolby2.h"
};

static ULONG AC3Dolby3[] =
{
#include "AC3Dolby3.h"
};

static ULONG AC3Dolby4[] =
{
#include "AC3Dolby4.h"
};

static ULONG AC3Dolby5[] =
{
#include "AC3Dolby5.h"
};

static ULONG AC3Dolby6[] =
{
#include "AC3Dolby6.h"
};

typedef struct
{
    ULONG table_size;
    ULONG do_surround_encode;
    ULONG do_game_encode;
	ULONG do_downmix_encode;
    ULONG pcm_sample_rate_code;
    ULONG input_ch_config;
    ULONG lfe_present;
    ULONG ac3_dialnorm;
    ULONG ac3_bandwidth;
    ULONG channel_lpf_enabled;
    ULONG lfe_lpf_enabled;
    ULONG dc_hpf_enabled;
    ULONG dynrng_exists;
    ULONG ac3_dynrng_code;
    ULONG compr_exists;
    ULONG ac3_compr_code;
    ULONG surround_gain_enabled;
    ULONG surround_gain;
    ULONG surround_mode;
	ULONG reserved_1;
	ULONG reserved_2;
	ULONG reserved_3;
	ULONG reserved_4;
	ULONG reserved_5;
} DOLBY_CONFIG_TABLE;

typedef struct
{
    // the entries in this table are actually all 24-bit DSP words, but when
    // we store DSP code in system memory, we extend each word to fill 32 bits,
    // so we can store them in U032S.  
    ULONG tableSize;         // table size                  
    ULONG maxProgs;          // number of programs

    struct
    {
        ULONG ptr;
        ULONG size;
    } prog[AC3_MAX_PROGRAMS];

    ULONG pcm_ptr;              // pointer to input PCM buffer
    ULONG pcm_size;             // size of input PCM buffer
    ULONG ltrt_ptr;             // pointer to output Lt/Rt buffer  (Note: actually a FIFO index!!)
    ULONG ltrt_size;            // size of output Lt/Rt buffer
    ULONG ac3_ptr;              // pointer to output AC-3 buffer   (Note: actually a FIFO index!!)
    ULONG ac3_size;             // size of output AC-3 buffer
    ULONG config_ptr;           // pointer to config table
    ULONG config_size;          // size of config table
    ULONG pingpong_offset;      // current pingpong buffer offset
    ULONG reserved1;
    ULONG ac3_zero_fill;        // AC3 zero fill (Note: actually a FIFO index!!)
    ULONG reserved2;
    ULONG ac3_preamble;         // AC3 preamble (Note: actually a FIFO index!!)
    ULONG reserved3;
    ULONG heap_ptr;             // pointer to heap data buffer
    ULONG heap_size;            // size of heap data buffer
} DOLBY_LOADER_TABLE;

static DOLBY_CONFIG_TABLE AC3ConfigTable =
  {
    24,        // table size
	1,         // perform dolby surround encode flag
    1,         // perform dolby game encode flag
	1,		   // perform downmix
    0, 		   // ac3 pcm sampling rate code
    7,         // ac3 dolby input channel config code (acmod)
    1,         // lfe channel present flag
    16,        // ac3 dialnorm value
    9,         // ac3 channel bandwidth code
    0,         // channel lpf enabled flag
    1,         // lfe channel lpf enabled flag
    1,         // dc hpf enabled flag
    0,         // dynrng exists flag
    0,         // ac3 dynrng code
    1,         // compr exists flag
    0xEF,      // ac3 compr code
    1,         // dolby surround gain enabled flag
    0x47FACD,  // dolby surround encoder gain value
    2,          // dolby surround mode. Only used for 2/0 Mode
	0,			// reserved 1
	0,			// reserved 2
	0,			// reserved 3
	0,			// reserved 4
	0			// reserved 5
  };

EXTERN_C HRESULT WINAPI
DirectSoundLoadEncoder
(
    LPCVOID                 pvImageBuffer, 
    DWORD                   dwImageSize, 
    LPVOID *                ppvScratchData, 
    LPDIRECTSOUND *         ppDirectSound
);

VOID
AC3GetProgram(ULONG uIndex, PVOID *ppData, ULONG *pSize)
{
    switch(uIndex)
    {
    case 0:
        *ppData = (PVOID)AC3Dolby1;
        *pSize = sizeof(AC3Dolby1)/sizeof(DWORD);
        break;
    case 1:
        *ppData = (PVOID)AC3Dolby2;
        *pSize = sizeof(AC3Dolby2)/sizeof(DWORD);
        break;
    case 2:
        *ppData = (PVOID)AC3Dolby3;
        *pSize = sizeof(AC3Dolby3)/sizeof(DWORD);
        break;
    case 3:
        *ppData = (PVOID)AC3Dolby4;
        *pSize = sizeof(AC3Dolby4)/sizeof(DWORD);
        break;
    case 4:
        *ppData = (PVOID)AC3Dolby5;
        *pSize = sizeof(AC3Dolby5)/sizeof(DWORD);
        break;
    case 5:
        *ppData = (PVOID)AC3Dolby6;
        *pSize = sizeof(AC3Dolby6)/sizeof(DWORD);
        break;

    default:
        *ppData = NULL;
        *pSize = 0;
        break;
    }
}

HRESULT LoadDolbyCode()
{

    HRESULT hr=S_OK;
//    DWORD err;
    DWORD dwSize;
    DWORD dwOffset = 0;
    DOLBY_LOADER_TABLE LoaderTable;
    PVOID pCode;

    //
    // base virtual address of the EP scratch space
    //

    PVOID pScratchAddr;

    PDWORD pBuffer;

    dwSize = (AC3_DOLBYBASE * sizeof(ULONG) + 
            sizeof(AC3Dolby1) + 
            sizeof(AC3Dolby2) + 
            sizeof(AC3Dolby3) + 
            sizeof(AC3Dolby4) +
            sizeof(AC3Dolby5) +
            sizeof(AC3Dolby6) +
            sizeof(DOLBY_CONFIG_TABLE) + 
            AC3_HEAPLENGTH * sizeof(ULONG));


    pBuffer = new DWORD[dwSize/sizeof(DWORD)];

    if (pBuffer == NULL) {
        return E_OUTOFMEMORY;
    }

    memset(pBuffer,0,dwSize);

    //
    // super exec goes first in memory
    //

    dwOffset = AC3_SUPREXECBASE;
    memcpy(pBuffer+dwOffset,AC3SuperExec,sizeof(AC3SuperExec));

    //
    // now the loader
    //

    dwOffset = AC3_LOADERBASE;
    memcpy(pBuffer+dwOffset,AC3Loader,sizeof(AC3Loader));

    //
    // now prepare the table
    //

    memset(&LoaderTable,0,sizeof(DOLBY_LOADER_TABLE));

    dwOffset = AC3_DOLBYBASE;

    for (ULONG uCnt = 0; uCnt < AC3_MAX_PROGRAMS; uCnt++)
    {
        AC3GetProgram(uCnt, &pCode, &dwSize);
        memcpy(pBuffer+dwOffset, (VOID *)pCode, dwSize*sizeof(ULONG));

        //
        // update the loader table
        //

        LoaderTable.prog[uCnt].ptr = dwOffset;
        LoaderTable.prog[uCnt].size = dwSize ;
        dwOffset += dwSize;
    }

    //
    // now the config table
    //

    ULONG *pConfig = (ULONG *)&AC3ConfigTable;

    //
    // Note: the AC3 config table is actually a DOLBY_CONFIG_TABLE,
    // not a ULONG...  The size is contained in the first word of the
    // table, so we will treat it here as a ULONG array.
    //

    dwSize = pConfig[0];

    LoaderTable.config_ptr = dwOffset;
    LoaderTable.config_size = dwSize;

    memcpy(pBuffer+dwOffset, (VOID *)pConfig, dwSize * sizeof(ULONG));
    dwOffset += dwSize;

    //Init pingpong buffer offset
    LoaderTable.pingpong_offset = 0;
    LoaderTable.reserved1 = 0;
    LoaderTable.reserved2 = 0;
    LoaderTable.reserved3 = 0;

	// Init buffer pointers JMW 6/27/01

	LoaderTable.pcm_ptr			= 0x00003c00;
	LoaderTable.pcm_size		= 0x00000600;
	LoaderTable.ltrt_ptr		= 0x00000000;
	LoaderTable.ltrt_size		= 0x00000800;
	LoaderTable.ac3_ptr			= 0x00000001;
	LoaderTable.ac3_size		= 0x00001000;
	LoaderTable.ac3_zero_fill	= 0x00000001;
	LoaderTable.ac3_preamble	= 0x00000001;
    
    //
    // now for the heap
    //

    LoaderTable.heap_ptr = dwOffset;
    LoaderTable.heap_size = AC3_HEAPLENGTH;

    dwOffset = AC3_LOADERTABLEBASE;
    memcpy(pBuffer+dwOffset,&LoaderTable,sizeof(DOLBY_LOADER_TABLE));

    //
    // pass this buffer to dsound so it gets downloaded to the EP before it starts...
    //

    dwSize = (AC3_DOLBYBASE * sizeof(ULONG) + 
            sizeof(AC3Dolby1) + 
            sizeof(AC3Dolby2) + 
            sizeof(AC3Dolby3) + 
            sizeof(AC3Dolby4) +
            sizeof(AC3Dolby5) + 
            sizeof(AC3Dolby6) +
            sizeof(DOLBY_CONFIG_TABLE) + 
            AC3_HEAPLENGTH * sizeof(ULONG));

    LPDIRECTSOUND pDirectSound;

/*    hr = DirectSoundLoadEncoder(pBuffer,
                                dwSize,
                                &pScratchAddr,
                                &pDirectSound);
*/

    return hr;
}

