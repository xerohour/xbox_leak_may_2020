#pragma once 
#include "ctiunk.h"
#include "dmusici.h"



//===========================================================================
// CtIDirectMusicLoader
//
// Wrapper class for IDirectMusicLoader
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicLoader : public CtIUnknown
{
/*
    public:
    // constructor / destructor
    CtIDirectMusicLoader();
    ~CtIDirectMusicLoader();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicLoader *pdmLoader);
    virtual HRESULT GetRealObjPtr(IDirectMusicLoader **ppdmLoader);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicLoader methods
    // NOTENOTE: we are overloading here due to a late change in the GetObject
    //  method on the actual IDirectMusicLoader interface.  Since we have
    //  quite a number of test cases implemented using the old version of
    //  GetObject, we simply re-implemented it in our class
    virtual HRESULT GetObject(CtIDirectMusicObject **ppWObject,
                                LPDMUS_OBJECTDESC pDesc);
    virtual HRESULT GetObject(LPDMUS_OBJECTDESC pDesc,
                            REFIID riid,
                            void** ppv);
*/
/*
    virtual HRESULT SetSearchDirectory(GUID idClass,
                                        *pzPath,
                                        BOOL fClear);
    virtual HRESULT CacheObject(CtIDirectMusicObject *pObject);
    virtual HRESULT ReleaseObject(CtIDirectMusicObject *pObject);
    virtual HRESULT ClearCache(GUID idClass);
    virtual HRESULT EnableCache(GUID idClass,
                                BOOL fEnable);
    virtual HRESULT EnumObject(GUID idClass,
                                DWORD dwIndex,
                                LPDMUS_OBJECTDESC pDesc);
*/

}; // ** end CtIDirectMusicLoader

//===========================================================================
// CtIDirectMusicLoader8
//
// Wrapper class for IDirectMusicLoader8
//
// Inherits from CtIDirectMusicLoader
//===========================================================================
class CtIDirectMusicLoader8 : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicLoader8();
    //~CtIDirectMusicLoader8();
    // test class helpers
    //virtual HRESULT InitTestClass(IDirectMusicLoader8 *pdmLoader8);

    virtual HRESULT LoadObjectFromFile(REFGUID rguidClassID,
                                        REFIID iidInterfaceID,
                                        CHAR *pzFilePath,
                                        void ** ppObject);

    virtual HRESULT SetObject(LPDMUS_OBJECTDESC pDesc);


    // IDirectMusicLoader8
//  virtual HRESULT EnableGarbageCollector(BOOL fEnable);
/*
    virtual void CollectGarbage(void);
    virtual HRESULT ReleaseObjectByUnknown(CtIUnknown *pObject);
*/
//  virtual HRESULT GetDynamicallyReferencedObject(CtIDirectMusicObject *pSourceObject,
//                                                           LPDMUS_OBJECTDESC pDesc,
//                                                           REFIID riid,
//                                                           LPVOID FAR *ppv);
//  virtual HRESULT ReportDynamicallyReferencedObject(CtIDirectMusicObject *pSourceObject,
//                                                           CtIUnknown *pReferencedObject);


}; // ** end CtIDirectMusicLoader8
