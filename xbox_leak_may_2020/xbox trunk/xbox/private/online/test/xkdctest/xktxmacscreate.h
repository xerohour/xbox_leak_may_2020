#ifndef XKTXMACSCREATE_H
#define XKTXMACSCREATE_H

#include "xktest.h"


class XKTXmacsCreate : public XKTest
{
public:
    virtual void getName(OUT char *sz) {strcpy(sz, "XmacsCreate");}
    virtual HRESULT runTest(IN CXoTest *xo, IN char *szSectionName, IN Config &cfg);

private:
    char *getStrX(Config &cfg, char *szSectionName, char *szItem, INT nIteration, INT nCreations);
    HRESULT MakeCreationAccount(IN char *szSectionName, IN Config &cfg, IN INT nIteration, IN INT nCreations, OUT XONLINEP_USER *pAccount);
    HRESULT VerifyCreate(IN CXoTest *xo, Config &cfg, XONLINEP_USER &MachineAccount, XKERB_TGT_CONTEXT &tgt);
};

#endif

