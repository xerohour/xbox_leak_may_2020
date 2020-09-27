//-----------------------------------------------------------------------------
// File: XbSavedGame.h
//
// Desc: Saved game container
//
// Hist: 02.05.01 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBSAVED_GAME_H
#define XBSAVED_GAME_H

#include <xtl.h>




//-----------------------------------------------------------------------------
// Name: class CXBSavedGame
// Desc: Xbox saved game container
//-----------------------------------------------------------------------------
class CXBSavedGame
{

    WCHAR         m_strName[ MAX_GAMENAME ]; // name from container
    CHAR          m_strDir[ MAX_PATH ];      // folder
    mutable DWORD m_dwSize;                  // size in bytes
    mutable LPDIRECT3DTEXTURE8 m_pImage;     // game image
    mutable BOOL  m_bIsValidImage;           // TRUE if SaveImage.xbx is valid format

    union
    {
        mutable FILETIME  m_ftCreationTime;  // save date/time
        mutable ULONGLONG m_qwCreationTime;
    };

public:

    CXBSavedGame();
    CXBSavedGame( const CXBSavedGame& );
    CXBSavedGame& operator =( const CXBSavedGame& );
    ~CXBSavedGame();
    explicit CXBSavedGame( const _XGAME_FIND_DATA& );
    CXBSavedGame( const WCHAR* strName, const CHAR* strDir );

    BOOL  CreateGame( CHAR chDestDrive, const WCHAR* strName );
    BOOL  OpenGame( CHAR chDestDrive, const WCHAR* strName,
                    DWORD dwOpenDisposition );
    BOOL  DeleteGame();
    BOOL  IsEmpty() const;
    CHAR  GetDrive() const;
    DWORD GetSize() const;

    FILETIME     GetCreationTime() const;
    ULONGLONG    GetCreationQword() const;
    BOOL         GetImage( LPDIRECT3DTEXTURE8* ) const;
    BOOL         SaveImage( const LPDIRECT3DTEXTURE8 );
    const WCHAR* GetName() const;
    const CHAR*  GetDirectory() const;

private:

    VOID Init();
    static DWORD GetDirSize( const CHAR* strDir, DWORD dwClusterSize );

};

#endif // XBSAVED_GAME_H
