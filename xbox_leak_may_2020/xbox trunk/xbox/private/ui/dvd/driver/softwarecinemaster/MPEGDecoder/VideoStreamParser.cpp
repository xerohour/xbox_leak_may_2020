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

#include "VideoStreamParser.h"
#include "Mp2HuffTables.h"
#include "..\common\PerformanceMonitor.h"
#include "library\common\vddebug.h"
#define VIONA_VERSION	1
#define ONLY_EXTERNAL_VISIBLE
#include "library\hardware\mpeg2dec\generic\mp2eldec.h"

static const QuantizationMatrix DefIntraQuantMatrix = {
	0x08, 0x10, 0x10, 0x13, 0x10, 0x13, 0x16, 0x16,
	0x16, 0x16, 0x16, 0x16, 0x1A, 0x18, 0x1A, 0x1B,
	0x1B, 0x1B, 0x1A, 0x1A, 0x1A, 0x1A, 0x1B, 0x1B,
	0x1B, 0x1D, 0x1D, 0x1D, 0x22, 0x22, 0x22, 0x1D,
	0x1D, 0x1D, 0x1B, 0x1B, 0x1D, 0x1D, 0x20, 0x20,
	0x22, 0x22, 0x25, 0x26, 0x25, 0x23, 0x23, 0x22,
	0x23, 0x26, 0x26, 0x28, 0x28, 0x28, 0x30, 0x30,
	0x2E, 0x2E, 0x38, 0x38, 0x3A, 0x45, 0x45, 0x53
	};

const static int PictureRate[]    = {     0, 23976, 24000, 25000,
												  29970, 30000, 50000, 59940,
												  60000, 30002,     0,     0,
												  30001,     0,     0,     0};

void VideoStreamParser::ParseSequenceHeader(void)
	{
	QuantizationMatrix	mat;
	int i;

	if (bitStream->GetBits(32) != SEQUENCE_HEADER_CODE)
		{
//		::OutputDebugString("No sequence header\n");
		throw VideoStreamSyntaxError(1);
		}

	sequenceWidth = (WORD)bitStream->GetBits(12);
	sequenceHeight = (WORD)bitStream->GetBits(12);
	sequenceAspectRatio = bitStream->GetBits(4); // aspect ratio

	if (sequenceWidth > 720 || sequenceHeight > 576) throw VideoStreamSyntaxError(2);

	pictureParser->SetImageSize((sequenceWidth + 15) & ~15, (sequenceHeight + 15) & ~15, sequenceAspectRatio);


	sequenceFPS = bitStream->GetBits(4); // frame rate code
	if (!PictureRate[sequenceFPS]) throw VideoStreamSyntaxError(3);

	pictureParser->SetFrameRate(PictureRate[sequenceFPS]);
	bitStream->GetBits(18); // bit rate value;

	if (!bitStream->GetBits(1)) throw VideoStreamSyntaxError(4);

	bitStream->GetBits(10); // vbv buffer size
	bitStream->GetBits(1); // constrained parameter flag

	if (bitStream->GetBits(1))
		{
		for (i=0; i<64; i++) mat[i] = bitStream->GetBits(8);
		pictureParser->SetQuantMatrix(LUMA_INTRA_QUANTMATRIX, mat);
		pictureParser->SetQuantMatrix(CHROMA_INTRA_QUANTMATRIX, mat);
		usingDefaultQuantizerTable[0] = FALSE;
		}
	else if (!usingDefaultQuantizerTable[0])
		{
		pictureParser->SetQuantMatrix(LUMA_INTRA_QUANTMATRIX, DefIntraQuantMatrix);
		pictureParser->SetQuantMatrix(CHROMA_INTRA_QUANTMATRIX, DefIntraQuantMatrix);
		usingDefaultQuantizerTable[0] = TRUE;
		}

	if (bitStream->GetBits(1))
		{
		for (i=0; i<64; i++) mat[i] = bitStream->GetBits(8);
		pictureParser->SetQuantMatrix(LUMA_NONINTRA_QUANTMATRIX, mat);
		pictureParser->SetQuantMatrix(CHROMA_NONINTRA_QUANTMATRIX, mat);
		usingDefaultQuantizerTable[1] = FALSE;
		}
	else if (!usingDefaultQuantizerTable[1])
		{
		for(i=0; i<64; i++) mat[i] = 16;
		pictureParser->SetQuantMatrix(LUMA_NONINTRA_QUANTMATRIX, mat);
		pictureParser->SetQuantMatrix(CHROMA_NONINTRA_QUANTMATRIX, mat);
		usingDefaultQuantizerTable[1] = TRUE;
		}


	hasValidSequenceHeader = TRUE;

	bitStream->NextStartCode();
	}

