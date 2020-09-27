/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
* This file is part of the Microsoft Private Communication Technology
* reference implementation, version 1.0
*
* The Private Communication Technology reference implementation, version 1.0
* ("PCTRef"), is being provided by Microsoft to encourage the development and
* enhancement of an open standard for secure general-purpose business and
* personal communications on open networks.  Microsoft is distributing PCTRef
* at no charge irrespective of whether you use PCTRef for non-commercial or
* commercial use.
*
* Microsoft expressly disclaims any warranty for PCTRef and all derivatives of
* it.  PCTRef and any related documentation is provided "as is" without
* warranty of any kind, either express or implied, including, without
* limitation, the implied warranties or merchantability, fitness for a
* particular purpose, or noninfringement.  Microsoft shall have no obligation
* to provide maintenance, support, upgrades or new releases to you or to anyone
* receiving from you PCTRef or your modifications.  The entire risk arising out
* of use or performance of PCTRef remains with you.
*
* Please see the file LICENSE.txt,
* or http://pct.microsoft.com/pct/pctlicen.txt
* for more information on licensing.
*
* Please see http://pct.microsoft.com/pct/pct.htm for The Private
* Communication Technology Specification version 1.0 ("PCT Specification")
*
* 1/23/96
*----------------------------------------------------------------------------*/


#include <spbase.h>
#include <pct2msg.h>

#define PCT_OFFSET_OF(t, v) (DWORD)&(((t)NULL)->v)


#define SIZEOF(pHeader)    COMBINEBYTES(((PPCT2_HS_MESSAGE_HEADER) pHeader)->RecHeader.LengthMSB, ((PPCT2_HS_MESSAGE_HEADER) pHeader)->RecHeader.LengthLSB)

DWORD MapCipherToExternal(CipherSpec Internal, ExtCipherSpec *External)
{
    *External = htonl(Internal);
    return TRUE;
}

DWORD MapHashToExternal(HashSpec Internal, ExtHashSpec *External)
{
    *External = htons((ExtHashSpec)Internal);
    return TRUE;
}

DWORD MapCertToExternal(CertSpec Internal, ExtCertSpec *External)
{
    *External = htons((ExtCertSpec)Internal);
    return TRUE;
}

DWORD MapExchToExternal(ExchSpec Internal, ExtExchSpec *External)
{
    *External = htons((ExtExchSpec)Internal);
    return TRUE;
}

DWORD MapMsgToExternal(Pct2MsgSpec Internal, ExtMsgSpec *External)
{
    *External = htons((ExtMsgSpec)Internal);
    return TRUE;
}

DWORD MapSigToExternal(SigSpec Internal, ExtSigSpec *External)
{
    *External = htons((ExtSigSpec)Internal);
    return TRUE;
}

CipherSpec MapCipherFromExternal(ExtCipherSpec External)
{
    return (CipherSpec)ntohl(External);
}

HashSpec MapHashFromExternal(ExtHashSpec External)
{
    return (HashSpec)ntohs(External);
}

CertSpec MapCertFromExternal(ExtCertSpec External)
{
    return (CertSpec)ntohs(External);
}

ExchSpec MapExchFromExternal(ExtExchSpec External)
{
    return (ExchSpec)ntohs(External);
}
Pct2MsgSpec MapMsgFromExternal(ExtMsgSpec External)
{
    return (Pct2MsgSpec)ntohs(External);
}

SigSpec MapSigFromExternal(ExtSigSpec External)
{
    return (SigSpec)ntohs(External);
}

#define CALC_PUBLIC_VALUE_SIZE(v)   (v)->cbUserInfo +   \
                               (v)->cbParameter1 +  \
                               (v)->cbParameter2 +  \
                                PCT_OFFSET_OF(PPCT2_PUBLIC_VALUE, VariantData)

#define CALC_ENCRYPTED_KEY_SIZE(k)  (k)->cbEncryptedKey1 +     \
                               (k)->cbEncryptedKey2 +      \
                               (k)->cbKeyArg +              \
                               PCT_OFFSET_OF(PPCT2_ENCRYPTED_KEY, VariantData)


#define CALC_CANONICAL_PUBLIC_VALUE_SIZE(s) s - PCT_OFFSET_OF(PPCT2_PUBLIC_VALUE, VariantData) + sizeof(Pct2_Public_Value)

#define CALC_CANONICAL_ENCRYPTED_KEY_SIZE(s) s - PCT_OFFSET_OF(PPCT2_ENCRYPTED_KEY, VariantData) + sizeof(Pct2_Encrypted_Key)

/* Helper to pack a PublicValue */
DWORD
Pct2PackPublicValue(
    PPct2_Public_Value   pCanonical,
    PUCHAR               pOut)
{
     PPCT2_PUBLIC_VALUE   pValue = (PPCT2_PUBLIC_VALUE)pOut;
     PUCHAR pBuffer = pValue->VariantData;
     pValue->ValueTypeMsb = MSBOF(pCanonical->ValueType);
     pValue->ValueTypeLsb = LSBOF(pCanonical->ValueType);
     pValue->UserSpecInfoLenMsb = MSBOF(pCanonical->cbUserInfo);
     pValue->UserSpecInfoLenLsb = LSBOF(pCanonical->cbUserInfo);
     pValue->Parameter1LenMsb = MSBOF(pCanonical->cbParameter1);
     pValue->Parameter1LenLsb = LSBOF(pCanonical->cbParameter1);
     pValue->Parameter2LenMsb = MSBOF(pCanonical->cbParameter2);
     pValue->Parameter2LenLsb = LSBOF(pCanonical->cbParameter2);
     CopyMemory(pBuffer, pCanonical->pUserInfo, pCanonical->cbUserInfo);
     pBuffer +=  pCanonical->cbUserInfo;

     CopyMemory(pBuffer, pCanonical->pUserInfo, pCanonical->cbParameter1);
     pBuffer +=  pCanonical->cbParameter1;

     CopyMemory(pBuffer, pCanonical->pUserInfo, pCanonical->cbParameter2);
     return CALC_PUBLIC_VALUE_SIZE(pCanonical);


}

DWORD
Pct2UnpackPublicValue(
    PUCHAR pIn,
    PPct2_Public_Value pCanonical)
{
    PPCT2_PUBLIC_VALUE   pValue = (PPCT2_PUBLIC_VALUE)pIn;
    pCanonical->ValueType = COMBINEBYTES(pValue->ValueTypeMsb, pValue->ValueTypeLsb);
    pCanonical->cbUserInfo = COMBINEBYTES(pValue->UserSpecInfoLenMsb, pValue->UserSpecInfoLenLsb);
    pCanonical->cbParameter1 = COMBINEBYTES(pValue->Parameter1LenMsb, pValue->Parameter1LenLsb);
    pCanonical->cbParameter2 = COMBINEBYTES(pValue->Parameter2LenMsb, pValue->Parameter2LenLsb);

    CopyMemory((PUCHAR)pCanonical+1, pValue->VariantData, pCanonical->cbUserInfo +
                                                          pCanonical->cbParameter1 +
                                                          pCanonical->cbParameter2);
    pCanonical->pUserInfo = (PUCHAR)(pCanonical+1);
    pCanonical->pParameter1 = pCanonical->pUserInfo + pCanonical->cbUserInfo;
    pCanonical->pParameter2 = pCanonical->pParameter1 + pCanonical->cbParameter1;

    return sizeof(Pct2_Public_Value) +
           pCanonical->cbUserInfo +
           pCanonical->cbParameter1 +
           pCanonical->cbParameter2;


}

/* Helper to pack a Encrypted_Key */
DWORD
Pct2PackEncryptedKey(
    PPct2_Encrypted_Key  pCanonical,
    PUCHAR               pOut)
{
     PPCT2_ENCRYPTED_KEY   pValue = (PPCT2_ENCRYPTED_KEY)pOut;
     PUCHAR pBuffer = pValue->VariantData;
     pValue->EncryptedKey1LenMsb = MSBOF(pCanonical->cbEncryptedKey1);
     pValue->EncryptedKey1LenLsb = LSBOF(pCanonical->cbEncryptedKey1);
     pValue->EncryptedKey2LenMsb = MSBOF(pCanonical->cbEncryptedKey2);
     pValue->EncryptedKey2LenLsb = LSBOF(pCanonical->cbEncryptedKey2);

     pValue->KeyArgLenMsb = MSBOF(pCanonical->cbKeyArg);
     pValue->KeyArgLenLsb = LSBOF(pCanonical->cbKeyArg);
     CopyMemory(pBuffer, pCanonical->pEncryptedKey1, pCanonical->cbEncryptedKey1);
     pBuffer +=  pCanonical->cbEncryptedKey1;

     CopyMemory(pBuffer, pCanonical->pEncryptedKey2, pCanonical->cbEncryptedKey2);
     pBuffer +=  pCanonical->cbEncryptedKey2;

     CopyMemory(pBuffer, pCanonical->pKeyArg, pCanonical->cbKeyArg);
     return CALC_ENCRYPTED_KEY_SIZE(pCanonical);

}

DWORD
Pct2UnpackEncryptedKey(
    PUCHAR pIn,
    PPct2_Encrypted_Key pCanonical)
{
    PPCT2_ENCRYPTED_KEY   pKey = (PPCT2_ENCRYPTED_KEY)pIn;
    pCanonical->cbEncryptedKey1 = COMBINEBYTES(pKey->EncryptedKey1LenMsb, pKey->EncryptedKey1LenLsb);
    pCanonical->cbEncryptedKey2 = COMBINEBYTES(pKey->EncryptedKey2LenMsb, pKey->EncryptedKey2LenLsb);
    pCanonical->cbKeyArg = COMBINEBYTES(pKey->KeyArgLenMsb, pKey->KeyArgLenLsb);

    CopyMemory((PUCHAR)pCanonical+1, pKey->VariantData, pCanonical->cbEncryptedKey1 +
                                                        pCanonical->cbEncryptedKey2+
                                                        pCanonical->cbKeyArg);
    pCanonical->pEncryptedKey1 = (PUCHAR)(pCanonical+1);
    pCanonical->pEncryptedKey2 = pCanonical->pEncryptedKey1 + pCanonical->cbEncryptedKey1;
    pCanonical->pKeyArg = pCanonical->pEncryptedKey2 + pCanonical->cbEncryptedKey2;

    return sizeof(Pct2_Encrypted_Key) +
           pCanonical->cbEncryptedKey1 +
           pCanonical->cbEncryptedKey2 +
           pCanonical->cbKeyArg;

}



