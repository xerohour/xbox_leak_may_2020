
#include <precomp.h>

//
//  patchlzx.c
//
//  Author: Tom McGuire (tommcg) 2/97 - 9/97
//
//  Copyright (C) Microsoft, 1997-2001.
//
//  MICROSOFT CONFIDENTIAL
//



typedef struct _LZX_OUTPUT_CONTEXT {
    PUCHAR                   PatchBufferPointer;
    ULONG                    PatchBufferSize;
    ULONG                    PatchSize;
    BOOL                     DiscardOutput;
    } LZX_OUTPUT_CONTEXT, *PLZX_OUTPUT_CONTEXT;


ULONG
__fastcall
LzxWindowSize(
    IN ULONG OldDataSize,
    IN ULONG NewDataSize,
    IN DWORD OptionFlags,
    IN ULONG AbsoluteMax
    )
    {
    ULONG WindowSize;
    ULONG DataSize;

    DataSize = ROUNDUP2( OldDataSize, LZX_BLOCKSIZE ) + NewDataSize;

    if ( DataSize <= LZX_MINWINDOW )
    {
        return LZX_MINWINDOW;
    }

    WindowSize = LzxMaxWindowSize( OptionFlags, AbsoluteMax );

    while (( WindowSize >> 1 ) >= DataSize )
    {
        WindowSize = WindowSize >> 1;
    }

    return WindowSize;
    }


ULONG
__fastcall
LzxMaxWindowSize(
    IN ULONG OptionFlags,
    IN ULONG AbsoluteMax
    )
{
    ULONG WindowSize = ( OptionFlags & PATCH_OPTION_USE_LZX_LARGE ) ?
                            LZX_MAXWINDOW_32 :
                            LZX_MAXWINDOW_8;

    if ( AbsoluteMax )
    {
        if ( AbsoluteMax < LZX_MINWINDOW )
        {
             AbsoluteMax = LZX_MINWINDOW;
        }

        while ( WindowSize > AbsoluteMax )
        {
            WindowSize >>= 1;
        }
    }

    return WindowSize;
}


ULONG
__fastcall
LzxOldFileInsertSize(
    IN ULONG OldDataSize,
    IN DWORD OptionFlags,
    IN ULONG AbsoluteMaxLzxWindow,
    IN PPATCH_INTERLEAVE_MAP InterleaveMap
    )
{
    ULONG OldInsertSize = 0;

    if (( OptionFlags & PATCH_OPTION_INTERLEAVE_FILES ) &&
        ( InterleaveMap != NULL ) &&
        ( InterleaveMap->CountRanges > 0 ))
    {
        ULONG i;

        for ( i = 0; i < InterleaveMap->CountRanges; i++ )
        {
            OldInsertSize += InterleaveMap->Range[ i ].OldLength;
        }
    }
    else
    {
        ULONG MaxWindowSize = LzxMaxWindowSize( OptionFlags, AbsoluteMaxLzxWindow );

        OldInsertSize = MIN( OldDataSize, MaxWindowSize );
    }

    return OldInsertSize;
}


//
//  Following group of functions and exported apis are exclusively for
//  creating patches.  If we're only compiling the apply code, ignore
//  this group of functions.
//

#ifndef PATCH_APPLY_CODE_ONLY