void VideoStreamParser::ParseExtensionAndUserData(void)
	{
	DWORD ext;

	for(;;)
		{
		if (bitStream->PeekBits(32) == EXTENSION_START_CODE)
			{
			bitStream->Advance(32);
			switch(ext = bitStream->PeekBits(4))
				{
				case SEQUENCE_DISPLAY_EXTENSION_ID:
					ParseSequenceDisplayExtension();
					break;
				case SEQUENCE_EXTENSION_ID:
					ParseSequenceExtension();
					break;
				default:
//					PMMSGX("Unknown sequence extension %02x\n", ext);
//					bitStream->NextStartCode();
					throw VideoStreamSyntaxError(5);
				}
			}
		else if (bitStream->PeekBits(32) == USER_DATA_START_CODE)
			{
			ParseUserData();
//			bitStream->Advance(32);
//			while (bitStream->PeekBits(24) != 0x000001) bitStream->Advance(8);
			}
		else
			break;
		}
	bitStream->NextStartCode();
	}

void VideoStreamParser::ParseSequenceExtension(void)
	{
	if (bitStream->GetBits(4) != SEQUENCE_EXTENSION_ID) throw VideoStreamSyntaxError(6);

	bitStream->GetBits(8); // profile and level
	progressiveSequence = bitStream->GetBits(1) !=0; // progressive sequence
	pictureParser->SetProgressiveSequence(progressiveSequence);
	bitStream->GetBits(2); // chroma format
	bitStream->GetBits(2); // horizontal size extension
	bitStream->GetBits(2); // vertical size extension
	bitStream->GetBits(12); // bit rate extension

	if (!bitStream->GetBits(1)) throw VideoStreamSyntaxError(7);

	bitStream->GetBits(8); // vbv buffer size extension
	bitStream->GetBits(1); // low delay
	bitStream->GetBits(2); // frame rate extension n
	bitStream->GetBits(5); // frame rate extension m

	bitStream->NextStartCode();
	}

void VideoStreamParser::ParseSequenceDisplayExtension(void)
	{
	if (bitStream->GetBits(4) != SEQUENCE_DISPLAY_EXTENSION_ID) throw VideoStreamSyntaxError(8);

	bitStream->GetBits(3); // video format
	if (bitStream->GetBits(1)) // color description
		{
		bitStream->GetBits(8); // color primaries
		bitStream->GetBits(8); // transfer characteristics
		bitStream->GetBits(8); // matrix coefficients
		}

	bitStream->GetBits(14); // display horizontal size
	if (!bitStream->GetBits(1)) throw VideoStreamSyntaxError(9);
	bitStream->GetBits(14); // display vertical size

	bitStream->NextStartCode();
	}

void VideoStreamParser::ParseGroupOfPicturesHeader(void)
	{
	if (bitStream->GetBits(32) != GROUP_START_CODE) throw VideoStreamSyntaxError(10);

	bitStream->GetBits(25); // time code
	if (bitStream->GetBits(1)) pictureParser->SetClosedGOP();
	if (bitStream->GetBits(1)) pictureParser->SetBrokenLink();

	bitStream->NextStartCode();
	}

