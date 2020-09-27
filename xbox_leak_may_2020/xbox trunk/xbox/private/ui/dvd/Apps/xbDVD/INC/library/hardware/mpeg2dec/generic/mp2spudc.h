#ifndef MP2SPUDC
#define MP2SPUDC

#include "mp2eldec.h"
#include "library\general\lists.h"
#include "library\lowlevel\kernlmem.h"

#define GNR_SPU_OVERLAY_NOT_SUPPORTED		MKERR(ERROR, SUBPICTURE, GENERAL, 0x01)

#ifndef ONLY_EXTERNAL_VISIBLE

class OSDDisplay
	{
	protected:
		WORD osdColorMask;
	public:
	   virtual Error KillOSDBitmap(void) = 0;
	   
	   virtual Error SetOSDBitmap(WORD numColors, 
	   								   BYTE __far * colorTable, BYTE __far * contrastTable,
	                              WORD x, WORD y, WORD width, WORD height,
	                              DWORD __huge * odd, DWORD __huge * even) = 0;
	
		virtual Error SetOSDPalette(int entry, int y, int u, int v) = 0;	                                   
		
		virtual Error ShowOSD(BOOL enable) = 0;
	};

class SPUDisplay
	{
	public:
		virtual ~SPUDisplay(void) {}
		
		virtual Error KillSPUBitmap(void) = 0;
		virtual Error SetSPUBitmap(BYTE __far * odd, BYTE __far * even) = 0;

		virtual Error SetSPUPosition(int x, int y, int w, int h) = 0;
		virtual Error SetSPUColors(int back, int front, int emph1, int emph2) = 0;
		virtual Error SetSPUContrast(int back, int front, int emph1, int emph2) = 0;
	   virtual Error SetSPUExtColors(int sBack, int sFront, int sEmph1, int sEmph2,
	                                 int aBack, int aFront, int aEmph1, int aEmph2) = 0;
	   virtual Error SetSPUExtContrast(int sBack, int sFront, int sEmph1, int sEmph2,
	                                   int aBack, int aFront, int aEmph1, int aEmph2) = 0;
		virtual Error SetSPUPalette(int entry, int y, int u, int v) = 0;	                                   
		
		virtual Error SetButtonPosition(int x, int y, int w, int h) = 0;

		virtual Error ShowSPU(BOOL enable) = 0;		
		virtual Error ShowButton(SPUButtonState state) = 0;
		
		virtual Error Update(void) {GNRAISE_OK;}
	};


class SPUonOSDDisplay : public SPUDisplay
	{
	private:
		OSDDisplay	*	osd;
	   
	   BYTE				baseColorTable[12];
	   BYTE				baseContrastTable[12];
	   
	   BYTE				usedColorTable[8];
	   BYTE				usedContrastTable[8];
	   
	   KernelMemory	oddBitmapMemory;
	   KernelMemory	evenBitmapMemory;
	   
	   DWORD	__huge *	oddBitmapBuffer;
	   DWORD	__huge *	evenBitmapBuffer;
	   
	   BYTE	__far *	odd;
	   BYTE	__far *	even;
	   
	   WORD GetBitmapUsage(BYTE __far * map, WORD width, WORD height, WORD visibleMask, 
	                       WORD __far & firstVisible, WORD __far & lastVisible);
		
		Error DecodeBitmap2(BYTE __far * map, DWORD __huge * bitmapBuffer, 
		                    WORD width, WORD height, BYTE __far * colorLUT,
		                    WORD skipTop, WORD skipLeft, WORD skipRight);
		Error DecodeBitmap2Button(BYTE __far * map, DWORD __huge * bitmapBuffer, 
		                          WORD width, WORD height, BYTE __far * colorLUT,
		                          WORD skipTop,
		                          WORD buttonLeft, WORD buttonTop, WORD buttonWidth, WORD buttonHeight);
		Error DecodeBitmap4Button(BYTE __far * map, DWORD __huge * bitmapBuffer, 
		                          WORD width, WORD height, BYTE __far * colorLUT,
		                          WORD skipTop,
		                          WORD buttonLeft, WORD buttonTop, WORD buttonWidth, WORD buttonHeight);

		DWORD				changed;
		
		BOOL				enabled;
		WORD				x, y, w, h;
		WORD				bx, by, bw, bh;
	   SPUButtonState	buttonState;

   	WORD colorMask, colorVisibleMask;
		WORD topVisible, bottomVisible;
	public:
		SPUonOSDDisplay(OSDDisplay * osd);
		~SPUonOSDDisplay(void);
		
		Error KillSPUBitmap(void);
		Error SetSPUBitmap(BYTE __far * odd, BYTE __far * even);
		
		Error SetSPUPosition(WORD x, WORD y, WORD w, WORD h);
		Error SetSPUColors(int back, int front, int emph1, int emph2);
		Error SetSPUContrast(int back, int front, int emph1, int emph2);
	 	Error SetSPUExtColors(int sBack, int sFront, int sEmph1, int sEmph2,
	                         int aBack, int aFront, int aEmph1, int aEmph2);
		Error SetSPUExtContrast(int sBack, int sFront, int sEmph1, int sEmph2,
	                           int aBack, int aFront, int aEmph1, int aEmph2);
		Error SetSPUPalette(int entry, int y, int u, int v);
	              
		Error SetButtonPosition(WORD x, WORD y, WORD w, WORD h);
		
		Error ShowSPU(BOOL enable);
		Error ShowButton(SPUButtonState state);

	   Error Update(void);
	};
	
