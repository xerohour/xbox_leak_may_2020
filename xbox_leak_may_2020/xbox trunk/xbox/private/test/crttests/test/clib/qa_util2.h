#if !defined(__QALIB_UTIL2_H__)
#define __QALIB_UTIL2_H__

#include "qa_macro.h"

///////////////////////////////////////////////////////////////////////////////
// header includes
//

#if defined(QA_USE_STRSTRM) || defined(QA_INC_ALL)
#include <sstream>
#endif //QA_USE_STRSTRM

#if defined(QA_USE_TEMP_DATA_FILE) || defined(QA_INC_ALL)
#include <stdio.h>
#include <io.h>
#include <sys\stat.h>
#endif //QA_USE_TEMP_DATA_FILE

#if defined(QA_USE_COMPOSE) || defined(QA_INC_ALL)
#include <functional>
#endif //QA_USE_COMPOSE

namespace QaLib
{

///////////////////////////////////////////////////////////////////////////////
// typedefs
#if defined(QA_USE_STRSTRM) || defined(QA_INC_ALL)

typedef _tostringstream COStrStream;
typedef _tistringstream CIStrStream;
#define QA_DEF_STRSTRM QaLib::COStrStream  ostrstrm
#define QA_DECL_STRSTRM(x) QaLib::COStrStream x
#define QA_DEF_OSTRSTRM QaLib::COStrStream  ostrstrm
#define QA_DECL_OSTRSTRM(x) QaLib::COStrStream x
#define QA_DEF_ISTRSTRM QaLib::CIStrStream  istrstrm
#define QA_DECL_ISTRSTRM(x) QaLib::CIStrStream x

#endif //QA_USE_STRSTRM

#if defined(QA_USE_TEMP_DATA_FILE) || defined(QA_INC_ALL)

//
// create datafile.
//
class CTempDataFile
{
public:
    CTempDataFile(const char* file, const char* data = 0, bool bClose = false)
    : m_file(file)
    {
        m_pFile = _tfopen(file, _T("w+"));
        if (m_pFile && data)
            _fputts(data, m_pFile);
        if (bClose)
            Close();
    }

    ~CTempDataFile()
    {
        Close();
        _tchmod(m_file, S_IWRITE);
        _tunlink(m_file);
    }

    bool operator !() const { return Fail(); }
    FILE* operator *() const { return m_pFile; }

    bool            Fail() const { return !m_pFile; }
    const char*     Path() const { return m_file; }

