#include "xkdctest.h"
#include "ticketcache.h"


Vector<TicketCache::TicketInfo> TicketCache::m_vctTickets;
Vector<TicketCache::AccountInfo> TicketCache::m_vctAccounts;

HRESULT TicketCache::GenerateSignature(
    IN XONLINEP_USER *rgUsers,
    OUT TicketInfo &ticketinfo)
{
    HRESULT hr = E_FAIL;

    for (INT i = 0; i < 1 + XONLINE_MAX_LOGON_USERS; i++)
    {
        ticketinfo.rgXuids[i] = rgUsers[i].xuid;
        hr = S_OK;
    }

    return hr;
}


HRESULT TicketCache::FindTicket(
    IN XONLINEP_USER *rgUsers,
    OUT XKERB_TGT_CONTEXT *ptkt)
{
    HRESULT hr = E_FAIL;

    TicketInfo ticketinfo;
    if (SUCCEEDED(GenerateSignature(rgUsers, ticketinfo)))
    {
        hr = S_FALSE;
        for (INT i = 0; i < m_vctTickets.GetCount(); i++)
        {
            if (!memcmp(ticketinfo.rgXuids, m_vctTickets[i].rgXuids, sizeof(XUID) * (1 + XONLINE_MAX_LOGON_USERS)))
            {
                *ptkt = m_vctTickets[i].tkt;
                hr = S_OK;
            }
        }
    }

    return hr;
}


HRESULT TicketCache::AddTicket(
    IN XONLINEP_USER *rgUsers,
    IN XKERB_TGT_CONTEXT *ptkt)
{
    HRESULT hr = E_FAIL;

    TicketInfo ticketinfo;
    memset(&ticketinfo, 0, sizeof(TicketInfo));

    hr = GenerateSignature(rgUsers, ticketinfo);

    ticketinfo.tkt = *ptkt;
    m_vctTickets.Add(ticketinfo);

    return hr;
}


HRESULT TicketCache::FindAccount(
    IN char* szAccountIndicator,
    OUT XONLINEP_USER *pUser)
{
    HRESULT hr = E_FAIL;

    hr = S_FALSE;
    for (INT i = 0; i < m_vctAccounts.GetCount(); i++)
    {
        if (!strcmp(szAccountIndicator, m_vctAccounts[i].szAccount))
        {
            *pUser = m_vctAccounts[i].User;
            hr = S_OK;
        }
    }

    return hr;
}


HRESULT TicketCache::AddAccount(
    IN char* szAccountIndicator,
    IN XONLINEP_USER *pUser)
{
    HRESULT hr = E_FAIL;

    AccountInfo accountinfo;
    memset(&accountinfo, 0, sizeof(AccountInfo));

    strcpy(accountinfo.szAccount, szAccountIndicator);
    accountinfo.User = *pUser;

    if (m_vctAccounts.Add(accountinfo))
    {
        hr = S_OK;
    }

    return hr;
}

