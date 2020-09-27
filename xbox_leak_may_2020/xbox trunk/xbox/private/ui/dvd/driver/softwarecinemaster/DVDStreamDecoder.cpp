////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "DVDStreamDecoder.h"
#include "library\common\vddebug.h"
#include "common\PerformanceMonitor.h"

#define AUDIO_DUMP						0
#define VIDEO_DUMP						0
#define INDUCE_ERRORS_IN_BITSTREAM	0
#define DUMP_STREAM_EVENTS				0

#if DUMP_STREAM_EVENTS
void DTE(char * p)
	{
	char buffer[100];
	wsprintf(buffer, "%8d %s\n", GetInternalTime(), p);
	OutputDebugString(buffer);
	}
#else
void DTE(char * p)
	{
	}
#endif

//
// MPEG 2 program stream demultiplexing
//
// see ISO/IEC 13818-1
//

// ProgramStream 		::=	{PackHeader Packet}
// PackHeader		   ::=   PackStartCode:32 01:2 SCR:42 ProgramMuxRate:22 pad:5 PackStuffingLength::3 PackStuffingByte::x
// 								SystemHeader {PESPacket}
// SystemHeader		::=	SystemHeaderStartCode:32 HeaderLength:16 RateBound:22 AudioBound:6 FixedFlag:1 CSPSFlag:1
// 								AudioLockFlag:1 VideoLockFlag:1 VideoBound:5 NLoop:24*n
// NLoop					::=	StreamID:8 11:2 BufferBoundScale:1 BufferSizeBound:13
//
// PESPacket			::=	PacketStartCodePrefix:24 StreamID:8 PESPacketLength:16 OptionalPESHeader PESDataBytes
// OptionalPESHeader	::=	10:2 PESScramblingControl:2 PESPriority:1 DataAlignmentIndicator:1 Copyright:1
// 								OriginalOrCopy:1 SevenFlags:8 PESHeaderDataLength:8 OptionalFields1 Stuffing:8*n
// OptionalFields1	::=	PTS_DTS:33 ESCR:42 ESRate:22 DSMTrickMode:8 AdditionalCopyInfo:7 PreviousOESCRC:16 PESExtension
// PESExtension		::=	FiveFlags:5 OptionalFields2
// OptionalFields2	::=	PESPrivateData:128 PackHeaderField:8 ProgramPacketSeqCntr:8 PSTDBuffer:16
// 								PESExtensionFieldLength:7 PESExtensionField
// ProgramStreamMap	::=	PacketStartCodePrefix:24 MapStreamID:8 ProgramStreamMapLength:16

//
// 0 : 0
// 1 : 1
// # : Value
// X : Don't care
//

#define M0	1
#define M1	1
#define MX	0
#define V0	0
#define V1	1
#define VX	0

