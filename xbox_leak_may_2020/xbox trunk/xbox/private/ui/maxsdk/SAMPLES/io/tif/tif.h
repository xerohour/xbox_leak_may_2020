//-----------------------------------------------------------------------------
// --------------------
// File ....: tif.h
// --------------------
// Author...: Tom Hudson
// Date ....: Feb 1996
// Descr....: TIF File I/O Module
//
// History .: Feb. 20, 1996 - Started file
//            
//-----------------------------------------------------------------------------

// Uncomment the following when LZW compression is OK'd
//#define ALLOW_COMPRESSION

// Need to bracket the C-based library stuff with an explicit declaration:
#ifdef	__cplusplus
extern "C" {
#endif

#include "tif_port.h"
#include "tiffio.h"

// TIF library functions
extern TIFF * TIFFReadHdr( FILE *fd );

#ifdef	__cplusplus
	}
#endif

#ifdef DESIGN_VER
#include "gtiffio.h"
#endif

// Pack all the tif structs
#pragma pack(1)

#define TIFCLASSID 0xfc12

#define DLLEXPORT __declspec(dllexport)

#define TIFVERSION 102		// 101 = Original release
							// 102 = No-compression fix 7/5/96

#define TIFCONFIGNAME _T("tif.cfg")

#define BLOCKSIZE       (long)65536      // changed from 16384 DS 2/15/99

#define KPBYTE        (short)1   /* tag type defs */
#define KPASCII       (short)2
#define KPSHORT       (short)3
#define KPLONG        (short)4
#define KPRATIONAL    (short)5

/* LZW decode parameters */

#define BITS_MIN    9           /* start with 9 bits */
#define BITS_MAX    12          /* max of 12 bit strings */

/* packbits encode parameters */

#define     BASE            0
#define     LITERAL         1
#define     RUN             2
#define     LITERAL_RUN     3

/* predefined codes for LZW compression */

#define CODE_CLEAR  256         /* code to clear string table */
#define CODE_EOI    257         /* end-of-information code */
#define CODE_FIRST  258         /* first free code entry */
#define	CODE_MAX	MAXCODE(BITS_MAX)
#define HSIZE       11252       /* 80% occupancy */
/* #define HSIZE       9001  */      /* 80% occupancy */

struct tag        /* one element in a tiff header - type: LONG */
{
    short tagn;   /* tag id number */
    short type;   /* 1=byte, 2=ascii, 3=short, 4=long, 5=rational (2 longs) */
    long  n;      /* number of values to follow */
    long  val;    /* the value (if n=1) or an offset to table of values */
};

struct stag       /* one element in a tiff header - type: SHORT */
{
    short tagn;   /* tag id number */
    short type;   /* 1=byte, 2=ascii, 3=short, 4=long, 5=rational (2 longs) */
    long  n;      /* number of values to follow */
    short val;    /* the value (if n=1) or an offset to table of values */
    short junk;   /* junk value for filler */
};

struct tiffhead    /* structure for our TIFF Version 5.0 header */
{
    short order;                    /* byte order in file = II for Intel */
    short version;                  /* TIFF magic number is always 42   */
    long offset1;                   /* offset of 1st sub directory = 8 */
    short tags;                     /* number of entries in directory */
    struct tag NewSubfileType;      /* tag254 = 0 */
    struct tag ImageWidth;          /* tag256 = pixels/line */
    struct tag ImageLength;         /* tag257 = lines/image */
    struct stag BitsPerSample;      /* tag258 = 8 or offset to rbits */
    struct stag Compression;        /* tag259 = 1 for none, 5 for LZW */
    struct stag Photometric;        /* tag262 = 1 for grey, 2 for rgb */
    struct stag FillOrder;          /* tag266 = 1 (for Aldus PageMaker) */
    struct tag StripOffsets;        /* tag273 = offset of strip offsets */
    struct stag Orientation;        /* tag274 = 1 (for Aldus PageMaker) */
    struct stag SamplesPerPixel;    /* tag277 = 1 for grey, 3 for rgb */
    struct stag RowsPerStrip;       /* tag278 = 1 */
    struct tag StripByteCounts;     /* tag279 = offset of byte count table */
    struct tag XResolution;         /* tag282 = offset of xres1 */
    struct tag YResolution;         /* tag283 = offset of yres1 */
    struct stag PlanarConfiguration;/* tag284 = 1 for rgbrgbrgb... */
    struct stag ResolutionUnit;     /* tag296 = 1 for undefined */
    struct stag Predictor;          /* tag317 = 1 for none, 2 for LZW */
    long offsetn;                   /* offset of next subdirectory = 0 */
    long xres1,xres2,yres1,yres2;   /* x & y aspect */
    short rbits,gbits,bbits,abits;   /* bits per pixel of red, green, blue */
};