PPATCH_INTERLEAVE_MAP
CreateDefaultInterleaveMap(
    IN HANDLE SubAllocator,
    IN ULONG  OldFileSize,
    IN ULONG  NewFileSize,
    IN ULONG  OptionFlags,
    IN ULONG  MaxWindow
    )
{
    ULONG LzxWindow = LzxMaxWindowSize( OptionFlags, MaxWindow );
    PPATCH_INTERLEAVE_MAP Imap = NULL;

    if (( ! ( OptionFlags & PATCH_OPTION_INTERLEAVE_FILES )) ||
        ( OldFileSize == 0 ) ||
        ( NewFileSize == 0 ))
    {
    return NULL;
    }

    if (( ROUNDUP2( OldFileSize, LZX_BLOCKSIZE ) + NewFileSize ) > LzxWindow )
    {
        ULONG NewChunkSize = ROUNDUP2(( LzxWindow / 4 ), LZX_BLOCKSIZE );
        ULONG OldChunkSize = NewChunkSize * 3;
        ULONG RangeCount   = ( NewFileSize / NewChunkSize ) + 1;
        ULONG AllocSize    = sizeof( *Imap ) + ( RangeCount * sizeof( Imap->Range[ 0 ] ));
        ULONG RangeIndex   = 0;

        Imap = SubAllocate( SubAllocator, AllocSize );

        if ( Imap != NULL )
        {
            ULONG NewRemain = NewFileSize;
            ULONG NewLength = MIN( NewChunkSize, NewFileSize );
            ULONG OldLength = MIN( OldChunkSize, OldFileSize );
            ULONG NewOffset = 0;
            ULONG OldOffset = 0;

            while ( NewRemain > 0 ) {

                ULONG NewMidPoint = NewOffset + ( NewChunkSize / 2 );
                ULONG OldMidPoint = (ULONG)(((UINT64) OldFileSize * NewMidPoint ) / NewFileSize );

                if ( OldMidPoint > ( OldChunkSize / 2 ))
                {
                    OldOffset = OldMidPoint - ( OldChunkSize / 2 );
                }

                if (( OldOffset + OldLength ) > OldFileSize )
                {
                    if ( OldFileSize > OldChunkSize )
                    {
                        OldOffset = OldFileSize - OldChunkSize;
                        OldLength = OldChunkSize;
                    }
                    else
                    {
                        OldOffset = 0;
                        OldLength = OldFileSize;
                    }
                }

                ASSERT( RangeIndex < RangeCount );

                Imap->Range[ RangeIndex ].OldOffset = OldOffset;
                Imap->Range[ RangeIndex ].OldLength = OldLength;
                Imap->Range[ RangeIndex ].NewLength = NewLength;

                NewOffset += NewLength;
                NewRemain -= NewLength;

                if ( NewRemain < NewChunkSize )
                {
                     NewLength = NewRemain;
                }

                RangeIndex++;
            }

            Imap->CountRanges = RangeIndex;

#ifdef TESTCODE

            {
                ULONG TotalNew = 0;
                ULONG i;

                printf( "\n" );

                for ( i = 0; i < Imap->CountRanges; i++ )
                {
                    printf(
                        "Preload  Old: 0x%08X (%u) from offset 0x%08X\n"
                        "Compress New: 0x%08X (%u) from offset 0x%08X\n",
                        Imap->Range[ i ].OldLength,
                        Imap->Range[ i ].OldLength,
                        Imap->Range[ i ].OldOffset,
                        Imap->Range[ i ].NewLength,
                        Imap->Range[ i ].NewLength,
                        TotalNew
                        );

                    TotalNew += Imap->Range[ i ].NewLength;
                }

                printf( "\n" );

                ASSERT( TotalNew == NewFileSize );
            }

#endif /* TESTCODE */

        }
    }
    else
    {
        Imap = SubAllocate( SubAllocator, sizeof( PATCH_INTERLEAVE_MAP ));

        if ( Imap != NULL )
        {
            Imap->CountRanges = 1;
            Imap->Range[ 0 ].OldOffset = 0;
            Imap->Range[ 0 ].OldLength = MIN( OldFileSize, LzxWindow );
            Imap->Range[ 0 ].NewLength = NewFileSize;
        }
    }

    return Imap;
}


ULONG
WINAPI
EstimateLzxCompressionMemoryRequirement(
    IN ULONG OldDataSize,
    IN ULONG NewDataSize,
    IN ULONG OptionFlags,
    IN ULONG AbsoluteMaxLzxWindow
    )
    {
    //
    //  Currently the LZX engine requires 9 times the size of the window
    //  plus a fixed overhead of just under 0x1A0000 bytes (1.7MB).
    //

    ULONG WindowSize = LzxWindowSize(
                           OldDataSize,
                           NewDataSize,
                           OptionFlags,
                           AbsoluteMaxLzxWindow
                           );

    return (( WindowSize * 9 ) + 0x1A0000 );
    }


