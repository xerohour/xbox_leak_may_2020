#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>

#define __NVUCL_INCLUDED
#include "nvucl.h"

/*
 * global macros
 */
#define PREFETCH(x)     (volatile DWORD)*(DWORD*)(x)

/*
struct UCL_BASE : public NVUCL_BASE
{
private:
    DWORD m_dwRefCount;
public:
    virtual NVUCL_RESULT NVUCL_STDCALL attach (void)
    {
        m_dwRefCount++;
        return NVUCL_OK;
    }
    virtual NVUCL_RESULT NVUCL_STDCALL detach (void)
    {
        if (--m_dwRefCount) return NVUCL_OK;
        delete this;
        return NVUCL_OK;
    }
    virtual ~UCL_BASE (void)
    {
    }
};
*/