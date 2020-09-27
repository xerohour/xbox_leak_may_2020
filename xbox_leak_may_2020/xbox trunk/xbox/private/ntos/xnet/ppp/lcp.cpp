/*++

Copyright (c) 2000 Microsoft Corporation

    lcp.cpp

Abstract:

    Line Control Protocol.

Revision History:

    07-11-00    vadimg      created

--*/

#include "precomp.h"

LCP_OPTIONS gLcpDefault = {
        0,                  // Negotiation flags
        LCP_DEFAULT_MRU,    // Default value for MRU
        0xFFFFFFFF,         // Default ACCM value.
        0,                  // no authentication ( for client )
        0,                  // no authentication data ( for client )
        NULL,               // no authentication data ( for client )
        0,                  // Magic Number.
        FALSE,               // Protocol field compression.
        FALSE,               // Address and Contorl-Field Compression.
        0,                  // Callback Operation message field
        LCP_DEFAULT_MRU,    // Default value for MRRU == MRU according to RFC1717
        0,                  // No short sequencing
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // No endpoint discriminator
        0,                  // Length of Endpoint Discriminator
        0                   // Link Discriminator (for BAP/BACP)
};

DWORD gSizeOfOption[] = {
        0,                          // unused 
        PPP_OPTION_HDR_LEN + 2,     // MRU 
        PPP_OPTION_HDR_LEN + 4,     // ACCM 
        PPP_OPTION_HDR_LEN + 2,     // authentication 
        0,                          // Unused.
        PPP_OPTION_HDR_LEN + 4,     // magic number 
        0,                          // Reserved, unused
        PPP_OPTION_HDR_LEN + 0,     // Protocol compression 
        PPP_OPTION_HDR_LEN + 0,     // Address/Control compression 
        0,                          // Unused
        0,                          // Unused
        0,                          // Unused
        0,                          // Unused
        PPP_OPTION_HDR_LEN + 1,     // Callback
        0,                          // Unused
        0,                          // Unused
        0,                          // Unused
        PPP_OPTION_HDR_LEN + 2,     // MRRU
        PPP_OPTION_HDR_LEN + 0,     // Short Sequence Header Format
        PPP_OPTION_HDR_LEN,         // Endpoint Discriminator
        0,                          // Unused
        0,                          // Unused
        0,                          // Unused
        PPP_OPTION_HDR_LEN + 2      // Link Discriminator (for BAP/BACP)
};

DWORD gLcpNegotiate = LCP_N_MRU | LCP_N_ACCM | LCP_N_AUTHENT | LCP_N_MAGIC | LCP_N_PFC | LCP_N_ACFC;

/***************************************************************************\
* MakeOption
*
\***************************************************************************/

DWORD MakeOption(LCP_OPTIONS *pOption, DWORD dwOptionCode,
        PPP_OPTION *pSendOption, DWORD cbSendOption)
{
    if (cbSendOption < gSizeOfOption[dwOptionCode]) {
        return ERROR_BUFFER_TOO_SMALL;
    }

    pSendOption->Type = (BYTE)dwOptionCode;
    pSendOption->Length = (BYTE)gSizeOfOption[dwOptionCode];

    switch (dwOptionCode) {
    case LCP_OPTION_MRU:
        HostToWireFormat16((WORD)pOption->MRU, pSendOption->Data);
        break;

    case LCP_OPTION_ACCM:
        HostToWireFormat32(pOption->ACCM, pSendOption->Data);
        break;

    case LCP_OPTION_AUTHENT:
        if (cbSendOption < (gSizeOfOption[dwOptionCode] + pOption->APDataSize)) {
            return ERROR_BUFFER_TOO_SMALL;
        }
        HostToWireFormat16((WORD)pOption->AP, pSendOption->Data);
        CopyMemory(pSendOption->Data + 2, pOption->pAPData, pOption->APDataSize);
        pSendOption->Length += (BYTE)pOption->APDataSize;
        break;

    case LCP_OPTION_MAGIC:
        HostToWireFormat32(pOption->MagicNumber, pSendOption->Data);
        break;
    
    case LCP_OPTION_PFC:
        break;

    case LCP_OPTION_ACFC:
        break;

    case LCP_OPTION_MRRU:    
        HostToWireFormat16((WORD)pOption->MRRU, pSendOption->Data);
        break;

    default: 
        return ERROR_INVALID_PARAMETER;
    }

    return NO_ERROR;
}