SP_STATUS
Pct2PackClientHello(
    PPct2_Client_Hello pCanonical,
    PSPBuffer          pCommOutput)
{
    DWORD                    cbMessage;
    PPCT2_CLIENT_HELLO       pMessage;
    PPCT2_HS_MESSAGE_HEADER  pHeader;
    DWORD               Size;
    PUCHAR              pBuffer;
    DWORD               i, iBuff;
    DWORD               cbQuickPublicValue=0;
    DWORD               cbQuickServerSize=0;
    DWORD               cbQuickEncryptedKey=0;

    if(pCanonical == NULL || pCommOutput == NULL) return PCT_INT_INTERNAL_ERROR;
    pCommOutput->cbData = 0;

    /* Now add on the Quick Value sizes, if there are any */

    if(pCanonical->pQuickPublicValue) {
        cbQuickPublicValue =   CALC_PUBLIC_VALUE_SIZE(pCanonical->pQuickPublicValue);

    }

    if(pCanonical->pQuickServerPublicValue) {
        cbQuickServerSize    = CALC_PUBLIC_VALUE_SIZE(pCanonical->pQuickServerPublicValue);

    }
    if(pCanonical->pQuickEncryptedKey) {
        cbQuickEncryptedKey  = CALC_ENCRYPTED_KEY_SIZE(pCanonical->pQuickEncryptedKey);

    }

    cbMessage = PCT_OFFSET_OF(PPCT2_CLIENT_HELLO, VariantData) +
                    pCanonical->cCipherSpecs * sizeof(ExtCipherSpec) +
                    pCanonical->cHashSpecs * sizeof(ExtHashSpec) +
                    pCanonical->cCertSpecs * sizeof(ExtCertSpec) +
                    pCanonical->cExchSpecs * sizeof(ExtExchSpec) +
                    pCanonical->cbKeyArg +
                    pCanonical->cMessageSpecs * sizeof(ExtMsgSpec) +
                    pCanonical->cSigSpecs * sizeof(ExtSigSpec) +
                    pCanonical->cbCertifiers +
                    cbQuickPublicValue +
                    cbQuickServerSize +
                    cbQuickEncryptedKey;


    if (cbMessage > PCT_MAX_SHAKE_LEN)
        return PCT_INT_DATA_OVERFLOW;

    pCommOutput->cbData = cbMessage + sizeof(PCT2_HS_MESSAGE_HEADER);
    /* are we allocating our own memory? */
    if(pCommOutput->pvBuffer == NULL) {
        pCommOutput->pvBuffer = SPExternalAlloc(pCommOutput->cbData);
        if (NULL == pCommOutput->pvBuffer)
            return PCT_INT_INTERNAL_ERROR;
        pCommOutput->cbBuffer = pCommOutput->cbData;
    }

    /* We cannot divide up a client hello into multiple messages */
    if(pCommOutput->cbData > pCommOutput->cbBuffer) return PCT_INT_BUFF_TOO_SMALL;

    pHeader = (PPCT2_HS_MESSAGE_HEADER)(pCommOutput->pvBuffer);

    pMessage = (PPCT2_CLIENT_HELLO)(pHeader+1);


    CopyMemory( pMessage->SessionIdData,
                pCanonical->SessionID,
                pCanonical->cbSessionID);

    CopyMemory( pMessage->ChallengeData,
                pCanonical->Challenge,
                pCanonical->cbChallenge);

    pMessage->VersionMsb = MSBOF(PCT_VERSION_2);
    pMessage->VersionLsb = LSBOF(PCT_VERSION_2);

    pMessage->OffsetMsb = MSBOF(PCT_CH_OFFSET_V2);
    pMessage->OffsetLsb = LSBOF(PCT_CH_OFFSET_V2);

    pBuffer = pMessage->VariantData;

    /* Pack cipher specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cCipherSpecs ; i++ )
    {
        if (MapCipherToExternal(pCanonical->pCipherSpecs[i],
                                &((ExtCipherSpec *) pBuffer)[iBuff]) )
        {
            iBuff++;
        }
    }
    Size = iBuff*sizeof(ExtCipherSpec);

    pMessage->CipherSpecsLenMsb = MSBOF(Size);
    pMessage->CipherSpecsLenLsb = LSBOF(Size);
    pBuffer += Size;

    cbMessage -= (pCanonical->cCipherSpecs - iBuff)*sizeof(ExtCipherSpec);

    /* Pack Hash Specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cHashSpecs ; i++ )
    {
        if (MapHashToExternal(pCanonical->pHashSpecs[i],
                              &((ExtHashSpec *) pBuffer)[iBuff]) )
        {
            iBuff++;
        }
    }
    Size = iBuff*sizeof(ExtHashSpec);
    pBuffer += Size;

    pMessage->HashSpecsLenMsb = MSBOF(Size);
    pMessage->HashSpecsLenLsb = LSBOF(Size);
    cbMessage -= (pCanonical->cHashSpecs - iBuff)*sizeof(ExtHashSpec);

    /* pack Cert specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cCertSpecs ; i++ )
    {
        if (MapCertToExternal(pCanonical->pCertSpecs[i],
                                &((ExtCertSpec *) pBuffer)[iBuff]))
        {
            iBuff ++;
        }
    }
    Size = iBuff*sizeof(ExtCertSpec);
    pBuffer += Size;

    pMessage->CertSpecsLenMsb = MSBOF(Size);
    pMessage->CertSpecsLenLsb = LSBOF(Size);
    cbMessage -= (pCanonical->cCertSpecs - iBuff)*sizeof(ExtCertSpec);

    /* Pack Exch Specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cExchSpecs ; i++ )
    {
        if (MapExchToExternal(pCanonical->pExchSpecs[i],
                                &((ExtExchSpec *) pBuffer)[iBuff]) )
        {
            iBuff++;
        }
    }
    Size = iBuff*sizeof(ExtExchSpec);
    pBuffer += Size;

    pMessage->ExchSpecsLenMsb = MSBOF(Size);
    pMessage->ExchSpecsLenLsb = LSBOF(Size);
    cbMessage -= (pCanonical->cExchSpecs - iBuff)*sizeof(ExtExchSpec);

    CopyMemory(pBuffer, pCanonical->pKeyArg, pCanonical->cbKeyArg);
    pBuffer += pCanonical->cbKeyArg;
    /* End of PCT1 compatable data */

    /* Pack Msg Specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cMessageSpecs ; i++ )
    {
        if (MapMsgToExternal(pCanonical->pMessageSpecs[i],
                                &((ExtMsgSpec *) pBuffer)[iBuff]) )
        {
            iBuff++;
        }
    }
    Size = iBuff*sizeof(ExtMsgSpec);
    pBuffer += Size;

    pMessage->MsgListLenMsb = MSBOF(Size);
    pMessage->MsgListLenLsb = LSBOF(Size);
    cbMessage -= (pCanonical->cMessageSpecs - iBuff)*sizeof(ExtMsgSpec);

    /* Pack Sig Specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cSigSpecs ; i++ )
    {
        if (MapSigToExternal(pCanonical->pSigSpecs[i],
                                &((ExtSigSpec *) pBuffer)[iBuff]) )
        {
            iBuff++;
        }
    }
    Size = iBuff*sizeof(ExtSigSpec);
    pBuffer += Size;

    pMessage->SigListLenMsb = MSBOF(Size);
    pMessage->SigListLenLsb = LSBOF(Size);
    cbMessage -= (pCanonical->cSigSpecs - iBuff)*sizeof(ExtSigSpec);

    /* Certifier list data */
    CopyMemory(pBuffer, pCanonical->pCertifiers, pCanonical->cbCertifiers);
    pMessage->CertifierListLenMsb = MSBOF(pCanonical->cbCertifiers);
    pMessage->CertifierListLenLsb = LSBOF(pCanonical->cbCertifiers);
    pBuffer += pCanonical->cbCertifiers;

    if(pCanonical->pQuickPublicValue) {
        pBuffer += Pct2PackPublicValue(pCanonical->pQuickPublicValue, pBuffer);
    }
    pMessage->QuickPublicValueLenMsb = MSBOF(cbQuickPublicValue);
    pMessage->QuickPublicValueLenLsb = LSBOF(cbQuickPublicValue);

    if(pCanonical->pQuickServerPublicValue) {
        pBuffer += Pct2PackPublicValue(pCanonical->pQuickServerPublicValue, pBuffer);
    }
    pMessage->QuickServerPublicValueLenMsb = MSBOF(cbQuickServerSize);
    pMessage->QuickServerPublicValueLenLsb = LSBOF(cbQuickServerSize);

    if(pCanonical->pQuickEncryptedKey){
        pBuffer += Pct2PackEncryptedKey(pCanonical->pQuickEncryptedKey, pBuffer);
    }
    pMessage->QuickEncryptedKeyLenMsb = MSBOF(cbQuickEncryptedKey);
    pMessage->QuickEncryptedKeyLenLsb = LSBOF(cbQuickEncryptedKey);

    pMessage->AuthOptionsMsb = MSBOF(pCanonical->AuthOptions);
    pMessage->AuthOptionsLsb = LSBOF(pCanonical->AuthOptions);

    pCommOutput->cbData = cbMessage + sizeof(PCT2_HS_MESSAGE_HEADER);

    pHeader->RecHeader.LengthMSB = MSBOF(cbMessage) | 0x80;
    pHeader->RecHeader.LengthLSB = LSBOF(cbMessage);
    pHeader->RecHeader.TypeMSB = MSBOF(PCT2_RT_HANDSHAKE);
    pHeader->RecHeader.TypeLSB = MSBOF(PCT2_RT_HANDSHAKE);
    pHeader->MessageTypeMsb = MSBOF(PCT2_HS_CLIENT_HELLO);
    pHeader->MessageTypeMsb = LSBOF(PCT2_HS_CLIENT_HELLO);
    pHeader->RecordFlagsMsb = 0; /* we can't break up message in CH */
    pHeader->RecordFlagsLsb = 0; /* we can't break up message in CH */

    return(PCT_ERR_OK);
}

