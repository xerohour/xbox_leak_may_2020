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

#include <stdio.h>
#include "MPEGStatistics.h"

int statCoefficients;
int statBlocks;
int statBlocksIntra;
int statBlocksPattern;
int statBlocksSkipped;
int statPredicts[2][2][2];

int maxStatCoefficients;
int sumStatCoefficients;
int maxStatBlocks;
int sumStatBlocks;
int maxStatBlocksIntra;
int sumStatBlocksIntra;
int maxStatBlocksPattern;
int sumStatBlocksPattern;
int maxStatBlocksSkipped;
int sumStatBlocksSkipped;
int maxStatPredicts[2][2][2];
int sumStatPredicts[2][2][2];
int maxStatPredFetch;
int sumStatPredFetch;
int numStatFrames;
int numStatSkipped;

void StatFrameSkipped(void)
	{
	numStatSkipped++;
	}

void GetStatistics(char * str)
	{
	sprintf(str, "Skipped : %d of %d", numStatSkipped, numStatFrames + numStatSkipped);
	}


void StatFrameCompleted(void)
	{
	int i, j, k;
	int statPredFetch;

	sumStatCoefficients += statCoefficients;
	if (statCoefficients > maxStatCoefficients) maxStatCoefficients = statCoefficients;
	statCoefficients = 0;

	sumStatBlocks += statBlocks;
	if (statBlocks > maxStatBlocks) maxStatBlocks = statBlocks;
	statBlocks = 0;
	sumStatBlocksIntra += statBlocksIntra;
	if (statBlocksIntra > maxStatBlocksIntra) maxStatBlocksIntra = statBlocksIntra;
	statBlocksIntra = 0;
	sumStatBlocksPattern += statBlocksPattern;
	if (statBlocksPattern > maxStatBlocksPattern) maxStatBlocksPattern = statBlocksPattern;
	statBlocksPattern = 0;
	sumStatBlocksSkipped += statBlocksSkipped;
	if (statBlocksSkipped > maxStatBlocksSkipped) maxStatBlocksSkipped = statBlocksSkipped;
	statBlocksSkipped = 0;

	statPredFetch = 0;

	for(i=0; i<2; i++)
		{
		for(j=0; j<2; j++)
			{
			for(k=0; k<2; k++)
				{
				statPredFetch += statPredicts[i][j][k] * (16 + j) * (i ? (16 + k) : (8 + k));

				sumStatPredicts[i][j][k] += statPredicts[i][j][k];
				if (statPredicts[i][j][k] > maxStatPredicts[i][j][k]) maxStatPredicts[i][j][k] = statPredicts[i][j][k];
				statPredicts[i][j][k] = 0;
				}
			}
		}
	sumStatPredFetch += statPredFetch;
	if (statPredFetch > maxStatPredFetch) maxStatPredFetch = statPredFetch;

	numStatFrames++;
	}

void StatDisplay(FILE * f)
	{
	int i, j, k;

	if (numStatFrames)
		{
		fprintf(f, "Frames : %d\n", numStatFrames);
		fprintf(f, "Coefficients : %7d : %11.3f\n", maxStatCoefficients,  (double)sumStatCoefficients / numStatFrames);
		fprintf(f, "Blocks       : %7d : %11.3f\n", maxStatBlocks,        (double)sumStatBlocks / numStatFrames);
		fprintf(f, "Macroblocks  :\n");
		fprintf(f, "  Intra      : %7d : %11.3f\n", maxStatBlocksIntra,   (double)sumStatBlocksIntra / numStatFrames);
		fprintf(f, "  Pattern    : %7d : %11.3f\n", maxStatBlocksPattern, (double)sumStatBlocksPattern / numStatFrames);
		fprintf(f, "  Skipped    : %7d : %11.3f\n", maxStatBlocksSkipped, (double)sumStatBlocksSkipped / numStatFrames);

		for(i=0; i<2; i++)
			{
			for(j=0; j<2; j++)
				{
				for(k=0; k<2; k++)
					{
					fprintf(f, "Pred %d%d%d     : %7d : %11.3f\n", i,j,k, maxStatPredicts[i][j][k], (double)sumStatPredicts[i][j][k] / numStatFrames);
					}
				}
			}
		fprintf(f, "Pred Fetch   : %7d : %11.3f\n", maxStatPredFetch * 3 / 2, (double)sumStatPredFetch * 1.5 / numStatFrames);
		}
	}

void StatReset(void)
	{
	int i, j, k;

	numStatFrames = 0;
	numStatSkipped = 0;
	maxStatCoefficients = 0;
	sumStatCoefficients = 0;
	statCoefficients = 0;

	maxStatBlocks = 0;
	sumStatBlocks = 0;
	statBlocks = 0;
	maxStatBlocksIntra = 0;
	sumStatBlocksIntra = 0;
	statBlocksIntra = 0;
	maxStatBlocksPattern = 0;
	sumStatBlocksPattern = 0;
	statBlocksPattern = 0;
	maxStatBlocksSkipped = 0;
	sumStatBlocksSkipped = 0;
	statBlocksSkipped = 0;

	for(i=0; i<2; i++)
		{
		for(j=0; j<2; j++)
			{
			for(k=0; k<2; k++)
				{
				maxStatPredicts[i][j][k] = 0;
				sumStatPredicts[i][j][k] = 0;
				statPredicts[i][j][k] = 0;
				}
			}
		}
	}
