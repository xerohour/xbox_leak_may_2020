/*****************************************************
*** testconfig.h
***
*** Header file for our Test Config info class.
*** This class will hold information on a
*** configuration for a test.
*** 
*** by James N. Helm
*** December 5th, 2001
*** 
*****************************************************/

#ifndef _TESTCONFIG_H_
#define _TESTCONFIG_H_

#include "configitems.h"

class CTestConfig
{
public:
    CTestConfig( void );
    CTestConfig( const CTestConfig& TestConfig );
    ~CTestConfig( void );

    // Operators
    CTestConfig& operator=( const CTestConfig& TestConfig );

    long m_lConfigVersion;                                      // Used to determine which config version we should use
    CLinkedList< CConfigItem* > m_ConfigItems;                  
                                                                
    // Methods                                                  
    void AddConfigItem( CConfigItem* pConfigItem )              // Add a config item to our list
        { m_ConfigItems.AddNode( pConfigItem ); };              
                                                                
    unsigned int GetNumConfigItems( void ) const;               // Returns the number of config items this configuration contains
    void SetConfigVersion( long lVersion )                      // Set the configuration version
        { m_lConfigVersion = lVersion; };
    long GetConfigVersion( void ) const                         // Return the current configuration version
        { return m_lConfigVersion; };
    void ClearConfigItems( void )                               // Clear out the config items
        { m_ConfigItems.EmptyList(); };

	BOOL MoveNextConfigItem( void );                            // Move to the next config item in the list
	BOOL MovePrevConfigItem( void );                            // Move to the prev config item in the list
	BOOL MoveFirstConfigItem( void );                           // Move to the first config item in the list
	BOOL MoveLastConfigItem( void );                            // Move to the last config item in the list
	CConfigItem* GetCurrentConfigItem( void ) const;            // Get the config item that is currently being pointed to
	CConfigItem* GetConfigItem( unsigned int uiIndex ) const;   // Get the config item at the specified index

    // Operators
    BOOL operator>( const CTestConfig& item );
    BOOL operator<( const CTestConfig& item );
    BOOL operator==( const CTestConfig& item );
};

#endif // _TESTCONFIG_H_
