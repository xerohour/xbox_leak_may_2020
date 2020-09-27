#pragma once

#include <vccodemodels.h>

#include "VCDesignerObjectInternal.h"
#include <textmgr.h>
#include <vsshell.h>
#include <vcbudefs.h>
#include <bscext.h>
#include <ncparex.h>


// HRESULTs
//

#define S_CM_ZOMBIE					0x4CE00001

#define E_CM_NCB_UNAVAILABLE		0x8CE00001
#define E_CM_ZOMBIE					0x8CE00002
#define E_CM_GLOBAL_IINST			0x8CE00003
#define E_CM_UNRESOLVABLE_BASE		0x8CE00004
#define E_CM_UNDONE					0x8CE00005
#define E_CM_UNDONE_MCxx			0x8CE00006
#define E_CM_UNDONE_FORMS			0x8CE00007
#define E_CM_UNDONE_LAME			0x8CE00008
#define E_CM_UNDONE_POSTPONABLE		0x8CE00009
#define E_CM_POSTPONED				0x8CE0000A
#define E_CM_FILE_NOT_IN_PROJECT	0x8CE0000B
#define E_CM_INTRINSIC_NAME			0x8CE00011
#define E_CM_INVALID_IDENTIFIER		0x8CE00012
#define E_CM_READONLY				0x8CE00013
#define E_CM_NOT_READY				0x8CE00014


// VC Code Models Internal.h
//


// Things the users must never, never see...
//
// Therefore, it is OKAY to use inheritance.
//

enum vsCMPosition
{
	vsCMPositionInvalid = -1,
	vsCMPositionDeclaration = 0, // Declaration, including specifiers not including attribute block
	vsCMPositionDefinition = 1, // Definition, including specifiers not including attribute block
	vsCMPositionFullDefinition = 2, // Entire definition, from beginning of attributes to semi-colon.
	vsCMPositionAttributeBlock = 3,
	vsCMPositionDefinitionFullBody = 4, // From before open curly to afer end curly
	vsCMPositionDefinitionBody = 5, // From after open curly to before end curly
	vsCMPositionDeclarationName = 6,
	vsCMPositionDefinitionName = 7
};


// VCCMLocation
//

typedef DWORD VCCMLocationFlags;

const VCCMLocationFlags vccmLocationModule			(0x01);
const VCCMLocationFlags vccmLocationStartLine		(0x02);
const VCCMLocationFlags vccmLocationStartColumn		(0x04);
const VCCMLocationFlags vccmLocationEndLine			(0x08);
const VCCMLocationFlags vccmLocationEndColumn		(0x10);
const VCCMLocationFlags vccmLocationIVsTextLines	(0x20);
const VCCMLocationFlags vccmLocationStart			(vccmLocationModule | vccmLocationStartLine | vccmLocationStartColumn);
const VCCMLocationFlags vccmLocationEnd				(vccmLocationModule | vccmLocationEndLine | vccmLocationEndColumn);
const VCCMLocationFlags vccmLocationNCB				(vccmLocationModule | vccmLocationStartLine | vccmLocationEndLine);
const VCCMLocationFlags vccmLocationAll				(vccmLocationModule | vccmLocationStart | vccmLocationEnd | vccmLocationIVsTextLines);

enum vcCMRelativePosition
{
	vcCMRelativePositionUndetermined,
	vcCMRelativePositionOnly,
	vcCMRelativePositionOnlyNoBoundary,
	vcCMRelativePositionFirst,
	vcCMRelativePositionLast,
	vcCMRelativePositionInterior
};

class CVCCMLocation
{
public :

	VCCMLocationFlags m_vccmLocationFlags;
	vcCMRelativePosition m_RelativePosition;

	CComBSTR bstrModule;
	LONG dwStartLine;
	LONG dwStartColumn;
	LONG dwEndLine;
	LONG dwEndColumn;
	CComPtr<IVsTextLines> spIVsTextLines;

	CVCCMLocation(VCCMLocationFlags vccmLocationFlags = vccmLocationAll)
	: m_vccmLocationFlags(vccmLocationFlags), 
	  m_RelativePosition(vcCMRelativePositionUndetermined),
	  dwStartLine(-1), 
	  dwStartColumn(-1), 
	  dwEndLine(-1), 
	  dwEndColumn(-1)
	{
	}

	CVCCMLocation(const CVCCMLocation & rSource)
	: m_vccmLocationFlags(rSource.m_vccmLocationFlags), 
	  m_RelativePosition(rSource.m_RelativePosition),
	  bstrModule(rSource.bstrModule),
	  dwStartLine(rSource.dwStartLine), 
	  dwStartColumn(rSource.dwStartColumn), 
	  dwEndLine(rSource.dwEndLine), 
	  dwEndColumn(rSource.dwEndColumn),
	  spIVsTextLines(rSource.spIVsTextLines)
	{
	}