SP_STATUS
Pct2UnpackClientHello(
    PSPBuffer          pInput,
    PPct2_Client_Hello *     ppClient)
{

    PPCT2_HS_MESSAGE_HEADER  pHeader;
    PPCT2_CLIENT_HELLO   pMessage;

    DWORD               ReportedSize;
    DWORD               CipherSpecsSize, HashSpecsSize, CertSpecsSize;
    DWORD               ExchSpecsSize, MsgSpecsSize, SigSpecsSize;
    DWORD               cbCertifierList, cbQuickPub, cbQuickServerPub;
    DWORD               cbQuickEncryptedKey;

    DWORD               cCipherSpecs, cHashSpecs, cCertSpecs, cExchSpecs,
                        cMsgSpecs, cSigSpecs;

    DWORD               cbKeyArg;
    DWORD               cOffsetBytes;
    PPct2_Client_Hello  pCanonical;
    PUCHAR              pBuffer, pCurUnpack;
    DWORD               i;
    DWORD               cbCanonical;

    pHeader = pInput->pvBuffer;

    if(pInput->cbBuffer < sizeof(PCT2_HS_MESSAGE_HEADER)) {
        return PCT_INT_INCOMPLETE_MSG;
    }

    /*******
     * NOTE: For the client hello only, we are guaranteed that the
     * complete message will come as one record
     *******/
    ReportedSize = SIZEOF(pHeader);

    if ((ReportedSize+sizeof(PCT2_HS_MESSAGE_HEADER)) > pInput->cbBuffer)
    {
        return PCT_INT_INCOMPLETE_MSG;
    }

    if(ReportedSize < PCT_OFFSET_OF(PPCT2_CLIENT_HELLO, VariantData))
    {
        return PCT_ERR_ILLEGAL_MESSAGE;
    }
    /* Now start validating the message */
    pMessage = (PPCT2_CLIENT_HELLO)(pHeader+1);

    if(COMBINEBYTES(pHeader->MessageTypeMsb, pHeader->MessageTypeLsb) !=
       PCT2_HS_CLIENT_HELLO)
    {
        return PCT_ERR_ILLEGAL_MESSAGE;
    }
    /* Client hello messages can't be chopped */
    if(COMBINEBYTES(pHeader->RecordFlagsMsb, pHeader->RecordFlagsLsb) &
       ( PCT2_HS_FLAG_TO_BE_CONTD | PCT2_HS_FLAG_CONTINUATION))
    {
        return PCT_ERR_ILLEGAL_MESSAGE;
    }


    /* We don't recognize hello messages of less version than ourselves,
     * those will be handled by a previous version of the code */
    if(COMBINEBYTES(pMessage->VersionMsb, pMessage->VersionLsb) < PCT_VERSION_2)
    {
        return PCT_ERR_ILLEGAL_MESSAGE;
    }

    cOffsetBytes = COMBINEBYTES( pMessage->OffsetMsb,
                                  pMessage->OffsetLsb );

    if(cOffsetBytes < PCT_CH_OFFSET_V2)  {
        return(PCT_ERR_ILLEGAL_MESSAGE);
    }

    *ppClient = NULL;

    CipherSpecsSize = COMBINEBYTES( pMessage->CipherSpecsLenMsb,
                                    pMessage->CipherSpecsLenLsb );

    HashSpecsSize = COMBINEBYTES( pMessage->HashSpecsLenMsb,
                                  pMessage->HashSpecsLenLsb );

    CertSpecsSize = COMBINEBYTES( pMessage->CertSpecsLenMsb,
                                  pMessage->CertSpecsLenLsb );

    ExchSpecsSize = COMBINEBYTES( pMessage->ExchSpecsLenMsb,
                                  pMessage->ExchSpecsLenLsb );

    MsgSpecsSize =  COMBINEBYTES( pMessage->MsgListLenMsb,
                                  pMessage->MsgListLenLsb );

    SigSpecsSize =  COMBINEBYTES( pMessage->SigListLenMsb,
                                  pMessage->SigListLenLsb );

    cbKeyArg     =  COMBINEBYTES( pMessage->KeyArgLenMsb,
                                  pMessage->KeyArgLenLsb );


    cbCertifierList = COMBINEBYTES(pMessage->CertifierListLenMsb,
                                   pMessage->CertifierListLenLsb);

    cbQuickPub      = COMBINEBYTES(pMessage->QuickPublicValueLenMsb,
                                   pMessage->QuickPublicValueLenLsb);

    cbQuickServerPub= COMBINEBYTES(pMessage->QuickServerPublicValueLenMsb,
                                   pMessage->QuickServerPublicValueLenLsb);

    cbQuickEncryptedKey= COMBINEBYTES(pMessage->QuickEncryptedKeyLenMsb,
                                   pMessage->QuickEncryptedKeyLenLsb);


    /* we do this to precalc the offset to certifier list */
    i = PCT_OFFSET_OF(PPCT2_CLIENT_HELLO, VariantData)
         + cOffsetBytes - PCT_CH_OFFSET_V2
         + CipherSpecsSize
         + HashSpecsSize
         + CertSpecsSize
         + ExchSpecsSize
         + MsgSpecsSize
         + SigSpecsSize
         + cbKeyArg;

    /* check that this all fits into the message */
    if (i + cbCertifierList
         + cbQuickPub
         + cbQuickServerPub
         +cbQuickEncryptedKey  > ReportedSize)
    {
        return PCT_ERR_ILLEGAL_MESSAGE;
    }

    cCipherSpecs = CipherSpecsSize / sizeof(ExtCipherSpec);
    cHashSpecs = HashSpecsSize / sizeof(ExtHashSpec);
    cExchSpecs = ExchSpecsSize / sizeof(ExtExchSpec);
    cCertSpecs = CertSpecsSize / sizeof(ExtCertSpec);
    cMsgSpecs  = MsgSpecsSize / sizeof(ExtMsgSpec);
    cSigSpecs  = SigSpecsSize / sizeof(ExtSigSpec);

    /* Count certifiers */
    cbCanonical =  sizeof(Pct2_Client_Hello) +
                   cbKeyArg +
                   cCipherSpecs * sizeof(CipherSpec) +
                   cHashSpecs * sizeof(HashSpec) +
                   cCertSpecs * sizeof(CertSpec) +
                   cExchSpecs * sizeof(ExchSpec) +
                   cMsgSpecs * sizeof(Pct2MsgSpec) +
                   cSigSpecs * sizeof(SigSpec) +
                   cbCertifierList;

    if(cbQuickPub) cbCanonical += CALC_CANONICAL_PUBLIC_VALUE_SIZE(cbQuickPub);
    if(cbQuickServerPub) cbCanonical += CALC_CANONICAL_PUBLIC_VALUE_SIZE(cbQuickServerPub);
    if(cbQuickEncryptedKey) cbCanonical += CALC_CANONICAL_ENCRYPTED_KEY_SIZE(cbQuickEncryptedKey);


    /* Allocate a buffer for the canonical client hello */
    pCanonical = (PPct2_Client_Hello)SPExternalAlloc(cbCanonical);

    if (!pCanonical)
    {
        return(PCT_INT_INTERNAL_ERROR);
    }


     /* Begin Unpacking Client Hello */

    pCanonical->AuthOptions = COMBINEBYTES(pMessage->AuthOptionsMsb, pMessage->AuthOptionsLsb);

    CopyMemory( pCanonical->SessionID,
                pMessage->SessionIdData,
                PCT2_SESSION_ID_SIZE);
    pCanonical->cbSessionID = PCT2_SESSION_ID_SIZE;

    CopyMemory( pCanonical->Challenge,
                pMessage->ChallengeData,
                PCT2_CHALLENGE_SIZE );
    pCanonical->cbChallenge = PCT2_CHALLENGE_SIZE;

    /* Pre-set all sizes */
    pCanonical->cHashSpecs = cHashSpecs;
    pCanonical->cCipherSpecs = cCipherSpecs;
    pCanonical->cCertSpecs = cCertSpecs;
    pCanonical->cExchSpecs = cExchSpecs;
    pCanonical->cbKeyArg = cbKeyArg;
    pCanonical->cMessageSpecs = cMsgSpecs;
    pCanonical->cSigSpecs = cSigSpecs;
    pCanonical->cbCertifiers = cbCertifierList;

    /* Start unpacking the variant data */
    pBuffer = &pMessage->OffsetLsb + 1 + cOffsetBytes;

    /* Pointer to free memory in canonical */
    pCurUnpack = (PUCHAR)(pCanonical + 1);



    /* Unpack Cipher Specs */
    if(pCanonical->cCipherSpecs) {
        pCanonical->pCipherSpecs = (PCipherSpec)pCurUnpack;
        for (i = 0 ; i < cCipherSpecs ; i++ )
        {
            pCanonical->pCipherSpecs[i] = MapCipherFromExternal(*(ExtCipherSpec *)
                                                        pBuffer);
        }
        pCurUnpack += sizeof(CipherSpec)*pCanonical->cCipherSpecs;
        pBuffer += sizeof(ExtCipherSpec)*pCanonical->cCipherSpecs;
    }

    /* Unpack Hash Specs */
    if(pCanonical->cHashSpecs) {
        pCanonical->pHashSpecs = (PHashSpec)pCurUnpack;

        for (i = 0 ; i < cHashSpecs ; i++ )
        {
            pCanonical->pHashSpecs[i] = MapHashFromExternal(*(ExtHashSpec *)
                                                        pBuffer);
        }
        pCurUnpack += sizeof(HashSpec)*pCanonical->cHashSpecs;
        pBuffer += sizeof(ExtHashSpec)*pCanonical->cHashSpecs;
    }

    /* Unpack Cert Specs */
    if(pCanonical->cCertSpecs) {
        pCanonical->pCertSpecs = (PCertSpec) pCurUnpack;
        for (i = 0 ; i < cCertSpecs ; i++ )
        {
            pCanonical->pCertSpecs[i] = MapCertFromExternal(*(ExtCertSpec *)
                                                        pBuffer);
        }
        pCurUnpack += sizeof(CertSpec)*pCanonical->cCertSpecs;
        pBuffer += sizeof(ExtCertSpec)*pCanonical->cCertSpecs;
    }


    /* Unpack Key Exchange Specs */
    if(pCanonical->cExchSpecs) {
        pCanonical->pExchSpecs = (PExchSpec)pCurUnpack;
        for (i = 0 ; i < cExchSpecs ; i++ )
        {
            pCanonical->pExchSpecs[i] = MapExchFromExternal(*(ExtExchSpec *)
                                                        pBuffer);
        }
        pBuffer += sizeof(ExtExchSpec)*pCanonical->cExchSpecs;
        pCurUnpack += sizeof(ExchSpec)*pCanonical->cExchSpecs;
    }

    /* Unpack KeyArg */
    if(pCanonical->cbKeyArg) {
        pCanonical->pKeyArg = pCurUnpack;
        CopyMemory(pCanonical->pKeyArg, pBuffer, cbKeyArg);
        pBuffer += pCanonical->cbKeyArg;
        pCurUnpack += pCanonical->cbKeyArg;
    }

    /* Unpack Messages */
    if(pCanonical->cMessageSpecs) {
        pCanonical->pMessageSpecs = (PPct2MsgSpec)pCurUnpack;
        for (i = 0 ; i < cMsgSpecs ; i++ )
        {
            pCanonical->pMessageSpecs[i] = MapMsgFromExternal(*(ExtMsgSpec *)
                                                        pBuffer);
        }
        pBuffer += sizeof(ExtMsgSpec)*pCanonical->cMessageSpecs;
        pCurUnpack += sizeof(Pct2MsgSpec)*pCanonical->cMessageSpecs;
    }

    /* Unpack Sig Specs */
    if(pCanonical->cSigSpecs) {
        pCanonical->pSigSpecs = (PSigSpec)pCurUnpack;
        for (i = 0 ; i < cSigSpecs ; i++ )
        {
            pCanonical->pSigSpecs[i] = MapSigFromExternal(*(ExtSigSpec *)
                                                        pBuffer);
        }
        pBuffer += sizeof(ExtSigSpec)*pCanonical->cSigSpecs;
        pCurUnpack += sizeof(SigSpec)*pCanonical->cSigSpecs;
    }

    /* Unpack the certifier list */

    if(pCanonical->cbCertifiers) {
        pCanonical->pCertifiers = (PUCHAR) pCurUnpack;
        CopyMemory(pCurUnpack, pBuffer, cbCertifierList);

        pBuffer += cbCertifierList;
        pCurUnpack += cbCertifierList;
    }

    if(cbQuickPub) {
        pCanonical->pQuickPublicValue = (PPct2_Public_Value)pCurUnpack;
        pCurUnpack += Pct2UnpackPublicValue(pBuffer, pCanonical->pQuickPublicValue);
        pBuffer += cbQuickPub;
    }
    if(cbQuickServerPub) {
        pCanonical->pQuickServerPublicValue = (PPct2_Public_Value)pCurUnpack;
        pCurUnpack += Pct2UnpackPublicValue(pBuffer, pCanonical->pQuickServerPublicValue);
        pBuffer += cbQuickServerPub;
    }
    if(cbQuickEncryptedKey) {
        pCanonical->pQuickEncryptedKey = (PPct2_Encrypted_Key)pCurUnpack;
        pCurUnpack += Pct2UnpackEncryptedKey(pBuffer, pCanonical->pQuickEncryptedKey);
        pBuffer += cbQuickEncryptedKey;
    }

    *ppClient = pCanonical;
    pInput->cbData = ReportedSize + sizeof(PCT2_RECORD_HEADER);
    return( PCT_ERR_OK);
}

