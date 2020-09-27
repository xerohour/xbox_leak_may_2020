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


#include "huffdec.h"


HuffmanTable::HuffmanTable(void)
	{
	root = new HuffEntry;
	}

HuffmanTable::HuffmanTable(HuffInit * init)
	{
	root = new HuffEntry;

	if (init)
		{
		while (init->str)
			{
			AddEntry(init->str, init->val);
			init++;
			}

		BuildXTable();
		}
	}

HuffmanTable::~HuffmanTable(void)
	{
	if (root) delete root;
	}

void HuffmanTable::AddEntry(const char * str, long val)
	{
	char c;
	HuffEntry * e = root;
	bool append = FALSE;

	while (c = *str++)
		{
		if (e->final)
			throw ObjectInUse();
		else if (c == '0')
			{
			if (!e->zero)
      		e->zero = new HuffEntry;
      	e = e->zero;
			}
		else if (c == '1')
			{
			if (!e->one)
      		e->one = new HuffEntry;
      	e = e->one;
			}
		else if (c == 'x')
			append = TRUE;
		else
			throw RangeViolation();
		}

	if (e->one || e->zero)
		throw ObjectInUse();

	e->final = TRUE;
	e->append = append;

	e->val = val;
	}

void HuffmanTable::BuildXRest(HuffXEntry * xentry, HuffEntry * entry, int predepth)
	{
	int i, j, k;
	HuffEntry * e;


	for(i=0; i<HUFF_PARTITION_SIZE; i++)
		{
		e = entry;
		j = i;
		k = 0;

		while (k < HUFF_PARTITION_BITS && e && !e->final)
			{
			if (j & (1 << (HUFF_PARTITION_BITS - 1)))
				e = e->one;
			else
				e = e->zero;
			j <<= 1;
			k++;
			}

		if (e)
			{
			if (e->final)
				{
				if (e->append) k++;

				xentry[i].u.val = (e->val << 8) | ((predepth + k) << 1) | 1;
				}
			else
				{
				xentry[i].u.sub = new HuffXEntry[HUFF_PARTITION_SIZE];
				BuildXRest(xentry[i].u.sub, e, predepth + HUFF_PARTITION_BITS);
				}
			}
		else
			xentry[i].u.val = 0;
		}
	}

void HuffmanTable::SecureXRest(HuffXEntry * xentry)
	{
	int i;

	for(i=0; i<HUFF_PARTITION_SIZE; i++)
		{
		if (!xentry[i].u.val)
			xentry[i].u.sub = xentry;
		else if (!(xentry[i].u.val & 1) && xentry[i].u.sub != xentry)
			SecureXRest(xentry[i].u.sub);
		}
	}

HuffmanTable::HuffXEntry * HuffmanTable::MergeXRest(HuffXEntry * me, HuffXEntry * with)
	{
	int i;

	if (me && with)
		{
		for(i=0; i<HUFF_PARTITION_SIZE; i++)
			{
			if (!(me[i].u.val & 1) && !(with[i].u.val & 1))
				me[i].u.sub = MergeXRest(me[i].u.sub, with[i].u.sub);

			if (me[i].u.val != with[i].u.val) return me;
			}

		return with;
		}
	else
		return me;
	}

void HuffmanTable::BuildXTable(void)
	{
	BuildXRest(xroot, root, 0);

	delete root;
	root = NULL;
	}

void HuffmanTable::Merge(HuffmanTable & table)
	{
	int i;

	for(i=0; i<HUFF_PARTITION_SIZE; i++)
		{
		if (!(      xroot[i].u.val & 1) &&
			 !(table.xroot[i].u.val & 1))
			xroot[i].u.sub = MergeXRest(xroot[i].u.sub, table.xroot[i].u.sub);
		}
	}

void HuffmanTable::Secure(void)
	{
	SecureXRest(xroot);
	}

void HuffmanTable::PrepareByteTable(int add)
	{
	int i, v;

	for(i=0; i<256; i++)
		{
		v = xroot[i].u.val;
		v = (v & 0xffffff00) | (((v & 0xff) >> 1) + add);
		xroot[i].u.val = v;
		}
	}