	CVCCMLocation & operator = (const CVCCMLocation & rSource)
	{
		m_vccmLocationFlags = rSource.m_vccmLocationFlags; 
		m_RelativePosition = rSource.m_RelativePosition;
		bstrModule = rSource.bstrModule;
		dwStartLine = rSource.dwStartLine; 
		dwStartColumn = rSource.dwStartColumn; 
		dwEndLine = rSource.dwEndLine; 
		dwEndColumn = rSource.dwEndColumn;
		spIVsTextLines = rSource.spIVsTextLines;

		return *this;
	}

	// This resets all the internal values
	void Reset(void)
	{
		bstrModule.Empty();
		spIVsTextLines.Release();
		dwStartColumn = -1;
		dwStartLine = -1;
		dwEndColumn = -1;
		dwEndLine = -1;
		m_RelativePosition = vcCMRelativePositionUndetermined;
	}

	bool ExactLocationRequested(void)
	{ 
		return 0 != (m_vccmLocationFlags & (vccmLocationStartColumn | vccmLocationEndColumn));
	}

	bool IVsTextLinesRequested(void)
	{
		return 0 != (m_vccmLocationFlags & vccmLocationIVsTextLines);
	}

	bool operator == (const CVCCMLocation & vccmlOther)
	{
		if (!bstrModule || !vccmlOther.bstrModule)
		{
			return false;
		}
		else if (_wcsicmp(bstrModule, vccmlOther.bstrModule))
		{
			return false;
		}
		else if (dwStartLine != vccmlOther.dwStartLine ||
			dwStartColumn != vccmlOther.dwStartColumn ||
			dwEndLine != vccmlOther.dwEndLine ||
			dwEndColumn != vccmlOther.dwEndColumn)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	bool Contains(const CVCCMLocation & vccmlOther)
	{
		// REVIEW : For now, "Contains" means "Contains the start of"
		//

		if (!bstrModule || !vccmlOther.bstrModule)
		{
			return false;
		} 
		if (_wcsicmp(bstrModule, vccmlOther.bstrModule))
		{
			return false;
		}
		if (-1 == dwStartLine || -1 == dwEndLine || 
			-1 == dwStartColumn || -1 == dwEndColumn || 
			-1 == vccmlOther.dwStartLine || -1 == vccmlOther.dwEndLine ||
			-1 == vccmlOther.dwStartColumn || -1 == vccmlOther.dwEndColumn)
		{
			return false;
		}
		
		if (vccmlOther.dwStartLine < dwStartLine || vccmlOther.dwStartLine > dwEndLine)
		{
			return false;
		}
		else if (vccmlOther.dwStartLine == dwStartLine)
		{
			return vccmlOther.dwStartColumn >= dwStartColumn;
		}
		else if (vccmlOther.dwStartLine == dwEndLine)
		{
			return vccmlOther.dwStartColumn <= dwEndColumn;
		}
		else
		{
			return true;
		}
	}
};

[
	uuid(29BF7879-FE8C-11d2-AAE9-00C04F72DB55),
]
class ATL_NO_VTABLE VCCodeElementI : public VCDesignElementI
{
public :

	virtual HRESULT STDMETHODCALLTYPE Remove(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE IsInSolution(BOOL * pbIsInSolution) =0;
	virtual HRESULT STDMETHODCALLTYPE get_PrimaryCollection(CodeElements ** ppCodeElements) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetLocation(const vsCMPart Part, const vsCMWhere Where, 
		CVCCMLocation &	vccmLocation) = 0;
	virtual HRESULT STDMETHODCALLTYPE NavigateTo(const vsCMWhere Where, const vsCMPart Part = vsCMPartName) = 0;
};


[
	uuid(BFEE2CBD-0725-4d2c-B6D9-DC57086DAA59),
]
class ATL_NO_VTABLE VCNcbElementI : public VCCodeElementI
{
public :

	virtual HRESULT STDMETHODCALLTYPE GetBSC(BscEx ** ppBscEx) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_usage(UsageType * pusage) = 0;
};


[
	uuid(C0D88760-94AC-463b-B3C9-45D657129B30),
]
class ATL_NO_VTABLE VCIinstElementI : public VCNcbElementI
{
public :

	virtual HRESULT STDMETHODCALLTYPE put_IINST(IINST iinst) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IINST(IINST * piinst) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UnnamedName(BSTR * pbstrUnnamedName) = 0;
};


[
	uuid(29BF787A-FE8C-11d2-AAE9-00C04F72DB55),
]
class ATL_NO_VTABLE VCCodeAttributeI : public VCIinstElementI
{
public :

	virtual HRESULT STDMETHODCALLTYPE get_IndexInGrammar(LONG * pnIndex) = 0;
};


[
	uuid(29BF789A-FE8C-11d2-AAE9-00C04F72DB55),
]
class ATL_NO_VTABLE VCFileCodeModelI : public VCIinstElementI
{
public :

