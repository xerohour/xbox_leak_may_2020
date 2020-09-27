//-----------------------------------------------------------------------------
// ----------------
// File ....: avi.h
// ----------------
// Author...: Gus J Grubba
// Date ....: November 1995
// Descr....: AVI File I/O Module
//
// History .: Nov, 10 1995 - Started
//            
//-----------------------------------------------------------------------------
        
#ifndef _AVICLASS_
#define _AVICLASS_

#define DLLEXPORT __declspec(dllexport)

#define AVIVERSION     100
#define AVICONFIGNAME _T("avi.cfg")

//-----------------------------------------------------------------------------
//-- Configuration Block ------------------------------------------------------
//

typedef struct tagAVIDATA {
     DWORD    version;
     BOOL       saved;
     COMPVARS      cv;
     int        depth;
     DWORD cpdatasize;
} AVIDATA;

//-----------------------------------------------------------------------------
//-- Class Definition ---------------------------------------------------------
//

class BitmapIO_AVI : public BitmapIO {
    
     private:
     
        void              *cpdata;
        AVIDATA            data;
        PAVISTREAM         pavi;
        PAVISTREAM         pcomp;
        PAVIFILE           pfile;
        AVISTREAMINFO      avi;
        PGETFRAME          getframe;
        TCHAR              name[MAX_PATH];
        BOOL               initialized,formatInit;
        AVICOMPRESSOPTIONS compOptions;
        int	             aframe;

        //-- Private Functions
        
        BMMRES         LocalGetInfo       ( AVISTREAMINFO *avi, BitmapInfo *bi);
        void           CleanUp            ( );
        void           GetCfgFilename     ( TCHAR *filename );
        BOOL           ReadCfg            ( );
        void           WriteCfg           ( );
        
     public:
     
        //-- Constructors/Destructors
        
                       BitmapIO_AVI       ( );
                      ~BitmapIO_AVI       ( );
               
        //-- Number of extemsions supported
        
        int            ExtCount           ( )       { return 1;}
        
        //-- Extension #n (i.e. "3DS")
        
        const TCHAR   *Ext                ( int n ) {return _T("avi");}
        
        //-- Descriptions
        
        const TCHAR   *LongDesc           ( );
        const TCHAR   *ShortDesc          ( );

        //-- Miscelaneous Messages
        
        const TCHAR   *AuthorName         ( )       { return _T("Gus J Grubba");}
        const TCHAR   *CopyrightMessage   ( )       { return _T("Copyright 1995, Yost Group");}
        
        unsigned int   Version            ( )       { return (AVIVERSION);}

        //-- Driver capabilities
        
        int            Capability         ( )       { return BMMIO_READER     | 
                                                             BMMIO_WRITER     | 
                                                             BMMIO_EXTENSION  |
                                                             BMMIO_INFODLG    |
                                                             BMMIO_OWN_VIEWER |
															  BMMIO_UNINTERRUPTIBLE |
															  BMMIO_MULTIFRAME |
															  BMMIO_NON_CONCURRENT_ACCESS |
                                                             BMMIO_CONTROLWRITE; }
        
        //-- Driver Configuration
        
        BOOL           LoadConfigure      ( void *ptr );
        BOOL           SaveConfigure      ( void *ptr );
        DWORD          EvaluateConfigure  ( );
        
        //-- Show DLL's "About..." box
        
        void           ShowAbout          ( HWND hWnd );  

        //-- Play AVI File
        
        BOOL           ShowImage          ( HWND hWnd, BitmapInfo *bi );

        //-- Show DLL's Control Panel
        
        BOOL           ShowControl        ( HWND hWnd, DWORD flag );

        //-- Return info about image
        
        BMMRES         GetImageInfoDlg    ( HWND hWnd, BitmapInfo *fbi, const TCHAR *filename);
        BMMRES         GetImageInfo       ( BitmapInfo *fbi );        

        //-- Image Input
        
        BitmapStorage *Load               ( BitmapInfo *fbi, Bitmap *map, BMMRES *status);

        //-- Image Output
        
        BMMRES         OpenOutput         ( BitmapInfo *fbi, Bitmap *map );
        BMMRES         Write              ( int frame );
        int            Close              ( int flag );
		PAVIFILE			GetPaviFile        ( ) { return pfile; }

        //-- Dialog Functions
        
        BOOL           ImageInfoDlg       ( HWND,UINT,WPARAM,LPARAM );
        BOOL           ControlDlg         ( HWND,UINT,WPARAM,LPARAM );
        
};

#endif

//-- EOF: avi.h ---------------------------------------------------------------
