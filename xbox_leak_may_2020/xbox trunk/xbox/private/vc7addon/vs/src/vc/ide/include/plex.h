// (KPerry) This was the MFC collection stuff
// Now it has been purged of all other MFC influences and is a stand alone entity.

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __VCPLEX_H__
#define __VCPLEX_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef AFX_COLL_SEG
#pragma code_seg(AFX_COLL_SEG)
#endif

struct CVCPlex     // warning variable length structure
{
	CVCPlex* pNext;
#if (_AFX_PACKING >= 8)
	DWORD dwReserved[1];    // align on 8 byte boundary
#endif
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static CVCPlex* PASCAL Create(CVCPlex*& head, UINT_PTR nMax, UINT_PTR cbElement);
			// like 'calloc' but no zero fill
			// may throw memory exceptions

	void FreeDataChain();       // free this one and links
};

#ifdef AFX_COLL_SEG
#pragma code_seg()
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#endif //__AFXPLEX_H__

/////////////////////////////////////////////////////////////////////////////
