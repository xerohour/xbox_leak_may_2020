
#ifndef XACT_COMMON_H
#define XACT_COMMON_H

//
// common data/code
//


EXTERN_C DWORD XACTpCreateHashFromName(const PCHAR pszName,DWORD dwSeed);

//
// common functions for host and xbox side code
//

DWORD XACTpCreateHashFromName(const PCHAR pszName,DWORD dwSeed)
{
    DWORD dwTemp;
    DWORD dwResult = 0;

	srand(dwSeed);
    int r = rand();
    int r1 = rand();

    PCHAR pBuffer = pszName;

    for (DWORD i=0;i<sizeof(dwResult);i++){

        dwTemp = (DWORD) pBuffer;
        dwTemp ^= dwTemp*r*r*r1 | (*(pBuffer+3) << 16);

        dwResult = (dwResult ^ dwTemp) * dwTemp;
        pBuffer ++;   

    }

    return dwResult;

}

#endif // XACT_COMMON_H
