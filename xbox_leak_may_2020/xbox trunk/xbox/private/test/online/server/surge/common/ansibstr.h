/*++

Copyright (C) 1999 Microsoft Corporation

Module Name:

    AnsiBstr.h

Abstract:

    

Author:

    Josh Poley (jpoley) 1-1-1999

Revision History:

--*/

#ifndef _ANSIBSTR_H_
#define _ANSIBSTR_H_

class AnsiBstr
    {
    public:
        PCHAR astr;
        BSTR bstr;

    public:
        AnsiBstr();
        AnsiBstr(BSTR b);
        AnsiBstr(PCHAR a);

        void SetStr(BSTR b);
        void SetStr(PCHAR a);

        operator PCHAR(void) const { return astr; }
        operator BSTR(void) const { return bstr; }


        ~AnsiBstr();
    };

extern PCHAR CreateAnsiFromBstr(BSTR bstr);
extern BSTR CreateBstrFromAnsi(PCHAR pszText);

#endif // _ANSIBSTR_H_