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

#include "MPEGUserDataDecoder.h"

MPEGUserDataDecoder::MPEGUserDataDecoder(VideoBitStream * bitStream)
	{
	this->bitStream = bitStream;
	line21Decoder = NULL;
	fieldID = false;
	}

bool MPEGUserDataDecoder::ParseUserData(void)
	{
	BYTE ibuffer[256];
	BYTE obuffer[256];

	int i, j, n;

	for(i=0; i<4; i++)
		{
		ibuffer[i] = (BYTE)(bitStream->GetBits(8));
		}

	while (i < 256 && bitStream->PeekBits(24) != 0x000001) ibuffer[i++] = (BYTE)(bitStream->GetBits(8));

	while (bitStream->PeekBits(24) != 0x000001) bitStream->Advance(8);

	if (ibuffer[4] == 0x43 && ibuffer[5] == 0x43)
		{
		if (line21Decoder)
			{
			if (line21Decoder->NeedsPureData())
				{
				n = i;
				i = 0;
				j = 9;
				while (j <n && (ibuffer[j] & 0xfe) == 0xfe)
					{
					if (ibuffer[j] & 0x01)
						{
						obuffer[i] = ibuffer[j+1];
						obuffer[i+1] = ibuffer[j+2];
						i += 2;
						}
					j += 3;
					}
				}
			else
				{
				n = i;
				for(i=0; i<n; i++)
					obuffer[i] = ibuffer[i];
				i = 9 + 3 * (ibuffer[8] & 63);
				}

			line21Decoder->SendLine21Data(bitStream->GetCurrentTimeStamp(), obuffer, i);
			}
		}
	else if (ibuffer[4] == 0x47 && ibuffer[5] == 0x41 && ibuffer[6] == 0x39 && ibuffer[7] == 0x34)
		{
		n = ibuffer[9] & 0x1f;
		if (n && line21Decoder && ibuffer[8] == 0x03 && (ibuffer[9] & 0x40) != 0)
			{
			if (line21Decoder->NeedsPureData())
				{
				i = 0;
				for(j=0; j<n; j++)
					{
					if ((ibuffer[3*j+11] & 0x06) == 0x04)
						{
						obuffer[i  ] = ibuffer[3*j+12];
						obuffer[i+1] = ibuffer[3*j+13];
						i+=2;
						}
					}
				}
			else
				{
				obuffer[0] = ibuffer[0];
				obuffer[1] = ibuffer[1];
				obuffer[2] = ibuffer[2];
				obuffer[3] = ibuffer[3];
				obuffer[4] = 0x43;
				obuffer[5] = 0x43;
				obuffer[6] = 0x01;
				obuffer[7] = 0xf8;
				obuffer[8] = fieldID ? 0x40 : 0xc0;
				i = 0;
				for(j=0; j<n;j++)
					{
					if ((ibuffer[3*j+11] & 0x06) == 0x04)
						{
						if (fieldID != ((ibuffer[3*j+11] & 0x01) != 0))
							{
							obuffer[ 9+3*i] = 0xfe;
							obuffer[10+3*i] = 0x00;
							obuffer[11+3*i] = 0x00;
							i++;
							}

						obuffer[ 9+3*i] = 0xff;
						obuffer[10+3*i] = ibuffer[3*j+12];
						obuffer[11+3*i] = ibuffer[3*j+13];
						i++;

						fieldID = ((ibuffer[3*j+11] & 0x01) == 0);
						}
					}
				obuffer[8] |= i;
				i = i * 3 + 9;
				}

			line21Decoder->SendLine21Data(bitStream->GetCurrentTimeStamp(), obuffer, i);
			}
		}

	return TRUE;
	}

