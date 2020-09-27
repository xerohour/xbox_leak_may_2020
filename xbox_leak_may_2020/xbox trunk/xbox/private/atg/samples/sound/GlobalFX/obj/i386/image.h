
typedef enum _DSP_IMAGE_MY_FIRST_GRAPH_FX_INDICES {
    OSCILLATOR_CHAIN_OSCILLATOR = 0,
    DISTORTION_CHAIN_DISTORTION0 = 1,
    DISTORTION_CHAIN_DISTORTION1 = 2,
    CHORUS_CHAIN_CHORUS = 3,
    AMPMOD_FLANGE_CHAIN_AMPMOD = 4,
    AMPMOD_FLANGE_CHAIN_FLANGE = 5,
    ECHO_CHAIN_ECHO = 6
} DSP_IMAGE_MY_FIRST_GRAPH_FX_INDICES;

typedef struct _OSCILLATOR_CHAIN_FX0_OSCILLATOR_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwOutMixbinPtrs[4];     // XRAM offsets in DSP WORDS, of output mixbins
} OSCILLATOR_CHAIN_FX0_OSCILLATOR_STATE, *LPOSCILLATOR_CHAIN_FX0_OSCILLATOR_STATE;

typedef const OSCILLATOR_CHAIN_FX0_OSCILLATOR_STATE *LPCOSCILLATOR_CHAIN_FX0_OSCILLATOR_STATE;

typedef struct _DISTORTION_CHAIN_FX0_DISTORTION0_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} DISTORTION_CHAIN_FX0_DISTORTION0_STATE, *LPDISTORTION_CHAIN_FX0_DISTORTION0_STATE;

typedef const DISTORTION_CHAIN_FX0_DISTORTION0_STATE *LPCDISTORTION_CHAIN_FX0_DISTORTION0_STATE;

typedef struct _DISTORTION_CHAIN_FX1_DISTORTION1_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} DISTORTION_CHAIN_FX1_DISTORTION1_STATE, *LPDISTORTION_CHAIN_FX1_DISTORTION1_STATE;

typedef const DISTORTION_CHAIN_FX1_DISTORTION1_STATE *LPCDISTORTION_CHAIN_FX1_DISTORTION1_STATE;

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

typedef struct _AMPMOD_FLANGE_CHAIN_FX0_AMPMOD_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[3];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[2];     // XRAM offsets in DSP WORDS, of output mixbins
} AMPMOD_FLANGE_CHAIN_FX0_AMPMOD_STATE, *LPAMPMOD_FLANGE_CHAIN_FX0_AMPMOD_STATE;

typedef const AMPMOD_FLANGE_CHAIN_FX0_AMPMOD_STATE *LPCAMPMOD_FLANGE_CHAIN_FX0_AMPMOD_STATE;

typedef struct _AMPMOD_FLANGE_CHAIN_FX1_FLANGE_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[3];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[2];     // XRAM offsets in DSP WORDS, of output mixbins
} AMPMOD_FLANGE_CHAIN_FX1_FLANGE_STATE, *LPAMPMOD_FLANGE_CHAIN_FX1_FLANGE_STATE;

typedef const AMPMOD_FLANGE_CHAIN_FX1_FLANGE_STATE *LPCAMPMOD_FLANGE_CHAIN_FX1_FLANGE_STATE;

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