/***************************************************************************\
* CheckOption
*
\***************************************************************************/

DWORD CheckOption(LCP_SIDE *pLcpSide, PPP_OPTION *pOption, BOOL fMakingResult)
{
    DWORD dwIndex;
    DWORD dwAPDataSize;
    DWORD dwRetCode = CONFIG_ACK;

    if (pOption->Length < gSizeOfOption[pOption->Type]) {
        return CONFIG_REJ;
    }

    // If we do not want to negotiate the option we CONFIG_REJ it.
    if (!(pLcpSide->WillNegotiate & (1 << pOption->Type))) {
        return CONFIG_REJ;
    }

    switch (pOption->Type) {
    case LCP_OPTION_MRU:
        pLcpSide->Work.MRU = WireToHostFormat16(pOption->Data);

        // Check to see if this value is appropriate
        if (fMakingResult) {

            // We cannot send packets smaller than LCP_DEFAULT_MRU
            if (pLcpSide->Work.MRU < LCP_DEFAULT_MRU) {
                pLcpSide->Work.MRU = pLcpSide->Want.MRU;
                dwRetCode = CONFIG_NAK;
            }
        } else {

            // We cannot receive bigger packets.
            if (pLcpSide->Work.MRU > pLcpSide->Want.MRU) {
                pLcpSide->Work.MRU = pLcpSide->Want.MRU;
                dwRetCode = CONFIG_NAK;
            }
        }
    break;

    case LCP_OPTION_ACCM:
        pLcpSide->Work.ACCM = WireToHostFormat32(pOption->Data);

        // If we are responding to a request, we accept it blindly, if we are
        // processing a NAK, then the remote host may ask to escape more
        // control characters than we require, but must escape at least the
        // control chars that we require.
        if (!fMakingResult) {
            if (pLcpSide->Work.ACCM !=
                    (pLcpSide->Work.ACCM | pLcpSide->Want.ACCM)) {
                pLcpSide->Work.ACCM |= pLcpSide->Want.ACCM;
                dwRetCode = CONFIG_NAK;
            }
        }
        break;

    case LCP_OPTION_AUTHENT:
        pLcpSide->Work.AP = WireToHostFormat16( pOption->Data );

        // If there was Authentication data.
        if (pOption->Length > PPP_OPTION_HDR_LEN + 2) {

            dwAPDataSize = pOption->Length - PPP_OPTION_HDR_LEN - 2;

            if (dwAPDataSize != pLcpSide->Work.APDataSize) {

                pLcpSide->Work.APDataSize = dwAPDataSize;

                if (pLcpSide->Work.pAPData != NULL) {
                    Free(pLcpSide->Work.pAPData);
                    pLcpSide->Work.pAPData = NULL;
                }

                pLcpSide->Work.pAPData = (PBYTE)Alloc(pLcpSide->Work.APDataSize);

                if (pLcpSide->Work.pAPData == NULL) {
                    pLcpSide->Work.APDataSize = 0;
                    return CONFIG_REJ;
                }
            }

            CopyMemory(pLcpSide->Work.pAPData, pOption->Data + 2,
                    pLcpSide->Work.APDataSize);

        } else {

            pLcpSide->Work.APDataSize = 0;
        }

        switch (pLcpSide->Work.AP) {
        case PPP_PAP_PROTOCOL:
            if (!(pLcpSide->APsAvailable & LCP_AP_PAP)) {
                dwRetCode = CONFIG_NAK;
            }
            break;

        default:
            dwRetCode = CONFIG_NAK;
            break;
        }
        break;

    case LCP_OPTION_MAGIC:
        pLcpSide->Work.MagicNumber = WireToHostFormat32(pOption->Data);

        if (fMakingResult) {

            // Ensure that magic numbers are different and that the remote
            // request does not contain a magic number of 0.
            if ((pLcpSide->Work.MagicNumber == gLcp.Local.Work.MagicNumber) ||
                    (pLcpSide->Work.MagicNumber == 0)) {

                if (pLcpSide->Work.MagicNumber == gLcp.Local.Work.MagicNumber) {
                    ++gLcp.dwMagicNumberFailureCount;
                }

                pLcpSide->Work.MagicNumber = GetMagicNumber();

                dwRetCode = CONFIG_NAK;
            }
        } else {

            // The remote peer NAK'ed with a magic number, check to see if
            // the magic number in the NAK is the same as what we NAK'ed last
            if (pLcpSide->Work.MagicNumber == gLcp.Remote.Work.MagicNumber) {

                ++gLcp.dwMagicNumberFailureCount;

                pLcpSide->Work.MagicNumber = GetMagicNumber();

                dwRetCode = CONFIG_NAK;
            }
        }
        break;

    case LCP_OPTION_PFC:
        pLcpSide->Work.PFC = TRUE;

        if (pLcpSide->Want.PFC == FALSE) {
            dwRetCode = CONFIG_REJ;
        }
        break;

    case LCP_OPTION_ACFC:
        pLcpSide->Work.ACFC = TRUE;

        if (pLcpSide->Want.ACFC == FALSE) {
            dwRetCode = CONFIG_REJ;
        }
        break;

    case LCP_OPTION_MRRU:    
        pLcpSide->Work.MRRU = WireToHostFormat16(pOption->Data);

        // Check to see if this value is appropriate
        if (fMakingResult) {

            // We cannot send smaller reconstructed packets.
            if (pLcpSide->Work.MRRU < pLcpSide->Want.MRRU) {
                pLcpSide->Work.MRRU = pLcpSide->Want.MRRU;
                dwRetCode = CONFIG_NAK;
            }
        } else {

            // We cannot receive bigger reconstructed packets.
            if (pLcpSide->Work.MRRU > pLcpSide->Want.MRRU) {
                pLcpSide->Work.MRRU = pLcpSide->Want.MRRU;
                dwRetCode = CONFIG_NAK;
            }
        }
        break;

    default:
        // If we do not recognize the option we CONFIG_REJ it.
        dwRetCode = CONFIG_REJ;
        break;
    }

    return dwRetCode;
}

