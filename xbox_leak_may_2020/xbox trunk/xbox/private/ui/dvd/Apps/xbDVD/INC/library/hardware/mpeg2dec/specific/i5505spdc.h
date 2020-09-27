#ifndef I5505SPDC_H
#define I5505SPDC_H

#include "library\common\prelude.h"
#include "library\lowlevel\hardwrio.h"
#include "library\general\asncstrm.h"
#include "..\generic\mp2spudc.h"

#define NUM_5505SPU_CONTROL_BUFFER	16	// min 14 !

class STi5505SPUDecoder : public GenericSPUDecoder, public PTSCallback
	{
	protected:
		ByteIndexedInOutPort		*	port;
		ByteIndexedInOutPort		*	mpegPort;
		ASyncOutStream				*	strm;
		PTSCaller * ptsCaller;

		int downScaleFactor;

		enum SPUControlState {SCS_IDLE, SCS_FILLFIRST, SCS_FILLING, SCS_READY, SCS_PENDING, SCS_EXECUTING};

		struct SPUControlBuffer
			{
			DWORD		startPTS;
			DWORD		bufferStart, bufferEnd;
			SPUControlState	state;
			} controlBuffer[NUM_5505SPU_CONTROL_BUFFER];

		DWORD nextPTS;
		DWORD bufferWritePosition, bufferReadPosition, bufferBase, bufferSize, bufferEndPosition;
		BYTE upperByte;

		SPUButtonState buttonState;
		int buttonX, buttonY, buttonW, buttonH;
		DWORD selectedColor, activeColor;

		struct PalEntry
			{
			BYTE y, u, v;
			} palette[16];

		int	idleControlBuffer, fillingControlBuffer, pendingControlBuffer, executingControlBuffer;

		BOOL	delayedSwitchOff;
		BOOL	isPaused;
		BOOL	isEnabled;

	public:
		STi5505SPUDecoder (ByteIndexedInOutPort *port, ByteIndexedInOutPort *mpegPort,
		                   ASyncOutStream *strm, PTSCaller *ptsCaller,
		                   DWORD bufferBase,   // byte offset inside the SDRAM
		                   DWORD bufferSize);
		virtual ~STi5505SPUDecoder(void);

		virtual Error EnableSPU(BOOL enable);

		virtual Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);

		virtual DWORD SendData(HPTR data, DWORD size);
		virtual void CompleteData(void);
		virtual void FlushData(void);
		virtual DWORD LastTransferLocation(DWORD scale) {return 0;}

		virtual void PutPTS(DWORD pts);

		// Functions from PTSCallback.
		virtual Error PTSCallbackReached (DWORD currentPTS);
		virtual Error SequenceStartReached (void);

		virtual Error SetButtonState(SPUButtonState state);
		virtual Error SetButtonPosition(int x, int y, int w, int h);
		virtual Error SetButtonColors(DWORD selected, DWORD active);
		virtual Error SetPalette(int entry, int y, int u, int v);

		virtual Error Configure (TAG *tags);
	};

#endif
