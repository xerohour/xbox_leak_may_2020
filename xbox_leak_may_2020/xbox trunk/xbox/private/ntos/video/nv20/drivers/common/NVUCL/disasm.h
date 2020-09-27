/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: DISASM.H                                                          *
*   Push buffer disassembler private interface                              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 07/21/98 - Created                      *
*                                                                           *
\***************************************************************************/
#ifndef _disasm_h
#define _disasm_h

struct UCL_DISASM : public NVUCL_DISASM
{
    /*
     * exported
     */
public:
    NVUCL_RESULT NVUCL_STDCALL attach        (void);
    NVUCL_RESULT NVUCL_STDCALL detach        (void);

    NVUCL_RESULT NVUCL_STDCALL redirect      (char* pszFilename);
    NVUCL_RESULT NVUCL_STDCALL traverse      (void* pBuffer,DWORD dwByteCount);

    /*
     * private
     */
protected:
    /*
     * variables
     */
    char  *m_pszFilename;
    DWORD  m_dwRefCount;
    BOOL   m_bToDebugger;
    FILE  *m_hFile;          // if NULL we go to debugger also

    /*
     * methods
     */
    void emit (char *szFormat,...);

    /*
     * construction
     */
public:
    UCL_DISASM (void);
    virtual ~UCL_DISASM (void);
};

#endif //_disasm_h