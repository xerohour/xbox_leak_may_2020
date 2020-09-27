/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    xctool.cpp

Abstract:

    Implements the CXCTool class which handles most of the functionality of the tool

--*/

#include <windows.h>
#include <stdlib.h>
#include <rsa.h>
#include <sha.h>
#include <rc4.h>
#include <xcrypt.h>

#include "xctool.h"
#include "xcmain.h"
#include "key.h"


//
// CXCTool class
//

BOOL
CXCTool::GeneratePubPair(
    IN LPCSTR pszKeyFileName,
    IN BOOL bEnc
    )
{
    CKeyFile KeyFile;

    //
    // Generate public/private keys
    //
    if (!KeyFile.CreatePubKeyPair(XC_KEY_BITS))
    {
        return FALSE;
    }

    if (bEnc) 
    {
        char szPassword[PASSWORD_MAXSIZE];
        char szPassword2[PASSWORD_MAXSIZE];

        if (ReadPassword("Password to encrypt new file: ", szPassword))
        {
            if (ReadPassword("Reenter password to verify: ", szPassword2))
            {
                if (strcmp(szPassword, szPassword2) == 0)
                {
                    //
                    // Write the keys out as cipher text
                    //
                    return KeyFile.WriteToEncFile(pszKeyFileName, szPassword);
                }
                else 
                {
                    ERROR_OUT("Password reentered incorrectly");
                }
            }
        }
    }
    else
    {
        //
        // Write the keys out as plain text
        //
        return KeyFile.WriteToFile(pszKeyFileName);
    }
    return FALSE;
}


BOOL
CXCTool::GenerateSymKey(
    IN LPCSTR pszKeyFileName,
    IN BOOL bEnc
    )
{
    CKeyFile KeyFile;

    //
    // Generate symmetric key
    //
    if (!KeyFile.CreateSymKey2())
    {
        return FALSE;
    }

    if (bEnc) 
    {
        char szPassword[PASSWORD_MAXSIZE];
        char szPassword2[PASSWORD_MAXSIZE];

        if (ReadPassword("Password to encrypt new file: ", szPassword))
        {
            if (ReadPassword("Reenter password to verify: ", szPassword2))
            {
                if (strcmp(szPassword, szPassword2) == 0)
                {
                    //
                    // Write the keys out as cipher text
                    //
                    return KeyFile.WriteToEncFile(pszKeyFileName, szPassword);
                }
                else 
                {
                    ERROR_OUT("Password reentered incorrectly");
                }
            }
        }
    }
    else
    {
        //
        // Write the keys out as plain text
        //
        return KeyFile.WriteToFile(pszKeyFileName);
    }
    return FALSE;
}


