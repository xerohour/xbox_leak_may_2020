/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xmvformat.h
 *  Content:    Xbox FMV file format
 *
 ****************************************************************************/

#pragma once

// This should be changed to '1' when we're sure that the format is stable.
// The decoder will refuse to load any version of the file that does not
// match.
//
#define XMV_FILE_VERSION        (-1)

// The magic cookie used to identify XMV files.
#define XMV_MAGIC_COOKIE        ('Xbox')

/*
// The data in the XMV file is organized into video packets designed to be
// loaded into a constant-sized buffer.  Each packet starts with the size
// of the next packet to allow it to be loaded via an overlapped IO call
// while the current packet is being processed.
*/

/*
// The very first video packet starts with a header describing the contents
// of the file followed by the normal video packet data.  The size of the
// first packet includes the size of this header so it can be read into
// the normal read buffer.
*/

// NOTE: I've tapped this down to 12 bytes to ensure that our packet
//   header size is always under 4096 bytes.  We need to keep it under 16 byes
//   or this little assumption I've made won't be true in perverse cases.
//
typedef struct _XMVAudioDataHeader
{
    // The WAVE_FORMAT tag that describes how the audio data in the stream is
    // encoded.  This can be either WAVE_FORMAT_PCM or WAVE_FORMAT_ADPCM.
    //
    WORD WaveFormat;

    // The number of channels in the audio stream.  Can be 1, 2, 4 or 6.
    WORD ChannelCount;

    // The number of samples per second (Hz) in the audio stream.
    DWORD SamplesPerSecond;

    // The number of bits in each sample.  
    DWORD BitsPerSample;
}
XMVAudioDataHeader;

typedef struct _XMVVideoDataHeader
{
    // A kinda-unique value to help verify that the decoder is actually
    // loading an XMV file.  Defined to always be XMV_MAGIC_COOKIE.
    //
    DWORD MagicCookie;

    // The file format version of this file to ensure that the version
    // of xmvtool that creates the XMV file matches the version of the
    // decoder.
    //
    DWORD FileVersion;

    // The size of the very first packet.
    DWORD ThisPacketSize;

    // The number of frames per second to display the video at.  All timing of
    // the playback is based off this value.  It can be any integer value
    // from zero to 60.  
    //
    DWORD FramesPerSecond;

    // The number of slices that the picture is divided into.
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
    
    // The width of the encoded video in pixels.  If this is zero then there
    // is no video encoded in this file.
    //
    DWORD Width;

    // The height of the encoded video in pixels.
    DWORD Height;

    // The buffer size required to load all of the video packets in this
    // file.
    //
    DWORD RequiredBufferSize;

    // The number of audio streams contained in each video packet.
    DWORD AudioStreamCount;

    // The descriptor for each audio stream.
    XMVAudioDataHeader AudioHeaders[0];
}
XMVVideoDataHeader;

/*
// The size of each video packet must be a multiple of 4096 bytes.
// 
// Every video packet begins with the following header:
*/

typedef struct _XMVPacketHeader
{
    // The size of the next video packet in bytes, including this
    // header.
    //
    DWORD NextPacketSize;
}
XMVPacketHeader;

/*
// The VideoPacketHeader immediately the PacketHeader.  It contains all of the
// information necessary for the decoder to find the different streams needed 
// to reconstruct the video.
*/

typedef struct _XMVAudioStreamHeader
{
    // The length of the audio data in the stream in bytes.  The actual space
    // reserved for the stream is always a multiple of 4 bytes.
    //
    DWORD AudioStreamSize;
}
XMVAudioStreamHeader;

typedef struct _XMVVideoPacketHeader
{
    // The length of the video stream in bytes.  Any padding needed
    // to take the packet out to a multiple of 4096 bytes should 
    // be added as zeros at the end of this stream.
    //
    // The size is not inclusive of any headers, including this one.
    //
    DWORD VideoStreamLength:24;

    // The number of frames encoded within this packet.  The time
    // represented by this packet is base solely off of the framerate and the
    // count here.  So if the video is encoded at 30fps and this packet
    // contains 15 frames then the packet represents exactly 1/2 a second.
    //
    DWORD FrameCount:8;

    // The audio stream sizes.  Each video packet contains the same
    // number of audio streams and they're always encoded in the
    // exact same order.
    //
    XMVAudioStreamHeader AudioStreams[0];
}
XMVVideoPacketHeader;

/*
// The data in the video stream doesn't seem to have any easy way to determine
// what the size of the data for each frame is.  The decode doesn't care much
// about this but it makes the tools difficult to implement.  Just shove
// a DWORD in front of each one for now and do something more fancy if this
// should prove to be too much.
*/

typedef struct _XMVVideoFrame
{
    DWORD FrameSize;
}
XMVVideoFrame;

/*
// ADPCM audio streams can't allow a single packet to cross a page boundary 
// or we'll have to allocate the entire buffer in contiguous memory...
// something I do not want to have to do.  We will instead break the stream
// up into chunks that do not cross such a boundary and can be fed
// individually to the audio hardware.
//
// A chunk always starts exactly at a page boundary.  The size in the chunk
// header always indicates how much audio data is contained in that chunk. 
// The next chunk starts at the next page boundary.
*/

typedef struct _XMVADPCMChunk
{
    // The size of the audio data in this chunk, not including any padding
    // needed to take this out to a page boundary.
    //
    DWORD AudioDataSize;
}
XMVADPCMChunk;