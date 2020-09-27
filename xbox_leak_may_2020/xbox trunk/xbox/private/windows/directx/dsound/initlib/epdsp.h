
#if !defined _EPDSP_H_
#define _EPDSP_H_


// BUGBUG make sure all these stays in sync with whatver dolby does

#define AC3_SUPREXECBASE        0x000
#define AC3_LOADERBASE          0x180
#define AC3_LOADERTABLEBASE     0x100  
#define AC3_DOLBYBASE           0x300

#define AC3_MAX_PROGRAMS        6

#define AC3_HEAPLENGTH            8192  // length in words

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

#define DOLBY_LOADER_TABLE_OFFSET(x)    (ULONG)&(((DOLBY_LOADER_TABLE *)0)->x)

class CMcpxEPDspManagerTest 
{
public:
    CMcpxEPDspManagerTest();
    ~CMcpxEPDspManagerTest();

public:
	VOID	Initialize(CMcpxGPDspManagerTest *pGlobalProc);
    
    VOID   AC3SetAnalogOutput(ULONG uOffset, ULONG uSize);   // actually a FIFO index
    VOID   AC3SetDigitalOutput(ULONG uOffset, ULONG uSize);  // actually a FIFO index

    VOID *GetScratchSpaceAddr() { return m_pScratchDma->GetScratchSpaceLinAddr(); }

    DOLBY_CONFIG_TABLE* GetDolbyConfigTable();

    // AC3 functions
    static ULONG AC3GetTotalScratchSize();
    static VOID AC3GetSuperExec(PVOID *ppData, ULONG *pSize, ULONG *pBase);
    static VOID AC3GetLoader(PVOID *ppData, ULONG *pSize, ULONG *pBase);
    static VOID AC3GetProgram(ULONG uIndex, PVOID *ppData, ULONG *pSize);
    
    static PVOID AC3GetInitialConfigTable();
    
    static ULONG AC3GetLoaderTableBase() { return AC3_LOADERTABLEBASE; }
    static ULONG AC3GetProgramBase() { return AC3_DOLBYBASE; }
    static ULONG AC3GetMaxPrograms() { return AC3_MAX_PROGRAMS; }
    static ULONG AC3GetHeapSize() { return AC3_HEAPLENGTH; }

private:
    
    VOID   AC3StartGpInput(ULONG uLinOffset);

    CMcpxDspScratchDmaTest  *m_pScratchDma;
    CMcpxGPDspManagerTest   *m_pGlobalProc;
    PUCHAR               m_pConfigTable;

};


#endif