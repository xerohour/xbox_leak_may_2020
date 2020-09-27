// AutoFile.h
// declares what we implement for the DTE ProjectItem

#include "PrjNode.h"
#include "AutoBase.h"
#include "VCFile.h"

class CAutoProject; // FORWARD...

// classes in this header
class CAutoFile;
class CAutoItems;

// string version of the guid
_declspec(selectany) LPOLESTR szguidCATIDAutoFile = L"{610d461f-d0d5-11d2-8599-006097c68e81}";

class CAutoFile : 
	public CAutoBase<CAutoFile, VCFile, szguidCATIDAutoFile>,
	public CComDualImpl<_ProjectItem, &IID__ProjectItem, &LIBID_DTE, 7, 0>, 
	public ISupportVSProperties,
	public CComObjectRoot
{
public:
	CAutoFile();
	~CAutoFile();
	void Initialize(IDispatch* ppAutoFiles, IDispatch* pAutoProject, IDispatch* pItem);
	static HRESULT CreateInstance(CAutoFile** ppAutoFile, IDispatch* pAutoItems, IDispatch* pAutoProject,
		IDispatch* pItem);
	VCFile *GetFile() { return(m_spFile); }

BEGIN_COM_MAP(CAutoFile)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(_ProjectItem)
	COM_INTERFACE_ENTRY(ISupportVSProperties)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CAutoItems) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

// ProjectItem
public:
    /* _ProjectItem methods */
    STDMETHOD(get_IsDirty)(VARIANT_BOOL FAR* lpfReturn);
    STDMETHOD(put_IsDirty)(VARIANT_BOOL DirtyFlag);
    STDMETHOD(get_FileNames)(short Index, BSTR FAR* lpbstrReturn);
    STDMETHOD(SaveAs)(BSTR NewFileName, VARIANT_BOOL FAR* lpfReturn);
    STDMETHOD(get_FileCount)(short FAR* lpsReturn);
    STDMETHOD(get_Reserved1)(long FAR* lpulReturn) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved1)(long Value) { return E_NOTIMPL; }
    STDMETHOD(Reserved2)(BSTR FileName) { return E_NOTIMPL; }
    STDMETHOD(Reserved3)() { return E_NOTIMPL; }
    STDMETHOD(get_Name)(BSTR FAR* pbstrReturn);
    STDMETHOD(put_Name)(BSTR bstrName);
    STDMETHOD(get_Reserved14)(IDispatch * FAR* ppDispatch) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved4)(IDispatch * FAR* ppVbaModule) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved5)(long FAR* pkind) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved6)(IDispatch * FAR* lppaReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Collection)(ProjectItems FAR* FAR* lppcReturn);
    STDMETHOD(get_Reserved15)(VARIANT_BOOL FAR* lpfReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Properties)(Properties FAR* FAR* ppObject);
    STDMETHOD(Reserved7)(IDispatch * FAR* lppcReturn) { return E_NOTIMPL; }
    STDMETHOD(Reserved8)(BSTR Key, BSTR FAR* lpbstrValue) { return E_NOTIMPL; }
    STDMETHOD(Reserved9)(BSTR Key, BSTR Value) { return E_NOTIMPL; }
    STDMETHOD(Reserved10)() { return E_NOTIMPL; }
    STDMETHOD(get_Reserved11)(long FAR* lpdwContextID) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved11)(long dwContextID) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved12)(BSTR FAR* lpbstrDescription) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved12)(BSTR bstrDescription) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved13)(BSTR FAR* pbstrReturn) { return E_NOTIMPL; }
    STDMETHOD(get_DTE)(DTE FAR* FAR* lppaReturn);
    STDMETHOD(get_Kind)(BSTR FAR* lpbstrFileName);
    STDMETHOD(get_ProjectItems)(ProjectItems FAR* FAR* lppcReturn);
    STDMETHOD(get_IsOpen)(BSTR LogView, VARIANT_BOOL FAR* lpfReturn);
    STDMETHOD(Open)(BSTR LogView, Window FAR* FAR* lppfReturn);
    STDMETHOD(Remove)();
    STDMETHOD(Delete)();
    STDMETHOD(ExpandView)();
    STDMETHOD(get_Object)(IDispatch **ppProjectItemModel);
    STDMETHOD(get_Extender)(BSTR bstrExtenderName, IDispatch **ppExtender);
    STDMETHOD(get_ExtenderNames)(VARIANT *pvarExtenderNames);
    STDMETHOD(get_ExtenderCATID)(BSTR *pbstrRetval);

    STDMETHOD(get_Saved)(VARIANT_BOOL *lpfReturn);
    STDMETHOD(put_Saved)(VARIANT_BOOL Dirty);
    STDMETHOD(get_SourceControl)(SourceControl ** /*ppSourceControl*/);
    STDMETHOD(get_ConfigurationManager)(ConfigurationManager ** /*ppConfigurationManager*/);
    STDMETHOD(get_CodeModel)(CodeModel ** /*ppCodeModel*/);
    STDMETHOD(Save)(BSTR /*FileName*/);
    STDMETHOD(get_Document)(Document ** /*ppDocument*/);
    STDMETHOD(get_SubProject)(Project ** /*ppProject*/);
    STDMETHOD(get_ContainingProject)(Project ** /*ppProject*/);
    STDMETHOD(get_FileCodeModel)(FileCodeModel ** ppFileCodeModel);
    STDMETHOD(AddFolder)(BSTR Name, BSTR Kind, ProjectItem **pProjectItem);

