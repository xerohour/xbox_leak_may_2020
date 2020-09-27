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

#ifndef FRAMESTORE_H
#define FRAMESTORE_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "..\spudecoder\SPUDisplayBuffer.h"
#include "GenericMMXYUVConverter.h"
#include "XMMXYUVConverter.h"

enum PictureStructure
	{
	PS_TOP_FIELD = 1,
	PS_BOTTOM_FIELD = 2,
	PS_FRAME_PICTURE = 3
	};

enum Inverse32PulldownHint
	{
	IPDH_UNKNOWN,
	IPDH_FRAME_FRAME1,
	IPDH_FRAME_FRAME2,
	IPDH_FRAME_TOP,
	IPDH_FRAME_BOTTOM,
	IPDH_TOP_FRAME,
	IPDH_BOTTOM_FRAME,
	IPDH_TOP_BOTTOM,
	IPDH_BOTTOM_TOP
	};

struct VideoDownscaleFilterLevelSet
	{
	VideoDownscaleFilterLevel	fullX : 8;
	VideoDownscaleFilterLevel	fullY : 8;
	VideoDownscaleFilterLevel	halfX : 8;
	VideoDownscaleFilterLevel	halfY : 8;
	};

static const VideoDownscaleFilterLevelSet	MinimumVideoDownscaleFilterLevelSet  = {VSQL_BINARY, VSQL_BINARY, VSQL_BINARY, VSQL_BINARY};

