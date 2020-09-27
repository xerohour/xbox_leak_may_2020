/*****************************************************
*** harddrive.h
***
*** Header file for our Hard Drive class.
*** This class will contain functions and information
*** pertaining to the Xbox Hard Drive
***
*** by James N. Helm
*** April 2nd, 2001
***
*****************************************************/

#ifndef _HARDDRIVE_H_
#define _HARDDRIVE_H_

#include "memoryarea.h"
#include "dashst.h"

class CHardDrive : public CMemoryArea
{
public:
    // Contructor and Destructors
    CHardDrive();
    ~CHardDrive();

    // Public Methods
    char GetTDataDrive() { return m_cTDataDriveLetter; };                   // Get the Drive letter of TDATA
    void SetTDataDrive( char cDrive ) { m_cTDataDriveLetter = cDrive; };    // Set the TDATA Drive letter
    HRESULT CreateSavedGame( IN WCHAR* pwszSavedGameName,                   // Create a Saved Game
                             OUT char* pszGamePath,
                             IN unsigned int cbBuffSize,
                             IN SYSTEMTIME* pSysTimeGameDate,
                             IN char* pszSaveImage,
                             IN BOOL bNoCopyGame );
    HRESULT RemoveAllSoundtracks();                                         // Remove all of the Soundtracks that live on the hard drive
    HRESULT RemoveAllNicknames();                                           // Remove all of the Nicknames that live on the hard drive

    HRESULT MapUDataToOrigLoc();                                            // Map the UDATA drive to it's orignal location
    HRESULT MapUDataToTitleID( char* pszTitleID );                          // Map the UDATA drive letter to the specified Title ID
    HRESULT MapUDataToTitleID( WCHAR* pwszTitleID );                        // Map the UDATA drive letter to the specified Title ID
    HRESULT MapUDataToTitleID( DWORD dwTitleID );                           // Map the UDATA drive letter to the specified Title ID

    HRESULT MapTDataToOrigLoc();                                            // Map the TDATA drive to it's orignal location
    HRESULT MapTDataToTitleID( char* pszTitleID );                          // Map the TDATA drive letter to the specified Title ID
    HRESULT MapTDataToTitleID( WCHAR* pwszTitleID );                        // Map the TDATA drive letter to the specified Title ID
    HRESULT MapTDataToTitleID( DWORD dwTitleID );                           // Map the TDATA drive letter to the specified Title ID

private:
    char m_cTDataDriveLetter;                                               // Drive letter for our TDATA

    ANSI_STRING m_ostTDataDrive;                                            // Used to create our link to the TDATA directory
    ANSI_STRING m_ostTDataPath;                                             // Used to create our link to the TDATA directory
    ANSI_STRING m_ostUDataDrive;                                            // Used to create our link to the UDATA directory
    ANSI_STRING m_ostUDataPath;                                             // Used to create our link to the UDATA directory
};

// Hard drive specific defines
#define HARDDRIVE_NICKNAME_FILENAME     "NICKNAME.XBN"
#define HARDDRIVE_NAME_PATTERN         L"Xbox Hard Disk"

#define HARDDRIVE_TDATA_DRIVE_LETTER_A      'V'
#define HARDDRIVE_TDATA_PATH_A              "\\Device\\Harddisk0\\partition1\\tdata"
#define HARDDRIVE_UDATA_DRIVE_LETTER_A      'X'
#define HARDDRIVE_UDATA_PATH_A              "\\Device\\Harddisk0\\partition1\\udata"

#endif // _HARDDRIVE_H_