namespace ATL
{

template<>
class CComQIPtr< IStream, &__uuidof( IStream ) > :
	public CComPtr< IStream >
{
public:
	CComQIPtr() throw()
	{
	}
	CComQIPtr( IStream* p_ ) throw() :
		CComPtr< IStream >( p_ )
	{
	}
	CComQIPtr( const CComQIPtr& p_ ) throw() :
		CComPtr< IStream >( p_ )
	{
	}
	CComQIPtr( IUnknown* p_ ) throw()
	{
		if( p_ != NULL )
		{
			p_->QueryInterface( __uuidof( IStream ), reinterpret_cast< void** >( &p ) );
		}
	}
	~CComQIPtr() throw()
	{
	}

	CComQIPtr& operator=( IStream* p_ ) throw()
	{
		AtlComPtrAssign( reinterpret_cast< IUnknown** >( &p ), p_ );
		return( *this );
	}
	CComQIPtr& operator=( const CComQIPtr& p_ ) throw()
	{
		AtlComPtrAssign( reinterpret_cast< IUnknown** >( &p ), p_.p );
		return( *this );
	}
	CComQIPtr& operator=( IUnknown* p_ ) throw()
	{
		AtlComQIPtrAssign( reinterpret_cast< IUnknown** >( &p ), p_, __uuidof( IStream ) );
		return( *this );
	}

public:
	void CopyTo( IStream* pStream, ULONGLONG nBytes = _UI64_MAX ) throw( ... )
	{
		ULARGE_INTEGER nBytes2;
		HRESULT hr;

		ATLASSERT( p != NULL );

		nBytes2.QuadPart = nBytes;
		hr = p->CopyTo( pStream, nBytes2, NULL, NULL );
		if( FAILED( hr ) )
		{
			AtlThrow( hr );
		}
	}
	ULONGLONG GetSeekPosition() throw( ... )
	{
		HRESULT hr;
		LARGE_INTEGER nMove;
		ULARGE_INTEGER iSeek;

		ATLASSERT( p != NULL );

		nMove.QuadPart = 0;
		hr = p->Seek( nMove, STREAM_SEEK_CUR, &iSeek );
		if( FAILED( hr ) )
		{
			AtlThrow( hr );
		}

		return( iSeek.QuadPart );
	}
	ULONGLONG GetSize() throw( ... )
	{
		HRESULT hr;
		STATSTG stat;

		ATLASSERT( p != NULL );

		hr = p->Stat( &stat, STATFLAG_NONAME );
		if( FAILED( hr ) )
		{
			AtlThrow( hr );
		}

		return( stat.cbSize.QuadPart );
	}
	void Read( void* pBuffer, ULONG nBytes ) throw( ... )
	{
		ULONG nBytesRead;
		HRESULT hr;

		ATLASSERT( pBuffer != NULL );
		ATLASSERT( p != NULL );

		hr = p->Read( pBuffer, nBytes, &nBytesRead );
		if( FAILED( hr ) )
		{
			AtlThrow( hr );
		}
		if( nBytesRead != nBytes )
		{
			AtlThrow( E_FAIL );
		}
	}
	ULONGLONG Seek( LONGLONG nMove, STREAM_SEEK eOrigin = STREAM_SEEK_SET ) throw( ... )
	{
		HRESULT hr;
		LARGE_INTEGER nMove2;
		ULARGE_INTEGER iSeek;

		ATLASSERT( p != NULL );

		nMove2.QuadPart = nMove;
		hr = p->Seek( nMove2, eOrigin, &iSeek );
		if( FAILED( hr ) )
		{
			AtlThrow( hr );
		}

		return( iSeek.QuadPart );
	}
	void SetSize( ULONGLONG nBytes ) throw( ... )
	{
		HRESULT hr;
		ULARGE_INTEGER nSize;

		ATLASSERT( p != NULL );

		nSize.QuadPart = nBytes;
		hr = p->SetSize( nSize );
		if( FAILED( hr ) )
		{
			AtlThrow( hr );
		}
	}
	void Write( const void* pBuffer, ULONG nBytes ) throw( ... )
	{
		HRESULT hr;

		ATLASSERT( pBuffer != NULL );
		ATLASSERT( p != NULL );

		hr = p->Write( pBuffer, nBytes, NULL );
		if( FAILED( hr ) )
		{
			AtlThrow( hr );
		}
	}

//REVIEW: make as globals
#ifdef __ATLCOLL_H__

	template< class E, class ETraits >
	CComQIPtr< IStream >& operator<<( const CAtlList< E, ETraits >& list )
	{
		POSITION posElement;

		(*this)<<ULONG( list.GetCount() );
		posElement = list.GetHeadPosition();
		while( posElement != NULL )
		{
		   (*this)<<list.GetAt( posElement );
		   list.GetNext( posElement );
		}

		return( *this );
	}

	template< class E, class ETraits >
	CComQIPtr< IStream >& operator>>( CAtlList< E, ETraits >& list )
	{
		ULONG nElements;
		E element;

		(*this)>>nElements;
		for( ULONG iElement = 0; iElement < nElements; iElement++ )
		{
		   (*this)>>element;
		   list.AddTail( element );
		}

		return( *this );
	}

	template< typename E, class ETraits >
	CComQIPtr< IStream >& operator<<( const CAtlArray< E, ETraits >& array )
	{
		(*this)<<ULONG( array.GetSize() );
		for( size_t iElement = 0; iElement < array.GetSize(); iElement++ )
		{
		   (*this)<<array[iElement];
		}

		return( *this );
	}

	template< typename E, class ETraits >
	CComQIPtr< IStream >& operator>>( CAtlArray< E, ETraits >& array )
	{
		ULONG nElements;

		(*this)>>nElements;
		array.SetSize( nElements );
		for( size_t iElement = 0; iElement < array.GetSize(); iElement++ )
		{
		   (*this)>>array[iElement];
		}

		return( *this );
	}

#endif  // __ATLCOLL_H__

public:
	static ULONG GetPersistedDataSize( const ATL::CStringW& str ) throw()
	{
		return( sizeof( ULONG )+(str.GetLength()*sizeof( OLECHAR )) );
	}
};

CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, const GUID& guid ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, float f ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, double f ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, LPCSTR psz ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, LPCWSTR psz ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, const CStringA& str ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, const CStringW& str ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, BYTE b ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, USHORT n ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, LONG n ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, ULONG n ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, LONGLONG n ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, ULONGLONG n ) throw( ... );
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, IUnknown* pUnknown ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, GUID& guid ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, float& f ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, double& f ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, CStringA& str ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, CStringW& str ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, BYTE& b ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, USHORT& n ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, LONG& n ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, ULONG& n ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, LONGLONG& n ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, ULONGLONG& n ) throw( ... );
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, CComPtr< IUnknown >& pUnknown ) throw( ... );
template< class T, IID* piid > CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, CComQIPtr< T, 
	piid >& pObject ) throw( ... );

