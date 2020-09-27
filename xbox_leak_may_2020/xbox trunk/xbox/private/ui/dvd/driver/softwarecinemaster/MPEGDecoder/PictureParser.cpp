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


#include "PictureParser.h"
#include "Mp2HuffTables.h"
#include "library\common\vddebug.h"
#include "..\common\TimedFibers.h"
#include "..\common\PerformanceMonitor.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Statistics
//
///////////////////////////////////////////////////////////////////////////////

#define FRAME_STATISTICS	0
#define FRAME_ANNOTATION	0
#define NO_DROPPED_FRAMES	0
#define ONLY_I_FRAMES		0
#define ONLY_IP_FRAMES		0

#if FRAME_ANNOTATION
static int decodeFrameCount;
static int displayFrameCount;
#endif

#if FRAME_STATISTICS
#include <stdio.h>

struct StatusRecord
	{
	int	displayTime, enterTime, postTime, getTime, offsetTime, decodeTime;
	char	frameType;
	bool	skipped;
	int	ptsTime;
	bool	repeatFirstField, progressiveFrame;
	int	counter;
	};

static int StatusFramesDecoded[3];
static int StatusFrameDuration[3];

static StatusRecord	sr[10244];
static int				nsr;
static int				ntsr;

inline void AddStatus(int displayTime, int enterTime, int postTime, int getTime, int offsetTime, char frameType, bool skipped, int ptsTime, bool repeatFirstField, bool progressiveFrame, int decodeTime, int counter)
	{
	if (nsr < 10240)
		{
		sr[nsr].displayTime = displayTime;
		sr[nsr].enterTime = enterTime;
		sr[nsr].getTime = getTime;
		sr[nsr].postTime = postTime;
		sr[nsr].offsetTime = offsetTime;
		sr[nsr].frameType = frameType;
		sr[nsr].skipped = skipped;
		sr[nsr].ptsTime = ptsTime;
		sr[nsr].repeatFirstField = repeatFirstField;
		sr[nsr].progressiveFrame = progressiveFrame;
		sr[nsr].decodeTime = decodeTime;
		sr[nsr].counter = counter;

		nsr++;
		}
	}

void DumpStatusList(FILE * f)
	{
	int i, total, skipped;

	total = 0;
	skipped = 0;

	for(i=0; i<nsr; i++)
		{
		fprintf(f, "%4d(%4d) : %c(%c%c)%c D: %6d (%6d)- T %3d E: %6d P: %6d S: %6d G: %6d (%d) %s\n",
			i, sr[i].counter, sr[i].frameType,
			sr[i].repeatFirstField ? 'R' : '-',
			sr[i].progressiveFrame ? 'P' : '-',
			sr[i].progressiveFrame != sr[i].frameType < 'a' ? '*' : ' ',
			sr[i].displayTime, sr[i+1].displayTime - sr[i].displayTime,
			sr[i].decodeTime,
			sr[i].enterTime,
			sr[i].postTime,
			sr[i].ptsTime,
			sr[i].getTime,
			sr[i].offsetTime,
			sr[i].skipped ? "skipped" : "done"
			);
		if (sr[i].skipped) skipped++;
		total++;
		}
	if (total) fprintf(f, "Total : %6d Skipped : %6d (%3d%%)\n", total, skipped, skipped * 100 / total);
	}

void Partition(int first, int last)
	{
	int pivot, i;
	StatusRecord	swap;

	if (last > first)
		{
		swap = sr[first];
		pivot = first;
		for(i=first+1; i<=last; i++)
			{
			if (sr[i].decodeTime > swap.decodeTime)
				{
				sr[pivot] = sr[i];
				sr[i] = sr[pivot+1];
				pivot++;
				}
			}
		sr[pivot] = swap;
		Partition(first, pivot-1);
		Partition(pivot+1, last);
		}
	}

void SortStatus(void)
	{
	Partition(0, nsr-1);
	}

void DumpStatus(void)
	{
	char fname[100];
	FILE * f;

	wsprintf(fname, "c:\\mpstat\\sdump.%d.txt", ntsr);

	if (nsr)
		{
		f = fopen(fname, "w");
		if (f)
			{
			DumpStatusList(f);
			printf("\n------------------------------------------------------------------------------------\n\n");
			SortStatus();
			DumpStatusList(f);
			fclose(f);
			ntsr++;
			nsr = 0;
			}
		}
	}

#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Picture Parser
//
///////////////////////////////////////////////////////////////////////////////

//
//  Parse Picture Header
//

void PictureParser::ParsePictureHeader(void)
	{
	bool fullPel;
	BYTE fcode;

	if (bitStream->GetBits(32) != PICTURE_START_CODE) throw PictureHeaderSyntaxError();

	frameContinuationCounter = bitStream->GetBits(10);

	codingType = (PictureCodingType)(bitStream->GetBits(3));

	sliceParser->SetCodingType(codingType);

	isIPFrame = codingType != BIDIRECTIONAL_PREDICTIVE_CODED;

	bitStream->GetBits(16); // vbv delay

	if (codingType == PREDICTIVE_CODED || codingType == BIDIRECTIONAL_PREDICTIVE_CODED)
		{
		fullPel = bitStream->GetBool();
		fcode = (BYTE)bitStream->GetBits(3);
		sliceParser->SetForwardVector(fullPel, fcode);
		}

	if (codingType == BIDIRECTIONAL_PREDICTIVE_CODED)
		{
		fullPel = bitStream->GetBool();
		fcode = (BYTE)bitStream->GetBits(3);
		sliceParser->SetBackwardVector(fullPel, fcode);
		}

	while (bitStream->GetBits(1))
		bitStream->GetBits(8);

	bitStream->NextStartCode();

	frameRepeat = 1;
	}

//
//  Parse Picture Coding Extension
//

void PictureParser::ParsePictureCodingExtension(void)
	{
	BYTE fcodes[2][2];
	BYTE intraDCPrecision;

	if (bitStream->GetBits(32) != EXTENSION_START_CODE) throw PictureHeaderSyntaxError();
	if (bitStream->GetBits(4) != PICTURE_CODING_EXTENSION_ID) throw PictureHeaderSyntaxError();

	fcodes[0][0] = (BYTE)bitStream->GetBits(4);
	fcodes[0][1] = (BYTE)bitStream->GetBits(4);
	fcodes[1][0] = (BYTE)bitStream->GetBits(4);
	fcodes[1][1] = (BYTE)bitStream->GetBits(4);

	intraDCPrecision = (BYTE)bitStream->GetBits(2);
	pictureStructure = (PictureStructure)(bitStream->GetBits(2));

	pictureFlags = (BYTE)bitStream->GetBits(6);

	sliceParser->SetFCodes(fcodes[0][0], fcodes[0][1], fcodes[1][0], fcodes[1][1]);
	sliceParser->SetPictureParameters(intraDCPrecision, pictureStructure, pictureFlags);

	repeatFirstField = bitStream->GetBool();

	bitStream->GetBits(1);

	progressiveFrame = bitStream->GetBool();

	bitStream->GetBits(1);

	if (progressiveSequenceStructure)
		{
		if (repeatFirstField)
			{
			if (PF_TOP_FIELD_FIRST(pictureFlags))
				frameRepeat = 3;
			else
				frameRepeat = 2;

			repeatFirstField = false;
			}
		}

	bitStream->NextStartCode();
	}