protected:
	CComPtr<IDispatch> m_spAutoItems;
	CComPtr<IDispatch> m_spAutoProject;
	CComQIPtr<VCFile> m_spFile;

	CVCProjectNode* GetProjectNode(CVCFileNode* pFileNode = NULL);
	CVCFileNode* GetFileNode();
	HRESULT IsOpen(BSTR LogView, BOOL bAlwaysProvideLogView, VARIANT_BOOL* lpfReturn, IVsWindowFrame** ppWindowFrame);

// ISupportVSProperties
public:
	STDMETHOD(NotifyPropertiesDelete)()
	{
		return E_NOTIMPL;	// CAutoFile::NotifyPropertiesDelete
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class CAutoItems

class CAutoItems : 
	public CComDualImpl<_ProjectItems, &IID__ProjectItems, &LIBID_DTE, 7, 0>, 
	public ISupportErrorInfo,
	public CComObjectRoot
{
public:
	CAutoItems();
	~CAutoItems();
	void Initialize(IDispatch* pAutoParent, IDispatch *pAutoProj, IDispatch* pDispItems);
	static HRESULT CreateInstance(CAutoItems **ppEnum, IDispatch* pAutoParent, IDispatch *pAutoProj, 
		IDispatch* pDispItems);

BEGIN_COM_MAP(CAutoItems)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(_ProjectItems)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CAutoItems) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ProjectItems
public:
    /* _ProjectItems methods */
    STDMETHOD(Item)(VARIANT index, ProjectItem FAR* FAR* lppcReturn);
    STDMETHOD(get_Parent)(IDispatch * FAR* lppptReturn);
    STDMETHOD(get_Count)(long FAR* lplReturn);
    STDMETHOD(_NewEnum)(IUnknown * FAR* lppiuReturn);
    STDMETHOD(Reserved1)(IDispatch * VBComponent) { return E_NOTIMPL; }
    STDMETHOD(Reserved2)(long ComponentType, IDispatch * FAR* lppComponent) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved3)(IDispatch * FAR* lppaReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved4)(VARIANT FAR* lpVar) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved4)(VARIANT varInput) { return E_NOTIMPL; }
    STDMETHOD(Reserved5)(BSTR FileName, VARIANT_BOOL RelatedDocuement, IDispatch * FAR* lppComponent) { return E_NOTIMPL; }
    STDMETHOD(Reserved6)(BSTR FileName, IDispatch * FAR* lppComponent) { return E_NOTIMPL; }
    STDMETHOD(Reserved7)(BSTR ProgID, IDispatch * FAR* lppComponent) { return E_NOTIMPL; }
    STDMETHOD(get_DTE)(DTE FAR* FAR* lppaReturn);
    STDMETHOD(get_Kind)(BSTR FAR* lpbstrFileName);
    STDMETHOD(AddFromFile)(BSTR FileName, ProjectItem FAR* FAR* lppcReturn);
    STDMETHOD(AddFromTemplate)(BSTR FileName, BSTR Name, ProjectItem FAR* FAR* lppcReturn);
    STDMETHOD(AddFromDirectory)(BSTR Directory, ProjectItem FAR* FAR* lppcReturn);
    STDMETHOD(get_ContainingProject)(Project ** /*ppProject*/);
    STDMETHOD(AddFolder)(BSTR Name, BSTR Kind, ProjectItem **pProjectItem);
    STDMETHOD(AddFromFileCopy)(BSTR FilePath, ProjectItem** ppOut);

// helpers
public:
	static HRESULT GetItem(IDispatch** ppProjItem, IDispatch* pOwner, IDispatch* pAutoParent, IDispatch* pAutoProject, IDispatch* pVCItem);

protected:
	HRESULT GetBaseObject(IDispatch** ppObject);
	CVCBaseFolderNode* GetBaseNode();
	HRESULT SetupForAddItemDlg(CVCBaseFolderNode** ppBaseNode, HWND* phwndDlg, ProjectItem** ppOut);
	HRESULT FinishFromAddItemDlg(HRESULT hrReturn, VSADDRESULT addResult, BSTR bstrPath, ProjectItem** ppItem);

protected:
	CComPtr<IDispatch> m_spAutoParent;
	CComPtr<IDispatch> m_spAutoProject;
	CComQIPtr<IVCCollection> m_spItems;
};
