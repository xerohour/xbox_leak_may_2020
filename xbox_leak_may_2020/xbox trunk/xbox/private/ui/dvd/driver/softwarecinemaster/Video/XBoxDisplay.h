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

#ifndef XBOXDISPLAY
#define XBOXDISPLAY

#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "VideoDisplay.h"
#include "library\common\profiles.h"
#include "..\common\ASyncErrorMessages.h"

//#define USE_TEXTURE_SURFACES
#undef USE_TEXTURE_SURFACES

#define RAV_COLOR_KEY  	D3DCOLOR_XRGB(0xff, 0x80, 0xd0)


#ifdef USE_TEXTURE_SURFACES
typedef LPDIRECT3DTEXTURE8 LPDIRECTDRAWSURFACE;
typedef LPDIRECT3DTEXTURE8 LPDIRECTDRAWSURFACE3;
#else
typedef LPDIRECT3DSURFACE8 LPDIRECTDRAWSURFACE;
typedef LPDIRECT3DSURFACE8 LPDIRECTDRAWSURFACE3;
#endif
#define DD_OK D3D_OK


class XBoxDisplayBuffer;

class XBoxDisplay : public GenericPictureDisplay
    {
    friend class XBoxDisplayBuffer;
    protected:
        bool                            macrovisionHideDisplay;
        bool                            enforceFlipSync;
        VideoDownscaleFilterLevelSet    downscaleFilter;
        LPDIRECTDRAWSURFACE             lpDDSOverlay;
        DWORD                           colorKeyColor;
        PictureStructure                currentDisplayMode;
        int                             decodeWidth, decodeHeight;
        TimedFiberMutex                 lock;
        XBoxDisplayBuffer				* currentDisplayBuffer, * previousDisplayBuffer;
		  bool									 overlayOpen;
		  DWORD									previousVBlankCount;


        virtual Error OpenOverlay(void);
        virtual void InitFrameBufferDisplay(void){}
        virtual void CloseOverlay(void);
        virtual void FreeDisplayBuffers(void);
        void ClearSurface(LPDIRECTDRAWSURFACE surface);
        void AllocateDisplayBuffers(void);
		void ClearDisplayBuffers(void) {}
        DWORD GetDeinterlaceFlags(void);

#ifdef USE_TEXTURE_SURFACES
		HRESULT RenderFrame(LPDIRECTDRAWSURFACE surf);
#endif


    public:
        int                     flipDoneTime;
        bool                    displayFreezeMode;
        LPDIRECTDRAWSURFACE     lpDDSecondary;
		int						sourceLeft, sourceTop, sourceWidth, sourceHeight;
		int						destLeft, destTop, destWidth, destHeight;
		int						outputLeft, outputTop, outputWidth, outputHeight;
		bool					sourceAdapt;
		bool					destAdapt;
		  bool					overlayAvailable;
		  int					horizScan;
        VideoCopyMode           eCGMSMode;

        XBoxDisplay(HINSTANCE hinst, GenericProfile * profile, GenericProfile * globalProfile);
        virtual ~XBoxDisplay(void);
        void InitDisplay(int width, int height, int minStretch);
        void EnableDisplay(bool enable);
        void UpdateDisplay(void);
        bool GetDisplayRect(RECT & rc);
        void EraseRectangle(const RECT & rc);

        virtual void SetColorKeyColor(DWORD color);

        void BeginFreezeDisplay(void);
        void EndFreezeDisplay(void);
        virtual void DisplayFrame(XBoxDisplayBuffer * buffer, PictureStructure mode);
        bool SupportsStripeAccess(void);
        bool NeedsPreviousFrame(void);
        void SetCropRectangle(WORD left, WORD top, WORD right, WORD bottom) { }
        virtual void SetSourceRectangle(bool autoAdapt, WORD left, WORD top, WORD width, WORD height);
        virtual void SetDestRectangle(bool autoAdapt, WORD left, WORD top, WORD width, WORD height);
        virtual void SetClientRectangle(bool autoAdapt, WORD left, WORD top, WORD width, WORD height);
        virtual void SetOutputRectangle(WORD left, WORD top, WORD width, WORD height);
		void GetDisplayWidth(short & w) { w = 640; }
		void GetDisplayHeight(short & h) { h = 480; }
        Error CheckHardwareResources(void) {GNRAISE_OK;}
        void SetMacrovisionLevel(int level);
        void SetCGMSMode(VideoCopyMode eCGMSMode);
        virtual bool FlipOverlay(LPDIRECTDRAWSURFACE to, DWORD flags);
        void CalculateFlipDoneTime(void);

    };


