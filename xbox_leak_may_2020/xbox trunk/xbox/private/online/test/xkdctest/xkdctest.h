#ifndef XKDCTEST_H
#define XKDCTEST_H

#include "xonp.h"
#include "xonver.h"
#include <xonlinep.h>
#include "xkdctestdebug.h"
#include "xkdctestkerb.h"


DWORD ToIP(
    char *szIP);

HRESULT HexCharToByte(
    IN char hex, 
    OUT BYTE *byte);

HRESULT HexPairToByte(
    IN char *hex, 
    OUT BYTE *byte);

#endif