BOOL RepackHsMessage(DWORD * pcbMessage,
                     PPCT2_HS_MESSAGE_HEADER *ppHeader,
                     PUCHAR *ppBuffer,
                     DWORD MsgType,
                     DWORD Flags)
{
    (*ppHeader)->RecHeader.LengthMSB = MSBOF(min(PCT_MAX_SHAKE_LEN, *pcbMessage));
    (*ppHeader)->RecHeader.LengthLSB = LSBOF(min(PCT_MAX_SHAKE_LEN, *pcbMessage));
    (*ppHeader)->RecHeader.TypeMSB = MSBOF(PCT2_RT_HANDSHAKE);
    (*ppHeader)->RecHeader.TypeLSB = LSBOF(PCT2_RT_HANDSHAKE);
    (*ppHeader)->MessageTypeMsb = MSBOF(MsgType);
    (*ppHeader)->MessageTypeLsb = LSBOF(MsgType);
    (*ppHeader)->RecordFlagsMsb = MSBOF(Flags);
    (*ppHeader)->RecordFlagsLsb = LSBOF(Flags);

    if(*pcbMessage > PCT_MAX_SHAKE_LEN) {
        *ppHeader = (PPCT2_HS_MESSAGE_HEADER)((PUCHAR)*ppHeader) + PCT_MAX_SHAKE_LEN;
        *pcbMessage -= PCT_MAX_SHAKE_LEN;
        CopyMemory((*ppHeader)+1, *ppHeader, *pcbMessage);
        *ppBuffer += sizeof(PCT2_HS_MESSAGE_HEADER);
        return TRUE;
    }
    return FALSE;
}

SP_STATUS
Pct2PackServerHello(
    PPct2_Server_Hello       pCanonical,
    PSPBuffer          pCommOutput)
{
    DWORD               cbMessage;
    PPCT2_HS_MESSAGE_HEADER pHeader;
    PPCT2_SERVER_HELLO      pMessage;
    DWORD               Size;
    PUCHAR              pBuffer;
    DWORD               i, iBuff;
    DWORD               cbPublicValue,  cbQuickEncryptedKey;

    DWORD               cbMessageSent;

    DWORD               cbCopied, iCopy;
    DWORD               Flags;

    /* Note, when packing messages that are longer than the max message length,
     * we break them into multiple records, and pack them all into one buffer
     */
    if(pCanonical == NULL || pCommOutput == NULL) return PCT_INT_INTERNAL_ERROR;
    pCommOutput->cbData = 0;

    /* Now add on the Quick Value sizes, if there are any */
    if(pCanonical->pPublicValue) {
        cbPublicValue =   CALC_PUBLIC_VALUE_SIZE(pCanonical->pPublicValue);

    }

    if(pCanonical->pQuickEncryptedKey) {
        cbQuickEncryptedKey  = CALC_ENCRYPTED_KEY_SIZE(pCanonical->pQuickEncryptedKey);

    }

    cbMessageSent =  PCT_OFFSET_OF(PPCT2_SERVER_HELLO, VariantData) +
                    pCanonical->cAltCipherSpecs * sizeof(ExtCipherSpec) +
                    pCanonical->cAltHashSpecs * sizeof(ExtHashSpec) +
                    pCanonical->cMessageSpecs * sizeof(ExtMsgSpec) +
                    pCanonical->cExchSpecs * sizeof(ExtExchSpec) +
                    pCanonical->cCertSpecs * sizeof(ExtCertSpec) +
                    cbQuickEncryptedKey +
                    pCanonical->cbResponse;

    cbMessage = cbMessageSent +
                pCanonical->cbCertifiers +
                cbPublicValue +
                pCanonical->cbSigCert;


    /* The number of message headers should be equal to the number
     * of blocks we split the message into.
     */
    pCommOutput->cbData = cbMessage + sizeof(PPCT2_HS_MESSAGE_HEADER)*((cbMessage / PCT_MAX_SHAKE_LEN)+1);

    /* are we allocating our own memory? */
    if(pCommOutput->pvBuffer == NULL) {
        pCommOutput->pvBuffer = SPExternalAlloc(pCommOutput->cbData);
        if (NULL == pCommOutput->pvBuffer)
            return PCT_INT_INTERNAL_ERROR;
        pCommOutput->cbBuffer = pCommOutput->cbData;
    }

    if(pCommOutput->cbData > pCommOutput->cbBuffer) return PCT_INT_BUFF_TOO_SMALL;


    pHeader = pCommOutput->pvBuffer;
    pMessage = (PPCT2_SERVER_HELLO)(pHeader+1);

    /* Build base portion of server hello message */
    pMessage->VersionMsb = MSBOF(PCT_VERSION_2);
    pMessage->VersionLsb = LSBOF(PCT_VERSION_2);
    pMessage->AuthOptionsMsb = MSBOF(pCanonical->AuthOptions);
    pMessage->AuthOptionsLsb = LSBOF(pCanonical->AuthOptions);

    MapCipherToExternal(pCanonical->SrvCipherSpec, &pMessage->CipherSpecData);
    MapHashToExternal(pCanonical->SrvHashSpec, &pMessage->HashSpecData);
    MapExchToExternal(pCanonical->SrvExchSpec, &pMessage->ExchSpecData);
    MapSigToExternal(pCanonical->SrvSigSpec, &pMessage->SigSpecData);

    CopyMemory(pMessage->ConnectionIdData, pCanonical->ConnectionID,
               pCanonical->cbConnectionID);

    CopyMemory(pMessage->SessionIdData, pCanonical->SessionID,
               pCanonical->cbSessionID);


    pBuffer = pMessage->VariantData;

    /* Start packing the various specs */

    /* Pack cipher specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cAltCipherSpecs ; i++ )
    {
        if (MapCipherToExternal(pCanonical->pAltCipherSpecs[i],
                                &((ExtCipherSpec *) pBuffer)[iBuff]) )
        {
            iBuff++;
        }
    }
    Size = iBuff*sizeof(ExtCipherSpec);

    pMessage->AltCipherListLenMsb = MSBOF(Size);
    pMessage->AltCipherListLenLsb = LSBOF(Size);
    pBuffer += Size;

    cbMessageSent -= (pCanonical->cAltCipherSpecs - iBuff)*sizeof(ExtCipherSpec);

    /* Pack Hash Specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cAltHashSpecs ; i++ )
    {
        if (MapHashToExternal(pCanonical->pAltHashSpecs[i],
                              &((ExtHashSpec *) pBuffer)[iBuff]) )
        {
            iBuff++;
        }
    }
    Size = iBuff*sizeof(ExtHashSpec);
    pBuffer += Size;

    pMessage->AltHashListLenMsb = MSBOF(Size);
    pMessage->AltHashListLenLsb = LSBOF(Size);
    cbMessageSent -= (pCanonical->cAltHashSpecs - iBuff)*sizeof(ExtHashSpec);
    /* Pack Msg Specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cMessageSpecs ; i++ )
    {
        if (MapMsgToExternal(pCanonical->pMessageSpecs[i],
                                &((ExtMsgSpec *) pBuffer)[iBuff]) )
        {
            iBuff++;
        }
    }
    Size = iBuff*sizeof(ExtMsgSpec);
    pBuffer += Size;

    pMessage->MsgListLenMsb = MSBOF(Size);
    pMessage->MsgListLenLsb = LSBOF(Size);
    cbMessageSent -= (pCanonical->cMessageSpecs - iBuff)*sizeof(ExtMsgSpec);

    /* Pack Exch Specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cExchSpecs ; i++ )
    {
        if (MapExchToExternal(pCanonical->pExchSpecs[i],
                                &((ExtExchSpec *) pBuffer)[iBuff]) )
        {
            iBuff++;
        }
    }
    Size = iBuff*sizeof(ExtExchSpec);
    pBuffer += Size;

    pMessage->ExchListLenMsb = MSBOF(Size);
    pMessage->ExchListLenLsb = LSBOF(Size);
    cbMessageSent -= (pCanonical->cExchSpecs - iBuff)*sizeof(ExtExchSpec);

    /* pack Cert specs */
    iBuff = 0;
    for (i = 0; i < pCanonical->cCertSpecs ; i++ )
    {
        if (MapCertToExternal(pCanonical->pCertSpecs[i],
                                &((ExtCertSpec *) pBuffer)[iBuff]))
        {
            iBuff ++;
        }
    }
    Size = iBuff*sizeof(ExtCertSpec);
    pBuffer += Size;

    pMessage->CertListLenMsb = MSBOF(Size);
    pMessage->CertListLenLsb = LSBOF(Size);
    cbMessageSent -= (pCanonical->cCertSpecs - iBuff)*sizeof(ExtCertSpec);




    if(pCanonical->pQuickEncryptedKey){
        pBuffer += Pct2PackEncryptedKey(pCanonical->pQuickEncryptedKey, pBuffer);
    }
    pMessage->QuickEncryptedKeyLenMsb = MSBOF(cbQuickEncryptedKey);
    pMessage->QuickEncryptedKeyLenLsb = LSBOF(cbQuickEncryptedKey);

    CopyMemory(pBuffer, pCanonical->pResponse, pCanonical->cbResponse);
    pMessage->ResponseLenMsb = MSBOF(pCanonical->cbResponse);
    pMessage->ResponseLenLsb = LSBOF(pCanonical->cbResponse);

    /* Okay, we shouldn't have run over our record length max yet, but just to
     * be sure, repack.
     */
    /* Certifier list data */

    CopyMemory(pBuffer, pCanonical->pCertifiers, pCanonical->cbCertifiers);
    pBuffer += pCanonical->cbCertifiers;
    pMessage->CertifierListLenMsb = MSBOF(pCanonical->cbCertifiers);
    pMessage->CertifierListLenLsb = LSBOF(pCanonical->cbCertifiers);


    if(pCanonical->pPublicValue) {
        pBuffer += Pct2PackPublicValue(pCanonical->pPublicValue, pBuffer);
    }
    pMessage->PublicValueLenMsb = MSBOF(cbPublicValue);
    pMessage->PublicValueLenLsb = LSBOF(cbPublicValue);

    while(cbMessageSent >= PCT_MAX_SHAKE_LEN) {
        if(Flags & PCT2_HS_FLAG_TO_BE_CONTD) Flags = PCT2_HS_FLAG_CONTINUATION;
        else if(Flags == 0) Flags = PCT2_HS_FLAG_TO_BE_CONTD;
        RepackHsMessage(&cbMessageSent, &pHeader, &pBuffer, PCT2_HS_SERVER_HELLO, Flags);
    }

    /* Okay, now pack the potentially big things */
    pMessage->SigCertLenMsb = MSBOF(pCanonical->cbSigCert);
    pMessage->SigCertLenLsb = LSBOF(pCanonical->cbSigCert);



    /* Yeah, I know this looks wierd, but we should not have to do any Copies
     * to move data out of the way of an inline header if we do this */

    iCopy = 0;
    while(iCopy < pCanonical->cbSigCert) {

        cbCopied = min(pCanonical->cbSigCert-iCopy, (PCT_MAX_SHAKE_LEN-cbMessageSent));
        CopyMemory(pBuffer, pCanonical->pSigCert + iCopy, cbCopied);
        iCopy += cbCopied;
        pBuffer += iCopy;

        while(cbMessageSent >= PCT_MAX_SHAKE_LEN) {
            if(Flags & PCT2_HS_FLAG_TO_BE_CONTD) Flags = PCT2_HS_FLAG_CONTINUATION;
            else if(Flags == 0) Flags = PCT2_HS_FLAG_TO_BE_CONTD;
            RepackHsMessage(&cbMessageSent, &pHeader, &pBuffer, PCT2_HS_SERVER_HELLO, Flags);
        }

    }

    /* Finish up */
    RepackHsMessage(&cbMessageSent, &pHeader, &pBuffer, PCT2_HS_SERVER_HELLO, Flags);




    return( PCT_ERR_OK );

}