#if 0
#ifdef __ATLCOLL_H__

template< class E, class ETraits >
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, const CAtlList< E, ETraits >& list )
{
	POSITION posElement;

	(*this)<<ULONG( list.GetCount() );
	posElement = list.GetHeadPosition();
	while( posElement != NULL )
	{
	   (*this)<<list.GetAt( posElement );
	   list.GetNext( posElement );
	}

	return( *this );
}

template< class E, class ETraits >
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, CAtlList< E, ETraits >& list )
{
	ULONG nElements;
	E element;

	(*this)>>nElements;
	for( ULONG iElement = 0; iElement < nElements; iElement++ )
	{
	   (*this)>>element;
	   list.AddTail( element );
	}

	return( *this );
}

template< typename E, class ETraits >
CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, const CAtlArray< E, ETraits >& array )
{
	(*this)<<ULONG( array.GetSize() );
	for( size_t iElement = 0; iElement < array.GetSize(); iElement++ )
	{
	   (*this)<<array[iElement];
	}

	return( *this );
}

template< typename E, class ETraits >
CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, CAtlArray< E, ETraits >& array )
{
	ULONG nElements;

	(*this)>>nElements;
	array.SetSize( nElements );
	for( size_t iElement = 0; iElement < array.GetSize(); iElement++ )
	{
	   (*this)>>array[iElement];
	}

	return( *this );
}

#endif  // __ATLCOLL_H__
#endif

