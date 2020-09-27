//  LANGID.H
//
//  Created 18-Feb-2002 [JimHelm]

#ifndef _LANGID_H_
#define _LANGID_H_

#include <list>

class CLangID
{
public:
    // Constructors and Destructor
    CLangID();
    CLangID( const CLangID& LangID );
    ~CLangID();

    // Public Methods
    BOOL ParseLangIDs( char* pszLine );     // Takes a NULL terminated string, parses it for lang IDs
                                            // which should be separated by colons ':' (if there is more
                                            // than one), and adds them to the list
    BOOL IsLangIDValid( LANGID lid );       // Will return TRUE if the Lang ID is valid for this machine
    BOOL MoveFirstLangID();                 // Move our pointer to the first item in the list
    BOOL MoveNextLangID();                  // Move our pointer to the next item in the list
    DWORD GetCurrentLangID();               // Retreive the current langID being pointed to
    DWORD GetNumLangIDs()                   // Returns the number of lang IDs in the list
        { return m_LangIDs.size(); };

private:
    // List of LangIDs
    std::list<DWORD> m_LangIDs;             // Linked list of LANGIDs
    unsigned int m_uiListPosition;          // Used to track the position in our list

};

#endif // _LANGID_H_