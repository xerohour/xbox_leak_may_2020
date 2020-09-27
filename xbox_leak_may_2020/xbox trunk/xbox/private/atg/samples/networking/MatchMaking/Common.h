//-----------------------------------------------------------------------------
// File: Common.h
//
// Desc: Matchmaking global header
//
// Hist: 10.19.01 - New for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef MATCHMAKING_COMMON_H
#define MATCHMAKING_COMMON_H

#include "xtl.h"
#include "xonline.h"
#include "XBRandName.h"
#include "XBNetMsg.h"
#include "XBOnlineTask.h"

#pragma warning( disable: 4786 )
#include <vector>
#include <string>
#include <queue>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_LEVEL_STR       = 16;
const DWORD MAX_STYLE_BLOB      = 9;
const DWORD MAX_SESSION_STR     = 12;
const DWORD MAX_STATUS_STR      = 128;
const DWORD MAX_TYPE_STR        = 8;
const DWORD MAX_SESSION_ATTRIBS = 5;
enum
{
    // Match menu
    MATCH_QUICK = 0,
    MATCH_CUSTOM,
    MATCH_MAX,

    // Customize menu
    CUSTOM_TYPE = 0,
    CUSTOM_LEVEL,
    CUSTOM_STYLE,
    CUSTOM_NAME,
    CUSTOM_FIND,
    CUSTOM_MAX,

    // Game type menu
    TYPE_ANY = 0,
    TYPE_SHORT,
    TYPE_MEDIUM,
    TYPE_LONG,
    TYPE_MAX,

    // Player type menu
    LEVEL_ANY = 0,
    LEVEL_BEGINNER,
    LEVEL_INTERMEDIATE,
    LEVEL_ADVANCED,
    LEVEL_MAX,

    // Game style menu
    STYLE_ANY = 0,
    STYLE_HEAVY,
    STYLE_LIGHT,
    STYLE_MIXED,
    STYLE_MAX,

    // Game menu
    GAME_WAVE = 0,
    GAME_LEAVE,
    GAME_MAX
};


typedef enum
{
    ADD_PLAYER,
    REMOVE_PLAYER
} SESSION_UPDATE_ACTION;

//-----------------------------------------------------------------------------
// Strings
//-----------------------------------------------------------------------------
extern const WCHAR* const strANY;

extern const WCHAR* const strSHORT;
extern const WCHAR* const strMEDIUM;
extern const WCHAR* const strLONG;

extern const WCHAR* const strBEGINNER;
extern const WCHAR* const strINTERMEDIATE;
extern const WCHAR* const strADVANCED;

extern const  CHAR* const strHEAVY;
extern const  CHAR* const strLIGHT;
extern const  CHAR* const strMIXED;

extern const WCHAR* const strHEAVYw;
extern const WCHAR* const strLIGHTw;
extern const WCHAR* const strMIXEDw;




//-----------------------------------------------------------------------------
// Name: class SessionInfo
// Desc: Session information from the matchmaking server
//-----------------------------------------------------------------------------
class SessionInfo
{
    XMATCH_SEARCHRESULT m_xms;

    // Attributes
    // These are placed into a separate structure for
    // the benefit of XOnlineMatchSearchParse. The
    // members are required to be packed on a byte boundaries
    // and the ordering is also important.
    //
#pragma pack(push, 1)

    struct
    {
        ULONGLONG m_qwGameType;
        WCHAR     m_strPlayerLevel[ MAX_LEVEL_STR ];
        WCHAR     m_strSessionName[ MAX_SESSION_STR ];
        WORD      m_wStyleLen;
        BYTE      m_GameStyle[ MAX_STYLE_BLOB ];
        WCHAR     m_strOwnerName[ XONLINE_USERNAME_SIZE ];
    } m_Attributes;

#pragma pack(pop)

public:

    SessionInfo();
    SessionInfo( XMATCH_SEARCHRESULT&, const CXBOnlineTask&, DWORD dwResult );

    XNKID* GetSessionID()              { return &m_xms.SessionID; }
    XNKEY* GetKeyExchangeKey()         { return &m_xms.KeyExchangeKey; }
    XNADDR* GetHostAddr()              { return &m_xms.HostAddress; }

    // Session attributes
    DWORD GetPublicAvail()             { return m_xms.dwPublicAvailable; }
    ULONGLONG GetGameType()            { return m_Attributes.m_qwGameType; }
    WCHAR* GetPlayerLevel()            { return m_Attributes.m_strPlayerLevel; }
    WCHAR* GetSessionName()            { return m_Attributes.m_strSessionName; }
    WCHAR* GetOwnerName()              { return m_Attributes.m_strOwnerName; }

    WORD GetStyleLen()                 { return m_Attributes.m_wStyleLen; }
    BYTE* GetStylePtr()                { return m_Attributes.m_GameStyle; }

    VOID SetGameType( ULONGLONG );
    VOID SetPlayerLevel( const WCHAR* );
    VOID SetSessionName( const WCHAR* );
    VOID SetOwnerName( const WCHAR* );
    VOID SetStyle( const VOID*, WORD );

    VOID GenRandSessionName( const CXBRandName& );

};




//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------
typedef std::vector< std::wstring > SessionNameList;
typedef std::queue< SESSION_UPDATE_ACTION > SessionUpdateQ;
typedef std::vector< SessionInfo >  SessionList;




#endif // MATCHMAKING_COMMON_H
