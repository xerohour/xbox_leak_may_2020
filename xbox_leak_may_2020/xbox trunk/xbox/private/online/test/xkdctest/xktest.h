#ifndef XKTEST_H
#define XKTEST_H

#include "config.h"

class XKTest
{
public:
    XKTest() {}
    ~XKTest() {}
    
    virtual void getName(OUT char *sz) = 0;

    virtual HRESULT runTest(IN CXoTest *xo, IN char *szSectionName, IN Config &cfg) = 0;
};

#endif
