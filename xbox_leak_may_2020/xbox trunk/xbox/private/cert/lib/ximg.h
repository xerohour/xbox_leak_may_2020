/*
 *
 * ximg.cpp
 *
 * ximage.dat processing
 *
 */

#ifndef _XIMG_H
#define _XIMG_H

typedef BOOL (*PFN_HANDLEXBE)(int ilyr, CXBEFile *pxbe, DWORD cblk,
    CTapeDevice *ptapDst);
typedef BOOL (*PFN_HANDLEAP)(int ilyr, int ixph, CTapeDevice *ptapDst);
BOOL FProcessXImage(int ilyr, DXL *pdxl, int idxr, CTapeDevice *ptapSrc,
    CTapeDevice *ptapDst, PFN_HANDLEXBE pfnXbe, PFN_HANDLEAP pfnAP=NULL);

/* The following are expected to be provided by the caller */
CDiskFile *PstmCreateTemp(LPCSTR szName, DWORD dwFlags);
extern CCheckSum xsumBlk, xsumRed, xsumGrn, xsumMediaIn;
void MsgOut(LPCSTR sz, ...);

#endif // _XIMG_H