void VideoStreamParser::PreParseVideoSequence(void)
	{
	DWORD header;
	int i, prev;
	int displayPosition;

	numGOPFrames = 0;

	if (bitStream->PeekBits(32) == SEQUENCE_HEADER_CODE)
		{
		do {
			ParseSequenceHeader();
			if (bitStream->PeekBits(32) == EXTENSION_START_CODE)
				{
				pictureParser->SetVideoCodingStandard(VCS_MPEG_2);
				bitStream->Advance(32);

				ParseSequenceExtension();
				ParseExtensionAndUserData();
				do {
					if (bitStream->PeekBits(32) == GROUP_START_CODE)
						{
						ParseGroupOfPicturesHeader();
						ParseExtensionAndUserData();
						}

					if (!bitStream->RefillBuffer()) return;
					header = bitStream->PeekBits(32);
					while (header != PICTURE_START_CODE &&
							 header != GROUP_START_CODE &&
							 header != SEQUENCE_HEADER_CODE &&
							 header != SEQUENCE_END_CODE)
						{
						bitStream->Advance(8);
						bitStream->NextStartCode();
						if (!bitStream->RefillBuffer()) return;
						header = bitStream->PeekBits(32);
						}

					while (header == PICTURE_START_CODE)
						{
						gopFrameData[gopFrameDataIndex][numGOPFrames].position = bitStream->CurrentLocation();
						pictureParser->PreParse(gopFrameData[gopFrameDataIndex][numGOPFrames].codingType);
						numGOPFrames++;

						if (!bitStream->RefillBuffer()) return;
						header = bitStream->PeekBits(32);
						while (header != PICTURE_START_CODE &&
								 header != GROUP_START_CODE &&
								 header != SEQUENCE_HEADER_CODE &&
								 header != SEQUENCE_END_CODE)
							{
							bitStream->Advance(8);
							bitStream->NextStartCode();
							if (!bitStream->RefillBuffer()) return;
							header = bitStream->PeekBits(32);
							}
						}

					} while (header == GROUP_START_CODE);
				}
			} while (header == SEQUENCE_HEADER_CODE);
		}
	else
		throw VideoStreamSyntaxError(11);

	if (header == SEQUENCE_END_CODE)
		{
		bitStream->Advance(32);
		}

	nextGOPPosition = bitStream->CurrentLocation();

	displayPosition = 0;
	prev = -1;

	firstGOPIPFrame = -1;
	for(i=0; i<numGOPFrames; i++)
		{
		if (gopFrameData[gopFrameDataIndex][i].codingType != BIDIRECTIONAL_PREDICTIVE_CODED)
			{
			if (firstGOPIPFrame == -1) firstGOPIPFrame = i;
			lastGOPIPFrame = i;

			if (prev >= 0)
				{
				gopFrameData[gopFrameDataIndex][prev].displayPosition = displayPosition;
				displayPosition++;
				}

			prev = i;
			}
		else
			{
			gopFrameData[gopFrameDataIndex][i].displayPosition = displayPosition;
			displayPosition++;
			}

		if (gopFrameData[gopFrameDataIndex][i].codingType == INTRA_CODED)
			{
			lastGOPIFrame = i;
			}

		}

	gopFrameData[gopFrameDataIndex][prev].displayPosition = displayPosition;

	for(i=0; i<numGOPFrames; i++)
		{
		gopFrameData[gopFrameDataIndex][gopFrameData[gopFrameDataIndex][i].displayPosition].displayFrame = i;
		}
	}