SP_STATUS
Pct2UnpackServerHello(
    PSPBuffer          pInput,
    PPct2_Server_Hello *     ppServer)
{

    PPCT2_HS_MESSAGE_HEADER  pHeader;
    PPCT2_SERVER_HELLO   pMessage;

    DWORD               ReportedSize;
    DWORD               cbAltCipherList, cbAltHashList, cbCertList;
    DWORD               cbExchList, cbMsgList;
    DWORD               cbCertifierList, cbPublicValue, cbSigCert, cbResponse;
    DWORD               cbQuickEncryptedKey;

    DWORD               cAltCipherSpecs, cAltHashSpecs, cCertSpecs, cExchSpecs,
                        cMsgSpecs;

    PPct2_Server_Hello  pCanonical;
    PUCHAR              pBuffer;
    DWORD               i;
    DWORD               cbCanonical;

    DWORD               dwRecordFlags;
    pHeader = pInput->pvBuffer;

    if(pInput->cbBuffer < sizeof(PCT2_HS_MESSAGE_HEADER)) {
        return PCT_INT_INCOMPLETE_MSG;
    }

    /* BUGBUG: Right now, we're not dealing with record spanning server
     * hello's */

    ReportedSize = SIZEOF(pHeader);

    if ((ReportedSize+sizeof(PCT2_HS_MESSAGE_HEADER)) > pInput->cbBuffer)
    {
        return PCT_INT_INCOMPLETE_MSG;
    }

    dwRecordFlags = COMBINEBYTES(pHeader->RecordFlagsMsb, pHeader->RecordFlagsLsb);
    /* Only create a new ClientHello canonical if this is a complete
     * message, or the first message in a completion string */

    if(!(dwRecordFlags & PCT2_HS_FLAG_CONTINUATION)) {

        if(ReportedSize < PCT_OFFSET_OF(PPCT2_CLIENT_HELLO, VariantData))
        {
            return PCT_ERR_ILLEGAL_MESSAGE;
        }
        /* Now start validating the message */
        pMessage = (PPCT2_SERVER_HELLO)(pHeader+1);

        if(COMBINEBYTES(pHeader->MessageTypeMsb, pHeader->MessageTypeLsb) !=
           PCT2_HS_CLIENT_HELLO)
        {
            return PCT_ERR_ILLEGAL_MESSAGE;
        }


        /* We don't recognize hello messages of less version than ourselves,
         * those will be handled by a previous version of the code */
        if(COMBINEBYTES(pMessage->VersionMsb, pMessage->VersionLsb) < PCT_VERSION_2)
        {
            return PCT_ERR_ILLEGAL_MESSAGE;
        }

        *ppServer = NULL;

        cbAltCipherList =     COMBINEBYTES( pMessage->AltCipherListLenMsb,
                                            pMessage->AltCipherListLenLsb );

        cbAltHashList =       COMBINEBYTES( pMessage->AltHashListLenMsb,
                                            pMessage->AltHashListLenLsb );

        cbMsgList =           COMBINEBYTES( pMessage->MsgListLenMsb,
                                            pMessage->MsgListLenLsb );

        cbExchList =          COMBINEBYTES( pMessage->ExchListLenMsb,
                                            pMessage->ExchListLenLsb );

        cbCertList =          COMBINEBYTES( pMessage->CertListLenMsb,
                                            pMessage->CertListLenLsb );

        cbCertifierList=      COMBINEBYTES( pMessage->CertifierListLenMsb,
                                            pMessage->CertifierListLenLsb);

        cbPublicValue =       COMBINEBYTES( pMessage->PublicValueLenMsb,
                                            pMessage->PublicValueLenLsb);

        cbQuickEncryptedKey = COMBINEBYTES( pMessage->QuickEncryptedKeyLenMsb,
                                            pMessage->QuickEncryptedKeyLenLsb);

        cbSigCert =           COMBINEBYTES( pMessage->SigCertLenMsb,
                                            pMessage->SigCertLenLsb);

        cbResponse =          COMBINEBYTES( pMessage->ResponseLenMsb,
                                            pMessage->ResponseLenLsb);

        /* we do this to precalc the offset to certifier list */
        i = PCT_OFFSET_OF(PPCT2_CLIENT_HELLO, VariantData)
             + cbAltCipherList
             + cbAltHashList
             + cbMsgList
             + cbExchList
             + cbCertList;

        cAltCipherSpecs = cbAltCipherList / sizeof(ExtCipherSpec);
        cAltHashSpecs = cbAltHashList / sizeof(ExtHashSpec);
        cMsgSpecs  = cbMsgList   / sizeof(ExtMsgSpec);
        cExchSpecs = cbExchList  / sizeof(ExtMsgSpec);
        cCertSpecs = cbCertList  / sizeof(ExtCertSpec);

        cbCanonical =  sizeof(Pct2_Client_Hello) +
                       cAltCipherSpecs * sizeof(CipherSpec) +
                       cAltHashSpecs * sizeof(HashSpec) +
                       cMsgSpecs * sizeof(Pct2MsgSpec) +
                       cExchSpecs * sizeof(ExchSpec) +
                       cCertSpecs * sizeof(CertSpec) +
                       cbCertifierList +
                       cbSigCert +
                       cbResponse;

        if(cbPublicValue) cbCanonical += CALC_CANONICAL_PUBLIC_VALUE_SIZE(cbPublicValue);
        if(cbQuickEncryptedKey) cbCanonical += CALC_CANONICAL_ENCRYPTED_KEY_SIZE(cbQuickEncryptedKey);


        /* Allocate a buffer for the canonical client hello */
        pCanonical = (PPct2_Server_Hello)SPExternalAlloc(cbCanonical);

        if (!pCanonical)
        {
            return(PCT_INT_INTERNAL_ERROR);
        }

        /* Begin Unpacking Client Hello */

        pCanonical->AuthOptions = COMBINEBYTES(pMessage->AuthOptionsMsb, pMessage->AuthOptionsLsb);

        pCanonical->SrvCipherSpec = MapCipherFromExternal(pMessage->CipherSpecData);
        pCanonical->SrvHashSpec   = MapHashFromExternal(pMessage->HashSpecData);

        pCanonical->SrvExchSpec   = MapExchFromExternal(pMessage->ExchSpecData);
        pCanonical->SrvSigSpec    = MapSigFromExternal(pMessage->SigSpecData);

        CopyMemory( pCanonical->SessionID,
                    pMessage->SessionIdData,
                    PCT2_SESSION_ID_SIZE);
        pCanonical->cbSessionID = PCT2_SESSION_ID_SIZE;

        CopyMemory( pCanonical->ConnectionID,
                    pMessage->ConnectionIdData,
                    PCT2_CONNECTION_ID_SIZE );
        pCanonical->cbConnectionID = PCT2_CONNECTION_ID_SIZE;

        /* Pre-set all sizes */
        pCanonical->cAltCipherSpecs = cAltCipherSpecs;
        pCanonical->cAltHashSpecs =   cAltHashSpecs;
        pCanonical->cMessageSpecs =   cMsgSpecs;
        pCanonical->cCertSpecs =      cCertSpecs;
        pCanonical->cExchSpecs =      cExchSpecs;
        pCanonical->cbSigCert = cbSigCert;
        pCanonical->cbResponse = cbResponse;

        /* Start unpacking the variant data */
        pBuffer = pMessage->VariantData;
        /* Pointer to free memory in canonical */
        pCanonical->pCurUnpack = (PUCHAR)(pCanonical + 1);

        /* Unpack Cipher Specs */
        if(pCanonical->cAltCipherSpecs) {
            pCanonical->pAltCipherSpecs = (PCipherSpec)pCanonical->pCurUnpack;
            for(i=0; i< cAltCipherSpecs; i++ )
            {
                pCanonical->pAltCipherSpecs[i] = MapCipherFromExternal(*(ExtCipherSpec *)
                                                            pBuffer);
            }
            pCanonical->pCurUnpack += sizeof(CipherSpec)*pCanonical->cAltCipherSpecs;
            pBuffer += sizeof(ExtCipherSpec)*pCanonical->cAltCipherSpecs;
        }

        /* Unpack Hash Specs */
        if(pCanonical->cAltHashSpecs) {
            pCanonical->pAltHashSpecs = (PHashSpec)pCanonical->pCurUnpack;

            for(i=0; i< cAltHashSpecs; i++ )
            {
                pCanonical->pAltHashSpecs[i] = MapHashFromExternal(*(ExtHashSpec *)
                                                            pBuffer);
            }
            pCanonical->pCurUnpack += sizeof(HashSpec)*pCanonical->cAltHashSpecs;
            pBuffer += sizeof(ExtHashSpec)*pCanonical->cAltHashSpecs;
        }

        /* Unpack Messages */
        if(pCanonical->cMessageSpecs) {
            pCanonical->pMessageSpecs = (PPct2MsgSpec)pCanonical->pCurUnpack;

            for(i=0; i< cMsgSpecs; i++ )
            {
                pCanonical->pMessageSpecs[i] = MapMsgFromExternal(*(ExtMsgSpec *)
                                                            pBuffer);
            }
            pBuffer += sizeof(ExtMsgSpec)*pCanonical->cMessageSpecs;
            pCanonical->pCurUnpack += sizeof(Pct2MsgSpec)*pCanonical->cMessageSpecs;
        }

        /* Unpack Key Exchange Specs */
        if(pCanonical->cExchSpecs) {
            pCanonical->pExchSpecs = (PExchSpec)pCanonical->pCurUnpack;
            for(i=0; i< cExchSpecs; i++ )
            {
                pCanonical->pExchSpecs[i] = MapExchFromExternal(*(ExtExchSpec *)
                                                            pBuffer);
            }
            pBuffer += sizeof(ExtExchSpec)*pCanonical->cExchSpecs;
            pCanonical->pCurUnpack += sizeof(ExchSpec)*pCanonical->cExchSpecs;
        }

        /* Unpack Cert Specs */
        if(pCanonical->cCertSpecs) {
            pCanonical->pCertSpecs = (PCertSpec) pCanonical->pCurUnpack;
            for(i=0; i< cCertSpecs; i++ )
            {
                pCanonical->pCertSpecs[i] = MapCertFromExternal(*(ExtCertSpec *)
                                                            pBuffer);
            }
            pCanonical->pCurUnpack += sizeof(CertSpec)*pCanonical->cCertSpecs;
            pBuffer += sizeof(ExtCertSpec)*pCanonical->cCertSpecs;
        }

        /* Unpack Quick Encrypted Key */
        if(cbQuickEncryptedKey) {
            pCanonical->pQuickEncryptedKey = (PPct2_Encrypted_Key)pCanonical->pCurUnpack;
            pCanonical->pCurUnpack += Pct2UnpackEncryptedKey(pBuffer, pCanonical->pQuickEncryptedKey);
            pBuffer += cbQuickEncryptedKey;
        }

        /* Unpack Response */
        pCanonical->pResponse = pCanonical->pCurUnpack;
        CopyMemory(pCanonical->pResponse, pBuffer, cbResponse);
        pCanonical->pCurUnpack += cbResponse;
        pBuffer += cbResponse;

        pCanonical->UnpackState = SH_UNPACK_CERTIFIERS;
        pCanonical->SubState = 0;
        pCanonical->pCertifiers = pCanonical->pCurUnpack;
    } else {
        pBuffer = (PUCHAR)(pHeader+1);
    }
    *ppServer = pCanonical;
    pInput->cbData = ReportedSize + sizeof(PCT2_RECORD_HEADER);

    /* Everything up to the certifier list is required to come in the first
     * message.  All of the following stuff could come split across records
     */
    switch(pCanonical->UnpackState) {
        case SH_UNPACK_CERTIFIERS:
            /* Unpack the certifier list */
            /* pBuffer should point to data where the certifier list is */
            /* pCurUnpack is where the data goes */
            /* ReportedSize - DWORD(pBuffer - pHeader) is how much data we have */

            i = min(pCanonical->cbCertifiers - pCanonical->SubState,
                    ReportedSize - (DWORD)(pBuffer - (PUCHAR)pHeader));

            CopyMemory(pCanonical->pCurUnpack, pBuffer, i);
            pCanonical->pCurUnpack += i;
            pBuffer += i;
            pCanonical->SubState += i;
            /* We've not yet finished unpacking */
            if(pCanonical->SubState < pCanonical->cbCertifiers) return PCT_INT_NEXT_RECORD;

            if(cbPublicValue) {
                pCanonical->SubState = 0;
                pCanonical->UnpackState = SH_UNPACK_PUBLIC_VALUE;
                pCanonical->pPublicValue = (PPct2_Public_Value)pCanonical->pCurUnpack;
            } else {
                pCanonical->SubState = 0;
                pCanonical->UnpackState = SH_UNPACK_SIGCERT;
                pCanonical->pSigCert = (PUCHAR)pCanonical->pCurUnpack;
            }
        case SH_UNPACK_PUBLIC_VALUE:
            if(cbPublicValue) {
                pCanonical->pCurUnpack += Pct2UnpackPublicValue(pBuffer, pCanonical->pPublicValue);
                pBuffer += cbPublicValue;
            }
            pCanonical->SubState = 0;
            pCanonical->UnpackState = SH_UNPACK_SIGCERT;
            pCanonical->pSigCert = (PUCHAR)pCanonical->pCurUnpack;

        case SH_UNPACK_SIGCERT:
            i = min(pCanonical->cbSigCert - pCanonical->SubState,
                    ReportedSize - (DWORD)(pBuffer - (PUCHAR)pHeader));

            CopyMemory(pCanonical->pCurUnpack, pBuffer, i);
            pCanonical->pCurUnpack += i;
            pBuffer += i;
            pCanonical->SubState += i;
            /* We've not yet finished unpacking */
            if(pCanonical->SubState < pCanonical->cbSigCert) return PCT_INT_NEXT_RECORD;
            return PCT_ERR_OK;
        default:
              break;
    }
    return( PCT_ERR_ILLEGAL_MESSAGE);
}

