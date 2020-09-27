// ---------------------------------------------------------------------------------------
// vlan.h
//
// Virtual Lan Library
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef __VLAN_H__
#define __VLAN_H__

BOOL WINAPI VLanInit();
BOOL WINAPI VLanDriver();
BOOL WINAPI VLanAttach(char * pszLan, BYTE * pbEnet, void * pvArg);
void WINAPI VLanRecv(BYTE * pb, UINT cb, void * pvArg);
BOOL WINAPI VLanXmit(BYTE * pb, UINT cb);
BOOL WINAPI VLanDetach(BYTE * pbEnet);
void WINAPI VLanTerm();

#endif
