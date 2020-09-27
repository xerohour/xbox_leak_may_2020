// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B05A63F4_9900_4362_9E2A_2C8E360B3B70__INCLUDED_)
#define AFX_STDAFX_H__B05A63F4_9900_4362_9E2A_2C8E360B3B70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import <msxml4.dll> raw_interfaces_only 
using namespace MSXML2;

#include <stdio.h>

#ifndef DECLSPEC_SELECTANY
#if (_MSC_VER >= 1100)
#define DECLSPEC_SELECTANY  __declspec(selectany)
#else
#define DECLSPEC_SELECTANY
#endif
#endif

#ifndef CONST
#define CONST const
#endif

#include <d3d8-xbox.h>
#include <d3d8types-xbox.h>

#include "XBContent.h"

// Bundler class types
#include "texture.h"
#include "vb.h"
#include "indexbuffer.h"
#include "pushbuffer.h"
#include "CVertexShader.h"
#include "userdata.h"

#include "effect.h"
#include "frame.h"
#include "anim.h"
#include "xbrc.h"

#ifndef ASSERT
#define ASSERT(X) { if (!(X)) { __asm int 3 } }
#endif

//////////////////////////////////////////////////////////////////////
// Returns true if the name matches the token
//
inline bool Match(CONST WCHAR *pToken, CONST WCHAR *pName, INT cchName)
{
	return (pToken[0] == pName[0])
		&& _wcsnicmp(pToken, pName, cchName) == 0
		&& wcslen(pToken) == (UINT)cchName;
}


//////////////////////////////////////////////////////////////////////
// Set quaternion keys to match axis/angle rotation.
//
HRESULT MatchAxisAngleCurve(AnimRotate *pRotate, CONST D3DXVECTOR3 *pvAxis, CONST AnimCurve *pAngle, FLOAT feps);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B05A63F4_9900_4362_9E2A_2C8E360B3B70__INCLUDED_)