class FrameStore
	{
	protected:
		int			width, height, bytesPerRow;
		BYTE		*	iyb, * iuvb;
		BYTE		*	yb, * uvb, *maxybFrame, *maxybField;
		int			precision;
		int			stripeMask, mainStripeMask;

		GenericMMXYUVVideoConverter	*	yuvConverter;

		void ConvertToRGB32Stripe(BYTE * map,
										  int mwidth, int mheight, int mstride,
										  int sstart, int sheight, bool lace, PictureStructure fmode,
										  BYTE * bottomMap);
		void ConvertToRGB15Stripe(BYTE * map,
										  int mwidth, int mheight, int mstride,
										  int sstart, int sheight, bool lace, PictureStructure fmode,
										  BYTE * bottomMap);
		void ConvertToRGB16Stripe(BYTE * map,
										  int mwidth, int mheight, int mstride,
										  int sstart, int sheight, bool lace, PictureStructure fmode,
										  BYTE * bottomMap);

		void ConvertToRGB15ThroughOSDStripe(BYTE * map, FullWidthMMXSPUDisplayBuffer * osd,
			                                 int mwidth, int mheight, int mstride,
														int sstart, int sheight,
														bool lace, PictureStructure fmode,
														BYTE * bottomMap);

	public:
		FrameStore(GenericMMXYUVVideoConverter	*	yuvConverter, int width, int height, bool stripe = false);
		FrameStore(void);
		virtual ~FrameStore(void);

		bool IsFullFrame(void) {return stripeMask == 0xffffffff;}
		void EnableStripeMode(bool stripe);

		void SetPrecision(int prec) {precision = prec;}

		void ConvertToRGB(BYTE * map);


			//
			//	topMap          : target topMap or full map if bottomMap = NULL
			// bottomMap	    : target bottomMap, or NULL if interleaved bitmap
			// mwidth, mheight : target rectangle of full frame
			// mstride         : bytes per line in target map
			// sstart, sheight : vertical slice position
			// lace            : source material is interlaced, split U and V.  If the
			//                   bottom map is non NULL, lace has to be true
			// singleField     : the bottom field is dropped. The bottomMap argument
			//                   is ignored
			// fmode           : Picture structure of source picture
			// osd             : OSD bitmap which is to be overlayed
			//
		virtual void ConvertToYUVStripe(BYTE * topMap, BYTE * bottomMap,
			                             int mwidth, int mheight, int mstride,
												  int sstart, int sheight,
												  bool lace, bool singleField, PictureStructure fmode,
												  VideoDownscaleFilterLevelSet filterLevel,
												  FullWidthMMXSPUDisplayBuffer * osd = NULL);

			//
			//	topMap          : target topMap or full map if bottomMap = NULL
			// bottomMap	    : target bottomMap, or NULL if interleaved bitmap
			// mwidth, mheight : target rectangle of full frame
			// mstride         : bytes per line in target map
			// xoffset         : horizontal offset in 65536th of a pixel
			// sstart, sheight : vertical slice position
			// lace            : source material is interlaced, split U and V.  If the
			//                   bottom map is non NULL, lace has to be true
			// singleField     : the bottom field is dropped. The bottomMap argument
			//                   is ignored
			// fmode           : Picture structure of source picture
			// osd             : OSD bitmap which is to be overlayed
			//
		virtual void ConvertToYUVStripePanScan(BYTE * topMap, BYTE * bottomMap,
			                                    int mwidth, int mheight, int mstride, int xoffset,
												         int sstart, int sheight,
												         bool lace, bool singleField, PictureStructure fmode,
												         VideoDownscaleFilterLevelSet filterLevel,
												         FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripePanScanAverageDeinterlace(BYTE * topMap,
			                                                      int mwidth, int mheight, int mstride, int xoffset,
 												                           int sstart, int sheight,
												                           bool lace,
												                           VideoDownscaleFilterLevelSet filterLevel,
												                           FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripePanScanDeinterlace(BYTE * previousMap, BYTE * currentMap, BYTE * nextMap, FrameStore * previous,
			                                               int mwidth, int mheight, int mstride, int xoffset,
																        int sstart, int sheight,
																        bool topFieldFirst, DWORD flags,
																        VideoDownscaleFilterLevelSet filterLevel,
																        FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripePanScanInterpolate(BYTE * map, FrameStore * previous,
			                                               int mwidth, int mheight, int mstride, int xoffset,
												                    int sstart, int sheight,
												                    VideoDownscaleFilterLevelSet filterLevel,
												                    FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripePanScanDeinterlaceStretch(BYTE * topMap, BYTE * bottomMap,
			                                                      int mwidth, int mheight, int mstride, int xoffset,
																               int sstart, int sheight,
																               DWORD flags,
																               VideoDownscaleFilterLevelSet filterLevel,
																               FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeLetterbox(BYTE * topMap, BYTE * bottomMap,
			                                      int mwidth, int mheight, int mstride,
												           int sstart, int sheight,
												           bool lace, bool singleField, PictureStructure fmode,
												           VideoDownscaleFilterLevelSet filterLevel,
												           FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeLetterboxAverageDeinterlace(BYTE * topMap,
																					  int mwidth, int mheight, int mstride,
																					  int sstart, int sheight,
																					  bool lace,
																					  VideoDownscaleFilterLevelSet filterLevel,
																					  FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeLetterboxDeinterlace(BYTE * previousMap, BYTE * currentMap, BYTE * nextMap, FrameStore * previous,
			                                                 int mwidth, int mheight, int mstride,
																          int sstart, int sheight,
																          bool topFieldFirst, DWORD flags,
																          VideoDownscaleFilterLevelSet filterLevel,
																          FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeLetterboxInterpolate(BYTE * map, FrameStore * previous,
			                                                 int mwidth, int mheight, int mstride,
												                      int sstart, int sheight,
												                      VideoDownscaleFilterLevelSet filterLevel,
												                      FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeLetterboxDeinterlaceStretch(BYTE * topMap, BYTE * bottomMap,
			                                                        int mwidth, int mheight, int mstride,
																                 int sstart, int sheight,
																                 DWORD flags,
																                 VideoDownscaleFilterLevelSet filterLevel,
																                 FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeLetterboxSingleInverse32Pulldown(BYTE * topMap, FrameStore * previous,
			                                                             int mwidth, int mheight, int mstride,
												                                  int sstart, int sheight,
												                                  Inverse32PulldownHint phint,
												                                  VideoDownscaleFilterLevelSet filterLevel,
												                                  FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeSingleDeinterlace(BYTE * topMap, FrameStore * previous,
			                                              int mwidth, int mheight, int mstride,
												                   int sstart, int sheight,
												                   bool topField,
												                   VideoDownscaleFilterLevelSet filterLevel,
												                   FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeAverageDeinterlace(BYTE * topMap,
			                                               int mwidth, int mheight, int mstride,
												                    int sstart, int sheight,
												                    bool lace,
												                    VideoDownscaleFilterLevelSet filterLevel,
												                    FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeSingleInverse32Pulldown(BYTE * topMap, FrameStore * previous,
			                                                    int mwidth, int mheight, int mstride,
												                         int sstart, int sheight,
												                         Inverse32PulldownHint phint,
												                         VideoDownscaleFilterLevelSet filterLevel,
												                         FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeDeinterlace(BYTE * previousMap, BYTE * currentMap, BYTE * nextMap, FrameStore * previous,
			                                        int mwidth, int mheight, int mstride,
																 int sstart, int sheight,
																 bool topFieldFirst, DWORD flags,
																 VideoDownscaleFilterLevelSet filterLevel,
																 FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeDeinterlaceStretch(BYTE * topMap, BYTE * bottomMap,
			                                               int mwidth, int mheight, int mstride,
																        int sstart, int sheight,
																        DWORD flags,
																        VideoDownscaleFilterLevelSet filterLevel,
																        FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToYUVStripeInterpolate(BYTE * map, FrameStore * previous,
			                                        int mwidth, int mheight, int mstride,
												             int sstart, int sheight,
												             VideoDownscaleFilterLevelSet filterLevel,
												             FullWidthMMXSPUDisplayBuffer * osd = NULL);

		virtual void ConvertToRGBThroughOSDStripe(int depth, BYTE * map, FullWidthMMXSPUDisplayBuffer * osd,
			                                       int mwidth, int mheight, int mstride,
																int sstart, int sheight,
																bool lace, PictureStructure fmode,
																BYTE * bottomMap = NULL);

		virtual void ConvertToRGBStripe(int depth, BYTE * map,
			                             int mwidth, int mheight, int mstride,
												  int sstart, int sheight, bool lace, PictureStructure fmode,
												  BYTE * bottomMap = NULL);

		virtual bool Is32PulldownFrame(FrameStore * previousFrame, Inverse32PulldownHint phint);

		virtual bool Detect32PulldownFrame(FrameStore * previousFrame, Inverse32PulldownHint & phint, DWORD & history);

		virtual bool IsRenderingComplete(void) {return TRUE;}


		void PutY(int x, int y, BYTE val) {yb[x+y*bytesPerRow] = val;}
		void PutU(int x, int y, BYTE val) {uvb[2*x+y*bytesPerRow] = val;}
		void PutV(int x, int y, BYTE val) {uvb[2*x+y*bytesPerRow+1] = val;}

		void DrawRect(int x, int y, int w, int h, unsigned yuv);

		void GetPP(int x, int y, BPTR & yp, BPTR & uvp, int & stride)
			{
			y &= stripeMask;
			stride = bytesPerRow;
			yp = yb + x + y * bytesPerRow;
			x >>= 1;
			y >>= 1;
			uvp = uvb + 2*x + y * bytesPerRow;
			}

		void GetMPP(int x, int y, BPTR & yp, BPTR & uvp, int & stride)
			{
			y &= stripeMask;
			stride = bytesPerRow;
			y *= bytesPerRow;
			yp = yb + x + y;
			uvp = uvb + x + (y >> 1);
			}

		void GetIMPP(int f, int x, int y, BPTR & yp, BPTR & uvp, int & stride)
			{
			y &= stripeMask;
			stride = 2 * bytesPerRow;
			yp = yb + x + (2 * y + f) * bytesPerRow;
			uvp = uvb + x + (y + f) * bytesPerRow;
			}

		bool GetYPP(int x, int y, BPTR & yp)
			{
			yp = yb + x + y * bytesPerRow;
			return yp >= yb && yp < maxybFrame && x >= 0 && x < width;
			}

		void GetUVPP(int x, int y, BPTR & uvp)
			{
			uvp = uvb + 2*x + y * bytesPerRow;
			}

		void GetPPF(int f, int x, int y, BPTR & yp, BPTR & uvp)
			{
			yp = yb + x + (2 * y + f) * bytesPerRow;
			x >>= 1;
			y >>= 1;
			uvp = uvb + 2*x + (2 * y + f) * bytesPerRow;
			}

		bool GetYPPF(int f, int x, int y, BPTR & yp)
			{
			yp = yb + x + (2 * y + f) * bytesPerRow;
			return yp >= yb && yp < maxybField && x >= 0 && x < width;
			}

		void GetUVPPF(int f, int x, int y, BPTR & uvp)
			{
			uvp = uvb + 2*x + (2 * y + f) * bytesPerRow;
			}

		int GetY(int x, int y) {return yb[x+y*bytesPerRow];}
		int GetU(int x, int y) {return uvb[2*x+y*bytesPerRow];}
		int GetV(int x, int y) {return uvb[2*x+y*bytesPerRow+1];}

		int Width(void) {return width;}
		int Height(void) {return height;}
		int Stride(void) {return bytesPerRow;}
	};

class TiledFrameStore : public FrameStore
	{
	protected:
		int bytesPerTile;
	public:
		TiledFrameStore(GenericMMXYUVVideoConverter	*	yuvConverter, int width, int height, bool stripe = false);

		int Stride(void) {return bytesPerTile;}

		void GetMPP(int x, int y, BPTR & yp, BPTR & uvp, int & stride)
			{
			y &= stripeMask;
			stride = bytesPerTile;
			yp = yb + (x & 7) + (x >> 3) * bytesPerTile + y * 8;
			uvp = uvb + (x & 7) + (x >> 3) * (bytesPerTile >> 1) + (y >> 1) * 8;
			}

		void GetIMPP(int f, int x, int y, BPTR & yp, BPTR & uvp, int & stride)
			{
			y &= stripeMask;
			stride = bytesPerTile;
			yp = yb + (x & 7) + (x >> 3) * bytesPerTile + (2 * y + f) * 8;
			uvp = uvb + (x & 7) + (x >> 3) * (bytesPerTile >> 1) + (y + f) * 8;
			}

		bool GetYPP(int x, int y, BPTR & yp)
			{
			yp = yb + (x & 7) + (x >> 3) * bytesPerTile + y * 8;
			return yp >= yb && yp < maxybFrame;
			}

		void GetUVPP(int x, int y, BPTR & uvp)
			{
			uvp = uvb + (x & 3) * 2 + (x >> 2) * (bytesPerTile >> 1) + y * 8;
			}

		bool GetYPPF(int f, int x, int y, BPTR & yp)
			{
			yp = yb + (x & 7) + (x >> 3) * bytesPerTile + (2 * y + f) * 8;
			return yp >= yb && yp < maxybField && x >= 0 && x < width;
			}

		void GetUVPPF(int f, int x, int y, BPTR & uvp)
			{
			uvp = uvb + (x & 3) * 2 + (x >> 2) * (bytesPerTile >> 1) + (2 * y + f) * 8;
			}
	};

//#include "..\mpegDecoder\SplitYUVFrameBuffer.h"

#endif
