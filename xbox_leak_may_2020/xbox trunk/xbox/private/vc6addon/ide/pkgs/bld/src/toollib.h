//
// Common Library Manager Tool
//
// [v-danwh],[matthewt]
//

#ifndef _INCLUDE_TOOLLIB_CMN_H
#define _INCLUDE_TOOLLIB_CMN_H

#include "schmztl.h"			// the CSchmoozeTool class
#include "toollink.h"			// our COMMON linker tool we derive from
#include "optnlib.h"			// our COMMON library manager tool options

///////////////////////////////////////////////////////////////////////////////
// CLibTool is derived from CLinkerTool. There are only minor differences
// such as different option handler, name of tool etc.
///////////////////////////////////////////////////////////////////////////////

class BLD_IFACE CLibTool : public CLinkerTool
{
	DECLARE_DYNAMIC (CLibTool)

	static const SchmoozeData m_LibSchmoozeData;
	virtual const SchmoozeData& GetMacs () const {return m_LibSchmoozeData;};

public:
	CLibTool();

	virtual BOOL IsConsumableFile ( const CPath *pPath );	

	// Filter function to pick files this item migth make:
	virtual BOOL IsProductFile 	  ( const CPath *pPath );

	// Filter function to pick files to delete on rebuild:
	virtual BOOL IsDelOnRebuildFile	  ( const CPath *pPath );

	virtual void GetGenericDescription(CString & strDescription);
};

#endif // _INCLUDE_TOOLLIB_CMN_H

