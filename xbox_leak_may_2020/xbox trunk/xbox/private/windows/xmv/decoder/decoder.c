/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       decoder.c
 *  Content:    manages loading and parsering XMV data.
 *
 ****************************************************************************/

#include <xtl.h>
#include <xdbg.h>
#include <xmv.h>

#include "..\inc\xmvformat.h"

#include "decoder.h"

#define MAX_BUFFER_SIZE     (512 * 1024)

/*
 * Create the decoder, load the headers from the movie file and create
 * all of the video decoding buffers.
 */

HRESULT XMVCreateDecoder
(
    char *szFileName, 
    XMVDecoder **ppDecoder
)
{
    LONGLONG FileSize;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    BYTE *pBuffer = NULL;
    BYTE *pFrame  = NULL;

    DWORD BytesRead;
    OVERLAPPED Overlapped;
    DWORD Size, TotalSize;
    DWORD AudioStreamCount;
    DWORD i;
    BYTE *pWalk;

    XMVVideoDataHeader *pVideoDataHeader;
    XMVAudioDataHeader *pAudioDataHeader;
    XMVDecoder         *pDecoder = NULL;

    HRESULT hr;

    memset(&Overlapped, 0, sizeof(Overlapped));

    // Open the input file.
    hFile = CreateFile(szFileName, 
                       GENERIC_READ, 
                       FILE_SHARE_READ, 
                       NULL, 
                       OPEN_EXISTING, 
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        goto LastError;
    }

    // Remember the file's size.
    if (!GetFileSizeEx(hFile, (LARGE_INTEGER *)&FileSize))
    {
        goto LastError;
    }

    // Reserve enough address space for our maximum buffer sizes.
    pBuffer = (BYTE *)VirtualAlloc(NULL, MAX_BUFFER_SIZE * 2, MEM_RESERVE, PAGE_READWRITE);

    if (!pBuffer)
    {
        goto LastError;
    }

    // Commit the first page.
    if (!VirtualAlloc(pBuffer, 4096, MEM_COMMIT, PAGE_READWRITE))
    {
        goto LastError;
    }

    // Read the first page from the file.  This is a royal pain in the neck
    // because we opened it for overlapped IO.  We'll take the pain for this
    // pair of reads.
    //
    Overlapped.Offset     = 0;
    Overlapped.OffsetHigh = 0;

    if (!ReadFile(hFile, pBuffer, 4096, NULL, &Overlapped))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            goto LastError;
        }
    }

    // Wait for the read to finish.
    if (!GetOverlappedResult(hFile, &Overlapped, &BytesRead, TRUE))
    {
        goto LastError;
    }

    // Find the location of the header.
    pVideoDataHeader = (XMVVideoDataHeader *)pBuffer;

    // Make sure it's valid.

#if DBG

    if (BytesRead != 4096 
        || pVideoDataHeader->MagicCookie != XMV_MAGIC_COOKIE 
        || pVideoDataHeader->FileVersion != XMV_FILE_VERSION)
    {
        RIP("Input is not a valid XMV file.\n");
    }
    
