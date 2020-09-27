#ifndef __COMENVI_H__
#define __COMENVI_H__

//
// Class to interpret a buffer containing a list of null
// strings. List is terminated with a 0.
//

template <class T>
inline const T* EndStr( const T* sz );

template <class T>
class StrList   // input must be formatted correctly
{
public:
    StrList( const T* pStart )
        : m_first( pStart )
    {
        for ( m_end = pStart; m_end != 0 && *m_end != 0; m_end = EndStr( m_end ) )
            ;
    }

    unsigned long Count() const {
        unsigned long i=0;
        for ( iterator it = begin(); it != end(); ++it )
            ++i;
        return i;
    }

    typedef const T* _Nodeptr;

    class iterator  // based on STL forward iterator
    {
    public:
	    iterator()
		    {}
	    iterator(_Nodeptr _P)
		    : _Ptr(_P) {}
	    const T& operator*() const
		    {return *_Ptr; }
//	    const T* operator->() const
//		    {return (&**this); }
	    iterator& operator++()
		    {_Ptr = EndStr( _Ptr );
		    return (*this); }
	    iterator operator++(int)
		    {iterator _Tmp = *this;
		    ++*this;
		    return (_Tmp); }
	    bool operator==(const iterator& _X) const
		    {return (_Ptr == _X._Ptr); }
	    bool operator!=(const iterator& _X) const
		    {return (!(*this == _X)); }
	    _Nodeptr _Mynode() const
		    {return (_Ptr); }
    private:
        _Nodeptr _Ptr;
    };

    iterator begin() { return iterator( m_first ); }
    iterator end() { return iterator( m_end ); }
    iterator begin() const { return iterator( m_first ); }
    iterator end() const { return iterator( m_end ); }

    bool empty() const { return begin() == end(); }

private:

    const T* m_first;
    const T* m_end;
};

typedef StrList<_TCHAR> tcStrList;
inline const _TCHAR* EndStr( const _TCHAR* sz ) { return sz+_tcslen(sz)+1; }

//
//	Class to interpret a buffer containing environment data
//
//  Format:
//			[<tag>'\0'<value>'\0']*
//			'\0'
//
//	The environ contains a list of 0 or more tag-value pairs,
//  each of which is a '\0' terminated string. A '\0' terminates
//  the entire list. Thus the trivial environ is a single '\0'.
//
class StrEnviron
{
public:
    StrEnviron( const _TCHAR* pStart, const _TCHAR* pEnd = 0 )
        : m_strlist( pStart )
    {
        assert( pEnd == 0 || &*m_strlist.end() <= pEnd );
        assert( m_strlist.Count() % 2 == 0 );
        // REVIEW: how about some way to test these in release?
    }
    
    const _TCHAR* szValueForTag( const _TCHAR* szTag ) const {
        if ( szTag != 0 ) {
            tcStrList::iterator itEnd = m_strlist.end();
            for ( tcStrList::iterator it = m_strlist.begin(); it != itEnd; ++it )
            {
                assert( *it != 0 );
                if ( _tcscmp( &*it++, szTag ) == 0 ) {
                    return &*(it);
                }
            }
        }
        return 0;
    }

    bool empty() const { return m_strlist.empty(); }
    unsigned long Count() const { return m_strlist.Count()/2; }
#ifdef _OSTREAM_
    void Dump( ostream& o )
    {
        tcStrList::iterator itEnd = m_strlist.end();
        for ( tcStrList::iterator it = m_strlist.begin(); it != itEnd; ++it ) {
            o << &*it++ << " " << &*it << '\n';
        }
    }
#endif
private:
    tcStrList m_strlist;
};

//
// Class to interpret the environment data in a COMPILESYM record
//
// This field contains an ST followed by an environment
//

#define IS_STTYPE(x)        fNeedsSzConversion(x->rectype)

class CompEnviron: public StrEnviron
{
public:

#ifdef LNGNM
    CompEnviron(COMPILESYM *psym) 
        : StrEnviron((const _TCHAR *)
            (fNeedsSzConversion((PSYM)psym) 
            ? ((psym->verSt + *psym->verSt < pbEndSym((PSYM)psym))
                    ? psym->verSt + *psym->verSt + 1
                    : NULL)
            : ((psym->verSt + strlen((char *)psym->verSt) + 1 < pbEndSym((PSYM)psym))
                    ? psym->verSt + strlen((char *)psym->verSt) + 1
                    : NULL)))
    {}
#else
    CompEnviron( const unsigned char* st, const unsigned char* pEnd ) 
        : StrEnviron( st + *st + 1 < pEnd  
        ? reinterpret_cast< const char* >( st ) + *st + 1 
        : 0 )
    {}
#endif
};

#define ENC_CWD "cwd" /* <the current working directory>*/
#define ENC_CL  "cl"  /*<full pathname to cl.exe> */
#define ENC_SRC "src" /*<full pathname to the source file>*/
#define ENC_CMD "cmd" /*<command line to reinvoke cl.exe with for an edit-n-continue recompile> */
#define ENC_PDB "pdb" /*<full pathname to compiler pdb file> */



#endif
