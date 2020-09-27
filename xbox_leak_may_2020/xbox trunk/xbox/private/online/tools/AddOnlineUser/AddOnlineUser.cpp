// ---------------------------------------------------------------------------------------
// conline.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xonp.h"
#include "xonver.h"
#include <string.h>

#if 0
#include <windows.h>
#include <xbox.h>
#include <winsockx.h>
#include <winsockp.h>
#include <xonlinep.h>
#include <stdlib.h>
#include <stdio.h>
#include <xonp.h>
#endif

#ifdef XONLINE_FEATURE_XBOX
extern CXo *         g_pXo;
#define GetXo()      g_pXo
#endif

static const LPSTR INI_FILE_LOCATION = "d:\\AddOnlineUser.ini";

#define MAX_USERS 9
char g_acArgvFromIniFile[MAX_USERS][256];
int g_ctUsers = 0;

#define Verify(x) do { if (!(x)) DbgBreak(); } while (0)

HRESULT ReadUserData( char* pcUser, PXONLINEP_USER pNewUser );
HRESULT ParseIniFile( void );
HRESULT XOnlineSetMachineAccount(IN PXONLINE_USER pUser);

#ifdef _XBOX
int __cdecl main(int argc, char * argv[])
#else
class CTestXOnline : public CXOnline
{
public:
    CTestXOnline(char * pszXbox = NULL) : CXOnline(pszXbox) {}
    int Main(int argc, char * argv[]);
};

int __cdecl main(int argc, char * argv[])
{
    int result;
    char buffer[128];

    sprintf(buffer, "%s@Nic/0", getenv("COMPUTERNAME") );
    CTestXOnline * pTestXOnline = new CTestXOnline(buffer);
    result = pTestXOnline->Main(argc, argv);
    delete pTestXOnline;
    return(result);
}

int CTestXOnline::Main(int argc, char * argv[])
#endif
{
    HRESULT hr;
    NTSTATUS Status;
    XONLINE_USER users[XONLINE_MAX_STORED_ONLINE_USERS];
    DWORD cUsers;
    XONLINEP_USER newUser;
    char* pcNewUserInfo = NULL;
    BYTE abConfigData[CONFIG_DATA_LENGTH];
    XC_ONLINE_MACHINE_ACCOUNT_STRUCT* pMachineAccount;
    HANDLE hConfigVolume;
    DWORD dwPort, dwSlot;
    CHAR chDrive;
    int i;
    
#ifdef XNET_FEATURE_TRACE
    Tag(Verbose) = TAG_ENABLE;
    Tag(Warning) = TAG_ENABLE;
#endif

    hr = XOnlineStartup(NULL);
    Verify(hr == S_OK);

#ifdef _XBOX
    XInitDevices(0,NULL);

    //
    // Sleep a while to make sure the USB is ready
    //
    Sleep (1000);
#endif

    hr = XOnlineGetUsers( &users[0], &cUsers );
    Verify ( SUCCEEDED( hr ) );

    for (i=0; i<(int)cUsers; ++i)
    {
        TraceSz2( Verbose, "XOnlineGetUsers returned user %s with options 0x%08X", users[i].name, users[i].dwUserOptions);

        if ((users[i].dwUserOptions & XONLINE_USER_OPTION_CAME_FROM_MU) == 0)
        {
            hr = _XOnlineRemoveUserFromHD( &users[i] );
            Verify ( SUCCEEDED( hr ) );
        }
    }

#ifdef _XBOX
    hr = ParseIniFile();
    Verify(hr == S_OK);

    for (i=0; i<g_ctUsers; ++i)
    {
        pcNewUserInfo = g_acArgvFromIniFile[i];

        hr = ReadUserData( pcNewUserInfo, &newUser );
        Verify(hr == S_OK);

        //
        // Check if machine account.
        //
        if (_stricmp(newUser.kingdom, "_devkit_") == 0)
        {
            if (!GetXo()->SetMachineAccount( &newUser ))
            {
                DbgBreak();
            }
        }
        else if (newUser.index == 0xFFFFFFFF)
        {
            hr = _XOnlineAddUserToHD((PXONLINE_USER)&newUser);
            Verify(hr == S_OK);
            TraceSz1(Verbose, "AddOnlineUser succeeded for %s", pcNewUserInfo);
        }
        else
        {
            dwPort = newUser.index;
            dwSlot = newUser.hr;
            Assert( dwPort < XGetPortCount() && dwSlot < 2 );
            Status = XMountMU( dwPort, dwSlot, &chDrive );
            if (Status == STATUS_SUCCESS)
            {
                hr = _XOnlineSetUserInMU(chDrive, (PXONLINE_USER)&newUser);            
                Verify(hr == S_OK);
                TraceSz3(Verbose, "AddOnlineUser succeeded for %s into port:%d slot:%d", pcNewUserInfo, dwPort, dwSlot);
                XUnmountMU( dwPort, dwSlot );
            }
        }
        
    }
#else
    if (argc != 2)
    {
        printf("Usage:\n    AddOnlineUser PuidInHex,name,kingdom,domain,KeyInHex,OptionsInHex,PinInHex\n");
        return (1);
    }

    hr = ReadUserData( argv[1], &newUser );
    Verify(hr == S_OK);

    //
    // Check if machine account.
    //
    if (_stricmp(newUser.kingdom, "_devkit_") == 0)
    {
        if (!GetXo()->SetMachineAccount( &newUser ))
        {
            DbgBreak();
        }
    }
    else if (newUser.index == 0xFFFFFFFF)
    {
        hr = _XOnlineAddUserToHD((PXONLINE_USER)&newUser);
        Verify(hr == S_OK);    
        printf("AddOnlineUser succeeded for %s\n",argv[1]);
    }
#endif

    hr = XOnlineCleanup();
    Verify(hr == S_OK);

    return (0);
}


