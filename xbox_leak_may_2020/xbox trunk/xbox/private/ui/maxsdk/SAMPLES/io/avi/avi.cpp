//-----------------------------------------------------------------------------
// ------------------
// File ....: avi.cpp
// ------------------
// Author...: Gus J Grubba
// Date ....: November 1995
// Descr....: AVI File I/O Module
//
// History .: Nov, 10 1995 - Started
//            
//-----------------------------------------------------------------------------
		
//-- Include files

#include <Max.h>
#include <bmmlib.h>
#include <mmsystem.h>
#include <vfw.h>
#include <float.h>
#include "avi.h"
#include "avirc.h"

//-- Globals ------------------------------------------------------------------

HINSTANCE hInst = NULL;

//-----------------------------------------------------------------------------
//-- File Class

class File {
	public:
		FILE  *stream;
		File  ( const TCHAR *name, const TCHAR *mode) { stream = _tfopen(name,mode); }
		~File ( ) { Close(); }
		void Close() { if(stream) fclose(stream); stream = NULL; }
};

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
// AVI Class Description

class AVIClassDesc:public ClassDesc {
	
	public:

		int             IsPublic     ( )                   { return 1;                }
		void           *Create       ( BOOL loading=FALSE) { return new BitmapIO_AVI; }
		const TCHAR    *ClassName    ( )                   { return GetString(IDS_AVI);     }
		SClass_ID       SuperClassID ( )                   { return BMM_IO_CLASS_ID;  }
		Class_ID        ClassID      ( )                   { return Class_ID(AVICLASSID,0);    }
		const TCHAR    *Category     ( )                   { return GetString(IDS_BITMAP_IO); }

};

static AVIClassDesc AVIDesc;

//-----------------------------------------------------------------------------
// Interface

DLLEXPORT const TCHAR * LibDescription ( )  { 
	return GetString(IDS_AVI_DESC); 
}

DLLEXPORT int LibNumberClasses ( ) { 
	return 1; 
}

DLLEXPORT ClassDesc *LibClassDesc(int i) {
	switch(i) {
		case  0: return &AVIDesc; break;
		default: return 0;        break;
	}
}

DLLEXPORT ULONG LibVersion ( )  { 
	return ( VERSION_3DSMAX ); 
}

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}

//-----------------------------------------------------------------------------
// *> AboutCtrlDlgProc()
//

