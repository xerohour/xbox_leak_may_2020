/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: MATRIX.H                                                          *
*   Matrix private interface                                                *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 07/07/98 - Created                      *
*                                                                           *
\***************************************************************************/
#ifndef _matrix_h
#define _matrix_h

struct UCL_MATRIX : public NVUCL_MATRIX
{
    /*
     * exported
     */
public:
    NVUCL_RESULT NVUCL_STDCALL attach        (void);
    NVUCL_RESULT NVUCL_STDCALL detach        (void);

    NVUCL_RESULT NVUCL_STDCALL setStore      (float *pfAddress);
    NVUCL_RESULT NVUCL_STDCALL setUnit       (void);

    NVUCL_RESULT NVUCL_STDCALL setElement    (DWORD dwIndex,float  fValue);
    NVUCL_RESULT NVUCL_STDCALL setRow        (DWORD dwRow,  float *pfValue);
    NVUCL_RESULT NVUCL_STDCALL setAll        (float *pfValue);

    NVUCL_RESULT NVUCL_STDCALL getElement    (DWORD dwIndex,float *pfValue);
    NVUCL_RESULT NVUCL_STDCALL getRow        (DWORD dwRow,  float *pfValue);
    NVUCL_RESULT NVUCL_STDCALL getAll        (float *pfValue);

    NVUCL_RESULT NVUCL_STDCALL multiply      (void *pMatrix);
    NVUCL_RESULT NVUCL_STDCALL invert        (void);

    /*
     * private
     */
protected:
    /*
     * variables
     */
    float  m_fInternalStorage[16];
    float *m_fElement;
    DWORD  m_dwRefCount;
    enum e_Class { unit,rotation,translation,threebyfour,general } m_eClass;

    /*
     * methods
     */
    void classify (void);

    /*
     * construction
     */
public:
    UCL_MATRIX (void);
};

#endif //_matrix_h