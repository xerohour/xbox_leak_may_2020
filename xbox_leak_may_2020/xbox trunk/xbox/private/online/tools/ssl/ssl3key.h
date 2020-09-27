/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
*	Owner 			:ramas
*	Date			:4/16/96
*	description		: Main Crypto functions for SSL3
*----------------------------------------------------------------------------*/
#ifndef _SSL3KEY_H_
#define _SSL3KEY_H_

void 
Ssl3BuildMasterKeys(
    PSPContext pContext, 
    PUCHAR pbPreMaster
);

SP_STATUS
Ssl3MakeMasterKeyBlock(PSPContext pContext);

SP_STATUS
Ssl3MakeWriteSessionKeys(PSPContext pContext);

SP_STATUS
Ssl3MakeReadSessionKeys(PSPContext pContext);


#endif _SSL3KEY_H_