BOOL CALLBACK AboutCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {

	switch (message) {
		
		case WM_INITDIALOG: {
			  CenterWindow(hWnd,GetParent(hWnd));
			  return 1;
		}

		case WM_COMMAND:

			  switch (LOWORD(wParam)) {
				  
				  case IDOK:              
						 EndDialog(hWnd,1);
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
// #> BitmapIO_AVI::CleanUp()

void BitmapIO_AVI::CleanUp() {
	if (getframe)    AVIStreamGetFrameClose(getframe);
	if (pavi)        AVIStreamRelease(pavi);
	if (pcomp)       AVIStreamRelease(pcomp);
	if (pfile)       AVIFileRelease(pfile);
	if (initialized) AVIFileExit();
	getframe    = NULL;
	pavi        = NULL;
	pcomp       = NULL;
	pfile       = NULL;
	initialized = FALSE;
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::BitmapIO_AVI()

BitmapIO_AVI::BitmapIO_AVI  ( ) {
	pavi            = NULL; 
	pcomp           = NULL;
	pfile           = NULL; 
	getframe        = NULL;
	initialized     = FALSE;
	memset(&data,0,sizeof(AVIDATA));
	data.version    = AVIVERSION;
	data.saved      = FALSE;
	data.depth      = 24;
	data.cpdatasize = 0;
	cpdata          = (void *)NULL;
}

BitmapIO_AVI::~BitmapIO_AVI ( ) {
  
	CleanUp();
	
	if (cpdata) {
		LocalFree(cpdata);
		data.cpdatasize = 0;
		cpdata = (void *)NULL;
	}
	
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::LongDesc()

const TCHAR *BitmapIO_AVI::LongDesc() {
	return GetString(IDS_AVI_FILE);
}
	
//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::ShortDesc()

const TCHAR *BitmapIO_AVI::ShortDesc() {
	return GetString(IDS_AVI);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::LoadConfigure()

BOOL BitmapIO_AVI::LoadConfigure ( void *ptr ) {
	AVIDATA *buf = (AVIDATA *)ptr;
	if (buf->version == AVIVERSION) {
		memcpy((void *)&data,ptr,sizeof(AVIDATA));
		if (data.cpdatasize) {
			cpdata = (void *)LocalAlloc(LPTR,data.cpdatasize);
			if (cpdata) {
				BYTE *stuff = (BYTE *)ptr + sizeof(AVIDATA);
				memcpy(cpdata,stuff,data.cpdatasize);
			} else
				data.cpdatasize = 0;
		}
		return (TRUE);
	} else
		return (FALSE);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::SaveConfigure()

BOOL BitmapIO_AVI::SaveConfigure ( void *ptr ) {
	if (ptr) {
		data.saved = TRUE;
		memcpy(ptr,(void *)&data,sizeof(AVIDATA));
		if (cpdata) {
			BYTE *stuff = (BYTE *)ptr + sizeof(AVIDATA);
			memcpy(stuff,cpdata,data.cpdatasize);
		}
		return (TRUE);
	} else
		return (FALSE);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::EvaluateConfigure()

DWORD BitmapIO_AVI::EvaluateConfigure ( ) {
	 return (sizeof(AVIDATA) + data.cpdatasize);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::ShowAbout()

void BitmapIO_AVI::ShowAbout(HWND hWnd) {
	DialogBoxParam(
		hInst,
		MAKEINTRESOURCE(IDD_AVI_ABOUT),
		hWnd,
		(DLGPROC)AboutCtrlDlgProc,
		(LPARAM)0);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::ControlDlg
//

BOOL BitmapIO_AVI::ControlDlg(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
#if defined _M_IX86
    unsigned int oldMask, newMask;
#endif  
	switch (message) {
	
		case WM_INITDIALOG: {

			  CenterWindow(hWnd,GetParent(hWnd));
			  SetCursor(LoadCursor(NULL,IDC_ARROW));

			  if (!data.saved)
				  ReadCfg();

			  if (data.cv.fccHandler) {
				  ICINFO ic;
				  memset((void *)&ic,0,sizeof(ICINFO));
				  ic.dwSize = sizeof(ICINFO);
				  HIC hic = ICOpen(data.cv.fccType,data.cv.fccHandler,ICMODE_QUERY);
				  if (hic) {
					  ICSendMessage(hic,ICM_GETINFO,(DWORD)(LPVOID)&ic,sizeof(ICINFO));
					  ICClose(hic);
				  } else
					  goto undefined;
				  SetDlgItemTextW(hWnd,IDC_CONTROL_COMPRESSOR,ic.szDescription);
			  } else {
				  undefined:
				  TCHAR text[128];
				  LoadString(hInst,IDS_UNDEFINED,text,128);
				  SetDlgItemText(hWnd, IDC_CONTROL_COMPRESSOR,text);
			  }

			  CheckRadioButton(
				 hWnd,
				 IDC_RAD_24,
				 IDC_RAD_8,
				 data.depth == 24 ? IDC_RAD_24:IDC_RAD_8
			  );

			  return 1;
			  
		}

		case WM_COMMAND:

			  switch (LOWORD(wParam)) {

				  //-- Compressor Setup ---------------------

				  case IDC_CONTROL_SETUP:

						 COMPVARS tcv;
						 
						 //-- Define Bitmap Type
						 
						 BITMAPINFOHEADER bi;
						 memset(&bi,0,sizeof(BITMAPINFOHEADER));
						 bi.biSize        = sizeof(BITMAPINFOHEADER);
						 bi.biWidth       = 640;
						 bi.biHeight      = 480;
						 bi.biPlanes      = 1;
						 bi.biBitCount    = 24;
						 bi.biCompression = BI_RGB;
						 bi.biSizeImage   = 0;
						 
						 if (data.cv.fccHandler) {
							 memcpy(&tcv,&data.cv,sizeof(COMPVARS));
							 tcv.hic     = ICOpen(tcv.fccType,tcv.fccHandler,ICMODE_QUERY);
							 if (!tcv.hic)
								 goto noinfo;
							 tcv.dwFlags = ICMF_COMPVARS_VALID;
							 tcv.lpbiOut = (BITMAPINFO *)(void *)&bi;
							 tcv.lpbiIn  = NULL;
							 if (cpdata && data.cpdatasize) {
								 ICSetState(tcv.hic,cpdata,data.cpdatasize);
								 tcv.lpState = NULL;
								 tcv.cbState = 0;
							 } else {
								 tcv.lpState = NULL;
								 tcv.cbState = 0;
							 }
						 } else {
							 noinfo:
							 memset(&tcv,0,sizeof(COMPVARS));
							 tcv.cbSize  = sizeof(COMPVARS);
						 }
						 
						 // Keith Trummel November 21, 1996
						 // This is to work around a strange problem exhibited on some sytems
						 // under Windows 95 where the call ICCompressorChoose resets the 
						 // floating point mask causing crashes later
#if defined _M_IX86
						 oldMask = _control87 (0, 0);						 
#endif
						 if ( ICCompressorChoose (
									hWnd,                  //-- HWND hwnd, 
									ICMF_CHOOSE_KEYFRAME,  //-- UINT uiFlags, 
									(LPVOID)&bi,           //-- LPVOID pvIn, 
									NULL,                  //-- LPVOID lpData, 
									&tcv,                  //-- PCOMPVARS pc, 
									NULL)                  //-- LPSTR lpszTitle); 
							  
							 ) {
							 
							 if (tcv.hic) {
							 
								 //-- Display Driver Name -------
								 
								 ICINFO ic;
								 ICSendMessage(tcv.hic, ICM_GETINFO,(DWORD)(LPVOID)&ic,sizeof(ICINFO));
								 SetDlgItemTextW(hWnd, IDC_CONTROL_COMPRESSOR, ic.szDescription);
								 
								 //-- Free Any Previous Data ----
								 
								 if (cpdata) {
									 LocalFree(cpdata);
									 data.cpdatasize = 0;
									 cpdata = (void *)NULL;
								 }
								 
								 //-- Save Compressor State -----
								 
								 memcpy( &data.cv, &tcv, sizeof(COMPVARS));
								 data.cpdatasize = ICGetStateSize(tcv.hic);

								 if (data.cpdatasize)
									 cpdata = (void *)LocalAlloc(LPTR,data.cpdatasize);
								 else
									 cpdata = NULL; 
								 
								 if (!cpdata) {
									 data.cpdatasize = 0;
								 } else {
									 ICGetState(tcv.hic,cpdata,data.cpdatasize);
								 }
								 

							 }
							 // Handle floating point exception mask being changed on us
#if defined _M_IX86
							 newMask = _control87 (0, 0);
							 if (newMask != oldMask) 
								 newMask = control87 ( _CW_DEFAULT, 0xfffff );
#endif

							 ICCompressorFree(&tcv);
						 
						 }
						 break;

				  //-- Changes Accepted ---------------------

				  case IDOK:
				  
						 if (IsDlgButtonChecked(hWnd,IDC_RAD_24))
							data.depth = 24;
						 else   
							data.depth =  8;
				  
						 WriteCfg();
						 EndDialog(hWnd,1);
						 break;

				  //-- Changes Aborted ----------------------

				  case IDCANCEL:
						 EndDialog(hWnd,0);
						 break;
		
			  }
			  return 1;

	}
	return 0;

}

//-----------------------------------------------------------------------------
// *> ControlDlgProc()
//

static BOOL CALLBACK ControlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static BitmapIO_AVI *io = NULL;
	if (message == WM_INITDIALOG) 
		io = (BitmapIO_AVI *)lParam;
	if (io) 
		return (io->ControlDlg(hWnd,message,wParam,lParam));
	else
		return(FALSE);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::ShowControl()

BOOL BitmapIO_AVI::ShowControl(HWND hWnd, DWORD flag ) {

	if (!data.saved)
		ReadCfg();

	COMPVARS tcv;
	
	//-- Define Bitmap Type
	
	BITMAPINFOHEADER bi;
	memset(&bi,0,sizeof(BITMAPINFOHEADER));
	bi.biSize        = sizeof(BITMAPINFOHEADER);
	bi.biWidth       = 640;
	bi.biHeight      = 480;
	bi.biPlanes      = 1;
	bi.biBitCount    = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage   = 0;
	
	if (data.cv.fccHandler) {
		memcpy(&tcv,&data.cv,sizeof(COMPVARS));
		tcv.hic     = ICOpen(tcv.fccType,tcv.fccHandler,ICMODE_QUERY);
		if (!tcv.hic && (data.cv.fccHandler != 0x20424944) /* "DIB" -- doesn't have compressor */ )
			goto noinfo;
		tcv.dwFlags = ICMF_COMPVARS_VALID;
//		tcv.lpbiOut = (BITMAPINFO *)(void *)&bi;
		tcv.lpbiOut = (BITMAPINFO *)NULL;
		tcv.lpbiIn  = NULL;
		tcv.fccType = 0;
		if (cpdata && data.cpdatasize) {
//			ICSetState(tcv.hic,cpdata,data.cpdatasize);
			tcv.lpState = cpdata;
			tcv.cbState = data.cpdatasize;
		} else {
			tcv.lpState = NULL;
			tcv.cbState = 0;
		}
	} else {
		noinfo:
		memset(&tcv,0,sizeof(COMPVARS));
		tcv.cbSize  = sizeof(COMPVARS);
	}

	// Keith Trummel November 21, 1996
	// This is to work around a strange problem exhibited on some sytems
	// under Windows 95 where the call ICCompressorChoose resets the 
	// floating point mask causing crashes later
#if defined _M_IX86
	unsigned int oldMask = _control87 (0, 0);						 
#endif

	if ( ICCompressorChoose (
			  hWnd,                  //-- HWND hwnd, 
			  ICMF_CHOOSE_KEYFRAME,  //-- UINT uiFlags, 
			  (LPVOID)&bi,           //-- LPVOID pvIn, 
			  NULL,                  //-- LPVOID lpData, 
			  &tcv,                  //-- PCOMPVARS pc, 
			  NULL)                  //-- LPSTR lpszTitle); 
		 
		) {
		
		memcpy( &data.cv, &tcv, sizeof(COMPVARS));

		//-- DS 6/14/99: Moved this outside of the "if (tvc.hic)" test to fix defect 188811
		if (cpdata) {
//	 		LocalFree(cpdata);	my guess is that this was already free'd by ICCompressorChoose
//	 		Since this is totally undocumented, we might never know...  DB 3/12/96
			data.cpdatasize = 0;
			cpdata = (void *)NULL;
		}

		if (tcv.hic) {
		
			//-- Display Driver Name -------
			
			ICINFO ic;
			ICSendMessage(tcv.hic, ICM_GETINFO,(DWORD)(LPVOID)&ic,sizeof(ICINFO));
			SetDlgItemTextW(hWnd, IDC_CONTROL_COMPRESSOR, ic.szDescription);
			
			
			//-- Save Compressor State -----
			
			data.cpdatasize = ICGetStateSize(tcv.hic);

			if (data.cpdatasize)
				cpdata = (void *)LocalAlloc(LPTR,data.cpdatasize);
			else
				cpdata = NULL; 
			
			if (!cpdata) {
				data.cpdatasize = 0;
			} else {
				ICGetState(tcv.hic,cpdata,data.cpdatasize);
			}
		}
			

							 // Handle floating point exception mask being changed on us
#if defined _M_IX86
		unsigned int newMask = _control87 (0, 0);
		if (newMask != oldMask) 
			newMask = control87 ( _CW_DEFAULT, 0xfffff );
#endif

		WriteCfg();
		ICCompressorFree(&tcv);
		return (TRUE);

	} else

		return (FALSE);

	/*
	return (
		DialogBoxParam(
		hInst,
		MAKEINTRESOURCE(IDD_AVI_CONTROL),
		hWnd,
		(DLGPROC)ControlDlgProc,
		(LPARAM)this)
	);
	*/
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::GetImageInfo()

BMMRES BitmapIO_AVI::GetImageInfo ( BitmapInfo *fbi ) {

	//-- Get File Info -------------------------
	
	BMMRES res = LocalGetInfo (&avi,fbi);
	
	if (res != BMMRES_SUCCESS)
		return (res);
	
	//-- Update Bitmap Info ------------------------------
	
	fbi->SetWidth( (WORD)(avi.rcFrame.right-avi.rcFrame.left));
	fbi->SetHeight((WORD)(avi.rcFrame.bottom-avi.rcFrame.top));
	fbi->SetType(BMM_NO_TYPE);
	//fbi->SetGamma(1.0f);
	fbi->SetAspect(1.0f);
	fbi->SetFirstFrame(0);
	fbi->SetLastFrame(avi.dwLength-1);

	return BMMRES_SUCCESS;

}

//-----------------------------------------------------------------------------
//-- BitmapIO_AVI::Load()

BitmapStorage *BitmapIO_AVI::Load(BitmapInfo *fbi, Bitmap *map, BMMRES *status) {

	BitmapStorage    *s       = NULL;
	BITMAPINFOHEADER *pbi     = NULL;

	int frame;
	
	//-- Initialize Status Optimistically

	*status = BMMRES_SUCCESS;

	//-- Make sure nothing weird is going on

	if(openMode != BMM_NOT_OPEN) {
		*status = ProcessImageIOError(fbi,BMMRES_INTERNALERROR);
		return NULL;
	}

	//-- Open AVI File -----------------------------------
	
	HRESULT  res;

	AVIFileInit();
	initialized = TRUE;
	
	res = AVIFileOpen(&pfile, fbi->Name(), OF_SHARE_DENY_WRITE, 0L);
	
	if (res) {
		switch (res) {
			case AVIERR_BADFORMAT:
				*status = ProcessImageIOError(fbi,GetString(IDS_BAD_FORMAT));
				break;
			case AVIERR_MEMORY:
				*status = ProcessImageIOError(fbi,BMMRES_MEMORYERROR);
				break;
			case AVIERR_FILEREAD:
				*status = ProcessImageIOError(fbi,GetString(IDS_READ_ERROR));
				break;
			case AVIERR_FILEOPEN:
				*status = ProcessImageIOError(fbi,GetString(IDS_OPEN_ERROR));
				break;
			case REGDB_E_CLASSNOTREG:
				*status = ProcessImageIOError(fbi,GetString(IDS_BAD_CLASS));
				break;
			default:
				*status = ProcessImageIOError(fbi,GetString(IDS_BAD_FILE));
				break;
		}
		CleanUp();
		return NULL;
	}

	//-- Open Video Stream -------------------------------
	
	res = AVIFileGetStream(pfile, &pavi, streamtypeVIDEO, 0);
	
	if (res) {
		*status = ProcessImageIOError(fbi,GetString(IDS_BAD_FILE));
		if(*status != BMMRES_RETRY)
			CleanUp();
		return NULL;
	}
	
	//-- Get Stream Info ---------------------------------
	
	AVISTREAMINFO avi;
	
	res = AVIStreamInfo(pavi, &avi, sizeof(AVISTREAMINFO));
	
	if (res) {
		*status = ProcessImageIOError(fbi,GetString(IDS_BAD_FILE));
		bail_out:
		if (s)
			delete s;
		CleanUp();
		return NULL;
	}
	
	//-- Update Bitmap Info ------------------------------
	
	fbi->SetWidth( (WORD)(avi.rcFrame.right-avi.rcFrame.left));
	fbi->SetHeight((WORD)(avi.rcFrame.bottom-avi.rcFrame.top));
	//fbi->SetGamma(1.0f);
	fbi->SetAspect(1.0f);
	fbi->SetFirstFrame(0);
	fbi->SetLastFrame(avi.dwLength-1);

	//-- Handle Given Frame Number -----------------------
	
	*status = GetFrame(fbi,&frame);

	if (*status != BMMRES_SUCCESS)
		goto bail_out;
		  
	//-- Load Frame --------------------------------------
	
	/*
	memset (&bih,0,sizeof(BITMAPINFOHEADER));
	bih.biSize         = sizeof(BITMAPINFOHEADER);
	bih.biWidth        = fbi->Width();
	bih.biHeight       = fbi->Height();
	bih.biPlanes       = 1;
	bih.biBitCount     = 24;
	bih.biCompression  = BI_RGB;
	*/
	
	getframe = AVIStreamGetFrameOpen(pavi,NULL);
	
	if (!getframe) {
		*status = ProcessImageIOError(fbi,GetString(IDS_BAD_FILE));
		goto bail_out;
	}        
	
	pbi = (BITMAPINFOHEADER *)AVIStreamGetFrame(getframe,frame);
	
	if (!pbi) {
		*status = ProcessImageIOError(fbi,BMMRES_BADFILEHEADER);
		goto bail_out;
	}        
	
	//-- Create Image Storage ---------------------------- 
	
	if (pbi->biBitCount >= 16) {
		fbi->SetType(BMM_TRUE_24);
		s = BMMCreateStorage(map->Manager(),BMM_TRUE_32);
	} else if (pbi->biBitCount == 8) {
		fbi->SetType(BMM_PALETTED);
		s = BMMCreateStorage(map->Manager(),BMM_PALETTED);
	} else {
		*status = ProcessImageIOError(fbi,BMMRES_INTERNALERROR);
		goto bail_out;
	}   

	if (!s) {
		*status = ProcessImageIOError(fbi,BMMRES_INTERNALERROR);
		goto bail_out;
	}

	//-- Allocate Image Storage --------------------------
	
	if (s->Allocate(fbi,map->Manager(),BMM_OPEN_R)==0) {
		*status = ProcessImageIOError(fbi,BMMRES_MEMORYERROR);
		goto bail_out;
	}

	s->bi.CopyImageInfo(fbi);
	map->SetStorage(s);

	//-- Handle 8 bit bitmaps ----------------------------

	if (pbi->biBitCount == 8) {

		//-- Handle Palette -------------------------------
		
		if (!pbi->biClrUsed) 
			pbi->biClrUsed = 256;

		BMM_Color_48 *pal = (BMM_Color_48 *)malloc(pbi->biClrUsed * sizeof(BMM_Color_48));
		
		if (!pal)
			goto bail_out;
		
		BYTE *ptr = (BYTE *)(void *)pbi;
		ptr += pbi->biSize;
		RGBQUAD *rgb = (RGBQUAD *)(void *)ptr;
		
		for (int j = 0; j < (int)pbi->biClrUsed; j++) {
			pal[j].r = rgb[j].rgbRed   << 8;
			pal[j].g = rgb[j].rgbGreen << 8;
			pal[j].b = rgb[j].rgbBlue  << 8;
		}
		
		s->SetPalette(0,pbi->biClrUsed,pal);
		
		free(pal);

		//-- Handle Bitmap --------------------------------
		
		BYTE *p  = (BYTE *)(void *)ptr;
		p       += pbi->biClrUsed * sizeof(RGBQUAD);
		int h    = pbi->biHeight;
		int w    = pbi->biWidth;
		int wBytes = (w + 3) & ~3;
		
		for (int y = 0; y < h; y++) {
			s->PutIndexPixels(0,(h - y - 1),w,p);
			p+=wBytes;

			//-- Progress Report
			
			if (fbi->GetUpdateWindow())
				SendMessage(fbi->GetUpdateWindow(),BMM_PROGRESS,y,h);

		}

	//-- Handle 24 bit bitmaps ---------------------------

	} else {
	
		//-- Convert DIB to Bitmap ------------------------
		
		if (fbi->GetUpdateWindow())
			SendMessage(fbi->GetUpdateWindow(),BMM_PROGRESS,100,100);

		if (!map->FromDib((BITMAPINFO *)(void *)pbi)) {
			*status = ProcessImageIOError(fbi,BMMRES_INTERNALERROR);
			goto bail_out;
		}
		
	}   
	
	CleanUp();
	return s;
	
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::OpenOutput()
//
//
//    For now I'm using AVIMakeCompressedStream() to handle the compression. I
//    need to be able to set up the compressor myself which this function does
//    not allow. There are two options: Find a way to talk to the compressor
//    from here, or replace this with an actual sequence of ICOpen, ICSetState,
//    ICImageCompress, etc... drag...
//
//

BMMRES BitmapIO_AVI::OpenOutput(BitmapInfo *fbi, Bitmap *map) {

	if (openMode != BMM_NOT_OPEN)
		return (ProcessImageIOError(fbi,BMMRES_INTERNALERROR));
		
	if (!map)
		return (ProcessImageIOError(fbi,BMMRES_INTERNALERROR));
		
	//-- Check for Default Configuration -----------------
	
	if (!data.saved)
		ReadCfg();
	
	//-- Create AVI File ---------------------------------
	
	HRESULT res;

	AVIFileInit();

	initialized = TRUE;
	formatInit  = FALSE;
	aframe      = 0;
	
	remove(fbi->Name());	// this is because the AVIFileOpen doesn't truncate the file size
	res = AVIFileOpen(&pfile, fbi->Name(), OF_CREATE | OF_WRITE, NULL);
	
	if (res) {
		CleanUp();
		switch (res) {
			case AVIERR_BADFORMAT:
				return (ProcessImageIOError(fbi,GetString(IDS_BAD_FORMAT)));
			case AVIERR_MEMORY:
				return (ProcessImageIOError(fbi,BMMRES_MEMORYERROR));
			case AVIERR_FILEREAD:
				return(ProcessImageIOError(fbi,GetString(IDS_READ_ERROR)));
			case AVIERR_FILEOPEN:
				return(ProcessImageIOError(fbi,GetString(IDS_OPEN_ERROR)));
			case REGDB_E_CLASSNOTREG:
				return(ProcessImageIOError(fbi,GetString(IDS_BAD_CLASS)));
			default:
				return(ProcessImageIOError(fbi,GetString(IDS_BAD_FILE)));
		}
	}

	//-- Default Compressor and Data ---------------------
	
	DWORD compHandler;
	DWORD qFactor;
	DWORD lKeys;
	
	if (data.cv.fccHandler) {
		compHandler = data.cv.fccHandler;
		qFactor     = data.cv.lQ;
		lKeys       = data.cv.lKey;
	} else {
		compHandler = mmioFOURCC('M', 'S', 'V', 'C');
		qFactor     = 7500;
		lKeys       = 15;
	}
	
	//-- Create Video Stream -----------------------------
	
	memset(&avi,0,sizeof(AVISTREAMINFO));
	
	avi.fccType    = streamtypeVIDEO;
	avi.fccHandler = compHandler;
	avi.dwScale    = 1;
	avi.dwRate     = GetFrameRate();
	avi.dwQuality  = qFactor;
	avi.dwLength   = fbi->NumberFrames();

	res = AVIFileCreateStream(pfile, &pavi, &avi);

	if (res != AVIERR_OK) {
		res = ProcessImageIOError(&bi,GetString(IDS_BAD_FILE));
		if(res != BMMRES_RETRY)
			CleanUp();
		return (BMMRES)res;
	}

	//-- Initialize Compression --------------------------
	
	memset(&compOptions, 0, sizeof(AVICOMPRESSOPTIONS));

	compOptions.dwFlags         = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES;
	compOptions.fccType         = streamtypeVIDEO;
	compOptions.fccHandler      = compHandler;
	compOptions.dwQuality       = qFactor;
	compOptions.dwKeyFrameEvery = lKeys;
	
	//-- Compressor Specific Data ------------------------
	
	if (cpdata && data.cpdatasize) {
		compOptions.lpParms = cpdata;
		compOptions.cbParms = data.cpdatasize;
	}
    
	res = AVIMakeCompressedStream(&pcomp, pavi, &compOptions, NULL); 

	if (res != AVIERR_OK) {
		res = ProcessImageIOError(&bi,GetString(IDS_BAD_FILE));
		if(res != BMMRES_RETRY)
			CleanUp();
		return (BMMRES)res;
	}

	//-- Save Image Info Data

	bi.CopyImageInfo(fbi);    
	bi.SetUpdateWindow(fbi->GetUpdateWindow());

	this->map   = map;
	openMode    = BMM_OPEN_W;
	
	return(BMMRES_SUCCESS);

}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::Write()
//
//

BMMRES BitmapIO_AVI::Write(int frame) {
	
	HRESULT res;

	//-- If we haven't gone through an OpenOutput(), leave

	if (openMode != BMM_OPEN_W)
		return (ProcessImageIOError(&bi,BMMRES_INTERNALERROR));

	if (bi.GetUpdateWindow())
		SendMessage(bi.GetUpdateWindow(),BMM_PROGRESS,50,100);

	//-- Convert Bitmap to DIB ---------------------------
	
	PBITMAPINFO pbmi = GetOutputDib(24);

	if (!pbmi) {
		res = ProcessImageIOError(&bi,BMMRES_INTERNALERROR);
		if(res != BMMRES_RETRY)
			CleanUp();
		return (BMMRES)res;
	}
	
	//-- Initialize Format -------------------------------
	
	if (!formatInit) {
		res = AVIStreamSetFormat(pcomp, 0, pbmi, pbmi->bmiHeader.biSize);
		if (res != AVIERR_OK) {
			res = ProcessImageIOError(&bi,GetString(IDS_CREATE_ERROR));
			if(res != BMMRES_RETRY)
				CleanUp();
			return (BMMRES)res;
		}
		formatInit = TRUE;
	}

	//-- Write to AVI File -------------------------------
	
	if (bi.GetUpdateWindow())
		SendMessage(bi.GetUpdateWindow(),BMM_PROGRESS,100,100);

	res = AVIStreamWrite(pcomp,aframe++,1,(LPBYTE) pbmi->bmiColors,
						pbmi->bmiHeader.biSizeImage,AVIIF_KEYFRAME,NULL, NULL);
	
	LocalFree(pbmi);
	
	if (res != AVIERR_OK) {
		res = ProcessImageIOError(&bi,GetString(IDS_STREAM_ERROR));
		if(res != BMMRES_RETRY)
			CleanUp();
		return (BMMRES)res;
	}

	return BMMRES_SUCCESS;

}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::Close()
//

int  BitmapIO_AVI::Close( int flag ) {

	CleanUp();
	return 1;

}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::LocalGetInfo()

BMMRES BitmapIO_AVI::LocalGetInfo ( AVISTREAMINFO *avi, BitmapInfo *fbi ) {

	//-- Open AVI File -----------------------------------
	
	HRESULT  res;

	AVIFileInit();
	initialized = TRUE;
	
	res = AVIFileOpen(&pfile, fbi->Name(), OF_SHARE_DENY_WRITE, 0L);
	
	if (res) {
	
		switch (res) {
		
			case AVIERR_BADFORMAT:
			case AVIERR_MEMORY:
			case AVIERR_FILEREAD:
			case AVIERR_FILEOPEN:
			case REGDB_E_CLASSNOTREG:

			;
			
		}
		
		//-- Can't Open File
		//-- Check for silent mode
		//-- Display Error Dialog Box
		//-- Log Error
	
		CleanUp();
		return BMMRES_ERRORTAKENCARE;
	
	}

	//-- Open Video Stream -------------------------------
	
	res = AVIFileGetStream(pfile, &pavi, streamtypeVIDEO, 0);
	
	if (res) {
	
		switch (res) {
		
			case AVIERR_NODATA:
			case AVIERR_MEMORY:

			;
			
		}
		
		//-- Can't Open File
		//-- Check for silent mode
		//-- Display Error Dialog Box
		//-- Log Error
	
		CleanUp();
		return BMMRES_ERRORTAKENCARE;
	
	}
	
	//-- Get Stream Info ---------------------------------
	
	res = AVIStreamInfo(pavi, avi, sizeof(AVISTREAMINFO));
	
	if (res) {
	
		switch (res) {
		
			case AVIERR_BADFORMAT:
			case AVIERR_MEMORY:
			case AVIERR_FILEREAD:
			case AVIERR_FILEOPEN:
			case REGDB_E_CLASSNOTREG:

			;
			
		}
		
		//-- Can't Open File
		//-- Check for silent mode
		//-- Display Error Dialog Box
		//-- Log Error
	
		CleanUp();
		return BMMRES_ERRORTAKENCARE;
	
	}
	
	CleanUp();
	return BMMRES_SUCCESS;

}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::ImageInfoDlg
//

BOOL BitmapIO_AVI::ImageInfoDlg(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {

	switch (message) {
	
		case WM_INITDIALOG: {

			  CenterWindow(hWnd,GetParent(hWnd));
			  SetCursor(LoadCursor(NULL,IDC_ARROW));

			  //-- Filename --------------------------------
			  
			  SetDlgItemText(hWnd, IDC_INFO_FILENAME, name);
		
			  //-- Handle Resolution -----------------------
		
			  int w = (avi.rcFrame.right-avi.rcFrame.left);
			  int h = (avi.rcFrame.bottom-avi.rcFrame.top);
			  char buf[64];
			  sprintf(buf,"%dx%d",w,h);
			  SetDlgItemText(hWnd, IDC_INFO_RESOLUTION, buf);

			  //-- Handle Type -----------------------------
		
			  // SetDlgItemText(hWnd, IDC_INFO_TYPE, avi.szName);
			  
			  //-- Handle Number of Frames -----------------
			  
			  sprintf(buf,"%d",avi.dwLength);
			  SetDlgItemText(hWnd, IDC_INFO_FRAMES, buf);

			  //-- Handle Quality --------------------------
			  
			  sprintf(buf,"%6.02f%%",(float)avi.dwQuality/100.0f);
			  SetDlgItemText(hWnd, IDC_INFO_QUALITY, buf);

			  //-- Handle Rate -----------------------------
			  
			  float r = ((float)avi.dwRate/(float)avi.dwScale) + 0.45f;
			  sprintf(buf,"%d fps",(int)r);
			  SetDlgItemText(hWnd, IDC_INFO_RATE, buf);

			  //-- Handle Date and Size -----------------------

			  char sizetxt[128];
			  char datetxt[128];

			  char na[32];
			  LoadString(hInst,IDS_NA,na,32);
			  _tcscpy(sizetxt,na);
			  _tcscpy(datetxt,na);

			  HANDLE findhandle;
			  WIN32_FIND_DATA file;
			  SYSTEMTIME t,l;
			  findhandle = FindFirstFile(name,&file);
			  FindClose(findhandle);
			  if (findhandle != INVALID_HANDLE_VALUE) {
				sprintf(sizetxt,"%d",file.nFileSizeLow);
				FileTimeToSystemTime(&file.ftLastWriteTime,&t);
				if(!SystemTimeToTzSpecificLocalTime(NULL,&t,&l))
					l = t;
				sprintf(datetxt,"%d/%02d/%02d %02d:%02d:%02d",l.wYear,
					l.wMonth,l.wDay,l.wHour,l.wMinute,l.wSecond);
			  }
		
			  SetDlgItemText(hWnd, IDC_INFO_DATE, datetxt);
			  SetDlgItemText(hWnd, IDC_INFO_SIZE, sizetxt);

			  return 1;
			  
		}

		case WM_COMMAND:

			  switch (LOWORD(wParam)) {

				  //-- Changes Accepted ---------------------

				  case IDCANCEL:
				  case IDOK:
						 EndDialog(hWnd,1);
						 break;
		
			  }
			  return 1;

	}
	return 0;

}

//-----------------------------------------------------------------------------
// *> ImageInfoDlgProc()
//

static BOOL CALLBACK InfoCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static BitmapIO_AVI *io = NULL;
	if (message == WM_INITDIALOG) 
		io = (BitmapIO_AVI *)lParam;
	if (io) 
		return (io->ImageInfoDlg(hWnd,message,wParam,lParam));
	else
		return(FALSE);
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::Control()

BMMRES BitmapIO_AVI::GetImageInfoDlg(HWND hWnd, BitmapInfo *fbi, const TCHAR *filename) {

	//-- Take care of local copy of BitmapInfo
	
	BitmapInfo *ibi = new BitmapInfo;
	
	if (!ibi)
		return(BMMRES_MEMORYERROR);
	
	ibi->Copy(fbi);

	//-- Prepare BitmapInfo if needed
	
	if (filename)
		ibi->SetName(filename);

	//-- Get File Info -------------------------
	
	BMMRES res = LocalGetInfo (&avi,ibi);
	
	delete ibi;
	
	if (res != BMMRES_SUCCESS)
		return (res);

	_tcscpy(name, fbi->Name());
	
	//-- Display Dialogue ----------------------
	
	DialogBoxParam(
		hInst,
		MAKEINTRESOURCE(IDD_AVI_INFO),
		hWnd,
		(DLGPROC)InfoCtrlDlgProc,
		(LPARAM)this);
		
	return (BMMRES_SUCCESS);
	
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::GetCfgFilename()
//

void BitmapIO_AVI::GetCfgFilename( TCHAR *filename ) {
	_tcscpy(filename,TheManager->GetDir(APP_PLUGCFG_DIR));
	int len = _tcslen(filename);
	if (len) {
		if (_tcscmp(&filename[len-1],_T("\\")))
			_tcscat(filename,_T("\\"));
	}   
	_tcscat(filename,AVICONFIGNAME);   
}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::ReadCfg()
//

BOOL BitmapIO_AVI::ReadCfg() {
	
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
// #> BitmapIO_AVI::WriteCfg()
//

void BitmapIO_AVI::WriteCfg() {
 
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

void KillProcess(HANDLE hProcess)
{
	DWORD exitCode;

	if(hProcess) {
		GetExitCodeProcess(hProcess, &exitCode);
		if(exitCode == STILL_ACTIVE) {
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}
	}
}

static HANDLE playProcess = NULL;

//-----------------------------------------------------------------------------
// *> PlayShellFile()
//
// Play using explorer's default viewer
BOOL PlayShellFile(const TCHAR *filename) {
	SHELLEXECUTEINFO pShellExecInfo;

	if(playProcess)
		KillProcess(playProcess);
	
	playProcess = NULL;

	pShellExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	pShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	pShellExecInfo.hwnd = NULL;
	pShellExecInfo.lpVerb = "play";
	pShellExecInfo.lpFile = filename;
	pShellExecInfo.lpParameters = NULL;
	pShellExecInfo.lpDirectory = NULL;
	pShellExecInfo.nShow = SW_SHOWNORMAL;

	BOOL bRetVal = ShellExecuteEx(&pShellExecInfo);
	if (bRetVal) {
		playProcess = pShellExecInfo.hProcess;
		}

	return bRetVal;
}

//-----------------------------------------------------------------------------
// *> PlayFile()
//
BOOL PlayFile ( TCHAR *cmd ) {

	PROCESS_INFORMATION process;
	STARTUPINFO	si;
	BOOL			res =	FALSE;

	//-- Startup Info	Structure --------------------------

	memset(&si,0,sizeof(STARTUPINFO));
	
	si.cb				= sizeof(STARTUPINFO);
	si.dwFlags		= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow	= SW_SHOWDEFAULT;
	si.hStdError	= NULL;

	if(playProcess)
		KillProcess(playProcess);
	
	playProcess = NULL;
	
	//-- Start the	Thing	---------------------------------

	res =	CreateProcess(
		  (LPCTSTR)	NULL,							  //-- Pointer	to	name of executable module 
		  (LPTSTR)	cmd,  						  //-- Pointer	to	command line string
		  (LPSECURITY_ATTRIBUTES)	NULL,		  //-- Pointer	to	process security attributes 
		  (LPSECURITY_ATTRIBUTES)	NULL,		  //-- Pointer	to	thread security attributes	
		  FALSE,										  //-- Handle inheritance flag 
		  (DWORD)0,									  //-- Creation flags 
		  (LPVOID)NULL,							  //-- Pointer	to	new environment block 
		  (LPCTSTR)NULL,							  //-- Pointer	to	current directory	name 
		  (LPSTARTUPINFO)&si,					  //-- Pointer	to	STARTUPINFO	
		  (LPPROCESS_INFORMATION)&process	  //-- Pointer	to	PROCESS_INFORMATION	
	);		 
	
	if	(!res)
		return (FALSE);
	
	CloseHandle(process.hThread);

	// NO, Don't Wait!!!
	// MAX won't be able to process any messages while this is waiting!!
#if 0
	//-- Wait for process to finish ----------------------
	DWORD			exitcode;

	exitcode	= WaitForSingleObject(process.hProcess,INFINITE);
	CloseHandle(process.hProcess);
#else
	playProcess = process.hProcess;
#endif
		
	return (TRUE);

}

//-----------------------------------------------------------------------------
// #> BitmapIO_AVI::ShowImage()
//

BOOL BitmapIO_AVI::ShowImage( HWND hWnd, BitmapInfo *lbi ) {
	if (!BMMGetFullFilename(lbi))
		return FALSE;

	TCHAR	cmd[512];

	if (!PlayShellFile(lbi->Name())) {
		wsprintf(cmd,_T("mplay32.exe \"%s\""),lbi->Name());
		if (!PlayFile(cmd)) {
			wsprintf(cmd,_T("mplayer.exe \"%s\""),lbi->Name());
			if(!PlayFile(cmd))
				return FALSE;
			}
		}

	return TRUE;
}

//-- EOF: avi.cpp -------------------------------------------------------------
