#ifndef __D3DXEXCEPT_H__
#define __D3DXEXCEPT_H__

#include <string.h>
#include "calloc.h"

#define D3DX_THROW( hResult, string )  throw CD3DXException( hResult, string, __LINE__);
#define D3DX_CATCH   catch( CD3DXException e ) { e.DebugString(); return e.error; }
#define D3DX_TRY     try

class CD3DXException : public CD3duAlloc
{
public:
    CD3DXException(HRESULT res, char *msg, int LineNumber = __LINE__) 
    {
        error = res; 
        strcpy(message, msg); 
        line = LineNumber;
    }
    char message[128];
    HRESULT error;
    int line;
    
    void DebugString() {};
    void Popup() {};
};

#endif // __D3DXEXCEPT_H__
