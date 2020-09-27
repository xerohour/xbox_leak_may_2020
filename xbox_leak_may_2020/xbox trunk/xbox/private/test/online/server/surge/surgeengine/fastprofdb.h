/*++

Copyright (C) 1999 Microsoft Corporation

Module Name:

    FastProfDB.h

Abstract:

    

Author:

    Josh Poley (jpoley) 1-1-1999

Revision History:

--*/

#ifndef _FASTPROFDB_H_
#define _FASTPROFDB_H_

#include <io.h>

#define _FASTPROFDB

enum
    {
    PROFDB_USERNAME,
    PROFDB_DOMAIN,
    PROFDB_PASSWORD
    };

/*++

Class Description:

    FastProfDB

Methods:



Data:



NOTE: 
    

--*/
class FastProfDB
    {
    private:
        FILE *f;
        unsigned long len;
        char buff[1024];
        LARGE_INTEGER liPerf;

    public:
        FastProfDB(char *filename);
        ~FastProfDB();

        void GetNamePW(char *dbline, char *username, char *domain, char *pw, int size);

    public:
        char* GetRandUser(void);
        char* GetNextUser(void);
    };

#endif // _FASTPROFDB_H_