/***************************************************************************\
* BuildOptionList
*
\***************************************************************************/

DWORD BuildOptionList(BYTE *pOptions, DWORD *pcbOptions,
        LCP_OPTIONS *LcpOptions, DWORD Negotiate)
{
    DWORD OptionType;
    DWORD dwRetCode;
    DWORD cbOptionLength = *pcbOptions;
    DWORD dwResult;

    for (OptionType = 1; OptionType <= LCP_OPTION_LIMIT; OptionType++) {
        if (Negotiate & (1 << OptionType)) {

            dwResult = MakeOption(LcpOptions, OptionType,
                    (PPP_OPTION*)pOptions, cbOptionLength);

            if (dwResult != NO_ERROR) {
                return dwResult;
            }

            cbOptionLength -= ((PPP_OPTION*)pOptions)->Length;
            pOptions += ((PPP_OPTION*)pOptions)->Length;
        }
    }
    *pcbOptions -= cbOptionLength;

    return NO_ERROR;
}

/***************************************************************************\
* LcpMakeConfigResult
*
\***************************************************************************/

DWORD LcpMakeConfigResult(PPP_CONFIG *pRecvConfig, PPP_CONFIG *pSendConfig,
        DWORD cbSendConfig, BOOL fRejectNaks)
{
    DWORD dwDesired;
    DWORD dwRetCode;
    DWORD ResultType = CONFIG_ACK;
    PPP_OPTION *pRecvOption = (PPP_OPTION*)pRecvConfig->Data;
    PPP_OPTION *pSendOption = (PPP_OPTION*)pSendConfig->Data;
    LONG lSendLength = cbSendConfig - PPP_CONFIG_HDR_LEN;
    LONG lRecvLength = WireToHostFormat16(pRecvConfig->Length) - PPP_CONFIG_HDR_LEN;

    // Clear negotiate mask
    gLcp.Remote.Work.Negotiate = 0;

    while (lRecvLength > 0) {

        if (pRecvOption->Length == 0) {
            return ERROR_PPP_INVALID_PACKET;
        }

        lRecvLength -= pRecvOption->Length;

        if (lRecvLength < 0 ) {
            return ERROR_PPP_INVALID_PACKET;
        }

        dwRetCode = CheckOption(&gLcp.Remote, pRecvOption, TRUE);

        // If we were building an ACK and we got a NAK or reject OR
        // we were building a NAK and we got a reject.
        if ((ResultType == CONFIG_ACK && dwRetCode != CONFIG_ACK) ||
                (ResultType == CONFIG_NAK && dwRetCode == CONFIG_REJ)) {
            ResultType  = dwRetCode;
            pSendOption = (PPP_OPTION*)pSendConfig->Data;
            lSendLength = cbSendConfig - PPP_CONFIG_HDR_LEN;
        }

        // Remember that we processed this option
        if (dwRetCode != CONFIG_REJ && pRecvOption->Type <= LCP_OPTION_LIMIT) {
            gLcp.Remote.Work.Negotiate |= (1 << pRecvOption->Type);
        }

        if (dwRetCode == ResultType) {

            // If this option is to be rejected, simply copy the
            // rejected option to the send buffer
            if (dwRetCode == CONFIG_REJ  ||
                    (dwRetCode == CONFIG_NAK && fRejectNaks)) {
                CopyMemory(pSendOption, pRecvOption, pRecvOption->Length);
            } else {

                dwRetCode = MakeOption(&gLcp.Remote.Work, pRecvOption->Type,
                        pSendOption, lSendLength);

                if (dwRetCode != NO_ERROR) {
                    return dwRetCode;
                }
            }

            lSendLength -= pSendOption->Length;

            pSendOption  = (PPP_OPTION*)((BYTE*)pSendOption + pSendOption->Length);
        }

        pRecvOption = (PPP_OPTION*)((BYTE*)pRecvOption + pRecvOption->Length);
    }

    // If this was an NAK and we cannot send any more NAKS then we
    // make this a REJECT packet
    if (ResultType == CONFIG_NAK && fRejectNaks) {
        pSendConfig->Code = CONFIG_REJ;
    } else {
        pSendConfig->Code = (BYTE)ResultType;
    }

    HostToWireFormat16((WORD)(cbSendConfig - lSendLength), pSendConfig->Length);

    if (ResultType == CONFIG_NAK && gLcp.dwMagicNumberFailureCount > 3) {
        return ERROR_PPP_LOOPBACK_DETECTED;
    }

    return NO_ERROR;
}

