/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: MATRIX.CPP                                                        *
*   Push buffer diassembler private implementation                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 07/21/98 - Created                      *
*                                                                           *
\***************************************************************************/
#include "precomp.h"
#include "disasm.h"

/*
 * attach
 *
 * bump reference count up
 */
NVUCL_RESULT NVUCL_STDCALL UCL_DISASM::attach
(
    void
)
{
    m_dwRefCount++;
    return NVUCL_OK;
}

/*
 * detach
 *
 * bump reference count down & delete object if it reaches zero
 */
NVUCL_RESULT NVUCL_STDCALL UCL_DISASM::detach
(
    void
)
{
    if (--m_dwRefCount)
    {
        return NVUCL_OK;
    }
    /*
     * kill self
     */
    delete this;
    return NVUCL_OK;
}

/*
 * redirect
 *
 * redirects output to the specified file
 */
NVUCL_RESULT NVUCL_STDCALL UCL_DISASM::redirect
(
    char* pszFilename
)
{
    /*
     * copy filename
     */
    m_pszFilename = strdup(pszFilename);

    /*
     * open file
     */
    m_hFile = fopen(pszFilename,"wt");
    if (!m_hFile)
    {
        /*
         * fail
         */
        return NVUCL_ERROR_ACCESSDENIED;
    }
    m_bToDebugger = FALSE;

    /*
     * banner
     */
    //todo

    /*
     * close file
     */
    fclose (m_hFile);
    m_hFile = 0;

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * traverse
 *
 * disassembles a push buffer
 */
NVUCL_RESULT NVUCL_STDCALL UCL_DISASM::traverse
(
    void* pBuffer,
    DWORD dwByteCount
)
{
    DWORD dwBuffer;

    /*
     * open file
     */
    if (!m_bToDebugger)
    {
        m_hFile = fopen(m_pszFilename,"a+t");
        if (!m_hFile)
        {
            /*
             * fail
             */
            return NVUCL_ERROR_ACCESSDENIED;
        }
    }

    emit ("PushBuffer %08x, ByteCount = %d\n",pBuffer,dwByteCount);

    dwBuffer = (DWORD)pBuffer;
    while (dwByteCount)
    {
        DWORD method  = *(DWORD*)dwBuffer;
        DWORD count   = (method & 0x00fc0000) >> 16;

        if (method & 0xff000000)
        {
            /*
             * not a method
             */
            emit ("   other: %08x - count=%d\n",method,count/4);

            /*
             * next
             */
            dwBuffer    += 4;
            dwByteCount -= 4;

            /*
             * show contents
             */
            while (count)
            {
                DWORD data = *(DWORD*)dwBuffer;

                /*
                 * print
                 */
                emit ("        : %08x\n",data);

                /*
                 * next
                 */
                count       -= 4;
                dwBuffer    += 4;
                dwByteCount -= 4;
            }
        }
        else
        {
            DWORD channel = (method & 0x0003e000) >> 13;
            DWORD addr    = (method & 0x00001fff) >>  0;

            emit ("\n  method: %08x - ch=%d, start=0x%x, count = %d\n",method,channel,addr,count/4);

            /*
             * next
             */
            dwBuffer    += 4;
            dwByteCount -= 4;

            /*
             * show contents
             */
            while (count)
            {
                DWORD data = *(DWORD*)dwBuffer;

                /*
                 * print
                 */
                emit ("%08x: %08x\n",addr,data);

                /*
                 * next
                 */
                addr        += 4;
                count       -= 4;
                dwBuffer    += 4;
                dwByteCount -= 4;
            }
        }
    }

    emit ("\n");

    /*
     * close file
     */
    if (!m_bToDebugger)
    {
        fflush (m_hFile);
        fclose (m_hFile);
        m_hFile = 0;
    }

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * emit
 *
 * helper function that either fprintf or sprintf/OutDB depending on file handle
 */
void UCL_DISASM::emit 
(
    char *szFormat,
    ...
)
{
    if (m_hFile)
    {
        vfprintf (m_hFile,szFormat,(char*)(((DWORD)&szFormat)+4));
    }
    else
    {
        char sz[1024];
        vsprintf (sz,szFormat,(char*)(((DWORD)&szFormat)+4));
        OutputDebugString (sz);
    }
}

/*
 * construction
 */
UCL_DISASM::UCL_DISASM (void)
{
    m_dwRefCount  = 1;
    m_bToDebugger = TRUE;
    m_hFile       = NULL;
    m_pszFilename = NULL;
}

UCL_DISASM::~UCL_DISASM (void)
{
    if (m_hFile)
    {
        fclose (m_hFile);
    }
    if (m_pszFilename)
    {
        free (m_pszFilename);
    }
}