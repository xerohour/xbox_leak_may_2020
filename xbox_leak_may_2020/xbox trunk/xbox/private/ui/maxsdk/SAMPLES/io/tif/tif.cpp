//-----------------------------------------------------------------------------
// --------------------
// File ....: tif.cpp
// --------------------
// Author...: Tom Hudson
// Date ....: Feb. 20, 1996
// Descr....: TIF File I/O Module
//
// History .: Feb. 20 1996 - Started
//

//-- Includes -----------------------------------------------------------------

#include <Max.h>
#include <bmmlib.h>
#include "pixelbuf.h"
#include "tif.h"
#include "tifrc.h"

// Some handy macros:

#define TIFWRITE(ptr,sz) ((fwrite((void *)ptr, sz, 1, stream)!=1) ? 0:1)

//#include <stdarg.h>

//-----------------------------------------------------------------------------
//-- File Class

class File {
     public:
        FILE *stream;
        File(const TCHAR *name, const TCHAR *mode) { stream = _tfopen(name,mode); }
        ~File() { Close(); }
        void Close() { if(stream) fclose(stream); stream = NULL; }
     };

//-- Globals ------------------------------------------------------------------

HINSTANCE hInst = NULL;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-- DLL Declaration

BOOL WINAPI DllMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved) {
     switch (fdwReason) {
         case DLL_PROCESS_ATTACH:
              if (hInst)
                 return(FALSE);
              hInst = hDLLInst;
              break;
         case DLL_PROCESS_DETACH:
              hInst  = NULL;
              break;
         case DLL_THREAD_ATTACH:
              break;
         case DLL_THREAD_DETACH:
              break;
     }
     return TRUE;
}

TCHAR *GetString(int id)
	{
	static TCHAR buf[256];

	if (hInst)
		return LoadString(hInst, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
	}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// TIF Class Description

class TIFClassDesc:public ClassDesc {
     public:
        int           IsPublic     ( ) { return 1; }
        void         *Create       ( BOOL loading=FALSE) { return new BitmapIO_TIF; }
        const TCHAR  *ClassName    ( ) { return GetString(IDS_TIF); }
        SClass_ID     SuperClassID ( ) { return BMM_IO_CLASS_ID; }
        Class_ID      ClassID      ( ) { return Class_ID(TIFCLASSID,0); }
        const TCHAR  *Category     ( ) { return GetString(IDS_BITMAP_IO);  }
};

static TIFClassDesc TIFDesc;

//-----------------------------------------------------------------------------
// Interface

DLLEXPORT const TCHAR * LibDescription ( )  { 
     return GetString(IDS_TIF_DESC); 
}

DLLEXPORT int LibNumberClasses ( ) { 
     return 1; 
}

DLLEXPORT ClassDesc *LibClassDesc(int i) {
     switch(i) {
        case  0: return &TIFDesc; break;
        default: return 0;        break;
     }
}

__declspec( dllexport ) ULONG LibVersion ( )  { 
     return ( VERSION_3DSMAX ); 
}

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}

BitmapIO_TIF::BitmapIO_TIF() {
	loadStorage = saveStorage = NULL;
	inStream = NULL;
	// Init user data
    UserData.version = TIFVERSION;
	UserData.saved = FALSE;
	UserData.writeType = WRITE_COLOR;
#ifdef ALLOW_COMPRESSION
	UserData.compressed = TRUE;
#endif //ALLOW_COMPRESSION
	}

//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::LongDesc()

const TCHAR *BitmapIO_TIF::LongDesc()  {
     return GetString(IDS_TIF_FILE);
}
     
//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::ShortDesc()

