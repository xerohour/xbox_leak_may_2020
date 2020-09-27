#if !defined(WIZARDS_SHELLEXT_IAXRO_H_INCLUDED_)
#define WIZARDS_SHELLEXT_IAXRO_H_INCLUDED_

//
//	Header file which defines the IDevStudioActiveXReferenceObject
//	interface which allows clients of the interface to determine the
//	sort of object which was found in the folder.
//
//	Descriptions:
//
//		GetTypeGuid()	- returns the guid of the type of object.
//						  returns GUID_DEVSTUDIO_ACTIVEX_REFOBJ_TYPE_CONTROL
//					      for ActiveX Controls,
//						  GUID_DEVSTUDIO_ACTIVEX_REFOBJ_TYPE_DEVSTUDIO_COMPONENT
//						  for DevStudio gallery components.
//
//		GetClsid()		- Returns the CLSID of the object
//
//		GetDisplayName()- Returns the name of the object, as desired
//						  by IShellFolder::GetDisplayNameOf()
//

interface IDevStudioActiveXReferenceObject : public IUnknown
{
	STDMETHOD(GetTypeGuid)(GUID &rguidType) PURE;
	STDMETHOD(GetClsid)(CLSID &rguidClsid) PURE;
	STDMETHOD(GetDisplayName)(DWORD uFlags, LPSTRRET lpName) PURE;
};

#endif
