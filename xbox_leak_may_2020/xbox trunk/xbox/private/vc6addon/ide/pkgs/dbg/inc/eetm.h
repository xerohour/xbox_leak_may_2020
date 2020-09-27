#ifndef __EETM_H__
#define __EETM_H__
// This file contains the basic EE TM structure encapsulations.
// The hope is that a similar interface can be provided by
// encapsulating the EE.

class CTM;
typedef CTypedPtrArray<CObArray, CTM*> CTMArray;

#define htmNil ((HTM)NULL)
extern struct CXF CxfWatch( ); 

// To allow for overloading on different types of CodeView
// HANDLES we define an enumeration which is used as
// an extra argument.

enum  HANDLE_TYPE { IS_HTM, IS_HSYM };

class CTM : public CObject
{
	protected:																	
		DECLARE_DYNAMIC(CTM);

	public:
		// Return values from the Refresh function.
		enum RES_REFRESH  { SAME_TYPE,  TYPE_CHANGED  };  
	public:
		virtual ~CTM ( );
	// Interface function.
	public:
		// Properties of this TM.
		virtual BOOL	IsExpandable() const { return FALSE; }
		virtual BOOL 	IsNameEditable( ) const { return FALSE; }
		virtual BOOL    IsValueEditable( ) { return FALSE; }
		virtual BOOL	IsSynthChildNode( ) const { return FALSE; }
		virtual BOOL	HasSynthChild( ) const { return FALSE; }

		// Is the value valid or has an error occured.
		virtual BOOL	IsValueValid( ) const { return FALSE; }
		virtual BOOL	DidValueChange( ) const { return FALSE; }
		virtual BOOL	CouldParseName() const { return FALSE; } 

		// Does this TM correspond to a data value.
		// Types and functions will return FALSE.
		virtual BOOL	IsDataTM( ) const { return FALSE; }	
		
		virtual BOOL	IsClass() const { return FALSE; }
		virtual BOOL	IsTypeIdentical(CTM *) { return FALSE; }
		// Did the last eval have any side effects?	
		virtual BOOL 	HadSideEffects( ) const { return FALSE; }

		virtual CString	GetName( ) = 0 ;
		virtual CString GetExpr( ) = 0 ;
		virtual CString GetValue( ) = 0 ; 
		virtual CString GetType( ) = 0;

		virtual CString GetEditableValue( ) { return GetValue (); }

		// Should be called only if IsExpandable is true.
		virtual BOOL GetTMChildren(CTMArray& ) = 0;
		virtual DWORD GetChildCount() = 0 ; 
		virtual CTM * GetTMChild(int childNo) = 0;
		virtual CTM * GetSynthChildTM( ) = 0;

		virtual BOOL  GetBCIA(PHBCIA) { return FALSE; }
		virtual void  FreeBCIA(PHBCIA) { };

		virtual RES_REFRESH	Refresh( ) { return SAME_TYPE; }
		virtual VOID	Restart( ) { };

		virtual BOOL  ModifyValue( const CString& ) { return FALSE; }
};

class CRootTM : public CTM
{
	public:
		CRootTM(LPCSTR lpszRootName, BOOL bNoFuncEval = FALSE);
		CRootTM( HIND  handle, HANDLE_TYPE  );
		virtual ~CRootTM ( );

		virtual BOOL  IsExpandable ( ) const;
		virtual BOOL  IsNameEditable( ) const;
		virtual BOOL  IsValueEditable( );
		virtual BOOL  IsSynthChildNode( ) const;
		virtual BOOL  HasSynthChild(  )  const;
		virtual BOOL  IsValueValid( ) const;
		virtual BOOL  DidValueChange( ) const { return m_bValueChanged; }
		virtual BOOL  CouldParseName() const;
		virtual BOOL  IsDataTM() const;

		virtual BOOL  IsClass() const;
		virtual BOOL  IsTypeIdentical(CTM *);
		virtual BOOL  HadSideEffects( ) const;

