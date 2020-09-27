/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       StrUtils.h
 *  Content:    Defines the string utils
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *   02/12/2000	rmt		Created
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#ifndef __STRUTILS_H
#define __STRUTILS_H

HRESULT STR_jkWideToAnsi(LPSTR lpStr,LPCWSTR lpWStr,int cchStr);
HRESULT STR_jkAnsiToWide(LPWSTR lpWStr,LPCSTR lpStr,int cchWStr);

#endif