int
__stdcall
MyLzxOutputCallback(
    PVOID  CallerContext,
    PUCHAR CompressedData,
    LONG   CompressedSize,
    LONG   UncompressedSize
    )
    {
    PLZX_OUTPUT_CONTEXT OutputContext = CallerContext;

    UNREFERENCED_PARAMETER( UncompressedSize );

    OutputContext->PatchSize += CompressedSize + sizeof( USHORT );

    if ( ! OutputContext->DiscardOutput ) {
        if ( OutputContext->PatchSize <= OutputContext->PatchBufferSize ) {
            *(UNALIGNED USHORT *)( OutputContext->PatchBufferPointer ) = (USHORT) CompressedSize;
            memcpy( OutputContext->PatchBufferPointer + sizeof( USHORT ), CompressedData, CompressedSize );
            OutputContext->PatchBufferPointer += CompressedSize + sizeof( USHORT );
            }
        }

    return TRUE;
    }


ULONG
WINAPI
CreateRawLzxPatchDataFromBuffers(
    IN  PVOID    OldDataBuffer,
    IN  ULONG    OldDataSize,
    IN  PVOID    NewDataBuffer,
    IN  ULONG    NewDataSize,
    IN  ULONG    PatchBufferSize,
    OUT PVOID    PatchBuffer,
    OUT ULONG   *PatchSize,
    IN  ULONG    OptionFlags,
    IN  ULONG    AbsoluteMaxWindowSize,
    IN  PPATCH_INTERLEAVE_MAP InterleaveMap,
    IN  PFNALLOC pfnAlloc,
    IN  HANDLE   AllocHandle,
    IN  PPATCH_PROGRESS_CALLBACK ProgressCallback,
    IN  PVOID    CallbackContext,
    IN  ULONG    ProgressInitialValue,
    IN  ULONG    ProgressMaximumValue
    )
    {
    LZX_OUTPUT_CONTEXT OutputContext;
    PATCH_INTERLEAVE_MAP SingleMap;
    PIMAGE_NT_HEADERS NtHeader;
    PVOID  LzxContext;
    ULONG  LzxWindow;
    ULONG  LzxOptE8;
    LONG   LzxStatus;
    PUCHAR PadBuffer;
    PUCHAR OldBlockPointer;
    PUCHAR NewBlockPointer;
    ULONG  NewBlockLength;
    ULONG  NewBytesRemaining;
    ULONG  OddBytes;
    ULONG  OldOffset;
    ULONG  OldLength;
    ULONG  NewLength;
    ULONG  ProgressPosition;
    LONG   Estimate;
    BOOL   Success;
    ULONG  ImapCount;
    ULONG  ImapIndex;

    if ( ! ( OptionFlags & ( PATCH_OPTION_USE_LZX_A | PATCH_OPTION_USE_LZX_B ))) {
        return ERROR_INVALID_PARAMETER;
        }

    LzxWindow = LzxWindowSize( OldDataSize, NewDataSize, OptionFlags, AbsoluteMaxWindowSize );

    if (( InterleaveMap == NULL ) ||
        ( InterleaveMap->CountRanges == 0 )) {

        InterleaveMap = &SingleMap;

        SingleMap.CountRanges = 1;
        SingleMap.Range[ 0 ].OldOffset = 0;
        SingleMap.Range[ 0 ].OldLength = MIN( OldDataSize, LzxWindow );
        SingleMap.Range[ 0 ].NewLength = NewDataSize;
        }

    else {

        //
        //  Verify specified interleave map is valid for LZX:
        //
        //    Each OldLength value non-zero and multiple of LZX_BLOCKSIZE.
        //    Each NewLength value non-zero and multiple of LZX_BLOCKSIZE,
        //      except for last NewLength which is ignored.
        //    Each OldOffset/OldLength pair does not exceed OldDataSize.
        //    Sum of Count-1 NewLength values does not exceed NewDataSize.
        //
        //

        ULONG TotalNew = 0;

        ImapCount = InterleaveMap->CountRanges;

        for ( ImapIndex = 0; ImapIndex < ImapCount; ImapIndex++ ) {

            OldOffset = InterleaveMap->Range[ ImapIndex ].OldOffset;
            OldLength = InterleaveMap->Range[ ImapIndex ].OldLength;
            NewLength = InterleaveMap->Range[ ImapIndex ].NewLength;

            if ( ImapIndex == ( ImapCount - 1 )) {
                NewLength = NewDataSize - TotalNew;
                }

            TotalNew += NewLength;

            if ((( TotalNew > NewDataSize ))               ||
                (( OldOffset + OldLength ) > OldDataSize ) ||
                (( OldLength == 0 ))                       ||
                (( NewLength == 0 ))                       ||
                (( NewLength % LZX_BLOCKSIZE ) &&
                 ( TotalNew != NewDataSize ))) {

                return ERROR_INVALID_PARAMETER;
                }
            }
        }

    //
    //  If OptionFlags requests LZX E8 transformation, and
    //     If file has MZ signature AND it's NOT a PE image,
    //     OR it's a PE image AND it's an x86 image,
    //       turn on x86-specific E8 call translation optimization.
    //

    NtHeader = GetNtHeader( NewDataBuffer, NewDataSize );

    if (( OptionFlags & PATCH_OPTION_USE_LZX_B ) &&
        ((( NtHeader ) && ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 )) ||
         (( ! NtHeader ) && ( *(UNALIGNED USHORT *)NewDataBuffer == 0x5A4D )))) {

        LzxOptE8 = NewDataSize;
        }
    else {
        LzxOptE8 = 0;
        }

    OutputContext.DiscardOutput      = TRUE;
    OutputContext.PatchBufferSize    = PatchBufferSize;
    OutputContext.PatchBufferPointer = PatchBuffer;
    OutputContext.PatchSize          = 0;

    Success = LZX_EncodeInit(
                  &LzxContext,
                  LzxWindow,
                  LZX_BLOCKSIZE,
                  pfnAlloc,
                  AllocHandle,
                  MyLzxOutputCallback,
                  &OutputContext
                  );

    PadBuffer = pfnAlloc( AllocHandle, LZX_BLOCKSIZE );

    if (( ! Success ) || ( PadBuffer == NULL )) {
        return ERROR_OUTOFMEMORY;
        }

    ProgressPosition = ProgressInitialValue;

    NewBlockPointer   = NewDataBuffer;
    NewBytesRemaining = NewDataSize;

    ImapCount = InterleaveMap->CountRanges;

    for ( ImapIndex = 0; ImapIndex < ImapCount; ImapIndex++ ) {

        OldOffset = InterleaveMap->Range[ ImapIndex ].OldOffset;
        OldLength = InterleaveMap->Range[ ImapIndex ].OldLength;
        NewLength = InterleaveMap->Range[ ImapIndex ].NewLength;

        if ( ImapIndex == ( ImapCount - 1 )) {
            NewLength = NewBytesRemaining;      // ignore final Imap NewLength
            }

        ASSERT( NewLength <= NewBytesRemaining );

        ASSERT(( OldOffset + OldLength ) <= OldDataSize );

#ifdef TESTCODE

        if ( ImapCount > 1 ) {

            printf(
                "\nLZX inserting   0x%08X bytes from old file at offset 0x%08X\n",
                OldLength,
                OldOffset
                );
            }
#endif

        OutputContext.DiscardOutput = TRUE;

        if ( ImapIndex > 0 ) {
            LZX_EncodeNewGroup( LzxContext );
            }

        OldBlockPointer = (PUCHAR)OldDataBuffer + OldOffset;

        OddBytes = OldLength % LZX_BLOCKSIZE;

#ifdef TRACING
        EncTracingDefineOffsets(
            LzxWindow,
            OddBytes ? (LZX_BLOCKSIZE - OddBytes) : 0,
            OddBytes ? (OldBytesRemaining + LZX_BLOCKSIZE - OddBytes) : OldLength
            );
#endif

        if ( OddBytes ) {

            //
            //  We must insert a multiple of LZX_BLOCKSIZE (32K) of old
            //  data.  If we have OddBytes, we pre-fill a 32K buffer with
            //  zeros and copy the first OddBytes to the tail of the 32K
            //  buffer.  By dealing with the odd bytes in the first 32K
            //  of inserted data, the last block of inserted data will come
            //  out exactly a 32K boundary (adjacent to following new data).
            //

            ZeroMemory( PadBuffer, LZX_BLOCKSIZE - OddBytes );

            memcpy(
                PadBuffer + ( LZX_BLOCKSIZE - OddBytes ),
                OldBlockPointer,
                OddBytes
                );

            Success = LZX_EncodeInsertDictionary(
                          LzxContext,
                          PadBuffer,
                          LZX_BLOCKSIZE
                          );

            if ( ! Success ) {
                return ERROR_INVALID_DATA;  // don't know how could fail
                }

            OldLength        -= OddBytes;
            OldBlockPointer  += OddBytes;
            ProgressPosition += OddBytes;

            Success = ProgressCallbackWrapper(
                          ProgressCallback,
                          CallbackContext,
                          ProgressPosition,
                          ProgressMaximumValue
                          );

            if ( ! Success ) {
                return GetLastError();      // guaranteed non-zero by wrapper
                }
            }

        //
        //  Now insert remaining 32K packets of old data.
        //

        ASSERT(( OldLength % LZX_BLOCKSIZE ) == 0 );

        while ( OldLength > 0 ) {

            Success = LZX_EncodeInsertDictionary(
                          LzxContext,
                          OldBlockPointer,
                          LZX_BLOCKSIZE
                          );

            if ( ! Success ) {
                return ERROR_INVALID_DATA;  // don't know how could fail
                }

            OldLength        -= LZX_BLOCKSIZE;
            OldBlockPointer  += LZX_BLOCKSIZE;
            ProgressPosition += LZX_BLOCKSIZE;

            Success = ProgressCallbackWrapper(
                          ProgressCallback,
                          CallbackContext,
                          ProgressPosition,
                          ProgressMaximumValue
                          );

            if ( ! Success ) {
                return GetLastError();      // guaranteed non-zero by wrapper
                }
            }

        LZX_EncodeResetState( LzxContext );

        OutputContext.DiscardOutput = FALSE;

#ifdef TESTCODE

        if ( ImapCount > 1 ) {

            printf(
                "\nLZX compressing 0x%08X bytes from new file at offset 0x%08X\n",
                NewLength,
                NewBlockPointer - (PUCHAR) NewDataBuffer
                );
            }
#endif

        while ( NewLength > 0 ) {

            NewBlockLength = MIN( NewLength, LZX_BLOCKSIZE );

            LzxStatus = LZX_Encode(
                            LzxContext,
                            NewBlockPointer,
                            NewBlockLength,
                            &Estimate,
                            LzxOptE8
                            );

            if ( LzxStatus != ENCODER_SUCCESS ) {
                return ERROR_INVALID_DATA;      // don't know how could fail
                }

            NewBytesRemaining -= NewBlockLength;
            NewLength         -= NewBlockLength;
            NewBlockPointer   += NewBlockLength;
            ProgressPosition  += NewBlockLength;

            Success = ProgressCallbackWrapper(
                          ProgressCallback,
                          CallbackContext,
                          ProgressPosition,
                          ProgressMaximumValue
                          );

            if ( ! Success ) {
                return GetLastError();      // guaranteed non-zero by wrapper
                }
            }

        Success = LZX_EncodeFlush( LzxContext );

        if ( ! Success ) {
            return ERROR_INVALID_DATA;      // don't know how could fail
            }
        }

    *PatchSize = OutputContext.PatchSize;

    if ( OutputContext.PatchSize > OutputContext.PatchBufferSize ) {
        return ERROR_INSUFFICIENT_BUFFER;
        }

    return NO_ERROR;
    }



