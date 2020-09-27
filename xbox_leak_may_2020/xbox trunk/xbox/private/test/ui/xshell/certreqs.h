/*****************************************************
*** certreqs.h
***
*** Header file for our Certification Requirements 
*** class.  This class will configure the Xbox in
*** various ways to satisfy certain certification
*** requirements
***
*** by James N. Helm
*** May 26th, 2001
***
*****************************************************/

#ifndef _CERTREQS_H_
#define _CERTREQS_H_

class CCertReqs
{
public:
    // Constructors and Destructor
    CCertReqs();
    ~CCertReqs();

    // Public Properties
    
    // Public Methods
    HRESULT Req10_1_11();                           // Removes all System Files: Soundtracks and Nicknames
    HRESULT Req10_1_12();                           // List all global settings
    HRESULT Req10_2_01();                           // Fragments the TDATA and UDATA partitions
    HRESULT Req10_2_02();                           // Removes all Soundtracks
    HRESULT Req10_2_07( char pszTitleID[9] );       // Fills udata\<titleid> with 4096 entries
    HRESULT Req10_2_10( DWORD dwTitleID );          // Clears Utility partition for <titleid>
    HRESULT Req10_2_18();                           // Draws outline of non-HDTV safe area
    HRESULT Req10_2_19();                           // Draws outline of HDTV safe area
    // HRESULT Req10_3_02();                           // Fills udata area, but with less than 4096 entries
    // HRESULT Req10_3_03();                           // Fills tdata area, but with less than 4096 entries
    HRESULT Req10_3_04( DWORD dwFileSize );         // Fills tdata and udata area
    HRESULT Req10_3_11();                           // Lists available blocks for all attached memory devices
    HRESULT Req10_3_14();                           // Change Language Setting
    HRESULT Req10_3_15();                           // Change Time Zone Setting
    HRESULT Req10_3_26_1();                         // Create Soundtrack names with localized text
    HRESULT Req10_3_26_2();                         // Create Nicknames with localized text
    HRESULT Req10_3_26_3();                         // Create MU names with localized text
    HRESULT Req10_3_26_4( char pszTitleID[9] );     // Create Saved Game names with localized text
    HRESULT Req10_3_33();                           // Makes list of all files on console, both hard disk and MUs
    HRESULT Req10_6_05(  char pszTitleID[9] );      // Corrupts Saved Games for a specific Title

    HRESULT CleanUp( char* pszStartDir );           // Clean up any files created by the cert process
    HRESULT CleanDir( char* pszDir, char* pszFileExtension );

private:
    // Private Properties
    BOOL m_bUtilityMounted;                         // Used to determine if the utility drive was already mounted
    
    // Private Methods
    void DumpFiles( char* pszDirName,               // Dump files to our hard disk
                    char* pszDestFile,
                    unsigned int uiPartitionNumber );
};

#define FILE_DATA_FILELIST_FILENAME     FILE_EXECUTABLE_DIRECTORY_A "\\xbox_all_files.xdk"
#define FILE_DATA_PARTITION_PATTERN     "\\Device\\Harddisk0\\Partition"
#define FILE_DATA_DUMPFILE_LINK_DRIVE   'W'

#endif // _CERTREQS_H_