
#ifndef _CREATEOBJECT_H_
#define _CREATEOBJECT_H_

template <class T> T* CreateObject(const CLSID &classid, const IID &iid)
    {
    IUnknown* pUnk = NULL;
    HRESULT hRes;
    T* obj = NULL;

    hRes = CoCreateInstance(classid, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pUnk);
    if(FAILED(hRes))
        {
        return NULL;
        }
    hRes = pUnk->QueryInterface(iid, (LPVOID*)&obj);
    pUnk->Release();
    if(FAILED(hRes))
        {
        return NULL;
        }
    obj->AddRef();
    return obj;
    }

template <class T> T* CreateServer(const CLSID &classid, const IID &iid, BSTR computerName=NULL, BSTR username=NULL, BSTR domain=NULL, BSTR pw=NULL)
    {
    HRESULT hRes;
    T* obj = NULL;

    COAUTHIDENTITY ident = { username, wcslen(username), domain, wcslen(domain), pw, wcslen(pw), SEC_WINNT_AUTH_IDENTITY_UNICODE };
    COAUTHINFO auth = { RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IMPERSONATE, &ident, EOAC_NONE };
    COSERVERINFO server = { 0, computerName, username?&auth:NULL, 0 };
    MULTI_QI info = { &iid, 0, 0 };
    DWORD location = CLSCTX_LOCAL_SERVER;

    if(computerName) location = CLSCTX_REMOTE_SERVER;

    hRes = CoCreateInstanceEx(classid, NULL, location, &server, 1, &info);
    if(FAILED(hRes))
        {
        char msg[1024];
        sprintf(msg, "CoCreateInstanceEx returned 0x%X\n", hRes);
        OutputDebugString(msg);
        return NULL;
        }
    obj = (T*)info.pItf;
    obj->AddRef();
    return obj;
    }

template <class T> void DestroyObject(T* obj)
    {
    if(obj) obj->Release();
    }

template <class T> T* CopyObject(const IID &iid, IDispatch *original)
    {
    T *obj = NULL;
    if(!original) return NULL;
    original->QueryInterface(iid, (LPVOID*)&obj);
    if(obj) obj->AddRef();
    return obj;
    }

#endif // _CREATEOBJECT_H_