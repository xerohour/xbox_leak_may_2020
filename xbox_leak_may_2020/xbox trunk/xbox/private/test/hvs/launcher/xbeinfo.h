/*****************************************************
*** xbeinfo.h
***
*** Header file for our XBE info class.
*** This file will contain the definition an XBE
*** info object.
*** 
*** by James N. Helm
*** November 27th, 2001
*** 
*****************************************************/

#ifndef _XBEINFO_H_
#define _XBEINFO_H_

#include <xbeimage.h>
#include "testconfig.h"

class CXBEInfo
{
public:
    CXBEInfo( void );
    ~CXBEInfo( void );

    // Methods
    void AddNewTestConfig( void );                                  // Add a test configuration based on the Default Config
    void AddTestConfig( CTestConfig* pTestConfig )                  // Add a test config to our XBE
        { m_TestConfigurations.AddNode( pTestConfig ); };           
                                                                    
    unsigned int GetNumTestConfigs( void );                         // Returns the number of test configs associated with this XBE
    
    void MoveToTestConfig( unsigned int uiIndex );                  // Move to the specified test configuration
	BOOL MoveNextTestConfig( void );                                // Move to the next test config in the list
	BOOL MovePrevTestConfig( void );                                // Move to the prev test config in the list
	BOOL MoveFirstTestConfig( void );                               // Move to the first test config in the list
	BOOL MoveLastTestConfig( void );                                // Move to the last test config in the list
    char* GetDirectory( void ) { return m_pszDirectory; };          // Get the directory of the XBE
    char* GetFilename( void ) { return m_pszFilename; };            // Get the filename of the XBE
    char* GetFileExtension( void ) { return m_pszFileExtension; };  // Get the file extension of the XBE
    WCHAR* GetTitleName( void ) { return m_pwszTitleName; };        // Get the Title Name of the XBE
    CTestConfig* GetDefaultTestConfig( void );                      // Get the default test config for our XBE
	CTestConfig* GetCurrentTestConfig( void );                      // Get the test config that is currently being pointed to
    unsigned int GetCurrentTestConfigIndex( void )                  // Get the index of the test config that is currently being pointed to
        { return m_TestConfigurations.GetCurrentIndex(); };
    BOOL GetLoaded( void ) { return m_bLoaded; };                   // Get if the XBE info was loaded from disk

	CTestConfig* GetTestConfig( unsigned int uiIndex );	            // Get the test config at the specified index
    HRESULT DeleteTestConfig( unsigned int uiIndex );	            // Delete the test config at the specified index
    HRESULT DeleteCurrentTestConfig( void );                        // Delete the test config at current index
    void ClearTestConfigs( void )                                   // Clear out all test configs for this XBE
        { m_TestConfigurations.EmptyList(); };

    void SetDirectory( char* pszDirectory );                        // Set the directory of the XBE
    void SetFilename( char* pszFilename );                          // Set the filename of the XBE
    void SetFileExtension( char* pszFileExtension );                // Set the file extension of the XBE
    void SetTitleName( WCHAR* pwszTitleName );                      // Set the Title Name of the XBE
    void SetLoaded( BOOL bLoad ) { m_bLoaded = bLoad; };            // Set that the XBE info was loaded from disk

    // Operators
    BOOL operator>( const CXBEInfo& item );
    BOOL operator<( const CXBEInfo& item );
    BOOL operator==( const CXBEInfo& item );

private:
    // Properties
    char m_pszDirectory[MAX_PATH + 1];
    char m_pszFilename[MAX_PATH + 1];
    char m_pszFileExtension[4];
    WCHAR m_pwszTitleName[XBEIMAGE_TITLE_NAME_LENGTH + 1];
    BOOL m_bLoaded;     // Used to determine if the XBE had it's config's loaded from the Hard Drive

    CTestConfig m_DefaultTestConfig;                    // Default Test Configuration for our XBE
    CLinkedList< CTestConfig* > m_TestConfigurations;   // Test Configurations for this XBE
};

#endif // _XBEINFO_H_