void VideoStreamParser::ParseVideoSequenceTrick(void)
	{
	bool backward, prevGOPValid, nextGOPPrefetched;
	int numGOPPrefetched;
	int targetFrameID;
	int frameID, frame, i;
	__int64 currentGOPPosition;
	int prevLastGOPIFrame, prevNumFrames;
	int lastIPFrameDecoded;

	gopFrameDataIndex = 0;
	backward = FALSE;
	prevGOPValid = FALSE;
	nextGOPPrefetched = FALSE;
	currentGOPPosition = 0;
	numGOPPrefetched = 0;

	bitStream->MarkResetPosition();

	PreParseVideoSequence();

	requestedFrameID = currentFrameID = gopFrameData[gopFrameDataIndex][firstGOPIPFrame].displayPosition;
	bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][firstGOPIPFrame].position);
	pictureParser->Parse(PPM_DECODE_AND_DISPLAY);
	lastIPFrameDecoded = 0;

	while (!terminate)
		{
		targetFrameID = requestedFrameID;

		while (!terminate && targetFrameID != currentFrameID)
			{
			if (requestedFrameID > currentFrameID)
				{
				if (backward)
					{
					if (prevGOPValid)
						{
						bitStream->SetCurrentLocation(currentGOPPosition);
						bitStream->MarkResetPosition();

						prevGOPValid = FALSE;
						}
					if (nextGOPPrefetched)
						{
						WaitForSegmentCompleted();
						currentSegmentIndex++;
						nextGOPPrefetched = FALSE;
						}
					bitStream->SetFillLocation(nextGOPPosition);
					currentSegmentIndex += numGOPPrefetched;
					numGOPPrefetched = 0;
					lastIPFrameDecoded = -1;
					backward = FALSE;
					}

				frameID = targetFrameID - firstFrameID;
				while (frameID >= numGOPFrames)
					{
					if (numGOPPrefetched || nextGOPPrefetched || currentSegmentIndex < lastSegmentIndex)
						{
						frameID -= numGOPFrames;
						firstFrameID += numGOPFrames;

						currentGOPPosition = nextGOPPosition;
						prevGOPValid = TRUE;
						prevLastGOPIFrame = lastGOPIFrame;
						prevNumFrames = numGOPFrames;
						gopFrameDataIndex = 1 - gopFrameDataIndex;
						bitStream->SetCurrentLocation(currentGOPPosition);

						if (numGOPPrefetched)
							{
							numGOPPrefetched--;
							}
						else if (nextGOPPrefetched)
							{
							nextGOPPrefetched = FALSE;
							}
						else
							{
							currentSegmentIndex++;
							TrickSegmentRequested();
							}

						bitStream->RefillBuffer();
						bitStream->NextStartCode();

						PreParseVideoSequence();
						}
					else
						{
						frameID = numGOPFrames - 1;
						firstFrameID = targetFrameID - frameID;

						if (errorMessenger) errorMessenger->PostErrorMessage(GNR_FRAME_ADVANCE_BOUNDARY_REACHED);
						}
					}

				if (frameID >= gopFrameData[gopFrameDataIndex][firstGOPIPFrame].displayPosition)
					{
					if (prevGOPValid)
						{
						bitStream->SetCurrentLocation(currentGOPPosition);
						bitStream->MarkResetPosition();

						prevGOPValid = FALSE;
						}
					if (!nextGOPPrefetched)
						{
						if (currentSegmentIndex < lastSegmentIndex && numGOPPrefetched < 2)
							{
							currentSegmentIndex++;
							TrickSegmentRequested();
							nextGOPPrefetched = TRUE;
							}
						}
					else if (IsSegmentCompleted())
						{
						numGOPPrefetched++;
						nextGOPPrefetched = FALSE;
						}
					}
				else if (prevGOPValid)
					{
					i = prevLastGOPIFrame;
					while (i < prevNumFrames)
						{
						if (gopFrameData[1 - gopFrameDataIndex][i].codingType != BIDIRECTIONAL_PREDICTIVE_CODED)
							{
							bitStream->SetCurrentLocation(gopFrameData[1 - gopFrameDataIndex][i].position);
							pictureParser->Parse(PPM_DECODE_AND_KEEP);
							}
						i++;
						}

					bitStream->SetCurrentLocation(currentGOPPosition);
					bitStream->MarkResetPosition();

					prevGOPValid = FALSE;
					}

				frame = gopFrameData[gopFrameDataIndex][frameID].displayFrame;

				i = frame;

				while (gopFrameData[gopFrameDataIndex][i].codingType != INTRA_CODED) i--;
				if (i <= lastIPFrameDecoded && frame > lastIPFrameDecoded) i = lastIPFrameDecoded + 1;

				while (i < frame)
					{
					if (gopFrameData[gopFrameDataIndex][i].codingType != BIDIRECTIONAL_PREDICTIVE_CODED)
						{
						bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][i].position);
						pictureParser->Parse(PPM_DECODE_AND_KEEP);
						lastIPFrameDecoded = i;
						}
					i++;
					}

				bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][i].position);
				pictureParser->Parse(PPM_DECODE_AND_DISPLAY);
				if (gopFrameData[gopFrameDataIndex][i].codingType != BIDIRECTIONAL_PREDICTIVE_CODED)
					lastIPFrameDecoded = i;

				currentFrameID = targetFrameID;
				}
			else
				{
				if (!backward)
					{
					if (prevGOPValid)
						{
						bitStream->SetCurrentLocation(currentGOPPosition);
						bitStream->MarkResetPosition();

						prevGOPValid = FALSE;
						}
					if (nextGOPPrefetched)
						{
						WaitForSegmentCompleted();
						currentSegmentIndex--;
						nextGOPPrefetched = FALSE;
						}
					bitStream->SetFillLocation(nextGOPPosition);
					currentSegmentIndex -= numGOPPrefetched;
					numGOPPrefetched = 0;

					// fill in here
					backward = TRUE;
					lastIPFrameDecoded = -1;
					}

				frameID = targetFrameID - firstFrameID;
				while (frameID < gopFrameData[gopFrameDataIndex][firstGOPIPFrame].displayPosition)
					{
					if (numGOPPrefetched || nextGOPPrefetched || currentSegmentIndex > firstSegmentIndex)
						{
						currentGOPPosition = nextGOPPosition;
						prevGOPValid = TRUE;
						prevLastGOPIFrame = lastGOPIFrame;
						prevNumFrames = numGOPFrames;
						lastIPFrameDecoded = -1;
						gopFrameDataIndex = 1 - gopFrameDataIndex;
						bitStream->SetCurrentLocation(currentGOPPosition);

						if (numGOPPrefetched)
							numGOPPrefetched--;
						else if (nextGOPPrefetched)
							nextGOPPrefetched = FALSE;
						else
							{
							currentSegmentIndex--;
							TrickSegmentRequested();
							}

						bitStream->RefillBuffer();
						bitStream->NextStartCode();

						PreParseVideoSequence();

						frameID += numGOPFrames;
						firstFrameID -= numGOPFrames;
						}
					else
						{
						frameID = gopFrameData[gopFrameDataIndex][firstGOPIPFrame].displayPosition;
						firstFrameID = targetFrameID - frameID;
						if (errorMessenger) errorMessenger->PostErrorMessage(GNR_FRAME_ADVANCE_BOUNDARY_REACHED);
						}
					}

				if (frameID < numGOPFrames)
					{
					if (prevGOPValid)
						{
						bitStream->SetCurrentLocation(currentGOPPosition);
						bitStream->MarkResetPosition();

						prevGOPValid = FALSE;
						}
					if (!nextGOPPrefetched)
						{
						if (currentSegmentIndex > firstSegmentIndex  && numGOPPrefetched < 2)
							{
							currentSegmentIndex--;
							TrickSegmentRequested();
							nextGOPPrefetched = TRUE;
							}
						}
					else if (IsSegmentCompleted())
						{
						numGOPPrefetched++;
						nextGOPPrefetched = FALSE;
						}

					frame = gopFrameData[gopFrameDataIndex][frameID].displayFrame;

					i = frame;

					while (gopFrameData[gopFrameDataIndex][i].codingType != INTRA_CODED) i--;
					if (i <= lastIPFrameDecoded && frame > lastIPFrameDecoded) i = lastIPFrameDecoded + 1;

					while (i < frame)
						{
						if (gopFrameData[gopFrameDataIndex][i].codingType != BIDIRECTIONAL_PREDICTIVE_CODED)
							{
							bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][i].position);
							pictureParser->Parse(PPM_DECODE_AND_KEEP);
							lastIPFrameDecoded = i;
							}
						i++;
						}

					bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][i].position);
					pictureParser->Parse(PPM_DECODE_AND_DISPLAY);
					if (gopFrameData[gopFrameDataIndex][i].codingType != BIDIRECTIONAL_PREDICTIVE_CODED)
						lastIPFrameDecoded = i;
					}
				else if (prevGOPValid)
					{
					i = lastGOPIFrame;
					while (i < numGOPFrames)
						{
						if (gopFrameData[gopFrameDataIndex][i].codingType != BIDIRECTIONAL_PREDICTIVE_CODED)
							{
							bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][i].position);
							pictureParser->Parse(PPM_DECODE_AND_KEEP);
							}
						i++;
						}

					frame = gopFrameData[1 - gopFrameDataIndex][frameID - numGOPFrames].displayFrame;

					i = 0;

					while (i < frame)
						{
						if (gopFrameData[1 - gopFrameDataIndex][i].codingType != BIDIRECTIONAL_PREDICTIVE_CODED)
							{
							bitStream->SetCurrentLocation(gopFrameData[1 - gopFrameDataIndex][i].position);
							pictureParser->Parse(PPM_DECODE_AND_KEEP);
							}
						i++;
						}

					bitStream->SetCurrentLocation(gopFrameData[1 - gopFrameDataIndex][i].position);
					pictureParser->Parse(PPM_DECODE_AND_DISPLAY);

					bitStream->SetCurrentLocation(currentGOPPosition);
					bitStream->MarkResetPosition();

					lastIPFrameDecoded = -1;
					prevGOPValid = FALSE;
					}
				else
					{
					frame = gopFrameData[1 - gopFrameDataIndex][frameID - numGOPFrames].displayFrame;
					bitStream->SetCurrentLocation(gopFrameData[1 - gopFrameDataIndex][frame].position);
					pictureParser->Parse(PPM_DECODE_AND_DISPLAY);
					}

				currentFrameID = targetFrameID;
				}

			targetFrameID = requestedFrameID;
			}
		event.WaitForever();
		}
	}

