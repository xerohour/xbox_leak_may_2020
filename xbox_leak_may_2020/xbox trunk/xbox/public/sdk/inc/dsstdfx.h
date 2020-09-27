
typedef enum _DSP_IMAGE_DSSTDFX_FX_INDICES {
    I3DL2_CHAIN_I3DL2_REVERB = 0,
    I3DL2_CHAIN_XTALK = 1,
    CHORUS_CHAIN_CHORUS = 2,
    FLANGE_CHAIN_FLANGE = 3,
    AMPMOD_CHAIN_AMPMOD = 4,
    ECHO_CHAIN_ECHO = 5,
    IIR2_CHAIN_IIR2_LEFT = 6,
    IIR2_CHAIN_IIR2_RIGHT = 7,
    REVERB_CHAIN_MINI_REVERB = 8
} DSP_IMAGE_DSSTDFX_FX_INDICES;

typedef struct _I3DL2_CHAIN_FX0_I3DL2_REVERB_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[35];     // XRAM offsets in DSP WORDS, of output mixbins
} I3DL2_CHAIN_FX0_I3DL2_REVERB_STATE, *LPI3DL2_CHAIN_FX0_I3DL2_REVERB_STATE;

typedef const I3DL2_CHAIN_FX0_I3DL2_REVERB_STATE *LPCI3DL2_CHAIN_FX0_I3DL2_REVERB_STATE;

typedef struct _I3DL2_CHAIN_FX1_XTALK_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[4];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[4];     // XRAM offsets in DSP WORDS, of output mixbins
} I3DL2_CHAIN_FX1_XTALK_STATE, *LPI3DL2_CHAIN_FX1_XTALK_STATE;

typedef const I3DL2_CHAIN_FX1_XTALK_STATE *LPCI3DL2_CHAIN_FX1_XTALK_STATE;

typedef struct _CHORUS_CHAIN_FX0_CHORUS_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[3];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[2];     // XRAM offsets in DSP WORDS, of output mixbins
} CHORUS_CHAIN_FX0_CHORUS_STATE, *LPCHORUS_CHAIN_FX0_CHORUS_STATE;

typedef const CHORUS_CHAIN_FX0_CHORUS_STATE *LPCCHORUS_CHAIN_FX0_CHORUS_STATE;

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

typedef struct _IIR2_CHAIN_FX0_IIR2_LEFT_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} IIR2_CHAIN_FX0_IIR2_LEFT_STATE, *LPIIR2_CHAIN_FX0_IIR2_LEFT_STATE;

typedef const IIR2_CHAIN_FX0_IIR2_LEFT_STATE *LPCIIR2_CHAIN_FX0_IIR2_LEFT_STATE;

typedef struct _IIR2_CHAIN_FX1_IIR2_RIGHT_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} IIR2_CHAIN_FX1_IIR2_RIGHT_STATE, *LPIIR2_CHAIN_FX1_IIR2_RIGHT_STATE;

typedef const IIR2_CHAIN_FX1_IIR2_RIGHT_STATE *LPCIIR2_CHAIN_FX1_IIR2_RIGHT_STATE;

typedef struct _REVERB_CHAIN_FX0_MINI_REVERB_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[35];     // XRAM offsets in DSP WORDS, of output mixbins
} REVERB_CHAIN_FX0_MINI_REVERB_STATE, *LPREVERB_CHAIN_FX0_MINI_REVERB_STATE;

typedef const REVERB_CHAIN_FX0_MINI_REVERB_STATE *LPCREVERB_CHAIN_FX0_MINI_REVERB_STATE;
