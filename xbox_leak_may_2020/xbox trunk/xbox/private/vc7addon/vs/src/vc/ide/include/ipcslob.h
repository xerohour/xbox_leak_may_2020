/////////////////////////////////////////////////////////////////////////////
//	IPCSLOB.H
//		Defines classes which inherits from CSlob and also provides an
//		ITrackSelection interface to manage an ISelectionContainer object
//		This class works in conjunction with COlePropertyPageSite to allow
//		components that support IPropertyPage, ISelectionContainer to be 
//		used within the package framework

#ifndef __IPCSLOB_H__
#define __IPCSLOB_H__
#include "slob.h"
#include "objext.h"
#include "ipcits.h"
#include "oleref.h"

//#undef AFX_DATA
//#define AFX_DATA AFX_EXT_DATA

class CIPCompSlob;
class COleTrackSelection;
class CIPCompDoc;
interface IPropertyPageUndoString;
interface IInternalTrackSelection;

// This GUID is defined in fm20uuid.lib, which we don't want to use because there
// do not exist RISC builds and the IDE needs to be built for some RISC platforms.
// Also see guids.cpp
DEFINE_GUID(IID_IElement,	0x3050f1ff, 0x98b5, 0x11cf, 0xbb, 0x82, 0x00, 0xaa, 0x00, 0xbd, 0xce, 0x0b);

/////////////////////////////////////////////////////////////////////////////
//	COleTrackSelection

class COleTrackSelection : public CCmdTarget
{
public:
	DECLARE_DYNAMIC(COleTrackSelection)

public:
	COleTrackSelection(CIPCompSlob *pIpCompSlob);
	virtual ~COleTrackSelection();
	void Zombie() { m_pIpCompSlob = NULL; };

// Attributes
protected:
	// called when ITrackSelection::OnSelectChange is called
	HRESULT OnSelectionContainerChange(ISelectionContainer *pSelectionCntr);

	CIPCompSlob *m_pIpCompSlob;

// Interface Maps   
protected:
	// ITrackSelection
	BEGIN_INTERFACE_PART(TrackSelection, ITrackSelection)
		INIT_INTERFACE_PART(COleTrackSelection, TrackSelection)
		STDMETHOD(OnSelectChange)(ISelectionContainer *pSelectionCntr);
	END_INTERFACE_PART(TrackSelection)

	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	CInternalTrackSelection

class CInternalTrackSelection : public CCmdTarget
{
public:
	DECLARE_DYNAMIC(CInternalTrackSelection)

public:
	CInternalTrackSelection(CIPCompSlob* pIpCompSlob);
	virtual ~CInternalTrackSelection();
	void Zombie() { m_pIpCompSlob = NULL; };

// Attributes
protected:
	HRESULT	IsExternalSelectChange(
				VARIANT_BOOL* pbReturn);
	HRESULT	OnInternalSelectChange(
				DWORD dwCookie);
	HRESULT	GetInternalSelectList(
				CALPOLESTR* pcaStringsOut,
				CADWORD* pcaCookiesOut);

	CIPCompSlob *m_pIpCompSlob;

// Interface Maps   
protected:
	// ITrackSelection
	BEGIN_INTERFACE_PART(InternalTrackSelection, IInternalTrackSelection)
		INIT_INTERFACE_PART(CInternalTrackSelection, InternalTrackSelection)
		STDMETHOD(IsExternalSelectChange)(VARIANT_BOOL* pbReturn);
		STDMETHOD(OnInternalSelectChange)(DWORD dwCookie);
		STDMETHOD(GetInternalSelectList)(CALPOLESTR* pcaStringsOut, CADWORD* pcaCookiesOut);
	END_INTERFACE_PART(InternalTrackSelection)

	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	CIPCompSlob
//
class CIPCompSlob : public CSlob
{
public:
	DECLARE_DYNAMIC(CIPCompSlob)

	CIPCompSlob(CIPCompDoc *pDoc);
	virtual ~CIPCompSlob();

public:
	virtual BOOL Create();	// 2nd phase construction
	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);
//	C3dPropertyPage* GetPropPage(UINT nPage);

	// called when ITrackSelection::OnSelectChange is called
	virtual HRESULT OnSelectionContainerChange(ISelectionContainer *pSelectionCntr);
	IUnknown *GetTrackSelection();

	virtual void BeginUndo(UINT nID);
	virtual BOOL SetUndoStringMgr(IPropertyPageUndoString *pPPUS);
	virtual void EndUndo(BOOL bAbort = FALSE);

	HRESULT	GetInternalTrackSelection(IInternalTrackSelection** ppITS);
	HRESULT	GetInternalSelectList(CALPOLESTR* pcaStringsOut, CADWORD* pcaCookiesOut);

	void	SetSelectChangeFlag(BOOL bFlag)
			{
				m_bSelectChange = bFlag;
			}
	BOOL	GetSelectChangeFlag()
			{
				return m_bSelectChange;
			}
	BOOL	ShowLevelCombo()
			{
				return m_bShowLevelCombo;
			}

	void	OnInternalSelectChange(IUnknown* pUnkInternalSelectChange);

// Attributes
protected:
	COleTrackSelection				 *m_pOleTrackSelection;
	COleRef<ITrackSelection>		  m_srpTrackSelection;
	COleRef<ISelectionContainer>	  m_srpSelectionCntr;
	CInternalTrackSelection*			m_pInternalTrackSelection;
	COleRef<IInternalTrackSelection>	m_srpInternalTrackSelection;

	// NOTE:
	// the following interface pointer is not addref'ed, so do not
	// release it at any time, this interface pointer is released
	// by the owner (CSheetWnd) when appropriate.
	// Also, that is the reason, this is not a smart ref
	IUnknown*	m_pUnkInternalSelectChange;	// for trident
	
	void	ReleaseCachedObjects();
	BOOL	IntersectElements(IUnknown* pUnk);
	BOOL	GetElementsIntoArray(IUnknown* pUnk, CPtrArray* pElementsArray);

private:
	CIPCompDoc*	m_pDoc;
	IUnknown*	m_pSingleObject;
	IUnknown**	m_rgpCachedObjects;
	ULONG		m_nCachedObjectCnt;
	BOOL		m_bSelectChange;		// true for external select change and
										// false for internal select change
	CPtrArray	m_ElementsArray;
	BOOL		m_bShowLevelCombo;
};

#endif	// __IPCSLOB_H__	