//
//  Parse Quantizer Matrix Extension
//

void PictureParser::ParseQuantMatrixExtension(void)
	{
	QuantizationMatrix	mat;
	int i;

	if (bitStream->GetBits(4) != QUANT_MATRIX_EXTENSION_ID) throw PictureHeaderSyntaxError();

	if (bitStream->GetBits(1))
		{
		for (i=0; i<64; i++) mat[i] = bitStream->GetBits(8);
		sliceParser->SetQuantMatrix(LUMA_INTRA_QUANTMATRIX, mat);
		}
	if (bitStream->GetBits(1))
		{
		for (i=0; i<64; i++) mat[i] = bitStream->GetBits(8);
		sliceParser->SetQuantMatrix(LUMA_NONINTRA_QUANTMATRIX, mat);
		}
	if (bitStream->GetBits(1))
		{
		for (i=0; i<64; i++) mat[i] = bitStream->GetBits(8);
		sliceParser->SetQuantMatrix(CHROMA_INTRA_QUANTMATRIX, mat);
		}
	if (bitStream->GetBits(1))
		{
		for (i=0; i<64; i++) mat[i] = bitStream->GetBits(8);
		sliceParser->SetQuantMatrix(CHROMA_NONINTRA_QUANTMATRIX, mat);
		}

	bitStream->NextStartCode();
	}

//
//  Parse Picture Display Extension
//

void PictureParser::ParsePictureDisplayExtension(void)
	{
	int offset;

	if (bitStream->GetBits(4) != PICTURE_DISPLAY_EXTENSION_ID) throw PictureHeaderSyntaxError();

	offset = bitStream->GetBits(16);
	panScanOffset = (width << 13) - ((offset << 16) >> 4);

	bitStream->NextStartCode();
	}

//
//  Parse Extension And User Data
//

void PictureParser::ParseExtensionAndUserData(void)
	{
	for(;;)
		{
		if (bitStream->PeekBits(32) == EXTENSION_START_CODE)
			{
			bitStream->Advance(32);
			switch(bitStream->PeekBits(4))
				{
				case QUANT_MATRIX_EXTENSION_ID:
					ParseQuantMatrixExtension();
					break;
				case PICTURE_DISPLAY_EXTENSION_ID:
					ParsePictureDisplayExtension();
					break;
				default:
					throw PictureHeaderSyntaxError();
				}
			}
		else if (bitStream->PeekBits(32) == USER_DATA_START_CODE)
			{
			if (userDataDecoder)
				userDataDecoder->ParseUserData();
			else
				{
				bitStream->Advance(32);
				while (bitStream->PeekBits(24) != 0x000001) bitStream->Advance(8);
				}
			}
		else
			break;
		}

	bitStream->NextStartCode();
	}

bool PictureParser::WaitForPictureDataComplete(void)
	{
	__int64	startPos;

	switch (playbackMode)
		{
		case VDPM_PLAY_FORWARD:
			if (bitStream->HasBufferEnoughData(400 * 1024 * 8))
				{
				return bitStream->RefillBuffer(400 * 1024 * 8);
				}
			else
				{
				bitStream->MarkResetPosition();
				startPos = bitStream->CurrentLocation();
				do {
					bitStream->Advance(32);
					bitStream->NextStartCode();
					if (!bitStream->RefillBuffer(32))
						{
						bitStream->SetCurrentLocation(startPos);
						bitStream->RemoveResetPosition();
						return false;
						}
					} while (IS_SLICE_HEADER(bitStream->PeekBits(32)));

				if (pictureStructure != PS_FRAME_PICTURE)
					{
					do {
						bitStream->Advance(32);
						bitStream->NextStartCode();
						if (!bitStream->RefillBuffer(32))
							{
							bitStream->SetCurrentLocation(startPos);
							bitStream->RemoveResetPosition();
							return false;
							}
						} while (IS_SLICE_HEADER(bitStream->PeekBits(32)));
					}

				bitStream->SetCurrentLocation(startPos);
				bitStream->RemoveResetPosition();

				return true;
				}
			break;
		case VDPM_PLAY_BACKWARD:
		case VDPM_TRICKPLAY:
			return bitStream->RefillBuffer(64 * 1024 * 8);
			break;
		case VDPM_SCANNING:
			return bitStream->RefillBuffer(400 * 1024 * 8);
			break;
		}

	return false;
	}

//
//  Parse Picture Body
//

static int pictureCnt;

bool PictureParser::ParsePictureBody(void)
	{
	int sliceLevelScheduling;

	sliceLevelScheduling = 4;
	do {
		if (IsHigherPriorityTimedFiberReady(bitStream->AvailBits() < 8 * 500000 ? -1 : 6))
			{
			if (sliceParser->SuspendFrameDecoding())
				{
				YieldTimedFiber(0);

				sliceParser->ResumeFrameDecoding();
				}
			sliceLevelScheduling = 6;
			}

		sliceParser->Parse();
		} while (IS_SLICE_HEADER(bitStream->PeekBits(32)));

	if (pictureStructure != PS_FRAME_PICTURE)
		{
		sliceParser->DoneFirstFieldDecoding();

		try
			{
			bool oldIsIPFrame = isIPFrame;
			ParsePictureHeader();
			if (oldIsIPFrame != isIPFrame) return FALSE;
			ParsePictureCodingExtension();
			ParseExtensionAndUserData();
			}
		catch (PictureHeaderSyntaxError)
			{
			return FALSE;
			}

		if (WaitForPictureDataComplete())
			{
			sliceParser->BeginSecondFieldDecoding();

			sliceLevelScheduling = 4;

			do {
				if (IsHigherPriorityTimedFiberReady())
					{
					if (sliceParser->SuspendFrameDecoding())
						{
						YieldTimedFiber(0);

						sliceParser->ResumeFrameDecoding();
						}
					sliceLevelScheduling = 6;
					}

				sliceParser->Parse();
				} while (IS_SLICE_HEADER(bitStream->PeekBits(32)));
			}
		else
			{
			do {
				if (!bitStream->RefillBuffer()) return FALSE;

				bitStream->Advance(32);
				bitStream->NextStartCode();
				} while (IS_SLICE_HEADER(bitStream->PeekBits(32)));
			}
		}

	return TRUE;
	}

//
//  Skip Picture Body
//

