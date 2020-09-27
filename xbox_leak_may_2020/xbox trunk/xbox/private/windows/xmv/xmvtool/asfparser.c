/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       asfparser.c
 *  Content:    ASF file parser
 *
 ****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "fileio.h"
#include "xmvtool.h"

// The default packet size of the ASF file.
static DWORD g_DefaultPacketSize;

/*
 * Structures
 */

/* 
 * File structures...helpers to more easily parse the file
 */

#pragma pack(push, 1)

//
// Base object structure, all objects start with these two fields
// 

typedef struct _BaseObject
{
    GUID     ID;
    LONGLONG Size;
} 
BaseObject;

// 
// ASF header object structures.  
//

typedef struct _HeaderObject
{
    DWORD SubObjectCount;
    BYTE  Reserved1;
    BYTE  Reserved2;
}
HeaderObject;

typedef struct _FilePropertiesObject
{
    GUID     FileID;
    LONGLONG FileSize;
    LONGLONG CreationDate;
    LONGLONG DataPacketsCount;
    LONGLONG PlayDuration;
    LONGLONG SendDuration;
    LONGLONG Preroll;
    DWORD    Flags;
    DWORD    MinimumDataPacketSize;
    DWORD    MaximumDataPacketSize;
    DWORD    MaximumBitrate;
}
FilePropertiesObject;

typedef struct _StreamPropertiesObject
{
    GUID     StreamType;
    GUID     ErrorCorrectionType;
    LONGLONG TimeOffset;
    DWORD    TypeSpecificDataLength;
    DWORD    ErrorCorrectionDataLength;
    WORD     Flags;
    DWORD    Reserved;
}
StreamPropertiesObject;

typedef struct _VideoMediaData
{
    DWORD    EncodedImageWidth;
    DWORD    EncodedImageHeight;
    BYTE     Reserved;
    WORD     FormatDataSize;
}
VideoMediaData;

typedef struct _VideoFormatData
{
    DWORD    FormatDataSize;
    LONG     ImageWidth;
    LONG     ImageHeight;
    WORD     Reserved;
    WORD     BitsPerPixel;
    DWORD    CompressionID;
    DWORD    ImageSize;
    LONG     HorizontalPixelsPerMeter;
    LONG     VerticalPixelsPerMeter;
    DWORD    ColorsUsed;
    DWORD    ImportantColors;
}
VideoFormatData;

typedef struct _WMV2VideoFormatData
{
    DWORD    Unused:7;
    DWORD    SliceCode:3;  // Slice size.
    DWORD    DCTTableSwitchingEnabled:1;
    DWORD    HybridMotionVectorEnabled:1;
    DWORD    XIntra8IPictureCodingEnabled:1;
    DWORD    VariableSizedTransformEnabled:1;
    DWORD    LoopFilterEnabled:1; 
    DWORD    MixedPelMotionCompensationEnable:1; 
    DWORD    BitRate:11;
    DWORD    FrameRate:5;
} 
WMV2VideoFormatData;

typedef struct _AudioMediaData
{
    WORD     CodecID;
    WORD     ChannelCount;
    DWORD    SamplesPerSecond;
    DWORD    AveBytesPerSecond;
    WORD     BlockAlignment;
    WORD     BitsPerSample;
    WORD     CodecDataSize;
}
AudioMediaData;

typedef struct _SpreadAudioData
{
    BYTE     Span;
    WORD     VirtualPacketLength;
    WORD     VirtualChunkLength;
    WORD     SilenceDataLength;
}
SpreadAudioData;

typedef struct _ClockObject
{
    GUID     ClockType;
    WORD     ClockSize;
    DWORD    Reserved;
}
ClockObject;

typedef struct _CodecListObject
{
    GUID     Reserved;
    DWORD    EntryCount;
}
CodecListObject;

typedef struct _ScriptCommandObject
{
    GUID     Reserved;
    WORD     CommandCount;
    WORD     CommandTypeCount;
}
ScriptCommandObject;

