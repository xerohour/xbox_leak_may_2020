/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    rsrcdata.h

Author:

    Matt Bronder

Description:

    Resource data list.

*******************************************************************************/

#ifndef __RSRCDATA_H__
#define __RSRCDATA_H__

BOOL LoadResourceFile(LPCTSTR szName, LPVOID* ppvData, LPDWORD pdwSize);
void UnloadResourceFile(LPCTSTR szName);

#endif // __RSRCDATA_H__