HRESULT ParseIniFile( void )
{
    HRESULT hr = S_OK;
    FILE* pIniFile = NULL;
    int iVal = 0;

    //
    // open the ini file
    //

    if ( SUCCEEDED( hr ) )
    {
        pIniFile = fopen( INI_FILE_LOCATION, "rb" );
        if (pIniFile == NULL)
        {
            hr = E_FAIL;
            goto Cleanup;
        }
    }

    while (fgets( g_acArgvFromIniFile[g_ctUsers], sizeof(g_acArgvFromIniFile[g_ctUsers]), pIniFile ) != NULL)
    {
        iVal = strlen(g_acArgvFromIniFile[g_ctUsers]);
        if (g_acArgvFromIniFile[g_ctUsers][iVal-1] == '\n')
        {
            g_acArgvFromIniFile[g_ctUsers][iVal-1] = '\0';
        }

        g_ctUsers++;
        if (g_ctUsers == MAX_USERS)
        {
            break;
        }    
    }
    
    if (g_ctUsers == 0)
    {
        hr = E_FAIL;
        goto Cleanup;
    }
    
Cleanup:
    //
    // close the ini file
    //
    if ( pIniFile )
    {
        fclose( pIniFile );
    }

    return hr;
}

HRESULT
ReadUserData( char* pcUser, PXONLINEP_USER pNewUser )
{
    HRESULT hr = S_OK;
    INT iVal = 0;
    DWORD i;
    
    CHAR szString[256];
    BYTE byPin[XONLINE_PIN_LENGTH];

    union un {
        ULONGLONG ullVals[2];
        BYTE byVals[16];
    } unn;

    //
    // make sure the ini file is not NULL
    //

    if ( NULL == pcUser )
    {
        hr = E_POINTER;
        goto Cleanup;
    }

    //
    // make sure the user is not NULL
    //

    if ( NULL == pNewUser )
    {
        hr = E_POINTER;
        goto Cleanup;
    }

    RtlZeroMemory( pNewUser, sizeof(*pNewUser) );

    pNewUser->index = 0xFFFFFFFF;
    
    if ( SUCCEEDED( hr ) )
    {

        ZeroMemory( szString, sizeof( szString ) );

        //
        // the format string is:
        //
        // 16 hex charecters put into an 8 byte value
        // , delimieter
        // 63 maximum valid email string charecters
        // , delimeter
        // 32 hex charecters (16 + 16) put into 2-8 byte values
        // , delimiter
        // 255 maximum valid string charecters (optional)

        iVal = sscanf( pcUser, 
                       "%16I64x,%16[a-zA-Z0-9!#$%&'*+./=?^_`{|}~@-],%12[a-zA-Z0-9!#$%&'*+./=?^_`{|}~@-],%20[a-zA-Z0-9!#$%&'*+./=?^_`{|}~@-],%16I64x%16I64x,%255[a-zA-Z0-9,:]", 
                       &pNewUser->xuid.qwUserID,
                       pNewUser->name,
                       pNewUser->kingdom,
                       pNewUser->domain,
                       unn.ullVals + 1,
                       unn.ullVals,
                       szString );

        hr = 0 != iVal ? S_OK : E_FAIL;

        if (FAILED(hr))
        {
            goto Cleanup;
        }

        strcpy( pNewUser->realm, "passport.net" );

        if ( EOF == iVal )
        {
            hr = E_OUTOFMEMORY;
        }
    }
            
    //
    // copy the values from the union into the key
    //

    for ( i = 0; i < sizeof( ULONGLONG ) * 2 && SUCCEEDED( hr ); i++ )
    {
        pNewUser->key[i] = unn.byVals[sizeof( ULONGLONG ) * 2 - i - 1];
    }

    //
    // Parse the optional params. These are (all optional)
    // 8 bytes flags
    // , delimieter
    // 8 bytes pin
    // , delimiter
    // port,slot
    // : delimiter
    // 237 chars
    //

    if ( SUCCEEDED( hr ) )
    {
        iVal = sscanf( szString, "%8x,%8x:%d,%d", &pNewUser->dwUserOptions, byPin, &pNewUser->index, &pNewUser->hr );

        hr = 0 != iVal ? S_OK : E_FAIL;
        if (FAILED(hr))
        {
            goto Cleanup;
        }
    }

    //
    // rearange the pin value
    //

    for ( i = 0; i < XONLINE_PIN_LENGTH && SUCCEEDED( hr ); i++ )
    {
        pNewUser->pin[i] = byPin[XONLINE_PIN_LENGTH - i - 1];
    }

    TimeStampOnlineUserStruct( (XC_ONLINE_USER_ACCOUNT_STRUCT*) pNewUser );

    //
    // spit back the hr
    //
Cleanup:
    return hr;
}