BOOL 
CXCTool::CopyKeyFile(
    LPCSTR pszInFileName, 
    LPCSTR pszOutFileName, 
    BOOL bEncOut
    )
{
    CKeyFile KeyFile;
    BOOL bInEnc;
    char szPassword[PASSWORD_MAXSIZE];

    bInEnc = KeyFile.IsEncFile(pszInFileName);

    if (bInEnc) 
    {

        if (ReadPassword("Password to read file: ", szPassword))
        {
            if (!KeyFile.ReadFromEncFile(pszInFileName, szPassword))
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (!KeyFile.ReadFromFile(pszInFileName))
        {
            return FALSE;
        }
    }

    if (bEncOut) 
    {
        char szPassword2[PASSWORD_MAXSIZE];

        if (ReadPassword("Password to encrypt new file: ", szPassword))
        {
            if (ReadPassword("Reenter password to verify: ", szPassword2))
            {
                if (strcmp(szPassword, szPassword2) == 0)
                {
                    //
                    // Write the keys out as cipher text
                    //
                    return KeyFile.WriteToEncFile(pszOutFileName, szPassword);
                }
                else 
                {
                    ERROR_OUT("Password reentered incorrectly");
                }
            }
        }
    }
    else
    {
        //
        // Write the keys out as plain text
        //
        return KeyFile.WriteToFile(pszOutFileName);
    }
    return FALSE;

}



BOOL
CXCTool::DumpKeyFile(
    LPCSTR pszFN,
    DUMPTYPE DumpType
    )
{
    CKeyFile KeyFile;
    LPBSAFE_PUB_KEY pBSafe;
    BOOL bEnc;

    bEnc = KeyFile.IsEncFile(pszFN);

    if (bEnc) 
    {
        char szPassword[PASSWORD_MAXSIZE];

        if (ReadPassword("Password to read file: ", szPassword))
        {
            if (!KeyFile.ReadFromEncFile(pszFN, szPassword))
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (!KeyFile.ReadFromFile(pszFN))
        {
            return FALSE;
        }
    }

    if (KeyFile.KeyHeader()->KeyType == KEYFILE_SYM)
    {
        Out("%s SYMMETRIC KEY FILE: %s\n", bEnc ? "CIPHER TEXT" : "PLAIN TEXT", pszFN);
    }
    else
    {
        Out("%s PUBLIC KEY PAIR FILE: %s\n", bEnc ? "CIPHER TEXT" : "PLAIN TEXT", pszFN);
    }

    Out("    Bits in key:                %d\n", KeyFile.KeyHeader()->KeyBits);
    Out("    Key number:                 %d\n", KeyFile.KeyHeader()->KeyNumber);

    if (KeyFile.KeyHeader()->KeyType == KEYFILE_SYM)
    {
        DumpBytes(DumpType, "Symmetric Key Dump", KeyFile.SymmetricKey(), KeyFile.KeyHeader()->KeyBits / 8);
    }
    else
    {
        pBSafe = (LPBSAFE_PUB_KEY)(KeyFile.PublicKey());

        Out("    Bits in modulus(bitlen):    %d\n", pBSafe->bitlen);
        Out("    Size of modulus(keylen):    %d\n", pBSafe->keylen);
        Out("    Max bytes encoded(datalen): %d\n", pBSafe->datalen);
        Out("    Public exponent(pubexp):    %d\n", pBSafe->pubexp);
        Out("\n");
        //
        // dump public key
        //
        if (pBSafe != NULL)
        {
            DumpBytes(DumpType, "Public Key Dump", (PBYTE)pBSafe, KeyFile.KeyHeader()->PublicKeyBytes);
        }

        //
        // dump private key
        //

        pBSafe = (LPBSAFE_PUB_KEY)(KeyFile.PrivateKey());
        if (pBSafe != NULL)
        {
            DumpBytes(DumpType, "Private Key Dump", (PBYTE)pBSafe, KeyFile.KeyHeader()->PrivateKeyBytes);
        }
    }


    return TRUE;
}


void 
CXCTool::Out(
    LPCSTR, 
    ...
    )
/*++

Routine Description:

    printf functality.  Must be overridden by the calling module

Arguments:

Return Value:

    None

--*/
{
}




void
CXCTool::DumpBytes(
    DUMPTYPE DumpType,
    LPCSTR pszLabel, 
    PBYTE pbData, 
    DWORD dwDataLen
    )
/*++

Routine Description:

    Dumps the given data as hex and ascii

Arguments:

Return Value:

    None

--*/
{
    DWORD i;
    DWORD beg;

    Out("    %s (%d bytes):\n", pszLabel, dwDataLen);
    if (DumpType == DUMP_BIN)
    {
        beg = 0;
        for (i = 0; i < dwDataLen; i++)
        {
            Out("%02X ", pbData[i]);
            if ((i + 1) % 16 == 0 || (i + 1) == dwDataLen)
            {
                DWORD j;

                for (j = (i - beg); j < 16; j++)
                {
                    Out("   ");
                }

                for (j = beg; j <= i; j++)
                {
                    if (pbData[j] >= 32)
                        Out("%c", pbData[j]);
                    else
                        Out(".");
                }
                beg = i + 1;

                Out("\n");
            }
        }
        Out("\n");
    }
    else 
    {
        if (DumpType == DUMP_C)
            Out("    {\n");

        for (i = 0; i < dwDataLen; i++)
        {
            if (i % 8 == 0)
            {
                if (DumpType == DUMP_ASM)
                    Out("        DB ");
                else
                    Out("           ");
            }

            if (DumpType == DUMP_ASM)
            {
                Out("0%02Xh", pbData[i]);

                if ((i + 1) % 8 == 0)
                    Out("\n");
                else if ((i + 1) != dwDataLen)
                    Out(",");
            }
            else
            {
                Out("0x%02X", pbData[i]);
                
                if ((i + 1) != dwDataLen)
                    Out(",");
                if ((i + 1) % 8 == 0)
                    Out("\n");
            }
            
        }

        if (DumpType == DUMP_C)
            Out("\n    }\n");

        Out("\n");
    }
}


