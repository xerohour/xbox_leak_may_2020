#ifndef XKDCTESTKERB_H
#define XKDCTESTKERB_H

#include "xkdctest.h"
#include "accounts.h"
#include "kerberrorcompare.h"


class CXoTest : public CXOnline
{
public:
    CXoTest(char * pszXbox = NULL) : CXOnline(pszXbox) {}

    HRESULT UacsCreate(
        IN IN_ADDR IPAddress, 
        IN OUT XONLINEP_USER *pUser);

    HRESULT SignInAccount_AS(
        IN DWORD dwIP, 
        IN USHORT PortNumber,
        IN XONLINEP_USER *Account, 
        IN BOOL fTcp, 
        IN OUT XKERB_TGT_CONTEXT *pTGTContext, 
        OUT NTSTATUS &Status);

    HRESULT SendAndVerifyTGSRequest(
        IN DWORD dwIP, 
        IN USHORT PortNumber,
        IN XONLINEP_USER *pUsers, 
        IN XKERB_TGT_CONTEXT *pCombinedTGTContext, 
        IN OUT XKERB_SERVICE_CONTEXT *pKerbServiceContext, 
        OUT NTSTATUS &Status);

    NTSTATUS KerbMakeKdcCall(
        IN DWORD dwIP, 
        IN USHORT PortNumber,
        IN BOOLEAN UseTcp,
        IN PKERB_MESSAGE_BUFFER RequestMessage,
        IN PKERB_MESSAGE_BUFFER ReplyMessage);

    void SysFree(void *pv)
    {
        GetXo()->SysFree(pv);
    }

private:
    VOID KerbCloseSocket(
        IN SOCKET SocketHandle);

    NTSTATUS KerbBindSocketByAddress(
        IN ULONG Address,
        IN ULONG AddressType,
        IN BOOLEAN UseDatagram,
        IN USHORT PortNumber,
        OUT SOCKET * ContextHandle);

    NTSTATUS KerbCallKdc(
        IN DWORD dwIP, 
        IN ULONG AddressType,
        IN ULONG Timeout,
        IN BOOLEAN UseDatagram,
        IN USHORT PortNumber,
        IN PKERB_MESSAGE_BUFFER Input,
        OUT PKERB_MESSAGE_BUFFER Output);
};

#endif