class GenericDirectXDisplay : public XBoxDisplay {};

class DirectXDisplay : public XBoxDisplay {};

class XBoxDisplayBuffer : public PictureDisplayBuffer
    {
    friend class XBoxDisplay;
    private:
        TimedFiberMutex         lock;

        XBoxDisplay          * display;
        LPDIRECTDRAWSURFACE      lpDDSurface, lpDDSecondary, lpDDPrevious;
        D3DSURFACE_DESC         desc, desc2, desc3;
		D3DLOCKED_RECT			xLock, xLock2, xLock3;
        int                     width, height;

        BYTE                    * lpSurface, * lpSecondary, * lpPrevious;

        LPDIRECTDRAWSURFACE     lpSurface1, lpSurface2;

        int                     lPitch;
        bool                    flipped;
        bool                    firstFieldDeinterlaced;
        int                     flipDoneTime;
		unsigned int			flipDoneField;
        int                     voffset;

        int                     lastUnlockTime;
        int                     deinterlaceY;
        int                     sstart;
        XBoxDisplayBuffer    * previousFrame;

        int                     decodeWidth, decodeHeight;

        bool Lock1Surface(LPDIRECTDRAWSURFACE & lpDDSurface1);
        bool Lock2Surfaces(LPDIRECTDRAWSURFACE & lpDDSurface1, LPDIRECTDRAWSURFACE & lpDDSurface2);
        bool Lock3Surfaces(LPDIRECTDRAWSURFACE & lpDDSurface1, LPDIRECTDRAWSURFACE & lpDDSurface2, LPDIRECTDRAWSURFACE & lpDDSurface3);

//        bool IsFlipStatusCompleted(LPDIRECTDRAWSURFACE lpDDSurface);

        void DeinterlaceSurfaces(LPDIRECTDRAWSURFACE lpDDTop, LPDIRECTDRAWSURFACE lpDDBottom, PictureStructure mode, FrameStore * frame, FrameStore * previous, FullWidthMMXSPUDisplayBuffer * cosd, FullWidthMMXSPUDisplayBuffer * posd, bool hurryUp);
    public:
        XBoxDisplayBuffer(XBoxDisplay             * display,
                                    LPDIRECTDRAWSURFACE lpDDSurface,
                                    LPDIRECTDRAWSURFACE lpDDSecondary,
                                    int                 width,
                                    int                 height);
        ~XBoxDisplayBuffer(void);

        void DisplayFrame(PictureStructure mode);

        bool DisplayCompleted(void);

        int DisplayCompletionDelay(void);

        bool BeginStripeAccess(FrameStore * frame, int sheight, PictureStructure fmode);
        bool NeedsFrameDataWithDirectYUVAccess(void);

        void DoneStripeAccess(void);

        void ClearFrame(void);

        void CopyStripe(int ss, PictureStructure fmode);

        bool AttemptDirectStripeYUVAccess(int width, int height, int ss, BPTR & p, BPTR & ip, int & stride);

        void DoneDirectStripeAccess(PictureStructure fmode);

        void UpdateOSDBitmap(SPUDisplayBuffer * map);

        void CopyFrame(FrameStore * frame);
        bool DeinterlaceFrame(PictureDisplayBuffer * previous, Inverse32PulldownHint & phint, bool hurryUp);
        bool CompleteDeinterlaceFrame(void);
        bool PrepareDeinterlaceDuringDecode(PictureDisplayBuffer * previous);
        bool DeinterlaceInitialFrame(void);

        bool HasInterlaceArtifacts(void);

        LPDIRECTDRAWSURFACE GetDirectDrawSurface(void) {return lpDDSurface;}

        void __cdecl DebugPrint(int x, int y, char * format, ...);
    };

#endif