void VideoStreamParser::ParseVideoSequenceReverse(void)
	{
	int pastIFrame, futureIFrame;
	int pastIPFrame, futureIPFrame;
	int currentFrame;
	__int64 tempNextGOPPosition;
	int tempNumGOPFrames;

	gopFrameDataIndex = 0;

	bitStream->MarkResetPosition();

	PreParseVideoSequence();
	if (!bitStream->RefillBuffer()) return;

	bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][lastGOPIFrame].position);

	pictureParser->Parse(PPM_DECODE_AND_DISPLAY);
	futureIFrame = lastGOPIFrame;

	pastIFrame = futureIFrame;
	for(;;)
		{
		do {
			pastIFrame--;
			} while (pastIFrame >= 0 && gopFrameData[gopFrameDataIndex][pastIFrame].codingType != INTRA_CODED);

		if (pastIFrame < 0)
			{
			// no more I Frames before this GOP

			gopFrameDataIndex = 1 - gopFrameDataIndex;
			bitStream->SetCurrentLocation(nextGOPPosition);
			tempNextGOPPosition = nextGOPPosition;
			tempNumGOPFrames = numGOPFrames;

			PreParseVideoSequence();
			if (!bitStream->RefillBuffer()) return;

			pastIFrame = lastGOPIFrame;

			pastIPFrame = lastGOPIPFrame;
			futureIPFrame = futureIFrame;

			currentFrame = pastIFrame;
			while (currentFrame != pastIPFrame)
				{
				bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][currentFrame].position);
				pictureParser->Parse(PPM_DECODE_AND_DROP);

				do {
					currentFrame++;
					} while (gopFrameData[gopFrameDataIndex][currentFrame].codingType == BIDIRECTIONAL_PREDICTIVE_CODED);
				}

			bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][currentFrame].position);
			pictureParser->Parse(PPM_DECODE_AND_DISPLAY);

			currentFrame = futureIPFrame;
			do {
				currentFrame++;
				} while (currentFrame < tempNumGOPFrames && gopFrameData[1 - gopFrameDataIndex][currentFrame].codingType == BIDIRECTIONAL_PREDICTIVE_CODED);
			currentFrame--;

			while (currentFrame > futureIPFrame)
				{
				bitStream->SetCurrentLocation(gopFrameData[1 - gopFrameDataIndex][currentFrame].position);
				pictureParser->Parse(PPM_DECODE_AND_DISPLAY);
				currentFrame--;
				}

			futureIPFrame = pastIPFrame;

			bitStream->SetCurrentLocation(tempNextGOPPosition);
			bitStream->MarkResetPosition();
			}
		else
			{
			pastIPFrame = futureIFrame;
			futureIPFrame = futureIFrame;
			}

		while (pastIPFrame != pastIFrame)
			{
			do {
				pastIPFrame--;
				} while (gopFrameData[gopFrameDataIndex][pastIPFrame].codingType == BIDIRECTIONAL_PREDICTIVE_CODED);

			currentFrame = pastIFrame;
			while (currentFrame != pastIPFrame)
				{
				bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][currentFrame].position);
				pictureParser->Parse(PPM_DECODE_AND_DROP);

				do {
					currentFrame++;
					} while (gopFrameData[gopFrameDataIndex][currentFrame].codingType == BIDIRECTIONAL_PREDICTIVE_CODED);
				}

			bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][currentFrame].position);
			pictureParser->Parse(PPM_DECODE_AND_DISPLAY);

			currentFrame = futureIPFrame;
			do {
				currentFrame++;
				} while (currentFrame < numGOPFrames && gopFrameData[gopFrameDataIndex][currentFrame].codingType == BIDIRECTIONAL_PREDICTIVE_CODED);
			currentFrame--;

			while (currentFrame > futureIPFrame)
				{
				bitStream->SetCurrentLocation(gopFrameData[gopFrameDataIndex][currentFrame].position);
				pictureParser->Parse(PPM_DECODE_AND_DISPLAY);
				currentFrame--;
				}

			futureIPFrame = pastIPFrame;
			}

		futureIFrame = pastIFrame;
		}
	}

