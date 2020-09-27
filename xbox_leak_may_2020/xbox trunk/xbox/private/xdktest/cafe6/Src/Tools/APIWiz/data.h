// data.h 
//
/////////////////////////////////////////////////////////////////////////////


//  Data structure classes
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _APIDATA_
#define _APIDATA_
class CParam
{
public:
    CParam();
    ~CParam();
// Attributes
public:
	CString m_strName;
    CString m_strType;
    CString m_strValue;
	CString m_strDesc;

// Operations
};

typedef CTypedPtrList<CPtrList, CParam*> CParamList;

/////////////////////////////////////////////////////////////////////////////

class CAPI : public CObject
{
public:
	CAPI();

// Attributes
public:
    CString m_strArea;          // AppWiz, Image Ed, Src Ed, or Class 
	CString m_strSubarea;       // Creation, Navigation, Search... Matches index in listbox.
	CString m_strClass;         // Class API belongs to
	CString m_strName;          // Open()
    CString m_strDesc;          // description
    CString m_strReturnVal;     // Return Value
	CString m_strReturnDesc;	// Return Value description
    CParamList m_paramList;     // List of pointers

// Operations

// Implementation
public:
	virtual ~CAPI();
};

typedef CTypedPtrArray<CObArray,CAPI*> CAPIArray;

/////////////////////////////////////////////////////////////////////////////

class CArea : public CObject
{
public:
    CArea();

// Attributes
public:
	CString m_strCategory;
	CString m_strBaseClass;
    CString m_strName;
    CStringList m_strListSubarea;
// Operations

// Implementation
public:
	virtual ~CArea();
};

typedef CTypedPtrArray<CObArray,CArea*> CAreaArray;


#endif //_APIDATA_

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
