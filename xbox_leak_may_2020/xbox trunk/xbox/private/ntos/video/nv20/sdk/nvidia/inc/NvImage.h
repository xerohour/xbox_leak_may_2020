/***************************************************************************\
* Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
*  Portions: Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.*
*                                                                           *
* Module: NvImage.h                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Andrei Osnovich    10/03/2000  Created                              *
*                                                                           *
\***************************************************************************/

#ifndef _NVIMAGE_H_
#define _NVIMAGE_H_

#ifdef  DEFINE_JPEG_TYPES
#include <stdio.h>
#include <string.h>
#include "jpeglib.h"
#else   //~DEFINE_JPEG_TYPES
struct jpeg_decompress_struct {
    DWORD Pan;
};
typedef BYTE JSAMPLE;
#endif  //~DEFINE_JPEG_TYPES

typedef void (__stdcall *SCANLINEPROCESS)(void *, int);

typedef enum _JPEGParameterType_ {
    JPEG_WRITE_QUALITY      = 1,
    JPEG_READ_DEPTH         = 2,
} JPEGPARAMETERTYPE;

#ifdef __cplusplus

typedef class CImageObject
{
    /*
     * methods
     */
public:
    virtual void WINAPI CompressJPEG (void *pAddr, DWORD dwWidth, DWORD dwHeight, char * filename);
    virtual void WINAPI GetJPEGInfoFile (char * filename, DWORD *pWidth, DWORD *pHeight, DWORD *pBPP);
    virtual void WINAPI GetJPEGInfo (char *pcBuffer, int nLength, DWORD *pWidth, DWORD *pHeight, DWORD *pBPP);
    virtual DWORD WINAPI SetJPEGParameters (JPEGPARAMETERTYPE type, DWORD value);
    virtual void WINAPI DecompressJPEGFile (char * filename, SCANLINEPROCESS ScanlineProcess = NULL);
		virtual void WINAPI DecompressJPEG (char *pcBuffer, int nLength, SCANLINEPROCESS ScanlineProcess = NULL);
    virtual void WINAPI GetImageParameters (char *&Start, char *&Current, DWORD &Width, DWORD &Height, DWORD &BPP);
    virtual DWORD WINAPI DestroyImageObject(void);
    CImageObject();
    ~CImageObject();
protected:
    void write_JPEG_file (char * filename, int quality);
    int  get_JPEG_file_info (char * filename, struct jpeg_decompress_struct *pCinfo);
    int  get_JPEG_buffer_info (char *pBuffer, int nLength, struct jpeg_decompress_struct *pCinfo);
    int  read_JPEG_file (char * filename, SCANLINEPROCESS ScanlineProcess);
		int  read_JPEG_buffer (char *pBuffer, int nLength, SCANLINEPROCESS ScanlineProcess);
    void __stdcall InternalScanlineProcess(LPVOID pBuf, int stride);
protected:
    int       out_quality;          /* Specify quality of the subsequent compression 0-100, 100 being highest. */ 
    JSAMPLE * out_image_buffer;	    /* Points to large array of R,G,B-order data */
    int       out_image_height;	    /* Number of rows in image */
    int       out_image_width;		/* Number of columns in image */
    char     *image_buffer;
    char     *image_ptr;
    DWORD     image_width; 
    DWORD     image_height; 
    DWORD     image_depth;  
} CIMAGEOBJECT, *LPCIMAGEOBJECT;


typedef DWORD (WINAPI *CREATEIMAGEOBJECT)(LPCIMAGEOBJECT &pImageOpject);
extern DWORD WINAPI CreateImageObject(LPCIMAGEOBJECT &pImageOpject);

#else   //__cplusplus==0

typedef struct CImageObject
{
    struct  CImageObjectVtbl FAR *lpVtbl;
} CIMAGEOBJECT, *LPCIMAGEOBJECT;

struct  CImageObjectVtbl {
    void    (WINAPI *CompressJPEG)(LPCIMAGEOBJECT, void *pAddr, DWORD dwWidth, DWORD dwHeight, char * filename);
    void    (WINAPI *GetJPEGInfoFile)(LPCIMAGEOBJECT, char * filename, DWORD *pWidth, DWORD *pHeight, DWORD *pBPP);
    void    (WINAPI *GetJPEGInfo)(LPCIMAGEOBJECT, char *pcBuffer, int nLength, DWORD *pWidth, DWORD *pHeight, DWORD *pBPP);
    DWORD   (WINAPI *SetJPEGParameters)(LPCIMAGEOBJECT, JPEGPARAMETERTYPE type, DWORD value);
    void    (WINAPI *DecompressJPEGFile)(LPCIMAGEOBJECT, char * filename, SCANLINEPROCESS ScanlineProcess);
    void    (WINAPI *DecompressJPEG)(LPCIMAGEOBJECT, char *pcBuffer, int nLength, SCANLINEPROCESS ScanlineProcess);
    void    (WINAPI *GetImageParameters)(LPCIMAGEOBJECT, char **Start, char **Current, DWORD *Width, DWORD *Height, DWORD *BPP);
    DWORD   (WINAPI *DestroyImageObject)(LPCIMAGEOBJECT);
};


typedef DWORD (WINAPI *CREATEIMAGEOBJECT)(LPCIMAGEOBJECT *pImageOpject);
extern DWORD WINAPI CreateImageObject(LPCIMAGEOBJECT *pImageOpject);

#endif  //__cplusplus==0

#endif _NVIMAGE_H_

