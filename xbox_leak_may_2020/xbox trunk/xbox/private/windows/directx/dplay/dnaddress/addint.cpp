/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       addbase.cpp
 *  Content:    DirectPlay8Address Internal interace file
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *  ====       ==      ======
 * 02/04/2000	rmt		Created
 * 02/17/2000	rmt		Parameter validation work 
 * 03/21/2000   rmt     Renamed all DirectPlayAddress8's to DirectPlay8Addresses 
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dnaddri.h"

HRESULT IDirectPlay8Address_Lock(LPDIRECTPLAY8ADDRESS pDirectPlay8Address)
{
    return DP8ADDRESSOBJECT::GetDirectPlay8Address(pDirectPlay8Address)->Lock();
}

HRESULT IDirectPlay8Address_UnLock(LPDIRECTPLAY8ADDRESS pDirectPlay8Address)
{
    return DP8ADDRESSOBJECT::GetDirectPlay8Address(pDirectPlay8Address)->UnLock();
}