void HuffmanTable::BuildExtPatternTable(void)
	{
	int i;
	int v, d, s;

	for(i=0; i<128+32; i++)
		extPatternTable[i] = 0x00000000;

	for(i=4; i<256; i++)
		{
		v = xroot[i].u.val;
		s = (v & 0xff) >> 1;
		d = v >> 8;
		if (i & 0xc0)
			{
			extPatternTable[i >> 3] = d | ((s - 3) << 6);
			}
		else
			{
			extPatternTable[(i << 1)     + 32] = d | ((s - 6) << 6);
			extPatternTable[(i << 1) + 1 + 32] = d | ((s - 6) << 6);
			}
		}
	for(i=2; i<8; i++)
		{
		v = xroot[i >> 1].u.sub[(i & 1) << 7].u.val;
		s = (v & 0xff) >> 1;
		d = v >> 8;
		extPatternTable[i + 32] = d | ((s - 6) << 6);
		}

	}

void HuffmanTable::BuildExtDCTTable(void)
	{
	int i, j, k, l, b;
	long v, w;
	HuffXEntry * e;

//	extDCTTable = new unsigned long[0x407];
//	extDCTTable += ((32 - ((int)extDCTTable & 31)) & 31) >> 2;
//	extNIDCTTable = new unsigned long[0x407];
//	extNIDCTTable += ((32 - ((int)extNIDCTTable & 31)) & 31) >> 2;

	for(i=0; i<0x400; i++)
		{
		extDCTTable[i] = 0x00000000;
		extNIDCTTable[i] = 0x00000000;
		}

	for(i=0; i<0x200; i++)
		extNIDCTTable2[i] = 0x0000;

	for(i = 8; i < 512; i++)
		{
		v = xroot[i >> 1].u.val;

		if ((v >> 8) == -1)
			{
			extDCTTable[i] = 0;
			extNIDCTTable[i] = 0;
			extNIDCTTable2[i] = 0;
			}
		else if ((v >> 8) == -2)
			{
			extDCTTable[i] = (v & 0xff) >> 1 << 16;
			extNIDCTTable[i] = (v & 0xff) >> 1 << 16;
			extNIDCTTable2[i] = (unsigned short) ( (v & 0xff) >> 1 << 12 );
			}
		else
			{
			l = (v & 0x000000ff) >> 1;
			k = (v >> 20) + 1;
			j = (v & 0x000fff00) >> 8;
			v = k | (l << 16) | (j << 4);
			w = (k << 16) | l | (j << 5) | 16;
//			w = k | (l << 16) | (j << 5) | 16;

			if (i & (512 >> l))
				{
				v = (v ^ 0x0000fff0) + 0x00000010;
				w = (w ^ 0x0000fff0) + 0x00000010;
				}

			extDCTTable[i] = v;
			extNIDCTTable[i] = w;

			extNIDCTTable2[i] = (unsigned short) ( (w & 0x0f) | (w >> 16 << 12) | (w & 0x0ff0) );
			}
		}

	for(j = 0; j < 4; j++)
		{
		e = xroot[j].u.sub;

		for(i = 0; i < 512; i++)
			{
			v = e[i >> 1].u.val;
			if (v)
				{
				l = (v & 0x000000ff) >> 1;
				k = (v >> 20) + 1;
				v = (v & 0x000fff00) >> 8;

				b = (j << 9) | i;

				w = (l - 8) | (v << 5) | (k << 16) | 16;
				v = (l - 8) | (v << 4) | (k << 16);

				if (b & (1 << (17-l)))
					{
					v = (v ^ 0x0000fff0) + 0x00000010;
					w = (w ^ 0x0000fff0) + 0x00000010;
					}

				if (l <= 14)
					{
					extDCTTable[0x200 + (b >> 3)] = v;
					extNIDCTTable[0x200 + (b >> 3)] = w;
					}
				else
					{
					extDCTTable[0x300 + b] = v;
					extNIDCTTable[0x300 + b] = w;
					}
				}
			}
		}
	}