    bool            Close() 
    { 
        bool    retVal = false;
        if (m_pFile)
            retVal = fclose(m_pFile) == 0; 
        m_pFile = 0; 
        return retVal;
    }

protected:
    const char*     m_file;     // path to file.
    FILE*           m_pFile;    // file handle.
};

#endif // QA_USE_TEMP_DATA_FILE

#if defined(QA_USE_IOTAGEN) || defined(QA_INC_ALL)

template<class _Ty> struct IotaGen
{
    IotaGen() {}
    IotaGen(_Ty v) : init(v) {}
    _Ty operator()()
    {
        return init++; 
    }
private:
    _Ty     init;
};

#endif // QA_USE_IOTAGEN

///////////////////////////////////////////////////////////////////////////////
// template functions.

#if defined(QA_USE_IOTA) || defined(QA_INC_ALL)

//
// iota() : sets all values in container to 'v'.
//
template<class _OI, class _V> inline void iota(_OI _F, _OI _L, _V v)
{
    _V  x = v;
    for (; _F != _L; ++_F)
        *_F = x++;
}

#endif // QA_USE_IOTA

///////////////////////////////////////////////////////////////////////////////

#if defined(QA_USE_CLEAR) || defined(QA_INC_ALL)

//
// free contained pointers. assumes container contains pointers to T.
//
template<class _II> inline void ClearPtrContainer(_II _F, _II _L)
{
    for (; _F != _L; ++_F)
        delete *_F;
}

#endif // QA_USE_CLEAR

///////////////////////////////////////////////////////////////////////////////

#if defined(QA_USE_FILL_CONTAINER) || defined(QA_INC_ALL)

//
// Fill given container with values of T1 specified by "b" and "e".
//
template <class T, class T1> void FillContainer(T& l, T1* b, T1* e)
{
    T1* x = b;
    while (x != e)
        l.push_back(*x++);
}

#endif //QA_USE_FILL_CONTAINER

#if defined(QA_USE_COMPOSE) || defined(QA_INC_ALL)

//
// these functions are taken from the HP version of STL since the current
// c++ standard doesn't define them.
//

template <class Operation1, class Operation2>
class unary_compose : public STD unary_function<Operation2::argument_type,
                                                 Operation1::result_type> {
protected:
    Operation1 op1;
    Operation2 op2;
public:
    unary_compose(const Operation1& x, const Operation2& y) : op1(x), op2(y) {}
    result_type operator()(const argument_type& x) const {
    return op1(op2(x));
    }
};

template <class Operation1, class Operation2>
unary_compose<Operation1, Operation2> compose1(const Operation1& op1, 
                           const Operation2& op2) {
    return unary_compose<Operation1, Operation2>(op1, op2);
}

template <class Operation1, class Operation2, class Operation3>
class binary_compose : public STD unary_function<Operation2::argument_type,
                                                  Operation1::result_type> {
protected:
    Operation1 op1;
    Operation2 op2;
    Operation3 op3;
public:
    binary_compose(const Operation1& x, const Operation2& y, 
           const Operation3& z) : op1(x), op2(y), op3(z) { }
    result_type operator()(const argument_type& x) const {
    return op1(op2(x), op3(x));
    }
};

template <class Operation1, class Operation2, class Operation3>
binary_compose<Operation1, Operation2, Operation3> 
compose2(const Operation1& op1, const Operation2& op2, const Operation3& op3) {
    return binary_compose<Operation1, Operation2, Operation3>(op1, op2, op3);
}

#endif // QA_USE_COMPOSE

#if defined(QA_USE_VERIFY_SORT) || defined(QA_INC_ALL)

// verify a stable sort using a weak relation, op
// not a particularly fast implementation (choose to reuse):
// (1) checks that the sort is correct with VerifyWeakSort
// (2) checks that 
// NOTE: we require that a == be defined on these elements that
// can determine the correspondance between elements in the sorted and unsorted
// lists. If such an operator is not defined, the test is useless (but doesn't fail)
// REF: verifies the result of stable_sort, as defined in C++ Standard, 25.3.1.2
template <class T, class F>
bool VerifyStableSort(_tostream& o, T firstSorted, T lastSorted, F op, T firstUnsorted, T lastUnsorted)
{
  if (VerifyWeakSort(o, firstSorted, lastSorted, op))
    {
      for (T c = firstSorted; c != lastSorted; c++)
	{
	  T t = c;
	  t++;
	  for (; t != lastSorted; t++)
	    {
	      if (op(*c, *t))
		continue; // after we see an ordering,
	                  // skip to the next equivalent group
	      
	      // c and t are equivalent, check that c comes before t in the original
	      // (unsorted) sequence
	      for (T u = firstUnsorted; u != lastUnsorted; u++)
		{
		  // the order here is important if == is defined badly
		  // (e.g. (!op(*x,*y) && !op(*y,*x))).
		  if (*u == *c) break;
		  if (*u == *t)
		  {
		    o << "Invalid stable sort. ";
		    o << " Sorted element "  << c - firstSorted;
		    o << " is equivalent to sorted element " << t - firstSorted;
		    o << " but the pair's order is reversed with respect to ";
		    o << " the unsorted list" << STD endl;
		    return false;
		  };
		};
	    };
	};
      return true;
    };

    return false;
}

// verify a sort using a weak relation, op
// every element must be followed by 0 or more equivalent elements
// (for which the op returns false in both directions). All the elements
// after the equivalent set must make op(X,?)  true. 
// REF: verifies the result of sort, as defined in C++ Standard, 25.3.1.1
template <class T, class F>
bool VerifyWeakSort(_tostream& o, T f, T l, F op)
{
    for (T c = f; c != l; c++)
    {
      bool bEquiv = true;
        for (T t = c; t != l; t++)
        {
            if (!(*t == *c))
	      {
		if (op(*c, *t)) bEquiv = false;
		if (bEquiv)
		  {
		    // verify that t and c are equivalent
		    bool bTC = QACheck(!op(*t, *c));
		    bool bCT = QACheck(!op(*c, *t));
		    if (!bTC || !bCT)
		      {
			o << "Invalid weak sort: error at location: "
			  << f-c << " not equivalent, but not strongly ordered"
			  << STD endl;
			return false; // quit after first error -- don't pollute logs
		      };
		  }
		else
		  {
		    if (!QACheck(op(*c, *t)))
		      {
			o << "Invalid weak sort: error at location: "
			  << f-c << " after equivalence" << STD endl;
			return false; // quit after first error -- don't pollute logs
		      };
		  };
	      };
        }
    }
    return true;
}

// verify partial sort. all the elements from f to m are
// "op" w.r.t. all the elements from m to l.
// REF: verifies the result of partial_sort, as defined in C++ Standard, 25.3.1.3
template <class T, class F>
bool VerifyPartialSort(_tostream& o, T f, T m, T l, F op)
{
    for (T c = f; c != m-1; c++)
    {
          for (T t = m; t != l; t++)
        {
	  if (!QACheck(op(*c, *t)))
	    {
	      o << "Invalid partial sort: error at location: " << c-f << " , " << t-f <<
		" (middle at " << m-f << ") " << STD endl;
	      return false; // quit after first error -- don't pollute logs
	    };
        }
    }
    return true;
}

template <class T, class F>
void VerifySort(_tostream& o, T f, T l, F op)
{
    T s = f;
    for (T c = f; c != l; c++)
    {
        for (T t = s; t != c; t++)
        {
            if (!(*t == *c))
                if (!QACheck(op(*t, *c)))
                    o << "\terror at location: " << c - t << STD endl;
        }
    }
}

template <class T, class F>
void VerifySort(T f, T l, F op)
{
    VerifySort(_tcout, f, l, op);
}
                      

#endif //QA_USE_VERIFY_SORT

#if defined(QA_USE_PRINT_SEQ) || defined(QA_INC_ALL)

//
// Both functions print the sequence defined by "first" and "last" to 
// an output stream.  The first function takes the stream to print to.
// The second function is for compatibility with old tests - requires a global
// output stream object called "ostrstrm".
// When writing new code, use the first version.
//
template <class O, class T> O& PrintSeq(O& o, T first, T last)
{
    T   t = first;
    while (t != last)
        o << *t++ << _T(" ");
    if (first != last)
        o << STD endl;
    return o;
}

template <class T> void print_seq(T first, T last)
{
    T   t = first;
    while (t != last)
        ostrstrm << *t++ << _T(" ");
    if (first != last)
        ostrstrm << STD endl;
}

#endif // QA_USE_PRINT_SEQ

}   // namespace QaLib

#endif  //__QALIB_UTIL2_H__
