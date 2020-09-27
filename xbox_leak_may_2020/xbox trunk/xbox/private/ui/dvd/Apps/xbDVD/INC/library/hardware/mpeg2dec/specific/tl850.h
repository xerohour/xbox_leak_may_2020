// FILE:      library\hardware\mpeg2dec\specific\tl850.h
// AUTHOR:    Martin Stephan
// COPYRIGHT: (c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   05.05.1999
//
// PURPOSE:   TeraLogic TL850 Digital TV Decoder --- header file 
//
// HISTORY:

#ifndef TL850_H
#define TL850_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\intrctrl.h"
#include "library\general\asncstrm.h"
#include "..\..\clocks\generic\clocks.h"
#include "..\..\audio\generic\audiodac.h"
#if VIONA_VERSION
#include "..\generic\mp2spudc.h"
#endif

#include "..\generic\mp2eldec.h"
#include "..\generic\mp2vsprs.h"
#include "library\lowlevel\pci.h"

class FrameParameters
	{	
	public:
		BYTE				pfh, pfv, ppr1, ppr2;
		
		MPEG2FrameType				frameType;		
		MPEG2PictureStructure   fieldType;
		
		BOOL				repeatFirstField : 2;
		BOOL				topFieldFirst    : 2;
		BOOL				progressiveFrame : 2;
		BOOL				skipped          : 2;
		DWORD				temporalReference;
		DWORD				streamPosition;
		DWORD				pts;
		int				centerVerticalOffset;
		int				centerHorizontalOffset;
		BYTE				intraQuant[64];
		BYTE				nonIntraQuant[64];
				
		Error Init(MPEG2VideoHeaderParser	* params, DWORD streamPosition, DWORD pts, BYTE * intraQuant, BYTE * nonIntraQuant);
	};

class FrameParameterQueue
	{	
	private:
		FrameParameters	*	params;
		WORD	num, first, last;
	public:
		FrameParameterQueue(WORD size);
		virtual ~FrameParameterQueue(void);
		
		Error Reset(void);
		Error InsertFrame(MPEG2VideoHeaderParser * parser, DWORD streamPosition, DWORD pts, BYTE * intraQuant, BYTE * nonIntraQuant);
		Error MarkSkipped(void);
		BOOL IsSkipped(void);
		Error GetFrame(FrameParameters __far &params);
		Error NextFrame(void);
		WORD FrameAvail() {return (first <= last) ? (last - first) : (num + last - first);}
	};

class TL850MPEG2VideoHeaderParser : public MPEG2VideoHeaderParser
	{
	private:
		ByteIndexedInputPort	*	port;
		BOOL							odd;         
		BOOL							even;
		BYTE							second;
		Error WaitForFIFONonEmpty(void);
	protected:
		Error BeginParse(void);
		Error NextByte(void);
		Error EndParse(void);

		Error ParsePictureHeader(void);	
	public:
		BOOL	skipHeader;
		
		TL850MPEG2VideoHeaderParser(ByteIndexedInputPort	*	port);
	};

class TL850MPEG2Decoder;

class TL850PCIInterface
	{
	private:
		MemoryMappedIO		*mem;
			 
	public:
		TL850PCIInterface(TL850MPEG2Decoder * tl850);
		~TL850PCIInterface(void);

		Error Initialize(void);
		void	CheckPCIBusConfiguration(void);
		BOOL	CheckInterrupt(void);
		virtual void Interrupt(void);
	};

//all possible cursor looks are enumareted in an enum structure. Use this structure to choose a cursor!
enum CursorType
	{
	CURSOR_DEFAULT,
	CURSOR_ARROW,
	CURSOR_SMILEY
	};
		
