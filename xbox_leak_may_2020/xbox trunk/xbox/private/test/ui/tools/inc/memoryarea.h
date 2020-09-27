/*****************************************************
*** memoryarea.h
***
*** Header file for our Memory Area class.
*** This class will allow you to perform certain
*** actions that pertain to a memory area on the
*** Xbox, such as erase, create game titles, etc.
***
*** by James N. Helm
*** March 30th, 2001
***
*****************************************************/

#ifndef _MEMORYAREA_H_
#define _MEMORYAREA_H_

#include "gametitles.h"

class CMemoryArea
{
public:
    CMemoryArea();
    virtual ~CMemoryArea();

    // Public Methods
    virtual HRESULT AdjustFileSize( char* pszFileName,                              // Adjusts the file size by the specified amount
                                    LARGE_INTEGER liNumBytes );
    virtual HRESULT CopyGameImages( char* pszTitleDir,                              // Copy the Game images in to the title directory
                                    BOOL bCopyTitleImage=TRUE,
                                    BOOL bCopyDefSaveImage=TRUE );
    virtual HRESULT CreateTitleMetaXbx( char* pszGamePath,                          // Create a Title Meta XBX file for the specified game title
                                        WCHAR* pwszGameName,
                                        WCHAR* pwszLangCode,
                                        WCHAR* pwszMissingLangCode,
                                        BOOL bPrependLangCodeToName=TRUE );
    virtual HRESULT CreateGameTitle( char cDriveLetter,                             // Create a Game Title in the specified directory
                                     char* pszTitleDir,
                                     WCHAR* pwszGameName=NULL,
                                     BOOL bPrependLangCodeToName=TRUE );
    virtual HRESULT CreateSavedGame( IN WCHAR* pwszSavedGameName,                   // Create a Saved Game in the currently mapped UDATA directory
                                     OUT char* pszGamePath,
                                     IN unsigned int cbBuffSize,
                                     IN SYSTEMTIME* pSysTimeGameDate,
                                     IN char* pszSaveImage,
                                     IN BOOL bNoCopyGame );
    virtual DWORD   CreateZeroByteFile( char* pszFileName );                        // Create a file of zero bytes
    virtual HRESULT CreateFileOfSize( char* pszFileName,                            // Creates a file of a specified size
                                      LARGE_INTEGER liNumBytes );
    
    virtual void    CopyDirs( char* pszSource,                                      // Copy files from one directory to another
                              char* pszDest );
    virtual BOOL    CorruptPartition( POBJECT_STRING VolumePath );                  // Corrupt the specified partition by blanking out the initial page
    virtual BOOL    DeleteDirectory( char* pszPath );                               // Delete a directory and all files within
    virtual void    Erase( char cDriveLetter = '\0' );                              // Clear the Memory Area of all data
    virtual DWORD   GetMAFileSize( char* pszFileName );                             // This function will get the file size of the file, and return it to the caller
    virtual DWORD   GetFreeBlocks();                                                // Get the number of blocks that are free on the Memory Area
    virtual DWORD   GetTotalBlocks();                                               // Get the total number of blocks on the Memory Area
    virtual char    GetUDataDrive() { return m_cUDataDriveLetter; };                // Get the Drive letter of UDATA
    virtual void    SetUDataDrive( char cDrive ) { m_cUDataDriveLetter = cDrive; }; // Set the UDATA Drive Letter
    virtual BOOL    FileExists( char* pszFileName );                                // Will return TRUE if a file exists, otherise FALSE
    virtual void    FillArea( char* pszDirectory,                                   // Fill the Memory Area with a specified Number of Blocks worth of data
                              unsigned int uiNumBlocks );
    virtual HRESULT CreateFragTempFiles();                                          // Creates temporary files to use during fragmentations
    virtual HRESULT FragmentTree( char* pszStartDirectory );                        // Will fragment the specified directory, and all directories within
    virtual HRESULT FragmentDirectory( char* pszDirPath );                          // Fragments all files in a given directory
    virtual HRESULT FragmentFile( char* pszFilePath,                                // Fragments an individual file
                                  char* pszFileName );


    virtual DWORD MapDrive( char cDriveLetter,                                      // Map a drive to a partiticular drive letter
                            char* pszPartition );
    virtual DWORD UnMapDrive( char cDriveLetter );                                  // Un-map a drive from the specified drive letter
    virtual HRESULT GetDefaultName( WCHAR* pwszBuffer,                              // Used to get the Memory Area default name
                                    ULONG ulBufferLen );
    virtual WCHAR* GetDefaultNamePtr() { return m_pwszDefaultName; };               // Get the pointer to our default name