bool PictureParser::SkipPictureBody(void)
	{
	do {
		if (!bitStream->RefillBuffer()) return FALSE;

		bitStream->Advance(32);
		bitStream->NextStartCode();
		} while (IS_SLICE_HEADER(bitStream->PeekBits(32)));

	if (pictureStructure != PS_FRAME_PICTURE)
		{
		ParsePictureHeader();
		ParsePictureCodingExtension();
		ParseExtensionAndUserData();

		do {
			if (!bitStream->RefillBuffer(16 * 1024 * 8)) return FALSE;

			bitStream->Advance(32);
			bitStream->NextStartCode();
			} while (IS_SLICE_HEADER(bitStream->PeekBits(32)));
		}

	return TRUE;
	}

//
//  Release Frame Buffers
//

void PictureParser::ReleaseFrameBuffers(void)
	{
	sliceParser->InvalidateFrameReferences();

	if (ipFrameBuffers[0]) delete ipFrameBuffers[0];
	if (ipFrameBuffers[1]) delete ipFrameBuffers[1];
	if (ipFrameBuffers[2]) delete ipFrameBuffers[2];
	if (bFrameBuffers[0])  delete bFrameBuffers[0];
	if (bFrameBuffers[1])  delete bFrameBuffers[1];

	ipFrameBuffers[0] = NULL;
	ipFrameBuffers[1] = NULL;
	ipFrameBuffers[2] = NULL;
	bFrameBuffers[0] = NULL;
	bFrameBuffers[1] = NULL;

	previousFrame = NULL;
	previousBFrameID = 0;

	sliceParser->EndFrameSequence();
	}

//
//  Allocate Frame Buffers
//

void PictureParser::AllocateFrameBuffers(void)
	{
	if (!ipFrameBuffers[0])
		{
		sliceParser->BeginFrameSequence();

		ipFrameBuffers[0] = sliceParser->AllocFrameStore(PREDICTIVE_CODED, display->SupportsStripeAccess());
		ipFrameBuffers[1] = sliceParser->AllocFrameStore(PREDICTIVE_CODED, display->SupportsStripeAccess());
		if (playbackMode == VDPM_PLAY_BACKWARD || playbackMode == VDPM_TRICKPLAY)
			ipFrameBuffers[2] = sliceParser->AllocFrameStore(PREDICTIVE_CODED, display->SupportsStripeAccess());

		bFrameBuffers[0] = sliceParser->AllocFrameStore(BIDIRECTIONAL_PREDICTIVE_CODED, display->SupportsStripeAccess());
		if (display->NeedsPreviousFrame())
			bFrameBuffers[1] = sliceParser->AllocFrameStore(BIDIRECTIONAL_PREDICTIVE_CODED, display->SupportsStripeAccess());
		}
	}

//
//  Pre Parse Pictures
//

void PictureParser::PreParsePicture(PictureCodingType & codingType)
	{
	ParsePictureHeader();
	if (vcs == VCS_MPEG_2)
		{
		ParsePictureCodingExtension();
		ParseExtensionAndUserData();
		}
	else
		ParseExtensionAndUserData();

	codingType = this->codingType;

	SkipPictureBody();
	}

//
//  Parse Temp Pictures
//

void PictureParser::ParseTempPicture(void)
	{
	PictureStructure	fieldDisplayMode;
	int decodedIPFrame;
	int displayTime;
	bool didDisplay;

	AllocateFrameBuffers();

	displayTime = (int)((__int64)lowFieldCount * (500000 / 16) / framesPerSecond + timeOffset);

	ParsePictureHeader();
	if (vcs == VCS_MPEG_2)
		{
		ParsePictureCodingExtension();
		ParseExtensionAndUserData();
		}
	else
		{
		ParseExtensionAndUserData();
		pictureStructure = PS_FRAME_PICTURE;
		fieldDisplayMode = PS_FRAME_PICTURE;
		sliceParser->SetPictureParameters(0, PS_FRAME_PICTURE, 0x10); // FRAME_PRED_FRAME_DCT
		}

	if (isIPFrame)
		{
		if (ipDisplayBuffer)
			{
			PostIPFrameBuffer(displayTime, FALSE);
			initialBFrameSkip = FALSE;
			ipDisplayBuffer = NULL;
			didDisplay = TRUE;
			}
		else
			didDisplay = FALSE;
		if (!WaitForPictureDataComplete()) return;

		if (playbackMode == VDPM_PLAY_BACKWARD)
			decodedIPFrame = 3 - (pastIPFrame + futureIPFrame);
		else
			decodedIPFrame = pastIPFrame;

		if (sliceParser->BeginFrameDecoding(ipFrameBuffers[futureIPFrame], ipFrameBuffers[futureIPFrame], ipFrameBuffers[decodedIPFrame], NULL, FALSE))
			{
			if (ParsePictureBody())
				{
				sliceParser->DoneFrameDecoding();
				}
			else
				{
				sliceParser->DoneFrameDecoding();
				return;
				}
			}
		else
			{
			if (!SkipPictureBody()) return;
			}

		switch (playbackMode)
			{
			case VDPM_PLAY_FORWARD:
			case VDPM_TRICKPLAY:
			case VDPM_SCANNING:
				pastIPFrame = futureIPFrame;
				futureIPFrame = decodedIPFrame;
				break;
			case VDPM_PLAY_BACKWARD:
				futureIPFrame = decodedIPFrame;
				break;
			}
		}
	else
		{
		SkipPictureBody();
		}

	if (didDisplay)
		{
		fieldCount+=2;
		if (vcs == VCS_MPEG_2)
			{
			if (lastIPRepeatFirstField)
				fieldCount++;
			if (pullDownReconstruction && !mixedPullDownSequence)
				lowFieldCount = (7 * (lowFieldCount + 32) + fieldCount * 16) >> 3;
			else
				lowFieldCount = (3 * (lowFieldCount + 32) + fieldCount * 16) >> 2;
			}
		else
			lowFieldCount = 16 * fieldCount;
		}
	}

//
//  Post Previous Frame
//

static int Delay32Pulldown[] = {0, 0, 9, 0, 0, -8, -8, -17, -17};

void PictureParser::PostPreviousFrame(void)
	{
	if (previousDisplayBuffer)
		{
		previousDisplayBuffer->CompleteDeinterlaceFrame();
		if (previousWasIPFrame)
			display->PostIPFrameBuffer(previousDisplayBuffer, previousDisplayTime);
		else
			display->PostBFrameBuffer(previousDisplayBuffer, previousDisplayTime);

		previousDisplayBuffer = NULL;
		}
	}

//
//  Post IP Frame Buffer
//

