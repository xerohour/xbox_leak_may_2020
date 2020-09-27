///////////////////////////////////////////////////////////////////////////////
//  COBROWSE.H
//
//  Created by :            Date :
//      WayneBr                 1/21/94
//
//  Description :
//      Declaration of the COBrowse class
//

#ifndef __COBROWSE_H__
#define __COBROWSE_H__

#include "uibrowse.h"

#include "dbgxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  COBrowse class

// Special values for ambiguity index. Normal values for index are >= 0.
int const NoAmbiguity = -1;


typedef enum {LOC_DEF, LOC_REF} BROWSER_LOC_TYPE;
typedef enum {SYM_VAR, SYM_PARAM} BROWSER_SYM_TYPE;

// abbreviations to make casting shorter.
#define SDR SYM_DEFS_REFS
#define BL BROWSER_LOCATION
#define CI COMMENT_INFO

class COMMENT_INFO
	{
	public:
		CString file;
		CArray<int, int> lines;
	};

class BROWSER_LOCATION
	{
	public:
		// contructor
		BROWSER_LOCATION(){commented = FALSE;}
		// data
		CString name;
		CString file;
		int	line;
		BOOL commented;
		CPtrArray comments;
		CArray<int, int> dependents;
	};

class SYM_DEFS_REFS
	{
	public:
		// constructor
		SYM_DEFS_REFS(){commented = FALSE;}
		// data
		CString sym_name;
		CString sym_type;
		BOOL commented;
		// [0] is defs and [1] is refs
		CPtrArray locs[2];
	};


// BEGIN_CLASS_HELP
// ClassName: COBrowse
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COBrowse
{
public:

	// Navigation operations
	BOOL GotoNthDefinition(int index, LPCSTR name,int ambiguity = NoAmbiguity ); // Global context, NULL name use cursor loc
	BOOL GotoNthReference(int index, LPCSTR name,int ambiguity = NoAmbiguity );  // Global context, NULL name use cursor loc
	BOOL NextDefinition(void);
	BOOL PrevDefinition(void);
	BOOL NextReference(void);
	BOOL PrevReference(void);
	BOOL PopContext(void);

	// Batch query operations.
	BOOL GetMatches( CStringArray& matches, CString name, QueryType type, int filter = NONE );
	BOOL GetDefinitions( CStringArray& defs, CString name, QueryType type, int filter = NONE, int ambiguity = NoAmbiguity );
	BOOL GetReferences( CStringArray& refs, CString name, QueryType type, int filter = NONE, int ambiguity = NoAmbiguity );
	BOOL GetMembers( CStringArray& members, CString name, QueryType type, int filter = NONE, int ambiguity = NoAmbiguity );

	BOOL VerifyMultDefRefMatches(CPtrArray& expected, LPCSTR query_name = "");

	// get first level of descendents or ancestors in tree.
	BOOL GetFirstLevelBranches( CStringArray& branches, CString name, QueryType type, int filter = NONE, int ambiguity = NoAmbiguity  );

	// Utilities
	BOOL VerifyNavigation(LPCSTR file_line);
	BOOL GetLocations(CStringArray& defs, CStringArray& refs);
	CString BuildSymbolMatchStr(SDR *sdr);
	
private:
	UIBrowse m_uibrowse;
};

#endif // __CBROWSE_H__
