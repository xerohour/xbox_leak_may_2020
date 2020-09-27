//---------------------------------------------------------------------------
// CmdMnPar.h (Common Main Parameters)
//---------------------------------------------------------------------------
// Structure holding parameters for VStudioMain
// created in Stub (VStudio.exe), passed to VStudioMain()
// NOTE: This is a specialized structure with a specific use
//---------------------------------------------------------------------------
// Copyright (c) 1989-1997, Microsoft Corporation
//                 All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
//---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Comments to Other Stubs:
//
// Stub Basics:
//   All stubs should at least link to MsEnv.dll, fill out a MAINPARAM struct
//   and call VStudioMain(MAINPARAM* pMainParam);
//
// Explanation of MAINPARAM:
//   hinstExe, lpszCmdLine, and cmdShow are the same as the WinMain() 
//   counterparts
//
// Splash screens:
//   It is the stub's responsibility to create a splash screen, and 
//   provide a function to destroy it (pointed to by pfnDestroySS)
//
//   If no splash screen exists, set pfnDestroySS = NULL
//
//   If there is a splash screen, it must be created before the call to
//   VStudioMain() and MsEnv.dll will dereference pfnDestroySS (checks NULL)
//   to destroy the splash screen. Note that MsEnv.dll has no knowledge of 
//   the actual implementation of a SplashScreen.
//
//   If you want a splash screen with little bitmaps for all the installed
//   products, when you're doing /setup, fill in lpszSplashFileName, lpszAppDataPath,
//   hInstSplashExe, and uiIdSplashBmp
// ---------------------------------------------------------------------------
    
#ifndef _CMNMNPAR_H_       
#define _CMNMNPAR_H_

// nonstandard extension used : nameless struct/union
// Unreferenced inline function has been removed
#pragma warning( disable : 4201 ) 
#pragma warning( disable : 4514) 

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "Windows.h"

//----------------------------------------------------------------------------
// Call back functions
//----------------------------------------------------------------------------
typedef void (__stdcall *LPFNDetroySplashScrn)();

//----------------------------------------------------------------------------
// Common Structures
//----------------------------------------------------------------------------

#define PI_CCHMAX_LICENSE_NAME          53
#define PI_CCHMAX_LICENSE_COMPANY       53
#define PI_CCHMAX_SERIAL_NUMBER         24

//----------------------------------------------------------------------------
// Splash Screen
struct SplshText
  {
  char   szUserName[PI_CCHMAX_LICENSE_NAME];
  char   szOrgName[PI_CCHMAX_LICENSE_COMPANY];
  char   szLicensedTo[256];
  char   szCopyRight[256];
  char   szMemberOf[256];
  char   szVisualStudio[256];
  char   szBottomLine1[256];
  char   szBottomLine2[256];
  //char   szVers32[256];
  char   szEditionStd[128]; // Localized "Standard Edition" String
  char   szEditionPro[128]; // Localized "Professional Edition" String
  char   szEditionEnt[128]; // Localized "Enterprise Developer" String
  char   szEditionEntArch[128]; // Localized "Enterprise Architect" String
  char   szEditionTrial[128]; // Localized "Trial" String
  char   szEditionAcademicPro[128]; // Localized "Academic Edition" String
  char   szVSIPRequired[256];
  char   szBufferForOfficeCompatibility[200];
  char   szAlpha2[256]; 
  BOOL   fAlpha;
  WORD   wFontsize;
  BYTE	 bCharset;
  char	 szFontname[LF_FACESIZE];
  char   szVSFamily[256];
  };

//----------------------------------------------------------------------------
// Enum for cmdline options
enum CmdOptions
  {
  cmdOptInvalid=0,		    // invalid/not set, must be first
  cmdOptLoad,					    // load file - default
  cmdOptMake,					    // make file (build)
  cmdOptClean,				    // clean build
  cmdOptRebuild,			    // rebuild
  cmdOptDeploy,				    // deployment
  cmdOptDebug,				    // debug build
  cmdOptRetail,				    // retail build
  cmdOptDebugExe			    // use only with /debugexe switch
  };

//----------------------------------------------------------------------------
// Struct for CmdUIGuids 
struct CmdUIGuidsInit
  {
  CLSID                 clsid;              // cmd ui context guid
  BOOL                  fActive;            // state of context guid
  CmdUIGuidsInit	*pNext;		    // next filename & options
  };

//----------------------------------------------------------------------------
// Struct for file names
struct CmdLineFiles
  {
  PWSTR		wszFile;		    // file name
  CmdOptions	eCmdOption;		    // option to perform on this file
  BOOL		fOptionSet;		    // option has been set
  BOOL		fDebug;			    // debug|retail mode - def retail
  PWSTR		wszDeploy;		    // deployment map name 
  void		*pVoid;			    // void ptr, used for IVSHier ref
  CmdLineFiles	*pNext;			    // next filename & options
  };