typedef struct _ScriptCommandData
{
    DWORD    PresentationTime;
    WORD     TypeIndex;
    WORD     CommandNameLength;
}
ScriptCommandData;

typedef struct _MarkerObject
{
    GUID     Reserved;
    DWORD    MarkerCount;
    WORD     Reserved2;
    WORD     NameLength;
}
MarkerObject;

typedef struct _MarkerData
{
    LONGLONG Offset;
    LONGLONG PresentationTime;
    WORD     EntryLength;
    DWORD    SendTime;
    DWORD    Flags;
    DWORD    MarkerDescriptionLength;
}
MarkerData;

typedef struct _BitrateExclusionObject
{
    GUID     ExclusionType;
    WORD     StreamNumbersCount;
}
BitrateExclusionObject;

typedef struct _ErrorCorrectionObject
{
    GUID     ErrorCorrectionType;
    DWORD    DataLength;
}
ErrorCorrectionObject;

typedef struct ContentDescriptionObject
{
    WORD     TitleLength;
    WORD     AuthorLength;
    WORD     CopyrightLength;
    WORD     DescriptionLength;
    WORD     RatingLength;
}
ContentDescriptionObject;

typedef struct _StreamBitrateObject
{
    WORD     RecordCount;
}
StreamBitrateObject;

typedef struct _StreamBitrateData
{
    WORD     Flags;
    DWORD    AverageBitrate;
}
StreamBitrateData;

//
// ASF Data object structures
//

typedef struct _DataObject
{
    GUID     FileID;
    LONGLONG PacketCount;
    WORD     Reserved;
}
DataObject;

typedef struct _PayloadParsingInfo
{
    BYTE     LengthTypeFlags;
    BYTE     PropertyFlags;
}
PayloadParsingInfo;

typedef struct _PayloadParsingInfoField
{
    BYTE     MultiplePayloads:1;
    BYTE     SequenceType:2;
    BYTE     PaddingLengthType:2;
    BYTE     PacketLengthType:2;
    BYTE     ErrorCorrectionPresent:1;

    BYTE     ReplicatedDataLengthType:2;
    BYTE     OffsetIntoMediaObjectLengthType:2;
    BYTE     MediaObjectNumberLengthType:2;
    BYTE     StreamNumberLengthType:2;
}
PayloadParsingInfoField;

//
// ASF Index object structures.
//

typedef struct _IndexObject
{

    GUID     FileID;
    LONGLONG IndexEntryTimeInterval;
    DWORD    MaximumPacketCount;
    DWORD    IndexEntryCount;
}
IndexObject;

typedef struct _IndexData
{
    DWORD    PacketNumber;
    WORD     PacketCount;
}
IndexData;

#pragma pack(pop)

//
// Manages the information needed to walk over an individual stream.
//

typedef struct _StreamIterator
{
    // The stream we're using to read.
    MediaStream Stream;

    // Total number of remaining packets.
    LONGLONG PacketCount;

    BYTE *pPacketStart;

    DWORD PacketLength;
    DWORD PaddingLength;

    PayloadParsingInfoField PayloadInfo;

    DWORD PayloadDataLengthType;
    DWORD PayloadDataAndHeaderLength;
    DWORD PayloadStream;

    DWORD PayloadCount;

    DWORD CompressedPayloadRemainingData;
    DWORD PresentationTimeDelta;

    // Not sure what we're doing this this stuff...save it for now but it may
    // get evicted from this object.
    //
    DWORD MediaObjectNumber;
    DWORD OffsetIntoMediaObject;
    DWORD ReplicatedDataLength;
    DWORD ObjectSize;
    DWORD ObjectPres;
}
StreamIterator;

//
// Basic data structure we use to walk over the file
//

typedef struct _AsfFile
{
    // The default size of a packet.
    DWORD DefaultPacketSize;

    // Helpers to iterate over all of the streams.
    StreamIterator Streams[0];
}
AsfFile;