#define MKBCX(name, val) static const WORD B##name	= val;
#define MKBC(a7,a6,a5,a4,a3,a2,a1,a0) MKBCX(a7##a6##a5##a4##a3##a2##a1##a0, \
	(V##a7<<7) | (V##a6<<6) | (V##a5<<5) | (V##a4<<4) | (V##a3<<3) | (V##a2<<2) | (V##a1<<1) | V##a0 |	\
	(M##a7<<15) | (M##a6<<14) | (M##a5<<13) | (M##a4<<12) | (M##a3<<11) | (M##a2<<10) | (M##a1<<9) | (M##a0 << 8))

MKBC(0,0,0,0,0,0,0,0)
MKBC(0,0,0,0,0,0,0,1)
MKBC(0,0,0,0,1,1,1,1)
MKBC(0,0,0,1,X,X,X,1)
MKBC(0,0,1,0,X,X,X,1)
MKBC(0,0,1,1,X,X,X,1)
MKBC(0,1,X,X,X,1,X,X)
MKBC(0,1,X,X,X,X,X,X)
MKBC(1,0,1,1,1,0,0,1)
MKBC(1,0,1,1,1,0,1,0)
MKBC(1,0,1,1,1,0,1,1)
MKBC(1,0,X,X,X,X,X,X)
MKBC(1,1,X,X,X,X,X,X)
MKBC(1,1,1,1,1,1,1,1)
MKBC(1,X,X,X,X,X,X,X)
MKBC(X,1,X,X,X,X,X,X)
MKBC(X,X,1,X,X,X,X,X)
MKBC(X,X,X,1,X,X,X,X)
MKBC(X,X,X,X,1,X,X,X)
MKBC(X,X,X,X,X,1,X,X)
MKBC(X,X,X,X,X,X,1,X)
MKBC(X,X,X,X,X,X,1,1)
MKBC(X,X,X,X,X,X,X,1)
MKBC(X,X,X,X,X,X,X,X)


enum MPEG2ProgramParse {
	PROGSTRM_INITIAL,             //
	PROGSTRM_FINAL,					//

	PROGSTRM_PACKSTART0,          // 00000000
	PROGSTRM_PACKSTART1,				// 00000000
	PROGSTRM_PACKSTART2,				// 00000000
	PROGSTRM_PACKSTART3,				// 10111010 goto PACKHEADER_BYTE0
											// 10111001 goto PROGSTRM_FINAL

	MP1PACKHDR_BYTE1,					// ######## SysClockRef [29..22]
	MP1PACKHDR_BYTE2,					// #######  SysClockRef [21..15]
	                              //        1
	MP1PACKHDR_BYTE3,					// ######## SysClockRef [14..7]
	MP1PACKHDR_BYTE4,					// #######  SysClockRef [6..0]
											//        1
	MP1PACKHDR_BYTE5,					// 1
											//  ####### MuxRate[21..15]
	MP1PACKHDR_BYTE6,					// ######## MuxRate[14..7]
	MP1PACKHDR_BYTE7,					// #######  MuxRate[6..0]
											//        1
											// goto PESPACKET_STARTCODEPREFIX0

	PACKHEADER_BYTE0,					// if (01XXXXXX)
											//	01
											//   ###    SysClockRef [32..30]
											//      1
											//       ## SysClockRef [29..28]
											// mpeg2 = TRUE;
											// goto PACKHEADER_BYTE1
											// else if (0010XXXX)
											// 0010
											//     ###  SysClockRef [32..30]
											//        1
											// goto MP1PACKHDR_BYE1

	PACKHEADER_BYTE1,					// ######## SysClockRef [27..20]
	PACKHEADER_BYTE2,					// #####    SysClockRef [19..15]
	                              //      1
	                              //       ## SysClockRef [14..13]
	PACKHEADER_BYTE3,					// ######## SysClockRef [12..5]
	PACKHEADER_BYTE4,					// #####    SysClockRef [4..0]
											//      1
											//       ## SysClockRefExt [8..7]
	PACKHEADER_BYTE5,					// #######  SysClockRefExt [6..0]
											//        1
	PACKHEADER_BYTE6,					// ######## MuxRate[21..14]
	PACKHEADER_BYTE7,					// ######## MuxRate[13..6]
	PACKHEADER_BYTE8,					// ######   MuxRate[5..0]
											//       11
	PACKHEADER_BYTE9,					// XXXXX
											//      ### PackStuffingLength
	PACKHEADER_STUFFING,				// XXXXXXXX times PackStuffingLength
											// goto PESPACKET_STARTCODEPREFIX0

   SYSTEMHEADER_LENGTH0,			// ########
   SYSTEMHEADER_LENGTH1,			// ########
   SYSTEMHEADER_BYTE0,				// 1
   										//  ####### RateBound[21..15]
   SYSTEMHEADER_BYTE1,				// ######## RateBound[14..7]
   SYSTEMHEADER_BYTE2,				// #######  RateBound[6..0]
   										//        1
   SYSTEMHEADER_BYTE3,				// ######   AudioBound
   										//       #  FixedFlag
											//        # CSPSFlag
	SYSTEMHEADER_BYTE4,				// #        SystemAudioLockFlag
											//  #       SystemVideoLockFlag
											//   1      MarkerBit
											//    ##### VideoBound
	SYSTEMHEADER_BYTE5,				// #        PacketRateRestrictionBit
											//  XXXXXXX
	SYSTEMHEADER_BYTE6,				// 00000000 goto PESPACKET_STARTCODEPREFIX1
											// 1####### StreamID
	SYSTEMHEADER_STREAM0,			// 11
											//   #      PSTDBufferBoundScale
											//    ##### PSTDBufferSizeBound [12..8]
	SYSTEMHEADER_STREAM1,			// ######## PSTDBufferSizeBound [7..0]
											// goto SYSTEMHEADER_BYTE6

	PESPACKET_STARTCODEPREFIX0,	// 00000000
	PESPACKET_STARTCODEPREFIX1,	// 00000000
	PESPACKET_STARTCODEPREFIX2,	// 00000001
	PESPACKET_STREAMID,				// 10111011 goto SYSTEMHEADER_LENGTH0
											// 10111010 goto PACKHEADER_BYTE0
											// 10111001 goto PROGSTRM_FINAL
											// else
											// ######## streamID
	PESPACKET_LENGTH0,				// ########
	PESPACKET_LENGTH1,				// ########
											// if streamID != 10111101 &&
											//    streamID != videoStreamID && streamID != audioStreamID goto PESPACKET_SKIP
											// mpeg2 goto PESPACKET_HEADER0

	MP1PACKET_HEADER0,				// if (11111111) goto MP1PACKET_HEADER0
											// if (01XXXXXX) goto MP1PACKET_HEADER1
											// if (0010XXXX)
											// 0010
											//     ###  PTS[32..30]
											//        1 goto MP1PACKET_HEADER2
											// if (0011XXXX)
											// 0011
											//		 ###  PTS[32..30]
											//        1 goto MP1PACKET_HEADER6
											//	else
											// 00001111
											// goto MP1PACKET_DATA
	MP1PACKET_HEADER1,				// XXXXXXXX	goto MP1PACKET_HEADER0
	MP1PACKET_HEADER2,				// ######## PTS[29..22]
	MP1PACKET_HEADER3,				// #######  PTS[21..15]
											//        1
	MP1PACKET_HEADER4,				// ######## PTS[14..7]
	MP1PACKET_HEADER5,				// #######  PTS[6..0]
											//        1
											// goto MP1PACKET_DATA
	MP1PACKET_HEADER6,				// ######## PTS[29..22]
	MP1PACKET_HEADER7,				// #######  PTS[21..15]
											//        1
	MP1PACKET_HEADER8,				// ######## PTS[14..7]
	MP1PACKET_HEADER9,				// #######  PTS[6..0]
											//        1
	MP1PACKET_HEADER10,				// 0001
											//     ###  DTS[32..30]
											//        1
	MP1PACKET_HEADER11,				// ######## DTS[29..22]
	MP1PACKET_HEADER12,				// #######  DTS[21..15]
											//        1
	MP1PACKET_HEADER13,           // ######## DTS[14..7]
	MP1PACKET_HEADER14,           // #######  DTS[6..0]
	                              //        1
	                              // goto MP1PACKET_DATA

	PESPACKET_HEADER0,				// 10
											//   ##     Scrambling control
											//     #    Priority
											//      #   Data alignment
											//       #  Copyright
											//        # OriginalOrCopy
	PESPACKET_HEADER1,				// ##       PTSDTSFlags
											//   #      ESCRFlag
											//    #     ESRateFlag
											//     #    DSMTrickModeFlag
											//      #   AdditionalCopyInfoFlag
											//       #  PESCRCFlag
											//        # PESExtensionFlag
	PESPACKET_HEADER2,				// ######## PESHeaderDataLength

// if PTSDTSFlags == 10

	PESPACKET_HEADER3,				// 0010
											//     ###  PTS[32..30]
											//        1
	PESPACKET_HEADER4,				// ######## PTS[22..29]
	PESPACKET_HEADER5,				// #######  PTS[15..21]
											//        1
	PESPACKET_HEADER6,				// ######## PTS[14..7]
	PESPACKET_HEADER7,				// #######  PTS[0..6]
											//        1

// if PTSDTSFlags == 11

	PESPACKET_HEADER8,				// 0011
											//     ###	PTS[32..30]
											//        1
	PESPACKET_HEADER9, 				// ######## PTS[22..29]
	PESPACKET_HEADER10,				// #######  PTS[15..21]
											//        1
	PESPACKET_HEADER11,				// ######## PTS[14..7]
	PESPACKET_HEADER12,				// #######  PTS[0..6]
											//        1
	PESPACKET_HEADER13,				// 0001
											//     ###  DST[32..30]
											//        1
	PESPACKET_HEADER14,				// ######## DTS[22..29]
	PESPACKET_HEADER15,				// #######  DTS[15..21]
											//        1
	PESPACKET_HEADER16,				// ######## DTS[14..7]
	PESPACKET_HEADER17,				// #######  DTS[0..6]
											//        1

// if ESCRFlag

	PESPACKET_HEADER18,				// XX
											//   ###    ESCR[32..30]
											//      1
											//       ## ESCR[29..28]
	PESPACKET_HEADER19,				// ######## ESCR[27..20]
	PESPACKET_HEADER20,				// #####    ESCR[19..15]
											//      1
// PESPACKET_HEADER21 intentionaly left blank
	PESPACKET_HEADER22,				// ######## ESCR[12..5]
	PESPACKET_HEADER23,				// #####    ESCR[4..0]
											//      1
											//       ## ESCRX[8..7]
	PESPACKET_HEADER24,				// #######  ESCRX[6..0]
											//        1

// if ESRateFlag

	PESPACKET_HEADER25,				// 1
											//  ####### ESRATE[14..21]
	PESPACKET_HEADER26,				// ######## ESRATE[14..7]
	PESPACKET_HEADER27,				// #######  ESRATE[6..0]
											//        1

// if DSMTrickModeFlag

	PESPACKET_HEADER28,				// ###      TrickModeControl

											//    ##    FieldID
											//      #   IntraSlizeRefresh
											//       ## FrequencyTruncation
											//    ##### RepCntrl
											//    ##    FieldID
											//      ### Reserved

// if AdditionalCopyInfoFlag

	PESPACKET_HEADER29,				// 1
											//  ####### AdditionalCopyInfo


// if PESCRCFlag

	PESPACKET_HEADER30,				// ######## CRC[15..8]
	PESPACKET_HEADER31,				// ######## CRC[7..0]

// if PESExtensionFlag

	PESPACKET_HEADER32,				// #        PESPrivateDataFlag
											//  #       PackHeaderFieldFlag
											//   #      ProgramPacketSequenceCounterFlag
											//    #     PSTDBufferFlag
											//     XXX
											//        # PESExtensionFlag2

	// if PESPrivateDataFlag

	PESPACKET_PRIVATEDATA,			// label,,, :-)
	PESPACKET_PRIVATESKIP,			// XXXXXXXX times 32

	// if PackHeaderFieldFlag

	PESPACKET_PACKFIELDLENGTH,		// XXXXXXXX PackFieldLength

	PESPACKET_PACKHEDADER,			// ######## times PackFieldLength

	// if ProgramPacketSequenceCounterFlag

	PESPACKET_HEADER33,				// 1
											//  ####### ProgramPacketSequenceCounter
	PESPACKET_HEADER34,				// 1
											//  #       MPEG12Identifier
											//   ###### OriginalStuffLength

	// if PSTDBufferFlag

	PESPACKET_HEADER35,				// 01
											//   #      PSTDBufferScale
											//    ##### PSTDBufferSize[13..8]
	PESPACKET_HEADER36,				// ######## PSTDBufferSize[7..0]

	// if PESExtensionFlag2

	PESPACKET_HEADER37,				// 1
											//  ####### PESExtensionFieldLength
	PESPACKET_EXTENSIONFIELD,		// XXXXXXXX times PESExtensionFieldLength

	PESPACKET_STUFFING,				// XXXXXXXX times rest of PESHeaderDataLength

	PESPACKET_DATA,					// XXXXXXXX times PESPacketLength - 3 - PESHeaderDataLength
											// goto PESPACKET_STARTCODEPREFIX0
	MP1PACKET_DATA,					// ....
	PESPACKET_VIDEO,					// ######## times PESPacketlength - 3 - PESHeaderDataLength
											// goto PESPACKET_STARTCODEPREFIX1
	PESPACKET_AUDIO,					// ######## times PESPacketLength - 3 - PESHeaderDataLength
											// goto PESPACKET_STARTCODEPREFIX2
	PESPACKET_SKIP,					// ######## times PESPacketLength
	PESPACKET_PRIVATE,				// if ######## != AC3 GOTO PESPACKET_SKIP

	PESPACKET_AC3_SKIP0,				// 2*XXXXXXXX
	PESPACKET_AC3_SKIP1,				// 2*XXXXXXXX
	PESPACKET_AC3_SKIP2,				// 2*XXXXXXXX
	PESPACKET_AC3_SKIP3,				// 2*XXXXXXXX
	PESPACKET_AC3,						// ######## times PESPacketLength - 4 - PESHeaderDataLength
											// goto PESPACKET_STARTCODEPREFIX2

	PESPACKET_LPCM_SKIP0,			// 2*XXXXXXXX
	PESPACKET_LPCM_SKIP1,			// 2*XXXXXXXX
	PESPACKET_LPCM_SKIP2,			// 2*XXXXXXXX
	PESPACKET_LPCM_SKIP3,			// 2*XXXXXXXX
	PESPACKET_LPCM,					// ######## times PESPacketLength - 4 - PESHeaderDataLength
											// goto PESPACKET_STARTCODEPREFIX2

	PESPACKET_SUBPICTURE,			// ######## times PESPacketLength - 4 - PESHeaderDataLength
	PROGSTRM_RECOVER1,            // if !00000000 GOTO PROGSTRM_RECOVER1
	PROGSTRM_RECOVER2,            // if !00000000 GOTO PROGSTRM_RECOVER1
	PROGSTRM_RECOVER3,				// if !00000001 GOTO PROGSTRM_RECOVER1
	PROGSTRM_RECOVER4,				//	if !10111010 GOTO PROGSTRM_RECOVER1 else GOTO PACKHEADER_BYTE0

	DVDSTRM_PACKETSTART,
	DVDSTRM_VIDEOPACKET,
	DVDSTRM_VIDEOPESPACKET,
	DVDSTRM_AUDIOPACKET,
	DVDSTRM_AUDIOPESPACKET,
	DVDSTRM_AC3PACKET,
	DVDSTRM_AC3PESPACKET,
	DVDSTRM_LPCMPACKET,
	DVDSTRM_LPCMPESPACKET,
	DVDSTRM_SPUPACKET,
	DVDSTRM_SPUPESPACKET,
	DVDSTRM_STUFFING
	};

////////////////////////////////////////////////////////////////////
//
//  DVD Position Tracker
//
////////////////////////////////////////////////////////////////////

DVDPositionTracker::DVDPositionTracker(int num)
	{
	int i;

	i = 0;
	while (num)
		{
		i++;
		num >>= 1;
		}

	this->num = 1 << i;
	this->mask = this->num - 1;

	track = new Track[this->num];
	first = last = prev = 0;
	}

DVDPositionTracker::~DVDPositionTracker(void)
	{
	delete[] track;
	}

void DVDPositionTracker::Flush(void)
	{
	first = last = prev = 0;
	}

void DVDPositionTracker::AddTrack(__int64 mux, __int64 elem)
	{
	int prev = (last + num - 1) & mask;

	if (last == first || mux - track[prev].mux > 10000 || elem - track[prev].elem > 9000)
		{
		int next = (last + 1) & mask;
		track[last].mux = mux;
		track[last].elem = elem;
		if (first == next) first = (first+1) % num;
		last = next;
		}
	}

__int64 DVDPositionTracker::MuxToElem(__int64 mux)
	{
	int i = first;
	__int64 elem = 0;

	while (i != last && track[i].mux <= mux) {elem = track[i].elem; i = (i+1) & mask;}

	return elem;
	}

__int64 DVDPositionTracker::ElemToMux(__int64 elem)
	{
	int i = prev;
	__int64 mux = 0;
	__int64 pred = 0;

	if (i < first && i >= last)
		i = first;
	else if (track[i].elem >= elem)
		i = first;

	mux = track[i].mux;

	while (i != last && track[i].elem < elem)
		{
		pred = track[i].elem;
		mux = track[i].mux;
		i = (i+1) & mask;
		}

	if (i == last)
		return -1;
	else if (i != first)
		{
		prev = (i - 1) & mask;
		if (track[i].elem == pred)
			return mux;
		else
			return mux + (int)(elem - pred) * (int)(track[i].mux - mux) / (int)(track[i].elem - pred);
		}
	else
		return track[i].mux;
	}

////////////////////////////////////////////////////////////////////
//
//  Embedded MPEG Video Stream Decoder
//
////////////////////////////////////////////////////////////////////

EmbeddedMPEGVideoStreamDecoder::EmbeddedMPEGVideoStreamDecoder
                              (GenericPictureDisplay * display,
								GenericSliceParser * highQualitySliceParser,
			                    GenericSliceParser * lowQualitySliceParser,
								DVDStreamDecoder * dvdDecoder,
								GenericProfile * globalProfile,
								GenericProfile * profile,
								StreamFlipCopy * streamFlipCopy)
								: MPEGVideoStreamDecoder(display, highQualitySliceParser, lowQualitySliceParser, globalProfile, profile, streamFlipCopy)
	{
	this->dvdDecoder = dvdDecoder;
	}

void EmbeddedMPEGVideoStreamDecoder::RefillRequest(void)
	{
	dvdDecoder->InternalRefillRequest();
	}

void EmbeddedMPEGVideoStreamDecoder::StreamCompleted(void)
	{
	dvdDecoder->CheckSignal();
	}

void EmbeddedMPEGVideoStreamDecoder::ScanCompleted(void)
	{
	dvdDecoder->ScanCompleted();
	}

////////////////////////////////////////////////////////////////////
//
//  Embedded AC3 Stream Decoder
//
////////////////////////////////////////////////////////////////////

EmbeddedAC3StreamDecoder::EmbeddedAC3StreamDecoder(PCMWaveOut * pcmWaveOut,
													TimingSlave * timingSlave,
										            DVDStreamDecoder * dvdDecoder,
													GenericProfile * globalProfile,
													GenericProfile * profile,
													StreamFlipCopy * streamFlipCopy)
													: AC3StreamDecoder(pcmWaveOut, timingSlave, globalProfile, profile, streamFlipCopy)
	{
	this->dvdDecoder = dvdDecoder;
	}

void EmbeddedAC3StreamDecoder::RefillRequest(void)
	{
	dvdDecoder->InternalRefillRequest();
	}

void EmbeddedAC3StreamDecoder::StreamCompleted(void)
	{
	dvdDecoder->CheckSignal();
	}

////////////////////////////////////////////////////////////////////
//
//  Embedded DTS Stream Decoder
//
////////////////////////////////////////////////////////////////////

EmbeddedDTSStreamDecoder::EmbeddedDTSStreamDecoder(PCMWaveOut * pcmWaveOut,
														TimingSlave * timingSlave,
														DVDStreamDecoder * dvdDecoder,
														GenericProfile * globalProfile,
														GenericProfile * profile,
														StreamFlipCopy * streamFlipCopy)
														: DTSStreamDecoder(pcmWaveOut, timingSlave, globalProfile, profile, streamFlipCopy)
	{
	this->dvdDecoder = dvdDecoder;
	}

void EmbeddedDTSStreamDecoder::RefillRequest(void)
	{
	dvdDecoder->InternalRefillRequest();
	}

void EmbeddedDTSStreamDecoder::StreamCompleted(void)
	{
	dvdDecoder->CheckSignal();
	}


////////////////////////////////////////////////////////////////////
//
//  Embedded MPEG Audio Stream Decoder
//
////////////////////////////////////////////////////////////////////

EmbeddedMPEGAudioStreamDecoder::EmbeddedMPEGAudioStreamDecoder(PCMWaveOut * pcmWaveOut,
																TimingSlave * timingSlave,
																DVDStreamDecoder * dvdDecoder,
																GenericProfile * globalProfile,
																GenericProfile * profile,
																StreamFlipCopy * streamFlipCopy)
																: MPEGAudioStreamDecoder(pcmWaveOut, timingSlave, globalProfile, profile, streamFlipCopy)
	{
	this->dvdDecoder = dvdDecoder;
	}

void EmbeddedMPEGAudioStreamDecoder::RefillRequest(void)
	{
	dvdDecoder->InternalRefillRequest();
	}

void EmbeddedMPEGAudioStreamDecoder::StreamCompleted(void)
	{
	dvdDecoder->CheckSignal();
	}

////////////////////////////////////////////////////////////////////
//
//  Embedded LPCM Stream Decoder
//
////////////////////////////////////////////////////////////////////

EmbeddedLPCMStreamDecoder::EmbeddedLPCMStreamDecoder(PCMWaveOut * pcmWaveOut,
														TimingSlave * timingSlave,
														DVDStreamDecoder * dvdDecoder,
														StreamFlipCopy * streamFlipCopy)
														: LPCMStreamDecoder(pcmWaveOut, timingSlave, streamFlipCopy)
	{
	this->dvdDecoder = dvdDecoder;
	}

void EmbeddedLPCMStreamDecoder::RefillRequest(void)
	{
	dvdDecoder->InternalRefillRequest();
	}

void EmbeddedLPCMStreamDecoder::StreamCompleted(void)
	{
	dvdDecoder->CheckSignal();
	}

////////////////////////////////////////////////////////////////////
//
//  Embedded SPU Stream Decoder
//
////////////////////////////////////////////////////////////////////

EmbeddedSPUStreamDecoder::EmbeddedSPUStreamDecoder(SPUDisplay * display,
													DVDStreamDecoder * dvdDecoder)
													: SPUStreamDecoder(display)
	{
	this->dvdDecoder = dvdDecoder;
	}

void EmbeddedSPUStreamDecoder::RefillRequest(void)
	{
	dvdDecoder->InternalRefillRequest();
	}

////////////////////////////////////////////////////////////////////
//
//  DVD Stream Decoder
//
////////////////////////////////////////////////////////////////////

#if AUDIO_DUMP
static HANDLE audioStream;
#endif
#if VIDEO_DUMP
static HANDLE videoStream;
static int videoStreamCount;
#endif

//
//  Constructor
//

#pragma warning(disable : 4355)
#pragma warning(disable : 4786)

DVDStreamDecoder::DVDStreamDecoder(GenericPictureDisplay * display,
						GenericSliceParser * highQualitySliceParser,
						GenericSliceParser * lowQualitySliceParser,
						GenericAudioPlaybackDevice * audioPlaybackDevice,
						SPUDisplay * spuDisplay,
						GenericProfile * globalProfile,
						GenericProfile * profile,
						ClosedCaptionDecoder * ccDecoder,
						StreamFlipCopy * streamFlipCopy,
						GenericDecryptor * decryptor)
					  : pcmWaveOut(globalProfile, profile, audioPlaybackDevice, display),
						videoDecoder(display, highQualitySliceParser, lowQualitySliceParser, this, globalProfile, profile, streamFlipCopy),
						ac3Decoder(&pcmWaveOut, display, this, globalProfile, profile, streamFlipCopy),
					    dtsDecoder(&pcmWaveOut, display, this, globalProfile, profile, streamFlipCopy),
					    mpegAudioDecoder(&pcmWaveOut, display, this, globalProfile, profile, streamFlipCopy),
						lpcmDecoder(&pcmWaveOut, display, this, streamFlipCopy),
						spuDecoder(spuDisplay, this),
						audioTracker(128),
						videoTracker(256),
						bufferMonitorEvent(FALSE, TRUE),
						supervisor(this),
						TimedFiber(3)
	{
	int i;

	this->decryptor = decryptor;
	this->display = display;
	this->audioPlaybackDevice = audioPlaybackDevice;
	this->ccDecoder = ccDecoder;

	videoStreamID = 0xe0;
	ac3StreamID = 0x80;
	dtsStreamID = 0x88;
	audioStreamID = 0xc0;
	spuStreamID = 0x20;
	lpcmStreamID = 0xa0;

	audioType = MP2AUDTYP_LPCM;
	audioDecoder = &lpcmDecoder;
	samplesPerSecond = 48000;
	bitsPerSample = 16;
	channels = 2;
	initialAudioPacket = TRUE;
	deferedRunning = FALSE;

	streamType = DVST_PROGRAM_STREAM;

	FlushBuffer();

	streaming = FALSE;
	terminate = FALSE;

	for(i=0; i<4; i++)
		{
		mpeparse[i].streamType = DVSST_PROGRAM;
		mpeparse[i].dataEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		}


#if AUDIO_DUMP
	audioStream = ::CreateFile("f:\\ac3\\strm.ac3",
							  GENERIC_WRITE,
							  FILE_SHARE_WRITE,
							  NULL,
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);
#endif
#if VIDEO_DUMP
		{
		char buffer[100];
		videoStreamCount = 0;
		wsprintf(buffer, "c:\\videodump\\strm%03d.mpg", videoStreamCount);

		videoStream = ::CreateFile(buffer,
								  GENERIC_WRITE,
								  FILE_SHARE_WRITE,
								  NULL,
								  CREATE_ALWAYS,
								  FILE_ATTRIBUTE_NORMAL,
								  NULL);
		}
#endif

	int type;

	for(type=0; type<4; type++)
		{
		mpeparse[type].pendingData = NULL;
		mpeparse[type].pendingPairs = NULL;
		}

	StartFiber();
	supervisor.StartFiber();

	cinemSupDriver = CreateFile("\\\\.\\CINEMSUP.SYS", 0, 0, NULL, OPEN_EXISTING,
						0, 0);

	if (cinemSupDriver == INVALID_HANDLE_VALUE) cinemSupDriver = NULL;
	}
#pragma warning(default : 4355)

//
//  Shut Down Supervisor
//

void DVDStreamDecoder::ShutDownSupervisor(void)
	{
	terminate = TRUE;
	bufferMonitorEvent.SetEvent();
	CompleteFiber();
	supervisor.CompleteFiber();
	}

//
//  Destructor
//

DVDStreamDecoder::~DVDStreamDecoder(void)
	{
	int i;

	ShutDownSupervisor();

	for(i=0; i<4; i++)
		::CloseHandle(mpeparse[i].dataEvent);
#if AUDIO_DUMP
	::CloseHandle(audioStream);
#endif
#if VIDEO_DUMP
	::CloseHandle(videoStream);
#endif

	if (cinemSupDriver) CloseHandle(cinemSupDriver);

	}

//
//  Set IDs of different streams
//

void DVDStreamDecoder::SetStreamID(BYTE videoStreamID, BYTE ac3StreamID, BYTE audioStreamID, BYTE lpcmStreamID, BYTE spuStreamID, BYTE dtsStreamID)
	{
	this->videoStreamID = videoStreamID;
	this->ac3StreamID = ac3StreamID;
	this->audioStreamID = audioStreamID;
	this->spuStreamID = spuStreamID;
	this->lpcmStreamID = lpcmStreamID;
	this->dtsStreamID = dtsStreamID;
	}

//
//  Set audio coding mode
//

void DVDStreamDecoder::SetAudioCodingMode(MPEG2AudioType type, int samplesPerSecond, int bitsPerSample, int channels)
	{
	bool forceStartStop = (type == MP2AUDTYP_LPCM) && (this->samplesPerSecond != samplesPerSecond);

	this->samplesPerSecond = samplesPerSecond;
	this->bitsPerSample = bitsPerSample;
	this->channels = channels;

	// treat mpeg2 as mpeg1 for now
	if (type == MP2AUDTYP_MPEG2)
		{
		type = MP2AUDTYP_MPEG;
		}

	if (!streaming)
		{
		audioType = type;

		switch (audioType)
			{
			case MP2AUDTYP_AC3:
				audioDecoder = &ac3Decoder;
				break;
			case MP2AUDTYP_DTS:
				audioDecoder = &dtsDecoder;
				break;
			case MP2AUDTYP_LPCM:
				audioDecoder = &lpcmDecoder;
				break;
			case MP2AUDTYP_MPEG:
				audioDecoder = &mpegAudioDecoder;
				break;
			default:
				DP("UNKNOWN AudioType %d", audioType);
				break;
			}
		}
	else
		{
		if (type != audioType || forceStartStop)
			{
//			if (running)
//				{
//				audioDecoder->StopStreaming();
//				}
			audioDecoder->DetachStreaming();

			audioType = type;

			switch (audioType)
				{
				case MP2AUDTYP_AC3:
					audioDecoder = &ac3Decoder;
					break;
				case MP2AUDTYP_DTS:
					audioDecoder = &dtsDecoder;
					break;
				case MP2AUDTYP_LPCM:
					audioDecoder = &lpcmDecoder;
					break;
				case MP2AUDTYP_MPEG:
					audioDecoder = &mpegAudioDecoder;
					break;
				default:
					DP("UNKNOWN AudioType %d", audioType);
					break;
				}

			audioDecoder->SetAudioCodingMode(samplesPerSecond, bitsPerSample, channels);

			audioDecoder->AttachStreaming();
//			if (running)
//				{
//				audioDecoder->StartStreaming(playbackSpeed);
//				}
			}
		else
			{
			audioDecoder->SetAudioCodingMode(samplesPerSecond, bitsPerSample, channels);
			}
		}
	}

//
//  Set audio Output Config
//

void DVDStreamDecoder::SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg)
	{
	pcmWaveOut.SetAudioOutConfig(spkCfg, dualModeCfg, karaokeCfg);
	ac3Decoder.SetAudioOutConfig(spkCfg, dualModeCfg, karaokeCfg);
	lpcmDecoder.SetAudioOutConfig(spkCfg, dualModeCfg, karaokeCfg);
	mpegAudioDecoder.SetAudioOutConfig(spkCfg, dualModeCfg, karaokeCfg);
	}

//
//  Begin Streaming
//

void DVDStreamDecoder::BeginStreaming(VideoPlaybackMode playbackMode, bool highQuality, __int64 start)
	{
	this->playbackMode = playbackMode;
	this->highQuality = highQuality;

#if DUMP_STREAM_EVENTS>=1
	char buffer[100];
	wsprintf(buffer, "BeginStreaming mode %d", playbackMode);
	DTE(buffer);
#endif

	terminate = FALSE;
	signalPos = 0;
	streamInputOffset = start;
	streamingCompleted = FALSE;
	previousRefillRequestTime = 0;
	lastCheckTime = 0;
	initialVideoTime = -1;
	running = FALSE;
	splitCompletion = FALSE;
	deferedRunning = FALSE;
	initTimeStamp = 0;

	switch (streamType)
		{
		case DVST_PROGRAM_STREAM:
			audioDecoder->SetAudioCodingMode(samplesPerSecond, bitsPerSample, channels);

			videoDecoder.BeginStreaming(playbackMode, highQuality);
			if (playbackMode == VDPM_PLAY_FORWARD)
				{
				audioDecoder->BeginStreaming();
				spuDecoder.BeginStreaming(highQuality);
				if (ccDecoder) ccDecoder->BeginStreaming();
				}
			break;
		case DVST_ELEMENTARY_AUDIO_STREAM:
			audioDecoder->SetAudioCodingMode(samplesPerSecond, bitsPerSample, channels);
			audioDecoder->BeginStreaming();
			break;
		case DVST_ELEMENTARY_VIDEO_STREAM:
			videoDecoder.BeginStreaming(playbackMode, highQuality);
			break;
		}

	streaming = TRUE;

#if DUMP_STREAM_EVENTS>=1
	DTE("BeginStreaming done");
#endif
	}

//
//  Test if there are enough streams to start
//

bool DVDStreamDecoder::EnoughStreamsCompleteToStart(void)
	{
	return dataCompleted ||
		    mpeparse[DVEST_VIDEO].completed        ||
		    mpeparse[DVEST_VIDEO].completing       ||
			 mpeparse[DVEST_AUDIO].completed        ||
		    mpeparse[DVEST_AUDIO].completing       ||
			 mpeparse[DVEST_MULTIPLEXED].completed  ||
		    mpeparse[DVEST_MULTIPLEXED].completing ||
			 videoDecoder.EnoughDataToStart();
	}

//
//  Defered Start Streaming
//

void DVDStreamDecoder::DeferedStartStreaming(void)
	{
	lock.Enter();
	if (deferedRunning && !running)
		{
		switch (streamType)
			{
			case DVST_PROGRAM_STREAM:
				{
				if (playbackMode == VDPM_PLAY_FORWARD)
					{
					audioDecoder->StartStreaming(playbackSpeed);
					spuDecoder.StartStreaming(playbackSpeed);
					}
				videoDecoder.StartStreaming(playbackSpeed);

				if (ccDecoder)
				    {
				    if(playbackSpeed <= 0x10000)
				        {

                        // Playing speed is 1x (or less). It's OK to activate closed caption.
                        
				        ccDecoder->StartStreaming();
				        }
				    else
				        {

                        // Playing at higher speed (probably 2x) therefore we will not activate
                        // closed caption because it causes hiccups on the playback. We will,
                        // however, call Reset to remove any remaining captions.
				            
				        ccDecoder->Reset();
				        }
				    }

				}
				break;
			case DVST_ELEMENTARY_AUDIO_STREAM:
				audioDecoder->StartStreaming(playbackSpeed);
				break;
			case DVST_ELEMENTARY_VIDEO_STREAM:
				videoDecoder.StartStreaming(playbackSpeed);
				break;
			}

		running = TRUE;
		deferedRunning = FALSE;
		}
	lock.Leave();
	}

//
//  Start Streaming
//

void DVDStreamDecoder::StartStreaming(int playbackSpeed)
	{
#if DUMP_STREAM_EVENTS>=1
	char buffer[100];
	wsprintf(buffer, "StartStreaming %x (D %d)", playbackSpeed, streamInputPos);
	DTE(buffer);
#endif

	streamingCompleted = FALSE;
	this->playbackSpeed = playbackSpeed;

	deferedRunning = TRUE;

	if (streamInputPos > 500000 || audioInputPos > 100000 || spuInputPos > 50000 || EnoughStreamsCompleteToStart() || streamType != DVST_PROGRAM_STREAM || vobuInputPos > 2)
		DeferedStartStreaming();

#if DUMP_STREAM_EVENTS>=1
	DTE("StartStreaming done");
#endif
	}

//
//  Stop Streaming
//

void DVDStreamDecoder::StopStreaming(void)
	{
#if DUMP_STREAM_EVENTS>=1
	DTE("StopStreaming");
#endif

	if (running)
		{
		switch (streamType)
			{
			case DVST_PROGRAM_STREAM:
				videoDecoder.StopStreaming();
				if (playbackMode == VDPM_PLAY_FORWARD)
					{
					audioDecoder->StopStreaming();
					spuDecoder.StopStreaming();
					if (ccDecoder && (playbackSpeed <= 0x10000) ) ccDecoder->StopStreaming();
					}
				break;
			case DVST_ELEMENTARY_AUDIO_STREAM:
				audioDecoder->StopStreaming();
				break;
			case DVST_ELEMENTARY_VIDEO_STREAM:
				videoDecoder.StopStreaming();
				break;
			}

		running = FALSE;
		}
	else if (deferedRunning)
		{
		deferedRunning = FALSE;
		}

#if DUMP_STREAM_EVENTS>=1
	DTE("StopStreaming done");
#endif
	}

//
//  End Streaming
//

void DVDStreamDecoder::EndStreaming(bool fullReset)
	{
#if DUMP_STREAM_EVENTS>=1
	DTE("EndStreaming");
#endif

	if (streaming)
		{
		switch (streamType)
			{
			case DVST_PROGRAM_STREAM:
				videoDecoder.EndStreaming(fullReset);
				if (playbackMode == VDPM_PLAY_FORWARD)
					{
					audioDecoder->EndStreaming(fullReset);
					spuDecoder.EndStreaming(fullReset);
					if (ccDecoder) ccDecoder->EndStreaming();
					}
				break;
			case DVST_ELEMENTARY_AUDIO_STREAM:
				audioDecoder->EndStreaming(fullReset);
				break;
			case DVST_ELEMENTARY_VIDEO_STREAM:
				videoDecoder.EndStreaming(fullReset);
				break;
			}

		streaming = FALSE;
		FlushBuffer();
#if VIDEO_DUMP
		{
		if (videoStream) ::CloseHandle(videoStream);
		char buffer[100];
		videoStreamCount++;
		wsprintf(buffer, "c:\\videodump\\strm%03d.mpg", videoStreamCount);

		videoStream = ::CreateFile(buffer,
								  GENERIC_WRITE,
								  FILE_SHARE_WRITE,
								  NULL,
								  CREATE_ALWAYS,
								  FILE_ATTRIBUTE_NORMAL,
								  NULL);
		}
#endif
		}

#if DUMP_STREAM_EVENTS>=1
	DTE("EndStreaming Done");
#endif
	}

//
//  Advance Frame
//

void DVDStreamDecoder::AdvanceFrame()
	{
	videoDecoder.AdvanceFrame();
	audioDecoder->AdvanceFrame();
	}

//
//  Advance Trick Frame
//

void DVDStreamDecoder::AdvanceTrickFrame(int by)
	{
	videoDecoder.AdvanceTrickFrame(by);
	}

//
//  Enable SPU
//

void DVDStreamDecoder::EnableSPU(bool enable)
	{
	spuDecoder.EnableSPU(enable);
	}

//
//  Flush Buffer
//

void DVDStreamDecoder::FlushBuffer(void)
	{
	int i;

	for(i=0; i<4; i++)
		{
		mpeparse[i].pesState = idle;
		mpeparse[i].endPTM = 0;
		mpeparse[i].ptsOffset = 0;
		mpeparse[i].mpparse.state = PROGSTRM_INITIAL;
		mpeparse[i].mpparse.mpeg2 = FALSE;

		mpeparse[i].mpparse.ptsValid = FALSE;
		mpeparse[i].mpparse.dtsValid = FALSE;
		mpeparse[i].pendingData = NULL;
		mpeparse[i].pendingPairs = NULL;

		mpeparse[i].completing = FALSE;
		mpeparse[i].completed = FALSE;

		::SetEvent(mpeparse[i].dataEvent);
		}

	videoDecoder.FlushBuffer();
//	if (!scanning)
		{
		audioDecoder->FlushBuffer();
		spuDecoder.FlushBuffer();
		audioTracker.Flush();
		initialAudioPacket = TRUE;
		}

	videoTracker.Flush();
	audioInputPos = videoInputPos = spuInputPos = 0;
	vobuInputPos = 0;
	streamInputPos = 0;
	streamInputOffset = 0;
	lastCheckTime = 0;

	splitCompletion = FALSE;
	dataCompleted = FALSE;
	}

//
//  Internal current location
//

__int64 DVDStreamDecoder::InternalCurrentLocation(void)
	{
	__int64 videoPos, audioPos;

	if (streamingCompleted)
		{
		return streamInputPos;
		}
	else
		{
		switch (streamType)
			{
			case DVST_PROGRAM_STREAM:

				if (videoDecoder.StreamingCompleted())
					videoPos = -1;
				else
					videoPos = videoTracker.ElemToMux(videoDecoder.CurrentLocation() >> 3);

				if (playbackMode == VDPM_PLAY_FORWARD)
					{
					if (audioDecoder->StreamingCompleted())
						audioPos = -1;
					else
						audioPos = audioTracker.ElemToMux(audioDecoder->CurrentPlaybackLocation() >> 3);
					}
				else
					audioPos = -1;

				if (videoPos == -1 && audioPos == -1)
					{
					return streamInputPos + streamInputOffset;
					}
				else if (videoPos == -1)
					return audioPos + streamInputOffset;
				else
					return videoPos + streamInputOffset;
				break;
			case DVST_ELEMENTARY_AUDIO_STREAM:
				audioPos = audioDecoder->CurrentPlaybackLocation() >> 3;
				if (audioPos == -1)
					return streamInputPos + streamInputOffset;
				else
					return audioPos + streamInputOffset;
				break;
			case DVST_ELEMENTARY_VIDEO_STREAM:
				videoPos = videoDecoder.CurrentLocation() >> 3;
				if (videoPos == -1)
					return streamInputPos + streamInputOffset;
				else
					return videoPos + streamInputOffset;
				break;
			}
		}

	return 0;
	}

//
//  Current Location
//

__int64 DVDStreamDecoder::CurrentLocation(void)
	{
	DWORD time;

	time = GetInternalTime();
	if (!lastCheckTime || (time > lastCheckTime + 100))
		{
		lastCheckTime = time;
		lastCheckValue = InternalCurrentLocation();
		CheckSignal();
		}

	return lastCheckValue;
	}

//
//  Video Segment Requested
//

DWORD DVDStreamDecoder::VideoSegmentRequested(void)
	{
	return videoDecoder.VideoSegmentRequested();
	}

//
//  Set First Video Segment
//

void DVDStreamDecoder::SetFirstVideoSegment(DWORD seg)
	{
	videoDecoder.SetFirstVideoSegment(seg);
	}

//
//  Set Last Video Segment
//

void DVDStreamDecoder::SetLastVideoSegment(DWORD seg)
	{
	videoDecoder.SetLastVideoSegment(seg);
	}

//
//  Current Playback Time
//

int DVDStreamDecoder::CurrentPlaybackTime(void)
	{
	int time = display->CurrentTime();
#if DUMP_STREAM_EVENTS>=2
	char buffer[100];
	wsprintf(buffer, "Get Time %d", time);
	DTE(buffer);
#endif
	return time;
	}

//
//  Internal Refill Request
//

void DVDStreamDecoder::InternalRefillRequest(void)
	{
	bufferMonitorEvent.SetEvent();
	}

DWORD DVDStreamDecoder::SendDataMultiple(MPEGDataSizePair * data, DWORD size)
	{
	return SendDataSplitMultiple(DVEST_MULTIPLEXED, data, size);
	}

DWORD DVDStreamDecoder::SendDataSplitMultiple(DVDElementaryStreamType type, MPEGDataSizePair * data, DWORD num)
	{
	bool space;
	MPEParse * mp = mpeparse + type;

#if DUMP_STREAM_EVENTS>=2
		{
		char buffer[100];
		wsprintf(buffer, "SendData (%d) : %d at (%d)", type, num, display->CurrentTime());
		DTE(buffer);
		}
#endif

	if (streaming)
		{
		if(!running)
			{
			if (streamInputPos > 500000 || audioInputPos > 100000 || spuInputPos > 50000 || vobuInputPos > 2) DeferedStartStreaming();
			}

		if (num)
			{
			switch (streamType)
				{
				case DVST_PROGRAM_STREAM:
					switch(type)
						{
						case DVEST_VIDEO:
							space = videoDecoder.AvailBufferSpace() > (int)(num * 8);
							if (playbackMode == VDPM_PLAY_FORWARD && initialVideoTime >= 0 && initialAudioPacket && videoDecoder.AvailBits() > 256000 * 8)
								{
#if DUMP_STREAM_EVENTS>=2
		{
		char buffer[100];
		wsprintf(buffer, "SetVideoStartTime %d VideoBits %d", initialVideoTime, (DWORD)(videoDecoder.AvailBits()));
		DTE(buffer);
		}
#endif
								display->SetVideoStartTiming(initialVideoTime);
								initialVideoTime = -2;
								}
							break;
						case DVEST_AUDIO:
							space = audioDecoder->AvailBufferSpace() > (int)(num * 8);
							break;
						case DVEST_SUBPICTURE:
							space = spuDecoder.AvailBufferSpace() > (int)(num * 8);
							break;
						case DVEST_MULTIPLEXED:
							space = videoDecoder.AvailBufferSpace() > 65536 * 8 &&
										(playbackMode != VDPM_PLAY_FORWARD || audioDecoder->AvailBufferSpace() > 8192 * 8);
							break;
						}
					break;
				case DVST_ELEMENTARY_AUDIO_STREAM:
					space = audioDecoder->AvailBufferSpace() > 8192 * 8;
					break;
				case DVST_ELEMENTARY_VIDEO_STREAM:
					space = videoDecoder.AvailBufferSpace() > 65536 * 8;
					break;
				}

#if DUMP_STREAM_EVENTS>=3
		{
		char buffer[100];
		wsprintf(buffer, "SendData (%d) : space %d", space);
		DTE(buffer);
		}
#endif

			if (space)
				{
				::ResetEvent(mpeparse[type].dataEvent);
				mpeparse[type].pendingNum = num;
				mpeparse[type].pendingPairs = data;
				mpeparse[type].pendingTimeStamp = -1;
				bufferMonitorEvent.SetEvent();
				while (mpeparse[type].pendingPairs)
					{
					::WaitForSingleObject(mpeparse[type].dataEvent, INFINITE);
					}

#if DUMP_STREAM_EVENTS>=3
		{
		char buffer[100];
		wsprintf(buffer, "SendData (%d) : result %d", type, mpeparse[type].pendingNum);
		DTE(buffer);
		}
#endif
				return mpeparse[type].pendingNum;
				}
			else
				{
				if (playbackMode == VDPM_PLAY_FORWARD)
					{
					if (initialVideoTime >= 0 && initialAudioPacket)
						{
						display->SetVideoStartTiming(initialVideoTime);
						initialVideoTime = -2;
						}
					}

				if (type == DVEST_MULTIPLEXED) DeferedStartStreaming();

				return 0;
				}
			}
		else
			return 0;
		}
	else
		return 0;
	}

//
//  Send Data
//

DWORD DVDStreamDecoder::SendData(BYTE * ptr, DWORD num)
	{
	return SendDataSplit(DVEST_MULTIPLEXED, ptr, num, -1);
	}

//
//  Send Data Split
//

#include <stdio.h>

DWORD DVDStreamDecoder::SendDataSplit(DVDElementaryStreamType type, BYTE * ptr, DWORD num, int timeStamp)
	{
	bool space;
	MPEParse * mp = mpeparse + type;

#if DUMP_STREAM_EVENTS>=2
		{
		char buffer[100];
		wsprintf(buffer, "SendData (%d) : %d at %d (%d)", type, num, timeStamp, display->CurrentTime());
		DTE(buffer);
		}
#endif

	if (streaming)
		{
		if(!running)
			{
			if (streamInputPos > 500000 || audioInputPos > 100000 || spuInputPos > 50000 || vobuInputPos > 2) DeferedStartStreaming();
			else if (type == DVEST_VIDEO && initialVideoTime >=0 && timeStamp >= 0 && timeStamp >= initialVideoTime + 500) DeferedStartStreaming();
			else
				{
				if (!streamInputPos)
					{
					initTimeStamp = timeStamp;
					}
				else if ((timeStamp - initTimeStamp) > 500)
					{
					PMMSG("diff in TimeStamps forced DfrdStrtStrmng\n");
					DeferedStartStreaming();
					}
				}
			}

		if (num)
			{
			if (mp->streamType == DVSST_ELEMENTARY)
				{
	//			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
				int done;

				switch (type)
					{
					case DVEST_VIDEO:
						if (initialVideoTime == -1) initialVideoTime = timeStamp;
						if (timeStamp >= 0) videoDecoder.AddTimeStamp(timeStamp);
						videoTracker.AddTrack(streamInputPos, videoInputPos);
						done = videoDecoder.SendData(ptr, num);
						streamInputPos += done;
						videoInputPos += done;
						break;
					case DVEST_AUDIO:
						if (timeStamp >= 0) audioDecoder->AddTimeStamp(timeStamp);
						audioTracker.AddTrack(streamInputPos, audioInputPos);
						done =  audioDecoder->SendData(ptr, num);
						streamInputPos += done;
						audioInputPos += done;
						break;
					case DVEST_SUBPICTURE:
						if (timeStamp >= 0) spuDecoder.AddTimeStamp(timeStamp);
						done =  spuDecoder.SendData(ptr, num);
						streamInputPos += done;
						spuInputPos += done;
						break;
					default:
						done = num;
					}

				return done;
				}

			switch (streamType)
				{
				case DVST_PROGRAM_STREAM:
					switch(type)
						{
						case DVEST_VIDEO:
							space = videoDecoder.AvailBufferSpace() > (int)(num * 8);
							if (playbackMode == VDPM_PLAY_FORWARD && initialVideoTime >= 0 && initialAudioPacket && videoDecoder.AvailBits() > 256000 * 8)
								{
								display->SetVideoStartTiming(initialVideoTime);
								initialVideoTime = -2;
								}
							break;
						case DVEST_AUDIO:
							space = audioDecoder->AvailBufferSpace() > (int)(num * 8);

							//
							// Startup delay for audio gap
							//
							if (initialVideoTime == -2 && initialAudioPacket && timeStamp >= 0 &&
								 timeStamp > display->CurrentTime() + 100)
								{
								return 0;
								}
							// add workaround for 96k perfomance problem
							else if (num == 2048 && space && (ptr[20] & 0x30) == 0 && audioType == MP2AUDTYP_LPCM && samplesPerSecond == 96000)
								{
								return InternalSendDataSplit(type, ptr, num, timeStamp);
								}

							break;
						case DVEST_SUBPICTURE:
							space = spuDecoder.AvailBufferSpace() > (int)(num * 8);
							break;
						case DVEST_MULTIPLEXED:
							space = videoDecoder.AvailBufferSpace() > 65536 * 8 &&
										(playbackMode != VDPM_PLAY_FORWARD || audioDecoder->AvailBufferSpace() > 8192 * 8);
							break;
						}
					break;
				case DVST_ELEMENTARY_AUDIO_STREAM:
					space = audioDecoder->AvailBufferSpace() > 8192 * 8;
					break;
				case DVST_ELEMENTARY_VIDEO_STREAM:
					space = videoDecoder.AvailBufferSpace() > 65536 * 8;
					break;
				}

#if DUMP_STREAM_EVENTS>=3
		{
		char buffer[100];
		wsprintf(buffer, "SendData (%d) : space %d", space);
		DTE(buffer);
		}
#endif

			if (space)
				{
				::ResetEvent(mpeparse[type].dataEvent);
				mpeparse[type].pendingNum = num;
				mpeparse[type].pendingData = ptr;
				mpeparse[type].pendingTimeStamp = timeStamp;
				bufferMonitorEvent.SetEvent();
				while (mpeparse[type].pendingData)
					{
					::WaitForSingleObject(mpeparse[type].dataEvent, INFINITE);
					}

#if DUMP_STREAM_EVENTS>=3
		{
		char buffer[100];
		wsprintf(buffer, "SendData (%d) : result %d", type, mpeparse[type].pendingNum);
		DTE(buffer);
		}
#endif
				return mpeparse[type].pendingNum;
				}
			else
				{
				if (playbackMode == VDPM_PLAY_FORWARD)
					{
					if (initialVideoTime >= 0 && initialAudioPacket)
						{
						display->SetVideoStartTiming(initialVideoTime);
						initialVideoTime = -2;
						}
					}

				if (type == DVEST_MULTIPLEXED) DeferedStartStreaming();

				return 0;
				}
			}
		else
			return 0;
		}
	else
		return 0;
	}

//
//  DVD Stream Decoder Supervisor Constructor
//

DVDStreamDecoder::Supervisor::Supervisor(DVDStreamDecoder * decoder)
	: TimedFiber(1)
	{
	this->decoder = decoder;
	}

//
//  Supervisor Supervisor Fiber Routine
//

void DVDStreamDecoder::Supervisor::FiberRoutine(void)
	{
	SetFiberPriority(3);
	decoder->SupervisorFiberRoutine();
	}

//
//  Supervisor Fiber Routine
//

void DVDStreamDecoder::SupervisorFiberRoutine(void)
	{
	int videoAvail, audioAvail;

	while (!terminate)
		{
		switch (streamType)
			{
			case DVST_PROGRAM_STREAM:
				videoAvail = videoDecoder.AvailBufferSpace();
				perfmon.videoBitBufferFill = perfmon.videoBitBufferSize - videoAvail;
				audioAvail = audioDecoder->AvailBufferSpace();
				perfmon.audioBitBufferFill = perfmon.audioBitBufferSize - audioAvail;
				perfmon.audioSampleBufferSize = 4500;
				audioAvail = audioDecoder->TotalAudioBufferTime();
				perfmon.audioSampleBufferFill = audioAvail;

				if (!dataCompleted)
					{
					if (videoAvail > 6 * 65536 * 8 || audioAvail < 600)
						{
						SetFiberPriority(3);
						hurryUp = TRUE;
						}
					else if (videoAvail > 4 * 65536 * 8 || audioAvail < 1000)
						{
						SetFiberPriority(2);
						}
					}
				break;
			case DVST_ELEMENTARY_VIDEO_STREAM:
				videoAvail = videoDecoder.AvailBufferSpace();
				perfmon.videoBitBufferFill = perfmon.videoBitBufferSize - videoAvail;
				if (!dataCompleted)
					{
					if (videoAvail > 6 * 65536 * 8)
						{
						SetFiberPriority(3);
						hurryUp = TRUE;
						}
					else if (videoAvail > 4 * 65536 * 8)
						{
						SetFiberPriority(2);
						}
					}
				break;
			case DVST_ELEMENTARY_AUDIO_STREAM:
				audioAvail = audioDecoder->AvailBufferSpace();
				perfmon.audioBitBufferFill = perfmon.audioBitBufferSize - audioAvail;
				perfmon.audioSampleBufferSize = 4500;
				audioAvail = audioDecoder->TotalAudioBufferTime();
				perfmon.audioSampleBufferFill = audioAvail;
				break;
			}

		YieldTimedFiber(100);
		}
	}

//
//  Flush Debug Registers
//

static void __stdcall FlushDebugRegister(void * data)
	{
	__asm
		{
		mov	eax, dr7
		and	eax, 0xffffff00
		mov	dr7, eax
		xor	eax, eax
		mov	dr0, eax
		mov	dr1, eax
		mov	dr2, eax
		mov	dr3, eax
		}
	}

//
//  Fiber Routine
//

typedef void (__stdcall * Ring0Call)(void *);

void DVDStreamDecoder::FiberRoutine(void)
	{
	int videoAvail, audioAvail;
	int previousRequest;
	DWORD done;
	int type;

	previousRequest = GetInternalTime();

	while (!terminate)
		{
		bufferMonitorEvent.Wait(100);

		if (streaming) CheckSignal();

		switch (streamType)
			{
			case DVST_PROGRAM_STREAM:
#if PROTECTED
				{
				struct ProcCall pc;

				pc.call = FlushDebugRegister;
				pc.data = NULL;

				BEGIN_IOCTL_CALL(TEST)
					in.call = &pc;
					DO_IOCTL_WITH_DRIVER(TEST, cinemSupDriver);
				END_IOCTL_CALL
				}
#endif
				videoAvail = videoDecoder.AvailBufferSpace();
				audioAvail = audioDecoder->TotalAudioBufferTime();
				if (!dataCompleted)
					{
					if (videoAvail > 6 * 65536 * 8 || audioAvail < 600)
						{
						SetFiberPriority(3);
						hurryUp = TRUE;
						}
					else if (videoAvail > 4 * 65536 * 8 || audioAvail < 1000)
						{
						SetFiberPriority(2);
						}
					else
						{
						SetFiberPriority(1);
						}
					}
				else
					{
					SetFiberPriority(1);
					}
#if DUMP_STREAM_EVENTS>=4
	char buffer[100];
	wsprintf(buffer, "DemuxFiber video : %d audio : %d times %d / %d", videoAvail, audioAvail, previousRequest, GetInternalTime());
	DTE(buffer);
#endif

				for(type=0; type<4; type++)
					{
					if (mpeparse[type].pendingData)
						{
						done = InternalSendDataSplit((DVDElementaryStreamType)type, ((BYTE *)(mpeparse[type].pendingData)), mpeparse[type].pendingNum, mpeparse[type].pendingTimeStamp);
						}
					else if (mpeparse[type].pendingPairs)
						{
						DWORD res;
						DWORD num = mpeparse[type].pendingNum;
						volatile MPEGDataSizePair * data = mpeparse[type].pendingPairs;

						done = 0;
						while (done < num)
							{
							res = InternalSendDataSplit((DVDElementaryStreamType)type, (BYTE *)(data->data), data->size, data->timeStamp);

							done += res;
							if (res != data->size) break;

							data++;
							}
						}
					else
						continue;

					if (done != mpeparse[type].pendingNum)
						{
						SetFiberPriority(1);
						hurryUp = FALSE;
						}

					mpeparse[type].pendingNum = done;
					mpeparse[type].pendingData = NULL;
					mpeparse[type].pendingPairs = NULL;
					::SetEvent(mpeparse[type].dataEvent);
					if (hurryUp)
						{
//						Sleep(0);
						YieldTimedFiber(1);
//						DP("Force Sleep");
						}
					}

				if (streaming && videoAvail > 2 * 65536 * 8 &&
							(playbackMode != VDPM_PLAY_FORWARD || audioDecoder->AvailBufferSpace() > 8192 * 8) &&
							previousRequest + 200 < GetInternalTime())
					{
					RefillRequest();
					previousRequest = GetInternalTime();
					}
				break;
			case DVST_ELEMENTARY_AUDIO_STREAM:
				if (mpeparse[DVEST_MULTIPLEXED].pendingData)
					{
					done = audioDecoder->SendData(((BYTE *)(mpeparse[DVEST_MULTIPLEXED].pendingData)), mpeparse[DVEST_MULTIPLEXED].pendingNum);

#if AUDIO_DUMP
					DWORD dummy;
					::WriteFile(audioStream, (void *)(mpeparse[DVEST_MULTIPLEXED].pendingData), done, &dummy, NULL);
#endif
						streamInputPos += done;

					if (done != mpeparse[DVEST_MULTIPLEXED].pendingNum)
						{
						SetFiberPriority(1);
						hurryUp = FALSE;
						}

					mpeparse[DVEST_MULTIPLEXED].pendingNum = done;
					mpeparse[DVEST_MULTIPLEXED].pendingData = NULL;
					::SetEvent(mpeparse[DVEST_MULTIPLEXED].dataEvent);
					YieldTimedFiber(1);
					}

				if (streaming && audioDecoder->AvailBufferSpace() > 8192 * 8 &&
							previousRequest + 200 < GetInternalTime())
					{
					RefillRequest();
					previousRequest = GetInternalTime();
					}
				break;
			case DVST_ELEMENTARY_VIDEO_STREAM:
				videoAvail = videoDecoder.AvailBufferSpace();

				if (mpeparse[DVEST_MULTIPLEXED].pendingData)
					{
					done = videoDecoder.SendData(((BYTE *)(mpeparse[DVEST_MULTIPLEXED].pendingData)), mpeparse[DVEST_MULTIPLEXED].pendingNum);
					streamInputPos += done;

					if (done != mpeparse[DVEST_MULTIPLEXED].pendingNum)
						{
						SetFiberPriority(1);
						hurryUp = FALSE;
						}

					mpeparse[DVEST_MULTIPLEXED].pendingNum = done;
					mpeparse[DVEST_MULTIPLEXED].pendingData = NULL;
					::SetEvent(mpeparse[DVEST_MULTIPLEXED].dataEvent);
					YieldTimedFiber(1);
					}

				if (streaming && videoAvail > 2 * 65536 * 8 &&
							previousRequest + 200 < GetInternalTime())
					{
					RefillRequest();
					previousRequest = GetInternalTime();
					}
				break;
			}

		}
	}

//
//  Check Signal
//

void DVDStreamDecoder::CheckSignal(void)
	{
	__int64 pos;
	bool completed;

	lock.Enter();

	if (!streamingCompleted)
		{
		pos = InternalCurrentLocation();

		if (signalPos > 0 && pos >= signalPos)
			{
			pos = signalPos;
			signalPos = 0;

			SignalReached(pos);
			}

		if (playbackMode != VDPM_SCANNING && playbackMode != VDPM_TRICKPLAY)
			{
			if (splitCompletion)
				{
				if (videoDecoder.StreamingCompleted())
					{
					if (mpeparse[DVEST_VIDEO].completing && !mpeparse[DVEST_VIDEO].completed)
						{
						mpeparse[DVEST_VIDEO].completed = TRUE;
#if DUMP_STREAM_EVENTS>=1
						DTE("Completion message 0");
#endif
						StreamCompleted(DVEST_VIDEO);
						}
					}
				if (audioDecoder->StreamingCompleted())
					{
					if (mpeparse[DVEST_AUDIO].completing && !mpeparse[DVEST_AUDIO].completed)
						{
						mpeparse[DVEST_AUDIO].completed = TRUE;
#if DUMP_STREAM_EVENTS>=1
						DTE("Completion message 1");
#endif
						StreamCompleted(DVEST_AUDIO);
						}
					}
				if (spuDecoder.StreamingCompleted())
					{
					if (mpeparse[DVEST_SUBPICTURE].completing && !mpeparse[DVEST_SUBPICTURE].completed)
						{
						mpeparse[DVEST_SUBPICTURE].completed = TRUE;
#if DUMP_STREAM_EVENTS>=1
						DTE("Completion message 2");
#endif
						StreamCompleted(DVEST_SUBPICTURE);
						}
					}
				}
			else if (dataCompleted)
				{
				switch (streamType)
					{
					case DVST_PROGRAM_STREAM:
						completed = videoDecoder.StreamingCompleted() &&
										 (playbackMode != VDPM_PLAY_FORWARD || audioDecoder->StreamingCompleted());
						break;
					case DVST_ELEMENTARY_AUDIO_STREAM:
						completed = audioDecoder->StreamingCompleted();
						break;
					case DVST_ELEMENTARY_VIDEO_STREAM:
						completed = videoDecoder.StreamingCompleted();
						break;
					}

				if (completed)
					{
					if (signalPos > 0 && signalPos <= streamInputPos + streamInputOffset)
						{
						pos = signalPos;
						signalPos = 0;

						SignalReached(pos);
						}

#if DUMP_STREAM_EVENTS>=1
					DTE("Completed");
#endif

					streamingCompleted = TRUE;
					StreamCompleted(DVEST_MULTIPLEXED);
					}
				}
			}
		}

	lock.Leave();
	}

//
//  Scan Completed
//

void DVDStreamDecoder::ScanCompleted(void)
	{
	bool split;

	if (playbackMode == VDPM_SCANNING && !terminate)
		{
		split = splitCompletion;

		FlushBuffer();

		if (split)
			{
#if DUMP_STREAM_EVENTS>=1
			DTE("Scan Completion message 0");
#endif
			StreamCompleted(DVEST_VIDEO);
			}
		else
			{
#if DUMP_STREAM_EVENTS>=1
			DTE("Scan Completion message");
#endif
			StreamCompleted(DVEST_MULTIPLEXED);
			}
		}
	}

//
//  Set Signal Position
//

void DVDStreamDecoder::SetSignalPosition(__int64 pos)
	{
	signalPos = pos;
	CheckSignal();
	}

//
//  Some Defines For Splitting
//

#define STREAMERROR	{GOTO(PROGSTRM_RECOVER1);}

#define CHECK(b, check)			(((b) & HIBYTE(check)) == LOBYTE(check))
#define BLOCK(x)					{x}

#define CONSUME(b)				BLOCK(if (rest) {b = *p++; rest--;} else {streamInputPos += size; return size;})
#define CONSUMEIF(b, check)	BLOCK(if (rest) {if (!CHECK(*p, check)) STREAMERROR else {b = *p++; rest--;}} else {streamInputPos += size; return size;})
#define REJECT						BLOCK(STREAMERROR;)
#define SKIP(n) 					BLOCK(if (rest>=(DWORD)n) {rest-=(DWORD)n; p+=n;} else {(DWORD &)n-=rest; streamInputPos += size; return size;})
#define PROCEED					BLOCK(mp->mpparse.state++;)
#define GOTO(s)   				BLOCK(mp->mpparse.state = s; break;)
#define ACCEPT(check)			BLOCK(if (rest) {if (!CHECK(*p, check)) STREAMERROR else {p++; rest--; mp->mpparse.state++;}} else {streamInputPos += size; return size;})
#define ACCEPTNGO(check, s)	BLOCK(if (rest) {if (!CHECK(*p, check)) STREAMERROR else {p++; rest--; mp->mpparse.state=s;break;}} else {streamInputPos += size; return size;})

//
//  Internal Advance
//

void DVDStreamDecoder::InternalAdvance(MPEParse * mp, int by, BYTE * & ptr, DWORD & num)
	{
	mp->pesOffset += by;
	ptr += by;
	num -= by;
	streamInputPos += by;
	}

//
//  Internal Get Data
//

BYTE DVDStreamDecoder::InternalGetData(MPEParse * mp, int at, BYTE * ptr)
	{
	if (!(mp->encryptedBlock) || mp->pesOffset + at < 128)
		return ptr[at];
	else
		return mp->decBuffer[mp->pesOffset + at - 128];
	}

//
//  Internal Send Data
//

DWORD DVDStreamDecoder::InternalSendData(BYTE * ptr, DWORD num)
	{
	return InternalSendDataSplit(DVEST_MULTIPLEXED, ptr, num, -1);
	}

//
//  Set DVD Demux
//

void DVDStreamDecoder::SetDVDDemux(bool dvdDemux)
	{
	if (dvdDemux)
		mpeparse[DVEST_MULTIPLEXED].streamType = DVSST_DVD_ENCRYPTED;
	else
		mpeparse[DVEST_MULTIPLEXED].streamType = DVSST_PROGRAM;
	}

//
//  Set Split Stream Type
//

void DVDStreamDecoder::SetSplitStreamType(DVDElementaryStreamType type, DVDSplitStreamType streamType)
	{
	mpeparse[type].streamType = streamType;
	}

//
//  Internal Send Data Split
//

DWORD DVDStreamDecoder::InternalSendDataSplit(DVDElementaryStreamType type, BYTE * ptr, DWORD num, int externalTimeStamp)
	{
	MPEParse * mp = mpeparse + type;
	int timeStamp;

	switch (mp->streamType)
		{
		case DVSST_DVD_ENCRYPTED:
			{
			int done, initial;
			DWORD pts;
			int startPTM;
			bool	skipPacket;
			BYTE	*	bptr;
			BYTE h, x;

			initial = num;

			while (num)
				{
				switch (mp->pesState)
					{
					case idle:
						if (ptr[0] != 0x00 || ptr[1] != 0x00 ||
							 ptr[2] != 0x01 || ptr[3] != 0xba)
							{
#if DUMP_STREAM_EVENTS>=1
							::DTE("Error in bitstream");
#endif
							while (num >= 2048 && (ptr[0] != 0x00 || ptr[1] != 0x00 || ptr[2] != 0x01 || ptr[3] != 0xba))
								{
								num--;
								ptr++;
								streamInputPos++;
								}

							if (num < 2048)
								{
								streamInputPos += num;

								return initial;
								}
							}

	#if INDUCE_ERRORS_IN_BITSTREAM
							{
							static DWORD s0 = 0x1341231;
							static DWORD s1 = 0x4123534;

							s0 = s0 * 32142 + 523153;
							s1 = s1 * 532123 + 2134;

							s0 ^= s1 >> 16;
							s1 ^= s0 >> 27;

							if ((s0 & 0xf) == 0)
								{
								::OutputDebugString("E");
								ptr[(s1 >> 8) & 0x07ff] = s1 & 0xff;
								}
							}
	#endif

						bptr = ptr;
						mp->pesRest = ((int)ptr[18] << 8) + (int)ptr[19];
						if (mp->pesRest > 2048 - 20)
							{
#if DUMP_STREAM_EVENTS>=1
							::DTE("Error in bitstream");
#endif
							ptr += 2048;
							num -= 2048;
							streamInputPos += 2048;

							continue;
							}

						if (ptr[17] == 0xbb)
							{
							startPTM = ((DWORD)(ptr[57]) << 23) |
										  ((DWORD)(ptr[58]) << 15) |
										  ((DWORD)(ptr[59]) <<  7) |
										  ((DWORD)(ptr[60]) >>  1);
							mp->ptsOffset += mp->endPTM - startPTM;
							mp->endPTM   = ((DWORD)(ptr[61]) << 23) |
										  ((DWORD)(ptr[62]) << 15) |
										  ((DWORD)(ptr[63]) <<  7) |
										  ((DWORD)(ptr[64]) >>  1);

							ptr += 2048;
							num -= 2048;
							streamInputPos += 2048;
							if (playbackMode == VDPM_PLAY_BACKWARD)
								{
								BYTE EOS[] = {0x00, 0x00, 0x01, 0xb7};
								videoDecoder.SendData(EOS, 4);
								}
							vobuInputPos++;
							}
						else
							{
							skipPacket = FALSE;

							if (externalTimeStamp >= 0)
								{
								timeStamp = externalTimeStamp;
								externalTimeStamp = -2;
								}
							else if (ptr[21] & 0xc0 && externalTimeStamp == -1)
								{
								pts = ((DWORD)((ptr[23] >> 1) & 0x07) << 29) |
										((DWORD)( ptr[24]				   ) << 21) |
										((DWORD)((ptr[25] >> 1) & 0x7f) << 14) |
										((DWORD)( ptr[26]             ) <<  6) |
										((DWORD)((ptr[27] >> 2) & 0x3f));

								timeStamp = pts / 45 + mp->ptsOffset / 45;

								if (timeStamp < 0 && playbackMode != VDPM_TRICKPLAY)
									{
									skipPacket = TRUE;
									}

								}
							else
								timeStamp = -1;
#if DUMP_STREAM_EVENTS>=2
							if (timeStamp >= 0)
								{
								char buffer[100];
								wsprintf(buffer, "TIME %d (%02x): %8d", type, ptr[17], timeStamp);
								DTE(buffer);
								}
#endif
							if (skipPacket)
								{
								ptr += 2048;
								num -= 2048;
								streamInputPos += 2048;
								}
							else if (ptr[17] == videoStreamID)
								{
								mp->pesOffset = ptr[22];
								mp->pesRest -= (mp->pesOffset+3);
								mp->pesOffset += 23;
								mp->pesSkip = 2048 - mp->pesRest - mp->pesOffset;
								ptr += mp->pesOffset; num -= mp->pesOffset;
								mp->pesState = videoData;
								if (initialVideoTime == -1 && timeStamp >= 0) initialVideoTime = timeStamp;
								if (timeStamp >= 0) videoDecoder.AddTimeStamp(timeStamp);
								videoTracker.AddTrack(streamInputPos, videoInputPos);
								streamInputPos += mp->pesOffset;
								videoInputPos += mp->pesRest;
								}
							else if (playbackMode == VDPM_PLAY_FORWARD && audioType == MP2AUDTYP_MPEG && (ptr[17] & 0xef) == audioStreamID && (!initialAudioPacket || timeStamp >= 0))
								{
								mp->pesState = audioHeaderMpeg;
								mp->pesOffset = 14;
								if (timeStamp >= 0) audioDecoder->AddTimeStamp(timeStamp);
								audioTracker.AddTrack(streamInputPos, audioInputPos);
								streamInputPos += mp->pesOffset;
								ptr += mp->pesOffset; num -= mp->pesOffset;

								initialAudioPacket = FALSE;
								}
							else if (playbackMode == VDPM_PLAY_FORWARD && audioType == MP2AUDTYP_AC3 && ptr[17] == 0xbd && ptr[23 + ptr[22]] == ac3StreamID)
								{
								mp->pesOffset = ptr[22] + 4;
								mp->pesRest -= (mp->pesOffset+3);
								mp->pesOffset += 23;
								if (initialAudioPacket)
									{
									int delta = (ptr[mp->pesOffset - 2] << 8) + ptr[mp->pesOffset - 1] - 1;
									mp->pesOffset += delta;
									mp->pesRest -= delta;
									initialAudioPacket = FALSE;
									}

								mp->pesSkip = 2048 - mp->pesRest - mp->pesOffset;

								ptr += mp->pesOffset; num -= mp->pesOffset;

								mp->pesState = audioData;
								if (timeStamp >= 0) audioDecoder->AddTimeStamp(timeStamp);
								audioTracker.AddTrack(streamInputPos, audioInputPos);
								streamInputPos += mp->pesOffset;
								audioInputPos += mp->pesRest;
								}
							else if (playbackMode == VDPM_PLAY_FORWARD && audioType == MP2AUDTYP_DTS && ptr[17] == 0xbd && ptr[23 + ptr[22]] == dtsStreamID)
								{
								mp->pesOffset = ptr[22] + 4;
								mp->pesRest -= (mp->pesOffset+3);
								mp->pesOffset += 23;
								if (initialAudioPacket)
									{
//                     Spec outlines that this will always be set to 0001.  There are titles authored
//                     in error with bytes set to 0000, ignoring these bytes will avoid the issue.
//									int delta = (ptr[mp->pesOffset - 2] << 8) + ptr[mp->pesOffset - 1] - 1;
//									mp->pesOffset += delta;
//									mp->pesRest -= delta;
									initialAudioPacket = FALSE;
									}

								mp->pesSkip = 2048 - mp->pesRest - mp->pesOffset;

								ptr += mp->pesOffset; num -= mp->pesOffset;

								mp->pesState = audioData;
								if (timeStamp >= 0) audioDecoder->AddTimeStamp(timeStamp);
								audioTracker.AddTrack(streamInputPos, audioInputPos);
								streamInputPos += mp->pesOffset;
								audioInputPos += mp->pesRest;
								}
							else if (playbackMode == VDPM_PLAY_FORWARD && audioType == MP2AUDTYP_LPCM && ptr[17] == 0xbd && ptr[23 + ptr[22]] == lpcmStreamID)
								{
								mp->pesOffset = ptr[22] + 7;
								mp->pesRest -= (mp->pesOffset+3);
								mp->pesOffset += 23;
								mp->pesSkip = 2048 - mp->pesRest - mp->pesOffset;
								ptr += mp->pesOffset; num -= mp->pesOffset;
								mp->pesState = audioData;
								if (timeStamp >= 0) lpcmDecoder.AddTimeStamp(timeStamp);
								audioTracker.AddTrack(streamInputPos, audioInputPos);
								streamInputPos += mp->pesOffset;
								audioInputPos += mp->pesRest;

								initialAudioPacket = FALSE;
		#if AUDIO_DUMP
								DWORD dummy;
								::WriteFile(audioStream, ptr, mp->pesRest, &dummy, NULL);
		#endif
								}
							else if (playbackMode == VDPM_PLAY_FORWARD && ptr[17] == 0xbd && ptr[23 + ptr[22]] == spuStreamID)
								{
								mp->pesOffset = ptr[22] + 1;
								mp->pesRest -= (mp->pesOffset+3);
								mp->pesOffset += 23;
								mp->pesSkip = 2048 - mp->pesRest - mp->pesOffset;
								ptr += mp->pesOffset; num -= mp->pesOffset;
								mp->pesState = spuData;
								if (timeStamp >= 0) spuDecoder.AddTimeStamp(timeStamp);
		//						audioTracker.AddTrack(streamInputPos, videoInputPos);
								streamInputPos += mp->pesOffset;
								spuInputPos += mp->pesRest;
								}
							else
								{
								ptr += 2048;
								num -= 2048;
								streamInputPos += 2048;
								}

							if (mp->pesState != idle && mp->pesOffset + mp->pesRest > 128 && bptr[20] & 0x30)
								{
								if (decryptor) decryptor->DecryptBlock(bptr, mp->decBuffer);
#if VIDEO_DUMP==2
								if (mp->pesState == videoData)
									{
									DWORD dummy;
									::WriteFile(videoStream, bptr, 128, &dummy, NULL);
									::WriteFile(videoStream, mp->decBuffer, 2048 - 128, &dummy, NULL);
									}
#endif

								mp->encryptedBlock = TRUE;
								}
							else
								{
#if VIDEO_DUMP==2
								if (mp->pesState == videoData)
									{
									DWORD dummy;
									::WriteFile(videoStream, bptr, 2048, &dummy, NULL);
									}
#endif

								mp->encryptedBlock = FALSE;
								}
							}
						break;
					case videoData:
						if (mp->encryptedBlock)
							{
							if (mp->pesOffset < 128)
								{
								done = videoDecoder.SendData(ptr, 128 - mp->pesOffset);
#if VIDEO_DUMP==1
								DWORD dummy;
								::WriteFile(videoStream, ptr, done, &dummy, NULL);
#endif

								mp->pesRest -= done;
								mp->pesOffset += done;
								ptr += done;
								num -= done;
								streamInputPos += done;
								if (mp->pesOffset < 128) return initial - num;
								}

							done = videoDecoder.SendData(mp->decBuffer +(mp->pesOffset - 128), mp->pesRest);
#if VIDEO_DUMP==1
							DWORD dummy;
							::WriteFile(videoStream, mp->decBuffer +(mp->pesOffset - 128), done, &dummy, NULL);
#endif
							}
						else
							{
							done = videoDecoder.SendData(ptr, mp->pesRest);
#if VIDEO_DUMP==1
							DWORD dummy;
							::WriteFile(videoStream, ptr, done, &dummy, NULL);
#endif
							}

						mp->pesRest -= done;
						mp->pesOffset += done;
						ptr += done;
						num -= done;
						streamInputPos += done;
						if (mp->pesRest) return initial - num;
						ptr += mp->pesSkip;
						num -= mp->pesSkip;
						streamInputPos += mp->pesSkip;
						mp->pesState = idle;
						break;
					case audioHeaderMpeg:

						if (mp->pesOffset < 2040)
							{
							h = InternalGetData(mp, 3, ptr);
							mp->pesRest = ((int)(InternalGetData(mp, 4, ptr)) << 8) + (int)(InternalGetData(mp, 5, ptr));
							x = InternalGetData(mp, 8, ptr);
							InternalAdvance(mp, 9 + x, ptr, num);
							mp->pesRest -= 3 + x;
							if (h == audioStreamID)
								{
								audioInputPos += mp->pesRest;
								mp->pesState = audioDataMpeg;
								}
							else
								{
								InternalAdvance(mp, mp->pesRest, ptr, num);
								}
							}
						else
							{
							InternalAdvance(mp, 2048 - mp->pesOffset, ptr, num);
							mp->pesState = idle;
							}
						break;
					case audioDataMpeg:
						if (mp->encryptedBlock)
							{
							if (mp->pesOffset < 128)
								{
								done = audioDecoder->SendData(ptr, 128 - mp->pesOffset);
								mp->pesRest -= done;
								mp->pesOffset += done;
								ptr += done;
								num -= done;
								streamInputPos += done;
								if (mp->pesOffset < 128) return initial - num;
								}

							done = audioDecoder->SendData(mp->decBuffer + (mp->pesOffset - 128), mp->pesRest);
							}
						else
							{
							done = audioDecoder->SendData(ptr, mp->pesRest);
							}

						mp->pesRest -= done;
						mp->pesOffset += done;
						ptr += done;
						num -= done;
						streamInputPos += done;
						if (mp->pesRest) return initial - num;
						mp->pesState = audioHeaderMpeg;
						break;
					case audioData:
						if (mp->encryptedBlock)
							{
							if (mp->pesOffset < 128)
								{
								done = audioDecoder->SendData(ptr, 128 - mp->pesOffset);
		#if AUDIO_DUMP
								DWORD dummy;
								::WriteFile(audioStream, ptr, done, &dummy, NULL);
		#endif
								mp->pesRest -= done;
								mp->pesOffset += done;
								ptr += done;
								num -= done;
								streamInputPos += done;
								if (mp->pesOffset < 128) return initial - num;
								}

							done = audioDecoder->SendData(mp->decBuffer + (mp->pesOffset - 128), mp->pesRest);
	#if AUDIO_DUMP
							DWORD dummy;
							::WriteFile(audioStream, mp->decBuffer +(mp->pesOffset - 128), done, &dummy, NULL);
	#endif
							}
						else
							{
							done = audioDecoder->SendData(ptr, mp->pesRest);
	#if AUDIO_DUMP
							DWORD dummy;
							::WriteFile(audioStream, ptr, done, &dummy, NULL);
	#endif
							}

						mp->pesRest -= done;
						mp->pesOffset += done;
						ptr += done;
						num -= done;
						streamInputPos += done;
						if (mp->pesRest) return initial - num;

						ptr += mp->pesSkip;
						num -= mp->pesSkip;
						streamInputPos += mp->pesSkip;
						mp->pesState = idle;
						break;
					case spuData:
						if (mp->encryptedBlock)
							{
							if (mp->pesOffset < 128)
								{
								done = spuDecoder.SendData(ptr, 128 - mp->pesOffset);
								mp->pesRest -= done;
								mp->pesOffset += done;
								ptr += done;
								num -= done;
								streamInputPos += done;
								if (mp->pesOffset < 128) return initial - num;
								}

							done = spuDecoder.SendData(mp->decBuffer + (mp->pesOffset - 128), mp->pesRest);
							}
						else
							{
							done = spuDecoder.SendData(ptr, mp->pesRest);
							}

						mp->pesRest -= done;
						mp->pesOffset += done;
						ptr += done;
						num -= done;
						streamInputPos += done;
						if (mp->pesRest) return initial - num;
						ptr += mp->pesSkip;
						num -= mp->pesSkip;
						streamInputPos += mp->pesSkip;
						mp->pesState = idle;
						break;
					}

				if (!hurryUp) YieldTimedFiber(-1);
				else YieldTimedFiber(0);
				}

			return initial;
			}
		case DVSST_PES:
		case DVSST_PROGRAM:
			{
			BYTE	b;
			DWORD size = num;
			DWORD rest = size, done;
			BYTE * p = ptr;

			while (rest)
				{
				switch ((MPEG2ProgramParse) (mp->mpparse.state))
					{
					case PROGSTRM_INITIAL:           //
						if (mp->streamType == DVSST_PROGRAM)
							{
							GOTO(PROGSTRM_PACKSTART0);
							}
						else
							{
							mp->mpparse.mpeg2 = TRUE;
							GOTO(PESPACKET_STARTCODEPREFIX0);
							}
					case PROGSTRM_FINAL:					//
						if (rest) REJECT
						else
							{
							streamInputPos += size;
							return size;
							}

					case MP1PACKHDR_BYTE1:					// ######## SysClockRef [29..22]
						CONSUME(b);
						PROCEED;
					case MP1PACKHDR_BYTE2:					// #######  SysClockRef [21..15]
		                              					//        1
						CONSUMEIF(b, BXXXXXXX1);
						PROCEED;
					case MP1PACKHDR_BYTE3:					// ######## SysClockRef [14..7]
						CONSUME(b);
						PROCEED;
					case MP1PACKHDR_BYTE4:					// #######  SysClockRef [6..0]
																	//        1
						CONSUMEIF(b, BXXXXXXX1);
						PROCEED;
					case MP1PACKHDR_BYTE5:					// 1
																	//  ####### MuxRate[21..15]
						CONSUMEIF(b, B1XXXXXXX);
						PROCEED;
					case MP1PACKHDR_BYTE6:					// ######## MuxRate[14..7]
						CONSUME(b);
						PROCEED;
					case MP1PACKHDR_BYTE7:					// #######  MuxRate[6..0]
																	//        1
																	// goto PESPACKET_STARTCODEPREFIX0
						CONSUMEIF(b, BXXXXXXX1);
						GOTO(PESPACKET_STARTCODEPREFIX0);

					case PROGSTRM_PACKSTART0:        // 00000000
						ACCEPT(B00000000);
					case PROGSTRM_PACKSTART1:			// 00000000
						ACCEPT(B00000000);
					case PROGSTRM_PACKSTART2:			// 00000001
						ACCEPT(B00000001);
					case PROGSTRM_PACKSTART3:			// 10111010 goto PACKHEADER_BYTE0
																// 10111001 goto PROGSTRM_FINAL
						CONSUME(b);
						if      (CHECK(b, B10111010)) GOTO(PACKHEADER_BYTE0)
						else if (CHECK(b, B10111001)) GOTO(PROGSTRM_FINAL)
						else REJECT;

					case PACKHEADER_BYTE0:				// if (01XXXXXX)
																//	01
																//   ###    SysClockRef [32..30]
																//      1
																//       ## SysClockRef [29..28]
																// mpeg2 = TRUE;
																// goto PACKHEADER_BYTE1
																// else if (0010XXXX)
																// 0010
																//     ###  SysClockRef [32..30]
																//        1
																// goto MP1PACKHDR_BYE1
						CONSUME(b);
						//if (CHECK(b, B01XXX1XX)) // Workaround for Optibase clips
						if (CHECK(b, B01XXXXXX))
							{
							mp->mpparse.mpeg2 = TRUE;
							}
						else if (CHECK(b, B0010XXX1))
							{
							mp->mpparse.mpeg2 = FALSE;
							GOTO(MP1PACKHDR_BYTE1);
							}
						else
							REJECT;

						PROCEED;
					case PACKHEADER_BYTE1:				// ######## SysClockRef [27..20]
						CONSUME(b);
						PROCEED;
					case PACKHEADER_BYTE2:				// #####    SysClockRef [19..15]
			                              			//      1
			                              			//       ## SysClockRef [14..13]
						CONSUMEIF(b, BXXXXX1XX);
						PROCEED;
					case PACKHEADER_BYTE3:				// ######## SysClockRef [12..5]
						CONSUME(b);
						PROCEED;
					case PACKHEADER_BYTE4:				// #####    SysClockRef [4..0]
																//      1
																//       ## SysClockRefExt [8..7]
						CONSUMEIF(b, BXXXXX1XX);
						PROCEED;
					case PACKHEADER_BYTE5:				// #######  SysClockRefExt [6..0]
																//        1
						CONSUMEIF(b, BXXXXXXX1);
						PROCEED;
					case PACKHEADER_BYTE6:				// ######## MuxRate[21..14]
						ACCEPT(BXXXXXXXX);
					case PACKHEADER_BYTE7:				// ######## MuxRate[13..6]
						ACCEPT(BXXXXXXXX);
					case PACKHEADER_BYTE8:				// ######   MuxRate[5..0]
																//       11
						ACCEPT(BXXXXXX11);
					case PACKHEADER_BYTE9:				// XXXXX
																//      ### PackStuffingLength
						CONSUME(b);
						mp->mpparse.cnt = XTBFB(0, 3, b);
						PROCEED;
					case PACKHEADER_STUFFING:			// XXXXXXXX times PackStuffingLength
																// goto PESPACKET_STARTCODEPREFIX0
						SKIP(mp->mpparse.cnt);
						GOTO(PESPACKET_STARTCODEPREFIX0);

		   		case SYSTEMHEADER_LENGTH0:			// ########
		   			CONSUME(mp->mpparse.high);
		   			PROCEED;
		   		case SYSTEMHEADER_LENGTH1:			// ########
		   			CONSUME(mp->mpparse.low);
		   			mp->mpparse.length = MAKEWORD(mp->mpparse.low, mp->mpparse.high);
		   			PROCEED;
		   		case SYSTEMHEADER_BYTE0:			// 1
		   													//  ####### RateBound[21..15]
		   			ACCEPT(B1XXXXXXX);
		   		case SYSTEMHEADER_BYTE1:			// ######## RateBound[14..7]
		   			ACCEPT(BXXXXXXXX);
		   		case SYSTEMHEADER_BYTE2:			// #######  RateBound[6..0]
		   													//        1
		   			ACCEPT(BXXXXXXX1);
		   		case SYSTEMHEADER_BYTE3:			// ######   AudioBound
		   													//       #  FixedFlag
																//        # CSPSFlag
						ACCEPT(BXXXXXXXX);
					case SYSTEMHEADER_BYTE4:			// #        SystemAudioLockFlag
																//  #       SystemVideoLockFlag
																//   1      MarkerBit
																//    ##### VideoBound
						ACCEPT(BXX1XXXXX);
					case SYSTEMHEADER_BYTE5:			// #        PacketRateRestrictionBit
																//  XXXXXXX
						ACCEPT(BXXXXXXXX);
					case SYSTEMHEADER_BYTE6:			// 00000000 goto PESPACKET_STARTCODEPREFIX1
																// 1####### StreamID
						CONSUME(b);
						if (!b) GOTO(PESPACKET_STARTCODEPREFIX1);
						mp->mpparse.streamID = b;
						PROCEED;
					case SYSTEMHEADER_STREAM0:			// 11
																//   #      PSTDBufferBoundScale
																//    ##### PSTDBufferSizeBound [12..8]
						ACCEPT(B11XXXXXX);
					case SYSTEMHEADER_STREAM1:			// ######## PSTDBufferSizeBound [7..0]
																// goto SYSTEMHEADER_BYTE6
						ACCEPTNGO(BXXXXXXXX, SYSTEMHEADER_BYTE6);

					case PESPACKET_STARTCODEPREFIX0:	// 00000000
						ACCEPT(B00000000);
					case PESPACKET_STARTCODEPREFIX1:	// 00000000
						ACCEPT(B00000000);
					case PESPACKET_STARTCODEPREFIX2:	// 00000001
						ACCEPT(B00000001);
					case PESPACKET_STREAMID:			// 10111011 goto SYSTEMHEADER_LENGTH0
																// 10111010 goto PACKHEADER_BYTE0
																// 10111001 goto PROGSTRM_FINAL
																// else
																// ######## streamID
						CONSUME(b);
						if      (CHECK(b, B10111011)) GOTO(SYSTEMHEADER_LENGTH0)
						else if (CHECK(b, B10111010)) GOTO(PACKHEADER_BYTE0)
						else if (CHECK(b, B10111001)) GOTO(PROGSTRM_FINAL)
						mp->mpparse.streamID = b;
						mp->mpparse.ptsValid = FALSE;
						mp->mpparse.dtsValid = FALSE;
						PROCEED;
					case PESPACKET_LENGTH0:				// ########
						CONSUME(mp->mpparse.high);
						PROCEED;
					case PESPACKET_LENGTH1:				// ########
																// if streamID != 10111101 &&
																//    streamID != videoStreamID && streamID != audioStreamID goto PESPACKET_SKIP
																// mpeg2 goto PESPACKET_HEADER0
						CONSUME(mp->mpparse.low);
						mp->mpparse.length = MAKEWORD(mp->mpparse.low, mp->mpparse.high);

						if (mp->mpparse.streamID == videoStreamID ||
							 mp->mpparse.streamID == audioStreamID ||
							 mp->mpparse.streamID == 0xBD)
							{
							if (mp->mpparse.mpeg2)
								{
								GOTO(PESPACKET_HEADER0);
								}
							else
								PROCEED;
							}
						else
							GOTO(PESPACKET_SKIP);

					case MP1PACKET_HEADER0:				// if (11111111) goto MP1PACKET_HEADER0
																// if (01XXXXXX) goto MP1PACKET_HEADER1
																// if (0010XXXX)
																// 0010
																//     ###  PTS[32..30]
																//        1 goto MP1PACKET_HEADER2
																// if (0011XXXX)
																// 0011
																//		 ###  PTS[32..30]
																//        1 goto MP1PACKET_HEADER6
																//	else
																// 00001111
																// goto MP1PACKET_DATA
						CONSUME(b);
						mp->mpparse.length--;
						if (CHECK(b, B11111111))
							{
							GOTO(MP1PACKET_HEADER0);
							}
						else if (CHECK(b, B01XXXXXX))
							{
							GOTO(MP1PACKET_HEADER1);
							}
						else if (CHECK(b, B0010XXX1))
							{
							mp->mpparse.ptss[0] = b;
							GOTO(MP1PACKET_HEADER2);
							}
						else if (CHECK(b, B0011XXX1))
							{
							mp->mpparse.ptss[0] = b;
							GOTO(MP1PACKET_HEADER6);
							}
						else if (CHECK(b, B00001111))
							{
							GOTO(MP1PACKET_DATA);
							}
						else
							REJECT;

					case MP1PACKET_HEADER1:				// XXXXXXXX	goto MP1PACKET_HEADER0
						CONSUME(b);
						mp->mpparse.length--;
						GOTO(MP1PACKET_HEADER0);

					case MP1PACKET_HEADER2:				// ######## PTS[29..22]
						CONSUME(mp->mpparse.ptss[1]);
						mp->mpparse.length--;
						PROCEED;
					case MP1PACKET_HEADER3:				// #######  PTS[21..15]
																//        1
						CONSUMEIF(mp->mpparse.ptss[2], BXXXXXXX1);
						mp->mpparse.length--;
						PROCEED;
					case MP1PACKET_HEADER4:				// ######## PTS[14..7]
						CONSUME(mp->mpparse.ptss[3]);
						mp->mpparse.length--;
						PROCEED;
					case MP1PACKET_HEADER5:				// #######  PTS[6..0]
																//        1
																// goto MP1PACKET_DATA
						CONSUMEIF(mp->mpparse.ptss[4], BXXXXXXX1);
						mp->mpparse.length--;
						mp->mpparse.pts = MKBF(29, 3, XTBFB(1, 3, mp->mpparse.ptss[0])) |
										  MKBF(21, 8,             mp->mpparse.ptss[1]) |
										  MKBF(14, 7, XTBFB(1, 7, mp->mpparse.ptss[2])) |
										  MKBF(6, 8,              mp->mpparse.ptss[3]) |
										  MKBF(0, 6,  XTBFB(2, 6, mp->mpparse.ptss[4]));
						mp->mpparse.ptsValid = TRUE;
						GOTO(MP1PACKET_DATA);

					case MP1PACKET_HEADER6:				// ######## PTS[29..22]
						CONSUME(mp->mpparse.ptss[1]);
						mp->mpparse.length--;
						PROCEED;
					case MP1PACKET_HEADER7:				// #######  PTS[21..15]
																//        1
						CONSUMEIF(mp->mpparse.ptss[2], BXXXXXXX1);
						mp->mpparse.length--;
						PROCEED;
					case MP1PACKET_HEADER8:				// ######## PTS[14..7]
						CONSUME(mp->mpparse.ptss[3]);
						mp->mpparse.length--;
						PROCEED;
					case MP1PACKET_HEADER9:				// #######  PTS[6..0]
																//        1
						CONSUMEIF(mp->mpparse.ptss[4], BXXXXXXX1);
						mp->mpparse.length--;
						mp->mpparse.pts = MKBF(29, 3, XTBFB(1, 3, mp->mpparse.ptss[0])) |
										  MKBF(21, 8,             mp->mpparse.ptss[1]) |
										  MKBF(14, 7, XTBFB(1, 7, mp->mpparse.ptss[2])) |
										  MKBF(6, 8,              mp->mpparse.ptss[3]) |
										  MKBF(0, 6,  XTBFB(2, 6, mp->mpparse.ptss[4]));
						mp->mpparse.ptsValid = TRUE;

						PROCEED;
					case MP1PACKET_HEADER10:			// 0001
																//     ###  DTS[32..30]
																//        1
						CONSUMEIF(mp->mpparse.ptss[0], B0001XXX1);
						mp->mpparse.length--;
						PROCEED;
					case MP1PACKET_HEADER11:			// ######## DTS[29..22]
						CONSUME(mp->mpparse.ptss[1]);
						mp->mpparse.length--;
						PROCEED;
					case MP1PACKET_HEADER12:			// #######  DTS[21..15]
																//        1
						CONSUMEIF(mp->mpparse.ptss[2], BXXXXXXX1);
						mp->mpparse.length--;
						PROCEED;
					case MP1PACKET_HEADER13:         // ######## DTS[14..7]
						CONSUME(mp->mpparse.ptss[3]);
						mp->mpparse.length--;
						PROCEED;
					case MP1PACKET_HEADER14:         // #######  DTS[6..0]
		                              				//        1
		                              				// goto MP1PACKET_DATA
						CONSUMEIF(mp->mpparse.ptss[4], BXXXXXXX1);
						mp->mpparse.length--;
						mp->mpparse.dts = MKBF(29, 3, XTBFB(1, 3, mp->mpparse.ptss[0])) |
										  MKBF(21, 8,             mp->mpparse.ptss[1]) |
										  MKBF(14, 7, XTBFB(1, 7, mp->mpparse.ptss[2])) |
										  MKBF(6, 8,              mp->mpparse.ptss[3]) |
										  MKBF(0, 6,  XTBFB(2, 6, mp->mpparse.ptss[4]));

						mp->mpparse.dtsValid = TRUE;
						GOTO(MP1PACKET_DATA);

					case PESPACKET_HEADER0:				// 10
																//   ##     Scrambling control
																//     #    Priority
																//      #   Data alignment
																//       #  Copyright
																//        # OriginalOrCopy
						ACCEPT(B10XXXXXX);
					case PESPACKET_HEADER1:				// ##       PTSDTSFlags
																//   #      ESCRFlag
																//    #     ESRateFlag
																//     #    DSMTrickModeFlag
																//      #   AdditionalCopyInfoFlag
																//       #  PESCRCFlag
																//        # PESExtensionFlag
						CONSUME(mp->mpparse.flags);
						PROCEED;
					case PESPACKET_HEADER2:				// ######## PESHeaderDataLength
						CONSUME(mp->mpparse.headerLength);
						mp->mpparse.headerRest = mp->mpparse.headerLength;
						PROCEED;

			// if PTSDTSFlags == 10

					case PESPACKET_HEADER3:				// 0010
																//     ###  PTS[32..30]
																//        1
						if (!CHECK(mp->mpparse.flags, B10XXXXXX)) GOTO(PESPACKET_HEADER8);
						CONSUMEIF(mp->mpparse.ptss[0], B0010XXX1);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER4:				// ######## PTS[22..29]
						CONSUME(mp->mpparse.ptss[1]);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER5:				// #######  PTS[15..21]
																//        1
						CONSUMEIF(mp->mpparse.ptss[2], BXXXXXXX1);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER6:				// ######## PTS[14..7]
						CONSUME(mp->mpparse.ptss[3]);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER7:				// #######  PTS[0..6]
																//        1
						CONSUMEIF(mp->mpparse.ptss[4], BXXXXXXX1);
						mp->mpparse.headerRest--;

						mp->mpparse.pts = MKBF(29, 3, XTBFB(1, 3, mp->mpparse.ptss[0])) |
										  MKBF(21, 8,             mp->mpparse.ptss[1]) |
										  MKBF(14, 7, XTBFB(1, 7, mp->mpparse.ptss[2])) |
										  MKBF(6, 8,              mp->mpparse.ptss[3]) |
										  MKBF(0, 6,  XTBFB(2, 6, mp->mpparse.ptss[4]));
						mp->mpparse.ptsValid = TRUE;

						GOTO(PESPACKET_HEADER18);

			// if PTSDTSFlags == 11

					case PESPACKET_HEADER8:				// 0011
																//     ###	PTS[32..30]
																//        1
						if (!CHECK(mp->mpparse.flags, B11XXXXXX)) GOTO(PESPACKET_HEADER18);

						CONSUMEIF(mp->mpparse.ptss[0], B0011XXX1);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER9: 			// ######## PTS[22..29]
						CONSUME(mp->mpparse.ptss[1]);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER10:			// #######  PTS[15..21]
																//        1
						CONSUMEIF(mp->mpparse.ptss[2], BXXXXXXX1);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER11:			// ######## PTS[14..7]
						CONSUME(mp->mpparse.ptss[3]);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER12:			// #######  PTS[0..6]
																//        1
						CONSUMEIF(mp->mpparse.ptss[4], BXXXXXXX1);
						mp->mpparse.headerRest--;

						mp->mpparse.pts = MKBF(29, 3, XTBFB(1, 3, mp->mpparse.ptss[0])) |
										  MKBF(21, 8, mp->mpparse.ptss[1]) |
										  MKBF(14, 7, XTBFB(1, 7, mp->mpparse.ptss[2])) |
										  MKBF(6, 8,  mp->mpparse.ptss[3]) |
										  MKBF(0, 6,  XTBFB(2, 6, mp->mpparse.ptss[4]));
						mp->mpparse.ptsValid = TRUE;

						PROCEED;
					case PESPACKET_HEADER13:			// 0001
																//     ###  DTS[32..30]
																//        1
						CONSUMEIF(mp->mpparse.ptss[0], B0001XXX1);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER14:			// ######## DTS[22..29]
						CONSUME(mp->mpparse.ptss[1]);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER15:			// #######  DTS[15..21]
																//        1
						CONSUMEIF(mp->mpparse.ptss[2], BXXXXXXX1);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER16:			// ######## DTS[14..7]
						CONSUME(mp->mpparse.ptss[3]);
						mp->mpparse.headerRest--;
						PROCEED;
					case PESPACKET_HEADER17:			// #######  DTS[0..6]
																//        1
						CONSUMEIF(mp->mpparse.ptss[4], BXXXXXXX1);
						mp->mpparse.headerRest--;

						mp->mpparse.dts = MKBF(29, 3, XTBFB(1, 3, mp->mpparse.ptss[0])) |
										  MKBF(21, 8, mp->mpparse.ptss[1]) |
										  MKBF(14, 7, XTBFB(1, 7, mp->mpparse.ptss[2])) |
										  MKBF(6, 8,  mp->mpparse.ptss[3]) |
										  MKBF(0, 6,  XTBFB(2, 6, mp->mpparse.ptss[4]));
						mp->mpparse.dtsValid = TRUE;

						PROCEED;

			// if ESCRFlag

					case PESPACKET_HEADER18:			// XX
																//   ###    ESCR[32..30]
																//      1
																//       ## ESCR[29..28]
						if (!CHECK(mp->mpparse.flags, BXX1XXXXX)) GOTO(PESPACKET_HEADER25);

						ACCEPT(BXXXXX1XX);
						mp->mpparse.headerRest--;

					case PESPACKET_HEADER19:			// ######## ESCR[27..20]
						ACCEPT(BXXXXXXXX);
						mp->mpparse.headerRest--;

					case PESPACKET_HEADER20:			// #####    ESCR[19..15]
																//      1
																//       ## ESCR[14..13]
						ACCEPT(BXXXXX1XX);
						mp->mpparse.headerRest--;

					case PESPACKET_HEADER22:			// ######## ESCR[12..5]
						ACCEPT(BXXXXXXXX);
						mp->mpparse.headerRest--;

					case PESPACKET_HEADER23:			// #####    ESCR[4..0]
																//      1
																//       ## ESCRX[8..7]
						ACCEPT(BXXXXX1XX);
						mp->mpparse.headerRest--;

					case PESPACKET_HEADER24:			// #######  ESCRX[6..0]
																//        1
						ACCEPT(BXXXXXXX1);
						mp->mpparse.headerRest--;


			// if ESRateFlag

					case PESPACKET_HEADER25:			// 1
																//  ####### ESRATE[14..21]
						if (!CHECK(mp->mpparse.flags, BXXX1XXXX)) GOTO(PESPACKET_HEADER28);
						ACCEPT(B1XXXXXXX);
						mp->mpparse.headerRest--;

					case PESPACKET_HEADER26:			// ######## ESRATE[14..7]
						ACCEPT(BXXXXXXXX);
						mp->mpparse.headerRest--;

					case PESPACKET_HEADER27:			// #######  ESRATE[6..0]
																//        1
						ACCEPT(BXXXXXXX1);
						mp->mpparse.headerRest--;


			// if DSMTrickModeFlag

					case PESPACKET_HEADER28:			// ###      TrickModeControl
																//    ##    FieldID
																//      #   IntraSlizeRefresh
																//       ## FrequencyTruncation
																//    ##### RepCntrl
																//    ##    FieldID
																//      ### Reserved
						if (!CHECK(mp->mpparse.flags, BXXXX1XXX)) GOTO(PESPACKET_HEADER29)
						ACCEPT(BXXXXXXXX);
						mp->mpparse.headerRest--;

			// if AdditionalCopyInfoFlag

					case PESPACKET_HEADER29:			// 1
																//  ####### AdditionalCopyInfo

						if (!CHECK(mp->mpparse.flags, BXXXXX1XX)) GOTO(PESPACKET_HEADER30)
						ACCEPT(B1XXXXXXX);
						mp->mpparse.headerRest--;

			// if PESCRCFlag

					case PESPACKET_HEADER30:			// ######## CRC[15..8]
						if (!CHECK(mp->mpparse.flags, BXXXXXX1X)) GOTO(PESPACKET_HEADER32);
						ACCEPT(BXXXXXXXX);
						mp->mpparse.headerRest--;

					case PESPACKET_HEADER31:			// ######## CRC[7..0]
						ACCEPT(BXXXXXXXX);
						mp->mpparse.headerRest--;

			// if PESExtensionFlag

					case PESPACKET_HEADER32:			// #        PESPrivateDataFlag
																//  #       PackHeaderFieldFlag
																//   #      ProgramPacketSequenceCounterFlag
																//    #     PSTDBufferFlag
																//     XXX
																//        # PESExtensionFlag2
						if (!CHECK(mp->mpparse.flags, BXXXXXXX1)) GOTO(PESPACKET_STUFFING);
						CONSUME(mp->mpparse.xflags);
						mp->mpparse.headerRest--;
						PROCEED;

				// if PESPrivateDataFlag

					case PESPACKET_PRIVATEDATA:		// XXXXXXXX times 32
						if (!CHECK(mp->mpparse.xflags, B1XXXXXXX)) GOTO(PESPACKET_PACKFIELDLENGTH);
						mp->mpparse.low = 32;
						PROCEED;

					case PESPACKET_PRIVATESKIP:		// XXXXXXXX times 32
						SKIP(mp->mpparse.low);
						mp->mpparse.headerRest-=32;

						PROCEED;

				// if PackHeaderFieldFlag

					case PESPACKET_PACKFIELDLENGTH:	// XXXXXXXX PackFieldLength
						if (!CHECK(mp->mpparse.xflags, BX1XXXXXX)) GOTO(PESPACKET_HEADER33);

						CONSUME(mp->mpparse.low);
						mp->mpparse.headerRest--;

						PROCEED;
					case PESPACKET_PACKHEDADER:		// ######## times PackFieldLength
						SKIP(mp->mpparse.low);
						mp->mpparse.headerRest-=mp->mpparse.low;

						PROCEED;

				// if ProgramPacketSequenceCounterFlag

					case PESPACKET_HEADER33:			// 1
																//  ####### ProgramPacketSequenceCounter
						if (!CHECK(mp->mpparse.xflags, BXX1XXXXX)) GOTO(PESPACKET_HEADER35);

						ACCEPT(B1XXXXXXX);
						mp->mpparse.headerRest--;

						PROCEED;
					case PESPACKET_HEADER34:			// 1
																//  #       MPEG12Identifier
																//   ###### OriginalStuffLength
						ACCEPT(B1XXXXXXX);
						mp->mpparse.headerRest--;

				// if PSTDBufferFlag

			   		PROCEED;
					case PESPACKET_HEADER35:			// 01
																//   #      PSTDBufferScale
																//    ##### PSTDBufferSize[13..8]
						if (!CHECK(mp->mpparse.xflags, BXXX1XXXX)) GOTO(PESPACKET_HEADER37);
						ACCEPT(B01XXXXXX);
						mp->mpparse.headerRest--;
						PROCEED;

					case PESPACKET_HEADER36:			// ######## PSTDBufferSize[7..0]
						ACCEPT(BXXXXXXXX);
						mp->mpparse.headerRest--;

				// if PESExtensionFlag2

						PROCEED;

					case PESPACKET_HEADER37:			// 1
																//  ####### PESExtensionFieldLength
						if (!CHECK(mp->mpparse.xflags, BXXXXXXX1)) GOTO(PESPACKET_STUFFING);
						CONSUMEIF(b, B1XXXXXXX);
						mp->mpparse.headerRest--;
						mp->mpparse.low = XTBFB(0, 7, b);
						PROCEED;

					case PESPACKET_EXTENSIONFIELD:	// XXXXXXXX times PESExtensionFieldLength
						SKIP(mp->mpparse.low);
						mp->mpparse.headerRest-=mp->mpparse.low;
						PROCEED;

					case PESPACKET_STUFFING:			// XXXXXXXX times rest of PESHeaderDataLength
						SKIP(mp->mpparse.headerRest);
						PROCEED;
					case PESPACKET_DATA:					// XXXXXXXX times PESPacketLength - 3 - PESHeaderDataLength
																// goto PESPACKET_STARTCODEPREFIX0
						mp->mpparse.length-=mp->mpparse.headerLength+3;
						if      (mp->mpparse.streamID == videoStreamID)
							{
							videoTracker.AddTrack(streamInputPos + size - rest, videoInputPos);
							GOTO(PESPACKET_VIDEO);
							}
						else if (mp->mpparse.streamID == audioStreamID)
							{
							audioTracker.AddTrack(streamInputPos + size - rest, audioInputPos);
							GOTO(PESPACKET_AUDIO);
							}
						else if (mp->mpparse.streamID == 0xBD)
							{
							GOTO(PESPACKET_PRIVATE);
							}

						SKIP(mp->mpparse.length);
						GOTO(PESPACKET_STARTCODEPREFIX0);
					case MP1PACKET_DATA:
						if      (mp->mpparse.streamID == videoStreamID)
							{
							videoTracker.AddTrack(streamInputPos + size - rest, videoInputPos);
							GOTO(PESPACKET_VIDEO);
							}
						else if (mp->mpparse.streamID == audioStreamID)
							{
							audioTracker.AddTrack(streamInputPos + size - rest, audioInputPos);
							GOTO(PESPACKET_AUDIO);
							}

						SKIP(mp->mpparse.length);
						GOTO(PESPACKET_STARTCODEPREFIX0);
					case PESPACKET_VIDEO:				// ######## times PESPacketlength - 3 - PESHeaderDataLength
																// goto PESPACKET_STARTCODEPREFIX0
						if (mp->mpparse.ptsValid)
							{
							timeStamp = mp->mpparse.pts / 45;

							if (initialVideoTime == -1) initialVideoTime = timeStamp;

							videoDecoder.AddTimeStamp(timeStamp);
							mp->mpparse.ptsValid = FALSE;
							}

						if (mp->mpparse.length >= rest)
							{
							done = videoDecoder.SendData(p, rest);
							videoInputPos += done;
							mp->mpparse.length-=done;
							rest-=done;
							streamInputPos += size-rest;
							return size-rest;
							}
						else
							{
							done = videoDecoder.SendData(p, mp->mpparse.length);
							videoInputPos += done;
							mp->mpparse.length-=done;
							rest-=done;
							if (mp->mpparse.length)
								{
								streamInputPos += size-rest;
								return size-rest;
								}
							p+=done;
							}

						GOTO(PESPACKET_STARTCODEPREFIX0);
					case PESPACKET_AUDIO:				// ######## times PESPacketLength - 3 - PESHeaderDataLength
																// goto PESPACKET_STARTCODEPREFIX0
						// For MPEG audio in non-DVD sources.
						initialAudioPacket = FALSE;
						if (mp->mpparse.ptsValid)
							{
							audioDecoder->AddTimeStamp(mp->mpparse.pts / 45);
							mp->mpparse.ptsValid = FALSE;
							}

						if (mp->mpparse.length >= rest)
							{
							done = audioDecoder->SendData(p, rest);
							audioInputPos += done;
							mp->mpparse.length-=done;
							rest-=done;
							streamInputPos += size-rest;
							return size-rest;
							}
						else
							{
							done = audioDecoder->SendData(p, mp->mpparse.length);
							audioInputPos += done;
							mp->mpparse.length-=done;
							rest-=done;
							if (mp->mpparse.length)
								{
								streamInputPos += size-rest;
								return size-rest;
								}
							p+=done;
							}

						GOTO(PESPACKET_STARTCODEPREFIX0);
					case PESPACKET_SKIP:					// ######## times PESPacketLength
						SKIP(mp->mpparse.length);
						GOTO(PESPACKET_STARTCODEPREFIX0);

					case PESPACKET_PRIVATE:				// if ######## != AC3 GOTO PESPACKET_SKIP
						CONSUME(b);
						mp->mpparse.length--;

						if (b == spuStreamID)
							{
							GOTO(PESPACKET_SUBPICTURE);
							}

						if (b == lpcmStreamID && audioType == MP2AUDTYP_LPCM)
							{
							audioTracker.AddTrack(streamInputPos + size - rest, audioInputPos);

							GOTO(PESPACKET_LPCM_SKIP0);
							}

						if (b != ac3StreamID || audioType != MP2AUDTYP_AC3)
							{
							GOTO(PESPACKET_SKIP);
							}

						audioTracker.AddTrack(streamInputPos + size - rest, audioInputPos);
						PROCEED;
					case PESPACKET_AC3_SKIP0:			// 2*XXXXXXXX
						CONSUME(b);
						mp->mpparse.length--;
						PROCEED;
					case PESPACKET_AC3_SKIP1:			// 2*XXXXXXXX
						CONSUME(mp->mpparse.high);
						mp->mpparse.length--;
						PROCEED;
					case PESPACKET_AC3_SKIP2:			// 2*XXXXXXXX
						CONSUME(mp->mpparse.low);
						mp->mpparse.length--;
						if (!initialAudioPacket) GOTO(PESPACKET_AC3);

						if (mp->mpparse.low == 0 && mp->mpparse.high == 0)
							{
							GOTO(PESPACKET_STARTCODEPREFIX0);
							}
						else
							{
							mp->mpparse.headerRest = MAKEWORD(mp->mpparse.low, mp->mpparse.high)-1;
							mp->mpparse.length-=mp->mpparse.headerRest;
							initialAudioPacket = FALSE;
							PROCEED;
							}
					case PESPACKET_AC3_SKIP3:			// 2*XXXXXXXX
						SKIP(mp->mpparse.headerRest);
						PROCEED;
					case PESPACKET_AC3:					// ######## times PESPacketLength - 4 - PESHeaderDataLength
																// goto PESPACKET_STARTCODEPREFIX2
						if (mp->mpparse.ptsValid)
							{
							audioDecoder->AddTimeStamp(mp->mpparse.pts / 45);
							mp->mpparse.ptsValid = FALSE;
							}

						if (mp->mpparse.length >= rest)
							{
							done = audioDecoder->SendData(p, rest);
							audioInputPos += done;
							mp->mpparse.length-=done;
							rest-=done;
							streamInputPos += size-rest;
							return size-rest;
							}
						else
							{
							done = audioDecoder->SendData(p, mp->mpparse.length);
							audioInputPos += done;
							mp->mpparse.length-=done;
							rest-=done;
							if (mp->mpparse.length)
								{
								streamInputPos += size-rest;
								return size-rest;
								}
							p+=done;
							}

						GOTO(PESPACKET_STARTCODEPREFIX0);

					case PESPACKET_SUBPICTURE:			// ######## times PESPacketLength - 4 - PESHeaderDataLength
																// goto PESPACKET_STARTCODEPREFIX2
						if (mp->mpparse.ptsValid)
							{
							spuDecoder.AddTimeStamp(mp->mpparse.pts / 45);
							mp->mpparse.ptsValid = FALSE;
							}

						if (mp->mpparse.length >= rest)
							{
							done = spuDecoder.SendData(p, rest);
							mp->mpparse.length-=done;
							rest-=done;
							streamInputPos += size-rest;
							return size-rest;
							}
						else
							{
							done = spuDecoder.SendData(p, mp->mpparse.length);
							mp->mpparse.length-=done;
							rest-=done;
							if (mp->mpparse.length)
								{
								streamInputPos += size-rest;
								return size-rest;
								}
							p+=done;
							}

						GOTO(PESPACKET_STARTCODEPREFIX0);

					case PESPACKET_LPCM_SKIP0:			// 2*XXXXXXXX
						CONSUME(b);
						mp->mpparse.length--;
						PROCEED;
					case PESPACKET_LPCM_SKIP1:			// 2*XXXXXXXX
						CONSUME(mp->mpparse.high);
						mp->mpparse.length--;
						PROCEED;
					case PESPACKET_LPCM_SKIP2:			// 2*XXXXXXXX
						CONSUME(mp->mpparse.low);
						mp->mpparse.length--;
						if (!initialAudioPacket)
							{
							mp->mpparse.headerRest = 3;
							mp->mpparse.length-=mp->mpparse.headerRest;
							PROCEED;
							}
						else
							{
							if (mp->mpparse.low == 0 && mp->mpparse.high == 0)
								{
								GOTO(PESPACKET_STARTCODEPREFIX0);
								}
							else
								{
								mp->mpparse.headerRest = MAKEWORD(mp->mpparse.low, mp->mpparse.high)-1;
								mp->mpparse.length-=mp->mpparse.headerRest;
								initialAudioPacket = FALSE;
								PROCEED;
								}
							}
					case PESPACKET_LPCM_SKIP3:			// 2*XXXXXXXX
						SKIP(mp->mpparse.headerRest);
						PROCEED;
					case PESPACKET_LPCM:					// ######## times PESPacketLength - 4 - PESHeaderDataLength
																// goto PESPACKET_STARTCODEPREFIX2
						if (mp->mpparse.ptsValid)
							{
							lpcmDecoder.AddTimeStamp(mp->mpparse.pts / 45);
							mp->mpparse.ptsValid = FALSE;
							}

						if (mp->mpparse.length >= rest)
							{
							done = audioDecoder->SendData(p, rest);
							audioInputPos += done;
							mp->mpparse.length-=done;
							rest-=done;
							streamInputPos += size-rest;
							return size-rest;
							}
						else
							{
							done = audioDecoder->SendData(p, mp->mpparse.length);
							audioInputPos += done;
							mp->mpparse.length-=done;
							rest-=done;
							if (mp->mpparse.length)
								{
								streamInputPos += size-rest;
								return size-rest;
								}
							p+=done;
							}

						GOTO(PESPACKET_STARTCODEPREFIX0);

					case PROGSTRM_RECOVER1:          // if !00000000 GOTO PROGSTRM_RECOVER1
						CONSUME(b);
						if (!CHECK(b, B00000000)) GOTO(PROGSTRM_RECOVER1);
						PROCEED;
					case PROGSTRM_RECOVER2:          // if !00000000 GOTO PROGSTRM_RECOVER1
						CONSUME(b);
						if (!CHECK(b, B00000000)) GOTO(PROGSTRM_RECOVER1);
						PROCEED;
					case PROGSTRM_RECOVER3:				// if !00000001 GOTO PROGSTRM_RECOVER1
						CONSUME(b);
						if (CHECK(b, B00000000)) GOTO(PROGSTRM_RECOVER3);
						if (!CHECK(b, B00000001)) GOTO(PROGSTRM_RECOVER1);
						PROCEED;
					case PROGSTRM_RECOVER4:				//	if !10111010 GOTO PROGSTRM_RECOVER1 else GOTO PACKHEADER_BYTE0
						if (mp->streamType == DVSST_PROGRAM)
							{
							GOTO(PROGSTRM_PACKSTART3);
							}
						else
							{
							GOTO(PESPACKET_STREAMID);
							}
					default:
						REJECT;
					}
				}

			streamInputPos += size;
			return size;
			}
		case DVSST_ELEMENTARY:
			{
			int done;

			switch (type)
				{
				case DVEST_VIDEO:
					if (initialVideoTime == -1) initialVideoTime = externalTimeStamp;
					if (externalTimeStamp >= 0) videoDecoder.AddTimeStamp(externalTimeStamp);
					videoTracker.AddTrack(streamInputPos, videoInputPos);
					done = videoDecoder.SendData(ptr, num);
#if VIDEO_DUMP
					DWORD dummy;
					::WriteFile(videoStream, ptr, done, &dummy, NULL);
#endif
					streamInputPos += done;
					videoInputPos += done;
					break;
				case DVEST_AUDIO:
					if (externalTimeStamp >= 0) audioDecoder->AddTimeStamp(externalTimeStamp);
					audioTracker.AddTrack(streamInputPos, videoInputPos);
					done =  audioDecoder->SendData(ptr, num);
					streamInputPos += done;
					audioInputPos += done;
					break;
				case DVEST_SUBPICTURE:
					if (externalTimeStamp >= 0) spuDecoder.AddTimeStamp(externalTimeStamp);
					done =  spuDecoder.SendData(ptr, num);
					streamInputPos += done;
					break;
				default:
					done = num;
				}

			return done;
			}
		}

	return 0;
	}

//
//  Complete Data
//

void DVDStreamDecoder::CompleteData(void)
	{
#if DUMP_STREAM_EVENTS>=1
	DTE("CompleteData");
#endif

	BYTE EOS[] = {0x00, 0x00, 0x01, 0xb7, 0x00, 0x00, 0x00, 0x00};

	dataCompleted = TRUE;
	splitCompletion = FALSE;

	switch (streamType)
		{
		case DVST_PROGRAM_STREAM:
			if (playbackMode == VDPM_SCANNING || playbackMode == VDPM_TRICKPLAY)
				{
				videoDecoder.SendData(EOS, 8);
				}
			videoDecoder.CompleteData();
			if (playbackMode == VDPM_PLAY_FORWARD)
				{
				if (initialVideoTime >= 0 && initialAudioPacket)
					{
					display->SetVideoStartTiming(initialVideoTime);
					initialVideoTime = -1;
					}

				audioDecoder->CompleteData();
				spuDecoder.CompleteData();
				}
			break;
		case DVST_ELEMENTARY_AUDIO_STREAM:
			audioDecoder->CompleteData();
			break;
		case DVST_ELEMENTARY_VIDEO_STREAM:
			videoDecoder.CompleteData();
			break;
		}

	DeferedStartStreaming();
	}

//
//  Complete Data Split
//

void DVDStreamDecoder::CompleteDataSplit(DVDElementaryStreamType type)
	{
#if DUMP_STREAM_EVENTS>=1
		{
		char buffer[100];
		wsprintf(buffer, "Enter CompleteDataSplit %d", type);
		DTE(buffer);
		}
#endif

	mpeparse[type].completing = TRUE;
	mpeparse[type].completed = FALSE;

	switch (type)
		{
		case DVEST_VIDEO:
			splitCompletion = TRUE;
			videoDecoder.CompleteData();
			if (playbackMode == VDPM_PLAY_FORWARD)
				{
				if (initialVideoTime >= 0 && initialAudioPacket)
					{
					display->SetVideoStartTiming(initialVideoTime);
					initialVideoTime = -1;
					}
				}
			break;
		case DVEST_AUDIO:
			splitCompletion = TRUE;
			audioDecoder->CompleteData();
			break;
		case DVEST_SUBPICTURE:
			splitCompletion = TRUE;
			spuDecoder.CompleteData();
			break;
		case DVEST_MULTIPLEXED:
			CompleteData();
			break;
		}

	if (streamInputPos > 500000 || audioInputPos > 100000 || EnoughStreamsCompleteToStart()) DeferedStartStreaming();

#if DUMP_STREAM_EVENTS>=1
		{
		char buffer[100];
		wsprintf(buffer, "Leave CompleteDataSplit %d", type);
		DTE(buffer);
		}
#endif
	}

//
//  Restart Data Split
//

void DVDStreamDecoder::RestartDataSplit(DVDElementaryStreamType type)
	{
#if DUMP_STREAM_EVENTS>=1
		{
		char buffer[100];
		wsprintf(buffer, "Enter RestartDataSplit %d", type);
		DTE(buffer);
		}
#endif
	DeferedStartStreaming();

	switch (type)
		{
		case DVEST_VIDEO:
			DP("VD SV");
			videoDecoder.StopStreaming();
			DP("VD ES");
			videoDecoder.EndStreaming(FALSE);
			DP("VD DONE");
			break;
		case DVEST_AUDIO:
			audioDecoder->StopStreaming();
			audioDecoder->EndStreaming(FALSE);
			break;
		case DVEST_SUBPICTURE:
			spuDecoder.StopStreaming();
			spuDecoder.EndStreaming(FALSE);
			break;
		case DVEST_MULTIPLEXED:
			break;
		}

	mpeparse[type].completing = FALSE;
	mpeparse[type].completed = FALSE;

	mpeparse[type].pesState = idle;
	mpeparse[type].endPTM = 0;
	mpeparse[type].ptsOffset = 0;
	mpeparse[type].mpparse.state = PROGSTRM_INITIAL;
	mpeparse[type].mpparse.mpeg2 = FALSE;

	mpeparse[type].mpparse.ptsValid = FALSE;
	mpeparse[type].mpparse.dtsValid = FALSE;
	mpeparse[type].pendingData = NULL;

	switch (type)
		{
#if VIDEO_DUMP
		{
		if (videoStream) ::CloseHandle(videoStream);
		char buffer[100];
		videoStreamCount++;
		wsprintf(buffer, "c:\\videodump\\strm%03d.mpg", videoStreamCount);

		videoStream = ::CreateFile(buffer,
								  GENERIC_WRITE,
								  FILE_SHARE_WRITE,
								  NULL,
								  CREATE_ALWAYS,
								  FILE_ATTRIBUTE_NORMAL,
								  NULL);
		}
#endif
		case DVEST_VIDEO:
			videoDecoder.FlushBuffer();
			videoTracker.Flush();
			videoInputPos = 0;
			initialVideoTime = -1;
			videoDecoder.BeginStreaming(playbackMode, highQuality);
			videoDecoder.StartStreaming(playbackSpeed);

			// Hack for MS Navigator, in case video data discontinuity arrives
			// after spu data for this frame
			spuDecoder.VideoDisplayBeginStreaming();
			break;
		case DVEST_AUDIO:
			audioDecoder->FlushBuffer();
			audioTracker.Flush();
			audioInputPos = 0;
			initialAudioPacket = TRUE;
			audioDecoder->BeginStreaming();
			audioDecoder->StartStreaming(playbackSpeed);
			break;
		case DVEST_SUBPICTURE:
			spuDecoder.FlushBuffer();
			spuDecoder.BeginStreaming(highQuality);
			spuDecoder.StartStreaming(playbackSpeed);
			break;
		}
#if DUMP_STREAM_EVENTS>=1
		{
		char buffer[100];
		wsprintf(buffer, "Leave RestartDataSplit %d", type);
		DTE(buffer);
		}
#endif
	}