ULONG
WINAPI
RawLzxCompressBuffer(
    IN  PVOID    InDataBuffer,
    IN  ULONG    InDataSize,
    IN  ULONG    OutDataBufferSize,
    OUT PVOID    OutDataBuffer OPTIONAL,
    OUT PULONG   OutDataSize,
    IN  ULONG    MaxWindowSize OPTIONAL,
    IN  PFNALLOC pfnAlloc,
    IN  HANDLE   AllocHandle,
    IN  PPATCH_PROGRESS_CALLBACK ProgressCallback,
    IN  PVOID    CallbackContext,
    IN  ULONG    ProgressInitialValue,
    IN  ULONG    ProgressMaximumValue
    )
    {
    LZX_OUTPUT_CONTEXT OutputContext;
    PIMAGE_NT_HEADERS NtHeader;
    ULONG  ProgressPosition;
    PVOID  LzxContext;
    ULONG  LzxWindow;
    ULONG  LzxOptE8;
    LONG   LzxStatus;
    PUCHAR BlockPointer;
    ULONG  BytesRemaining;
    LONG   Estimate;
    BOOL   Success;
    ULONG  ErrorCode;

    if ( OutDataBufferSize == 0 ) {
         OutDataBuffer = NULL;
         }
    else if ( OutDataBuffer == NULL ) {
         OutDataBufferSize = 0;
         }

    ErrorCode = ERROR_OUTOFMEMORY;

    OutputContext.DiscardOutput      = OutDataBuffer ? FALSE : TRUE;
    OutputContext.PatchBufferSize    = OutDataBufferSize;
    OutputContext.PatchBufferPointer = OutDataBuffer;
    OutputContext.PatchSize          = 0;

    LzxWindow = LzxWindowSize( 0, InDataSize, LZX_MAXWINDOW_32, MaxWindowSize );

    Success = LZX_EncodeInit(
                  &LzxContext,
                  LzxWindow,
                  LZX_BLOCKSIZE,
                  pfnAlloc,
                  AllocHandle,
                  MyLzxOutputCallback,
                  &OutputContext
                  );

    if ( Success ) {

        LzxOptE8 = 0;
        NtHeader = GetNtHeader( InDataBuffer, InDataSize );

        //
        //  If file has MZ signature AND it's NOT a PE image,
        //  OR it's a PE image AND it's an i386 image, turn on
        //  the i386-specific E8 call translation optimization.
        //

        if ((( NtHeader ) && ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 )) ||
            (( ! NtHeader ) && ( *(UNALIGNED USHORT *)InDataBuffer == 0x5A4D ))) {

            LzxOptE8 = InDataSize;
            }

        ProgressPosition = ProgressInitialValue;
        ErrorCode        = ERROR_PATCH_ENCODE_FAILURE;
        BlockPointer     = InDataBuffer;
        BytesRemaining   = InDataSize;
        LzxStatus        = ENCODER_SUCCESS;
        Success          = TRUE;

        while (( BytesRemaining >= LZX_BLOCKSIZE ) && ( LzxStatus == ENCODER_SUCCESS ) && ( Success )) {

            LzxStatus = LZX_Encode(
                            LzxContext,
                            BlockPointer,
                            LZX_BLOCKSIZE,
                            &Estimate,
                            LzxOptE8
                            );

            if ( LzxStatus == ENCODER_SUCCESS ) {

                ProgressPosition += LZX_BLOCKSIZE;

                Success = ProgressCallbackWrapper(
                              ProgressCallback,
                              CallbackContext,
                              ProgressPosition,
                              ProgressMaximumValue
                              );

                if ( ! Success ) {
                    ErrorCode = GetLastError();
                    }
                }

            BlockPointer   += LZX_BLOCKSIZE;
            BytesRemaining -= LZX_BLOCKSIZE;
            }

        if (( BytesRemaining ) && ( LzxStatus == ENCODER_SUCCESS ) && ( Success )) {

            LzxStatus = LZX_Encode(
                            LzxContext,
                            BlockPointer,
                            BytesRemaining,
                            &Estimate,
                            LzxOptE8
                            );

            if ( LzxStatus == ENCODER_SUCCESS ) {

                ProgressPosition += BytesRemaining;

                Success = ProgressCallbackWrapper(
                              ProgressCallback,
                              CallbackContext,
                              ProgressPosition,
                              ProgressMaximumValue
                              );

                if ( ! Success ) {
                    ErrorCode = GetLastError();
                    }
                }
            }

        if (( LzxStatus == ENCODER_SUCCESS ) && ( Success )) {

            Success = LZX_EncodeFlush( LzxContext );

            if ( Success ) {

                if ( OutDataSize ) {
                    *OutDataSize = OutputContext.PatchSize;
                    }

                if (( OutDataBufferSize ) && ( OutputContext.PatchSize > OutDataBufferSize )) {
                    ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                    }
                else {
                    ErrorCode = NO_ERROR;
                    }
                }
            }
        }

    return ErrorCode;
    }