//
// Hold the information about one payload.
//

typedef struct _AsfPayload
{
    BYTE *pData;
    DWORD Size;

    DWORD MediaObjectNumber;
    DWORD OffsetIntoMediaObject;
    DWORD ObjectSize;
    DWORD ObjectPres;
}
AsfPayload;

/*
 * GUID tables
 */

#define ASF_GUID(n, a, b, c, d, e, f, g, h, i, j, k) n,

typedef enum _ASF_ID
{
    #include "asfguids.h"

    ASF_MAX
} 
ASF_GUID_ID;

#undef ASF_GUID

#define ASF_GUID(n, a, b, c, d, e, f, g, h, i, j, k) { a, b, c, { d, e, f, g, h, i, j, k } },

static GUID g_ASF_GUIDs[][4] = 
{
    #include "asfguids.h"
};

#undef ASF_GUID

/* 
 * Converts a guid to an ASF_ index which is much easier to deal with.
 */

static
DWORD IndexOfGUID
(
    GUID *pGUID
)
{
    DWORD i;
    
    // A table of constant guids...this is screaming for some type
    // of hash lookup.
    //
    for (i = 0; i < ASF_MAX; i++)
    {
        if (!memcmp(pGUID, g_ASF_GUIDs[i], sizeof(GUID)))
        {
            break;
        }
    }

    return i;
}

/*
 * Read a byte
 */

static
BYTE ReadByte
(
    MediaStream *pStream
)
{
    return *(BYTE *)ReadStream(pStream, 1);
}

/* 
 * Read a word
 */

static
WORD ReadWord
(
    MediaStream *pStream
)
{
    return *(WORD *)ReadStream(pStream, 2);
}

/*
 * Read a dword
 */

static
DWORD ReadDword
(
    MediaStream *pStream
)
{
    return *(DWORD *)ReadStream(pStream, 4);
}

/*
 * Read a variable-size field.
 */

#define SIZE_NONE       0
#define SIZE_BYTE       1
#define SIZE_WORD       2
#define SIZE_DWORD      3

static
DWORD ReadVariableField
(
    MediaStream *pStream,
    DWORD Size
)
{
    switch(Size)
    {
    default:
    case SIZE_NONE:

        return 0;

    case SIZE_BYTE:

        return *(BYTE *)ReadStream(pStream, 1);

    case SIZE_WORD:

        return *(WORD *)ReadStream(pStream, 2);

    case SIZE_DWORD:

        return *(DWORD *)ReadStream(pStream, 4);

    };
}

/*
 * Check the file to see if it's one of us.
 */

BOOL AsfCheckFormat
(
   MediaFile *pMediaFile
)
{
    MediaStream Stream;
    BaseObject *pObject;
    DWORD       iASF;

    OpenMediaStream(pMediaFile, &Stream);

    // Don't fall off the end.
    if (GetMediaSize(&Stream) < sizeof(BaseObject))
    {
        return FALSE;
    }

    // Read the first object.
    pObject = (BaseObject *)ReadStream(&Stream, sizeof(BaseObject));
    iASF = IndexOfGUID(&pObject->ID);

    return iASF == ASF_Header_Object;
}

/*
 * Process the header object.
 */

