//
// encerr.cpp
//
// Error message mapping for Edit and Continue
//

#include "stdafx.h"
#pragma hdrstop

#include "dbgenc.h"
#define assert ASSERT
#if defined(_DEBUG)
#define Debug(x)    x
#else
#define Debug(x)
#endif

#include "ref.h"
#include "simparray.h"
#include "enc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

typedef struct ERRTORES {
	ushort   errornum;
	UINT	 idResource;
} ERRTORES;

#define ENCERR(name)	{ name, IDS_##name },

// Edit and Continue error map
const static ERRTORES mpErrToRes[] = {
	ENCERR(Enc_Err_UnsupportedEdit)
	ENCERR(Enc_Err_CantInitialize)
	ENCERR(Enc_Err_ReadOnly)
	ENCERR(Enc_Err_CantCreateFile)
	ENCERR(Enc_Err_ExePath)
	ENCERR(Enc_Err_LinkedFromLib)
	ENCERR(Enc_Err_CantFindObj)
	ENCERR(Enc_Err_ObjMismatch)
	ENCERR(Enc_Err_IgnoringFile)
	ENCERR(Enc_Err_CmdNotExecuted)
	ENCERR(Enc_Err_CantUpdateFrame)
	ENCERR(Enc_Err_FrameFailed)
	ENCERR(Enc_Err_StackWalk)
	ENCERR(Enc_Err_TosEdited)
	ENCERR(Enc_Err_FrameEdited)
	ENCERR(Enc_Err_TooManyFiles)
};
#define MSGCNT	(sizeof (mpErrToRes)/sizeof (ERRTORES))


#define SEERR(name)		IDS_Enc_##name,
#define SEERR_CNT (LAST_SE_ERROR - FIRST_SE_ERROR + 1)

// Stack Editing error map
const static UINT mpSEErrToRes[ SEERR_CNT ] = {
	// These should be defined sequentially
	// in the order they appear in enc.h
	SEERR(E_SPNOTCANONICAL)
	SEERR(E_EXCEPTIONSTATE)
	SEERR(E_LOCALVARCHANGE)
	SEERR(E_IPNOTINEDIT)
	SEERR(E_FUNCSIGCHANGE)
	SEERR(E_FRAMESIZECHANGE)
	SEERR(E_LOCALREQCONDES)
	SEERR(E_UNSAFEFRAMECOND)
	SEERR(E_CANTFINDLINE)
	SEERR(E_DUPLICATEVAR)
	SEERR(E_FUNCBYTEMATCH)
	SEERR(E_REGISTERSPILL)
};


// Edit and Continue error text
LPCTSTR EncErrorText(CString& str, EncError encErr)
{
	for (int i=0; i < MSGCNT; i++) {
		if (mpErrToRes[ i ].errornum == encErr ) {
			if ( str.LoadString( mpErrToRes[ i ].idResource ) ) {
				return str;
			}
		}
	}
	ASSERT(FALSE);
	return NULL;
}


// Stack editing error text
LPCTSTR SeErrorText(CString& str, HRESULT hResult)
{
	int i = hResult - FIRST_SE_ERROR;
	if (i >= 0 && i < SEERR_CNT) {
		str.LoadString(mpSEErrToRes[ i ]);
		return str;
	}
	ASSERT(FALSE);
	return NULL;
}
			
