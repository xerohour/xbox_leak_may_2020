//
// Common Library Manager Tool
//
// [v-danwh],[matthewt]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "toollib.h"	// our local header file

IMPLEMENT_DYNAMIC(CLibTool, CLinkerTool)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////////
//
// --------------------------------- CLibTool ---------------------------------
//
///////////////////////////////////////////////////////////////////////////////
const CSchmoozeTool::SchmoozeData CLibTool::m_LibSchmoozeData =
{
		    _TEXT ("LIB32"),		 		//pszExeMacro;
		    _TEXT ("LIB32_FLAGS"),  		//pszFlagsMacro;
			_TEXT ("DEF_FLAGS"),			//pszSpecialFlags;
			_TEXT ("LIB32_OBJS"),			//pszConsumeMacro;
			_TEXT ("DEF_FILE"),				//pszSpecialConsume;
		    _TEXT (" $(LIB32)"),			//pszExeMacroEx;
		    _TEXT (" $(LIB32_FLAGS)"),   	//pszFlagsMacroEx;
			_TEXT (" $(DEF_FLAGS)"),		//pszSpecialFlagsEx;
		    _TEXT (" $(LIB32_OBJS)"),  		//pszConsumeMacroEx;
			_TEXT (" $(DEF_FILE)"),			//pszSpecialConsumeEx;
		    _TEXT ("lib;map"),				//pszProductExtensions;
		    _TEXT ("lib;map"),				//pszDelOnRebuildExtensions;
 };

///////////////////////////////////////////////////////////////////////////////
CLibTool::CLibTool() : CLinkerTool()
{
	// tool name
	m_nIDName = IDS_LIBMGR_TOOL;
	m_nIDUIName = IDS_LIBMGR_TOOL_UI;

	// tool exe name and input file set
	m_strToolExeName = _TEXT("link.exe -lib");
	m_strToolInput = _TEXT("*.obj;*.res;*.lib");
	m_strToolPrefix =  _TEXT ("LIB32");
}
///////////////////////////////////////////////////////////////////////////////
BOOL CLibTool::IsProductFile ( const CPath *pPath )
{
	return FileNameMatchesExtension(pPath, m_LibSchmoozeData.pszProductExtensions);
}

///////////////////////////////////////////////////////////////////////////////
BOOL CLibTool::IsDelOnRebuildFile ( const CPath *pPath )
{
	return FileNameMatchesExtension(pPath, m_LibSchmoozeData.pszDelOnRebuildExtensions);
}

///////////////////////////////////////////////////////////////////////////////
BOOL CLibTool::IsConsumableFile ( const CPath *pPath )
{
	return FileNameMatchesExtension(pPath, m_strToolInput);
}

///////////////////////////////////////////////////////////////////////////////
void CLibTool::GetGenericDescription(CString & strDescription)
{
	strDescription.LoadString(IDS_DESC_LIBING);
}