/***************************************************************************\
* LcpMakeConfigRequest
*
\***************************************************************************/

DWORD LcpMakeConfigRequest(PPP_CONFIG *pConfig, DWORD cbConfig)
{
    DWORD dwRetCode;

    cbConfig -= PPP_CONFIG_HDR_LEN;

    dwRetCode = BuildOptionList(pConfig->Data, &cbConfig, &gLcp.Local.Work,
            gLcp.Local.Work.Negotiate);

    if (dwRetCode != NO_ERROR) {
        return dwRetCode;
    }

    HostToWireFormat16((WORD)(cbConfig + PPP_CONFIG_HDR_LEN), pConfig->Length);

    return NO_ERROR;
}

/***************************************************************************\
* LcpConfigAckReceived
*
\***************************************************************************/

DWORD LcpConfigAckReceived(PPP_CONFIG *pConfig)
{
    DWORD dwRetCode;
    BYTE ConfigReqSent[LCP_DEFAULT_MRU];
    PPP_OPTION *pOption = (PPP_OPTION*)pConfig->Data;
    DWORD cbConfigReqSent = sizeof(ConfigReqSent);
    DWORD dwLength;
            
    dwLength = WireToHostFormat16(pConfig->Length) - PPP_CONFIG_HDR_LEN;

    // Get a copy of last request we sent
    dwRetCode = BuildOptionList(ConfigReqSent, &cbConfigReqSent,
            &gLcp.Local.Work, gLcp.Local.Work.Negotiate);

    if (dwRetCode != NO_ERROR) {
        return dwRetCode;
    }

    if (dwLength != cbConfigReqSent) {
        return ERROR_PPP_INVALID_PACKET;
    }

    if (memcmp(ConfigReqSent, pConfig->Data, dwLength) != 0) {
        return ERROR_PPP_INVALID_PACKET;
    }

    return NO_ERROR;
}

/***************************************************************************\
* LcpConfigNakReceived
*
\***************************************************************************/