class TL850MPEG2Decoder : public MPEG2VideoDecoder, protected InterruptHandler, protected ASyncRefillRequest,
									public OSDDisplay
	{
	friend class VirtualTL850MPEG2Decoder;
	friend class TL850VBlankIntServer;

	private: 
		
	protected:
		
		WORD	currentCursorXPos;
		WORD	currentCursorYPos;
		BYTE	currentCursorHScale;
		BYTE  currentCursorVScale;
		CursorType currentCursorType;

		WORD	currentOverlayXPos;
		WORD  currentOverlayYPos;
		WORD	currentOverlayWidth;
		WORD	currentOverlayHeight;
		BYTE	currentOverlayWindowHScale;
		BYTE  currentOverlayWindowVScale;

		Error OutByte(int idx, BYTE val);                             
		Error InByte(int idx, BYTE __far&val);
	
		Error OutDWordHL(int idx, DWORD val);
		Error OutDWordLH(int idx, DWORD val);
		Error OutDWordHL0(int idx, DWORD val);
		Error InDWordHL(int idx, DWORD __far& val);		

		//FrameParameterQueue					frames;  
		//TL850MPEG2VideoHeaderParser			parser;
		//PTSAssoc									ptsAssoc;
		BitIndexedInOutPort					* genPurposePort;
		ByteIndexedInOutPort					* auxVideoPort;
								
		enum XState
			{
			xreset,											//  0
			xinit_waitForFirstPictureHeader,    	//  1
			xinit_waitForBitBufferFull,         	//  2
			xinit_waitForStartSync,             	//  3
			xinit_waitForThirdFrame,            	//  4
			xfrozen,                            	//  5
			xplaying,                           	//  6
			xplayingRepeat0,								//  7
			xplayingRepeat1,                    	//  8 
			xplayingRepeat2,                    	//  9
			xplayingRepeat3,                    	// 10
			xstopped,                           	// 11
			xseeking,                           	// 12
			xresync_waitForFirstPictureHeader,  	// 13
			xresync_waitForBitBufferFull,       	// 14
			xresync_waitForStartSync,           	// 15
			xresync_waitForThirdFrame,          	// 16
			xresync_prefreeze1,							//	17
			xresync_prefreeze2,							//	18
			xplaying_recover,                   	// 19
			xstepping,                          	// 20
			xstarving,										// 21
			xcueing,											// 22
			xresyncue_waitForFirstPictureHeader,	// 23
			xresyncue_waitForBitBufferFull,			// 24
			xresyncue_waitForStart,						// 25
			xstill,		      							// 26
			xplayingStill,									// 27
			xscan_waitForFirstPictureHeader,			// 28
			xscan_waitForBitBufferFull,				// 29
			xscan_waitForStartSync,						// 30
			xscan_decoding									// 31
			} xstate;
			
		friend XState operator ++(XState __far& x, int a) {return (XState)(((int __far&)x)++);}

		MPEG2CommandQueue	commandQueue;

		DWORD GetPTS();

		Error SetPresentationMode(MPEG2PresentationMode presentationMode);
		Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		BOOL	CommandPending(DWORD tag);
		DWORD CurrentLocation(void);
		MPEGState CurrentState(void);
		DWORD LastTransferLocation(DWORD scale);
		
		Error HeaderInterrupt(void);
		Error DSyncInterrupt(void);
		Error VSyncInterrupt(BOOL top);
		Error BitBufferFullInterrupt(void);

		DWORD SendData(HPTR data, DWORD size);
		void	CompleteData(void);
		void	RefillRequest(DWORD free);

		Error SetSignalPosition(DWORD position);
		Error SetVideoBitrate(DWORD videoBitrate);
		Error SetVideoWidth(WORD width);
		Error SetVideoHeight(WORD height);
		Error SetVideoStandard(VideoStandard standard);
		Error SetVideoFPS(WORD fps);
		Error SetMPEG2Coding(BOOL mpeg2Coding);
		Error SetAspectRatio(WORD aspectRatio);
		 
		/* cursor methods */
		Error EnableCursor(void);
		Error DisableCursor(void);
		Error SetCursor(CursorType cursType, WORD xpos, WORD ypos, BYTE hscale, BYTE vscale);
		Error SetCursorLookupTable(BYTE tableNr, DWORD value);

		/* overlay methods */
		Error EnableOverlayWindow(void);
		Error DisableOverlayWindow(void);
		Error SetOverlayWindow(WORD width, WORD height, BYTE hscale, BYTE vscale, WORD xpos, WORD ypos);
		Error ShowOverlayColorFlow(void);

		/* OSD methods */
		Error KillOSDBitmap(void);
	   Error SetOSDBitmap(WORD numColors, 
	   					   BYTE __far * colorTable, BYTE __far * contrastTable,
	                     WORD x, WORD y, WORD width, WORD height,
	                     DWORD __huge * odd, DWORD __huge * even);
		Error SetOSDPalette(int entry, int y, int u, int v);
		Error ShowOSD(BOOL enable);

	public:
		
		/*
		TL850MPEG2Decoder(ByteIndexedInOutPort				* 	port,
		                     ASyncOutStream       		* 	strm,
		                     InterruptServer				*	irqServer,
		                     VirtualUnit						*	videoBus = NULL);
		*/
		TL850MPEG2Decoder(MemoryMappedIO *mem1, MemoryMappedIO *mem2, MemoryMappedIO *mem3);
		~TL850MPEG2Decoder(void);
		
		//
		// Memory area of the ASRs
		//
		MemoryMappedIO			*sdramMem;
		MemoryMappedIO			*regSpaceMem;
		MemoryMappedIO			*transBufferMem;
		TL850PCIInterface		*pciInterface;
		//Interface				*sdramInterface;

		//fast init is only for testing purpose. Init TL850 chip and show a test pattern.
		//All in one routine.
		Error FastInit(void);
		Error Initialize(void);
		Error BuildChipComponents(void);
		BOOL CheckChipInitialization(void);
		Error Configure(TAG __far * tags);

		void Interrupt(void);
		BOOL CheckInterrupt(void);

		VirtualUnit * CreateVirtual(void);		
   };

class VirtualTL850MPEG2Decoder : public VirtualMPEG2VideoDecoder
	{
	private:
		TL850MPEG2Decoder		*	decoder;
	protected:                                
		
	public:
		VirtualTL850MPEG2Decoder(TL850MPEG2Decoder * unit);

		Error Configure(TAG __far * tags);
	};

#endif
