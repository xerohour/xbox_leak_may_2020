#ifndef _UNKNOWN_HPP
#define _UNKNOWN_HPP

//===========================================================================
// This template implements the IUnknown portion of a given COM interface.

template <class I> class _unknown : public I
{
private:    long _refcount;

public:        
        _unknown() 
        { 
            _refcount = 1;
        }

        virtual ~_unknown()
        {
        }

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
        {
            if (riid == IID_IUnknown)
            {
                *ppvObject = static_cast<IUnknown*>(this);
            }
            else if (riid == __uuidof(I))
            {
                *ppvObject = static_cast<I*>(this);
            }
            else
            {
                *ppvObject = NULL;
                return E_NOINTERFACE;
            }
            reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();
            return S_OK;
        }
    
        virtual ULONG STDMETHODCALLTYPE AddRef( void)
        {
            return InterlockedIncrement(&_refcount);
        }
    
        virtual ULONG STDMETHODCALLTYPE Release( void)
        {
            if (InterlockedDecrement(&_refcount) == 0)
            {
                delete this;
                return 0;
            }
            return _refcount;
        }
};    


template <class I, class J> class _unknown2 : public I, public J
{
private:    long _refcount;

public:        
        _unknown2() 
        { 
            _refcount = 1;
        }

        virtual ~_unknown2()
        {
        }

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
        {
            if (riid == IID_IUnknown)
            {
                *ppvObject = static_cast<IUnknown*>(static_cast<I*>(this));
            }
            else if (riid == __uuidof(I))
            {
                *ppvObject = static_cast<I*>(this);
            }
	        else if (riid == __uuidof(J))
            {
                *ppvObject = static_cast<J*>(this);
            }
             else
            {
                *ppvObject = NULL;
                return E_NOINTERFACE;
            }
            reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();
            return S_OK;
        }
    
        virtual ULONG STDMETHODCALLTYPE AddRef( void)
        {
            return InterlockedIncrement(&_refcount);
        }
    
        virtual ULONG STDMETHODCALLTYPE Release( void)
        {
            if (InterlockedDecrement(&_refcount) == 0)
            {
                delete this;
                return 0;
            }
            return _refcount;
        }
};

template <class I, class J, class K> class _unknown3 : public I, public J, public K
{
private:    long _refcount;

public:        
        _unknown3() 
        { 
            _refcount = 1;
        }

        virtual ~_unknown3()
        {
        }

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
        {
            if (riid == IID_IUnknown)
            {
                *ppvObject = static_cast<IUnknown*>(static_cast<I*>(this));
            }
            else if (riid == __uuidof(I))
            {
                *ppvObject = static_cast<I*>(this);
            }
	        else if (riid == __uuidof(J))
            {
                *ppvObject = static_cast<J*>(this);
            }
	        else if (riid == __uuidof(K))
            {
                *ppvObject = static_cast<K*>(this);
            }
            else
            {
                *ppvObject = NULL;
                return E_NOINTERFACE;
            }
            reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();
            return S_OK;
        }
    
        virtual ULONG STDMETHODCALLTYPE AddRef( void)
        {
            return InterlockedIncrement(&_refcount);
        }
    
        virtual ULONG STDMETHODCALLTYPE Release( void)
        {
            if (InterlockedDecrement(&_refcount) == 0)
            {
                delete this;
                return 0;
            }
            return _refcount;
        }
};


template <class I, class J, class K, class L> class _unknown4 : public I, public J, public K, public L
{
private:    long _refcount;

public:        
        _unknown4() 
        { 
            _refcount = 1;
        }

        virtual ~_unknown4()
        {
        }

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
        {
            if (riid == IID_IUnknown)
            {
                *ppvObject = static_cast<IUnknown*>(static_cast<I*>(this));
            }
            else if (riid == __uuidof(I))
            {
                *ppvObject = static_cast<I*>(this);
            }
	        else if (riid == __uuidof(J))
            {
                *ppvObject = static_cast<J*>(this);
            }
	        else if (riid == __uuidof(K))
            {
                *ppvObject = static_cast<K*>(this);
            }
 	        else if (riid == __uuidof(L))
            {
                *ppvObject = static_cast<L*>(this);
            }
            else
            {
                *ppvObject = NULL;
                return E_NOINTERFACE;
            }
            reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();
            return S_OK;
        }
    
        virtual ULONG STDMETHODCALLTYPE AddRef( void)
        {
            return InterlockedIncrement(&_refcount);
        }
    
        virtual ULONG STDMETHODCALLTYPE Release( void)
        {
            if (InterlockedDecrement(&_refcount) == 0)
            {
                delete this;
                return 0;
            }
            return _refcount;
        }
};

#endif _UNKNOWN_HPP


