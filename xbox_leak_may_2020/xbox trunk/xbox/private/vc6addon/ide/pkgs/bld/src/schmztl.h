//
//	SCHMZTL.H
//
//		Definition of classes for tracking buidld tools.

#ifndef _SCHMZTL_H_
#define _SCHMZTL_H_

#ifndef _SUSHI_PROJECT
// If you are encluding this in a file that is being compiled under
// WIN16, either make sure everything compiles under WIN16 and remove
// this error, or include conditional on _SUSHI_PROJECT.
//
#error This code may not be Win 3.1 compatible.
#endif

#include <utilbld_.h>
#include "projitem.h"
#include "prjoptn.h"
#ifndef _PROJTOOL_H_
#include "projtool.h"
#endif

class BLD_IFACE CSchmoozeTool : public CBuildTool
{
	DECLARE_DYNAMIC (CSchmoozeTool)

public:
	CSchmoozeTool();
	~CSchmoozeTool();

	struct SchmoozeData
	{
		const TCHAR *pszExeMacro;
		const TCHAR *pszFlagsMacro;
		const TCHAR *pszSpecialFlags;
		const TCHAR *pszConsumeMacro;
		const TCHAR *pszSpecialConsume;
		const TCHAR *pszExeMacroEx;
		const TCHAR *pszFlagsMacroEx;
		const TCHAR *pszSpecialFlagsEx;
		const TCHAR *pszConsumeMacroEx;
		const TCHAR *pszSpecialConsumeEx;
		const TCHAR *pszProductExtensions;
		const TCHAR *pszDelOnRebuildExtensions;
	};

	virtual const SchmoozeData& GetMacs () const
		{ASSERT(FALSE); return *( (SchmoozeData*) NULL);}

	virtual void GetGenericDescription(CString & strDescription) {strDescription = "<Undefined>";}

	// Get the actual defered action command lines:
	virtual void GetDeferedScmoozeLines
	(
		CProject * pProject,
		CStringList & slCommandLines, 
		CStringList & slDescriptions,
		CErrorContext & EC = g_DummyEC
	)
		{ASSERT (FALSE);}

	// Filter function for above:
	virtual BOOL IsConsumableFile(const CPath * pPath);	

	virtual BOOL IsSpecialConsumable(const CPath * pPath) {return FALSE;}

	// Filter function to pick files this item might make:
	virtual BOOL IsProductFile(const CPath * pPath);

	// Filter function to pick files to delete on rebuild:
	virtual BOOL IsDelOnRebuildFile(const CPath * pPath);

	// Makefile writing functions from CBuildTool:
	virtual int  BeginWritingSession
	( 
		CProjItem * pItem,
		CActionSlob * pAction,
		CMakFileWriter * ppw,
		CMakFileWriter * pmw,
		const CDir * pBaseDir,
		CNameMunger * pnm
	); 

	virtual BOOL WritePerConfigData (CProjItem * pItem, CActionSlob * pAction);
	virtual BOOL WriteBuildRule 	(CActionSlob * pAction);
	virtual BOOL WriteHeaderSection (CActionSlob * pAction);

	virtual BOOL DoWriteBuildRule 	(CActionSlob * pAction);

	// UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE
	// These methods exist to replace 'IsKindOf(Tool_Class)' functionality
	// they should be removed with a more extensive re-work of the build system.
	// [matthewt]
	virtual BOOL StoreOptionsOnTarget() {return TRUE;}
	// UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE
};

#endif // _SCHMZTL_H_
