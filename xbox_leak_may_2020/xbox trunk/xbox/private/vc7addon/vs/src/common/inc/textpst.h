// textpst.h - Text file persistence
// Part of Unilib.
//------------------------------------------------------------------------------
// Copyright (c) 1998-1999, Microsoft Corporation, All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
// Author: Paul Chase Dempsey [paulde]
//------------------------------------------------------------------------------
#pragma once
#include "vsmem.h"
#include "textmgr.h"    // IVsPersistentTextImage
#include "fpstfmt.h"    // Additional STG_E_... and STG_I_...HRESULTs
#include "unipriv.h"    // UASSERT
#include "commacro.h"
#include "txfmt.h"

// !!!! WARNING !!!!
//
// If you load HTML or XML (anything that requires encoding/charset tag lookup),
// make sure to call FreeCharsetResources() from encoding.h during app termination.
//
// !!!! WARNING !!!!


//------------------------------------------------------------------------------
//
HRESULT WINAPI LoadTextImageFromMemory (IVsPersistentTextImage * pImage,
                                        DWORD                    cbData,
                                        PCBYTE                   pData,
                                        LTI_INFO *               pInfo 
                                        );

//------------------------------------------------------------------------------
//
HRESULT WINAPI LoadTextImageFromFile   (IVsPersistentTextImage * pImage, 
                                        PCWSTR                   pszFileName, 
                                        LTI_INFO *               pInfo
                                        );

//------------------------------------------------------------------------------
//
HRESULT WINAPI SaveTextImageToMemory   (IVsPersistentTextImage * pImage,
                                        DWORD *                  pcbData, 
                                        BYTE *                   pData, 
                                        LTI_INFO *               pInfo
                                        );

//------------------------------------------------------------------------------
//
HRESULT WINAPI SaveTextImageToFile     (IVsPersistentTextImage * pImage,
                                        PCWSTR                   pszFileName, 
                                        LTI_INFO *               pInfo
                                        );

//------------------------------------------------------------------------------
// GetTextImageSize
//
// Get persistent size of image, according to LTI_INFO and fExact.
//
// When fExact is FALSE, returns the minimum size guaranteed to hold the image.
// When fExact is TRUE, returns the exact size of the image (can be somewhat 
// expensive to calculate if not Unicode).
//
HRESULT WINAPI GetTextImageSize (IVsPersistentTextImage * pImage, 
                                 LTI_INFO *               pInfo, 
                                 DWORD *                  pdwSize, 
                                 BOOL                     fExact = FALSE
                                 );

// This goo makes VsHeapAlloc happy
#ifdef _DEBUG
#define MY_DEFAULT_HEAP (HANDLE)(-1)
#else
#define MY_DEFAULT_HEAP GetProcessHeap()
#endif

//------------------------------------------------------------------------------
// CMinPersistentTextImage - Minimal implementation of IVsPersistentTextImage
//
// If you don't have text buffer or storage available, you can use this minimal
// implementation with these functions.
//
// In the constructor, you can tell it to use a specific heap, and optionally
// own the heap and destroy it on final release.
//
class CMinPersistentTextImage : public IVsPersistentTextImage
{
public:
    CMinPersistentTextImage(HANDLE heap = 0, BOOL fOwnHeap = FALSE);
    ~CMinPersistentTextImage();

    // IUnknown
    //
    STDMETHODIMP            QueryInterface (REFIID riid, LPVOID *ppObj);
    STDMETHODIMP_(ULONG)    AddRef  ();
    STDMETHODIMP_(ULONG)    Release ();

    // IVsPersistentTextImage
    //
    STDMETHODIMP_(void *)   Image_Alloc   (int iSize);
    STDMETHODIMP_(void *)   Image_Realloc (void *p, int iSize);
    STDMETHODIMP_(void)     Image_Free    (void *p);
    STDMETHODIMP_(long)     Image_GetLength (void);
    STDMETHODIMP            Image_LoadText  (const WCHAR *pszText, INT iLength, DWORD dwFlags);
    // FullTextScan-style methods
    STDMETHODIMP            Image_OpenFullTextScan (void);
    STDMETHODIMP            Image_FullTextRead (const WCHAR **ppszText, long *piLength);
    STDMETHODIMP            Image_CloseFullTextScan (void);

private:
    ULONG   m_cRef;
    PCWSTR  m_pszText;
    INT     m_iLength;
    HANDLE  m_heap;
    BOOL    m_fOwnHeap;
};
