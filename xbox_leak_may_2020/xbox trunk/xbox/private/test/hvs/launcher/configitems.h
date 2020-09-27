/*****************************************************
*** configitems.h
***
*** Header file for our config item classes.
*** This file will contain the definition for all
*** types of config items
*** 
*** by James N. Helm
*** November 27th, 2001
*** 
*****************************************************/

#ifndef _CONFIGITEMS_H_
#define _CONFIGITEMS_H_

/*****************************************************
/* CConfigItem( void )
/****************************************************/
class CConfigItem
{
public:
    CConfigItem( void );
    CConfigItem( const CConfigItem& CConfigItem );
    virtual ~CConfigItem( void );
    
    // Operators
    virtual CConfigItem& operator=( const CConfigItem& CConfigItem );

    // Methods
    virtual HRESULT SetFieldName( WCHAR* pwszName );                                // Set the Field Name
    virtual HRESULT SetHelpText( WCHAR* pwszText );                                 // Set the Help Text
    virtual HRESULT SetType( WCHAR* pwszType );                                     // Type of item
    virtual void SetProcessed( BOOL bProcessed ) { m_bProcessed = bProcessed; };    // Should be set after an item is processed

    virtual BOOL GetProcessed( void ) { return m_bProcessed; };                     // Should be set after an item is processed
    virtual WCHAR* GetFieldName( void ) const { return m_pwszFieldName; };          // Get the Field Name
    virtual WCHAR* GetHelpText( void ) const { return m_pwszHelpText; };            // Get the Help Text
    virtual WCHAR* GetType( void ) const { return m_pwszType; };                    // Get the Type

    virtual HRESULT ProcessValue( char* pszValue ) = 0;                             // Process a value that was passed to the item

    // Operators
    virtual BOOL operator>( const CConfigItem& item );
    virtual BOOL operator<( const CConfigItem& item );
    virtual BOOL operator==( const CConfigItem& item );

protected:
    // Properties
    WCHAR* m_pwszFieldName;     // Name of the field we are dealing with
    WCHAR* m_pwszHelpText;      // Help text for the field we are dealing with
    WCHAR* m_pwszType;          // Type of item stored (string, number, etc)
    BOOL m_bProcessed;          // Determines if the item was processed
};


/*****************************************************
/* CConfigNumber( void )
/****************************************************/
class CConfigNumber : public CConfigItem
{
public:
    // Constructors and Destructors
    CConfigNumber( void );
    CConfigNumber( const CConfigNumber& ConfigNumber );
    ~CConfigNumber( void );
    
    // Operators
    CConfigNumber& operator=( const CConfigNumber& ConfigNumber );

    // Methods
    HRESULT ProcessValue( char* pszValue );     // Process a value that was passed to the item

    void SetDefault( long lDefault );                               // Set the Default value
    void SetMinimum( long lMinimum ) { m_lMinimum = lMinimum; };    // Set the Minimum value
    void SetMaximum( long lMaximum ) { m_lMaximum = lMaximum; };    // Set the Maximum value

    long GetDefault( void ) const { return m_lDefault; };           // Get the Default value
    long GetMinimum( void ) const { return m_lMinimum; };           // Get the Minimum value
    long GetMaximum( void ) const { return m_lMaximum; };           // Get the Maximum value

    void UpdateItem( WCHAR* pwszName,                               // Allow the user to update all values at once
                     WCHAR* pwszHelpText,
                     WCHAR* pwszType,
                     long lDefault,
                     long lMinimum,
                     long lMaximum );

private:
    long m_lDefault;    // Default Value
    long m_lMinimum;    // Minimum Value
    long m_lMaximum;    // Maximum Value
};


/*****************************************************
/* CConfigRange( void )
/****************************************************/
class CConfigRange : public CConfigItem
{
public:
    // Constructors and Destructors
    CConfigRange( void );
    CConfigRange( const CConfigRange& CConfigRange );
    ~CConfigRange( void );
    
