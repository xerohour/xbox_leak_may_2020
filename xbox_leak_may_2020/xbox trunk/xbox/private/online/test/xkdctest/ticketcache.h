#ifndef TICKETCACHE_H
#define TICKETCACHE_H

#include <stdio.h>
#include "config.h"

#include "xkdctest.h"


class TicketCache {
private:
    struct TicketInfo {XUID rgXuids[1 + XONLINE_MAX_LOGON_USERS]; XKERB_TGT_CONTEXT tkt;};
    static Vector<TicketInfo> m_vctTickets;
    struct AccountInfo {char szAccount[64]; XONLINEP_USER User;};
    static Vector<AccountInfo> m_vctAccounts;

    TicketCache();
    ~TicketCache();

    static HRESULT GenerateSignature(
        IN XONLINEP_USER *rgUsers,
        OUT TicketInfo &ticketinfo);

public:
    static HRESULT FindTicket(
        IN XONLINEP_USER *rgUsers,
        OUT XKERB_TGT_CONTEXT *ptkt);

    static HRESULT AddTicket(
        IN XONLINEP_USER *rgUsers,
        IN XKERB_TGT_CONTEXT *ptkt);

    static HRESULT FindAccount(
        IN char* szAccountIndicator,
        OUT XONLINEP_USER *pUser);
    
    static HRESULT AddAccount(
        IN char* szAccountIndicator,
        IN XONLINEP_USER *pUser);
};

#endif