DWORD LcpConfigNakReceived(PPP_CONFIG *pConfig)
{
    LONG cbConfig = WireToHostFormat16(pConfig->Length) - PPP_CONFIG_HDR_LEN;
    PPP_OPTION *pOption = (PPP_OPTION*)pConfig->Data;
    DWORD dwLastOption = 0;
    DWORD dwResult;

    while (cbConfig > 0) {

        if (pOption->Length == 0) {
            return ERROR_PPP_INVALID_PACKET;
        }

        cbConfig -= pOption->Length;

        if (cbConfig < 0) {
            return ERROR_PPP_INVALID_PACKET;
        }

        // If this option wasn't requested, mark it as negotiable.
        if (pOption->Type <= LCP_OPTION_LIMIT &&
                (gLcp.Local.WillNegotiate & (1 << pOption->Type)) &&
                !(gLcp.Local.Work.Negotiate & (1 << pOption->Type))) {
            gLcp.Local.Work.Negotiate |= (1 << pOption->Type);
        }

        dwLastOption = pOption->Type;

        dwResult = CheckOption(&gLcp.Local, pOption, FALSE);

        if (dwResult == CONFIG_REJ && pOption->Type <= LCP_OPTION_LIMIT) {
            gLcp.Local.Work.Negotiate &= ~(1 << pOption->Type);
        }

        pOption = (PPP_OPTION*)((BYTE*)pOption + pOption->Length);
    }

    return NO_ERROR;
}

/***************************************************************************\
* LcpConfigRejReceived
*
\***************************************************************************/

DWORD LcpConfigRejReceived(PPP_CONFIG *pConfig)
{
    LONG cbConfig = WireToHostFormat16(pConfig->Length) - PPP_CONFIG_HDR_LEN;
    PPP_OPTION *pOption = (PPP_OPTION*)pConfig->Data;
    DWORD dwLastOption = 0;
    DWORD dwResult;
    BYTE ReqOption[LCP_DEFAULT_MRU];

    while (cbConfig > 0) {

        if (pOption->Length == 0) {
            return ERROR_PPP_INVALID_PACKET;
        }

        cbConfig -= pOption->Length;

        if (cbConfig < 0) {
            return ERROR_PPP_INVALID_PACKET;
        }

        // Can't receive an option out of order or an option that wasn't
        // requested.
        if (pOption->Type <= LCP_OPTION_LIMIT &&
                (pOption->Type < dwLastOption ||
                !(gLcp.Local.Work.Negotiate & (1 << pOption->Type)))) {
            return ERROR_PPP_INVALID_PACKET;
        }

        dwResult = MakeOption(&gLcp.Local.Work, pOption->Type,
                (PPP_OPTION*)ReqOption, sizeof(ReqOption));

        if (dwResult != NO_ERROR) {
            return dwResult;
        }

        if (memcmp(ReqOption, pOption, pOption->Length) != 0) {
            return ERROR_PPP_INVALID_PACKET;
        }

        dwLastOption = pOption->Type;

        if (pOption->Type <= LCP_OPTION_LIMIT) {
            gLcp.Local.Work.Negotiate &= ~(1 << pOption->Type);
        }

        pOption = (PPP_OPTION*)((BYTE*)pOption + pOption->Length);
    }

    return NO_ERROR;
}

/***************************************************************************\
* LcpThisLayerUp
*
\***************************************************************************/

DWORD LcpThisLayerUp(VOID)
{
    if (gLcp.Local.Work.Negotiate & LCP_N_ACCM) {
        gFraming.RecvAccm = gLcp.Local.Work.ACCM;
    }
    if (gLcp.Local.Work.Negotiate & LCP_N_PFC) {
        gFraming.fRecvPfc = gLcp.Local.Work.PFC;
    }
    if (gLcp.Local.Work.Negotiate & LCP_N_ACFC) {
        gFraming.fRecvAcfc = gLcp.Local.Work.ACFC;
    }

    if (gLcp.Remote.Work.Negotiate & LCP_N_ACCM) {
        gFraming.SendAccm = gLcp.Remote.Work.ACCM;
    }
    if (gLcp.Remote.Work.Negotiate & LCP_N_PFC) {
        gFraming.fSendPfc = gLcp.Remote.Work.PFC;
    }
    if (gLcp.Remote.Work.Negotiate & LCP_N_ACFC) {
        gFraming.fSendAcfc = gLcp.Remote.Work.ACFC;
    }

    return NO_ERROR;
}