class SPUEntry : public Node
	{
	friend class SPUDecoder;
	private:
		DWORD	startPTS;
		DWORD	nextPTS;
			   
		
		KernelMemory	bufferMem;
		BYTE __far *	buffer;
		WORD				bufferSize;		
		
		WORD				nextCommand;
		
		SPUEntry(WORD size);
	};

class GenericSPUDecoder : public MPEG2StreamReceiver
	{
	protected:
		MPEG2SPUCommandHookHandle	commandHook;
	public:
		GenericSPUDecoder(void) {commandHook = NULL;}
		virtual ~GenericSPUDecoder(void) {}

		virtual Error EnableSPU(BOOL enable) = 0;

		virtual Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);

		Error SetCommandHook(MPEG2SPUCommandHookHandle commandHook) {this->commandHook = commandHook; GNRAISE_OK;}
		virtual Error SetButtonState(SPUButtonState state) = 0;
		virtual Error SetButtonPosition(int x, int y, int w, int h) = 0;
		virtual Error SetButtonColors(DWORD selected, DWORD active) = 0;
		virtual Error SetPalette(int entry, int y, int u, int v) = 0;

		virtual Error Configure(TAG __far * tags);
	};

class SPUDecoder : public GenericSPUDecoder, public PTSCallback
	{
	private:
		List				pending, free;
		SPUDisplay	*	display;
		PTSCaller	*	ptsCaller;
		SPUEntry		*	current;
		SPUEntry		*	filled;
		BOOL				predPending;
				
		enum FillingState {FS_IDLE, FS_FIRST, FS_REST, FS_SKIP} fillingState;
		
		WORD				fillSize;
		BYTE	__far	*	fillPtr;
		
		DWORD				nextPTS;
		BOOL				enabled;
		BOOL				visible;
	   SPUButtonState	buttonState;

		MPEG2SPUCommandHookHandle	commandHook;
	public:
		SPUDecoder(SPUDisplay * display, PTSCaller * ptsCaller);
		virtual ~SPUDecoder(void);
		
		Error EnableSPU(BOOL enable);
	
	   DWORD SendData(HPTR data, DWORD size);
	   void CompleteData(void);
	   void FlushData(void);
	   DWORD LastTransferLocation(DWORD scale) {return 0;}
	
	   void PutPTS(DWORD pts);	
	   
	   Error PTSCallbackReached(DWORD currentPTS);
	   
	   virtual Error SetCommandHook(MPEG2SPUCommandHookHandle commandHook);
	   virtual Error SetButtonState(SPUButtonState state);
	   virtual Error SetButtonPosition(int x, int y, int w, int h);
	   virtual Error SetButtonColors(DWORD selected, DWORD active);
	   virtual Error SetPalette(int entry, int y, int u, int v);
	};

class SPUPESDecoder : public SPUDecoder
	{
	protected:
		BOOL	pesMode;
	public:
		SPUPESDecoder(SPUDisplay * display, PTSCaller * ptsCaller);
		
		Error SendPESData(HPTR data, DWORD size, DWORD __far & done, DWORD __far & used);
		MPEG2PESType NeedsPES(void) {return pesMode ? MP2PES_DVD : MP2PES_ELEMENTARY;}
				
		Error SetPESMode(BOOL pesMode);
	};

class EncryptedSPUPESDecoder : public SPUPESDecoder
	{
	protected:
		struct PESBuffer
			{
			DWORD				pts;
			BOOL				pending;
			BYTE				space[2048];
			} __far * buffer;
		KernelMemory		bufferMem;
		WORD					bufferSize;
		WORD					bufferFirst, bufferLast;
		DWORD 				pesPTS;
      WORD					bufferSemaphore;
      
		void ResetPESBuffer(void);
		
		virtual DWORD SendDecryptionPacket(HPTR data, DWORD size) {ReturnDecryptionPacket(data, size); return size;}
		void ReturnDecryptionPacket(HPTR data, DWORD size);		
	public:		
		EncryptedSPUPESDecoder(SPUDisplay * display, PTSCaller * ptsCaller);
	
		Error SendPESData(HPTR data, DWORD size, DWORD __far & done, DWORD __far & used);
	   void FlushData(void);
	
	   void PutPTS(DWORD pts);		
	};

#endif	
	
#endif
