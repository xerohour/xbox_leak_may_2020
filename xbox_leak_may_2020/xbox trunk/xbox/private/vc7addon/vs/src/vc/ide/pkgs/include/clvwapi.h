/////////////////////////////////////////////////////////////////////////////
//	CLVWAPI.H
//		ClassView package interface declarations.

#ifndef __CLVWAPI_H__
#define __CLVWAPI_H__

#include <atlbase.h>
#include "ncparex.h"  // for IINST mostly

class IWorkspaceDO_Internal;	// defined in do.idl

__interface IObjectGallery;		// #include <galapi.h> if actually need to work with this
__interface IClassView;
__interface IClassProvider;
__interface IClassProviderFactory;

// char flags
#define CHAR_UNKNOWN			0x0001
#define	CHAR_COMMA				0x0002
#define CHAR_DOT				0x0004
#define CHAR_ARROW				0x0008
#define	CHAR_COLONCOLON			0x0010
#define	CHAR_OPENPAREN			0x0020
#define CHAR_CLOSEPAREN			0x0040
#define	CHAR_CRLF				0x0080
#define CHAR_CARETMOVE			0x0100
#define	CHAR_DELETED			0x0200
#define CHAR_LEFTANGLE			0x0400
#define CHAR_RIGHTANGLE			0x0800
#define CHAR_TYPE_INFO			0x1000
#define CHAR_ATTRIBUTE			0x2000

// command flags
#define CMD_NONE				0x0000
#define	CMD_TYPE_INFO			0x1000
#define CMD_PARAMETER_INFO		0x2000
#define CMD_COMPLETE_WORD		0x4000
#define CMD_AUTO_COMPLETE		0x8000
#define CMD_TYPE_INFO_DATATIP   0x0100  // VC7 treats hover as a data tip
#define CMD_PARAMETER_INFO_REINVOKE 0x0200  // for nested function calls

// comment location flags
#define COMMENTS_NONE           0x0000
#define COMMENTS_INDECLARATION  0x0001
#define COMMENTS_INDEFINITION   0x0002

typedef IClassView* LPCLASSVIEW;

struct LINE_COLUMN_POS {
	ULONG  m_iLine;
	ULONG  m_iIndex;
};

struct STATE_INFO {
	CComBSTR			m_bstrClass;
	CComBSTR			m_bstrCurClass;
	CComBSTR			m_bstrIdentifier;
	CComBSTR			m_bstrFileName;
	CComBSTR			m_bstrAttrArgument;
	CComBSTR			m_bstrAttrArgVal;
	ULONG				m_flagsCur;
	ULONG				m_flagsCmd;
	ULONG				m_iStreamIndexAtCursor; // document location
	ULONG				m_iCaretIndex; // This is within the buffer
	ULONG				m_iFECaretIndex; // This is within the buffer
	ULONG				m_iAnchorIndex;	// This is within the buffer
	ULONG				m_iAnchorPosition;
	ULONG				m_iFuncStart;
	LINE_COLUMN_POS 	m_Anchor; 
	LINE_COLUMN_POS		m_StartOfIdentifier;
	LINE_COLUMN_POS		m_StartOfParameters;
	LINE_COLUMN_POS		m_CurrentCursor;
	IINST				m_iinstCurClass;
	int					m_iCurParameter;
	ULONG				m_cbCurEditBuffer;
	ULONG				m_cbMaxEditBuffer;	
	BOOL				m_fInUse;
	BOOL				m_fForced;
	BOOL				m_fRecheck;
	WCHAR				*m_pwchUnicodeEditBuffer;	// WIDE version of m_pchEditBuffer
	BOOL				m_fComPlus;
	BOOL				m_fPassInlineClassParentIinst;
};


/////////////////////////////////////////////////////////////////////////////
// IClassView
class CVCCMLocation;
#undef  INTERFACE
#define INTERFACE IClassView
DECLARE_INTERFACE_(IClassView, IUnknown)
{
	// IClassView methods

	STDMETHOD(GetCurrentSelInfo)(char **psz, BYTE *ptyp, USHORT *patr) PURE;
	STDMETHOD(GetCurrentConfiguration)(BSTR* pbstrConfig) PURE;
	STDMETHOD(RegisterProviderFactory)(IClassProviderFactory* pClassProviderFactory) PURE;

	// AutoComplete methods
	STDMETHOD(IdleInitializeAutoCompletionManager)(INT count) PURE;
	STDMETHOD(DoAutoComplete) (ULONG cmdFlags, ULONG charFlags, LPCTSTR szIdentifier, 
		ULONG ichInsert, ULONG ilineInsert, ULONG ichRealInsert, ULONG ilineRealInsert, BOOL fJingle) PURE;
	STDMETHOD(OnChangeInTextEditor) (ULONG charFlags, LPCTSTR pszString, ULONG ichInsert, ULONG ilineInsert) PURE;
	STDMETHOD(CanShowQuickInfo) (BOOL *pfShow) PURE;
	STDMETHOD(RetrieveACInfo) (int *iBufferIndex, STATE_INFO **ppsi, BscEx** ppBscEx) PURE;
	STDMETHOD(OnTextViewDestroy) () PURE;
	STDMETHOD(GetCommentForIinst) (IINST iinst, BSTR *pbstrComment) PURE;
	STDMETHOD(GetCommentLocationForIinst)(IINST iinst, INT* pnCommentLoc, 
		CVCCMLocation *pvccmLocation, BOOL* pbEolComment) PURE;
	STDMETHOD(UpdateCurrentLocationInfo)(LPVOID pVoid, LPCTSTR szFileName, LONG iline, ULONG ich, BOOL fSyncClassView) PURE;

};

/////////////////////////////////////////////////////////////////////////////
// IClassProviderFactory

#undef  INTERFACE
#define INTERFACE IClassProviderFactory
DECLARE_INTERFACE_(IClassProviderFactory, IUnknown)
{
	STDMETHOD(GetClassProvider)(THIS_ VOID *pvBsc, IINST iinst,
		IClassProvider** ppClassProvider) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IClassProvider

#undef  INTERFACE
#define INTERFACE IClassProvider
DECLARE_INTERFACE_(IClassProvider, IUnknown)
{
	STDMETHOD(GetInterfaceInfo)(THIS_ IINST iisnt, UINT iInterface,
		LPCTSTR* pszInterfaceName) PURE;
	STDMETHOD(GetMemberInfo)(THIS_ IINST iinst, LPCTSTR* pszInterfaceName) PURE;
	STDMETHOD(CreateMember)(THIS_ LPCTSTR szMember, UINT ichClassName, UINT nAccess,
		LPCTSTR szInterfaceName, IObjectGallery* pObjectGallery) PURE;
	STDMETHOD(UpdateClass)() PURE;
};


#endif	// __CLVWAPI_H__