void PictureParser::PostIPFrameBuffer(int displayTime, bool hurryUp)
	{
	int delayTime;
	Inverse32PulldownHint	ophint = phint;

	if (bFrameBuffers[1])
		{
		if (isStreamPlayback)
			{
			delayTime = 0;

			if (previousDisplayBuffer)
				{
				ipDisplayBuffer->DeinterlaceFrame(previousDisplayBuffer, phint, hurryUp);

				if (ipDisplayBuffer->fmode != PS_FRAME_PICTURE &&
					 ipDisplayBuffer->mode == PS_FRAME_PICTURE) delayTime = Delay32Pulldown[phint];

#if FRAME_ANNOTATION
				previousDisplayBuffer->DebugPrint(8, 40, "%08lx -> %08lx", previousDisplayBuffer, ipDisplayBuffer);
				previousDisplayBuffer->DebugPrint(8, 41, "FrameType %d (3:2 %d->%d) PStruct (%d->%d), (%d->%d)",
					previousWasIPFrame, ophint, phint,
					previousDisplayBuffer->fmode, previousDisplayBuffer->mode,
					ipDisplayBuffer->fmode, ipDisplayBuffer->mode);
#endif

				if (previousWasIPFrame)
					display->PostIPFrameBuffer(previousDisplayBuffer, previousDisplayTime);
				else
					display->PostBFrameBuffer(previousDisplayBuffer, previousDisplayTime);

				previousDisplayBuffer = NULL;
				}
			else
				ipDisplayBuffer->DeinterlaceInitialFrame();

			previousDisplayBuffer = ipDisplayBuffer;
			previousDisplayTime = displayTime + delayTime;
			previousWasIPFrame = TRUE;
			}
		else
			{
			ipDisplayBuffer->CompleteDeinterlaceFrame();
			display->PostIPFrameBuffer(ipDisplayBuffer, displayTime);
			}
		}
	else
		{
#if FRAME_ANNOTATION
		ipDisplayBuffer->DebugPrint(8,  8, "IPFrame Disp# %3d", displayFrameCount++);
		ipDisplayBuffer->DebugPrint(8, 10, "Progr %d Repeat %d (%d)", (int)lastIPProgressiveFrame, (int)lastIPRepeatFirstField, (int)ipDisplayBuffer->mode);
#endif
		display->PostIPFrameBuffer(ipDisplayBuffer, displayTime);
		}
	}

//
//  Post B Frame Buffer
//

void PictureParser::PostBFrameBuffer(int displayTime, Inverse32PulldownHint bphint, bool hurryUp)
	{
	int delayTime;
	Inverse32PulldownHint	ophint = bphint;

	if (bFrameBuffers[1])
		{
		if (isStreamPlayback)
			{
			delayTime = 0;
			if (previousDisplayBuffer && bDisplayBuffer)
				{
				bDisplayBuffer->DeinterlaceFrame(previousDisplayBuffer, bphint, hurryUp);
				if (bphint == IPDH_UNKNOWN && !repeatFirstField) phint = IPDH_UNKNOWN;

				if (bDisplayBuffer->fmode != PS_FRAME_PICTURE &&
					 bDisplayBuffer->mode == PS_FRAME_PICTURE) delayTime = Delay32Pulldown[bphint];

#if FRAME_ANNOTATION
				previousDisplayBuffer->DebugPrint(8, 40, "%08lx -> %08lx", previousDisplayBuffer, bDisplayBuffer);
				previousDisplayBuffer->DebugPrint(8, 41, "FrameType %d (3:2 %d->%d) PStruct (%d->%d), (%d->%d)",
					previousWasIPFrame, ophint, bphint,
					previousDisplayBuffer->fmode, previousDisplayBuffer->mode,
					bDisplayBuffer->fmode, bDisplayBuffer->mode);
#endif

				if (previousWasIPFrame)
					display->PostIPFrameBuffer(previousDisplayBuffer, previousDisplayTime);
				else
					display->PostBFrameBuffer(previousDisplayBuffer, previousDisplayTime);

				previousDisplayBuffer = NULL;
				}
			else
				bDisplayBuffer->DeinterlaceInitialFrame();

			previousDisplayBuffer = bDisplayBuffer;
			previousDisplayTime = displayTime + delayTime;
			previousWasIPFrame = FALSE;
			}
		else
			{
			bDisplayBuffer->CompleteDeinterlaceFrame();
			display->PostBFrameBuffer(bDisplayBuffer, displayTime);
			}
		}
	else
		{
#if FRAME_ANNOTATION
		bDisplayBuffer->DebugPrint(8,  8, "BFrame  Disp# %3d", displayFrameCount++);
		bDisplayBuffer->DebugPrint(8, 10, "Progr %d Repeat %d (%d)", progressiveFrame, repeatFirstField, bDisplayBuffer->mode);
#endif
		display->PostBFrameBuffer(bDisplayBuffer, displayTime);
		}
	}

//
//  Parse Picture
//

