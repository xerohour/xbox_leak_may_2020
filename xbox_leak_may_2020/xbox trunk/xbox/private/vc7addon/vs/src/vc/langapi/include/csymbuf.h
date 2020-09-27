#ifndef _CSYMBUF_H_
#define _CSYMBUF_H_

#pragma warning( disable: 4786 )

class FuncBuffer;
typedef PROCSYM* PPROCSYM;
typedef PROCSYM32* PPROCSYM32;

//
// SymBuffer
//  understands the representation of cv symbolic information
//  as stored in an object file section. This object supplies
//  an interpretation of an existing buffer, it does not manage
//  the memory itself.
//

class SymBuffer 
{
public:
    typedef unsigned long ULONG;
    typedef ULONG SigType;
    SymBuffer( PB pbFirst = 0, CB cb = 0 ) : m_first( 0 ), m_last( 0 ), m_start( 0 )
    {
        Reset( pbFirst, cb );
    }
    void Reset( PB pbFirst, CB cb ) // set new buffer extent
    {
        m_sigSymsT = 0;
        assert( cb >= 0 );
        if ( pbFirst > 0 ) {
      	    m_sigSymsT = *reinterpret_cast<SigType*>(pbFirst);
            if (  CV_SIGNATURE_C6 < m_sigSymsT && m_sigSymsT < CV_SIGNATURE_RESERVED ) {
                m_first = reinterpret_cast<PSYM>(pbFirst+sizeof(SigType));
            } else {
                m_first = reinterpret_cast<PSYM>(pbFirst);
            }
        }
        m_last = reinterpret_cast<PSYM>(pbFirst+cb);
        m_start = pbFirst;
    }

    typedef PSYM _Nodeptr;

    class iterator  // based on STL forward iterator
    {
    public:
	    iterator()
		    {}
	    iterator(_Nodeptr _P)
		    : _Ptr(_P) {}
	    SYMTYPE& operator*() const
		    {return *_Ptr; }
	    SYMTYPE* operator->() const
		    {return (&**this); }
	    iterator& operator++()
		    {_Ptr = NextSym(_Ptr);
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

    iterator findNextProcSyms( iterator it, FuncBuffer& rfb );
    iterator findNextFileSym( iterator it );

    iterator begin() { return iterator( m_first ); }
    iterator end() { return iterator( m_last ); }
    iterator begin() const { return iterator( m_first ); }
    iterator end() const { return iterator( m_last ); }

    bool empty() const { return begin() == end(); }
    
    CB offSym( PSYM psym ) 
    { 
        return CB(asPb(psym)-pbfirst());    // REVIEW:WIN64 cast
    }
    PSYM cvsymOff( CB off )
    {
        return asPsym( pbfirst()+off );
    }
    SigType signature() { return m_sigSymsT; }

    template <class T> 
    bool findSym( int s, T** t )
    {
        for ( iterator it = begin(); it != end(); ++it ) {
            if (it->rectyp == (unsigned)s) {
                if ( t ) *t = reinterpret_cast<T*>(&*it);
                return true;
            }
        }
        return false;
    }

    template <class T> 
    bool findNextSym( int s, T** t, iterator& it )
    {
        for ( ; it != end(); ++it ) {
            if ( it->rectyp == (unsigned)s ) {
                if ( t ) *t = reinterpret_cast<T*>(&*it);
                return true;
            }
        }
        return false;
    }
    CB Count() { return CB(PB( m_last ) - m_start); /* REVIEW:WIN64 cast */ }
    PB Base() { return m_start; }
    virtual PB pbStart() { return m_start; }
    void updateOffsets();
    void updateTypeServer( USHORT itsm );

protected:
     _Nodeptr first() const { return m_first; }
     _Nodeptr last() const { return m_last; }
     PB pbfirst() const { return asPb(m_first); }
    static inline PB asPb( PSYM psym ) { return reinterpret_cast<PB>( psym ); }
    static inline PSYM asPsym( PB pb ) { return reinterpret_cast<PSYM>( pb ); }
   
private:
    friend FuncBuffer;
    PB m_start;
    _Nodeptr m_first;
    _Nodeptr m_last;  // actually, one past the last byte of the buffer
    SigType m_sigSymsT;
};


#endif
