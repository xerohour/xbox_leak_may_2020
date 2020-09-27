/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       decoder.h
 *  Content:    The main definitions for the XMV decoder.
 *
 ****************************************************************************/

#pragma once

/* 
 * Constants
 */

// Y block size.
#define MACROBLOCK_SIZE     16

// UV block size
#define BLOCK_SIZE          8

/* 
 * Debug stuff
 */

#if DBG
#    define RIP(msg)   XDebugError("XMV", msg)
#else
#    define RIP(msg)
#endif

/*
 * It really sucks that we have three different structures to represent the
 * same data for audio information, but it is really handy to have this info
 * in exact the form we need in each of the three places (file, API and 
 * implementation).
 */

typedef struct _XMVAudioImplementation
{
    // The WAVE_FORMAT tag that describes how the audio data in the stream is
    // encoded.  This can be either WAVE_FORMAT_PCM or WAVE_FORMAT_ADPCM.
    //
    DWORD WaveFormat;

    // The number of channels in the audio stream.  Can be 1, 2, 4 or 6.
    DWORD ChannelCount;

    // The number of samples per second (Hz) in the audio stream.
    DWORD SamplesPerSecond;

    // The number of bits in each sample.  
    DWORD BitsPerSample;
}
XMVAudioImplementation;

/* 
 * Information we need to save about the macroblocks while decoding.  We need
 * to keep two rows of these around for the decoding to work properly.
 *
 * We pre-allocate this along with the decoder simply to avoid doing memory
 * allocations each time we go to decode a frame.  We always allocate on blank
 * dummy entry to the left of our entries so that the CBPCY code doesn't have
 * to do any special casing.
 */

typedef struct _XMVMacroblockCBPCY
{
    BYTE Y1:1;
    BYTE Y2:1;
    BYTE Y3:1;
    BYTE Y4:1;
    BYTE U:1;
    BYTE V:1;
}
XMVMacroblockCBPCY;

/*
 * Holds all of the information needed by the block decoder in a handy little
 * package.  Saves us from 
 */

typedef struct _XMVACCoefficientDecoderTable
{
    WORD *pDTCACDecoderTable;
    DWORD DCTACDecoderEscapeCode;

    DWORD StartIndexOfLastRun;

    BYTE *RunTable;
    BYTE *NotLastDeltaRunTable;
    BYTE *LastDeltaRunTable;

    char *LevelTable;
    BYTE *NotLastDeltaLevelTable;
    BYTE *LastDeltaLevelTable;
}
XMVACCoefficientDecoderTable;

/*
 * The main XMV decoder structure, holds absolutely everything we need to do 
 * the decoding.
 */

struct _XMVDecoder
{
    /*
     * Members used by the file loader
     */

    // The size of the source file.
    LONGLONG FileSize;

    // How much of that file we've consumed.
    LONGLONG FileSizeRead;

    // The source file.
    HANDLE hFile;

    // The size of each of our buffers.
    DWORD BufferSize;

    // The buffer we're currently decoding from.
    BYTE *pDecodingBuffer;

    // The number of frames left in that buffer.
    DWORD RemainingFrames;

    // The buffer we're currently loading into.
    BYTE *pLoadingBuffer;

    // Where we are currently reading from in the file.
    OVERLAPPED Overlapped;

    // The size of the header on the initial frame of the video.
    DWORD InitialFrameHeaderSize;

    /*
     * Information needed by the decoder.
     */

    // The geometry of the video.  The surface that each frame is rendered
    // onto must be exactly this size.  If width and height are zero then 
    // there is no video stream in this file.
    //
    DWORD Width;
    DWORD Height;

    // The frame rate of the file.
    DWORD FramesPerSecond;

    // The number of slices the picture is divided into.
    DWORD Slices;

    // Various flags about how the picture is encoded.  This could be packed
    // a bit more but why bother to save a few bytes?
    //
    BOOLEAN MixedPelMotionCompensationEnable; 
    BOOLEAN LoopFilterEnabled; 
    BOOLEAN VariableSizedTransformEnabled;
    BOOLEAN XIntra8IPictureCodingEnabled;
    BOOLEAN HybridMotionVectorEnabled;
    BOOLEAN DCTTableSwitchingEnabled;

    // The number of audio streams encoded in this file.
    DWORD AudioStreamCount;

    // Array of audio descriptors.
    XMVAudioImplementation *Audio;

    /*
     * Members used during the actual decoding process
     */

    // Whether the next frame has already been decoded.
    BOOL IsNextFrameDecoded;

    // The size of the next packet.
    DWORD NextPacketSize;

    // The size of the UV part of the frame buffer.
    DWORD UVWidth;
    DWORD UVHeight;

    // The size of the picture in macroblocks.
    DWORD MBWidth;
    DWORD MBHeight;

    // The frame buffer currently being displayed.
    BYTE *pYDisplayed;
    BYTE *pUDisplayed;
    BYTE *pVDisplayed;

    // The frame buffer currently being built.
    BYTE *pYBuilding;
    BYTE *pUBuilding;
    BYTE *pVBuilding;

    // The macroblock CBPCY values for the previous row.
    XMVMacroblockCBPCY *pCBPCY;

    // The DC Precitors for the lower blocks of all of the macroblocks in the 
    // previous row.
    //
    short *pYAC;
    short *pUAC;
    short *pVAC;

    /*
     * Bit walker members
     */

    // Holds a byte-swapped version of 4 bytes of the buffer.
    DWORD BitCache;

    // The number of valid bits left in the cache.
    DWORD BitsRemaining;

    // The next 4 bytes we'll read from in the decoding buffer.
    BYTE *pDecodingPosition;
};

/* 
 * The rendering frontend (frontend.c)
 */

XMVACCoefficientDecoderTable g_InterDecoderTables[];
XMVACCoefficientDecoderTable g_IntraDecoderTables[];

BYTE g_HorizontalZigzag[];
BYTE g_VerticalZigzag[];
BYTE g_NormalZigzag[]; 

void DecodeOneFrame(XMVDecoder *pDecoder);

/*
 * The rendering backend (backend.c)
 */

void RenderBitmap(XMVDecoder *pDecoder, D3DSurface *pSurface);

/*
 * The huffman decoding routines and tables (huffman.c)
 */

WORD g_Huffman_ICBPCY[];

#define INTRADCYTCOEF_ESCAPE_CODE 119

WORD g_Huffman_DCTDCy_Talking[];
WORD g_Huffman_DCTDCy_HighMotion[];

#define INTRADCUVTCOEF_ESCAPE_CODE 119

WORD g_Huffman_DCTDCc_Talking[];
WORD g_Huffman_DCTDCc_HighMotion[];

DWORD HuffmanDecode(XMVDecoder *pDecoder, WORD *pHuffmanTable);

/*
 * The bit walker (bits.c)
 */

#if DBG

extern DWORD g_TotalBitsRead;

#endif

DWORD ReadOneBit(XMVDecoder *pDecoder);
DWORD ReadTriStateBits(XMVDecoder *pDecoder);

DWORD ReadBits(XMVDecoder *pDecoder, DWORD Bits);
DWORD PeekBits(XMVDecoder *pDecoder, DWORD Bits);
void  SkipBits(XMVDecoder *pDecoder, DWORD Bits);

