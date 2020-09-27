


#ifndef __LockPointerAlias_h__
#define __LockPointerAlias_h__



#include <CD3DTest.h>
#include <windows.h>



USETESTFRAME



#define COUNTOF(a) (sizeof(a)/sizeof((a)[0]))
#define DecodeHResult(hr) (GetHResultString(hr).c_str())

#define GROUP_CREATE(GroupName)  \
GroupName Class##GroupName



class LockPointerAlias : public CD3DTest
{
public:
    LockPointerAlias();

    UINT TestInitialize();
    UINT TestInitialize7();
    UINT TestInitialize8();
    bool ExecuteTest(UINT nTest);
    bool ExecuteTest7(UINT nTest);
    bool ExecuteTest8(UINT nTest);
};



#endif

