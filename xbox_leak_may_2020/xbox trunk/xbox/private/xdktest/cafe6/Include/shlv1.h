/////////////////////////////////////////////////////////////////////////////
// shlv1.h
//
// email	date		change
// briancr	12/05/94	created
//
// copyright 1994 Microsoft

// Backward compatibility

#ifndef __SHLV1_H__
#define __SHLV1_H__

#include "..\support\guitools\ide\shl\uwbframe.h"
#include "..\support\guitools\ide\shl\cowrkspc.h"

#include "shlxprt.h"

#pragma warning(disable: 4091)

#define LANG_JAPAN 1000

#define MAX_ATOM_LENGTH 256

//#define XSAFETY ((CVCTools*)m_pToolset)->GetIDE()->IsActive()
#define XSAFETY (((CIDESubSuite*)GetSubSuite())->GetIDE()->IsActive())

SHL_API LPCSTR CAFEv1GetLocString(UINT id, int nOffset = -1);

SHL_API struct LOCSTR_STRUCT
{
	UINT id;			// IDSS_FOO
	LPCSTR string;		// "localized 'foo'"
};

extern SHL_DATA LOCSTR_STRUCT m_aLocStr[];
extern SHL_DATA LOCSTR_STRUCT m_aLocStrJ[];

#endif // __SHLV1_H__
