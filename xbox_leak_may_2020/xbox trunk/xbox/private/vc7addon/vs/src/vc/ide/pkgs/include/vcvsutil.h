#pragma once

// VCUGetHierarchyAndItemFromFileName will do just what the name implies.
HRESULT VCUGetHierarchyAndItemFromFileName(BSTR bstrFileName, IVsHierarchy ** ppIVsHierarchy, VSITEMID * pItemID);

// VCUOpenFile will open the file
HRESULT VCUOpenFile(BSTR bstrFileName, bool bShow, IVsTextView** ppView);

// VCOpenTextLines will create just the buffer, but not open the file
HRESULT VCUOpenTextLines(BSTR bstrFileName, IVsTextLines** ppTextLines);

// Default to VC Package
__declspec(selectany) extern const char g_ptszVCPackageUISatelliteDLL[] = "vcpkgui.dll";

// VCUGetResourceInstance will also do just what the name implies.
HINSTANCE VCUGetResourceInstance(const char * ptszUISatelliteDLL = g_ptszVCPackageUISatelliteDLL);
HINSTANCE VCUGetResourceInstance(const wchar_t * ptszUISatelliteDLL);