SP_STATUS
Pct2PackClientMasterKey(
    PPct2_Client_Master_Key      pCanonical,
    PSPBuffer              pCommOutput)
{
    DWORD               cbMessage;
    PPCT2_HS_MESSAGE_HEADER pHeader;
    PPCT2_CLIENT_MASTER_KEY     pMessage;
    PUCHAR              pBuffer;
    DWORD               cbPublicValue,  cbEncryptedKey;

    DWORD               cbMessageSent;

    DWORD               cbCopied, iCopy;
    DWORD               Flags;

    /* Note, when packing messages that are longer than the max message length,
     * we break them into multiple records, and pack them all into one buffer
     */
    if(pCanonical == NULL || pCommOutput == NULL) return PCT_INT_INTERNAL_ERROR;
    pCommOutput->cbData = 0;

    /* Now add on the Quick Value sizes, if there are any */
    if(pCanonical->pPublicValue) {
        cbPublicValue =   CALC_PUBLIC_VALUE_SIZE(pCanonical->pPublicValue);

    }

    if(pCanonical->pEncryptedKey) {
        cbEncryptedKey  = CALC_ENCRYPTED_KEY_SIZE(pCanonical->pEncryptedKey);

    }

    cbMessageSent =  PCT_OFFSET_OF(PPCT2_SERVER_HELLO, VariantData) +
                    pCanonical->cbResponse +
                    cbEncryptedKey;

    cbMessage = cbMessageSent +
                cbPublicValue +
                pCanonical->cbSigCert;


    /* The number of message headers should be equal to the number
     * of blocks we split the message into.
     */
    pCommOutput->cbData = cbMessage + sizeof(PPCT2_HS_MESSAGE_HEADER)*((cbMessage / PCT_MAX_SHAKE_LEN)+1);

    /* are we allocating our own memory? */
    if(pCommOutput->pvBuffer == NULL) {
        pCommOutput->pvBuffer = SPExternalAlloc(pCommOutput->cbData);
        if (NULL == pCommOutput->pvBuffer)
            return PCT_INT_INTERNAL_ERROR;
        pCommOutput->cbBuffer = pCommOutput->cbData;
    }

    if(pCommOutput->cbData > pCommOutput->cbBuffer) return PCT_INT_BUFF_TOO_SMALL;


    pHeader = pCommOutput->pvBuffer;
    pMessage = (PPCT2_CLIENT_MASTER_KEY)(pHeader+1);

    /* Build base portion of server hello message */
    pMessage->EncryptedKeyLenMsb = MSBOF(cbEncryptedKey);
    pMessage->EncryptedKeyLenLsb = LSBOF(cbEncryptedKey);
    pMessage->ResponseLenMsb = MSBOF(pCanonical->cbResponse);
    pMessage->ResponseLenLsb = LSBOF(pCanonical->cbResponse);
    pMessage->PublicValueLenMsb = MSBOF(cbPublicValue);
    pMessage->PublicValueLenLsb = LSBOF(cbPublicValue);
    pMessage->SigCertLenMsb = MSBOF(pCanonical->cbSigCert);
    pMessage->SigCertLenLsb = LSBOF(pCanonical->cbSigCert);


    pBuffer = pMessage->VariantData;


    if(pCanonical->pEncryptedKey){
        pBuffer += Pct2PackEncryptedKey(pCanonical->pEncryptedKey, pBuffer);
    }

    CopyMemory(pBuffer, pCanonical->pResponse, pCanonical->cbResponse);

    pBuffer += pCanonical->cbResponse;

    /* Okay, we shouldn't have run over our record length max yet, but just to
     * be sure, repack.
     */

    if(pCanonical->pPublicValue) {
        pBuffer += Pct2PackPublicValue(pCanonical->pPublicValue, pBuffer);
    }

    while(cbMessageSent >= PCT_MAX_SHAKE_LEN) {
        if(Flags & PCT2_HS_FLAG_TO_BE_CONTD) Flags = PCT2_HS_FLAG_CONTINUATION;
        else if(Flags == 0) Flags = PCT2_HS_FLAG_TO_BE_CONTD;
        RepackHsMessage(&cbMessageSent, &pHeader, &pBuffer, PCT2_HS_CLIENT_MASTER_KEY, Flags);
    }



    /* Yeah, I know this looks wierd, but we should not have to do any Copies
     * to move data out of the way of an inline header if we do this */

    iCopy = 0;
    while(iCopy < pCanonical->cbSigCert) {

        cbCopied = min(pCanonical->cbSigCert-iCopy, (PCT_MAX_SHAKE_LEN-cbMessageSent));
        CopyMemory(pBuffer, pCanonical->pSigCert + iCopy, cbCopied);
        iCopy += cbCopied;
        pBuffer += iCopy;

        while(cbMessageSent >= PCT_MAX_SHAKE_LEN) {
            if(Flags & PCT2_HS_FLAG_TO_BE_CONTD) Flags = PCT2_HS_FLAG_CONTINUATION;
            else if(Flags == 0) Flags = PCT2_HS_FLAG_TO_BE_CONTD;
            RepackHsMessage(&cbMessageSent, &pHeader, &pBuffer, PCT2_HS_CLIENT_MASTER_KEY, Flags);
        }

    }

    /* Finish up */
    RepackHsMessage(&cbMessageSent, &pHeader, &pBuffer, PCT2_HS_CLIENT_MASTER_KEY, Flags);


    return( PCT_ERR_OK );

}