//----------------------------------------------------------------------------
// Main Common Struct
struct MAINPARAM
  {
  int			cbSize;		    // must = sizeof(MAINPARAM);
  HINSTANCE		hinstExe;	    // hInstance from LoadLibrary	
  PWSTR			wszCmdLine;	    // full cmd line
  int			cmdShow;	    // Windows show mode
  LPFNDetroySplashScrn  pfnDestroySS;	    // ptr to func
  BOOL			fOfficeMode;	    // VSE/Office mode
  CmdUIGuidsInit	*pCmdUIGuids;	    // initial cmdui guids
  PWSTR			wszMSODLL;         // full path to specific MSOx DLL, leave NULL for default behavior

  //---------------------------------------------------------------------------
  // Command Line Options handled by Shell
  //---------------------------------------------------------------------------
  PWSTR			wszCmdLineCopy;	    // copy of cmd line to manipulate
  CmdLineFiles		*pFiles;	    // names of files 
  BOOL			fMake;		    // /m or /make specified
  BOOL			fDeploy;	    // /deploy specified
  BOOL			fRebuild;	    // /rebuild specified
  BOOL			fClean;		    // /clean specified
  BOOL			fOut;		    // /out specified
  PWSTR			wszOut;	    // log file name
  BOOL			fMdi;		    // Mdi 
  BOOL			fSdi;		    // Sdi
  BOOL			fEZMdi;		    // EZMdi
  BOOL			fRun;		    // run
  BOOL			fRunExit;	    // runexit
  PWSTR			wszLcid;	    // language to be used
  PWSTR			wszFontName;	    // font name to use
  PWSTR			wszFontSize;	    // font size in points
  BOOL			fHelp;		    // display help dialog
  BOOL			fSelfDebug;	    // make devenv debuggabl through devenv
  
  BOOL			fDebugExe;        // /debugexe specified on commandline (all subsequent args                          
  PWSTR			wszDebugExeArgs; //  are then stored in lpszDebugExeArgs

  //---------------------------------------------------------------------------
  // Undocumented cmd line options
  //---------------------------------------------------------------------------
  BOOL			fSetup;		    // setup mode
  BOOL			fUnRegSvr;	    // regsvr mode
  BOOL			fEmbedding;	    // embedding
  BOOL			fJitDebug;	    // jit debug
  BOOL			fDDEExec;	    // DDE exec pending (started via file association)
  PWSTR			wszRegistryRoot;   // registry root

  //---------------------------------------------------------------------------
  // Debugging cmd line options (needed in retail mode as well)
  //---------------------------------------------------------------------------
  BOOL			fScriptFile;	    // debug scriptfile
  PWSTR			wszScriptFile;	    // filename
  BOOL			fScriptOut;	    // debug scriptfile out
  PWSTR			wszScriptOut;	    // filename
  PWSTR			wszTimingOut;	    // timing data filename
  BOOL			fStep;		    // step mode
  PWSTR			wszProfilePath;    // directory for profile files

  GUID			guidRegisterAppObjectAs;
  GUID			guidRegisterSolutionAs;

  //---------------------------------------------------------------------------
  // IVsAppCommandLine
  //---------------------------------------------------------------------------
  IUnknown * pAppCmdLine;   // actually IVsAppCmdLine

  //---------------------------------------------------------------------------
  // build cmd line options
  //---------------------------------------------------------------------------
  PWSTR                 wszBuildCfg;       // cfg to build
  PWSTR                 wszBuildProj;      // project to build

  //---------------------------------------------------------------------------
  // Information used by msenv to create a new splash screen with little bitmaps
  // for all the installed products at setup time.  If lpszSplashFileName is NULL
  // it won't try to create the new splash screen.
  //---------------------------------------------------------------------------
  PWSTR                 wszSplashFileName;  //file name (not path!) to be created, should sound boring like vs00023.tmp
  PWSTR                 wszAppDataPath;     //file will be put here
  HINSTANCE             hInstSplashExe;      //executable with base splash screen resource
  UINT                  uiIdSplashBmp;       //resource id of the base splash screen

  PWSTR                 wszBuildProjCfg;     // project cfg to build
  BOOL                  fBuildProj;          // /project specified
  BOOL                  fBuildProjCfg;       // /projectconfig specified

  //---------------------------------------------------------------------------
  // Performance logging options
  //---------------------------------------------------------------------------
  DWORD                 dwTickCountProcessStart;    // Tick count of process start (or as close as we can get)
  };

//----------------------------------------------------------------------------
// format for VStudioMain and VStudioTerm
//----------------------------------------------------------------------------
typedef int     (__cdecl *LPFuncVStudioMain)(const MAINPARAM *);
typedef HRESULT (__cdecl *LPFuncVStudioTerm)(DWORD);

#endif // _CMNMNPAR_H_