#endif DBG

    // Commit all of the memory we need for the input buffers.
    if (!VirtualAlloc(pBuffer + 4096, 
                      pVideoDataHeader->RequiredBufferSize * 2 - 4096, 
                      MEM_COMMIT, 
                      PAGE_READWRITE))
    {
        goto LastError;
    }

    // Check for a buffer overrun.
    if (pVideoDataHeader->ThisPacketSize > pVideoDataHeader->RequiredBufferSize)
    {
        RIP("Bad input file - buffer overrun.");
        hr = E_FAIL;

        goto Error;
    }

    TotalSize = pVideoDataHeader->RequiredBufferSize * 2;

    // Allocate our decoder object, zero it out then populate its members.
    AudioStreamCount = pVideoDataHeader->AudioStreamCount;

    Size = sizeof(*pDecoder) 
            + sizeof(XMVAudioImplementation) * AudioStreamCount
            + sizeof(XMVMacroblockCBPCY) * (pVideoDataHeader->Width / MACROBLOCK_SIZE + 1)
            + sizeof(short) * (pVideoDataHeader->Width / MACROBLOCK_SIZE) * BLOCK_SIZE * 4;

    TotalSize += Size;

    pDecoder = malloc(Size);

    if (!pDecoder)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    memset(pDecoder, 0, Size);

    // Buffer members.
    pDecoder->RemainingFrames  = 0;
    pDecoder->FileSize         = FileSize;
    pDecoder->FileSizeRead     = 4096;
    pDecoder->hFile            = hFile;;
    pDecoder->BufferSize       = pVideoDataHeader->RequiredBufferSize;

    pDecoder->pDecodingBuffer  = pBuffer + pDecoder->BufferSize;
    pDecoder->pLoadingBuffer   = pBuffer;

    // Decoder information.
    pDecoder->Width            = pVideoDataHeader->Width;
    pDecoder->Height           = pVideoDataHeader->Height;
    pDecoder->FramesPerSecond  = pVideoDataHeader->FramesPerSecond;
    pDecoder->Slices           = pVideoDataHeader->Slices;

    // Flags
    pDecoder->MixedPelMotionCompensationEnable = pVideoDataHeader->MixedPelMotionCompensationEnable; 
    pDecoder->LoopFilterEnabled                = pVideoDataHeader->LoopFilterEnabled; 
    pDecoder->VariableSizedTransformEnabled    = pVideoDataHeader->VariableSizedTransformEnabled;
    pDecoder->XIntra8IPictureCodingEnabled     = pVideoDataHeader->XIntra8IPictureCodingEnabled;
    pDecoder->HybridMotionVectorEnabled        = pVideoDataHeader->HybridMotionVectorEnabled;
    pDecoder->DCTTableSwitchingEnabled         = pVideoDataHeader->DCTTableSwitchingEnabled;

    // Audio stream information.
    pDecoder->AudioStreamCount = AudioStreamCount;

    if (AudioStreamCount)
    {
        pDecoder->Audio        = (XMVAudioImplementation *)(pDecoder + 1);

        pAudioDataHeader = pVideoDataHeader->AudioHeaders;

        for (i = 0; i < AudioStreamCount; i++)
        {
            pDecoder->Audio[i].WaveFormat       = pAudioDataHeader->WaveFormat;
            pDecoder->Audio[i].ChannelCount     = pAudioDataHeader->ChannelCount;
            pDecoder->Audio[i].SamplesPerSecond = pAudioDataHeader->SamplesPerSecond;
            pDecoder->Audio[i].BitsPerSample    = pAudioDataHeader->BitsPerSample;

            pAudioDataHeader++;
        }
    }

    // Save the size of the header.
    pDecoder->InitialFrameHeaderSize = (BYTE *)(pVideoDataHeader->AudioHeaders + AudioStreamCount) - pBuffer;

    // Initialize the frame buffers.
    pDecoder->UVWidth  = pDecoder->Width / 2;
    pDecoder->UVHeight = pDecoder->Height / 2;

    pDecoder->MBWidth  = pDecoder->Width / MACROBLOCK_SIZE;
    pDecoder->MBHeight = pDecoder->Height / MACROBLOCK_SIZE;

    // Set up the CBPCY fields.
    pWalk = (BYTE *)(pDecoder + 1);

    pDecoder->pCBPCY = (XMVMacroblockCBPCY *)pWalk;
    pWalk += sizeof(XMVMacroblockCBPCY) * (pDecoder->MBWidth  + 1);

    pDecoder->pYAC = (short *)pWalk;
    pWalk += sizeof(short) * pDecoder->MBWidth * MACROBLOCK_SIZE;

    pDecoder->pUAC = (short *)pWalk;
    pWalk += sizeof(short) * pDecoder->MBWidth * BLOCK_SIZE;

    pDecoder->pVAC = (short *)pWalk;
    pWalk += sizeof(short) * pDecoder->MBWidth * BLOCK_SIZE;

    // Initialize the memory for the frame buffers.  These are almost always
    // big enough to use Virtual Alloc so just use that...
    //
    Size = 2 * (pDecoder->Width * pDecoder->Height + 2 * pDecoder->UVWidth * pDecoder->UVHeight);

    TotalSize += Size;

    pFrame = VirtualAlloc(NULL, Size, MEM_COMMIT, PAGE_READWRITE);
    pWalk  = pFrame;

    // The frame buffer currently being displayed.
    pDecoder->pYDisplayed = pWalk;
    pWalk += pDecoder->Width * pDecoder->Height;

    pDecoder->pUDisplayed = pWalk;
    pWalk += pDecoder->UVWidth * pDecoder->UVHeight;

    pDecoder->pVDisplayed = pWalk;
    pWalk += pDecoder->UVWidth * pDecoder->UVHeight;

    // Set the U & V buffers so that we'll display black if this frame ever
    // gets presented.
    //
    memset(pDecoder->pUDisplayed, 0x80, pDecoder->UVWidth * pDecoder->UVHeight);
    memset(pDecoder->pVDisplayed, 0x80, pDecoder->UVWidth * pDecoder->UVHeight);

    // The frame buffer currently being built.
    pDecoder->pYBuilding = pWalk;
    pWalk += pDecoder->Width * pDecoder->Height;

    pDecoder->pUBuilding = pWalk;
    pWalk += pDecoder->UVWidth * pDecoder->UVHeight;

    pDecoder->pVBuilding = pWalk;
    pWalk += pDecoder->UVWidth * pDecoder->UVHeight;