#define CHECK_GAP	10000		/* enc_ratio check interval */

static	uchar	 rmask[9] =
	{ 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };
static	uchar lmask[9] =
    { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };
#undef HSIZE
#define	HSIZE		5003		/* 80% occupancy */
#define	HSHIFT		(8-(16-12))

/* LZW decompression status structure */
#define	LZW_HORDIFF4	0x01		/* hor. diff w/ 4-bit samples */
#define	LZW_HORDIFF8	0x02		/* hor. diff w/ 8-bit samples */
#define	LZW_HORDIFF16	0x04		/* hor. diff w/ 16-bit samples */
#define	LZW_HORDIFF32	0x08		/* hor. diff w/ 32-bit samples */
#define	LZW_RESTART	0x01		/* restart interrupted decode */

struct encode {
	int checkpoint;		/* point at which to clear table */
	long	ratio;			/* current compression ratio */
	long incount;		/* (input) data bytes encoded */
	long outcount;		/* encoded (output) bytes */
	int htab[HSIZE];		/* hash table */
	short	codetab[HSIZE];		/* code table */
   };

typedef struct {
	int  lzw_oldcode;			/* last code encountered */
	BYTE lzw_hordiff;
	WORD lzw_flags;
	WORD lzw_nbits;				/* number of bits/code */
	WORD lzw_stride;		/* horizontal diferencing stride */
	int   lzw_maxcode;			/* maximum code for lzw_nbits */
	long	lzw_bitoff;			 	/* bit offset into data */
	long	lzw_bitsize;			/* size of strip in bits */
	int	lzw_free_ent;			/* next free entry in hash table */
	union {
		struct encode enc;
		} u;
	} LZWState;

#define	enc_checkpoint	u.enc.checkpoint
#define	enc_ratio	u.enc.ratio
#define	enc_incount	u.enc.incount
#define	enc_outcount	u.enc.outcount
#define	enc_htab	u.enc.htab
#define	enc_codetab	u.enc.codetab

// Stop packing
#pragma pack()

//-----------------------------------------------------------------------------
//-- TIF data Structure -------------------------------------------------------
//

#define WRITE_MONO 0
#define WRITE_COLOR 1

#ifdef DESIGN_VER
typedef struct _tifuserdata {
     DWORD  version;
	 BOOL	saved;
     BYTE   writeType;
	 BOOL	geoInfo;
	 Matrix3 matrix;
} TIFUSERDATA;
#else
typedef struct _tifuserdata {
     DWORD  version;
	 BOOL	saved;
     BYTE   writeType;
#ifdef ALLOW_COMPRESSION
     BYTE   compressed;
#endif //ALLOW_COMPRESSION
} TIFUSERDATA;
#endif

// Reasons for TIF load failure:

#define TIF_SAVE_OK					1
#define TIF_SAVE_WRITE_ERROR		0

//-----------------------------------------------------------------------------
//-- Class Definition ---------------------------------------------------------
//

class BitmapIO_TIF : public BitmapIO {
    
     private:
     
        Bitmap *loadMap;
		BitmapStorage *loadStorage;
		BitmapStorage *saveStorage;
        FILE   *inStream;
        FILE   *outStream;
		
		BOOL load_alpha;
		int nsamp;	// Number of samples per pixel
		int width, height;
		BYTE *loadbuf;
		TIFF *tif;
		TIFFDirectory *td;

