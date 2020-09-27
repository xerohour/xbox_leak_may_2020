/******************************************************************************************
Used to ease porting tests from Windows.


******************************************************************************************/

#pragma once



#define CALLDMTEST( func )                                          \
{                                                                   \
    if (SUCCEEDED(hr))                                              \
    {                                                               \
        dwRes = func;                                               \
        if (dwRes == FNS_PASS)                                      \
            hr = S_OK;                                              \
        else                                                        \
            hr = E_FAIL;                                            \
    }                                                               \
}                                                                   



#define fnsIncrementIndent LogIncrementIndent
#define fnsDecrementIndent LogDecrementIndent

#define FNS_PASS                        0x00000000
#define FNS_FAIL                        0x00000001
#define FNS_ABORTED                     0x00000002
#define FNS_PENDING                     0x00000003


#define ghWnd NULL