const TCHAR *BitmapIO_TIF::ShortDesc() {
     return GetString(IDS_TIF);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::LoadConfigure()

BOOL BitmapIO_TIF::LoadConfigure ( void *ptr ) {
     TIFUSERDATA *buf = (TIFUSERDATA *)ptr;
     if (buf->version == TIFVERSION) {
        memcpy((void *)&UserData,ptr,sizeof(TIFUSERDATA));
        return (TRUE);
     } else
        return (FALSE);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::SaveConfigure()

BOOL BitmapIO_TIF::SaveConfigure ( void *ptr ) {
     if (ptr) {
 		UserData.saved = TRUE;
        memcpy(ptr,(void *)&UserData,sizeof(TIFUSERDATA));
        return (TRUE);
     } else
        return (FALSE);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::EvaluateConfigure()

DWORD BitmapIO_TIF::EvaluateConfigure ( ) {
      return (sizeof(TIFUSERDATA));
}

//-----------------------------------------------------------------------------
// *> AboutCtrlDlgProc()
//

BOOL CALLBACK AboutCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {

     switch (message) {
        
        case WM_INITDIALOG: 
             CenterWindow(hWnd,GetParent(hWnd));
             return 1;
        

        case WM_COMMAND:
             switch (LOWORD(wParam)) {
                case IDOK:              
                case IDCANCEL:
                     EndDialog(hWnd,1);
                     break;
             }
             return 1;

     }
     
     return 0;

}

//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::ShowAbout()

void BitmapIO_TIF::ShowAbout(HWND hWnd) {
     DialogBoxParam(
        hInst,
        MAKEINTRESOURCE(IDD_ABOUT),
        hWnd,
        (DLGPROC)AboutCtrlDlgProc,
        (LPARAM)0);
}


//-----------------------------------------------------------------------------
// *> ControlCtrlDlgProc()
//

BOOL BitmapIO_TIF::Control(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {

     static BOOL forced = FALSE;

     switch (message) {
        
        case WM_INITDIALOG: {
      
             CenterWindow(hWnd,GetParent(hWnd));
			 SetCursor(LoadCursor(NULL,IDC_ARROW));

             InitCommonControls();
             
             if (!UserData.saved)
                ReadCfg();

			 CheckDlgButton(hWnd,IDC_MONOCHROME,UserData.writeType == WRITE_MONO ? TRUE : FALSE); 
			 CheckDlgButton(hWnd,IDC_COLOR,UserData.writeType == WRITE_COLOR ? TRUE : FALSE); 
#ifdef ALLOW_COMPRESSION
			 CheckDlgButton(hWnd,IDC_COMPRESS,UserData.compressed); 
#endif //ALLOW_COMPRESSION
             return 1;
             
        }

        case WM_COMMAND:

             switch (LOWORD(wParam)) {
                
                case IDOK: {
                     if(IsDlgButtonChecked(hWnd,IDC_MONOCHROME))
					 	UserData.writeType = WRITE_MONO;
					 else
                     if(IsDlgButtonChecked(hWnd,IDC_COLOR))
					 	UserData.writeType = WRITE_COLOR;

#ifdef ALLOW_COMPRESSION
                     UserData.compressed = IsDlgButtonChecked(hWnd,IDC_COMPRESS) ? TRUE : FALSE;
#endif //ALLOW_COMPRESSION

  					WriteCfg();
                    EndDialog(hWnd,1);
					 }
                     break;

                case IDCANCEL:
                     EndDialog(hWnd,0);
                     break;
        
             }
             return 1;

     }
     
     return 0;

}

//-----------------------------------------------------------------------------
// *> ControlDlgProc
//

static BOOL CALLBACK ControlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
     static BitmapIO_TIF *bm = NULL;
     if (message == WM_INITDIALOG) 
        bm = (BitmapIO_TIF *)lParam;
     if (bm) 
        return (bm->Control(hWnd,message,wParam,lParam));
     else
        return(FALSE);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::Control()

BOOL BitmapIO_TIF::ShowControl(HWND hWnd, DWORD flag ) {
     return (
        DialogBoxParam(
        hInst,
        MAKEINTRESOURCE(IDD_TIF_CONTROL),
        hWnd,
        (DLGPROC)ControlDlgProc,
        (LPARAM)this)
     );
}


//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::GetImageInfo()

BMMRES BitmapIO_TIF::GetImageInfo ( BitmapInfo *fbi ) {
     
     //-- Get File Header
     
     File file(fbi->Name(), _T("rb"));

     if(!(inStream = file.stream))
		return (ProcessImageIOError(fbi));

     tif=TIFFReadHdr(inStream);
     if(tif==NULL) {
		DebugPrint("TIFFReadHdr failed\n");
		return (ProcessImageIOError(fbi,BMMRES_BADFILEHEADER));
		}
     td = &tif->tif_dir;

     /* Fill in appropriate info slots */

     nsamp = td->td_samplesperpixel;
     fbi->SetWidth(td->td_imagewidth);
     fbi->SetHeight(td->td_imagelength);
     fbi->SetAspect(1.0f);
     //fbi->SetGamma(1.0f);
     fbi->SetFirstFrame(0);
     fbi->SetLastFrame(0);
	 int type = BMM_NO_TYPE;

     switch (td->td_photometric) {
     	case 0:  case 1:
			if(td->td_bitspersample == 1)
	     		type = BMM_LINE_ART;
			else
     			type = BMM_GRAY_8;
     		break;
     	case 2:
     		type = (td->td_samplesperpixel==4) ? BMM_TRUE_32 : BMM_TRUE_24;
     		break;
     	case 3:
			type = BMM_PALETTED;
     		break;
     	}

     if(type == BMM_NO_TYPE) {
		DebugPrint("Unknown TIF type\n");
		return (ProcessImageIOError(fbi,GetString(IDS_UNKNOWN)));
		}

	 fbi->SetType(type);

     return BMMRES_SUCCESS;
}

//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::Load()

BitmapStorage *BitmapIO_TIF::Load(BitmapInfo *fbi, Bitmap *map, BMMRES *status) {

	//-- Initialize Status Optimistically

	*status = BMMRES_SUCCESS;

	//-- Make sure nothing weird is going on

	if(openMode != BMM_NOT_OPEN) {
		*status = ProcessImageIOError(fbi,BMMRES_INTERNALERROR);
		return NULL;
	}

     openMode = BMM_OPEN_R;
     loadMap  = map;
     
     BitmapStorage *s = ReadTIFFile(fbi, map->Manager(), status);

     if(!s)
        return NULL;

     //-- Set the storage's BitmapInfo

     s->bi.CopyImageInfo(fbi);

     return s;
}

//-----------------------------------------------------------------------------
// *> BitmapIO_TIF::ReadTIFFile()
//
//    Load a TIF file, returning the storage location

BitmapStorage *BitmapIO_TIF::ReadTIFFile(BitmapInfo *fbi, BitmapManager *manager, BMMRES *status) {

     BitmapStorage *s = NULL;

     File file(fbi->Name(), _T("rb"));

     if(!(inStream = file.stream)) {
		*status = ProcessImageIOError(fbi);
        return NULL;
	}

	int res=1;

     tif = TIFFReadHdr(inStream);
     if (tif==NULL) {
		*status = ProcessImageIOError(fbi,BMMRES_BADFILEHEADER);
		return(0);
	}

     td = &tif->tif_dir;
	
#ifdef DESIGN_VER
	 GTIFF * gtif = GeoTIFFRead(td);
	 if (gtif)
	 {
		GeoTableItem * data = new GeoTableItem;
		
		data->m_geoInfo = true;
//		GeoTIFFCoordSysName(gtif, UserData.name);
		data->m_matrix = GeoTIFFModelTransform(gtif);
		data->m_names.AddName(const_cast<TCHAR *>(fbi->Name()));
		GeoTIFFExtents(data, gtif);
		TheManager->Execute(0, (ULONG)(fbi->Name()), (ULONG)(data), 0);
	 }
#endif

     nsamp = td->td_samplesperpixel;
     if (nsamp>4)
		nsamp = 4;	
     width = td->td_imagewidth;
	 height = td->td_imagelength;
	 fbi->SetWidth(width);
     fbi->SetHeight(height);
     fbi->SetAspect(1.0f);
     //fbi->SetGamma(1.0f);
     fbi->SetFirstFrame(0);
     fbi->SetLastFrame(0);

     load_alpha = (td->td_samplesperpixel==4) ? TRUE : FALSE;

     /* allocate decompression buffer memory */
     PixelBuf8 lbuf(tif->tif_scanlinesize);
     loadbuf = lbuf.Ptr();

     switch (td->td_photometric) {
     	case 0:
		case 1:
            if(td->td_bitspersample == 1) {
	     		fbi->SetType(BMM_LINE_ART); 	
		 		s = TifReadLineArt(fbi, manager); 
				}
			else {
     			fbi->SetType(BMM_GRAY_8); 	
     			s = TifReadGrayScale(fbi, manager); 
				}
     		break;
     	case 2:
     		fbi->SetType(BMM_TRUE_32);
     		if (td->td_planarconfig==PLANARCONFIG_SEPARATE)
     			s = TifReadPlanarRGB(fbi, manager);
     		else 
     			s = TifReadChunkyRGB(fbi, manager);
     		break;
     	case 3:
			fbi->SetType(BMM_PALETTED);
     		s = TifReadColPal(fbi, manager); 
     		break;
     	}

#ifdef DESIGN_VER
	 GeoTIFFClose(gtif);
#endif
     TIFFClose(tif);
     if(s)
        return s;

     //-- If we get here, something went wrong!

	*status = ProcessImageIOError(fbi,GetString(IDS_READ_ERROR));
     return NULL;

}

// Our TIF loader functions:

BitmapStorage *BitmapIO_TIF::TifReadLineArt(BitmapInfo *fbi, BitmapManager *manager) {
 
     //-- Create a storage for this bitmap ------------------------------------

     BitmapStorage *s = BMMCreateStorage(manager,BMM_LINE_ART);

     if(!s)
        return NULL;

     if (s->Allocate(fbi,manager,BMM_OPEN_R)==0) {
        bail_out:
        if(s) {
           delete s;
           s = NULL;
        }
        return NULL;
     }

	uchar byte;
	int nbyte,bytesperrow,x,y,photo,bps,nmax;
	
	bytesperrow = tif->tif_scanlinesize;
	photo = td->td_photometric;
	bps = td->td_bitspersample;
	if (bps!=1) {
		DebugPrint("Bad bits per sample\n");
		goto bail_out;
		}

	nmax = (1<<bps)-1;

    PixelBuf line64(width);

	for (y = 0; y<height; y++)  {
		if (TIFFReadScanline(tif, loadbuf, y, 0)<0) 
			goto bail_out;
        BMM_Color_64 *l64=line64.Ptr();
		x = 0;
		for (nbyte=0; nbyte<bytesperrow; nbyte++) {
			unsigned char mask;
			unsigned int c;
			int nsh;
			byte = loadbuf[nbyte];
			mask = nmax<<(8-bps);
			for (nsh=8-bps; nsh>=0; nsh-=bps, mask>>=bps, ++l64) {
				c = (byte&mask)>>nsh;  
				c = (c*255)/nmax;
				if (photo==0) c = 255-c;
				l64->r = l64->g = l64->b = c ? 0xffff : 0;
				l64->a = 0;
				if (++x == width) break;
				}
			}
        if(s->PutPixels(0,y,width,line64.Ptr())!=1)
            goto bail_out;
		}
	return s;
	}

BitmapStorage *BitmapIO_TIF::TifReadGrayScale(BitmapInfo *fbi, BitmapManager *manager) {
     
	 //-- Create a storage for this bitmap ------------------------------------

     BitmapStorage *s = BMMCreateStorage(manager,BMM_GRAY_8);

     if(!s)
        return NULL;

     if (s->Allocate(fbi,manager,BMM_OPEN_R)==0) {
        bail_out:
        if(s) {
           delete s;
           s = NULL;
        }
        return NULL;
     }

	uchar byte;
	int nbyte,bytesperrow,x,y,photo,bps,nmax;
	
	bytesperrow = tif->tif_scanlinesize;
	photo = td->td_photometric;
	bps = td->td_bitspersample;
	if (bps>8||bps==3||bps==5||bps==7) {
		DebugPrint("Bad bits per sample\n");
		goto bail_out;
		}

	nmax = (1<<bps)-1;

    PixelBuf line64(width);

	int accum = 0;
	int acc_ix = 0;

	for (y = 0; y<height; y++)  {
		if (TIFFReadScanline(tif, loadbuf, y, 0)<0) 
			goto bail_out;
        BMM_Color_64 *l64=line64.Ptr();
		x = 0;
		for (nbyte=0; nbyte<bytesperrow; nbyte++) {
			unsigned char mask;
			unsigned int c;
			int nsh;
			byte = loadbuf[nbyte];

			mask = nmax<<(8-bps);
			for (nsh=8-bps; nsh>=0; nsh-=bps, mask>>=bps) {
				c = (byte&mask)>>nsh;  
				c = (c*255)/nmax;
				if (photo==0) c = 255-c;
				accum += c;
				acc_ix++;
				if(acc_ix == td->td_samplesperpixel) {
					l64->r = l64->g = l64->b = (WORD)accum << 8;
					l64->a = 0;
					l64++;
					x++;
					accum = acc_ix = 0;
					}
				if (x >= width) break;
				}
			if (x >= width) break;
			}
        if(s->PutPixels(0,y,width,line64.Ptr())!=1)
            goto bail_out;
		}
	return s;
	}

static void PutColorComponent(BMM_Color_64 *ptr, int component, int width, BYTE *data) {
	UWORD *work = ((UWORD *)ptr) + component;
	for(int i = 0; i < width; ++i, work+=4, data++)
		*work = *data << 8;
	}

BitmapStorage *BitmapIO_TIF::TifReadPlanarRGB(BitmapInfo *fbi, BitmapManager *manager) {

	 //-- Create a storage for this bitmap ------------------------------------

     BitmapStorage *s = BMMCreateStorage(manager, BMM_TRUE_32);

     if(!s)
        return NULL;

     if(nsamp == 4)
        fbi->SetFlags(MAP_HAS_ALPHA);

     if (s->Allocate(fbi,manager,BMM_OPEN_R)==0) {
        bail_out:
        if(s) {
           delete s;
           s = NULL;
        }
        return NULL;
     }

    PixelBuf line64(width);

	int y,isamp;

	for (isamp=0; isamp<nsamp; isamp++) {
		for (y = 0; y<height; y++)  {
			if (TIFFReadScanline(tif, loadbuf, y, isamp)<0)
				goto bail_out;
			s->GetPixels(0,y,width,line64.Ptr());			
			PutColorComponent(line64.Ptr(), isamp, width, loadbuf);
			if(s->PutPixels(0,y,width,line64.Ptr())!=1)
				goto bail_out;
			}
		}
	return s;
	}

BitmapStorage *BitmapIO_TIF::TifReadChunkyRGB(BitmapInfo *fbi, BitmapManager *manager) {

	 //-- Create a storage for this bitmap ------------------------------------

     BitmapStorage *s = BMMCreateStorage(manager, BMM_TRUE_32);

     if(!s)
        return NULL;

     if(nsamp == 4)
        fbi->SetFlags(MAP_HAS_ALPHA);

     if (s->Allocate(fbi,manager,BMM_OPEN_R)==0) {
        bail_out:
        if(s) {
           delete s;
           s = NULL;
        }
        return NULL;
     }

    PixelBuf line64(width);

	uchar *scanptr;
	int x,y;
	for (y = 0; y<height; y++)  {
		if (TIFFReadScanline(tif, loadbuf, y, 0)<0)
			goto bail_out;
        BMM_Color_64 *l64=line64.Ptr();
		scanptr = loadbuf;
		for(x = 0; x < width; ++x, ++l64) {
			l64->r = *scanptr++ << 8;
			l64->g = *scanptr++ << 8;
			l64->b = *scanptr++ << 8;
			if(nsamp == 4)
				l64->a = *scanptr++ << 8;
			else
				l64->a = 0;
			}
		if(s->PutPixels(0,y,width,line64.Ptr())!=1)
			goto bail_out;
		}
	return s;
	}

void BitmapIO_TIF::ScrunchColorMap(BMM_Color_48 *colpal) {
	int npal,i;
	npal = 1<<td->td_bitspersample;
	for (i=0; i<npal; i++) {
		colpal[i].r = td->td_redcolormap[i];
		colpal[i].g = td->td_greencolormap[i];
		colpal[i].b = td->td_bluecolormap[i];
		}
	}

BitmapStorage *BitmapIO_TIF::TifReadColPal(BitmapInfo *fbi, BitmapManager *manager) {
	 //-- Create a storage for this bitmap ------------------------------------

     BitmapStorage *s = BMMCreateStorage(manager, BMM_PALETTED);

     if(!s)
        return NULL;

     if (s->Allocate(fbi,manager,BMM_OPEN_R)==0) {
        bail_out:
        if(s) {
           delete s;
           s = NULL;
        }
        return NULL;
     }

    PixelBuf8 line8(width);

	int x,y,bps,shft;
	BMM_Color_48 cmap[256];
	uchar byte,mask,*pbuf;

	bps = td->td_bitspersample;
	mask = (1<<bps)-1;
	ScrunchColorMap(cmap);
	s->SetPalette(0, 256, cmap);
	for (y = 0; y<height; y++)  {
		if (TIFFReadScanline(tif, loadbuf, y, 0)<0)
			goto bail_out;
		if (bps==4||bps==2||bps==1) {
			shft = -1;
			pbuf = loadbuf;
			for (x=0; x<width; x++) {
				if (shft<0) {
					byte = *pbuf++;
					shft = 8-bps;
					}
				line8[x] = (byte>>shft)&mask;
				shft-=bps;
				if (shft<0) {
					byte = *pbuf++;
					shft = 8-bps;
					}
				}
			if(s->PutIndexPixels(0,y,width,line8.Ptr())!=1)
				goto bail_out;
			}
		else {
			if(s->PutIndexPixels(0,y,width,loadbuf)!=1)	// 8 bits -- just use load buffer
				goto bail_out;
			}
		}
	return s;
	}


//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::OpenOutput()

BMMRES BitmapIO_TIF::OpenOutput(BitmapInfo *fbi, Bitmap *map) {

	if (openMode != BMM_NOT_OPEN)
		return (ProcessImageIOError(fbi,BMMRES_INTERNALERROR));
		
	if (!map)
		return (ProcessImageIOError(fbi,BMMRES_INTERNALERROR));
		
	//-- Check for Default Configuration -----------------
	
	if (!UserData.saved)
		ReadCfg();
	
    //-- Save Image Info Data

    bi.CopyImageInfo(fbi);    

    this->map   = map;
    openMode    = BMM_OPEN_W;

    return BMMRES_SUCCESS;

}

//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::Write()
//

BMMRES BitmapIO_TIF::Write(int frame) {
     
	//-- If we haven't gone through an OpenOutput(), leave

	if (openMode != BMM_OPEN_W)
		return (ProcessImageIOError(&bi,BMMRES_INTERNALERROR));

	//-- Resolve Filename --------------------------------

	TCHAR filename[MAX_PATH];

	if (frame == BMM_SINGLEFRAME) {
		_tcscpy(filename,bi.Name());
	} else {
		if (!BMMCreateNumberedFilename(bi.Name(),frame,filename))
			return (ProcessImageIOError(&bi,BMMRES_NUMBEREDFILENAMEERROR));
	}
	
     //-- Create Image File -------------------------------
     
     File file(filename, _T("wb"));
     
     if (!file.stream)
		return (ProcessImageIOError(&bi));

	// Below this line formatted for Tom's editor (sorry.)

	outStream = file.stream;

	// Find out what kind of output file we're dealing with

	saveStorage = map->Storage();
	if(!saveStorage)
		return (ProcessImageIOError(&bi,BMMRES_INTERNALERROR));

	int result = SaveTIF(file.stream);

	switch(result) {
		case TIF_SAVE_OK:
			return BMMRES_SUCCESS;
		case TIF_SAVE_WRITE_ERROR:
		default:
			return (ProcessImageIOError(&bi,GetString(IDS_WRITE_ERROR)));
		}

}

/** If this is 1, does horizontal differencing before LZW encode **/
int  hordif = 0; 

/* an actual tiff file header - with defaults in place */

static struct tiffhead ctiff =		/* an actual tiff file header */
{									/* with defaults in place */
	0X4949,42,8L,17,
	254,KPLONG,1L,0L,				/* defaulted */
	256,KPLONG,1L,0L,
	257,KPLONG,1L,0L,
	258,KPSHORT,1L,0,0,
	259,KPSHORT,1L,5,0,		 /* defaulted to LZW */
	262,KPSHORT,1L,0,0,
	266,KPSHORT,1L,1,0,			 /* defaulted */
	273,KPLONG,0L,0L,
	274,KPSHORT,1L,1,0,			 /* defaulted */
	277,KPSHORT,1L,0,0,
	278,KPSHORT,1L,0,0,
	279,KPLONG,0L,0L,
	282,KPRATIONAL,1L,0L,
	283,KPRATIONAL,1L,0L,
	284,KPSHORT,1L,0,0,
	296,KPSHORT,1L,1,0,	  	/* defaulted */
	317,KPSHORT,1L,1,0,	  	/* predictor:  defaulted */
	0L,							/* mark end of header */
	1L,1L,1L,1L,  				/* the aspect rationals */
	8,8,8,8						/* BitsPerSample for color */
	};

/* fill in the blanks in the tiff header */

void
BitmapIO_TIF::MakeTiffhead() {
	long work;

	ctiff.ImageWidth.val	= (long)(width);		/* image width */
	ctiff.ImageLength.val = (long)(height);		/* image height */
	ctiff.RowsPerStrip.val= rps;					/* rows per strip */
	ctiff.Predictor.val=(hordif?2:1);

	work = 10L;						 /* 10 bytes at start of header */ 
	work += (long)(ctiff.tags * 12);	/* + 12 bytes/tag */
	work += 4L;						 /* long 0 at end of header */
	ctiff.XResolution.val = work;		/* where xres1 can be found */
	work += 8L;						 /* two longs (xres1 & xres2) */
	ctiff.YResolution.val = work;		/* where yres1 can be found */
	work += 8L;						 /* two longs (yres1 & yres2) */

	if (UserData.writeType == WRITE_COLOR)				/* color tiff */
		{
		ctiff.BitsPerSample.n = write_alpha?4L:3L;
		ctiff.BitsPerSample.val = (short)work;	 /* where 8,8,8 can be found */
		ctiff.Photometric.val = 2;
		work += write_alpha?8L:6L;		 /* 3 shorts (rbits, gbits, bbits) */
		}
	else									/* greyscale tiff */
		{
		ctiff.BitsPerSample.n = 1L;
		ctiff.BitsPerSample.val = 8;		/* 8 bit greyscale */
		ctiff.Photometric.val = 1;
		}

	ctiff.StripOffsets.n = (long)blocks; /* # of entries in table */
	ctiff.StripOffsets.val = work;		 /* where the offset table starts */
	work += (long)(blocks*4);			    /* 4 byte offset for each line */
	
	ctiff.StripByteCounts.n = (long)blocks; /* # of entries in the table */
	ctiff.StripByteCounts.val = work;	/* where the byte count table starts */
 
	return;					 
	}

/*-----------------------------------------------------------
 * LZW Encoding.
-------------------------------------------------------------*/

#define	 MAXCODE(n)			((1 << (n)) - 1)
#define	SetMaxCode(v) {	lzw_state.lzw_maxcode = (v)-1; }

#ifdef ALLOW_COMPRESSION
/* Reset encoding state at the start of a strip. */
int BitmapIO_TIF::LZWPreEncode(void) {
	lzw_state.lzw_flags = 0;
	lzw_state.lzw_hordiff = hordif?LZW_HORDIFF8:0;
	lzw_state.lzw_stride = spp;
	lzw_state.enc_ratio = 0;
	lzw_state.enc_checkpoint = CHECK_GAP;
	lzw_state.lzw_nbits = BITS_MIN;
	SetMaxCode(MAXCODE(BITS_MIN)+1);
	lzw_state.lzw_free_ent = CODE_FIRST;
	lzw_state.lzw_bitoff = 0;
	lzw_state.lzw_bitsize = (BLOCKSIZE << 3) - (BITS_MAX-1);
	ClearHash();		/* clear hash table */
	lzw_state.lzw_oldcode = -1;	/* generates CODE_CLEAR in LZWEncode */
	return (1);
	}
#endif //ALLOW_COMPRESSION

#define REPEAT4(n, op)		\
    switch (n) {		\
    default: { int i; for (i = n-4; i > 0; i--) { op; } } \
    case 4:  op;		\
    case 3:  op;		\
    case 2:  op;		\
    case 1:  op;		\
    case 0:  ;			\
    }


static void
horizontalDifference8(register char *cp, register int cc, register int stride)
	{
	if (cc > stride) {
		cc -= stride;
		cp += cc - 1;
		do {
			REPEAT4(stride, cp[stride] -= cp[0]; cp--)
			cc -= stride;
			} while (cc > 0);
		}
	}

static void
horizontalDifference16(register short *wp, register int wc, register int stride)
	{
	if (wc > stride) {
		wc -= stride;
		wp += wc - 1;
		do {
			REPEAT4(stride, wp[stride] -= wp[0]; wp--)
			wc -= stride;
			} while (wc > 0);
		}
	}

#ifdef ALLOW_COMPRESSION
/* Encode a scanline of pixels.
 * Uses an open addressing double hashing (no chaining) on the 
 * prefix code/next character combination.  We do a variant of
 * Knuth's algorithm D (vol. 3, sec. 6.4) along with G. Knott's
 * relatively-prime secondary probe.  Here, the modular division
 * first probe is gives way to a faster exclusive-or manipulation. 
 * Also do block compression with an adaptive reset, whereby the
 * code table is cleared when the compression ratio decreases,
 * but after the table fills.  The variable-length output codes
 * are re-sized at this point, and a CODE_CLEAR is generated
 * for the decoder. 
 */
int BitmapIO_TIF::LZWEncode(uchar *bp,	int cc )	{
	register long fcode;
	register int h, c, ent, disp;

	/* XXX horizontal differencing alters user's data XXX */
	switch (lzw_state.lzw_hordiff) {
		case LZW_HORDIFF8:
			horizontalDifference8((char *)bp, cc, (int)lzw_state.lzw_stride);
			break;
		case LZW_HORDIFF16:
			horizontalDifference16((short *)bp, cc/2, (int)lzw_state.lzw_stride);
			break;
		}

	ent = lzw_state.lzw_oldcode;
	if (ent == -1 && cc > 0) {
		PutNextCode( CODE_CLEAR);
		ent = *bp++; cc--; lzw_state.enc_incount++;
		}
	while (cc > 0) {
		c = *bp++; cc--; lzw_state.enc_incount++;
		fcode = ((long)c << BITS_MAX) + ent;
		h = (c << HSHIFT) ^ ent;	/* xor hashing */
		if (lzw_state.enc_htab[h] == fcode) {
			ent = lzw_state.enc_codetab[h];
			continue;
			}
		if (lzw_state.enc_htab[h] >= 0) {
			/*
			 * Primary hash failed, check secondary hash.
			 */
			disp = HSIZE - h;
			if (h == 0)
				disp = 1;
			do {
				if ((h -= disp) < 0)
					h += HSIZE;
				if (lzw_state.enc_htab[h] == fcode) {
					ent = lzw_state.enc_codetab[h];
					goto hit;
					}
				} while (lzw_state.enc_htab[h] >= 0);
			}
		/*
		 * New entry, emit code and add to table.
		 */
		PutNextCode(ent);
		ent = c;
		
		lzw_state.enc_codetab[h] = lzw_state.lzw_free_ent++;
		lzw_state.enc_htab[h] = fcode;
		if (lzw_state.lzw_free_ent == CODE_MAX-1) {
			/* table is full, emit clear code and reset */
			lzw_state.enc_ratio = 0;
			ClearHash();
			lzw_state.lzw_free_ent = CODE_FIRST;
			PutNextCode(CODE_CLEAR);
			SetMaxCode(MAXCODE(lzw_state.lzw_nbits = BITS_MIN)+1);
		} else {
			if (lzw_state.enc_incount >= lzw_state.enc_checkpoint)
				ClearBlock();
			/*
			 * If the next entry is going to be too big for
			 * the code size, then increase it, if possible.
			 */
			if (lzw_state.lzw_free_ent > lzw_state.lzw_maxcode) {
				lzw_state.lzw_nbits++;
				SetMaxCode(MAXCODE(lzw_state.lzw_nbits)+1);
				}
			}
		hit:
			;
		}
	lzw_state.lzw_oldcode = ent;
	return (1);
	}

/*
 * Finish off an encoded strip by flushing the last
 * string and tacking on an End Of Information code.
 */
int BitmapIO_TIF::LZWPostEncode(void){
	if (lzw_state.lzw_oldcode != -1)
		PutNextCode(lzw_state.lzw_oldcode);
	PutNextCode(CODE_EOI);
	return (1);
	}

void BitmapIO_TIF::PutNextCode(int c)	{
	register long r_off;
	register int bits, code = c;
	register uchar *bp;

	r_off = lzw_state.lzw_bitoff;
	bits = lzw_state.lzw_nbits;
 	/* Get to the first byte. */
	bp = comp_buf + (r_off >> 3);
	r_off &= 7;
	/*
	 * Note that lzw_bitoff is maintained as the bit offset
	 * into the buffer w/ a right-to-left orientation (i.e.
	 * lsb-to-msb).  The bits, however, go in the file in
	 * an msb-to-lsb order.
	 */
	bits -= (8 - r_off);
	*bp = (*bp & lmask[r_off]) | (code >> bits);
	bp++;
	if (bits >= 8) {
		bits -= 8;
		*bp++ = code >> bits;
		}
	if (bits)
		*bp = (code & rmask[bits]) << (8 - bits);
	/*
	 * enc_outcount is used by the compression analysis machinery
	 * which resets the compression tables when the compression
	 * ratio goes up.  lzw_bitoff is used here (in PutNextCode) for
	 * inserting codes into the output buffer.  tif_rawcc must
	 * be updated for the mainline write code in TIFFWriteScanline()
	 * so that data is flushed when the end of a strip is reached.
	 * Note that the latter is rounded up to ensure that a non-zero
	 * byte count is present. 
	 */
	lzw_state.enc_outcount += lzw_state.lzw_nbits;
	lzw_state.lzw_bitoff += lzw_state.lzw_nbits;
	rawcc = (lzw_state.lzw_bitoff + 7) >> 3;
	}

/*
 * Check compression ratio and, if things seem to
 * be slipping, clear the hash table and reset state.
 */

void BitmapIO_TIF::ClearBlock(void) {
	register long rat;

	lzw_state.enc_checkpoint = lzw_state.enc_incount + CHECK_GAP;
	if (lzw_state.enc_incount > 0x007fffff) {	/* shift will overflow */
		rat = lzw_state.enc_outcount >> 8;
		rat = (rat == 0 ? 0x7fffffff : lzw_state.enc_incount / rat);
		} 
	else
		rat = (lzw_state.enc_incount << 8) / lzw_state.enc_outcount; /* 8 fract bits */
	if (rat <= lzw_state.enc_ratio) {
		lzw_state.enc_ratio = 0;
		ClearHash();
		lzw_state.lzw_free_ent = CODE_FIRST;
		PutNextCode(CODE_CLEAR);
		SetMaxCode(MAXCODE(lzw_state.lzw_nbits = BITS_MIN)+1);
		} 
	else
		lzw_state.enc_ratio = rat;
	}

/*  Reset code table. */

void BitmapIO_TIF::ClearHash() {
	register int *htab_p = lzw_state.enc_htab+HSIZE;
	register long i, m1 = -1;

	i = HSIZE - 16;
 	do {
		*(htab_p-16) = m1;
		*(htab_p-15) = m1;
		*(htab_p-14) = m1;
		*(htab_p-13) = m1;
		*(htab_p-12) = m1;
		*(htab_p-11) = m1;
		*(htab_p-10) = m1;
		*(htab_p-9) = m1;
		*(htab_p-8) = m1;
		*(htab_p-7) = m1;
		*(htab_p-6) = m1;
		*(htab_p-5) = m1;
		*(htab_p-4) = m1;
		*(htab_p-3) = m1;
		*(htab_p-2) = m1;
		*(htab_p-1) = m1;
		htab_p -= 16;
		} while ((i -= 16) >= 0);
   for (i += 16; i > 0; i--)
		*--htab_p = m1;
	}
#endif //ALLOW_COMPRESSION

int BitmapIO_TIF::WriteTIF(FILE *stream) {
	long	offsets= ctiff.StripOffsets.val,packw;
	long	counts = ctiff.StripByteCounts.val;
	WORD y, j, k, l;
	unsigned long grey;
	int nstrip = 0;

	y = 0;

	do {
		rawcc = 0;
#ifdef ALLOW_COMPRESSION
		if (UserData.compressed)
			LZWPreEncode();
#endif //ALLOW_COMPRESSION
		for (j=0; j<rps; j++) {

			/* get a row and prepare for encoding */
			GetOutputPixels(0,y++,width,scanline); // Get gamma-corrected pixels

			/* remove the desired sample */
			if (UserData.writeType == WRITE_COLOR) {				/* color tiff */
				int index = 0;
				scanptr = scanline;
				for(k = 0; k < width; ++k, ++scanptr) {
					WORD *pixel = (WORD *)scanptr;
					for(l = 0; l < spp; ++l)
						shortstrip[index++] = pixel[l] >> 8;
					}
				packw = width*spp;
				}
			else {									/* grey scale tiff */
				scanptr = scanline;
				for (k=0; k<width; k++, ++scanptr) {
					grey = (unsigned long)scanptr->r * 30L;		/* 30% red */
					grey+= (unsigned long)scanptr->g * 59L;		/* 59% green */
					grey+= (unsigned long)scanptr->b * 11L;		/* 11% blue */
					grey/= 100L;
					shortstrip[k] = (BYTE)(grey >> 8);
					}
				packw = width;
				}
#ifdef ALLOW_COMPRESSION
			if (UserData.compressed)
				LZWEncode(shortstrip, packw);
			else {
#endif //ALLOW_COMPRESSION
				memcpy(comp_buf+rawcc,shortstrip,packw);
				rawcc+= packw;
#ifdef ALLOW_COMPRESSION
				}
#endif //ALLOW_COMPRESSION
			if (y == height) break;
			}

#ifdef ALLOW_COMPRESSION
		if (UserData.compressed)
			LZWPostEncode();
#endif //ALLOW_COMPRESSION

		fseek(stream, 0L, SEEK_END);
		long filehold = ftell(stream);

		long kount = rawcc;

		if (!TIFWRITE(comp_buf, kount)) return(-1);
		fseek(stream, offsets, SEEK_SET);
		if (!TIFWRITE(&filehold, 4))	 return(-1);

		fseek(stream, counts, SEEK_SET);
		if (!TIFWRITE(&kount, 4))		return(-1);
		offsets+=4;	counts+=4;
		}	while (y != height);

	return((int)blocks);
	}

int
BitmapIO_TIF::SaveTIF(FILE *stream) {
	unsigned short i, work, ret=1;
	uchar	 spaces[8];

	width = map->Width();
	height = map->Height();

//	themap = map;
	write_alpha = (UserData.writeType == WRITE_COLOR && map->HasAlpha()) ? TRUE : FALSE;
	ctiff.PlanarConfiguration.val = 1;	/* chunky */
	spp =(UserData.writeType == WRITE_COLOR)?(write_alpha?4:3):1;  
	ctiff.SamplesPerPixel.val = spp;

	/* allocate memory for work areas */

	PixelBuf64 sline(width);
	scanline = sline.Ptr();
	PixelBuf8 sstrip(spp * width);
	shortstrip = sstrip.Ptr();
	PixelBuf8 cbuf((BLOCKSIZE*3)/2);
	comp_buf = cbuf.Ptr();

	/*
	 * rows per strip must be carefully chosen - especially in light
	 * of RGB images. Assuming a worse case scenario wrt/packing bits,
	 * (ie: a single plane receives NO compression due to the complexity
	 * of the image) there might be 512 * RPS bytes in a strip; assuming
	 * the use of a targa or vision16 sized image (512 X 486). The
	 * value of the image width (512) times the rows per strip must
	 * NOT exceed the BLOCKSIZE definition. If this happens, the
	 * potential exists to corrupt memory.
	 */

	rps = BLOCKSIZE/(width*spp);						/* nice default value */
	if (rps==0)
		return TIF_SAVE_WRITE_ERROR;

	blocks = ((height+rps-1)/rps);

	if (blocks==1) { /* want at least 2 blocks */
		int bsize = (width*height+1)/2;
		rps = bsize/(width*spp);
		blocks = ((height+rps-1)/rps);
		}

#ifdef ALLOW_COMPRESSION
	if (UserData.compressed)
		ctiff.Compression.val = 5;	  		/* LZW */
	else
#endif //ALLOW_COMPRESSION
		ctiff.Compression.val = 1;				/* set for no compression */

	for (i=0; i<8; i++)
		spaces[i] = 0;							/* filler for strip data */

	/* construct tiff file header */
	MakeTiffhead();

	work = sizeof(struct tiffhead);

	if (UserData.writeType == WRITE_MONO)
		work -= 8;			/* if grey, BitsPerSample.val is not an offset */
	else if (spp==3)
		work -=2;		/* no alpha component */

	/* write header */
	if (!TIFWRITE(&ctiff, work))	{
		ret = TIF_SAVE_WRITE_ERROR;
		goto tif_save_abort;
		}

	for (i = 0; i < blocks; i++)
		if (!TIFWRITE(&spaces, 8)) {
			ret = TIF_SAVE_WRITE_ERROR;
			goto tif_save_abort;
			}

	if (WriteTIF(stream) != blocks)
		ret = TIF_SAVE_WRITE_ERROR;

	tif_save_abort:

	if (ret != 1)	 {
		/* invalidate tiff file */
		fseek(stream, 0L, SEEK_SET);
		char *invalid = "INVALID";
		TIFWRITE(invalid,7);
		}

	return(ret);
	}

//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::GetCfgFilename()
//

void BitmapIO_TIF::GetCfgFilename( TCHAR *filename ) {
	_tcscpy(filename,TheManager->GetDir(APP_PLUGCFG_DIR));
	int len = _tcslen(filename);
	if (len) {
		if (_tcscmp(&filename[len-1],_T("\\")))
			_tcscat(filename,_T("\\"));
	}   
	_tcscat(filename,TIFCONFIGNAME);   
}

//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::ReadCfg()
//

BOOL BitmapIO_TIF::ReadCfg() {
	
	TCHAR filename[MAX_PATH];
	GetCfgFilename(filename);

	//-- Open Configuration File
	
	File file(filename, _T("rb"));
	
	if (!file.stream)
		return (FALSE);
	
	fseek(file.stream,0,SEEK_END);
	DWORD size = (DWORD)ftell(file.stream);
	
	if (size) {

		fseek(file.stream,0,SEEK_SET);
		
		//-- Allocate Temp Buffer
		
		BYTE *buf = (BYTE *)LocalAlloc(LPTR,size);
		
		if (!buf)
			return (FALSE);
		
		//-- Read Data Block and Set it
		
		BOOL res = FALSE;
		
		if (fread(buf,1,size,file.stream) == size)
			res = LoadConfigure(buf);
		
		LocalFree(buf);
	
		return (res);
	
	}
	
	return (FALSE);
}
	
//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::WriteCfg()
//

void BitmapIO_TIF::WriteCfg() {
 
	TCHAR filename[MAX_PATH];
	GetCfgFilename(filename);
	
	//-- Find out buffer size
	
	DWORD size = EvaluateConfigure();
	
	if (!size)
		return;
	
	//-- Allocate Temp Buffer
	
	BYTE *buf = (BYTE *)LocalAlloc(LPTR,size);
	
	if (!buf)
		return;
	
	//-- Get Data Block and Write it
	
	if (SaveConfigure(buf)) {   
		File file(filename, _T("wb"));
		if (file.stream) {
			fwrite(buf,1,size,file.stream);
		}
	}
	
	LocalFree(buf);
	
}


//-----------------------------------------------------------------------------
// #> BitmapIO_TIF::Close()
//

int  BitmapIO_TIF::Close( int flag ) {
     if(openMode != BMM_OPEN_W)
        return 0;
     return 1;
}

//-- EOF: TIF.cpp -----------------------------------------------------------