void VideoStreamParser::ParseMPEG1GroupOfPictures(DWORD & header)
	{
	do {
		ParseGroupOfPicturesHeader();
		ParseExtensionAndUserData();
		do {
			pictureParser->Parse(PPM_DECODE_AND_DISPLAY);
			if (!bitStream->RefillBuffer()) return;
			header = bitStream->PeekBits(32);
			while ((playbackMode == VDPM_SCANNING || header != PICTURE_START_CODE &&
								     header != GROUP_START_CODE &&
								     header != SEQUENCE_END_CODE) &&
					 header != SEQUENCE_HEADER_CODE)
				{

				bitStream->Advance(8);
				bitStream->NextStartCode();
				if (!bitStream->RefillBuffer()) return;
				header = bitStream->PeekBits(32);
				}
			} while (bitStream->PeekBits(32) == PICTURE_START_CODE);
		} while (bitStream->PeekBits(32) == GROUP_START_CODE);
	}

void VideoStreamParser::ParseVideoSequence(void)
	{
	DWORD header;
	__int64 videoBitBufferPosition;

	if (bitStream->PeekBits(32) == SEQUENCE_HEADER_CODE)
		{
		ParseSequenceHeader();
		if (bitStream->PeekBits(32) == EXTENSION_START_CODE)
			{
			pictureParser->SetVideoCodingStandard(VCS_MPEG_2);
			bitStream->Advance(32);

			ParseSequenceExtension();
			do {
				ParseExtensionAndUserData();
				do {
					if (bitStream->PeekBits(32) == GROUP_START_CODE)
						{
						ParseGroupOfPicturesHeader();
						ParseExtensionAndUserData();
						}

					if (!bitStream->RefillBuffer()) return;
					header = bitStream->PeekBits(32);
					while (header != PICTURE_START_CODE &&
							 header != GROUP_START_CODE &&
							 header != SEQUENCE_HEADER_CODE &&
							 header != SEQUENCE_END_CODE)
						{
						bitStream->Advance(8);
						bitStream->NextStartCode();
						if (!bitStream->RefillBuffer()) return;
						header = bitStream->PeekBits(32);
						}

					while (header == PICTURE_START_CODE)
						{
						pictureParser->Parse(PPM_DECODE_AND_DISPLAY);
						if (!bitStream->RefillBuffer()) return;
						header = bitStream->PeekBits(32);
						while (header != PICTURE_START_CODE &&
								 header != GROUP_START_CODE &&
								 header != SEQUENCE_HEADER_CODE &&
								 header != SEQUENCE_END_CODE)
							{
							bitStream->Advance(8);
							bitStream->NextStartCode();
							if (!bitStream->RefillBuffer()) return;
							header = bitStream->PeekBits(32);
							}

						videoBitBufferPosition = bitStream->CurrentLocation();
						perfmon.videoBitsProcessed += videoBitBufferPosition - prevVideoBitBufferPosition;
						prevVideoBitBufferPosition = videoBitBufferPosition;
						}

					} while (header == GROUP_START_CODE);

				if (header != SEQUENCE_END_CODE)
					{
					ParseSequenceHeader();
					if (bitStream->GetBits(32) == EXTENSION_START_CODE)
						ParseSequenceExtension();
					else
						throw VideoStreamSyntaxError(12);
					}
				} while (bitStream->PeekBits(32) != SEQUENCE_END_CODE);

			bitStream->Advance(32);
			}
		else
			{
			pictureParser->SetVideoCodingStandard(VCS_MPEG_1);

			do {
				ParseExtensionAndUserData();

				ParseMPEG1GroupOfPictures(header);

				if (header != SEQUENCE_END_CODE)
					{
					ParseSequenceHeader();
					}

				} while (bitStream->PeekBits(32) != SEQUENCE_END_CODE);
			}
		}
	else if (bitStream->PeekBits(32) == GROUP_START_CODE)
		{
		pictureParser->SetImageSize((sequenceWidth + 15) & ~15, (sequenceHeight + 15) & ~15, sequenceAspectRatio);
		pictureParser->SetFrameRate(PictureRate[sequenceFPS]);
		pictureParser->SetVideoCodingStandard(VCS_MPEG_1);

		do {
			ParseMPEG1GroupOfPictures(header);

			if (header != SEQUENCE_END_CODE)
				{
				ParseSequenceHeader();
				}

			} while (bitStream->PeekBits(32) != SEQUENCE_END_CODE);
		}
	else
		throw VideoStreamSyntaxError(13);
	}