void *AsfOpenMedia
(
    MediaFile *pMediaFile,
    FileContents *ContentsArray
)
{
    MediaStream              Stream;
    AsfFile                 *pFile;

    BaseObject              *pObject;
    HeaderObject            *pHeader;
    FilePropertiesObject    *pFileProperties;
    StreamPropertiesObject  *pStreamProperties;
    DataObject              *pDataObject;
    VideoFormatData         *pVideoFormat;
    VideoMediaData          *pVideoMedia;
    DWORD                   *pWMV2VideoFormatRaw;
    WMV2VideoFormatData      WMV2VideoFormat;
    AudioMediaData          *pAudioMedia;

    LONGLONG HeaderObjectSize;
    LONGLONG ObjectSize;
    LONGLONG InitialOffset;

    DWORD SubObjectCount;
    DWORD iASF, iType, iError, i;
    DWORD DefaultPacketSize;
    DWORD StreamNumber = 0;

    DWORD ErrorLength;
    DWORD FormatDataSize;
    LONGLONG PacketCount;

    //
    // Read the header object.
    //

    // Go back to the beginning.
    OpenMediaStream(pMediaFile, &Stream);

    pObject = (BaseObject *)ReadStream(&Stream, sizeof(BaseObject));

    HeaderObjectSize = pObject->Size;
    iASF = IndexOfGUID(&pObject->ID);
    
    if (iASF != ASF_Header_Object)
    {
        fprintf(stderr, "ERROR: Invalid input file format; file is not a valid ASF file.\n");

        // Does not return.
        Done();
    }

    pHeader = (HeaderObject*)ReadStream(&Stream, sizeof(HeaderObject));

    SubObjectCount = pHeader->SubObjectCount;

    //
    // Load the sub objects.
    //

    while (SubObjectCount--)
    {
        InitialOffset = GetStreamPosition(&Stream);

        pObject = (BaseObject *)ReadStream(&Stream, sizeof(BaseObject));
        
        iASF = IndexOfGUID(&pObject->ID);
        ObjectSize = pObject->Size;

        switch(iASF)
        {
        default:
            SkipStreamData(&Stream, pObject->Size - sizeof(BaseObject));
            break;

        case ASF_File_Properties_Object:

            pFileProperties = (FilePropertiesObject *)ReadStream(&Stream, sizeof(FilePropertiesObject));

            DefaultPacketSize = pFileProperties->MaximumDataPacketSize;

            break;

        case ASF_Stream_Properties_Object:

            pStreamProperties = (StreamPropertiesObject *)ReadStream(&Stream, sizeof(StreamPropertiesObject));

            // Streams start at one, there is no stream zero.
            StreamNumber++;

            iType  = IndexOfGUID(&pStreamProperties->StreamType);
            iError = IndexOfGUID(&pStreamProperties->ErrorCorrectionType);

            ErrorLength = pStreamProperties->ErrorCorrectionDataLength;

            switch(iType)
            {
            default:
            case ASF_Command_Media:

                SkipStreamData(&Stream, pStreamProperties->TypeSpecificDataLength);
                break;

            case ASF_Video_Media:

                pVideoMedia = (VideoMediaData *)ReadStream(&Stream, sizeof(VideoMediaData));

                ContentsArray[StreamNumber].StreamType = STREAM_VIDEO;
                ContentsArray[StreamNumber].VideoStream.Width  = pVideoMedia->EncodedImageWidth;
                ContentsArray[StreamNumber].VideoStream.Height = pVideoMedia->EncodedImageHeight;

                FormatDataSize = pVideoMedia->FormatDataSize;

                pVideoFormat = (VideoFormatData *)ReadStream(&Stream, sizeof(VideoFormatData));

                ContentsArray[StreamNumber].VideoStream.CompressionID = pVideoFormat->CompressionID;

                if (ContentsArray[StreamNumber].VideoStream.CompressionID == '2VMW')
                {
                    // Is this always true?
                    assert(FormatDataSize - sizeof(VideoFormatData) == sizeof(WMV2VideoFormatData));

                    // We need to swap the bytes of the format code.
                    pWMV2VideoFormatRaw = (DWORD *)ReadStream(&Stream, sizeof(DWORD));
                    
                    *(DWORD *)&WMV2VideoFormat =   ((*pWMV2VideoFormatRaw & 0x000000FF) << 24)
                                                 | ((*pWMV2VideoFormatRaw & 0x0000FF00) << 8)
                                                 | ((*pWMV2VideoFormatRaw & 0x00FF0000) >> 8)
                                                 | ((*pWMV2VideoFormatRaw & 0xFF000000) >> 24);

                    ContentsArray[StreamNumber].VideoStream.FramesPerSecond                  = WMV2VideoFormat.FrameRate;
                    ContentsArray[StreamNumber].VideoStream.Slices                           = WMV2VideoFormat.SliceCode;
                    ContentsArray[StreamNumber].VideoStream.MixedPelMotionCompensationEnable = (BOOLEAN)WMV2VideoFormat.MixedPelMotionCompensationEnable; 
                    ContentsArray[StreamNumber].VideoStream.LoopFilterEnabled                = (BOOLEAN)WMV2VideoFormat.LoopFilterEnabled;
                    ContentsArray[StreamNumber].VideoStream.VariableSizedTransformEnabled    = (BOOLEAN)WMV2VideoFormat.VariableSizedTransformEnabled;
                    ContentsArray[StreamNumber].VideoStream.XIntra8IPictureCodingEnabled     = (BOOLEAN)WMV2VideoFormat.XIntra8IPictureCodingEnabled;
                    ContentsArray[StreamNumber].VideoStream.HybridMotionVectorEnabled        = (BOOLEAN)WMV2VideoFormat.HybridMotionVectorEnabled;
                    ContentsArray[StreamNumber].VideoStream.DCTTableSwitchingEnabled         = (BOOLEAN)WMV2VideoFormat.DCTTableSwitchingEnabled;
                }
                else
                {
                    SkipStreamData(&Stream, FormatDataSize - sizeof(VideoFormatData));
                }

                break;

            case ASF_Audio_Media:

                pAudioMedia = (AudioMediaData *)ReadStream(&Stream, sizeof(AudioMediaData));

                ContentsArray[StreamNumber].StreamType = STREAM_AUDIO;

                ContentsArray[StreamNumber].AudioStream.WaveFormat       = pAudioMedia->CodecID;
                ContentsArray[StreamNumber].AudioStream.ChannelCount     = pAudioMedia->ChannelCount;
                ContentsArray[StreamNumber].AudioStream.SamplesPerSecond = pAudioMedia->SamplesPerSecond;
                ContentsArray[StreamNumber].AudioStream.BitsPerSample    = pAudioMedia->BitsPerSample;

                SkipStreamData(&Stream, pAudioMedia->CodecDataSize);

                break;
            }

            SkipStreamData(&Stream, ErrorLength);

            break;
        }

        assert(GetStreamPosition(&Stream) - InitialOffset == ObjectSize);
    }

    assert(GetStreamPosition(&Stream) == HeaderObjectSize);

    //
    // Load the data header object
    //

    pObject = (BaseObject *)ReadStream(&Stream, sizeof(BaseObject));

    iASF = IndexOfGUID(&pObject->ID);

    if (iASF != ASF_Data_Object)
    {
        fprintf(stderr, "ERROR: Invalid input file format; file is not a valid ASF file.\n");

        // Does not return.
        Done();
    }

    pDataObject = (DataObject *)ReadStream(&Stream, sizeof(DataObject));
    PacketCount = pDataObject->PacketCount;

    //
    // Create the file object.
    //

    pFile = (AsfFile *)malloc(sizeof(AsfFile) + sizeof(StreamIterator) * (StreamNumber + 1));

    if (!pFile)
    {
        fprintf(stderr, "ERROR: Unable to allocate memory.\n");

        // Does not return;
        Done();
    }

    memset(pFile, 0, sizeof(AsfFile) + sizeof(StreamIterator) * (StreamNumber + 1));

    // Fill in the file info.
    pFile->DefaultPacketSize = DefaultPacketSize;

    // Fill in the stream array.
    for (i = 0; i <= StreamNumber; i++)
    {
        CloneMediaStream(&Stream, &pFile->Streams[i].Stream);
        pFile->Streams[i].PacketCount = PacketCount;
    }

    return pFile;
}

