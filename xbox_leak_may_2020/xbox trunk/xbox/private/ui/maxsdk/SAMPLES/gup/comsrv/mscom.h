//-----------------------------------------------------------------------------
// ----------------
// File ....: mscom.h
// ----------------
// Author...: Gus J Grubba
// Date ....: October 1995
// Descr....: MSCOM File I/O Module
//
// History .: Oct, 26 1995 - Started
//            
//
//
//-----------------------------------------------------------------------------
        
#ifndef _MSCOMCLASS_
#define _MSCOMCLASS_

#define DLLEXPORT __declspec(dllexport)

#define _REGISTERCOM	_T("RegisterMAXRenderer")
#define _UNREGISTERCOM	_T("UnregisterMAXRenderer")

//-----------------------------------------------------------------------------
//-- Class Definition ---------------------------------------------------------
//


class GUP_MSCOM : public GUP {
    
	public:
     
		//-- Constructors/Destructors
        
				GUP_MSCOM		( );
				~GUP_MSCOM		( );
	
		//-- GUP Methods

		DWORD	Start			( );
		void	Stop			( );
		DWORD	Control			( DWORD parameter );

		//-- Private Control Methods (For Utility PlugIn Access)

		bool	IsCOMRegistered	( );
		bool	RegisterCOM		( );
		bool	UnRegisterCOM	( );

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// MSCOM Class Description

class mClassDesc:public ClassDesc {
	
	public:
												
		int             IsPublic     ( );
		void           *Create       ( BOOL );
		const TCHAR    *ClassName    ( );
		SClass_ID       SuperClassID ( );
		Class_ID        ClassID      ( );
		const TCHAR    *Category     ( );

};

#endif

//-- EOF: mscom.h -------------------------------------------------------------