VideoStreamParser::VideoStreamParser(GenericPictureParser * pictureParser, VideoBitStream * bitStream)
	: MPEGUserDataDecoder(bitStream), event(FALSE, TRUE)
	{
	this->pictureParser = pictureParser;
	this->bitStream = bitStream;
	prevVideoBitBufferPosition = 0;
	hasValidSequenceHeader = FALSE;
	errorMessenger = NULL;
	pictureParser->SetUserDataDecoder(this);
	}

void VideoStreamParser::Parse(void)
	{
	QuantizationMatrix mat;
	int i;
	DWORD bits;

	for(i=0; i<64; i++) mat[i] = 16;

	pictureParser->SetQuantMatrix(LUMA_INTRA_QUANTMATRIX, DefIntraQuantMatrix);
	pictureParser->SetQuantMatrix(CHROMA_INTRA_QUANTMATRIX, DefIntraQuantMatrix);
	pictureParser->SetQuantMatrix(LUMA_NONINTRA_QUANTMATRIX, mat);
	pictureParser->SetQuantMatrix(CHROMA_NONINTRA_QUANTMATRIX, mat);

	usingDefaultQuantizerTable[0] = TRUE;
	usingDefaultQuantizerTable[1] = TRUE;

	if (!bitStream->RefillBuffer(1400000)) return;

	bits = bitStream->PeekBits(32);
	while (!(bits == SEQUENCE_HEADER_CODE ||
		      hasValidSequenceHeader && bits == GROUP_START_CODE))
		{
		bitStream->Advance(8);

		bitStream->NextStartCode();

		if (!bitStream->RefillBuffer()) return;

		bits = bitStream->PeekBits(32);
		}

	try
		{
		switch (playbackMode)
			{
			case VDPM_PLAY_FORWARD:
				ParseVideoSequence();
				break;
			case VDPM_PLAY_BACKWARD:
				ParseVideoSequenceReverse();
				break;
			case VDPM_SCANNING:
				ParseVideoSequence();
				break;
			case VDPM_TRICKPLAY:
				ParseVideoSequenceTrick();
				break;
			}
		}
	catch(PictureHeaderSyntaxError)
		{
		PMMSG("Picture header syntax error\n");
		}
	catch (VideoStreamSyntaxError e)
		{
		PMMSGX("Video stream syntax error %d\n", e.subCode);
		}
	}


