#ifndef XKTXONLINELOGON_H
#define XKTXONLINELOGON_H

#include "xktest.h"

class XKTXOnlineLogon : public XKTest
{
public:
    virtual void getName(OUT char *sz) {strcpy(sz, "XOnlineLogon");}

    virtual HRESULT runTest(IN CXoTest *pXOn, IN char *szSectionName, IN Config &cfg);
private:
    HRESULT NewUser(
        IN CXoTest *pXOn, 
        IN Config &cfg, 
        IN char *szName, 
        OUT XONLINEP_USER *pUser);

    HRESULT NewMachine(
        IN CXoTest *pXOn, 
        IN Config &cfg, 
        IN char *szSerial, 
        IN char *szPassword, 
        OUT XONLINEP_USER *pUser);

    HRESULT VerifyServices(
        IN Config &cfg, 
        IN char *szSectionName, 
        IN XKERB_SERVICE_CONTEXT *pKerbServiceContext);
    
    HRESULT VerifyUserHRs(
        IN Config &cfg, 
        IN char *szSectionName, 
        IN XONLINEP_USER *rgAccounts);

    HRESULT SignInServices(
        IN CXoTest *pXOn, 
        IN Config &cfg, 
        IN char *szSectionName,
        IN XONLINEP_USER *rgUsers,
        IN INT cUsers,
        IN XKERB_TGT_CONTEXT *ptkt);
};

#endif