// TEMPORARY UNTIL WE START ACTUALLY DECODING.
memset(pDecoder->pUBuilding, 0x80, pDecoder->UVWidth * pDecoder->UVHeight);
memset(pDecoder->pVBuilding, 0x80, pDecoder->UVWidth * pDecoder->UVHeight);

    // Read in the rest of the buffer while we finish our initialization.  The
    // header will always fit in the first 4096 bytes of the file so we can
    // party on it all we need.
    //
    pDecoder->Overlapped.Offset     = 4096;
    pDecoder->Overlapped.OffsetHigh = 0;

    if (!ReadFile(hFile, 
                  pBuffer + 4096, 
                  pVideoDataHeader->ThisPacketSize - 4096, 
                  NULL, 
                  &pDecoder->Overlapped))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            goto LastError;
        }
    }

    *ppDecoder = pDecoder;

#if DBG
    DbgPrint("XMV: Total allocated memory: %d bytes (%d K)\n", TotalSize, TotalSize / 1024);
#endif DBG

    return NOERROR;

LastError:
    hr = HRESULT_FROM_WIN32(GetLastError());

Error:

    if (pDecoder)
    {
        free(pDecoder);
    }

    if (hFile != INVALID_HANDLE_VALUE)
    {
        // Make sure any pending reads are done.
        GetOverlappedResult(hFile, &Overlapped, &BytesRead, TRUE);

        CloseHandle(hFile);
    }

    if (pBuffer)
    {
        VirtualFree(pBuffer, 0, MEM_RELEASE);
    }

    if (pFrame)
    {
        VirtualFree(pFrame, 0, MEM_RELEASE);
    }

    return hr;
}

/*
 * Destroy decoder, freeing all memory and closing the input file.
 */

void XMVCloseDecoder
(
    XMVDecoder *pDecoder
)
{
    BYTE *pBuffer;
    DWORD BytesRead;

    if (pDecoder)
    {
        // UNDONE: Need to wait on any pending reads...
        if (pDecoder->hFile != INVALID_HANDLE_VALUE)
        {
            // Make sure any pending reads are done.
            GetOverlappedResult(pDecoder->hFile, &pDecoder->Overlapped, &BytesRead, TRUE);

            CloseHandle(pDecoder->hFile);
        }

        if (pDecoder->pDecodingBuffer < pDecoder->pLoadingBuffer)
        {
            pBuffer = pDecoder->pDecodingBuffer;
        }
        else
        {
            pBuffer = pDecoder->pLoadingBuffer;
        }

        if (pBuffer)
        {
            VirtualFree(pBuffer, 0, MEM_RELEASE);
        }

        if (pDecoder->pYDisplayed < pDecoder->pYBuilding)
        {
            pBuffer = pDecoder->pYDisplayed;
        }
        else
        {
            pBuffer = pDecoder->pYBuilding;
        }

        if (pBuffer)
        {
            VirtualFree(pBuffer, 0, MEM_RELEASE);
        }

        free(pDecoder);
    }
}

/*
 * Get the general information about the file.
 */

void XMVGetVideoDescriptor
(
    XMVDecoder *pDecoder, 
    XMVVideoDescriptor *pVideoDescriptor
)
{
    pVideoDescriptor->Width            = pDecoder->Width;
    pVideoDescriptor->Height           = pDecoder->Height;
    pVideoDescriptor->FramesPerSecond  = pDecoder->FramesPerSecond;
    pVideoDescriptor->AudioStreamCount = pDecoder->AudioStreamCount;
}

/*
 * Get information about a specific audio stream.
 */

void XMVGetAudioDescriptor
(
    XMVDecoder *pDecoder, 
    DWORD AudioStream, 
    XMVAudioDescriptor *pAudioDescriptor
)
{
    if (AudioStream >= pDecoder->AudioStreamCount)
    {
        RIP("Invalid audio stream index.");
    }

    pAudioDescriptor->WaveFormat       = pDecoder->Audio[AudioStream].WaveFormat;
    pAudioDescriptor->ChannelCount     = pDecoder->Audio[AudioStream].ChannelCount;
    pAudioDescriptor->SamplesPerSecond = pDecoder->Audio[AudioStream].SamplesPerSecond;
    pAudioDescriptor->BitsPerSample    = pDecoder->Audio[AudioStream].BitsPerSample;
}