void VideoStreamParser::BeginStreaming(VideoPlaybackMode playbackMode)
	{
	this->playbackMode = playbackMode;
	switch (playbackMode)
		{
		case VDPM_PLAY_FORWARD:
		case VDPM_PLAY_BACKWARD:
		case VDPM_SCANNING:
			break;
		case VDPM_TRICKPLAY:
			terminate = FALSE;
			currentFrameID = 2;
			firstFrameID = 0;
			requestedFrameID = 2;
			currentSegmentIndex = 0x80000000;
			firstSegmentIndex = 0x00000000;
			lastSegmentIndex = 0xffffffff;
			break;
		}
	}

void VideoStreamParser::EndStreaming(bool fullReset)
	{
	terminate = TRUE;
	switch (playbackMode)
		{
		case VDPM_PLAY_FORWARD:
		case VDPM_PLAY_BACKWARD:
		case VDPM_SCANNING:
			break;
		case VDPM_TRICKPLAY:
			event.SetEvent();
			break;
		}
	playbackMode = VDPM_PLAY_FORWARD;
	prevVideoBitBufferPosition = 0;
	if (fullReset) hasValidSequenceHeader = FALSE;
	}

void VideoStreamParser::SetFirstVideoSegment(DWORD seg)
	{
	firstSegmentIndex = seg;
	}

void VideoStreamParser::SetLastVideoSegment(DWORD seg)
	{
	lastSegmentIndex = seg;
	}

DWORD VideoStreamParser::VideoSegmentRequested(void)
	{
	return currentSegmentIndex;
	}

void VideoStreamParser::AdvanceTrickFrame(int by)
	{
	requestedFrameID += by;
	event.SetEvent();
	}