void PictureParser::ParsePicture(void)
	{
	int displayTime, currentTime, newTimeStamp, deltaTime, timeStamp;
	bool isFieldMaterial, doDecode, didDisplay, thisRepeatFirstField;
	PictureStructure	fieldDisplayMode;
	DWORD deinterlaceMode;
	FrameStore * currentFrame, * currentDisplayFrame;
	PictureDisplayBuffer * currentDisplayBuffer;
	int decodedIPFrame;
	Inverse32PulldownHint	bphint = IPDH_UNKNOWN;

#if FRAME_STATISTICS
static	int enterTime, postTime, getTime;
static	char cts[] = {'X', 'I', 'P', 'B', 'x', 'i', 'p', 'b'};
#endif

	bool hurryUp;

	AllocateFrameBuffers();

	newTimeStamp = bitStream->GetCurrentTimeStamp();
	if (newTimeStamp == previousTimeStamp)
		newTimeStamp = INVALID_TIME_STAMP;
	else if (newTimeStamp != INVALID_TIME_STAMP)
		previousTimeStamp = newTimeStamp;

	ParsePictureHeader();
	if (vcs == VCS_MPEG_2)
		{
		ParsePictureCodingExtension();
		ParseExtensionAndUserData();

		pullDownSequence |= repeatFirstField;
		progressiveSequence |= progressiveFrame;

		progressiveHistory <<= 1;
		if (progressiveFrame) progressiveHistory |= 1;
		repeatFirstHistory <<= 1;
		progressiveDisplayHistory <<= 1;
		if (isIPFrame)
			{
			if (lastIPRepeatFirstField) repeatFirstHistory |= 1;
			if (lastIPProgressiveFrame) progressiveDisplayHistory |= 1;
			}
		else
			{
			if (repeatFirstField) repeatFirstHistory |= 1;
			if (progressiveFrame) progressiveDisplayHistory |= 1;
			}


		if (pullDownSequence)
			{
			DWORD upper = (progressiveHistory >> 8);
			DWORD lower = progressiveHistory & 0xff;

			if (((upper & 0x0000ff) == 0x0000ff ||
				  (upper & 0x00ff00) == 0x00ff00 ||
				  (upper & 0xff0000) == 0xff0000) && lower != 0xff) mixedPullDownSequence = TRUE;
			if ((progressiveHistory & 0x00ff) == 0x00fe) mixedPullDownSequence = TRUE;
			if ((progressiveHistory & 0xffff) == 0x578e) mixedPullDownSequence = TRUE;
//			if ((progressiveHistory & 0xffff) == 0x57fe) mixedPullDownSequence = TRUE;
			if ((progressiveHistory & 0x000f) == 0x0000) mixedPullDownSequence = TRUE;
			if ((progressiveHistory & 0x01ff) == 0x01c7) mixedPullDownSequence = FALSE;
			if ((progressiveHistory & 0x003f) == 0x003f) mixedPullDownSequence = FALSE;

			upper = repeatFirstHistory >> 16;
			lower = repeatFirstHistory & 0xffff;

			//if (lower == 0x0000) phint = IPDH_UNKNOWN;

			if ((repeatFirstHistory & 3) == 0) mixedPullDownSequence = TRUE;
			}

		switch (bobWeaveLevel)
			{
			case BWL_FORCE_BOB:
				isFieldMaterial = TRUE;
				break;
			case BWL_FORCE_WEAVE:
				isFieldMaterial = FALSE;
				break;
			case BWL_PREFERE_BOB:
				isFieldMaterial = !(PF_FRAME_PRED_FRAME_DCT(pictureFlags) && progressiveFrame);
				break;
			case BWL_PREFERE_WEAVE:
				isFieldMaterial = !(PF_FRAME_PRED_FRAME_DCT(pictureFlags) || progressiveSequence);
				break;
			case BWL_SMART_DECISION:
				isFieldMaterial = !(PF_FRAME_PRED_FRAME_DCT(pictureFlags) || progressiveSequence && !mixedPullDownSequence);
				break;
			}

		if (bFrameBuffers[1] && progressiveFrame) isFieldMaterial = FALSE;

		if (playbackMode == VDPM_PLAY_BACKWARD)
			{
			pictureFlags ^= 0x20; // invert top field first
			}

		if (pictureStructure == PS_FRAME_PICTURE)
			fieldDisplayMode = isFieldMaterial ? (PF_TOP_FIELD_FIRST(pictureFlags) ? PS_TOP_FIELD : PS_BOTTOM_FIELD) : pictureStructure;
		else
			fieldDisplayMode = pictureStructure;

		deinterlaceMode = display->GetDeinterlaceFlags();

		if (fieldDisplayMode == PS_FRAME_PICTURE)
			{
			if (!(deinterlaceMode & DEIF_DEINTERLACE_WEAVE))
				fieldDisplayMode = PF_TOP_FIELD_FIRST(pictureFlags) ? PS_TOP_FIELD : PS_BOTTOM_FIELD;
			}
		else
			{
			if (!(deinterlaceMode & DEIF_DEINTERLACE_BOB))
				fieldDisplayMode = PS_FRAME_PICTURE;
			}
		}
	else
		{
		ParseExtensionAndUserData();
		pictureStructure = PS_FRAME_PICTURE;
		fieldDisplayMode = PS_FRAME_PICTURE;
		isFieldMaterial = FALSE;
		sliceParser->SetPictureParameters(0, PS_FRAME_PICTURE, 0x10); // FRAME_PRED_FRAME_DCT
		}


#if FRAME_STATISTICS
	enterTime = display->CurrentTime();
#endif

	displayTime = (int)((__int64)lowFieldCount * (500000 / 16) / framesPerSecond + timeOffset);


	if (isIPFrame)
		{
		timeStamp = lastIPTimeStamp;
		lastIPTimeStamp = newTimeStamp;
		thisFrameRepeat = ipFrameRepeat;
		ipFrameRepeat = frameRepeat;
		}
	else
		{
		timeStamp = newTimeStamp;
		thisFrameRepeat = frameRepeat;
		}

	if (playbackMode != VDPM_PLAY_BACKWARD)
		{
		if (timeStamp >= 0)
			{
			deltaTime = displayTime - timeStamp;
			if (deltaTime < 0) deltaTime = -deltaTime;

			if (deltaTime < 200)
				timeStamp = (displayTime * 7 + timeStamp) >> 3;

			timeOffset += timeStamp - displayTime;
			displayTime = timeStamp;
			}
		}

	currentTime = display->CurrentTime();

	if (bFrameBuffers[1])
		hurryUp = (displayTime < currentTime + fieldTime * 13 / 4) && (fieldCount > 50);
	else
		hurryUp = (displayTime < currentTime + fieldTime * 5 / 4) && (fieldCount > 50);

	didDisplay = TRUE;

	if (alwaysHurryUp && framesPerSecond > 50) hurryUp = TRUE;

	frameDropCounter += frameDropIncrement;

#if NO_DROPPED_FRAMES
	if (isIPFrame || !initialBFrameSkip || playbackMode == VDPM_TRICKPLAY)
		{
		hurryUp = FALSE;
#elif ONLY_I_FRAMES
	if (codingType == INTRA_CODED)
		{
		hurryUp = FALSE;
#elif ONLY_IP_FRAMES
	if (isIPFrame)
		{
		hurryUp = FALSE;
#else
	if (isIPFrame || (displayTime >= currentTime && !initialBFrameSkip && frameDropCounter < 100) || playbackMode == VDPM_TRICKPLAY)
		{
		if (playbackMode == VDPM_TRICKPLAY) hurryUp = FALSE;
#endif
		if (hurryUp) perfmon.framesDegraded++;
		hurryUp = FALSE;
		if (isIPFrame)
			{
			if (ipDisplayBuffer)
				{
				if (bobWeaveLevel == BWL_SMART_DECISION &&
					(deinterlaceMode & DEIF_CAN_CHANGE_DEINTERLACE_DISPLAY) &&
					ipDisplayBuffer->mode == PS_FRAME_PICTURE &&
					fieldDisplayMode != PS_FRAME_PICTURE)
					{
					ipDisplayBuffer->SetFieldDisplayMode(PF_TOP_FIELD_FIRST(lastIPPictureFlags) ? PS_TOP_FIELD : PS_BOTTOM_FIELD, fieldTime);
					}

				if (ipDisplayBuffer->mode == PS_FRAME_PICTURE)
					perfmon.framesWeave++;
				else
					perfmon.framesBob++;

				PostIPFrameBuffer(displayTime, hurryUp);

				currentDisplayFrame = ipFrameBuffers[futureIPFrame];
				initialBFrameSkip = FALSE;
				ipDisplayBuffer = NULL;

				if (lastIPRepeatFirstField)
					{
					if (phint != IPDH_UNKNOWN ||
						 ((repeatFirstHistory & 0xffff) == 0xaaaa) ||
						 ((repeatFirstHistory & 0xffff) == 0x5555))
						phint = IPDH_FRAME_FRAME1;
					}
				else
					{
					switch (phint)
						{
						case IPDH_UNKNOWN:
							break;
						case IPDH_FRAME_FRAME1:
							phint = IPDH_FRAME_FRAME2;
							break;
						case IPDH_FRAME_FRAME2:
							if (PF_TOP_FIELD_FIRST(lastIPPictureFlags))
								phint = IPDH_FRAME_TOP;
							else
								phint = IPDH_FRAME_BOTTOM;
							break;
						case IPDH_FRAME_TOP:
							phint = IPDH_BOTTOM_TOP;
							if (lastIPProgressiveFrame) phint = IPDH_UNKNOWN;
							break;
						case IPDH_FRAME_BOTTOM:
							phint = IPDH_TOP_BOTTOM;
							if (lastIPProgressiveFrame) phint = IPDH_UNKNOWN;
							break;
						case IPDH_TOP_FRAME:
						case IPDH_BOTTOM_FRAME:
							phint = IPDH_FRAME_FRAME1;
							break;
						case IPDH_TOP_BOTTOM:
							phint = IPDH_TOP_FRAME;
							if (lastIPProgressiveFrame) phint = IPDH_UNKNOWN;
							break;
						case IPDH_BOTTOM_TOP:
							phint = IPDH_BOTTOM_FRAME;
							if (lastIPProgressiveFrame) phint = IPDH_UNKNOWN;
							break;
						}
					}
				}
			else
				{
				didDisplay = FALSE;
				currentDisplayFrame = NULL;
				}

			lastIPPictureFlags = pictureFlags;

#if FRAME_STATISTICS
			postTime = display->CurrentTime();
#endif
			if (!WaitForPictureDataComplete()) return;

			sliceParser->EndFrameReference(ipFrameBuffers[pastIPFrame]);

			currentDisplayBuffer = ipDisplayBuffer = display->GetIPFrameBuffer(displayTime + 3000000 / framesPerSecond);
			if (!ipDisplayBuffer) return;

#if FRAME_STATISTICS
			getTime = display->CurrentTime();
#endif

			ipDisplayBuffer->SetAspectRatio(aspectRatio, panScanOffset);
			ipDisplayBuffer->SetFieldDisplayMode(fieldDisplayMode, fieldTime);
			if (bFrameBuffers[1])
				ipDisplayBuffer->PrepareDeinterlaceDuringDecode(NULL);

			if (playbackMode == VDPM_PLAY_BACKWARD)
				decodedIPFrame = 3 - (pastIPFrame + futureIPFrame);
			else
				decodedIPFrame = pastIPFrame;

			YieldTimedFiber(-1);
			doDecode = sliceParser->BeginFrameDecoding(ipFrameBuffers[futureIPFrame], ipFrameBuffers[futureIPFrame], ipFrameBuffers[decodedIPFrame], ipDisplayBuffer, hurryUp);
			currentFrame = ipFrameBuffers[decodedIPFrame];

			sliceParser->BeginFrameReference(currentFrame);

			thisRepeatFirstField = lastIPRepeatFirstField;
			lastIPRepeatFirstField = repeatFirstField;
			lastIPProgressiveFrame = progressiveFrame;
			}
		else
			{
			if (previousDisplayBuffer && previousDisplayTime < currentTime)
				{
				PostPreviousFrame();
				}

			if (!WaitForPictureDataComplete()) return;

			currentDisplayBuffer = bDisplayBuffer = display->GetBFrameBuffer(displayTime);

#if FRAME_STATISTICS
			getTime = display->CurrentTime();
#endif
			if (!bDisplayBuffer) return;

			bDisplayBuffer->SetAspectRatio(aspectRatio, panScanOffset);
			bDisplayBuffer->SetFieldDisplayMode(fieldDisplayMode, fieldTime);
			bphint = phint;

			if (bFrameBuffers[1])
				{
				if (phint == IPDH_UNKNOWN || fieldDisplayMode == PS_FRAME_PICTURE)
					bDisplayBuffer->PrepareDeinterlaceDuringDecode(previousDisplayBuffer);
				else
					bDisplayBuffer->PrepareDeinterlaceDuringDecode(NULL);
				}

			if (bFrameBuffers[1])
				{
				currentFrame = bFrameBuffers[previousBFrameID];
				previousBFrameID = 1 - previousBFrameID;
				}
			else
				currentFrame = bFrameBuffers[0];

			currentDisplayFrame = currentFrame;
			YieldTimedFiber(-1);
			doDecode = sliceParser->BeginFrameDecoding(ipFrameBuffers[pastIPFrame], ipFrameBuffers[futureIPFrame], currentFrame, bDisplayBuffer, hurryUp);

			if (repeatFirstField)
				{
				if (phint != IPDH_UNKNOWN ||
					 ((repeatFirstHistory & 0xffff) == 0xaaaa) ||
					 ((repeatFirstHistory & 0xffff) == 0x5555))
					phint = IPDH_FRAME_FRAME1;
				}
			else
				{
				switch (phint)
					{
					case IPDH_UNKNOWN:
						break;
					case IPDH_FRAME_FRAME1:
						phint = IPDH_FRAME_FRAME2;
						break;
					case IPDH_FRAME_FRAME2:
						if (PF_TOP_FIELD_FIRST(pictureFlags))
							phint = IPDH_FRAME_TOP;
						else
							phint = IPDH_FRAME_BOTTOM;
						break;
					case IPDH_FRAME_TOP:
						phint = IPDH_BOTTOM_TOP;
						if (progressiveFrame) phint = IPDH_UNKNOWN;
						break;
					case IPDH_FRAME_BOTTOM:
						phint = IPDH_TOP_BOTTOM;
						if (progressiveFrame) phint = IPDH_UNKNOWN;
						break;
					case IPDH_TOP_FRAME:
					case IPDH_BOTTOM_FRAME:
						phint = IPDH_FRAME_FRAME1;
						break;
					case IPDH_TOP_BOTTOM:
						phint = IPDH_TOP_FRAME;
						if (progressiveFrame) phint = IPDH_UNKNOWN;
						break;
					case IPDH_BOTTOM_TOP:
						phint = IPDH_BOTTOM_FRAME;
						if (progressiveFrame) phint = IPDH_UNKNOWN;
						break;
					}
				}

			thisRepeatFirstField = repeatFirstField;
			}

		if (doDecode)
			{
			if (ParsePictureBody())
				{
				if (currentDisplayBuffer && !hurryUp &&
					 (deinterlaceMode & DEIF_CAN_CHANGE_DEINTERLACE_DISPLAY) &&
					 fieldDisplayMode == PS_FRAME_PICTURE &&
					 bobWeaveLevel == BWL_SMART_DECISION && !progressiveFrame)
					{
					if (currentDisplayBuffer->HasInterlaceArtifacts())
						{
						mixedPullDownSequence = TRUE;
						fieldDisplayMode = PF_TOP_FIELD_FIRST(pictureFlags) ? PS_TOP_FIELD : PS_BOTTOM_FIELD;
						currentDisplayBuffer->SetFieldDisplayMode(fieldDisplayMode, fieldTime);
						}
					}

				sliceParser->DoneFrameDecoding();

#if FRAME_ANNOTATION
				currentDisplayBuffer->DebugPrint(16,  9, "Decd# %3d", decodeFrameCount++);
#endif
				perfmon.framesDecoded++;
				}
			else
				{
				sliceParser->DoneFrameDecoding();
				PostBFrameBuffer(displayTime, bphint, FALSE);
				if (bFrameBuffers[1]) previousFrame = currentDisplayFrame;
				return;
				}
			}
		else
			{
			//
			// Don't decode to far into the future
			//

			if (fieldCount > 20)
				{
				if (displayTime - currentTime > 200)
					Wait(&event, currentTime + 200, FALSE);
				}

			if (!SkipPictureBody()) return;

			perfmon.framesDropped++;
			}

		if (isIPFrame)
			{
			switch (playbackMode)
				{
				case VDPM_PLAY_FORWARD:
					pastIPFrame = futureIPFrame;
					futureIPFrame = decodedIPFrame;
					break;
				case VDPM_PLAY_BACKWARD:
					futureIPFrame = pastIPFrame;
					pastIPFrame = decodedIPFrame;
					break;
				case VDPM_SCANNING:
					pastIPFrame = futureIPFrame;
					futureIPFrame = decodedIPFrame;
					break;
				case VDPM_TRICKPLAY:
					PostIPFrameBuffer(displayTime, FALSE);
					display->DoneStreaming();
					initialBFrameSkip = FALSE;
					ipDisplayBuffer = NULL;
					pastIPFrame = futureIPFrame;
					futureIPFrame = decodedIPFrame;
					break;
				}
			}
		else
			{
			if (bDisplayBuffer->mode == PS_FRAME_PICTURE)
				perfmon.framesWeave++;
			else
				perfmon.framesBob++;

			switch (playbackMode)
				{
				case VDPM_PLAY_FORWARD:
				case VDPM_PLAY_BACKWARD:
				case VDPM_SCANNING:
					PostBFrameBuffer(displayTime, bphint, hurryUp);
					break;
				case VDPM_TRICKPLAY:
					PostBFrameBuffer(displayTime, bphint, FALSE);
					display->DoneStreaming();
					break;
				}

#if FRAME_STATISTICS
			postTime = display->CurrentTime();
#endif
			}

		if (bFrameBuffers[1]) previousFrame = currentDisplayFrame;

#if FRAME_STATISTICS
		AddStatus(displayTime, enterTime, postTime, getTime, GetInternalTime() - display->CurrentTime(), cts[codingType + 4 * isFieldMaterial], FALSE, timeStamp, repeatFirstField, progressiveFrame, display->CurrentTime() - enterTime, frameContinuationCounter);
#endif
		}
	else
		{
#if FRAME_STATISTICS
		AddStatus(displayTime, enterTime, enterTime, enterTime, GetInternalTime() - display->CurrentTime(), cts[codingType + 4 * isFieldMaterial], TRUE, timeStamp, repeatFirstField, progressiveFrame, display->CurrentTime() - enterTime, frameContinuationCounter);
#endif

		//
		// Don't decode to far into the future
		//

		if (displayTime - currentTime > 200)
			Wait(&event, currentTime + 200, FALSE);

		PostPreviousFrame();
		phint = IPDH_UNKNOWN;

		SkipPictureBody();

		perfmon.framesDropped++;
		thisRepeatFirstField = repeatFirstField;

		if (frameDropCounter >= 100) frameDropCounter -= 100;
		}

	if (didDisplay)
		{
		fieldCount+=2*thisFrameRepeat;
		if (!progressiveSequenceStructure && vcs == VCS_MPEG_2)
			{
			if (thisRepeatFirstField)
				fieldCount++;

			if (pullDownReconstruction)
				{
				if ((progressiveDisplayHistory & 0x07) == 0x06)
					{
					lowFieldCount += 32;
					}
				else
					{
					switch (repeatFirstHistory & 0x03)
						{
						case 0x00: lowFieldCount += 32; break;
						case 0x01: lowFieldCount += 40; break;
						case 0x02: lowFieldCount += 40; break;
						case 0x03: lowFieldCount += 48; break;
						}
					}
				}
			else
				{
				lowFieldCount += 32;
				if (thisRepeatFirstField)
					lowFieldCount += 16;
				}
			}
		else
			lowFieldCount = 16 * fieldCount;
		}


	pictureCnt++;

	if (playbackMode != VDPM_SCANNING) bitStream->NextStartCode();

	PictureDecodingCompleted();
	}

//
//  Done Streaming
//

void PictureParser::DoneStreaming(void)
	{
	int displayTime;

	doneStreamingLock.Enter();
	if (ipDisplayBuffer)
		{
		if (lastIPTimeStamp >= 0)
			displayTime = lastIPTimeStamp;
		else
			displayTime = (int)((__int64)lowFieldCount * (500000 / 16) / framesPerSecond + timeOffset);

		if (bFrameBuffers[1])
			{
			if (previousDisplayBuffer)
				{
				ipDisplayBuffer->DeinterlaceFrame(previousDisplayBuffer, phint, FALSE);

				if (previousWasIPFrame)
					display->PostIPFrameBuffer(previousDisplayBuffer, previousDisplayTime);
				else
					display->PostBFrameBuffer(previousDisplayBuffer, previousDisplayTime);

				previousDisplayBuffer = NULL;
				}
			}

		ipDisplayBuffer->CompleteDeinterlaceFrame();
		display->PostIPFrameBuffer(ipDisplayBuffer, displayTime);
		}

	ipDisplayBuffer = NULL;
	progressiveSequence = FALSE;
	progressiveSequenceStructure = false;
	mixedPullDownSequence = FALSE;
	pullDownSequence = FALSE;
	progressiveHistory = 0xffffffff;
	repeatFirstHistory = 0x00000000;
	progressiveDisplayHistory = 0x00000000;

	display->DoneStreaming();
	doneStreamingLock.Leave();
	}

//
//  Set Image Size
//

void PictureParser::SetImageSize(int width, int height, int aspectRatio)
	{
	DWORD deinterlaceMode;

	if (aspectRatio == 3)
		this->aspectRatio = PDAR_16by9;
	else
		this->aspectRatio = PDAR_4by3;

	if (highQuality || bobWeaveLevel == BWL_FORCE_WEAVE)
		deinterlaceMode = DEIF_DEINTERLACE_WEAVE;
	else
		deinterlaceMode = DEIF_DEINTERLACE_BOB | DEIF_DEINTERLACE_WEAVE;

	display->SetPreferedDeinterlaceMode(deinterlaceMode);

	if (initialized &&
	//	(display->CheckDisplayBuffersLost() ||
	(
		 (width != this->width || height != this->height) ||
		 deinterlaceMode != currentDeinterlaceMode &&
		 !(display->GetDeinterlaceFlags() & DEIF_CAN_CHANGE_DEINTERLACE_DECODE)))
		{
		display->InvalidateDecodingBuffer();
		previousDisplayBuffer = NULL;

		if (width != this->width || height != this->height)
			{
			ReleaseFrameBuffers();
			}

		initialBFrameSkip = TRUE;
		ipDisplayBuffer = bDisplayBuffer = NULL;

		initialized = FALSE;
		}

	currentDeinterlaceMode = deinterlaceMode;

	if (!initialized)
		{
		this->width = width;
		this->height = height;

		sliceParser->SetImageSize(width, height);

		display->InitDisplay(width, height, sliceParser->MinimumStretch());

		futureIPFrame = 1;
		pastIPFrame = 0;

		initialized = TRUE;
		}

	panScanOffset = width << 13;
	}

//
//  Set Quantizer Matrix
//

void PictureParser::SetQuantMatrix(QuantMatrices type, const QuantizationMatrix & matrix)
	{
	sliceParser->SetQuantMatrix(type, matrix);
	}

//
//  Set Frame Rate
//

void PictureParser::SetFrameRate(int framesPerSecond)
	{
	this->framesPerSecond = framesPerSecond;
	if (framesPerSecond && playbackSpeed)
		fieldTime = 500000 / framesPerSecond;
	else
		fieldTime = 16;
	}

//
//  Set Closed GOP
//

void PictureParser::SetClosedGOP(void)
	{
	initialBFrameSkip = FALSE;
	}

//
//  Set Broken Link
//

void PictureParser::SetBrokenLink(void)
	{
	initialBFrameSkip = TRUE;
	}

//
//  Set Video Coding Standard
//

void PictureParser::SetVideoCodingStandard(VideoCodingStandard vcs)
	{
	this->vcs = vcs;
	sliceParser->SetVideoCodingStandard(vcs);
	}

void PictureParser::SetProgressiveSequence(bool progressiveSequence)
	{
	this->progressiveSequenceStructure = progressiveSequence;
	}

//
//  Pre Parse
//

void PictureParser::PreParse(PictureCodingType & codingType)
	{
	PreParsePicture(codingType);
	}

//
//  Parse
//

void PictureParser::Parse(PictureDecodeMode mode)
	{
	switch (mode)
		{
		case PPM_DECODE_AND_DISPLAY:
			ParsePicture();
			break;
		case PPM_DECODE_AND_DROP:
			ParseTempPicture();
			break;
		case PPM_DECODE_AND_KEEP:
			ParseTempPicture();
			break;
		}
	}

//
//  Constructor
//

PictureParser::PictureParser(GenericSliceParser * highQualitySliceParser,
                             GenericSliceParser * lowQualitySliceParser,
									  GenericPictureDisplay * display,
									  VideoBitStream * bitStream,
									  GenericProfile * globalProfile,
									  GenericProfile * profile) : TimingClient(display), event(FALSE, TRUE)
	{
	this->highQualitySliceParser = highQualitySliceParser;
	this->lowQualitySliceParser = lowQualitySliceParser;
	this->sliceParser = lowQualitySliceParser;

	this->display = display;
	this->bitStream = bitStream;
	this->globalProfile = globalProfile;
	this->profile = profile;

	currentDeinterlaceMode = 0;

	initialized = FALSE;

	ipFrameBuffers[0] = NULL;
	ipFrameBuffers[1] = NULL;
	ipFrameBuffers[2] = NULL;
	bFrameBuffers[0] = NULL;
	bFrameBuffers[1] = NULL;
	previousFrame = NULL;
	previousBFrameID = 0;
	previousDisplayBuffer = NULL;

	width = 0; height = 0;

	globalProfile->Read("VideoDecoder", "BobWeave", (int&)bobWeaveLevel, 4);
	globalProfile->Read("VideoDecoder", "PullDownReconstruction", pullDownReconstruction, TRUE);
	globalProfile->Read("VideoDecoder", "AlwaysHurryUp", alwaysHurryUp, FALSE);
	globalProfile->Read("VideoDecoder", "FrameDropIncrement", frameDropIncrement, 10, 0);
	}

PictureParser::~PictureParser(void)
	{
	if (ipFrameBuffers[0]) delete ipFrameBuffers[0];
	if (ipFrameBuffers[1]) delete ipFrameBuffers[1];
	if (ipFrameBuffers[2]) delete ipFrameBuffers[2];
	if (bFrameBuffers[0])  delete bFrameBuffers[0];
	if (bFrameBuffers[1])  delete bFrameBuffers[1];
	}

//
//  Begin Streaming
//

void PictureParser::BeginStreaming(VideoPlaybackMode playbackMode, bool highQuality)
	{
	doneStreamingLock.Enter();
#if FRAME_ANNOTATION
	decodeFrameCount = 0;
	displayFrameCount = 0;
#endif

	this->playbackMode = playbackMode;
	this->highQuality = highQuality;

	isStreamPlayback = (playbackMode == VDPM_PLAY_FORWARD || playbackMode == VDPM_PLAY_BACKWARD);

	sliceParser = highQuality ? highQualitySliceParser : lowQualitySliceParser;

	display->BeginStreaming(playbackMode == VDPM_SCANNING || playbackMode == VDPM_TRICKPLAY);

	if (initialized)
		{
		display->InvalidateDecodingBuffer();
		ReleaseFrameBuffers();

		initialized = FALSE;
		}

	fieldCount = 0;
	lowFieldCount = 0;
	previousTimeStamp = -1;
	timeOffset = 0;
	lastIPTimeStamp = 0;
	playbackSpeed = 0x10000;
	frameDropCounter = 0;

	progressiveSequence = FALSE;
	mixedPullDownSequence = FALSE;
	pullDownSequence = FALSE;
	progressiveHistory = 0xffffffff;

	ipDisplayBuffer = bDisplayBuffer = NULL;
	initialBFrameSkip = TRUE;
	phint = IPDH_UNKNOWN;

	sliceParser->InvalidateFrameReferences();
	doneStreamingLock.Leave();
	}

//
//  End Streaming
//

void PictureParser::EndStreaming(bool fullReset)
	{
	doneStreamingLock.Enter();
	display->EndStreaming(fullReset);
	sliceParser->InvalidateFrameReferences();

	previousDisplayBuffer = NULL;

	if (fullReset)
		{
		width = height = 0;
		}

#if FRAME_STATISTICS
	DumpStatus();
#endif
	doneStreamingLock.Leave();
	}

//
//  Start Streaming
//

void PictureParser::StartStreaming(int playbackSpeed)
	{
	doneStreamingLock.Enter();
	this->playbackSpeed = playbackSpeed;

	display->StartStreaming(playbackSpeed);

	if (framesPerSecond && playbackSpeed)
		fieldTime = 500000 / framesPerSecond;
	else
		fieldTime = 16;
	doneStreamingLock.Leave();
	}

//
//  Stop Streaming
//

void PictureParser::StopStreaming(void)
	{
	doneStreamingLock.Enter();
	display->StopStreaming();
	doneStreamingLock.Leave();
	}

//
//  Advance Frame
//

void PictureParser::AdvanceFrame(void)
	{
	display->AdvanceFrame();
	}

