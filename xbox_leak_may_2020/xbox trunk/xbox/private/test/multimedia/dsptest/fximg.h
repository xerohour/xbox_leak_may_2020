
typedef enum _DSP_IMAGE_DSSTDFX_FX_INDICES {
    OSCILLATOR_CHAIN_OSCILLATOR = 0,
    CHORUS_CHAIN_CHORUS_L = 1,
    CHORUS_CHAIN_CHORUS_R = 2,
    FLANGE_CHAIN_FLANGE_L = 3,
    FLANGE_CHAIN_FLANGE_R = 4,
    AMPMOD_CHAIN_AMPMOD = 5,
    ECHO_CHAIN_ECHO = 6,
    RMS_CHAIN_RMS = 7
} DSP_IMAGE_DSSTDFX_FX_INDICES;

typedef struct _OSCILLATOR_CHAIN_FX0_OSCILLATOR_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwOutMixbinPtrs[4];     // XRAM offsets in DSP WORDS, of output mixbins
} OSCILLATOR_CHAIN_FX0_OSCILLATOR_STATE, *LPOSCILLATOR_CHAIN_FX0_OSCILLATOR_STATE;

typedef const OSCILLATOR_CHAIN_FX0_OSCILLATOR_STATE *LPCOSCILLATOR_CHAIN_FX0_OSCILLATOR_STATE;

typedef struct _CHORUS_CHAIN_FX0_CHORUS_L_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} CHORUS_CHAIN_FX0_CHORUS_L_STATE, *LPCHORUS_CHAIN_FX0_CHORUS_L_STATE;

typedef const CHORUS_CHAIN_FX0_CHORUS_L_STATE *LPCCHORUS_CHAIN_FX0_CHORUS_L_STATE;

typedef struct _CHORUS_CHAIN_FX1_CHORUS_R_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} CHORUS_CHAIN_FX1_CHORUS_R_STATE, *LPCHORUS_CHAIN_FX1_CHORUS_R_STATE;

typedef const CHORUS_CHAIN_FX1_CHORUS_R_STATE *LPCCHORUS_CHAIN_FX1_CHORUS_R_STATE;

typedef struct _FLANGE_CHAIN_FX0_FLANGE_L_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} FLANGE_CHAIN_FX0_FLANGE_L_STATE, *LPFLANGE_CHAIN_FX0_FLANGE_L_STATE;

typedef const FLANGE_CHAIN_FX0_FLANGE_L_STATE *LPCFLANGE_CHAIN_FX0_FLANGE_L_STATE;

typedef struct _FLANGE_CHAIN_FX1_FLANGE_R_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} FLANGE_CHAIN_FX1_FLANGE_R_STATE, *LPFLANGE_CHAIN_FX1_FLANGE_R_STATE;

typedef const FLANGE_CHAIN_FX1_FLANGE_R_STATE *LPCFLANGE_CHAIN_FX1_FLANGE_R_STATE;

typedef struct _AMPMOD_CHAIN_FX0_AMPMOD_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[3];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[2];     // XRAM offsets in DSP WORDS, of output mixbins
} AMPMOD_CHAIN_FX0_AMPMOD_STATE, *LPAMPMOD_CHAIN_FX0_AMPMOD_STATE;

typedef const AMPMOD_CHAIN_FX0_AMPMOD_STATE *LPCAMPMOD_CHAIN_FX0_AMPMOD_STATE;

typedef struct _ECHO_CHAIN_FX0_ECHO_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[2];     // XRAM offsets in DSP WORDS, of output mixbins
} ECHO_CHAIN_FX0_ECHO_STATE, *LPECHO_CHAIN_FX0_ECHO_STATE;

typedef const ECHO_CHAIN_FX0_ECHO_STATE *LPCECHO_CHAIN_FX0_ECHO_STATE;

typedef struct _RMS_CHAIN_FX0_RMS_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[6];      // XRAM offsets in DSP WORDS, of input mixbins
} RMS_CHAIN_FX0_RMS_STATE, *LPRMS_CHAIN_FX0_RMS_STATE;

typedef const RMS_CHAIN_FX0_RMS_STATE *LPCRMS_CHAIN_FX0_RMS_STATE;
