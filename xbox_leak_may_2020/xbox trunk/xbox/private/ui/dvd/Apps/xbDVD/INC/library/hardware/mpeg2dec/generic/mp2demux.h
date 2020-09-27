
#ifndef MP2DEMUX_H
#define MP2DEMUX_H

#include "..\..\..\common\prelude.h"
#include "..\..\..\general\asncstrm.h"
#include "mp2eldec.h"

class MPEG2StreamMuxAssoc
	{
	private:
		struct Assoc {DWORD mux, elem;} __far * assoc;
		int first;
		int last; 
		int num;
		DWORD lastMux, lastElem;
	public:
		MPEG2StreamMuxAssoc(int num);
		~MPEG2StreamMuxAssoc(void);

		void PutPosition(DWORD mux, DWORD elem);
		DWORD GetMuxPosition(DWORD elem);
		DWORD GetElemPosition(DWORD mux);
		void Reset(void);
	};
	
class MPEG2StreamDemux //: public ASyncOutStream
	{        
	protected:                             
		MPEG2StreamReceiver	*	videoDecoder;
		MPEG2StreamReceiver	*	audioDecoder;
		MPEG2StreamReceiver	*	ac3Decoder;
#if VIONA_VERSION
		MPEG2StreamReceiver	*	lpcmDecoder;
		MPEG2StreamReceiver	*	subPictureDecoder;
		MPEG2StreamReceiver	*	dtsDecoder;
#endif		
		
		BYTE	videoStreamID, audioStreamID, ac3StreamID;
#if VIONA_VERSION
		BYTE	lpcmStreamID, subPictureStreamID, dtsStreamID;
#endif		
		
		int	state;	                      
		
		DWORD	cnt;
		DWORD	length; 
		BYTE 	high, low;    
		BYTE	headerLength;
		WORD	headerRest;
		BYTE	streamID;      
		BYTE	flags, xflags;    
		BYTE	ptss[5];         
		DWORD	pts;
		BOOL	ptsValid;
		DWORD	dts;
		BOOL	dtsValid;
      
      BOOL	mpeg2;

		DWORD ptsOffset, endPTM;
      
		DWORD audioPos;
		DWORD ac3Pos;
#if VIONA_VERSION
		DWORD lpcmPos;
		DWORD	dtsPos;
#endif		
		DWORD muxPos;
	public:
		DWORD videoPos;
		BOOL	dvdDemux, scanMode;
		
		DWORD	ScalePositionToDemux(DWORD muxPos, DWORD scale) 
			{return dvdDemux ? XScaleDWord(muxPos, scale, MP2SR_SCALE_DVDPES) : muxPos;}
		DWORD	ScalePositionFromDemux(DWORD elemPos, DWORD scale) 
			{return dvdDemux ? XScaleDWord(elemPos, MP2SR_SCALE_DVDPES, scale) : elemPos;}

		MPEG2StreamMuxAssoc				*	videoPosAssoc;
		MPEG2StreamMuxAssoc				*	audioPosAssoc;
		MPEG2StreamMuxAssoc				*	ac3PosAssoc;
#if VIONA_VERSION
		MPEG2StreamMuxAssoc				*	lpcmPosAssoc;
		MPEG2StreamMuxAssoc				*	dtsPosAssoc;

		MPEG2StreamDemux(MPEG2StreamReceiver * videoDecoder,	
		                 MPEG2StreamReceiver * audioDecoder,
		                 MPEG2StreamReceiver * ac3Decoder = NULL,
		                 MPEG2StreamReceiver * lpcmDecoder = NULL,
		                 MPEG2StreamReceiver * subPictureDecoder = NULL,
							  MPEG2StreamReceiver * dtsDecoder = NULL);
		~MPEG2StreamDemux(void);
				                 
	   Error SetStreamIDs(BYTE videoStreamID, 
	                      BYTE audioStreamID, 
	                      BYTE ac3StreamID = 0xff, 
	                      BYTE lpcmStreamID = 0xff,
	                      BYTE subPictureStreamID = 0xff,
								 BYTE dtsStreamID = 0xff)
	   	{
	   	if (videoStreamID != 0xff)      this->videoStreamID      = videoStreamID;
	   	if (audioStreamID != 0xff)      this->audioStreamID      = audioStreamID;
	   	if (ac3StreamID != 0xff)        this->ac3StreamID        = ac3StreamID;
	   	if (lpcmStreamID != 0xff)		  this->lpcmStreamID			= lpcmStreamID;
      	if (subPictureStreamID != 0xff) this->subPictureStreamID = subPictureStreamID;
			if (dtsStreamID != 0xff)        this->dtsStreamID        = dtsStreamID;
	   	
	   	GNRAISE_OK;
	   	}
#else
		MPEG2StreamDemux(MPEG2StreamReceiver * videoDecoder,	
		                 MPEG2StreamReceiver * audioDecoder,
		                 MPEG2StreamReceiver * ac3Decoder = NULL);
	   Error SetStreamIDs(BYTE videoStreamID, 
	                      BYTE audioStreamID, 
	                      BYTE ac3StreamID = 0xff)
	   	{
	   	if (videoStreamID != 0xff)      this->videoStreamID      = videoStreamID;
	   	if (audioStreamID != 0xff)      this->audioStreamID      = audioStreamID;
	   	if (ac3StreamID != 0xff)        this->ac3StreamID        = ac3StreamID;
	   	
	   	GNRAISE_OK;
	   	}
#endif		
	                
		void Reset(void);
		void ResetAudio(void);
		
		DWORD SendData(HPTR data, DWORD size);
		void CompleteData(void);              
	};
	

#endif
