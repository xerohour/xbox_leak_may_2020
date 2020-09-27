/*++

Copyright (c) Microsoft Corporation


Module Name:

    objectwithsite.h

Abstract:
    
    Declaration of an CShellObjectWithSite class that always
    queries for IShellFolderView. 

Environment:

    Windows 2000
    Uses ATL

Notes:

Revision History:

    July 2001 - created by Mitchell Dernis (mitchd)

--*/

class CShellObjectWithSite : public IObjectWithSite
{

  public:

   /*
   ** c'tor, d'tor
   */
   CShellObjectWithSite() : m_pUnknownSite(NULL) {}
   virtual ~CShellObjectWithSite()
   {
      if(m_pUnknownSite) m_pUnknownSite->Release();
   }

   /*
   **  IObjectWithSite Methods
   */
   STDMETHOD(SetSite)(IUnknown* pUnkSite);
   STDMETHOD(GetSite)(REFIID riid, void **ppvSite);
   
   /*
   **  An extra convenience function gets a service from the site interface
   */
   STDMETHOD(GetService)(REFGUID guidService, REFIID riid, void **ppvService);

  protected:
   IUnknown *m_pUnknownSite;
};