/*
 * Tell the decoder to play one of the audio tracks in the XMV file.  The 
 * caller can optionally get the IDirectSoundStream interface to manually
 * adjust the sound parameters.
 *
 * This API can be called multiple times for different streams to allow
 * them to be played at the same time.
 *
 * The Flags and pMixBins parameters are used when creating the stream.  
 * These can be safely set to zero and NULL, respectively.
 *
 * ppStream may be NULL if the caller does not want direct access to the
 * stream.
 */

HRESULT XMVEnableAudioStream
(
    XMVDecoder *pDecoder, 
    DWORD AudioStream, 
    DWORD Flags, 
    DSMIXBINS *pMixBins, 
    IDirectSoundStream **ppStream
)
{
    RIP("Audio is NYI");
    
    return E_NOTIMPL;
}

/*
 * Get the next frame to display.
 *
 * This method has four possible return values:
 *
 *   XMV_NOFRAME - There is no new data and the passed-in surface
 *       is unchanged.
 *
 *   XMV_NEWFRAME - The surface contains a new frame that needs to be
 *       displayed.
 *
 *   XMV_ENDOFILE - The movie is over, throw your trash away and go home.
 *
 *   XMV_FAIL - Some major catastrophy happened that prevents the video from
 *       being decoded any further.  This only happens with major data 
 *       corruption or some kind of read error.
 *
 * The Width and Height of the surface must match that of the video.
 *
 * This method must be called occasionally even if there is no video in the
 * input file, simply pass NULL for the surface.
 */

