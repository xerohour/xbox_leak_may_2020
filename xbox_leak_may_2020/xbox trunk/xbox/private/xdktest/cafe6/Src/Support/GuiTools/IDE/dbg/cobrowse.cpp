///////////////////////////////////////////////////////////////////////////////
//  COBROWSE.CPP
//
//  Created by :            Date :
//      WayneBr                 1/21/94
//
//  Description :
//      Implementation of the COBrowse class
//

#include "stdafx.h"
#include "cobrowse.h"
#include "testxcpt.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"
#include "..\eds\ueditor.h"
#include "Strings.h"
#include "..\shl\wbutil.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// indexed by BROWSER_LOC_TYPE for use in log (see VerifyMultDefRefMatch).
CString blt_str[2] = {"def", "ref"};


// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::GotoNthDefinition(int index, LPCSTR name, int ambiguity /*= NoAmbiguity*/)
// Description: Go to the n-th definition of the given symbol.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that specifies the n-th definition to go to.
// Param: name A pointer to a string that contains the name of the symbol to go to.
// Param: ambiguity An integer that specifies the index of the symbol in the list of ambiguous symbols, if applicable. If ambiguity is not expected, set this value to NoAmbiguity. (Default value is NoAmbiguity.)
// END_HELP_COMMENT
BOOL COBrowse::GotoNthDefinition(int index, LPCSTR name,int ambiguity ) {
    if (name) UIWB.SetToolbarCombo(name);
	//We don't need assertion here since the function always return true
    //EXPECT(m_uibrowse.FirstDefinition());
	m_uibrowse.FirstDefinition();

	// check for ambiguity resolution dialog
	UIAmbiguiousSymbolDialog dlg;
	if( dlg.AttachActive() )
	{
		if( ambiguity == NoAmbiguity ) {
			LOG->RecordInfo( "COBrowse::GotoNthDefinition - Ambiguity not expected" );
			dlg.Cancel();
			return FALSE;
		}
		else {
			if( !dlg.SelectAmbiguity( ambiguity ) ) {
				LOG->RecordInfo( "COBrowse::GotoNthDefinition - Could not select ambiguity item %d", ambiguity );
				dlg.Cancel();
				return FALSE;
			}
		}
	}
	else
	{
		// No ambiguity resolution dialog, make sure it wasn't expected.
		if( ambiguity >= 0 ) {
			LOG->RecordInfo( "COBrowse::GotoNthDefinition - Expected ambiguity resolution dialog" );
			return FALSE;
		}
	}
    for (int count=2; count<index; count++) m_uibrowse.NextDefinition();
    return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::GotoNthReference(int index, LPCSTR name, int ambiguity /*= NoAmbiguity*/)
// Description: Go to the n-th reference of the given symbol.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that specifies the n-th reference to go to.
// Param: name A pointer to a string that contains the name of the symbol to go to.
// Param: ambiguity An integer that specifies the index of the symbol in the list of ambiguous symbols, if applicable. If ambiguity is not expected, set this value to NoAmbiguity. (Default value is NoAmbiguity.)
// END_HELP_COMMENT
BOOL COBrowse::GotoNthReference(int index, LPCSTR name, int ambiguity) {
    if (name) UIWB.SetToolbarCombo(name);
    EXPECT(m_uibrowse.FirstReference());

	// check for ambiguity resolution dialog
	UIAmbiguiousSymbolDialog dlg;
	if( dlg.AttachActive() )
	{
		if( ambiguity == NoAmbiguity ) {
			LOG->RecordInfo( "COBrowse::GotoNthReference - Ambiguity not expected" );
			dlg.Cancel();
			return FALSE;
		}
		else {
			if( !dlg.SelectAmbiguity( ambiguity ) ) {
				LOG->RecordInfo( "COBrowse::GotoNthReference - Could not select ambiguity item %d", ambiguity );
				dlg.Cancel();
				return FALSE;
			}
		}
	}
	else
	{
		// No ambiguity resolution dialog, make sure it wasn't expected.
		if( ambiguity >= 0 ) {
			LOG->RecordInfo( "COBrowse::GotoNthReference - Expected ambiguity resolution dialog" );
			return FALSE;
		}
	}
    for (int count=2; count<index; count++) m_uibrowse.NextReference();
    return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::NextDefinition(void)
// Description: Go to the next definition of the currently selected symbol.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COBrowse::NextDefinition(void) {
    return m_uibrowse.NextDefinition();
}

// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::PrevDefinition(void)
// Description: Go to the previous definition of the currently selected symbol.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COBrowse::PrevDefinition(void) {
    return m_uibrowse.PrevDefinition();
}


// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::NextReference(void)
// Description: Go to the next reference of the currently selected symbol.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COBrowse::NextReference(void) {
    return m_uibrowse.NextReference();
}

// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::PrevReference(void)
// Description: Go to the previous reference of the currently selected symbol.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COBrowse::PrevReference(void) {
    return m_uibrowse.PrevReference();
}

// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::PopContext(void)
// Description: Pop context to the previous cursor location/context.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COBrowse::PopContext(void) {
    if (!m_uibrowse.PopContext())
    {
        // log warning
        LOG->RecordInfo( (LPCSTR) "Can't pop context");
        return FALSE;
    }
    return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::GetMatches( CStringArray& matches, CString name, QueryType type, int filter /*= NONE*/)
// Description: Get the list of symbols that match the name specified when the specified query is performed.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: matches A CStringArray reference that will contain the list of symbols that match the symbols specified by name.
// Param: name A CString that contains the name of the symbol.
// Param: type A QueryType value that specifies the type of query to perform: DefRef, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph.
// Param: filter A bit field that specifies the type of filter to apply to the query: NONE, CLASSES, FUNCTIONS, DATA, MACROS, TYPES. (Default value is NONE.)
// END_HELP_COMMENT
BOOL COBrowse::GetMatches( CStringArray& matches, CString name, QueryType type, int filter ) {

	matches.RemoveAll();

	if( m_uibrowse.Query( type, name, filter ) ) {
		EXPECT( m_uibrowse.IsActive() );
		if( m_uibrowse.GetMatches( matches ) ) {
			// remove filname which is listed with matches from file outline.
			if( type == FileOutline ) {
				EXPECT( matches.GetSize() > 0 );
				matches.RemoveAt( 0 );
			}

			m_uibrowse.CloseBrowser();
			return TRUE;
		}
		else {
			m_uibrowse.CloseBrowser();
			return FALSE;
		}
	}
	else if( m_uibrowse.GetStatus() == UIBrowse::AmbiguityDialog ) {

		CStringArray amb;
		EXPECT( m_uibrowse.GetAmbiguities( amb ) );

		// Cancel dialog
		m_uibrowse.CancelAmbiguity();
		int nAmb = amb.GetSize();

		for( int i=0; i < nAmb; i++ ) {
			CStringArray tmp;
			EXPECT( !m_uibrowse.Query( type, name, filter ) );
			EXPECT( m_uibrowse.GetStatus() == UIBrowse::AmbiguityDialog );
			EXPECT( m_uibrowse.SelectAmbiguity( i ) );
			EXPECT( m_uibrowse.GetMatches( tmp ) );
			matches.InsertAt( matches.GetSize(), &tmp );
			m_uibrowse.CloseBrowser();
		}
		return TRUE;
	}
	else {
		LOG->RecordInfo( "COBrowse::GetMatches - Query failed" );
		return FALSE;
	}
}

// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::GetDefinitions( CStringArray& definitions, CString name, QueryType type, int filter /*= NONE*/, int ambiguity /*= NoAmbiguity*/)
// Description: Get the list of definitions for a given symbol when a specified query is performed.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: definitions A CStringArray reference that will contain the list of definitions for the given symbol.
// Param: name A CString that contains the name of the symbol.
// Param: type A QueryType value that specifies the type of query to perform: DefRef, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph.
// Param: filter A bit field that specifies the type of filter to apply to the query: NONE, CLASSES, FUNCTIONS, DATA, MACROS, TYPES. (Default value is NONE.)
// Param: ambiguity An integer that specifies the index of the symbol in the list of ambiguous symbols, if applicable. If ambiguity is not expected, set this value to NoAmbiguity. (Default value is NoAmbiguity.)
// END_HELP_COMMENT
BOOL COBrowse::GetDefinitions( CStringArray& definitions, CString name, QueryType type, int filter, int ambiguity ) {

	definitions.RemoveAll();

	if( !m_uibrowse.Query( type, name, filter ) ) {
		if( m_uibrowse.GetStatus() == UIBrowse::AmbiguityDialog ) {

			if( ambiguity == NoAmbiguity ) {
				// Cancel dialog
				m_uibrowse.CancelAmbiguity();
				LOG->RecordInfo( "COBrowse::GetDefintions - Ambiguity not expected" );
				return FALSE;
			}
			else {
				// select ambiguity
				if( !m_uibrowse.SelectAmbiguity( ambiguity ) ) {
					m_uibrowse.CancelAmbiguity();
					LOG->RecordInfo( "COBrowse::GetDefintions - SelectAmbiguity failed" );
					return FALSE;
				}
			}
		}
		else {
			LOG->RecordInfo( "COBrowse::GetDefintions - Query failed" );
			return FALSE;
		}
	}

	// don't care about refs, but must pass dummy param.
	CStringArray references;
	return GetLocations(definitions, references);
}


// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::GetReferences( CStringArray& references, CString name, QueryType type, int filter /*= NONE*/, int ambiguity /*= NoAmbiguity*/)
// Description: Get the list of references for a given symbol when a specified query is performed.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: references A CStringArray reference that will contain the list of references for the given symbol.
// Param: name A CString that contains the name of the symbol.
// Param: type A QueryType value that specifies the type of query to perform: DefRef, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph.
// Param: filter A bit field that specifies the type of filter to apply to the query: NONE, CLASSES, FUNCTIONS, DATA, MACROS, TYPES. (Default value is NONE.)
// Param: ambiguity An integer that specifies the index of the symbol in the list of ambiguous symbols, if applicable. If ambiguity is not expected, set this value to NoAmbiguity. (Default value is NoAmbiguity.)
// END_HELP_COMMENT
BOOL COBrowse::GetReferences( CStringArray& references, CString name, QueryType type, int filter, int ambiguity ) {

	references.RemoveAll();

	if( !m_uibrowse.Query( type, name, filter ) ) {
		if( m_uibrowse.GetStatus() == UIBrowse::AmbiguityDialog ) {

			if( ambiguity == NoAmbiguity ) {
				// Cancel dialog
				m_uibrowse.CancelAmbiguity();
				LOG->RecordInfo( "COBrowse::GetReferences - Ambiguity not expected" );
				return FALSE;
			}
			else {
				// select ambiguity
				if( !m_uibrowse.SelectAmbiguity( ambiguity ) ) {
					m_uibrowse.CancelAmbiguity();
					LOG->RecordInfo( "COBrowse::GetReferences - SelectAmbiguity failed" );
					return FALSE;
				}
			}
		}
		else {
			LOG->RecordInfo( "COBrowse::GetReferences - Query failed" );
			return FALSE;
		}
	}

	// don't care about defs, but must pass dummy param.
	CStringArray definitions;
	return GetLocations(definitions, references);
}


// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::GetLocations(CStringArray& defs, CStringArray& refs)
// Description: Get the current list of definitnios and references and put them in separate lists.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: defs A CStringArray reference that will contain the list of current definitions. may be NULL if refs isn't.
// Param: refs A CStringArray reference that will contain the list of current references. may be NULL if defs isn't.
// END_HELP_COMMENT
BOOL COBrowse::GetLocations(CStringArray& defs, CStringArray& refs)

	{
	EXPECT(m_uibrowse.IsActive());
	CString defStr = GetLocString(IDSS_BRS_DEFINITIONS);
	CString refStr = GetLocString(IDSS_BRS_REFERENCES);
	CStringArray locs;

	if(m_uibrowse.GetLocations(locs))

		{
		EXPECT(locs[0] == defStr);
		int i = 1;
	
		while((i < locs.GetSize()) && (locs[i] != refStr))
			{
			defs.Add(locs[i]);
			i++;
			}

		EXPECT(locs[i] == refStr);
		i++;

		while(i < locs.GetSize())
			{
			refs.Add(locs[i]);
			i++;
			}

		return TRUE;
		}

	LOG->RecordInfo("COBrowse::GetLocations - couldn't get locations" );
	return FALSE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::GetMembers( CStringArray& members, CString name, QueryType type, int filter /*= NONE*/, int ambiguity /*= NoAmbiguity*/)
// Description: Get the list of members for a given symbol when a specified query is performed.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: members A CStringArray reference that will contain the list of members for the given symbol.
// Param: name A CString that contains the name of the symbol.
// Param: type A QueryType value that specifies the type of query to perform: DefRef, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph.
// Param: filter A bit field that specifies the type of filter to apply to the query: NONE, CLASSES, FUNCTIONS, DATA, MACROS, TYPES. (Default value is NONE.)
// Param: ambiguity An integer that specifies the index of the symbol in the list of ambiguous symbols, if applicable. If ambiguity is not expected, set this value to NoAmbiguity. (Default value is NoAmbiguity.)
// END_HELP_COMMENT
BOOL COBrowse::GetMembers( CStringArray& members, CString name, QueryType type, int filter, int ambiguity ) {
	members.RemoveAll();

	if( !m_uibrowse.Query( type, name, filter ) ) {
		if( m_uibrowse.GetStatus() == UIBrowse::AmbiguityDialog ) {

			if( ambiguity == NoAmbiguity ) {
				// Cancel dialog
				m_uibrowse.CancelAmbiguity();
				LOG->RecordInfo( "COBrowse::GetMembers - Ambiguity not expected" );
				return FALSE;
			}
			else {
				// select ambiguity
				if( !m_uibrowse.SelectAmbiguity( ambiguity ) ) {
					m_uibrowse.CancelAmbiguity();
					LOG->RecordInfo( "COBrowse::GetMembers - SelectAmbiguity failed" );
					return FALSE;
				}
			}
		}
		else {
			LOG->RecordInfo( "COBrowse::GetMembers - Query failed" );
			return FALSE;
		}
	}

	EXPECT( m_uibrowse.IsActive() );
	if( m_uibrowse.GetMembers( members ) ) {
		m_uibrowse.CloseBrowser();

		// Filter for public|protected|filter
		int index = 0;
		while( index < members.GetSize() ) {
			if( members[index] == "Public:" ) {
				members.RemoveAt( index );
			}
			else if( members[index] == "Protected:" ) {
				members.RemoveAt( index );
			}
			else if( members[index] == "Private:" ) {
				members.RemoveAt( index );
			}
			else {
				index++;
			}
		}
		return TRUE;
	}
	else {
		m_uibrowse.CloseBrowser();
		return FALSE;
	}
}


// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::GetFirstLevelBranches( CStringArray& branches, CString name, QueryType type, int filter /*= NONE*/, int ambiguity /*= NoAmbiguity*/)
// Description: Get the list of first level branches for a given symbol when a specified query is performed.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: branches A CStringArray reference that will contain the list of first level branches for the given symbol.
// Param: name A CString that contains the name of the symbol.
// Param: type A QueryType value that specifies the type of query to perform: DefRef, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph.
// Param: filter A bit field that specifies the type of filter to apply to the query: NONE, CLASSES, FUNCTIONS, DATA, MACROS, TYPES. (Default value is NONE.)
// Param: ambiguity An integer that specifies the index of the symbol in the list of ambiguous symbols, if applicable. If ambiguity is not expected, set this value to NoAmbiguity. (Default value is NoAmbiguity.)
// END_HELP_COMMENT
BOOL COBrowse::GetFirstLevelBranches( CStringArray& branches, CString name, QueryType type, int filter, int ambiguity ) {
	branches.RemoveAll();

	if( !m_uibrowse.Query( type, name, filter ) ) {
		if( m_uibrowse.GetStatus() == UIBrowse::AmbiguityDialog ) {

			if( ambiguity == NoAmbiguity ) {
				// Cancel dialog
				m_uibrowse.CancelAmbiguity();
				LOG->RecordInfo( "COBrowse::GetFirstLevelBranches - Ambiguity not expected" );
				return FALSE;
			}
			else {
				// select ambiguity
				if( !m_uibrowse.SelectAmbiguity( ambiguity ) ) {
					m_uibrowse.CancelAmbiguity();
					LOG->RecordInfo( "COBrowse::GetFirstLevelBranches - SelectAmbiguity failed" );
					return FALSE;
				}
			}
		}
		else {
			LOG->RecordInfo( "COBrowse::GetFirstLevelBranches - Query failed" );
			return FALSE;
		}
	}

	EXPECT( m_uibrowse.IsActive() );
	if( m_uibrowse.GetFirstLevelBranches( branches ) ) {
		m_uibrowse.CloseBrowser();
		return TRUE;
	}
	else {
		m_uibrowse.CloseBrowser();
		return FALSE;
	}
}


// Verifies line in file in source window
// uses notation returned from GetNthDefinition/reference
// ie "f:\cafe\cafe\wb\cobrowse.h(33)"
// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::VerifyNavigation(LPCSTR file_line)
// Description: Verify browser navigation navigated to the correct file and line number.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: file_line A pointer to a string that contains the file and line number to verify. The form of the string is: filename(line); example: f:\cafe\cafe\wb\cobrowse.h(33).
// END_HELP_COMMENT
BOOL COBrowse::VerifyNavigation(LPCSTR file_line) {
    char file[255];
    int line;
    BOOL success= TRUE;
    EXPECT(m_uibrowse.CrackFileLine(file_line, (char *) &file, &line));

	//
	//		Verify filename.
	//    
	CString strCurFile( ((UIEditor)UIWB.GetActiveEditor()).GetTitle() );
	CString strFile( file );

	//
	//	Case-insensitive compare works since all supportted systems are case-insensitive.
	//	May need to test file system for case-sensitivity someday.
	success = strFile.CompareNoCase( strCurFile ) == 0;

	//
	//		Verify file line.
	//    
    success = success && ( UIWB.GetEditorCurPos(GECP_LINE) == line );
    
    return success;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COBrowse::VerifyMultDefRefMatches(SYM_DEFS_REFS *expected, LPCSTR name)
// Description: Do def/ref query on name and verify all matches are listed along with correct defs/refs for each.  order is not verified.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expected An array of SYM_DEFS_REFS objects containing all matches, defs, and refs expected in browser window.
// Param: name An LPCSTR that contains the name of the symbol to query (default is "").
// END_HELP_COMMENT

BOOL COBrowse::VerifyMultDefRefMatches(CPtrArray& expected, LPCSTR query_name /* "" */)
	
	{
	CStringArray locs[2];
	CString cstr, sym_match;
	char str[8];
	int isym, iloc_act, iloc_exp, blt, count;
	BOOL result = TRUE;

	// do the query.
	if(!m_uibrowse.Query(DefRef, query_name))
		
		{
		LOG->RecordInfo("COBrowse::VerifyMultDefRefMatches - "
						"query on symbol \"%s\" failed", query_name);
		
		return FALSE;
		}

	// perform for each SYM_DEFS_REFS object.
	for(isym = 0; isym < expected.GetSize(); isym++)

		{
		sym_match = BuildSymbolMatchStr((SDR *)expected[isym]);

		// if symbol is commented, we expect not to find it in browser wnd.
		if(((SDR *)expected[isym])->commented)
			
			{
			if(m_uibrowse.FindMatch(sym_match) > -1)
				
				{
				LOG->RecordInfo("COBrowse::VerifyMultDefRefMatches - "
								"commented symbol \"%s\" found", sym_match);
			
				result = FALSE;
				continue;
				}
			}

		// symbol isn't commented so verify it's defs/refs.
		else 
		
			{
			if(!m_uibrowse.SelectMatch(sym_match))
				
				{
				LOG->RecordInfo("COBrowse::VerifyMultDefRefMatches - "
								"couldn't select symbol \"%s\"", sym_match);

				result = FALSE;
				continue;
				}
		
			// empty def and ref arrays that will recieve defs and refs in browser wnd.
			locs[LOC_DEF].RemoveAll();
			locs[LOC_REF].RemoveAll();

			// get the defs and refs for the currently selected symbol.
			if(!GetLocations(locs[LOC_DEF], locs[LOC_REF]))			
				
				{
				LOG->RecordInfo("COBrowse::VerifyMultDefRefMatches - "
								"couldn't get defs/refs for symbol \"%s\"", sym_match);

				result = FALSE;
				continue;
				}

			// verify that defs/refs match expected.
			for(blt = 0; blt < 2; blt++)
				
				{
				count = 0;

				// get the count of uncommented defs/refs.
				for(iloc_exp = 0; iloc_exp < ((SDR *)expected[isym])->locs[blt].GetSize(); iloc_exp++)
					{
					if(!(((BL *)(((SDR *)expected[isym])->locs[blt][iloc_exp]))->commented))
						count++;
					}
				
				// compare it to actual count.
				if(count != locs[blt].GetSize())
					
					{
					LOG->RecordInfo("COBrowse::VerifyMultDefRefMatches - "
									"%s count for symbol \"%s\" is %d instead of expected %d",
									blt_str[blt], sym_match, 
									locs[blt].GetSize(), count);

					result = FALSE;
					continue;
					}

				// start with the first def/ref element.
				iloc_exp = 0;

				// peform for each def/ref displayed in browser window.
				for(iloc_act = 0; iloc_act < locs[blt].GetSize(); iloc_act++)
					
					{
					// advance to next expected uncommented def/ref.
					while(((BL *)(((SDR *)expected[isym])->locs[blt][iloc_exp]))->commented)
						iloc_exp++;
						
					// TODO (michma): enable full string comparison.
					// build ref/def string as displayed in browser wnd.
					cstr = ((BL *)(((SDR *)expected[isym])->locs[blt][iloc_exp]))->file + "(" +
						   _itoa(((BL *)(((SDR *)expected[isym])->locs[blt][iloc_exp]))->line, str, 10) + ")";

					// search for the def/ref string in the actual string.
					if(locs[blt][iloc_act].Find(cstr) < 0)

						{
						LOG->RecordInfo("COBrowse::VerifyMultDefRefMatches - "
										"mismatch for \"%s\" of symbol \"%s\": "
										"could not find expected \"%s\" in \"%s\"",
										((BL *)(((SDR *)expected[isym])->locs[blt][iloc_exp]))->name, 
										sym_match, cstr, locs[blt][iloc_act]);
						
						result = FALSE;
						continue;
						}

					// advance to next def/ref element.
					iloc_exp++;
					}
				}
			}
		}

	if(!m_uibrowse.CloseBrowser())
		{
		LOG->RecordInfo("COBrowse::VerifyMultDefRefMatches - failed to close browser");
		return FALSE;
		}

	return result;
	}


CString COBrowse::BuildSymbolMatchStr(SDR *sdr)

	{
	CString sym_type_str;

	if(sdr->sym_type == "var")
		sym_type_str = " (variable)";
	else if(sdr->sym_type == "param")
		sym_type_str = " (parameter";
	else if(sdr->sym_type == "struct")
		sym_type_str = " (struct_name)";
	else if(sdr->sym_type == "union")
		sym_type_str = " (union_name)";
	else if(sdr->sym_type == "enum")
		sym_type_str = " (enum_name)";
	else if(sdr->sym_type == "enum_mem")
		sym_type_str = " (enum_mem)";
	else if(sdr->sym_type == "typedef")
		sym_type_str = " (typedef)";
	else if(sdr->sym_type == "const")
		sym_type_str = " (constant)";
	else if(sdr->sym_type == "macro")
		sym_type_str = " (macro)";
	else if(sdr->sym_type != "")
		sym_type_str = "(" + sdr->sym_type + ")";

	return sdr->sym_name + sym_type_str;
	}