	STDMETHOD(put_IMOD)(IMOD imod) = 0;
	STDMETHOD(get_IMOD)(IMOD * pimod) = 0;
};


[
	uuid(29BF787E-FE8C-11d2-AAE9-00C04F72DB55),
]
class ATL_NO_VTABLE VCCodeParameterI : public VCNcbElementI
{
public :

	virtual HRESULT STDMETHODCALLTYPE put_Index(LONG nIndex) = 0;
};


// IVCCodeElementCollectionDO_Internal
//

[
	uuid(29BF787F-FE8C-11d2-AAE9-00C04F72DB55),
]
class ATL_NO_VTABLE VCCodeElementsI : public VCDesignElementsI
{
public :

	virtual HRESULT STDMETHODCALLTYPE put_ElementKind(vsCMElement Kind) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ElementKind(vsCMElement * pKind) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetFilter(BSTR bstrFilter) = 0;
	virtual HRESULT STDMETHODCALLTYPE Sort(vsCMSort vscmSort) = 0;
};

[
	uuid(29BF789B-FE8C-11d2-AAE9-00C04F72DB55),
]
class ATL_NO_VTABLE VCIinstElementsI : public VCCodeElementsI
{
public :

	virtual HRESULT STDMETHODCALLTYPE LinkOrCreate(IINST iinst) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddCreate(IINST iinst) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddLink(IINST iinst, LPDISPATCH pDisp) = 0;
};

[
	uuid(2857E25D-05A0-4db2-8571-D759DEB5B815),
]
class ATL_NO_VTABLE VCFileElementsI : public VCIinstElementsI
{
public :

	virtual HRESULT STDMETHODCALLTYPE put_IMOD(IMOD imod) = 0;
};

// The VC Code Element Service.
//

[
	uuid(29BF7880-FE8C-11d2-AAE9-00C04F72DB55),
]
class ATL_NO_VTABLE VCCodeModelService : public IUnknown
{
public :

	// Process UpdateItems & add events to the event queue
	//

	virtual HRESULT STDMETHODCALLTYPE FileUpdate(UpdateItem * pUpdates, ULONG cUpdates) = 0;
	
	// Flushes the event queue
	//

	virtual HRESULT STDMETHODCALLTYPE FireEvents(void) = 0;

	// Given an IINST, gives back the CodeElement that represents it.
	//

	virtual HRESULT STDMETHODCALLTYPE LookupIINST(IINST iinst, IDispatch ** ppIDispatch) = 0;

	// Given an IINST, gives back the CodeElement that represents it.
	//

	virtual HRESULT STDMETHODCALLTYPE IsReady(BOOL * pbReady) = 0;

	// Create a blank VC Code Model Object
	//

	virtual HRESULT STDMETHODCALLTYPE CreateCodeModel(VCCodeModel ** ppCodeModel) = 0;

	// Create a blank VC Full Code Model Object
	//

	virtual HRESULT STDMETHODCALLTYPE CreateFullCodeModel(VCCodeModel ** ppFullCodeModel) = 0;

	// Create a blank VC File Code Model Object
	//

	virtual HRESULT STDMETHODCALLTYPE CreateFileCodeModel(BSTR bstrFileName, VCFileCodeModel ** ppVCFileCodeModel) = 0;

	virtual HRESULT STDMETHODCALLTYPE OnBufferChanged(IVsTextLines * pTextLines) = 0;
};

#define SID_SVCCodeModelService __uuidof(VCCodeModelService)

[
	uuid(29BF7882-FE8C-11d2-AAE9-00C04F72DB55)
]
class ATL_NO_VTABLE VCCodeTypeI : public VCIinstElementI
{
public :
	virtual HRESULT STDMETHODCALLTYPE AddMemberFunction
	(
		vsCMAccess Access, LPCWSTR wstrName, LPCWSTR bstrProtoHeader, LPCWSTR bstrProto,
		LPCWSTR bstrFunctionBody, LPCWSTR bstrImplFileName, vsCMAddPosition AddPosition, IDispatch** ppDispatch
	) = 0;

	virtual HRESULT STDMETHODCALLTYPE ValidateMember
	(
		BSTR bstrName, vsCMElement AddedKind, BSTR bstrType, VARIANT_BOOL* pbValid
	) = 0;

	virtual HRESULT STDMETHODCALLTYPE ReplaceChildMethodNames
	(
		UINT level,
		BSTR bstrOldName,
		BSTR bstrName
	) = 0;
};


[
	uuid(29BF7883-FE8C-11d2-AAE9-00C04F72DB55)
]
class ATL_NO_VTABLE VCCodeTypeRefI : public VCDesignElementI
{
public :

	virtual HRESULT STDMETHODCALLTYPE put_CodeModel(VCCodeModel * pVCCodeModel) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_TypeString(BSTR bstrType) = 0;
};

[
	uuid(29BF787D-FE8C-11d2-AAE9-00C04F72DB55),
]
class ATL_NO_VTABLE VCCodeModelI : public VCIinstElementI
{
public :

	// UNDONE : Remove Me, perhaps
};
