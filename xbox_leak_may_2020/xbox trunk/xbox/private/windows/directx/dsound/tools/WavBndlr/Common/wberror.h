/***************************************************************************
 *
 *  Copyright (C) 1/29/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wberror.h
 *  Content:    Wave Bundler error codes.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/29/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __WBERROR_H__
#define __WBERROR_H__

#define FACILITY_WBND       50

#define MAKE_WBND_HRESULT(code) \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WBND, code)
    
enum
{
    WBND_ERROR_BADFORMAT = 0,
    WBND_ERROR_SIGNATURE,
    WBND_ERROR_VERSION,
    WBND_ERROR_ENTRYEXISTS,
    WBND_ERROR_FILTERINIT,
    WBND_ERROR_COMPRESS,
    WBND_ERROR_BANKEXISTS,
    WBND_ERROR_BANKFILEEXISTS,
    WBND_ERROR_HEADERFILEEXISTS,
    WBND_ERROR_NOENTRIES,
};

#define WBND_BADFORMAT      MAKE_WBND_HRESULT(WBND_ERROR_BADFORMAT)
#define WBND_BADFORMAT      MAKE_WBND_HRESULT(WBND_ERROR_SIGNATURE)
#define WBND_BADFORMAT      MAKE_WBND_HRESULT(WBND_ERROR_VERSION)
#define WBND_BADFORMAT      MAKE_WBND_HRESULT(WBND_ERROR_ENTRYEXISTS)
#define WBND_BADFORMAT      MAKE_WBND_HRESULT(WBND_ERROR_FILTERINIT)
#define WBND_BADFORMAT      MAKE_WBND_HRESULT(WBND_ERROR_COMPRESS)
#define WBND_BADFORMAT      MAKE_WBND_HRESULT(WBND_ERROR_BANKEXISTS)
#define WBND_BADFORMAT      MAKE_WBND_HRESULT(WBND_ERROR_BANKFILEEXISTS)
#define WBND_BADFORMAT      MAKE_WBND_HRESULT(WBND_ERROR_HEADERFILEEXISTS)
#define WBND_BADFORMAT      MAKE_WBND_HRESULT(WBND_ERROR_NOENTRIES)

#include "namespc.h"

EXTERN_C LPCSTR WbndTranslateError(HRESULT hr);

#include "namespc.h"

#endif // __WBERROR_H__