/*
 * Destroy the file object.  The file will get closed by the main routine.
 */

void AsfCloseMedia
(
    void *pFile
)
{
    free(pFile);
}

/*
 * Find the next payload for a stream.
 */

DWORD g_Packet;

static
BOOL GetNextPayload
(
    AsfFile *pFile,
    DWORD StreamNumber,
    AsfPayload *pPayload
)
{
    LONGLONG PacketOffset;
    BYTE FirstByte;
    PayloadParsingInfoField PayloadInfo;
    DWORD PayloadDataLength;

    StreamIterator *pIterator;
    MediaStream *pStream;
    
    // Make this more convenient.
    pIterator = &pFile->Streams[StreamNumber];
    pStream = &pIterator->Stream;

    do
    {
        if (!pIterator->PayloadCount)
        {
            SkipStreamData(pStream, pIterator->PaddingLength);
        
            if (!pIterator->PacketCount)
            {
                memset(pPayload, 0, sizeof(AsfPayload));

                return FALSE;
            }

g_Packet++;

            pIterator->PacketCount--;

            // Save the starting spot.
            PacketOffset = GetStreamPosition(pStream);

            // Load the first byte so we can determine if we have an error correction
            // information.
            //
            FirstByte = ReadByte(pStream);

            if (FirstByte & 0x80)
            {
                SkipStreamData(pStream, FirstByte & 0x0F);

                // Load the header for the next packet.
                memcpy(&pIterator->PayloadInfo, ReadStream(pStream, sizeof(PayloadParsingInfoField)), sizeof(PayloadParsingInfoField));
            }
            else
            {
                *((BYTE *)&pIterator->PayloadInfo + 0) = FirstByte;
                *((BYTE *)&pIterator->PayloadInfo + 1) = ReadByte(pStream);
            }

            // Load the rest of the decription field.
            pIterator->PacketLength  = ReadVariableField(pStream, pIterator->PayloadInfo.PacketLengthType);
            (void)ReadVariableField(pStream, pIterator->PayloadInfo.SequenceType);  // Sequence
            pIterator->PaddingLength = ReadVariableField(pStream, pIterator->PayloadInfo.PaddingLengthType);
            (void)ReadDword(pStream);      // send time
            (void)ReadWord(pStream);       // duration

            if (pIterator->PacketLength == 0)
            {
                pIterator->PacketLength = pFile->DefaultPacketSize;
            }

            // A single payload packet.
            if (!pIterator->PayloadInfo.MultiplePayloads)
            {
                pIterator->PayloadCount = 1;
                pIterator->PayloadDataLengthType = SIZE_NONE;

                pIterator->PayloadDataAndHeaderLength = pIterator->PacketLength - (DWORD)(GetStreamPosition(pStream) - PacketOffset) - pIterator->PaddingLength;
            }
            else
            {
                FirstByte = ReadByte(pStream);

                pIterator->PayloadCount = FirstByte & 0x3F;
                pIterator->PayloadDataLengthType = FirstByte >> 6;
                pIterator->PayloadDataAndHeaderLength = 0;
            }
        }

        // Save the current stream pointer.
        PacketOffset = GetStreamPosition(pStream);

        // If we're not in the middle of a compressed payload then get the
        // next header.
        //
        if (!pIterator->CompressedPayloadRemainingData)
        {
            pIterator->PayloadStream         = ReadByte(pStream) & 0x7F;
            pIterator->MediaObjectNumber     = ReadVariableField(pStream, pIterator->PayloadInfo.MediaObjectNumberLengthType);
            pIterator->OffsetIntoMediaObject = ReadVariableField(pStream, pIterator->PayloadInfo.OffsetIntoMediaObjectLengthType);
            pIterator->ReplicatedDataLength  = ReadVariableField(pStream, pIterator->PayloadInfo.ReplicatedDataLengthType);

            pIterator->ObjectSize = 0;

            if (pIterator->ReplicatedDataLength >= 8)
            {
                pIterator->ObjectSize = ReadDword(pStream);
                pIterator->ObjectPres = ReadDword(pStream);

                SkipStreamData(pStream, pIterator->ReplicatedDataLength - 8);
            }
            else if (pIterator->ReplicatedDataLength == 1)
            {
                pIterator->PresentationTimeDelta = ReadByte(pStream);

                // I love how this was documented...
                pIterator->ObjectPres = pIterator->OffsetIntoMediaObject;
                pIterator->OffsetIntoMediaObject = 0;
            }
            else
            {
                SkipStreamData(pStream, pIterator->ReplicatedDataLength);
            }

            // Get the size of the payload.
            if (pIterator->PayloadDataLengthType != SIZE_NONE)
            {
                PayloadDataLength = ReadVariableField(pStream, pIterator->PayloadDataLengthType);
            }
            else
            {
                PayloadDataLength = pIterator->PayloadDataAndHeaderLength - (DWORD)(GetStreamPosition(pStream) - PacketOffset);
            }

            // Remember it if we're doing a compressed payload.
            if (pIterator->ReplicatedDataLength == 1)
            {
                pIterator->CompressedPayloadRemainingData = PayloadDataLength;
            }
        }

        // Compressed payloads have their sizes encoded directly in the data.
        if (pIterator->CompressedPayloadRemainingData)
        {
            PayloadDataLength = ReadByte(pStream);

            // The Subpayload size does not include the 1-byte header.
            pIterator->CompressedPayloadRemainingData -= PayloadDataLength + 1;

            // Use this as the object size as well.
            pIterator->ObjectSize = PayloadDataLength;
        }

        if (!pIterator->CompressedPayloadRemainingData)
        {
            pIterator->PayloadCount--;
        }

        pPayload->pData = (BYTE *)ReadStream(pStream, PayloadDataLength);
        pPayload->Size  = PayloadDataLength;

        pPayload->MediaObjectNumber     = pIterator->MediaObjectNumber;
        pPayload->OffsetIntoMediaObject = pIterator->OffsetIntoMediaObject;
        pPayload->ObjectSize            = pIterator->ObjectSize;
        pPayload->ObjectPres            = pIterator->ObjectPres;

        // Adjust the presentation time for compress payloads.
        if (pIterator->CompressedPayloadRemainingData)
        {
            pIterator->ObjectPres += pIterator->PresentationTimeDelta;
        }
    }
    while (pIterator->PayloadStream != StreamNumber);

    return TRUE;
}