void HuffmanTable::BuildFlatDCTTable(void)
	{
	int i, j, k, l, b;
	long v, w;
	HuffXEntry * e;

	flatDCTTable = new unsigned long[0x407];
	flatDCTTable += ((32 - ((int)flatDCTTable & 31)) & 31) >> 2;
	flatNIDCTTable = new unsigned long[0x407];
	flatNIDCTTable += ((32 - ((int)flatNIDCTTable & 31)) & 31) >> 2;

	for(i=0; i<0x400; i++)
		{
		flatDCTTable[i] = 0x00000040;
		flatNIDCTTable[i] = 0x00000040;
		}

	for(i = 8; i < 512; i++)
		{
		v = xroot[i >> 1].u.val;

		if ((v >> 8) == -1)
			{
			flatDCTTable[i]   = 0x40;
			flatNIDCTTable[i] = 0x40;
			}
		else if ((v >> 8) == -2)
			{
			flatDCTTable[i]   = ((v & 0xff) >> 1 << 8) | 0x80;
			flatNIDCTTable[i] = ((v & 0xff) >> 1 << 8) | 0x80;
			}
		else
			{
			l = (v & 0x000000ff) >> 1;		// size in bits
			k = (v >> 20);						// run 0..63
			j = (v & 0x000fff00) >> 8;		// level

			v = k | (l << 8) | (j << 17);
			w = k | (l << 8) | (j << 17) | 0x00010000;

			if (i & (512 >> l))
				{
				v = (v ^ 0xffff0000) + 0x00010000;
				w = (w ^ 0xffff0000) + 0x00010000;
				}

			flatDCTTable[i] = v;
			flatNIDCTTable[i] = w;
			}
		}

	for(j = 0; j < 4; j++)
		{
		e = xroot[j].u.sub;

		for(i = 0; i < 512; i++)
			{
			v = e[i >> 1].u.val;
			if (v)
				{
				l = (v & 0x000000ff) >> 1;		// size in bits
				k = (v >> 20);						// run 0..63
				v = (v & 0x000fff00) >> 8;		// level

				w = k | (l << 8) | (v << 17) | 0x00010000;
				v = k | (l << 8) | (v << 17);

				b = (j << 9) | i;

				if (b & (1 << (17-l)))
					{
					v = (v ^ 0xffff0000) + 0x00010000;
					w = (w ^ 0xffff0000) + 0x00010000;
					}

				if (l <= 14)
					{
					flatDCTTable  [0x200 + (b >> 3)] = v;
					flatNIDCTTable[0x200 + (b >> 3)] = w;
					}
				else
					{
					flatDCTTable  [0x300 + b] = v;
					flatNIDCTTable[0x300 + b] = w;
					}
				}
			}
		}
	}

DWORD VideoStreamBuffer[VIDEO_STREAM_BUFFER_SIZE+8];
int	BSBits;

VideoBitStream::VideoBitStream(StreamFlipCopy * streamFlipCopy)
	: BitStream(VideoStreamBuffer, VIDEO_STREAM_BUFFER_SIZE, BSBits, streamFlipCopy)
	{
	sequenceEndFound = FALSE;
	}

VideoBitStream::~VideoBitStream(void)
	{
	}

void VideoBitStream::NextStartCode(void)
	{
	int d, num;

	ByteAllign();

	RefillBuffer(4096 * 8);
	num = AvailBits();
	while (num >= 32)
		{
		if (num > 4096 * 8) num = 4096 * 8;
		while (num >= 32)
			{
			d = Peek();

			if ((d & 0xffffff00) == 0x00000100) return;

			if ((d & 0x000000ff) > 1)
				{
				Advance(32); num -= 32;
				}
			else
				{
				Advance(8); num -= 8;
				}
			}
		RefillBuffer(4096 * 8);
		num = AvailBits();
		}

	Advance(num);
	}

int VideoBitStream::AddData(BYTE * ptr, int num)
	{
	int done = BitStream::AddData(ptr, num);

	if (done > 4)
		{
		if (done == num && ptr[num-4] == 0x00 && ptr[num-3] == 0x00 &&
		                   ptr[num-2] == 0x01 && ptr[num-1] == 0xb7)
			sequenceEndFound = TRUE;
		}
	else if (done)
		sequenceEndFound = FALSE;

	return done;
	}

bool VideoBitStream::RefillBuffer(int requestBits, bool lowDataPanic)
	{
	if (sequenceEndFound)
		return BitStream::RefillBuffer(32, lowDataPanic);
	else
		return BitStream::RefillBuffer(requestBits, lowDataPanic);
	}

void VideoBitStream::FlushBuffer(void)
	{
	BitStream::FlushBuffer();
	sequenceEndFound = FALSE;
	}

