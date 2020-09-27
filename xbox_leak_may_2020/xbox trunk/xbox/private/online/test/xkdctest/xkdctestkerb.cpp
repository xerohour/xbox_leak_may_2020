#include "xkdctest.h"


#define XONLINE_AS_EXCHANGE_BUFFER_SIZE 1400
#define XONLINE_TGS_COMBINE_REQUEST_BUFFER_SIZE 5120
#define XONLINE_TGS_EXCHANGE_BUFFER_SIZE 1400

#define XBOX_REALM      "xbox.com"
#define MACS_REALM      "macs.xbox.com"


/*
HRESULT CXoTest::UacsCreate(
    IN IN_ADDR dwIP, 
    IN OUT XONLINEP_USER *pUser)
{
    HRESULT hr = E_FAIL;

    Assert(sizeof(XONLINE_USER) == 128);

    char pb[1024];
    _snprintf(pb, 1024, "POST /xuacs/createacct.ashx HTTP/1.0\r\nUser-agent: 5/1.0\r\nContent-Length: %d\r\nContent-Type: xon/5\r\n\r\n", sizeof(XONLINE_USER));
    INT cb = strlen(pb);

    Assert(cb + sizeof(XONLINE_USER) < 1024);
    memcpy(pb + cb, pUser, sizeof(XONLINE_USER));
    cb += sizeof(XONLINE_USER);

    SOCKET Socket = INVALID_SOCKET;
    if ((Socket = GetXo()->socket(PF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET)
    {
        GetXo()->XnSetInsecure(Socket);

        sockaddr_in ServerAddress;
        memset(&ServerAddress, 0, sizeof(sockaddr_in));
        ServerAddress.sin_family = AF_INET;
        ServerAddress.sin_addr = dwIP;
        ServerAddress.sin_port = 80 << 8;

        if (GetXo()->connect(Socket, (LPSOCKADDR)&ServerAddress, sizeof(ServerAddress)) != SOCKET_ERROR)
        {
            if (GetXo()->send(Socket, pb, cb, 0) != SOCKET_ERROR)
            {
                if ((cb = GetXo()->recv(Socket, pb, cb, 0)) != SOCKET_ERROR)
                {
                    if (!memcmp(pb + 9, "200", 3))
                    {
                        memcpy(pUser, pb + 149, 128);
                        INT err = KerbPasswordToKey(pUser->realm, pUser->key);
                        strcpy(pUser->realm, "passport.net");
                        hr = S_OK;
                    }
                    else
                    {
                        Assert(!memcmp(pb + 9, "500", 3));
                        char *szhrXError = strstr(pb, "X-Err: ");
                        if (szhrXError)
                        {
                            szhrXError += 7;
                            if (!memcmp(szhrXError, "80154000", 8))
                            {
                                hr = XUAC_E_NAME_TAKEN;
                            }
                            else if (!memcmp(szhrXError, "80154001", 8))
                            {
                                hr = XUAC_E_INVALID_KINGDOM;
                            }
                            else if (!memcmp(szhrXError, "80154002", 8))
                            {
                                hr = XUAC_E_INVALID_USER;
                            }
                            else if (!memcmp(szhrXError, "80154003", 8))
                            {
                                hr = XUAC_E_USER_NOT_LOCKED_OUT;
                            }
                            else if (!memcmp(szhrXError, "80154004", 8))
                            {
                                hr = XUAC_E_ACCOUNT_INACTIVE;
                            }
                        }
                    }
                }
            }
        }
    }

    if (Socket != INVALID_SOCKET)
    {
        GetXo()->closesocket(Socket);
    }

    return hr;
}
*/


HRESULT CXoTest::SignInAccount_AS(
    IN DWORD dwIP, 
    IN USHORT PortNumber,
    IN XONLINEP_USER *pUser, 
    IN BOOL fMachineAccount,
    IN OUT XKERB_TGT_CONTEXT *ptkt, 
    OUT NTSTATUS &Status)
{
    HRESULT hr = E_FAIL;

    BYTE RequestMessageBuffer[XONLINE_AS_EXCHANGE_BUFFER_SIZE];
    ULONG dwRequestMessageLength = XONLINE_AS_EXCHANGE_BUFFER_SIZE;

    Status = GetXo()->XkerbBuildAsRequest(
        fMachineAccount,
        pUser,
        RequestMessageBuffer,
        &dwRequestMessageLength,
        ptkt);
    if (NT_SUCCESS(Status))
    {
        KERB_MESSAGE_BUFFER RequestMessage;

        RequestMessage.Buffer = RequestMessageBuffer;
        RequestMessage.BufferSize = dwRequestMessageLength;

        KERB_MESSAGE_BUFFER ReplyMessage;

        Status = KerbMakeKdcCall(
            dwIP,
            PortNumber,
            FALSE, // Always use UDP
            &RequestMessage,
            &ReplyMessage);
        if (NT_SUCCESS(Status))
        {
            Status = GetXo()->XkerbVerifyAsReply(
                ReplyMessage.Buffer, 
                ReplyMessage.BufferSize, 
                ptkt);
            if (NT_SUCCESS(Status))
            {
                hr = S_OK;
            }
        }
    }

    return hr;
}


HRESULT CXoTest::SendAndVerifyTGSRequest(
    IN DWORD dwIP, 
    IN USHORT PortNumber,
    IN XONLINEP_USER *pUsers, 
    IN XKERB_TGT_CONTEXT *ptgtCombined, 
    IN OUT XKERB_SERVICE_CONTEXT *pKerbServiceContext, 
    OUT NTSTATUS &Status)
{
    HRESULT hr = E_FAIL;

    BYTE RequestMessageBuffer[XONLINE_TGS_EXCHANGE_BUFFER_SIZE];
    ULONG dwRequestMessageLength = XONLINE_TGS_EXCHANGE_BUFFER_SIZE;

    Status = GetXo()->XkerbBuildTgsRequest(
        ptgtCombined,
        RequestMessageBuffer,
        &dwRequestMessageLength,
        pKerbServiceContext);
    if (NT_SUCCESS(Status))
    {
        KERB_MESSAGE_BUFFER RequestMessage;

        RequestMessage.Buffer = RequestMessageBuffer;
        RequestMessage.BufferSize = dwRequestMessageLength;        

        KERB_MESSAGE_BUFFER ReplyMessage;

        Status = KerbMakeKdcCall(
            dwIP,
            PortNumber,
            FALSE, // Always use UDP
            &RequestMessage,
            &ReplyMessage);
        if (NT_SUCCESS(Status))
        {
            Status = GetXo()->XkerbVerifyTgsReply(
                pUsers,
                ptgtCombined,
                ReplyMessage.Buffer,
                ReplyMessage.BufferSize,
                pKerbServiceContext);
            if (NT_SUCCESS(Status))
            {
                hr = S_OK;
            }
        }
    }

    return hr;
}

