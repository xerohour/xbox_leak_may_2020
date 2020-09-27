
typedef enum _DSP_IMAGE_mixbinsImage_FX_INDICES {
    Graph0_Speakers = 0,
    Graph0_I3DL2_XTalk = 1,
    Graph0_FXSend0_To_5 = 2,
    Graph0_FXSend6_To_11 = 3,
    Graph0_FXSend12_To_17 = 4,
    Graph0_FXSend18_To_19 = 5
} DSP_IMAGE_mixbinsImage_FX_INDICES;

typedef struct _Graph0_FX0_Speakers_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[6];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph0_FX0_Speakers_STATE, *LPGraph0_FX0_Speakers_STATE;

typedef const Graph0_FX0_Speakers_STATE *LPCGraph0_FX0_Speakers_STATE;

typedef struct _Graph0_FX1_I3DL2_XTalk_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[6];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph0_FX1_I3DL2_XTalk_STATE, *LPGraph0_FX1_I3DL2_XTalk_STATE;

typedef const Graph0_FX1_I3DL2_XTalk_STATE *LPCGraph0_FX1_I3DL2_XTalk_STATE;

typedef struct _Graph0_FX2_FXSend0_To_5_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[6];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph0_FX2_FXSend0_To_5_STATE, *LPGraph0_FX2_FXSend0_To_5_STATE;

typedef const Graph0_FX2_FXSend0_To_5_STATE *LPCGraph0_FX2_FXSend0_To_5_STATE;

typedef struct _Graph0_FX3_FXSend6_To_11_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[6];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph0_FX3_FXSend6_To_11_STATE, *LPGraph0_FX3_FXSend6_To_11_STATE;

typedef const Graph0_FX3_FXSend6_To_11_STATE *LPCGraph0_FX3_FXSend6_To_11_STATE;

typedef struct _Graph0_FX4_FXSend12_To_17_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[6];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph0_FX4_FXSend12_To_17_STATE, *LPGraph0_FX4_FXSend12_To_17_STATE;

typedef const Graph0_FX4_FXSend12_To_17_STATE *LPCGraph0_FX4_FXSend12_To_17_STATE;

typedef struct _Graph0_FX5_FXSend18_To_19_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[6];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph0_FX5_FXSend18_To_19_STATE, *LPGraph0_FX5_FXSend18_To_19_STATE;

typedef const Graph0_FX5_FXSend18_To_19_STATE *LPCGraph0_FX5_FXSend18_To_19_STATE;
