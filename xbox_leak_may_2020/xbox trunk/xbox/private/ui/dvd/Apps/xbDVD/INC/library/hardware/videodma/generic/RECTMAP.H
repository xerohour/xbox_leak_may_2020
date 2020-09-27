
//
// FILE:      library\hardware\videodma\generic\rectmap.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   21.03.95
//
// PURPOSE:   Single plane bitmap operations, for video masks
//
// HISTORY:

#ifndef RECTMAP_H
#define RECTMAP_H

#include	"..\..\..\common\prelude.h"



// RectBitMap lines are aligned to DWORD boundary. The bit layout is
// selectable via enum BitLayout:
//
// BITLAYOUT_FIRST_BIT0         BITLAYOUT_FIRST_BIT7
//
// Byte   Bit   Pixel#          Byte   Bit   Pixel#
//   0     0     0                0     0     7
//   0     1     1                0     1     6
//   0     2     2                0     2     5
//   0     3     3                0     3     4
//   0     4     4                0     4     3
//   0     5     5                0     5     2
//   0     6     6                0     6     1
//   0     7     7                0     7     0
//   1     0     8                1     0    15
//   1     1     9                1     1    14
//   1     2    10                1     2    13
//   1     3    11                1     3    12
//   1     4    12                1     4    11
//   1     5    13                1     5    10
//   1     6    14                1     6     9
//   1     7    15                1     7     8
//   2     0    16                2     0    23
// etc.

class RectBitMap
	{
	protected:
		DWORD *leftMaskPtr, *rightMaskPtr;

	public:
		DWORD __far * data;
		int 	width, height;
		int 	left, top;  
		int 	dwpr;			// DWORDs per row

		enum BitLayout
			{
			BITLAYOUT_FIRST_BIT0, BITLAYOUT_FIRST_BIT7
			};

		BitLayout bitLayout;

		//
		// Create with size and offset. Prefill with bit pattern.
		//
		RectBitMap(int x, int y, int w, int h, DWORD fillBrush=0L, BitLayout layout=BITLAYOUT_FIRST_BIT0);
		~RectBitMap(void);
		
		//
		// Add a rectangle to the bitmap, i.e. set the bits to one.
		//
		virtual void AddRect(int x, int y, int w, int h);
		
		//
		// Remove a rectangle from the bitmap, i.e. set the bits to zero.
		//
		virtual void RemRect(int x, int y, int w, int h);
		
		//
		// Compare two bitmaps.
		//
		BOOL Equal(RectBitMap * m);
	};



// This is the same as above, except that it draws inverse.

class InverseRectBitMap : public RectBitMap
	{
	public:
		InverseRectBitMap (int x, int y, int w, int h, BitLayout layout=BITLAYOUT_FIRST_BIT0)
			: RectBitMap (x, y, w, h, 0xffffffff, layout)
			{}

		void AddRect(int x, int y, int w, int h) {RectBitMap::RemRect(x, y, w, h);}
		void RemRect(int x, int y, int w, int h) {RectBitMap::AddRect(x, y, w, h);}
	};



#endif