    CXboxGames* GetGameTitlePtr() { return &m_GameTitles; };                        // Get a pointer to our Game Title Object
    virtual void ProcessGameTitles();                                               // Will search the memory area and populate the GameTitles object

    virtual HRESULT MapUDataToOrigLoc() = 0;                                        // Map the UDATA drive to it's orignal location
    virtual HRESULT MapUDataToTitleID( char* pszTitleID ) = 0;                      // Map the UDATA drive letter to the specified Title ID
    virtual HRESULT MapUDataToTitleID( WCHAR* pwszTitleID ) = 0;                    // Map the UDATA drive letter to the specified Title ID
    virtual HRESULT MapUDataToTitleID( DWORD dwTitleID ) = 0;                       // Map the UDATA drive letter to the specified Title ID

    virtual void SetAbortProcess( BOOL bSetting );                                  // Used to set whether or not the current process should be aborted
    virtual BOOL GetAbortProcess();                                                 // Used to get the state of our current process


protected:
    // Private Properties
    WCHAR*              m_pwszDefaultName;                                          // Used to hold the default MA name
    char                m_cUDataDriveLetter;                                        // UDATA Drive letter for the current area
    CXboxGames          m_GameTitles;                                               // Handles interaction with Game Titles and Saved Games
    CRITICAL_SECTION    m_ProcessCritSec;                                           // Critical Section to protect our process abort memory
    BOOL                m_bAbortProcess;                                            // Aborts long running process such as Fragmentation

    // Private Methods
    HRESULT CalcSpace( DWORD* pdwTotalBlocks,                                       // Retreives block information for the current MU
                       DWORD* pdwFreeBlocks );
    HRESULT SetDefaultName( WCHAR* pwszName );                                      // Used to set the Default name of the Memory Area
};

#define MEMORY_AREA_DATA_DIRECTORY_A                "d:\\data"

#define MEMORY_AREA_TITLE_META_FILENAME             "titlemeta.xbx"
#define MEMORY_AREA_TITLE_META_TITLENAME_TOKEN      L"TitleName="
#define MEMORY_AREA_TITLE_IMAGE_FILENAME            "titleimage.xbx"
#define MEMORY_AREA_TITLE_IMAGE_PATHINFO            MEMORY_AREA_DATA_DIRECTORY_A "\\" MEMORY_AREA_TITLE_IMAGE_FILENAME
#define MEMORY_AREA_SAVEGAME_IMAGE_FILENAME         "saveimage.xbx"
#define MEMORY_AREA_SAVEGAME_IMAGE_PATHINFO         MEMORY_AREA_DATA_DIRECTORY_A "\\" MEMORY_AREA_SAVEGAME_IMAGE_FILENAME
#define MEMORY_AREA_DEFSAVE_IMAGE_FILENAME          "defsave.xbx"
#define MEMORY_AREA_DEFSAVE_IMAGE_PATHINFO          MEMORY_AREA_DATA_DIRECTORY_A "\\" MEMORY_AREA_DEFSAVE_IMAGE_FILENAME

#define MEMORY_AREA_FRAG_NUM_FILES                  2000
#define MEMORY_AREA_FRAG_FILE_DIR                   "c:\\devkit\\xdk_frag_files"
#define MEMORY_AREA_FRAG_FILL_FILENAME              MEMORY_AREA_FRAG_FILE_DIR "\\xdk_fill_file.xdk"
#define MEMORY_AREA_FRAG_FILE_PATTERN               "xdk_file_"

#define MEMORY_AREA_UNICODE_SIGNATURE               L'\xFEFF'

#define MEMORY_AREA_BLOCK_SIZE_MAX_NUM_LOW_PART     262143
#define MEMORY_AREA_BLOCK_SIZE_IN_BYTES             16384


// Meta Data Constants
enum MemAreaLangCodes
{
    ENUM_MEMAREA_LANGUAGECODE_DEFAULT,
    ENUM_MEMAREA_LANGUAGECODE_ENGLISH,
    ENUM_MEMAREA_LANGUAGECODE_JAPANESE,
    ENUM_MEMAREA_LANGUAGECODE_GERMAN,
    ENUM_MEMAREA_LANGUAGECODE_ITALIAN,
    ENUM_MEMAREA_LANGUAGECODE_FRENCH,
    ENUM_MEMAREA_LANGUAGECODE_SPANISH,

    ENUM_MEMAREA_LANGUAGECODE_NUM_ITEMS
};

static WCHAR* g_wpszMemAreaLanguageCodes[] = {
    L"[default]",
    L"[EN]",
    L"[JA]",
    L"[DE]",
    L"[IT]",
    L"[FR]",
    L"[ES]"
};

#endif // _MEMORYAREA_H_