/***************************************************************************\
* LcpThisLayerDown
*
\***************************************************************************/

DWORD LcpThisLayerDown(VOID)
{
    if (gLcp.Local.Work.Negotiate & LCP_N_ACCM) {
        gFraming.RecvAccm = gLcpDefault.ACCM;
    }
    if (gLcp.Local.Work.Negotiate & LCP_N_PFC) {
        gFraming.fRecvPfc = gLcpDefault.PFC;
    }
    if (gLcp.Local.Work.Negotiate & LCP_N_ACFC) {
        gFraming.fRecvAcfc = gLcpDefault.ACFC;
    }

    if (gLcp.Remote.Work.Negotiate & LCP_N_ACCM) {
        gFraming.SendAccm = gLcpDefault.ACCM;
    }
    if (gLcp.Remote.Work.Negotiate & LCP_N_PFC) {
        gFraming.fSendPfc = gLcpDefault.PFC;
    }
    if (gLcp.Remote.Work.Negotiate & LCP_N_ACFC) {
        gFraming.fSendAcfc = gLcpDefault.ACFC;
    }

    return NO_ERROR;
}

/***************************************************************************\
* LcpEnd
*
\***************************************************************************/

DWORD LcpEnd(VOID)
{
    if (gLcp.Local.Work.pAPData != NULL) {
        Free(gLcp.Local.Work.pAPData);
    }
    if (gLcp.Remote.Work.pAPData != NULL) {
        Free(gLcp.Remote.Work.pAPData);
    }
    return NO_ERROR;
}

/***************************************************************************\
* LcpBegin
*
\***************************************************************************/

DWORD LcpBegin(VOID)
{
    CopyMemory(&gLcp.Local.Want, &gLcpDefault, sizeof(LCP_OPTIONS));
    CopyMemory(&gLcp.Remote.Want, &gLcpDefault, sizeof(LCP_OPTIONS));

    gLcp.Local.WillNegotiate = gLcpNegotiate;
    gLcp.Remote.WillNegotiate = gLcpNegotiate;

    gLcp.Remote.APsAvailable = LCP_AP_PAP;

    gLcp.Local.Want.MagicNumber = GetMagicNumber();
    gLcp.Remote.Want.MagicNumber = gLcp.Local.Want.MagicNumber + 1;

    gLcp.Local.Want.ACCM = 0;
    gLcp.Local.Want.PFC = TRUE;
    gLcp.Local.Want.ACFC = TRUE;

    gLcp.Remote.Want.ACCM = 0;
    gLcp.Remote.Want.PFC = TRUE;
    gLcp.Remote.Want.ACFC = TRUE;

    gLcp.Local.Want.Negotiate = (LCP_N_MAGIC | LCP_N_ACCM | LCP_N_PFC | LCP_N_ACFC);
    gLcp.Remote.Want.Negotiate = (LCP_N_MAGIC | LCP_N_ACCM | LCP_N_PFC | LCP_N_ACFC);

    CopyMemory(&gLcp.Local.Work, &gLcp.Local.Want, sizeof(LCP_OPTIONS));
    CopyMemory(&gLcp.Remote.Work, &gLcp.Remote.Want, sizeof(LCP_OPTIONS));

    return NO_ERROR;
}

/***************************************************************************\
* GetLcpInfo
*
\***************************************************************************/

VOID GetLcpInfo(PPP_CP_INFO *pInfo)
{
    ZeroMemory(pInfo, sizeof(PPP_CP_INFO));
    pInfo->Protocol = PPP_LCP_PROTOCOL;
    pInfo->Recognize = DISCARD_REQ;
    pInfo->CP_Begin = LcpBegin;
    pInfo->CP_End = LcpEnd;
    pInfo->CP_ThisLayerDown = LcpThisLayerDown;
    pInfo->CP_ThisLayerUp = LcpThisLayerUp;
    pInfo->CP_ConfigNakReceived = LcpConfigNakReceived;
    pInfo->CP_ConfigRejReceived = LcpConfigRejReceived;
    pInfo->CP_ConfigAckReceived = LcpConfigAckReceived;
    pInfo->CP_MakeConfigResult = LcpMakeConfigResult;
    pInfo->CP_MakeConfigRequest = LcpMakeConfigRequest;
}