XMVRESULT XMVGetNextFrame
(
    XMVDecoder *pDecoder, 
    D3DSurface *pSurface
)
{
    DWORD BytesRead;
    DWORD Error;
    BYTE *pBuffer, *pSwap;
    DWORD i;

    BYTE *pStartFrame;
    DWORD FrameSize;

    XMVVideoPacketHeader *pVideoPacketHeader;

    /*
     * The decoder works as follows...
     *
     *  We always try to keep the frame that we're currently displaying
     *  on the screen and the next frame we want to display in memory.
     *
     *  When a new frame is displayed, we swap it to the current frame
     *  slot and mark the next frame as being dirty.
     *
     *  We load and decode that next frame at the very next call to this
     *  routine.  If we are unable to, such as the IO is not yet complete,
     *  then we leave it marked as dirty.
     *
     *  If the next frame is not ready when we need to display it then we 
     *  continue displaying the old frame until it is ready, then we will
     *  use the sychronization logic to skip back up to the correct 
     *  frame.
     */

    // Do we need to decode the next frame?
    if (!pDecoder->IsNextFrameDecoded)
    {
        // If we're out of frames, get the ones we just loaded in the
        // background and start a new set loading.
        //
        if (!pDecoder->RemainingFrames)
        {
            // Are we done?
            if (pDecoder->FileSizeRead == pDecoder->FileSize)
            {
                // If the audio is not done then don't report the EOF yet.
                if (/* Is Audio done? */ FALSE)
                {
                    return XMV_NOFRAME;
                }
                else
                {
                    return XMV_ENDOFFILE;
                }
            }

            // Check to see if the read is done....
            if (GetOverlappedResult(pDecoder->hFile, &pDecoder->Overlapped, &BytesRead, FALSE))
            {
                // Remember how much we read.
                pDecoder->FileSizeRead += BytesRead;

                // Swap the buffers.
                pBuffer = pDecoder->pLoadingBuffer;
                pDecoder->pLoadingBuffer = pDecoder->pDecodingBuffer;
                pDecoder->pDecodingBuffer = pBuffer;

                // Skip the initial header.
                pBuffer += pDecoder->InitialFrameHeaderSize;
                pDecoder->InitialFrameHeaderSize = 0;

                // Save the next packet size.
                pDecoder->NextPacketSize = ((XMVPacketHeader *)pBuffer)->NextPacketSize;

                pBuffer += sizeof(XMVPacketHeader);
                                         
                // Load the headers.
                pVideoPacketHeader = (XMVVideoPacketHeader *)pBuffer;

                pBuffer += sizeof(pVideoPacketHeader) 
                             + sizeof(XMVAudioStreamHeader) * pDecoder->AudioStreamCount;

                // Save our position in the buffer.
                pDecoder->pDecodingPosition = pBuffer;

                // We now have frames!
                pDecoder->RemainingFrames = pVideoPacketHeader->FrameCount;

                // Pump more data into the audio streams.
                pBuffer += pVideoPacketHeader->VideoStreamLength;

                for (i = 0; i < pDecoder->AudioStreamCount; i++)
                {
                    ASSERT((DWORD)pBuffer % 4 == 0);

                    // PumpMoreAudio(pBuffer, pDecoder->AudioStreams[i].AudioStreamSize);

                    pBuffer += (pVideoPacketHeader->AudioStreams[i].AudioStreamSize + 3) & ~3;
                }
            }

            // Error case.
            else
            {
                Error = GetLastError();

                // If we're waiting on the IO to finish, just do nothing.
                if (Error != ERROR_IO_INCOMPLETE)
                {
                    RIP("Unable to read from the input file.");
                    return XMV_FAIL;
                }
            }

        }
    }

    // Start the next load if needed and if we're done with that buffer.  We
    // do this before we decode the frame as the time we're doing that is an
    // optimal time to be loading stuff off the disk.
    //
    if (/* Done with that buffer */ TRUE && pDecoder->NextPacketSize)
    {
        // Start the next load.
        pDecoder->Overlapped.Offset     = (DWORD)(pDecoder->FileSizeRead & 0xFFFFFFFF);
        pDecoder->Overlapped.OffsetHigh = (DWORD)(pDecoder->FileSizeRead >> 32);

        if (pDecoder->NextPacketSize > pDecoder->BufferSize)
        {
            RIP("Bad input file - buffer overrun.");

            return XMV_FAIL;
        }

        if (!ReadFile(pDecoder->hFile, 
                      pDecoder->pLoadingBuffer, 
                      pDecoder->NextPacketSize, 
                      NULL, 
                      &pDecoder->Overlapped))
        {
            if (GetLastError() != ERROR_IO_PENDING)
            {
                RIP("Unable to read from the input file.");
                return XMV_FAIL;
            }
        }

        pDecoder->NextPacketSize = 0;
    }

    // Decode the next frame if we need to.
    if (!pDecoder->IsNextFrameDecoded && pDecoder->RemainingFrames)
    {
        // Set the buffer to the start of the frame pointer.
        FrameSize   = ((XMVVideoFrame *)pDecoder->pDecodingPosition)->FrameSize;
        pStartFrame = pDecoder->pDecodingPosition + sizeof(XMVVideoFrame);
        
        pDecoder->pDecodingPosition = pStartFrame;
        pDecoder->BitsRemaining     = 0;                

        // Decode the frame (frontend.c)
        DecodeOneFrame(pDecoder);

        // Reset to the start of the next frame.
        pDecoder->pDecodingPosition = pStartFrame + FrameSize;

        // Remember that we did it.
        pDecoder->RemainingFrames--;
        pDecoder->IsNextFrameDecoded = TRUE;
    }

    // Is it time to swap and we have the next frame...
    if (/* time to swap? */ TRUE && pDecoder->IsNextFrameDecoded)
    {
        // Swap the next frame to the current frame.
        pSwap                 = pDecoder->pYDisplayed;
        pDecoder->pYDisplayed = pDecoder->pYBuilding;
        pDecoder->pYBuilding  = pSwap;

        pSwap                 = pDecoder->pUDisplayed;
        pDecoder->pUDisplayed = pDecoder->pUBuilding;
        pDecoder->pUBuilding  = pSwap;

        pSwap                 = pDecoder->pVDisplayed;
        pDecoder->pVDisplayed = pDecoder->pVBuilding;
        pDecoder->pVBuilding  = pSwap;

        // Remember that we have to do the next frame.
        pDecoder->IsNextFrameDecoded = FALSE;

        // Convert the YUV format to whatever output we want (backend.c)
        RenderBitmap(pDecoder, pSurface);

        return XMV_NEWFRAME;
    }
    else
    {
        return XMV_NOFRAME;
    }
}

/*
 * Get the current frame.  This will not decode the next frame.
 *
 * This method cannot fail.  If it is called before the first call to 
 * GetNextFrame then the surface will be black.  If it's called after the end
 * of the file then it will return the very last frame.
 *
 * The passed-in surface does not have to have the same format as the one
 * passed into GetNextFrame.
 */

void XMVGetCurrentFrame
(
    XMVDecoder *pDecoder, 
    D3DSurface *pSurface
)
{
    // Convert the YUV format to whatever output we want (backend.c)
    RenderBitmap(pDecoder, pSurface);
}

