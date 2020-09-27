#ifndef  __CSTACK_H__
#define  __CSTACK_H__

class CD3DXMatrixStack : public ID3DXMatrixStack
{
private:
    int m_stackSize;
    D3DXMATRIX *m_pstack;

    int m_currentPos;
    ULONG m_ulRefCount;

public:
    CD3DXMatrixStack();
    ~CD3DXMatrixStack();

    HRESULT Initialize(int stackSize);

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID * ppvObj);
    ULONG  STDMETHODCALLTYPE AddRef();
    ULONG  STDMETHODCALLTYPE Release();

    // ID3DXMatrixStack
    STDMETHOD(Pop)(THIS);
    STDMETHOD(Push)(THIS);
    STDMETHOD(LoadIdentity)(THIS);
    STDMETHOD(LoadMatrix)(THIS_ const D3DXMATRIX* pM );
    STDMETHOD(MultMatrix)(THIS_ const D3DXMATRIX* pM );
    STDMETHOD(MultMatrixLocal)(THIS_ const D3DXMATRIX* pM );
    STDMETHOD(RotateAxis)(THIS_ const D3DXVECTOR3* pV, float angle);
    STDMETHOD(RotateAxisLocal)(THIS_ const D3DXVECTOR3* pV, float angle);
    STDMETHOD(RotateYawPitchRoll)(THIS_ float yaw, float pitch, float roll);
    STDMETHOD(RotateYawPitchRollLocal)(THIS_ float yaw, float pitch, float roll);
    STDMETHOD(Scale)(THIS_ float x, float y, float z);
    STDMETHOD(ScaleLocal)(THIS_ float x, float y, float z);
    STDMETHOD(Translate)(THIS_ float x, float y, float z );
    STDMETHOD(TranslateLocal)(THIS_ float x, float y, float z);
    STDMETHOD_(D3DXMATRIX*, GetTop)(THIS);

};

#endif //__CSTACK_H__
