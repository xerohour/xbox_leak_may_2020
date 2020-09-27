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

#include "SPUDecoder.h"
#include "library\common\vddebug.h"

#define DUMP_SPU_EVENTS		0

SPUDecoder::SPUDecoder(SPUBitStream		*	stream,
			              SPUDisplay		*	display)
	{
	this->stream = stream;
	this->display = display;
	}

SPUDecoder::~SPUDecoder(void)
	{
	}

#define FSTA_DSP		0x00
#define STA_DSP		0x01
#define STP_DSP		0x02
#define SET_COLOR		0x03
#define SET_CONTR		0x04
#define SET_DAREA		0x05
#define SET_DSPAX		0x06
#define CHG_COLCON	0x07
#define CMD_END		0xff

void SPUDecoder::Parse(void)
	{
	int size;
	BYTE u, l;
	int packetStartTime, startTime;
	BYTE * cur, * final, * exit;
	int current, next, ext;
	bool hasStartCommand, hasDisplayArea, hasDisplayData, hasColor, hasContrast, hasChanged;
	bool isCurrentStarted;
	WORD color, contrast;
	int startX, stopX, startY, stopY;
	int oddData, evenData;
	SPUDisplayBuffer	*	map;
	BYTE * buffer, * chcon;
	int cmd;
	DWORD colorCont;
	int	uniqueID;
	bool forced;

	uniqueID = 0x00000001;
	isCurrentStarted = FALSE;

	bool firstShot = FALSE;

	buffer = new BYTE[54000];

	while (!terminate && stream->RefillBuffer(16))
		{
		u = stream->GetByte();
		l = stream->GetByte();
		size = MAKEWORD(l, u);
		if (size)
			{
			if (size >= 2 && size < 54000)
				{
				if (!stream->RefillBuffer(size * 8 - 16)) break;

				packetStartTime = stream->GetCurrentTimeStamp() + 1;

#if DUMP_SPU_EVENTS
					{
					char buffer[100];
					wsprintf(buffer, "SPU : PacketStartTime %d\n", packetStartTime);
					OutputDebugString(buffer);
					}
#endif

				stream->GetBytes(buffer + 2, size - 2);
				next = MAKEWORD(buffer[3], buffer[2]);

				hasStartCommand = FALSE;
				hasDisplayArea = FALSE;
				hasDisplayData = FALSE;
				hasColor = FALSE;
				hasContrast = FALSE;
				hasChanged = TRUE;
				exit = buffer + size;

				do {
					if (next > size-4) break;

					current = next;
					cur = buffer + next;
					next = MAKEWORD(cur[3], cur[2]);
					startTime = packetStartTime + (int)MAKEWORD(cur[1], cur[0]) * 1024 / 90;

#if DUMP_SPU_EVENTS
					{
					char buffer[100];
					wsprintf(buffer, "SPU : StartTime %d\n", startTime);
					OutputDebugString(buffer);
					}
#endif

					final = buffer + next;
					cur += 4;
					chcon = NULL;
					forced = FALSE;

					while (cur < exit && cur != final && (cmd = *cur++) != CMD_END)
						{
#if DUMP_SPU_EVENTS
					{
					char buffer[100];
					wsprintf(buffer, "SPU : Command %02x\n", cmd);
					OutputDebugString(buffer);
					}
#endif
						switch (cmd)
							{
							case STP_DSP:
								if (isCurrentStarted)
									{
									display->PostOSDStopTime(startTime);
									isCurrentStarted = FALSE;
									}

								hasStartCommand = FALSE;
								hasChanged = TRUE;
								break;
							case FSTA_DSP:
								forced = TRUE;
							case STA_DSP:
								hasStartCommand = TRUE;
								hasChanged = TRUE;
								isCurrentStarted = TRUE;
								break;
							case SET_COLOR:
								color = MAKEWORD(cur[1], cur[0]);
								cur += 2;
								hasColor = TRUE;
								hasChanged = TRUE;
								break;
							case SET_CONTR:
								contrast = MAKEWORD(cur[1], cur[0]);
								cur += 2;
								hasContrast = TRUE;
								hasChanged = TRUE;
								break;
							case SET_DAREA:
								startX = *cur++;
								stopX = *cur++;
								startX = (startX << 4) | (stopX >> 4);
								stopX = ((stopX & 0x0f) << 8) | *cur++;

								startY = *cur++;
								stopY = *cur++;
								startY = (startY << 4) | (stopY >> 4);
								stopY = ((stopY & 0x0f) << 8) | *cur++;

#if DUMP_SPU_EVENTS
					{
					char buffer[100];
					wsprintf(buffer, "SPU : DisplayArea %d, %d - %d, %d\n",
						startX, startY, stopX, stopY);
					OutputDebugString(buffer);
					}
#endif
								uniqueID++;
								hasDisplayArea = TRUE;
								hasChanged = TRUE;
								break;
							case SET_DSPAX:
								evenData = MAKEWORD(cur[1], cur[0]);
								oddData = MAKEWORD(cur[3], cur[2]);
								cur += 4;

								uniqueID++;
								hasDisplayData = TRUE;
								hasChanged = TRUE;
								break;
							case CHG_COLCON:
								chcon = cur;
								ext = MAKEWORD(cur[1], cur[0]);
								cur += ext;

								break;
							default:
								ext = MAKEWORD(cur[1], cur[0]);
								cur += ext;
							}
						}

					if (hasStartCommand && hasDisplayArea && hasDisplayData &&
						 hasColor && hasContrast)
						{
						map = display->GetOSDBitmap();
						if (map)
							{
							colorCont = ((DWORD)color << 16) | contrast;

							map->SetImage(buffer + oddData, buffer + evenData,	exit,

														startX, startY, stopX - startX + 1, stopY - startY + 1,
														colorCont, chcon, uniqueID);

							display->PostOSDBitmap(map, startTime, colorCont, forced);

							hasChanged = FALSE;
							}
						}
					} while (!terminate && next > current && cur < exit);
				}
			else
				{
				stream->Advance(stream->AvailBits());
				}
			}
		}

	delete[] buffer;
	}

void SPUDecoder::BeginStreaming(bool highQuality)
	{
	terminate = FALSE;
	display->BeginStreaming(highQuality);
	}

void SPUDecoder::StartStreaming(int playbackSpeed)
	{
	display->StartStreaming(playbackSpeed);
	}

void SPUDecoder::StopStreaming(void)
	{
	display->StopStreaming();
	}

void SPUDecoder::EndStreaming(bool fullReset)
	{
	terminate = TRUE;
	display->EndStreaming(fullReset);
	}
