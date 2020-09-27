//
// FILE:      library\hardware\videodma\specific\dummypip.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   05.12.95
//
// PURPOSE:   
//
// HISTORY:
#ifndef DUMMYPIP_H
#define DUMMYPIP_H

#include "..\generic\pip.h"

class DummyPIP : public PIP
	{
	private:
		BOOL			changed;
		int			dstLeft, dstTop, dstWidth, dstHeight;
		HWND			win;
		BOOL			adaptClientSize;
		BOOL			enabled;
		HINSTANCE   hinst;
	protected:
		Error SetDestRect(int left, int top, int width, int height);
		Error SetWindow(HWND win);
		Error Update(void);       
		Error UpdateDisplay(void);
		Error SetAdaptClientSize(BOOL adaptClientSize);
		Error EnablePIP(BOOL enable);

		int GetDisplayLeft(void) {return 0;}
		int GetDisplayTop(void) {return 0;}
		int GetDisplayWidth(void) {return dstWidth;}
		int GetDisplayHeight(void) {return dstHeight;}

		Error SetVideoStandard(VideoStandard std) {GNRAISE_OK;}
		Error SetSampleMode(VideoSampleMode mode) {GNRAISE_OK;}
		Error SetHOffset(int offset) {GNRAISE_OK;}
		Error SetVOffset(int offset) {GNRAISE_OK;}
		Error SetSourceRect(int left, int top, int width, int height) {GNRAISE_OK;}
		Error GrabFrame(FPTR base, WORD width, WORD height, 
		                        WORD stride, GrabFormat fmt) {GNRAISE_OK;}
		Error SetAdaptSourceSize(BOOL adaptSourceSize) {GNRAISE_OK;}
		Error SetScreenDest(BOOL screenDest) {GNRAISE_OK;}

		Error SetSourceCrop(int left, int top, int right, int bottom) {GNRAISE_OK;}

		virtual Error SetOffscreenOverride(BOOL offscreen) {GNRAISE_OK;}
		virtual Error SetOffscreenBase(FPTR base) {GNRAISE_OK;}
		virtual Error SetOffscreenBytesPerRow(WORD bpr) {GNRAISE_OK;}
		virtual Error SetOffscreenPixelFormat(PIPFormat format) {GNRAISE_OK;}

		virtual Error InitGrabStream (StreamCaptureHookHandle hook, DWORD milliSecPerFrame,
		                              WORD width, WORD height, WORD stride, GrabFormat format) {GNRAISE_OK;}
		virtual Error FinishGrabStream (void) {GNRAISE_OK;}
		virtual Error StartGrabStream (void) {GNRAISE_OK;}
		virtual Error StopGrabStream (void) {GNRAISE_OK;}
		
		FPTR GetStreamGrabBuffer() {GNRAISE_OK;}
	
		Error StartGrabYStream(YStreamCaptureHookHandle hook,
		                               int left, int top,
		                               WORD width, WORD height) {GNRAISE_OK;}
		Error StopGrabYStream(void) {GNRAISE_OK;}		
		Error SupplyStreamGrabBuffer(HPTR buffer) {GNRAISE_OK;}
		
		VirtualUnit * CreateVirtual(void);
	public:
		DummyPIP(HINSTANCE hinst) {this->hinst = hinst;}
	};
	

#endif
