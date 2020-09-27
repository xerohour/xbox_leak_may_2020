#ifndef KERBERRORCOMPARE_H
#define KERBERRORCOMPARE_H

#include "xkdctest.h"


#define NUMERRORS 21
#define ERRORLENGTH 32

class KerbErrorCompare
{
private:
    static char *m_szErrorNames[NUMERRORS];
    static NTSTATUS m_Errors[NUMERRORS];

public:
    static INT GetErrorValue(IN char *szErrorName)
    {
        for (INT i = 0; i < NUMERRORS; i++)
        {
            if (!_stricmp(szErrorName, m_szErrorNames[i]))
            {
                break;
            }
        }

        if (i == NUMERRORS)
        {
            return -1;
        }

        return m_Errors[i];
    }
};

#endif