    // Operators
    CConfigRange& operator=( const CConfigRange& CConfigRange );

    // Methods
    HRESULT ProcessValue( char* pszValue );     // Process a value that was passed to the item

    void SetDefault1( long lDefault );                              // Set the Default 1st value
    void SetMinimum1( long lMinimum ) { m_lMinimum1 = lMinimum; };  // Set the Minimum 1st value
    void SetMaximum1( long lMaximum ) { m_lMaximum1 = lMaximum; };  // Set the Maximum 1st value
    void SetDefault2( long lDefault );                              // Set the Default 2nd value
    void SetMinimum2( long lMinimum ) { m_lMinimum2 = lMinimum; };  // Set the Minimum 2nd value
    void SetMaximum2( long lMaximum ) { m_lMaximum2 = lMaximum; };  // Set the Maximum 2nd value

    long GetDefault1( void ) { return m_lDefault1; };               // Get the Default 1st value
    long GetMinimum1( void ) { return m_lMinimum1; };               // Get the Minimum 1st value
    long GetMaximum1( void ) { return m_lMaximum1; };               // Get the Maximum 1st value
    long GetDefault2( void ) { return m_lDefault2; };               // Get the Default 2nd value
    long GetMinimum2( void ) { return m_lMinimum2; };               // Get the Minimum 2nd value
    long GetMaximum2( void ) { return m_lMaximum2; };               // Get the Maximum 2nd value

    void UpdateItem( WCHAR* pwszName,                               // Allow the user to update all values at once
                     WCHAR* pwszHelpText,
                     WCHAR* pwszType,
                     long lDefault1,
                     long lMinimum1,
                     long lMaximum1, 
                     long lDefault2,
                     long lMinimum2,
                     long lMaximum2 );

private:
    long m_lDefault1;   // Default 1st Value
    long m_lMinimum1;   // Minimum 1st Value
    long m_lMaximum1;   // Maximum 1st Value
    long m_lDefault2;   // Default 2nd Value
    long m_lMinimum2;   // Minimum 2nd Value
    long m_lMaximum2;   // Maximum 2nd Value
};


/*****************************************************
/* CConfigString( void )
/****************************************************/
class CConfigString : public CConfigItem
{
public:
    // Constructors and Destructors
    CConfigString( void );
    CConfigString( const CConfigString& ConfigString );
    ~CConfigString( void );
    
    // Operators
    CConfigString& operator=( const CConfigString& ConfigString );

    // Methods
    HRESULT ProcessValue( char* pszValue );             // Process a value that was passed to the item
                                                        
    HRESULT AddString( WCHAR* pwszString );             // Add a string item to our list
                                                        
    WCHAR* GetString( unsigned int uiIndex ) const      // Return the pointer to the string at the specified index
        { return m_StringList[uiIndex]; };              
                                                        
    WCHAR* GetCurrentString( void ) const               // Return the pointer to the string currently being pointed at
        { return m_StringList.GetCurrentNode(); };      
                                                        
    BOOL MoveFirstString( void );                       // Move to the first string in the list
    BOOL MoveLastString( void );                        // Move to the last string in the list
    BOOL MoveNextString( void );                        // Move to the next string in the list
    BOOL MovePrevString( void );                        // Move to the previous string in the list
                                                        
    void SetCurrentString( unsigned int uiIndex )       // Set the current string via index
        { m_StringList.MoveTo( uiIndex ); };            
                                                        
    unsigned int GetCurrentStringIndex( void ) const    // Get the index of the current string
        { return m_StringList.GetCurrentIndex(); };     
                                                        
    unsigned int GetNumStrings( void ) const            // Get the number of strings in our config item
        { return m_StringList.GetNumItems(); };

    void UpdateItem( WCHAR* pwszName,                   // Update the informational part of the item
                     WCHAR* pwszHelpText,
                     WCHAR* pwszType );

private:
    // BUGBUG: This linked list class uses "delete", not "delete[]" to clear the data items.
    // BUGBUG: Since we are creating a WCHAR* (new []), this is a bug!!
    CLinkedList< WCHAR* > m_StringList;  // String list -- 1st item is default
};