		virtual CString GetName ( );
		virtual CString GetExpr ( );
		virtual CString GetValue ( );
		virtual CString GetValueInRadix(int radix);
		virtual CString GetExprSansFormat();

		virtual CString GetEditableValue( );
		virtual CString GetType ( );
		virtual BOOL GetTMChildren(CTMArray&);
		virtual DWORD GetChildCount();
		virtual CTM * GetTMChild(int nChild);
		virtual CTM * GetSynthChildTM( );

		virtual BOOL  GetBCIA(PHBCIA);
		virtual void  FreeBCIA(PHBCIA);

		virtual RES_REFRESH  Refresh();
		virtual VOID	Restart();

		virtual BOOL ModifyValue(const CString& str);

	protected:
		BOOL		m_bNameTM : 1 ;			// Was this TM created by passing in a name.
		BOOL		m_fValEditableCache : 1;
		BOOL		m_fExpandedTM : 1;		
		BOOL		m_fValueCacheValid : 1;
		BOOL		m_fNameCacheValid : 1;
		BOOL		m_fValEditableCacheValid : 1;
		BOOL		m_fTypeCacheValid : 1;
		BOOL		m_bCheckedForFunc:1;	// Make function calling more robust.
		BOOL		m_bHasFunctionCall:1;
		BOOL		m_bNoFurtherEval:1;
		BOOL		m_bNoFuncEval:1;
		BOOL		m_bHadSideEffects:1;	// TRUE if last eval caused side-effects.
		BOOL		m_bValueChanged:1;     // TRUE if the value changed during the 
										   // last refresh.

		HTM			m_htm;
		HTM 		m_htmSaved;
		CString		m_cstrValueCache;
		CString		m_cstrNameCache;
		CString		m_cstrTypeCache;
		EESTATUS	m_eestatus;
		int			m_radix;

		EESTATUS RebindAndEvaluate();
		inline void InvalidateCaches(); 
		inline void CommonInit( ) ;
#if _DEBUG
		virtual void Dump(CDumpContext& dc) const;
#endif

	protected:
		DECLARE_DYNAMIC(CRootTM);
};

void CRootTM::InvalidateCaches()
{
	m_fValueCacheValid = FALSE;
	m_fValEditableCacheValid = FALSE;
	m_fTypeCacheValid = FALSE;
	m_bValueChanged = FALSE;
}

void CRootTM::CommonInit( )
{
	m_eestatus = EENOERROR;
	m_htmSaved = htmNil;
	m_bCheckedForFunc = FALSE;
	m_bHasFunctionCall = FALSE;
	m_bNoFurtherEval = FALSE;
	m_bNoFuncEval = FALSE;
	m_bHadSideEffects = FALSE;
	m_radix	= ::radix;
}

////////////////////////////////////////////////////////////////////////////////
// CReturnTM

class CReturnTM : public CTM
{
	public:
		CReturnTM (LPCSTR szName, LPCSTR szValue, LPCSTR szType = "");
		CReturnTM (const CReturnTM&);

		virtual ~CReturnTM () {};

		// Is the value valid or has an error occured.
		virtual BOOL	IsValueValid( ) const
			{ return m_fValueValid; }

		virtual CString	GetName ()
			{ return m_strName; }

		virtual CString	GetType ()
			{ return m_strType; }

		virtual CString	GetValue ()
			{ return m_strValue; }

		virtual CString	GetExpr ()
			{ return m_strExpr; }

		virtual BOOL GetTMChildren(CTMArray&)
			{ ASSERT (FALSE); return FALSE; }

		virtual DWORD GetChildCount()
			{ ASSERT (FALSE); return 0; }
		
		virtual CTM * GetTMChild(int nChild)
			{ ASSERT (FALSE); return NULL; }

		virtual CTM * GetSynthChildTM( )
			{ ASSERT (FALSE); return NULL; }

	private:
		BOOL	m_fValueValid;
		CString	m_strName;
		CString	m_strValue;
		CString	m_strType;
		CString	m_strExpr;

	protected:
		DECLARE_DYNAMIC(CReturnTM);
};
	
#endif	// __EETM_H__