        TIFUSERDATA   UserData;

        //-- This handler's private functions

        BitmapStorage *ReadTIFFile( BitmapInfo *fbi, BitmapManager *manager, BMMRES *status);
		BitmapStorage *LoadTIFStuff(BitmapInfo *fbi, BitmapManager *manager );
     	BitmapStorage *TifReadLineArt(BitmapInfo *fbi, BitmapManager *manager);
     	BitmapStorage *TifReadGrayScale(BitmapInfo *fbi, BitmapManager *manager);
     	BitmapStorage *TifReadPlanarRGB(BitmapInfo *fbi, BitmapManager *manager);
     	BitmapStorage *TifReadChunkyRGB(BitmapInfo *fbi, BitmapManager *manager);
		void ScrunchColorMap(BMM_Color_48 *colpal);
    	BitmapStorage *TifReadColPal(BitmapInfo *fbi, BitmapManager *manager);

		// Write stuff:
		unsigned short rps, spp;
		long rawcc;
		int blocks;
		LZWState lzw_state;
		BYTE *comp_buf, *shortstrip;
		BMM_Color_64 *scanline, *scanptr;
		BOOL write_alpha;
		void MakeTiffhead();
 		int SaveTIF(FILE *stream);
		int WriteTIF(FILE *stream);
		int LZWPreEncode(void);
		int LZWEncode(uchar *bp, int cc );
		int LZWPostEncode(void);
		void PutNextCode(int c);
		void ClearBlock(void);
		void ClearHash();

        void           GetCfgFilename     ( TCHAR *filename );
        BOOL           ReadCfg            ( );
        void           WriteCfg           ( );

     public:
     
        //-- Constructors/Destructors
        
                       BitmapIO_TIF       ( );
                      ~BitmapIO_TIF       ( ) {}

        //-- Number of extemsions supported
        
		#ifndef DESIGN_VER
        int            ExtCount           ( )       { return 1;}
		#else
		int            ExtCount           ( )       { return 2;}
		#endif // DESIGN_VER
        
        //-- Extension #n (i.e. "3DS")
        
		#ifndef DESIGN_VER
        const TCHAR   *Ext                ( int n ) { return _T("tif"); }
		#else
		const TCHAR   *Ext                ( int n ) { return (n==1) ? _T("tiff") : _T("tif"); }
		#endif // DESIGN_VER
        
        //-- Descriptions
        
        const TCHAR   *LongDesc           ( );
        const TCHAR   *ShortDesc          ( );

        //-- Miscelaneous Messages
        
        const TCHAR   *AuthorName         ( )       { return _T("Tom Hudson");}
        const TCHAR   *CopyrightMessage   ( )       { return _T("Copyright 1996 Yost Group");}
        UINT           Version            ( )       { return (100);}

        //-- Driver capabilities
        
        int            Capability         ( )       { return BMMIO_READER    | 
                                                             BMMIO_WRITER    | 
                                                             BMMIO_EXTENSION |
															 BMMIO_CONTROLWRITE; }
        
        //-- Driver Configuration
        
        BOOL           LoadConfigure      ( void *ptr );
        BOOL           SaveConfigure      ( void *ptr );
        DWORD          EvaluateConfigure  ( );
        
        //-- Show DLL's "About..." box
        
        void           ShowAbout          ( HWND hWnd );  

        //-- Show DLL's Control Panel
        
        BOOL           ShowControl        ( HWND hWnd, DWORD flag );

        //-- Return info about image
        
        BMMRES         GetImageInfo       ( BitmapInfo *fbi );        

        //-- Image Input
        
        BitmapStorage *Load               ( BitmapInfo *fbi, Bitmap *map, BMMRES *status);

        //-- Image Output
        
        BMMRES         OpenOutput         ( BitmapInfo *fbi, Bitmap *map );
        BMMRES         Write              ( int frame );
        int            Close              ( int flag );
  
        //-- This handler's specialized functions
        
        BOOL           Control            ( HWND ,UINT ,WPARAM ,LPARAM );
        
};
