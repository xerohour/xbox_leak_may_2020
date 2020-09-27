
typedef enum _DSP_IMAGE_DSSTDFX_FX_INDICES {
    FLANGE_CHAIN_FLANGE = 0,
    AMPMOD_CHAIN_AMPMOD = 1,
    SRC_CHAIN_SRC = 2
} DSP_IMAGE_DSSTDFX_FX_INDICES;

typedef struct _FLANGE_CHAIN_FX0_FLANGE_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[3];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[2];     // XRAM offsets in DSP WORDS, of output mixbins
} FLANGE_CHAIN_FX0_FLANGE_STATE, *LPFLANGE_CHAIN_FX0_FLANGE_STATE;

typedef const FLANGE_CHAIN_FX0_FLANGE_STATE *LPCFLANGE_CHAIN_FX0_FLANGE_STATE;

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

typedef struct _SRC_CHAIN_FX0_SRC_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} SRC_CHAIN_FX0_SRC_STATE, *LPSRC_CHAIN_FX0_SRC_STATE;

typedef const SRC_CHAIN_FX0_SRC_STATE *LPCSRC_CHAIN_FX0_SRC_STATE;
