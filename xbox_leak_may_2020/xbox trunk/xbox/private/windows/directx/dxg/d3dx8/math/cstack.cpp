#include "pchmath.h"

CD3DXMatrixStack::CD3DXMatrixStack()
{
    m_pstack = NULL;
    m_stackSize = 0;

    m_currentPos = 0;
    m_ulRefCount = 1;
}

CD3DXMatrixStack::~CD3DXMatrixStack()
{
    if(m_pstack)
        delete [] m_pstack;

    return;
}

HRESULT
CD3DXMatrixStack::Initialize(int stackSize)
{
    D3DXASSERT(m_pstack == NULL);

    if(!(m_pstack = new D3DXMATRIX[stackSize]))
        return E_OUTOFMEMORY;

    m_stackSize = stackSize;
    m_currentPos = 0;

    LoadIdentity();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
CD3DXMatrixStack::QueryInterface( REFIID riid, LPVOID * ppvObj)
{
    if( !ppvObj )
    {
        DPF(0, "ppvObj pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if( IsEqualIID( riid, IID_ID3DXMatrixStack ) )
    {
        *ppvObj = (ID3DXMatrixStack *) this;
    }
    else if( IsEqualIID( riid, IID_IUnknown ) )
    {
        *ppvObj = (IUnknown *) this;
    }
    else
    {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

ULONG  STDMETHODCALLTYPE
CD3DXMatrixStack::AddRef()
{
    return ++m_ulRefCount;
}

ULONG  STDMETHODCALLTYPE
CD3DXMatrixStack::Release()
{
    ULONG ulRefCount = --m_ulRefCount;
    if( ulRefCount == 0 )
    {
        delete this;
    }
    return ulRefCount; // return the copy on the stack, which is guaranteed to still exist.
}


HRESULT
CD3DXMatrixStack::Pop()
{
    if (m_currentPos == 0)
        return S_OK;
    else
        m_currentPos--;

    return S_OK;
}

HRESULT
CD3DXMatrixStack::Push()
{
    m_currentPos++;

    if (m_currentPos >= m_stackSize)
    {
        D3DXMATRIX *pstack;

        if(!(pstack = new D3DXMATRIX[m_stackSize * 2]))
            return E_OUTOFMEMORY;

        if(m_pstack)
        {
            memcpy(pstack, m_pstack, m_stackSize * sizeof(D3DXMATRIX));
            delete [] m_pstack;
        }

        m_pstack = pstack;
        m_stackSize *= 2;
    }

    m_pstack[m_currentPos] = m_pstack[m_currentPos-1];
    return S_OK;
}

HRESULT
CD3DXMatrixStack::LoadIdentity()
{
    D3DXMatrixIdentity(&m_pstack[m_currentPos]);
    return S_OK;
}

HRESULT
CD3DXMatrixStack::LoadMatrix( const D3DXMATRIX *pMat )
{
#if DBG
    if(!pMat)
    {
        DPF(0, "pMat pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
#endif

    m_pstack[m_currentPos] = *pMat;
    return S_OK;
}

HRESULT
CD3DXMatrixStack::MultMatrix( const D3DXMATRIX *pMat )
{
#if DBG
    if(!pMat)
    {
        DPF(0, "pMat pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
#endif

    m_pstack[m_currentPos] = m_pstack[m_currentPos] * *pMat;
    return S_OK;
}

HRESULT
CD3DXMatrixStack::MultMatrixLocal( const D3DXMATRIX *pMat )
{
#if DBG
    if(!pMat)
    {
        DPF(0, "pMat pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
#endif

    m_pstack[m_currentPos] = *pMat * m_pstack[m_currentPos];
    return S_OK;
}

D3DXMATRIX *
CD3DXMatrixStack::GetTop()
{
    return &m_pstack[m_currentPos];
}

HRESULT
CD3DXMatrixStack::RotateAxis( const D3DXVECTOR3 *pV, float angle )
{
#if DBG
    if(!pV)
    {
        DPF(0, "pV pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
#endif

    D3DXMATRIX tmp;

    D3DXMatrixRotationAxis( &tmp, pV, angle );
    m_pstack[m_currentPos] = m_pstack[m_currentPos] * tmp;
    return S_OK;
}

HRESULT
CD3DXMatrixStack::RotateAxisLocal( const D3DXVECTOR3 *pV, float angle )
{
#if DBG
    if(!pV)
    {
        DPF(0, "pV pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
#endif

    D3DXMATRIX tmp;

    D3DXMatrixRotationAxis( &tmp, pV, angle );
    m_pstack[m_currentPos] = tmp * m_pstack[m_currentPos];
    return S_OK;
}

HRESULT
CD3DXMatrixStack::RotateYawPitchRoll( float yaw, float pitch, float roll )
{
    D3DXMATRIX tmp;

    D3DXMatrixRotationYawPitchRoll( &tmp, yaw, pitch, roll );
    m_pstack[m_currentPos] = m_pstack[m_currentPos] * tmp;
    return S_OK;
}

HRESULT
CD3DXMatrixStack::RotateYawPitchRollLocal( float yaw, float pitch, float roll )
{
    D3DXMATRIX tmp;

    D3DXMatrixRotationYawPitchRoll( &tmp, yaw, pitch, roll );
    m_pstack[m_currentPos] = tmp * m_pstack[m_currentPos];
    return S_OK;
}

HRESULT
CD3DXMatrixStack::Scale( float x, float y, float z )
{
    D3DXMATRIX tmp;

    D3DXMatrixScaling( &tmp, x, y, z );
    m_pstack[m_currentPos] = m_pstack[m_currentPos] * tmp;
    return S_OK;
}

HRESULT
CD3DXMatrixStack::ScaleLocal( float x, float y, float z )
{
    D3DXMATRIX tmp;

    D3DXMatrixScaling( &tmp, x, y, z );
    m_pstack[m_currentPos] = tmp * m_pstack[m_currentPos];
    return S_OK;
}

HRESULT
CD3DXMatrixStack::Translate( float x, float y, float z )
{
    D3DXMATRIX tmp;

    D3DXMatrixTranslation( &tmp, x, y, z );
    m_pstack[m_currentPos] = m_pstack[m_currentPos] * tmp;
    return S_OK;
}

HRESULT
CD3DXMatrixStack::TranslateLocal( float x, float y, float z )
{
    D3DXMATRIX tmp;

    D3DXMatrixTranslation( &tmp, x, y, z );
    m_pstack[m_currentPos] = tmp * m_pstack[m_currentPos];
    return S_OK;
}




HRESULT WINAPI
D3DXCreateMatrixStack( DWORD flags, ID3DXMatrixStack **ppStack )
{
    HRESULT hr;

    // These flags will be used in the future to specify that
    // the user wants the internals to be double precision

    // Validate Parameters
    if( ppStack == NULL )
    {
        DPF(0, "ppStack pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    CD3DXMatrixStack *pStack = new CD3DXMatrixStack;

    if( pStack == NULL )
        return E_OUTOFMEMORY;

    if( FAILED(hr = pStack->Initialize(16)) )
    {
        pStack->Release();
        return hr;
    }

    *ppStack = pStack;
    return S_OK;
}