template< class T, IID* piid >
inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, CComQIPtr< T, piid >& pObject )
{
	CComPtr< IUnknown > punkObject;

	pStream>>punkObject;
	pObject = punkObject;
	if( pObject == NULL )
	{
		AtlThrow( E_NOINTERFACE );
	}

	return( *this );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, const GUID& guid )
{
	pStream.Write( &guid, sizeof( guid ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, float f )
{
	pStream.Write( &f, sizeof( float ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, double f )
{
	pStream.Write( &f, sizeof( double ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, LPCSTR psz )
{
	if( psz == NULL )
	{
		pStream<<ULONG( -1 );
	}
	else
	{
		CA2W pszW( psz );

		pStream<<pszW;
	}

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, LPCWSTR psz )
{
	if( psz == NULL )
	{
		pStream<<ULONG( -1 );
	}
	else
	{
		//TODO: Variable-length counts
		ULONG nLength = ULONG( ocslen( psz ) );
		(pStream)<<nLength;

		pStream.Write( psz, nLength*sizeof( wchar_t ) );
	}

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, const CStringA& str ) throw( ... )
{
	return( pStream<<CA2W( str ) );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, const CStringW& str ) throw( ... )
{
	return( pStream<<LPCWSTR( str ) );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, BYTE b ) throw( ... )
{
	pStream.Write( &b, sizeof( BYTE ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, USHORT n )
{
	pStream.Write( &n, sizeof( USHORT ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, LONG n )
{
	pStream.Write( &n, sizeof( LONG ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, ULONG n )
{
	pStream.Write( &n, sizeof( ULONG ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, LONGLONG n )
{
	pStream.Write( &n, sizeof( LONGLONG ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, ULONGLONG n )
{
	pStream.Write( &n, sizeof( ULONGLONG ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator<<( CComQIPtr< IStream >& pStream, IUnknown* pUnknown )
{
	HRESULT hr;

	ATLASSERT( pUnknown != NULL );
	ATLASSERT( pStream != NULL );

	CComQIPtr< IPersistStreamInit > pPSI = pUnknown;
	if( pPSI == NULL )
	{
		AtlThrow( E_NOINTERFACE );
	}

	CLSID clsid;
	hr = pPSI->GetClassID( &clsid );
	if( FAILED( hr ) )
	{
		AtlThrow( hr );
	}

	hr = ::WriteClassStm( pStream, clsid );
	if( FAILED( hr ) )
	{
		AtlThrow( hr );
	}

	hr = pPSI->Save( pStream, TRUE );
	if( FAILED( hr ) )
	{
		AtlThrow( hr );
	}

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, GUID& guid )
{
	pStream.Read( &guid, sizeof( GUID ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, float& f )
{
	pStream.Read( &f, sizeof( float ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, double& f )
{
	pStream.Read( &f, sizeof( double ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, CStringA& str )
{
	CStringW strW;

	pStream>>strW;
	str = strW;

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, CStringW& str )
{
	ULONG nLength;
	pStream>>nLength;
	if( nLength == ULONG( -1 ) )
	{
		str.Empty();
		return( pStream );
	}
	LPOLESTR pszBuffer = str.GetBuffer( nLength );
	_ATLTRY
	{
		pStream.Read( pszBuffer, nLength*sizeof( OLECHAR ) );
		str.ReleaseBuffer( nLength );
	}
	_ATLCATCHALL()
	{
		str.ReleaseBuffer( 0 );
		_ATLRETHROW;
	}

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, BYTE& b )
{
	pStream.Read( &b, sizeof( BYTE ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, USHORT& n )
{
	pStream.Read( &n, sizeof( USHORT ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, LONG& n )
{
	pStream.Read( &n, sizeof( LONG ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, ULONG& n )
{
	pStream.Read( &n, sizeof( ULONG ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, LONGLONG& n )
{
	pStream.Read( &n, sizeof( LONGLONG ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, ULONGLONG& n )
{
	pStream.Read( &n, sizeof( ULONGLONG ) );

	return( pStream );
}

inline CComQIPtr< IStream >& operator>>( CComQIPtr< IStream >& pStream, CComPtr< IUnknown >& pUnknown )
{
	HRESULT hr;

	ATLASSERT( pUnknown == NULL );
	ATLASSERT( pStream != NULL );

	CLSID clsid;
	hr = ::ReadClassStm( pStream, &clsid );
	if( FAILED( hr ) )
	{
		AtlThrow( hr );
	}

	CComQIPtr< IPersistStreamInit > pPSI;
	hr = pPSI.CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER );
	if( FAILED( hr ) )
	{
		AtlThrow( hr );
	}

	hr = pPSI->Load( pStream );
	if( FAILED( hr ) )
	{
		AtlThrow( hr );
	}

	pUnknown = pPSI;
	ATLASSERT( pUnknown != NULL );

	return( pStream );
}

};  // namespace ATL
