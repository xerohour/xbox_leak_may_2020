#ifndef XKDCTESTDEBUG_H
#define XKDCTESTDEBUG_H

#include "xkdctest.h"


enum XKTVerbose {XKTVerbose_None, XKTVerbose_Status, XKTVerbose_Process, XKTVerbose_All};

#define TestErrMsg TestMsg
void TestMsg(char *message, ...);
void TestMsg(XKTVerbose xktvLevel, char *message, ...);

#define TAHR(a) TestAssertHR(a, &hr);
void TestAssertHR(HRESULT hr, HRESULT *hrFunc = NULL);

#define TA(a) TestAssert(a, &hr)
void TestAssert(BOOL condition, HRESULT *hrFunc = NULL);

BOOL IsHex(char *sz, INT length);

HRESULT FillMemDbg(char *pb, INT cb);

HRESULT CheckMemDbg(char *pb, INT cb);

#endif