SP_STATUS
Pct2UnpackClientMasterKey(
    PSPBuffer          pInput,
    PPct2_Client_Master_Key *     ppClient)
{

    PPCT2_HS_MESSAGE_HEADER   pHeader;
    PPCT2_CLIENT_MASTER_KEY   pMessage;

    DWORD               ReportedSize;
    DWORD               cbPublicValue, cbSigCert, cbResponse;
    DWORD               cbEncryptedKey;


    PPct2_Client_Master_Key  pCanonical;
    PUCHAR              pBuffer;
    DWORD               i;
    DWORD               cbCanonical;

    DWORD               dwRecordFlags;
    pHeader = pInput->pvBuffer;

    if(pInput->cbBuffer < sizeof(PCT2_HS_MESSAGE_HEADER)) {
        return PCT_INT_INCOMPLETE_MSG;
    }

    /* BUGBUG: Right now, we're not dealing with record spanning server
     * hello's */

    ReportedSize = SIZEOF(pHeader);

    if ((ReportedSize+sizeof(PCT2_HS_MESSAGE_HEADER)) > pInput->cbBuffer)
    {
        return PCT_INT_INCOMPLETE_MSG;
    }

    dwRecordFlags = COMBINEBYTES(pHeader->RecordFlagsMsb, pHeader->RecordFlagsLsb);
    /* Only create a new ClientHello canonical if this is a complete
     * message, or the first message in a completion string */

    if(!(dwRecordFlags & PCT2_HS_FLAG_CONTINUATION)) {

        if(ReportedSize < PCT_OFFSET_OF(PPCT2_CLIENT_MASTER_KEY, VariantData))
        {
            return PCT_ERR_ILLEGAL_MESSAGE;
        }
        /* Now start validating the message */
        pMessage = (PPCT2_CLIENT_MASTER_KEY)(pHeader+1);

        if(COMBINEBYTES(pHeader->MessageTypeMsb, pHeader->MessageTypeLsb) !=
           PCT2_HS_CLIENT_MASTER_KEY)
        {
            return PCT_ERR_ILLEGAL_MESSAGE;
        }



        *ppClient = NULL;


        cbEncryptedKey =      COMBINEBYTES( pMessage->EncryptedKeyLenMsb,
                                            pMessage->EncryptedKeyLenLsb);

        cbResponse =          COMBINEBYTES( pMessage->ResponseLenMsb,
                                            pMessage->ResponseLenLsb);

        cbPublicValue =       COMBINEBYTES( pMessage->PublicValueLenMsb,
                                            pMessage->PublicValueLenLsb);

        cbSigCert =           COMBINEBYTES( pMessage->SigCertLenMsb,
                                            pMessage->SigCertLenLsb);




        cbCanonical =  sizeof(Pct2_Client_Master_Key) +
                       cbResponse +
                       cbSigCert;

        if(cbPublicValue) cbCanonical += CALC_CANONICAL_PUBLIC_VALUE_SIZE(cbPublicValue);
        if(cbEncryptedKey) cbCanonical += CALC_CANONICAL_ENCRYPTED_KEY_SIZE(cbEncryptedKey);


        /* Allocate a buffer for the canonical client hello */
        pCanonical = (PPct2_Client_Master_Key)SPExternalAlloc(cbCanonical);

        if (!pCanonical)
        {
            return(PCT_INT_INTERNAL_ERROR);
        }

        /* Begin Unpacking Client Hello */


        /* Pre-set all sizes */
        pCanonical->cbResponse = cbResponse;
        pCanonical->cbSigCert = cbSigCert;

        /* Start unpacking the variant data */
        pBuffer = pMessage->VariantData;
        /* Pointer to free memory in canonical */
        pCanonical->pCurUnpack = (PUCHAR)(pCanonical + 1);


        /* Unpack Quick Encrypted Key */
        if(cbEncryptedKey) {
            pCanonical->pEncryptedKey = (PPct2_Encrypted_Key)pCanonical->pCurUnpack;
            pCanonical->pCurUnpack += Pct2UnpackEncryptedKey(pBuffer, pCanonical->pEncryptedKey);
            pBuffer += cbEncryptedKey;
        }

        /* Unpack Response */
        pCanonical->pResponse = pCanonical->pCurUnpack;
        CopyMemory(pCanonical->pResponse, pBuffer, cbResponse);
        pCanonical->pCurUnpack += cbResponse;
        pBuffer += cbResponse;

        pCanonical->UnpackState = CMK_UNPACK_PUBLIC_VALUE;
        pCanonical->SubState = 0;
        pCanonical->pPublicValue = (PPct2_Public_Value)pCanonical->pCurUnpack;
    } else {
        pBuffer = (PUCHAR)(pHeader+1);
    }
    *ppClient = pCanonical;
    pInput->cbData = ReportedSize + sizeof(PCT2_RECORD_HEADER);

    /* Everything up to the certifier list is required to come in the first
     * message.  All of the following stuff could come split across records
     */
    switch(pCanonical->UnpackState) {
        case CMK_UNPACK_PUBLIC_VALUE:
            if(cbPublicValue) {
                pCanonical->pCurUnpack += Pct2UnpackPublicValue(pBuffer, pCanonical->pPublicValue);
                pBuffer += cbPublicValue;
            }
            pCanonical->SubState = 0;
            pCanonical->UnpackState = CMK_UNPACK_SIGCERT;
            pCanonical->pSigCert = (PUCHAR)pCanonical->pCurUnpack;

        case CMK_UNPACK_SIGCERT:
            i = min(pCanonical->cbSigCert - pCanonical->SubState,
                    ReportedSize - (DWORD)(pBuffer - (PUCHAR)pHeader));

            CopyMemory(pCanonical->pCurUnpack, pBuffer, i);
            pCanonical->pCurUnpack += i;
            pBuffer += i;
            pCanonical->SubState += i;
            /* We've not yet finished unpacking */
            if(pCanonical->SubState < pCanonical->cbSigCert) return PCT_INT_NEXT_RECORD;
            return PCT_ERR_OK;
        default:
              break;
    }
    return( PCT_ERR_ILLEGAL_MESSAGE);
}


SP_STATUS
Pct2PackServerVerify(
    PPct2_Server_Verify    pCanonical,
    PSPBuffer              pCommOutput)
{
    DWORD               cbMessage;
    PPCT2_HS_MESSAGE_HEADER pHeader;
    PPCT2_SERVER_VERIFY     pMessage;
    PUCHAR              pBuffer;
    DWORD               cbEncryptedKey;

    DWORD               cbMessageSent;

    DWORD               cbCopied, iCopy;
    DWORD               Flags;

    /* Note, when packing messages that are longer than the max message length,
     * we break them into multiple records, and pack them all into one buffer
     */
    if(pCanonical == NULL || pCommOutput == NULL) return PCT_INT_INTERNAL_ERROR;
    pCommOutput->cbData = 0;


    if(pCanonical->pEncryptedKey) {
        cbEncryptedKey  = CALC_ENCRYPTED_KEY_SIZE(pCanonical->pEncryptedKey);

    }

    cbMessageSent =  PCT_OFFSET_OF(PPCT2_SERVER_VERIFY, VariantData) +
                    pCanonical->cbResponse +
                    cbEncryptedKey;

    cbMessage = cbMessageSent +
                pCanonical->cbSigCert;


    /* The number of message headers should be equal to the number
     * of blocks we split the message into.
     */
    pCommOutput->cbData = cbMessage + sizeof(PPCT2_HS_MESSAGE_HEADER)*((cbMessage / PCT_MAX_SHAKE_LEN)+1);

    /* are we allocating our own memory? */
    if(pCommOutput->pvBuffer == NULL) {
        pCommOutput->pvBuffer = SPExternalAlloc(pCommOutput->cbData);
        if (NULL == pCommOutput->pvBuffer)
            return PCT_INT_INTERNAL_ERROR;
        pCommOutput->cbBuffer = pCommOutput->cbData;
    }

    if(pCommOutput->cbData > pCommOutput->cbBuffer) return PCT_INT_BUFF_TOO_SMALL;


    pHeader = pCommOutput->pvBuffer;
    pMessage = (PPCT2_SERVER_VERIFY)(pHeader+1);

    /* Build base portion of server hello message */
    pMessage->EncryptedKeyLenMsb = MSBOF(cbEncryptedKey);
    pMessage->EncryptedKeyLenLsb = LSBOF(cbEncryptedKey);
    pMessage->ResponseLenMsb = MSBOF(pCanonical->cbResponse);
    pMessage->ResponseLenLsb = LSBOF(pCanonical->cbResponse);
    pMessage->SigCertLenMsb = MSBOF(pCanonical->cbSigCert);
    pMessage->SigCertLenLsb = LSBOF(pCanonical->cbSigCert);


    pBuffer = pMessage->VariantData;


    if(pCanonical->pEncryptedKey){
        pBuffer += Pct2PackEncryptedKey(pCanonical->pEncryptedKey, pBuffer);
    }

    CopyMemory(pBuffer, pCanonical->pResponse, pCanonical->cbResponse);

    pBuffer += pCanonical->cbResponse;



    /* Yeah, I know this looks wierd, but we should not have to do any Copies
     * to move data out of the way of an inline header if we do this */

    iCopy = 0;
    while(iCopy < pCanonical->cbSigCert) {

        cbCopied = min(pCanonical->cbSigCert-iCopy, (PCT_MAX_SHAKE_LEN-cbMessageSent));
        CopyMemory(pBuffer, pCanonical->pSigCert + iCopy, cbCopied);
        iCopy += cbCopied;
        pBuffer += iCopy;

        while(cbMessageSent >= PCT_MAX_SHAKE_LEN) {
            if(Flags & PCT2_HS_FLAG_TO_BE_CONTD) Flags = PCT2_HS_FLAG_CONTINUATION;
            else if(Flags == 0) Flags = PCT2_HS_FLAG_TO_BE_CONTD;
            RepackHsMessage(&cbMessageSent, &pHeader, &pBuffer, PCT2_HS_SERVER_VERIFY, Flags);
        }

    }

    /* Finish up */
    RepackHsMessage(&cbMessageSent, &pHeader, &pBuffer, PCT2_HS_SERVER_VERIFY, Flags);


    return( PCT_ERR_OK );

}