#endif // ! PATCH_APPLY_CODE_ONLY


//
//  Following group of functions and exported apis are exclusively for
//  applying patches.  If we're only compiling the create code, ignore
//  this group of functions.
//

#ifndef PATCH_CREATE_CODE_ONLY


ULONG
WINAPI
EstimateLzxDecompressionMemoryRequirement(
    IN ULONG OldDataSize,
    IN ULONG NewDataSize,
    IN ULONG OptionFlags,
    IN ULONG WindowSize
    )
    {
    if ( WindowSize == 0 )
    {
         WindowSize = LzxWindowSize( OldDataSize, NewDataSize, OptionFlags, 0 );
    }

    //
    //  Currently the LZX decompression engine requires the size of the
    //  window plus some slop and the size of the context.  We'll add 64K
    //  to cover the context size and slop.
    //

    return ( WindowSize + 0x10000 );
    }


ULONG
WINAPI
ApplyRawLzxPatchToBuffer(
    IN  PVOID    OldDataBuffer,
    IN  ULONG    OldDataSize,
    IN  PVOID    PatchDataBuffer,
    IN  ULONG    PatchDataSize,
    OUT PVOID    NewDataBuffer,
    IN  ULONG    NewDataSize,
    IN  ULONG    OptionFlags,
    IN  ULONG    WindowSize,
    IN  PPATCH_INTERLEAVE_MAP InterleaveMap,
    IN  PFNALLOC pfnAlloc,
    IN  HANDLE   AllocHandle,
    IN  PPATCH_PROGRESS_CALLBACK ProgressCallback,
    IN  PVOID    CallbackContext,
    IN  ULONG    ProgressInitialValue,
    IN  ULONG    ProgressMaximumValue
    )
    {
    PPATCH_INTERLEAVE_MAP Imap        = InterleaveMap;
    PUCHAR CompressedInputPointer     = PatchDataBuffer;
    PUCHAR CompressedInputExtent      = CompressedInputPointer + PatchDataSize;
    PUCHAR UncompressedOutputPointer  = NewDataBuffer;
    ULONG  ProgressPosition           = ProgressInitialValue;

    PATCH_INTERLEAVE_MAP SingleMap;
    ULONG  UncompBlockLength;
    ULONG  CompressedLength;
    LONG   ActualSize;
    PUCHAR OldPointer;
    ULONG  OldOffset;
    ULONG  OldLength;
    ULONG  NewLength;
    ULONG  ImapIndex;
    ULONG  ImapCount;
    PVOID  LzxContext;
    ULONG  LzxWindow;
    LONG   LzxStatus;
    BOOL   Success;

    if ( ! ( OptionFlags & ( PATCH_OPTION_USE_LZX_A | PATCH_OPTION_USE_LZX_B ))) {
        return ERROR_INVALID_PARAMETER;
        }

    if ( WindowSize ) {
        LzxWindow = WindowSize;
        }
    else {
        LzxWindow = LzxWindowSize( OldDataSize, NewDataSize, OptionFlags, 0 );
        }

    if ( Imap == NULL ) {

        SingleMap.CountRanges = 1;
        SingleMap.Range[ 0 ].OldOffset = 0;
        SingleMap.Range[ 0 ].OldLength = MIN( OldDataSize, LzxWindow );
        SingleMap.Range[ 0 ].NewLength = NewDataSize;

        Imap = &SingleMap;
        }

#ifdef TESTCODE

    if (( OptionFlags & PATCH_OPTION_INTERLEAVE_FILES ) &&
        ( InterleaveMap != NULL ) &&
        ( InterleaveMap->CountRanges > 1 )) {

        ULONG TotalNew = 0;
        ULONG i;

        printf( "\n" );

        for ( i = 0; i < Imap->CountRanges; i++ ) {

            printf(
                "Preload  Old: 0x%08X (%u) from offset 0x%08X\n"
                "Decomp   New: 0x%08X (%u) from offset 0x%08X\n",
                Imap->Range[ i ].OldLength,
                Imap->Range[ i ].OldLength,
                Imap->Range[ i ].OldOffset,
                Imap->Range[ i ].NewLength,
                Imap->Range[ i ].NewLength,
                TotalNew
                );

            TotalNew += Imap->Range[ i ].NewLength;
            }

        ASSERT( TotalNew == NewDataSize );

        printf( "\n" );
        }

#endif /* TESTCODE */

    Success = LZX_DecodeInit(
                  &LzxContext,
                  LzxWindow,
                  pfnAlloc,
                  AllocHandle
                  );

    if ( ! Success ) {
        return ERROR_OUTOFMEMORY;
        }

    ImapCount = Imap->CountRanges;

    for ( ImapIndex = 0; ImapIndex < ImapCount; ImapIndex++ ) {

        OldOffset = Imap->Range[ ImapIndex ].OldOffset;
        OldLength = Imap->Range[ ImapIndex ].OldLength;
        NewLength = Imap->Range[ ImapIndex ].NewLength;

        ASSERT(( OldOffset + OldLength ) <= OldDataSize );
        ASSERT( OldLength <= LzxWindow );

        if ( ImapIndex > 0 ) {
            LZX_DecodeNewGroup( LzxContext );
            }

        OldPointer = (PUCHAR)OldDataBuffer + OldOffset;

        Success = LZX_DecodeInsertDictionary(
                      LzxContext,
                      OldPointer,
                      OldLength
                      );

        if ( ! Success ) {
            return ERROR_PATCH_DECODE_FAILURE;
            }

        while ( NewLength > 0 ) {

            CompressedLength = *(UNALIGNED USHORT *)( CompressedInputPointer );

            CompressedInputPointer += sizeof( USHORT );

            if (( CompressedLength == 0 ) ||
                (( CompressedInputPointer + CompressedLength ) > CompressedInputExtent )) {

                return ERROR_PATCH_DECODE_FAILURE;
                }

            UncompBlockLength = MIN( NewLength, LZX_BLOCKSIZE );

            LzxStatus = LZX_Decode(
                            LzxContext,
                            UncompBlockLength,
                            CompressedInputPointer,
                            CompressedLength,
                            UncompressedOutputPointer,
                            UncompBlockLength,
                            &ActualSize
                            );

            if (( LzxStatus != 0 ) || ((ULONG)ActualSize != UncompBlockLength )) {
                return ERROR_PATCH_DECODE_FAILURE;
                }

            CompressedInputPointer     += CompressedLength;
            UncompressedOutputPointer  += UncompBlockLength;
            NewLength                  -= UncompBlockLength;
            ProgressPosition           += UncompBlockLength;

            Success = ProgressCallbackWrapper(
                          ProgressCallback,
                          CallbackContext,
                          ProgressPosition,
                          ProgressMaximumValue
                          );

            if ( ! Success ) {
                return GetLastError();  // guaranteed nonzero by wrapper
                }
            }
        }

    return NO_ERROR;
    }


#endif // ! PATCH_CREATE_CODE_ONLY




