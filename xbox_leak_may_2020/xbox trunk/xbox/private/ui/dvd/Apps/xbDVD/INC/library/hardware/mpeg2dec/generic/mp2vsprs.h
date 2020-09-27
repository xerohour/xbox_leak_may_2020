#ifndef MP2VSPRS_H
#define MP2VSPRS_H

//
// MPEG 2 Video stream decoder
//

#include "..\..\..\common\prelude.h"
#include "..\..\..\common\gnerrors.h"

#define GNR_INVALID_VIDEO_START_CODE	MKERR(ERROR, MP2PARSER, FORMAT, 0x01) 

//
// potential MPEG1/2 video stream start codes
//
enum MPEG2VideoStartCode
	{
	MP2VH_PICTURE_START,
	MP2VH_USER_DATA, 
	MP2VH_SEQUENCE_HEADER, 
	MP2VH_SEQUENCE_ERROR,
	MP2VH_EXTENSION_START,
	MP2VH_SEQUENCE_END,
	MP2VH_GROUP_START,
	MP2VH_SLICE_HEADER,
	MP2VH_STUFFING,
	MP2VH_INVALID
	};


//
// potential MPEG1/2 frame/field types
//
enum MPEG2FrameType
	{
	M2FT_IFRAME, 
	M2FT_PFRAME, 
	M2FT_BFRAME,
	M2FT_UFRAME
	};         

enum MPEG2PictureStructure
	{
	M2PS_FRAME,
	M2PS_TOPFIELD,
	M2PS_BOTTOMFIELD,
	M2PS_UNKNOWNFIELD
	};

//
// potential mpeg2 chroma formats, typically only 422 used
//
enum MPEG2ChromaFormat
	{ 
	MP2CF_RESERVED,
	MP2CF_420,
	MP2CF_422,
	MP2CF_444
	};

//
// specified video output formats
//
enum MPEG2VideoFormat
	{ 
	MP2VF_COMPONENT,
	MP2VF_PAL,
	MP2VF_NTSC,
	MP2VF_SECAM,
	MP2VF_MAC,
	MP2VF_UNKNOWN
	};                
	
enum MPEG2CodingStandard
	{ 
	MP2CS_MPEG1,
	MP2CS_MPEG2
	};
	
//
// flags in changed
//
// Sequence header, sequence extension
//
#define MP2VPCHANGED_STANDARD				MKFLAG(0)
#define MP2VPCHANGED_SIZE					MKFLAG(1)
#define MP2VPCHANGED_FRAME_RATE			MKFLAG(2)
#define MP2VPCHANGED_INTRA_QUANT			MKFLAG(3)
#define MP2VPCHANGED_NON_INTRA_QUANT	MKFLAG(4)
#define MP2VPCHANGED_BIT_RATE				MKFLAG(5)
#define MP2VPCHANGED_VBV_BUFFER_SIZE	MKFLAG(6)

//
// Sequence extension
// 
#define MP2VPCHANGED_PROFILE				MKFLAG(8)
#define MP2VPCHANGED_DISPLAY_SIZE		MKFLAG(9)

//
// Picture header
//
#define MP2VPCHANGED_	MKFLAG()
#define MP2VPCHANGED_	MKFLAG()
#define MP2VPCHANGED_	MKFLAG()
#define MP2VPCHANGED_	MKFLAG()
#define MP2VPCHANGED_	MKFLAG()
#define MP2VPCHANGED_	MKFLAG()
#define MP2VPCHANGED_	MKFLAG()
#define MP2VPCHANGED_	MKFLAG()

//
// Flags determining which header types to parse
//
#define MP2VX_PICTURE_HEADER								MKFLAG(0)
#define MP2VX_USER_DATA										MKFLAG(1)
#define MP2VX_SEQUENCE_HEADER								MKFLAG(2)
#define MP2VX_EXTENSION_HEADER							MKFLAG(3)
#define MP2VX_GROUP_HEADER									MKFLAG(4)
#define MP2VX_SEQUENCE_ERROR								MKFLAG(5)
#define MP2VX_SEQUENCE_END									MKFLAG(6)

#define MP2VX_SEQUENCE_EXTENSION							MKFLAG(8)
#define MP2VX_SEQUENCE_DISPLAY_EXTENSION				MKFLAG(9)	
#define MP2VX_SEQUENCE_SCALABLE_EXTENSION				MKFLAG(10)
#define MP2VX_PICTURE_CODING_EXTENSION					MKFLAG(11)
#define MP2VX_QUANT_MATRIX_EXTENSION					MKFLAG(12)
#define MP2VX_PICTURE_DISPLAY_EXTENSION				MKFLAG(13)
#define MP2VX_PICTURE_TEMPORAL_SCALABLE_EXTENSION  MKFLAG(14)
#define MP2VX_PICTURE_SPATIAL_SCALABLE_EXTENSION	MKFLAG(15)