SP_STATUS
Pct2UnpackServerVerify(
    PSPBuffer                pInput,
    PPct2_Server_Verify*     ppServer)
{

    PPCT2_HS_MESSAGE_HEADER   pHeader;
    PPCT2_SERVER_VERIFY   pMessage;

    DWORD               ReportedSize;
    DWORD               cbSigCert, cbResponse;
    DWORD               cbEncryptedKey;


    PPct2_Server_Verify  pCanonical;
    PUCHAR              pBuffer;
    DWORD               i;
    DWORD               cbCanonical;

    DWORD               dwRecordFlags;
    pHeader = pInput->pvBuffer;

    if(pInput->cbBuffer < sizeof(PCT2_HS_MESSAGE_HEADER)) {
        return PCT_INT_INCOMPLETE_MSG;
    }

    /* BUGBUG: Right now, we're not dealing with record spanning server
     * hello's */

    ReportedSize = SIZEOF(pHeader);

    if ((ReportedSize+sizeof(PCT2_HS_MESSAGE_HEADER)) > pInput->cbBuffer)
    {
        return PCT_INT_INCOMPLETE_MSG;
    }

    dwRecordFlags = COMBINEBYTES(pHeader->RecordFlagsMsb, pHeader->RecordFlagsLsb);
    /* Only create a new ClientHello canonical if this is a complete
     * message, or the first message in a completion string */

    if(!(dwRecordFlags & PCT2_HS_FLAG_CONTINUATION)) {

        if(ReportedSize < PCT_OFFSET_OF(PPCT2_SERVER_VERIFY, VariantData))
        {
            return PCT_ERR_ILLEGAL_MESSAGE;
        }
        /* Now start validating the message */
        pMessage = (PPCT2_SERVER_VERIFY)(pHeader+1);

        if(COMBINEBYTES(pHeader->MessageTypeMsb, pHeader->MessageTypeLsb) !=
           PCT2_HS_SERVER_VERIFY)
        {
            return PCT_ERR_ILLEGAL_MESSAGE;
        }



        *ppServer = NULL;


        cbEncryptedKey =      COMBINEBYTES( pMessage->EncryptedKeyLenMsb,
                                            pMessage->EncryptedKeyLenLsb);

        cbResponse =          COMBINEBYTES( pMessage->ResponseLenMsb,
                                            pMessage->ResponseLenLsb);

        cbSigCert =           COMBINEBYTES( pMessage->SigCertLenMsb,
                                            pMessage->SigCertLenLsb);




        cbCanonical =  sizeof(Pct2_Server_Verify) +
                       cbResponse +
                       cbSigCert;

        if(cbEncryptedKey) cbCanonical += CALC_CANONICAL_ENCRYPTED_KEY_SIZE(cbEncryptedKey);


        /* Allocate a buffer for the canonical client hello */
        pCanonical = (PPct2_Server_Verify)SPExternalAlloc(cbCanonical);

        if (!pCanonical)
        {
            return(PCT_INT_INTERNAL_ERROR);
        }

        /* Begin Unpacking Client Hello */


        /* Pre-set all sizes */
        pCanonical->cbResponse = cbResponse;
        pCanonical->cbSigCert = cbSigCert;

        /* Start unpacking the variant data */
        pBuffer = pMessage->VariantData;
        /* Pointer to free memory in canonical */
        pCanonical->pCurUnpack = (PUCHAR)(pCanonical + 1);


        /* Unpack Encrypted Key */
        if(cbEncryptedKey) {
            pCanonical->pEncryptedKey = (PPct2_Encrypted_Key)pCanonical->pCurUnpack;
            pCanonical->pCurUnpack += Pct2UnpackEncryptedKey(pBuffer, pCanonical->pEncryptedKey);
            pBuffer += cbEncryptedKey;
        }

        /* Unpack Response */
        pCanonical->pResponse = pCanonical->pCurUnpack;
        CopyMemory(pCanonical->pResponse, pBuffer, cbResponse);
        pCanonical->pCurUnpack += cbResponse;
        pBuffer += cbResponse;

        pCanonical->SubState = 0;
        pCanonical->pSigCert = (PUCHAR)pCanonical->pCurUnpack;
    } else {
        pBuffer = (PUCHAR)(pHeader+1);
    }
    *ppServer = pCanonical;
    pInput->cbData = ReportedSize + sizeof(PCT2_RECORD_HEADER);

    /* Everything up to the certifier list is required to come in the first
     * message.  All of the following stuff could come split across records
     */
    i = min(pCanonical->cbSigCert - pCanonical->SubState,
            ReportedSize - (DWORD)(pBuffer - (PUCHAR)pHeader));

    CopyMemory(pCanonical->pCurUnpack, pBuffer, i);
    pCanonical->pCurUnpack += i;
    pBuffer += i;
    pCanonical->SubState += i;
    /* We've not yet finished unpacking */
    if(pCanonical->SubState < pCanonical->cbSigCert) return PCT_INT_NEXT_RECORD;
    return PCT_ERR_OK;
}


SP_STATUS
Pct2PackClientVerify(
    PPct2_Client_Verify    pCanonical,
    PSPBuffer              pCommOutput)
{
    DWORD               cbMessage;
    PPCT2_HS_MESSAGE_HEADER pHeader;
    PPCT2_CLIENT_VERIFY     pMessage;
    PUCHAR              pBuffer;

    DWORD               cbMessageSent;

    DWORD               cbCopied, iCopy;
    DWORD               Flags;

    /* Note, when packing messages that are longer than the max message length,
     * we break them into multiple records, and pack them all into one buffer
     */
    if(pCanonical == NULL || pCommOutput == NULL) return PCT_INT_INTERNAL_ERROR;
    pCommOutput->cbData = 0;



    cbMessageSent =  PCT_OFFSET_OF(PPCT2_CLIENT_VERIFY, VariantData) +
                    pCanonical->cbResponse;

    cbMessage = cbMessageSent +
                pCanonical->cbSigCert;


    /* The number of message headers should be equal to the number
     * of blocks we split the message into.
     */
    pCommOutput->cbData = cbMessage + sizeof(PPCT2_HS_MESSAGE_HEADER)*((cbMessage / PCT_MAX_SHAKE_LEN)+1);

    /* are we allocating our own memory? */
    if(pCommOutput->pvBuffer == NULL) {
        pCommOutput->pvBuffer = SPExternalAlloc(pCommOutput->cbData);
        if (NULL == pCommOutput->pvBuffer)
            return PCT_INT_INTERNAL_ERROR;
        pCommOutput->cbBuffer = pCommOutput->cbData;
    }

    if(pCommOutput->cbData > pCommOutput->cbBuffer) return PCT_INT_BUFF_TOO_SMALL;


    pHeader = pCommOutput->pvBuffer;
    pMessage = (PPCT2_CLIENT_VERIFY)(pHeader+1);

    /* Build base portion of server hello message */
    pMessage->ResponseLenMsb = MSBOF(pCanonical->cbResponse);
    pMessage->ResponseLenLsb = LSBOF(pCanonical->cbResponse);
    pMessage->SigCertLenMsb = MSBOF(pCanonical->cbSigCert);
    pMessage->SigCertLenLsb = LSBOF(pCanonical->cbSigCert);


    pBuffer = pMessage->VariantData;
    CopyMemory(pBuffer, pCanonical->pResponse, pCanonical->cbResponse);

    pBuffer += pCanonical->cbResponse;


    /* Yeah, I know this looks wierd, but we should not have to do any Copies
     * to move data out of the way of an inline header if we do this */

    iCopy = 0;
    while(iCopy < pCanonical->cbSigCert) {

        cbCopied = min(pCanonical->cbSigCert-iCopy, (PCT_MAX_SHAKE_LEN-cbMessageSent));
        CopyMemory(pBuffer, pCanonical->pSigCert + iCopy, cbCopied);
        iCopy += cbCopied;
        pBuffer += iCopy;

        while(cbMessageSent >= PCT_MAX_SHAKE_LEN) {
            if(Flags & PCT2_HS_FLAG_TO_BE_CONTD) Flags = PCT2_HS_FLAG_CONTINUATION;
            else if(Flags == 0) Flags = PCT2_HS_FLAG_TO_BE_CONTD;
            RepackHsMessage(&cbMessageSent, &pHeader, &pBuffer, PCT2_HS_CLIENT_VERIFY, Flags);
        }

    }

    /* Finish up */
    RepackHsMessage(&cbMessageSent, &pHeader, &pBuffer, PCT2_HS_CLIENT_VERIFY, Flags);


    return( PCT_ERR_OK );

}


SP_STATUS
Pct2UnpackClientVerify(
    PSPBuffer                pInput,
    PPct2_Client_Verify*     ppClient)
{

    PPCT2_HS_MESSAGE_HEADER   pHeader;
    PPCT2_CLIENT_VERIFY   pMessage;

    DWORD               ReportedSize;
    DWORD               cbSigCert, cbResponse;


    PPct2_Client_Verify  pCanonical;
    PUCHAR              pBuffer;
    DWORD               i;
    DWORD               cbCanonical;

    DWORD               dwRecordFlags;
    pHeader = pInput->pvBuffer;

    if(pInput->cbBuffer < sizeof(PCT2_HS_MESSAGE_HEADER)) {
        return PCT_INT_INCOMPLETE_MSG;
    }

    /* BUGBUG: Right now, we're not dealing with record spanning server
     * hello's */

    ReportedSize = SIZEOF(pHeader);

    if ((ReportedSize+sizeof(PCT2_HS_MESSAGE_HEADER)) > pInput->cbBuffer)
    {
        return PCT_INT_INCOMPLETE_MSG;
    }

    dwRecordFlags = COMBINEBYTES(pHeader->RecordFlagsMsb, pHeader->RecordFlagsLsb);
    /* Only create a new ClientHello canonical if this is a complete
     * message, or the first message in a completion string */

    if(!(dwRecordFlags & PCT2_HS_FLAG_CONTINUATION)) {

        if(ReportedSize < PCT_OFFSET_OF(PPCT2_CLIENT_VERIFY, VariantData))
        {
            return PCT_ERR_ILLEGAL_MESSAGE;
        }
        /* Now start validating the message */
        pMessage = (PPCT2_CLIENT_VERIFY)(pHeader+1);

        if(COMBINEBYTES(pHeader->MessageTypeMsb, pHeader->MessageTypeLsb) !=
           PCT2_HS_SERVER_VERIFY)
        {
            return PCT_ERR_ILLEGAL_MESSAGE;
        }



        *ppClient = NULL;



        cbResponse =          COMBINEBYTES( pMessage->ResponseLenMsb,
                                            pMessage->ResponseLenLsb);

        cbSigCert =           COMBINEBYTES( pMessage->SigCertLenMsb,
                                            pMessage->SigCertLenLsb);




        cbCanonical =  sizeof(Pct2_Client_Verify) +
                       cbResponse +
                       cbSigCert;


        /* Allocate a buffer for the canonical client hello */
        pCanonical = (PPct2_Client_Verify)SPExternalAlloc(cbCanonical);

        if (!pCanonical)
        {
            return(PCT_INT_INTERNAL_ERROR);
        }

        /* Begin Unpacking Client Hello */


        /* Pre-set all sizes */
        pCanonical->cbResponse = cbResponse;
        pCanonical->cbSigCert = cbSigCert;

        /* Start unpacking the variant data */
        pBuffer = pMessage->VariantData;
        /* Pointer to free memory in canonical */
        pCanonical->pCurUnpack = (PUCHAR)(pCanonical + 1);



        /* Unpack Response */
        pCanonical->pResponse = pCanonical->pCurUnpack;
        CopyMemory(pCanonical->pResponse, pBuffer, cbResponse);
        pCanonical->pCurUnpack += cbResponse;
        pBuffer += cbResponse;

        pCanonical->SubState = 0;
        pCanonical->pSigCert = (PUCHAR)pCanonical->pCurUnpack;
    } else {
        pBuffer = (PUCHAR)(pHeader+1);
    }
    *ppClient = pCanonical;
    pInput->cbData = ReportedSize + sizeof(PCT2_RECORD_HEADER);

    /* Everything up to the certifier list is required to come in the first
     * message.  All of the following stuff could come split across records
     */
    i = min(pCanonical->cbSigCert - pCanonical->SubState,
            ReportedSize - (DWORD)(pBuffer - (PUCHAR)pHeader));

    CopyMemory(pCanonical->pCurUnpack, pBuffer, i);
    pCanonical->pCurUnpack += i;
    pBuffer += i;
    pCanonical->SubState += i;
    /* We've not yet finished unpacking */
    if(pCanonical->SubState < pCanonical->cbSigCert) return PCT_INT_NEXT_RECORD;
    return PCT_ERR_OK;
}