/*****************************************************
/* CConfigString2( void )
/****************************************************/
class CConfigString2 : public CConfigItem
{
public:
    // Constructors and Destructors
    CConfigString2( void );
    CConfigString2( const CConfigString2& ConfigString2 );
    ~CConfigString2( void );
    
    // Operators
    CConfigString2& operator=( const CConfigString2& ConfigString2 );

    // Methods
    HRESULT ProcessValue( char* pszValue );             // Process a value that was passed to the item

    HRESULT SetString( WCHAR* pwszString );             // Set the string value
    WCHAR* GetString( void ) const                      // Return the pointer to the string
        { return m_pwszString; };

    void UpdateItem( WCHAR* pwszName,                   // Update the informational part of the item
                     WCHAR* pwszHelpText,
                     WCHAR* pwszType,
                     WCHAR* pwszString );

private:
    WCHAR* m_pwszString;                                // Value of our current item
};


/*****************************************************
/* CConfigNumberString( void )
/****************************************************/
class CConfigNumberString : public CConfigItem
{
public:
    // Constructors and Destructors
    CConfigNumberString( void );
    CConfigNumberString( const CConfigNumberString& ConfigNumberString );
    ~CConfigNumberString( void );
    
    // Operators
    CConfigNumberString& operator=( const CConfigNumberString& ConfigNumberString );

    // Methods
    HRESULT ProcessValue( char* pszValue );                         // Process a value that was passed to the item

    void SetDefault( long lDefault );                               // Set the Default value
    void SetMinimum( long lMinimum ) { m_lMinimum = lMinimum; };    // Set the Minimum value
    void SetMaximum( long lMaximum ) { m_lMaximum = lMaximum; };    // Set the Maximum value

    long GetDefault( void ) const { return m_lDefault; };           // Get the Default value
    long GetMinimum( void ) const { return m_lMinimum; };           // Get the Minimum value
    long GetMaximum( void ) const { return m_lMaximum; };           // Get the Maximum value

    HRESULT AddString( WCHAR* pwszString );                         // Add a string item to our list
    
    WCHAR* GetString( unsigned int uiIndex ) const                  // Return the pointer to the string at the specified index
        { return m_StringList[uiIndex]; };
    
    WCHAR* GetCurrentString( void ) const                           // Return the pointer to the string currently being pointed at
        { return m_StringList.GetCurrentNode(); };
    
BOOL MoveFirstString( void );                                       // Move to the first string in the list
BOOL MoveLastString( void );                                        // Move to the last string in the list
BOOL MoveNextString( void );                                        // Move to the next string in the list
BOOL MovePrevString( void );                                        // Move to the previous string in the list

    void SetCurrentString( unsigned int uiIndex )                   // Set the current string via index
        { m_StringList.MoveTo( uiIndex ); };

    unsigned int GetCurrentStringIndex( void ) const                // Get the index of the current string
        { return m_StringList.GetCurrentIndex(); };

    unsigned int GetNumStrings( void ) const                        // Get the number of strings in our config item
        { return m_StringList.GetNumItems(); };

    void UpdateItem( WCHAR* pwszName,                               // Allow the user to update all values, except the string value, at once
                     WCHAR* pwszHelpText,
                     WCHAR* pwszType,
                     long lDefault,
                     long lMinimum,
                     long lMaximum );

private:
    long m_lDefault;    // Default Value
    long m_lMinimum;    // Minimum Value
    long m_lMaximum;    // Maximum Value

    // BUGBUG: This linked list class uses "delete", not "delete[]" to clear the data items.
    // BUGBUG: Since we are creating a WCHAR* (new []), this is a bug!!
    CLinkedList< WCHAR* > m_StringList;  // String list -- 1st item is default
};

#endif // _CONFIGITEMS_H_