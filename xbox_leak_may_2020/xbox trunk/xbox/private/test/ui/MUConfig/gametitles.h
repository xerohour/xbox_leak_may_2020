/*****************************************************
*** gametitles.h
***
*** Header file for our game title classes.  These
*** classes will enumerate and store information
*** about all of the Game Titles, and their Saved
*** games on the Xbox
***
*** by James N. Helm
*** June 16th, 2001
***
*****************************************************/

#ifndef _GAMETITLES_H_
#define _GAMETITLES_H_

class CMemoryArea;

class CSavedGame
{
public:
    // Constructors and Destructor
    CSavedGame();
    ~CSavedGame();

    // Public Methods
    HRESULT Init( XGAME_FIND_DATA* gameData );  // Initialize the Saved Game object and populate it's data
    WCHAR* GetName();                           // Get a pointer to the name of the saved game
    char* GetDirectory();                       // Get a pointer to the Saved Game directory
    DWORD GetBlockSize();                       // Returns the Block size of the Saved Game
    char* GetGameID();                          // Get a pointer to the Game ID of the Saved Game

    // Public Properties

private:
    // Private Methods

    // Private Properties
    WCHAR* m_pwszName;          // The name of the Saved Game
    DWORD m_dwBlockSize;        // The size of the Saved Game in Blocks
    char* m_pszGameID;          // The Game ID of the Saved Game
    char* m_pszGameDirectory;   // The directory of the Saved Game
    SYSTEMTIME m_GameDate;      // The Date/Time information of the Saved Game
};


class CGameTitle
{
public:
    // Constructors and Destructor
    CGameTitle();
    ~CGameTitle();

    // Public Methods
    HRESULT Init( char* pszDirectory,                       // Initialize our Game Title
                  CMemoryArea* pMemoryArea );
    DWORD GetNumSavedGames();                               // Returns the number of saved games for the game title
    DWORD GetBlockSize();                                   // Get the total block size of the Game Title
    WCHAR* GetName();                                       // Get a pointer to the name of the Game Title
    char* GetDirectory();                                   // Get a pointer to the directory of the Game Title
    char* GetTitleID();                                     // Get a pointer to the title ID of the Game Title
    WCHAR* GetSavedGameName( unsigned int uiIndex );        // Get the name of a Saved Game
    char* GetSavedGameID( unsigned int uiIndex );           // Get the ID of a Saved Game
    DWORD GetSavedGameBlockSize( unsigned int uiIndex );    // Get the block size of a Saved Game

    // Public Properties

private:
    // Private Methods
    HRESULT ProcessSavedGames( char* pszTitleID,            // Process the Saved Games for the specified Game Title
                               CMemoryArea* pMemoryArea );
    
    // Private Properties
    CLinkedList< CSavedGame* >  m_SavedGameList;    // List of pointers to Saved Games
    WCHAR* m_pwszName;                              // Name of the Game Title
    DWORD m_dwBlockSize;                            // Block size of the Game Title
    char* m_pszTitleID;                             // Game Title ID
    char* m_pszDirectory;                           // Directory for our Game Title
};

#define GAME_TITLE_XBX_FILENAME     "titlemeta.xbx"
#define GAME_TITLE_MAX_NAME         40
#define GAME_TITLE_NAME_TOKEN_W     L"TitleName="

class CXboxGames
{
public:
    // Constructors and Destructor
    CXboxGames();
    ~CXboxGames();

    // Public Methods
    HRESULT Init( char* pszDirectory,               // Initialize the list starting at the specified directory
                  CMemoryArea* pMemoryArea );
    CGameTitle* GetGame( unsigned int uiIndex );    // Get a pointer to a game at the specified index
    DWORD GetNumGames();                            // Get the number of Games on the Xbox
    // Public Properties

private:
    // Private Methods
    
    // Private Properties
    CLinkedList< CGameTitle* >  m_GameList;     // List of pointers to GameTitles
};

#endif // _GAMETITLES_H_