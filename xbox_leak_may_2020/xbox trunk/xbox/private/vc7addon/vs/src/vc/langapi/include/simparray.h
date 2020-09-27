#pragma once
#include "crefobj.h"

template <class T>
class SimpleArray : public CRefCountedObj {
protected:
    size_t      _cT;
    T *         _rgT;

public:
    SimpleArray ( size_t cT ) {
        if ( cT ) {
            _rgT = new T[cT];
            if ( _rgT ) {
                _cT = cT;
                }
            }
        else {
            _cT = 0;
            _rgT = 0;
            }
        }

    ~SimpleArray() {
        if ( _rgT ) {
            delete [] _rgT;
            }
        _cT = 0;
        _rgT = 0;
        }

    size_t Count() const {
        return _cT;
        }

    const T * Base() const {
        return _rgT;
        }

    T * Base() {
        return _rgT;
        }

    T & operator[] ( unsigned it ) {
        return _rgT[it];
        }

    const T & operator[] ( unsigned it ) const {
        return _rgT[it];
        }
    };

class SimpleString: public SimpleArray<char>
{
public:
    size_t Length() const { return strlen( _rgT ); } // # non-nulls
    const char* operator=( const char *str ) {
        return Set( str, strlen( str ) );
    }
    bool operator==( SimpleString& str ) { return *this == str.Base(); }
    bool operator!=( SimpleString& str ) { return !(*this == str); }
    bool operator==( const char* str ) {
        return strcmp( str, Base() ) == 0; 
    }
    bool operator!=( const char* str ) { return !(*this == str); }
    void Clear() {
        assert( Count() > 0 );
        Set( "", 0 );
    }
    const char* Set( const char* str, size_t len ) {
        Grow( len+1 );
        strncpy( _rgT, str, len );
        _rgT[ len ] = '\0';
        return Base();
    }
    SimpleString( const SimpleString& str ) 
        : SimpleArray<char>( str.Length()+1 ) {
        Set( str.Base(), str.Length()+1 );
    }
    SimpleString( unsigned len = 256 ) : SimpleArray<char>( max( len, 1 ) ) {
        Set( "", 0 );
    }
private:
    bool Grow( size_t cb ) {
        if ( Count() < cb ) {
            delete [] _rgT;
            _rgT = new char[cb];
            _cT = cb;
        }
        return _rgT != 0;
    }

};
    
class SimpleUString: public SimpleArray<wchar_t>
{
public:
    size_t Length() const { return wcslen( _rgT ); } // # non-nulls
    const wchar_t* operator=( const wchar_t *str ) {
        return Set( str, wcslen( str ) );
    }
    bool operator==( SimpleUString& str ) { return *this == str.Base(); }
    bool operator!=( SimpleUString& str ) { return !(*this == str); }
    bool operator==( const wchar_t* str ) {
        return wcscmp( str, Base() ) == 0; 
    }
    bool operator!=( const wchar_t* str ) { return !(*this == str); }
    void Clear() {
        assert( Count() > 0 );
        Set( L"", 0 );
    }
    const wchar_t* Set( const wchar_t* str, size_t len ) {
        Grow( len+1 );
        wcsncpy( _rgT, str, len );
        _rgT[ len ] = L'\0';
        return Base();
    }
    SimpleUString( const SimpleUString& str ) 
        : SimpleArray<wchar_t>( str.Length()+1 ) {
        Set( str.Base(), str.Length() );
    }
    SimpleUString( size_t len = 256 ) : SimpleArray<wchar_t>( max( len, 1 ) ) {
        Set( L"", 0 );
    }
private:
    bool Grow( size_t cch ) {
        if ( Count() < cch ) {
            delete [] _rgT;
            _rgT = new wchar_t[cch];
            _cT = cch;
        }
        return _rgT != 0;
    }

};
    