//
// Parser for MPEG1 and 2 video streams.
//
// The Function NextByte is a template function and has to be
// replaced with the actual function, that retrieves the next
// character from the input stream.
// 
class MPEG2VideoHeaderParser
	{
	protected:                      
		BYTE	b;
		WORD	w;
		DWORD	dw;             
		DWORD bits;
		BOOL	bit;

		BOOL	extensionHeaderPending;

		//
		// Template stream parsing functions, Begin/EndParse() may be
		// overridden, to add additional logic to the parsing.
		//
		// NextByte() is supposed to place the next byte of the
		// stream in the instance variable b. All higher level
		// parsing functions are broken into NextByte() requests.
		//
		virtual Error BeginParse(void) {GNRAISE_OK;}
		virtual Error EndParse(void) {GNRAISE_OK;}
		virtual Error NextByte(void) = 0;      
      
      //
      // Higher level parsing functions, used to simplify the
      // parser.
      //
		Error NextWord(void);
		Error NextDWord(void);
		Error NextStartCode(void);
				
		int bitsRemaining;
		
		Error NextBits(int num);
		Error NextBit();
		
		//
		// Determine the current startcode
		//
		MPEG2VideoStartCode IdentifyStartCode(BYTE code);
		
		//
		// Parsing functions for different video headers.
		// They may be overridden, to add additional parsing.
		//
		virtual Error ParsePictureHeader(void);
		virtual Error ParseUserData(void);
		virtual Error ParseSequenceHeader(void);
		virtual Error ParseExtensionHeader(void);
		
		//
		// Parsing functions for MPEG2 extended headers
		//
		virtual Error ParseSequenceExtension(void);
		virtual Error ParseSequenceDisplayExtension(void);
		virtual Error ParseSequenceScalableExtension(void);
		virtual Error ParsePictureCodingExtension(void);
		virtual Error ParseQuantMatrixExtension(void);
		virtual Error ParsePictureDisplayExtension(void);
		virtual Error ParsePictureTemporalScalableExtension(void);
		virtual Error ParsePictureSpatialScalableExtension(void);
		
		virtual Error ParseGroupHeader(void);  
		
		virtual Error ParseSequenceEnd(void);
		virtual Error ParseSequenceError(void);
		
		//
		// Dispatcher for the different header parser.
		//
		virtual Error ParseHeader(MPEG2VideoStartCode header);
	public:                                                     
		//
		// Variable determining which MPEG 2 headers to parse,
		// and which simply to skip.  This field is composed
		// out of MP2VX_ flags.
		//
		DWORD						parseHeaderFlags;
		
		//
		// Flag set, reflecting which parts of the parser information
		// changed during the last parser run.
		//
		DWORD						changed;

		//
		// Extracted fields of the MPEG 2 video stream headers,
		// see the MPEG documentation.
		//
		MPEG2CodingStandard		codingStandard;		
		WORD							width, height;            
		WORD							aspectRatioFactor;
		BYTE							aspectRatio;
		BYTE							frameRate;
		BYTE							frameRateNominator;
		BYTE							frameRateDenominator;
		DWORD							bitRate;       
		DWORD 						vbvBufferSize;                
		BYTE							intraQuantMatrix[64];
		BYTE							nonIntraQuantMatrix[64];           
		BYTE							chromaIntraQuantMatrix[64];
		BYTE							chromaNonIntraQuantMatrix[64];           
		                 	
		BYTE							profileAndLevel;  
		BOOL							progressiveSequence;
		MPEG2ChromaFormat			chromaFormat;
		
	   DWORD							timeCode;
	   BOOL							closedGop;
	   BOOL							brokenLink; 

		DWORD							totalFrameCnt;
		WORD							groupFrameCnt;
	   
		MPEG2FrameType				frameType;   
		MPEG2PictureStructure	fieldType, initialFieldType, secondaryFieldType;

		BYTE							pictureCoding;
		BYTE							backwardVector, forwardVector;
		WORD							temporalReference;		
		DWORD							totalTemporalReference;
		
		int							centerVerticalOffset;
		int							centerHorizontalOffset;
		
		BYTE							colourPrimaries;
		BYTE							transferCharacteristics;
		BYTE							matrixCoefficients;
		
		WORD							displayWidth;
		WORD							displayHeight;
		BYTE							fcode[2][2];
		BYTE							intraDCPrecision;
		BYTE							pictureStructure;
		BOOL							topFieldFirst;
		BOOL							framePredFrameDCT;
		BOOL							concealmentMotionVectors;
		BOOL							qScaleType;
		BOOL							intraVLCFormat;
		BOOL							alternateScan;
		BOOL							repeatFirstField;
		BOOL							chroma420Type;
		BOOL							progressiveFrame;
		BOOL							compositeDisplayFlag;
		BOOL							vAxis;
		BYTE							fieldSequence;
		BOOL							subCarrier;
		BYTE							burstAmplitude;
		BYTE							subCarrierPhase;
		                 	
		MPEG2VideoFormat			videoFormat;
		
		MPEG2VideoHeaderParser(void);
		virtual ~MPEG2VideoHeaderParser(void);
		
		virtual void Reset(void);
		
		//
		// Parser entry point, call this to parse the _next_ header
		//
		virtual Error Parse(MPEG2VideoStartCode __far &header);
	};

#endif