/*
 * Get one video frame from a video stream.
 */

BOOL AsfGetNextVideoFrame
(
    void *pFileHandle, 
    DWORD StreamNumber, 
    BYTE **ppVideoBuffer, 
    BYTE *pVideoBufferMax
)
{
    AsfFile    *pFile;
    AsfPayload  Payload;
  
    DWORD Size;

    pFile = (AsfFile *)pFileHandle;

    if (!GetNextPayload(pFile, StreamNumber, &Payload))
    {
        return FALSE;
    }

    Size = Payload.ObjectSize;

    for (;;)
    {
        assert(Size >= Payload.Size);

        if (*ppVideoBuffer + Payload.Size > pVideoBufferMax)
        {
            fprintf(stderr, "ERROR: The buffer is too small to encode this data, increase its size.\n");
            Done();
        }

        // Copy the data into the buffer.
        memcpy(*ppVideoBuffer, Payload.pData, Payload.Size);

        // Adjust it all.
        *ppVideoBuffer += Payload.Size;
        Size           -= Payload.Size;

        if (!Size)
        {
            break;
        }

        if (!GetNextPayload(pFile, StreamNumber, &Payload))
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * Get one frame's worth of audio
 */

void AsfGetNextAudioFrame
(   
    void *pFile, 
    DWORD StreamNumber, 
    DWORD FPS, 
    BYTE **ppAudioBuffer, 
    BYTE *pAudioBufferMax
)
{
}