// tokens.h : header file
//
#ifndef __TOKENS_H__
#define __TOKENS_H__

/////////////////////////////////////////////////////////////////////////////
/// Support classes for wrapping the language packages

// Convenient wrapper for a TXTB. Knows about C++ token values.
// This can only be used with the C++ lexer package loaded. 
class CToken
{
public:
	CToken();
	CToken(const TOKEN&);

	const CToken& operator=(const CToken&);
	const CToken& operator=(const TOKEN&);
	operator TOKEN() const;

	TOKCLS GetTokClass() const;

	inline BOOL IsKeyword( ) const
	{	return (GetTokClass() == tokclsKeyWordMin); }

	inline BOOL IsInError( ) const
	{	return (GetTokClass() == tokclsError);  }

	inline BOOL IsConstant( ) const
	{ TOKCLS tokCls = GetTokClass();  return(tokCls >= tokclsConstInteger && tokCls <= tokclsConstString); }

	inline BOOL IsOperator( ) const
	{	return (GetTokClass() == tokclsOpMin); }

	inline BOOL IsUserIdent( ) const
	{	TOKCLS tokCls = GetTokClass(); return ( tokCls == tokclsIdentMin || tokCls == tokclsIdentUser); }


	BOOL IsCxxIdent( ) const; 		// includes "this" and user idents.
	BOOL IsAssignOp( ) const;
	BOOL IsIndirectionOp() const;	// '*' and '&'
	   
	  
private:
	TOKEN m_token;
	TOKCLS m_tokCls;
	static const TOKCLS * s_rgtokCls;
	static int ctokCls;
};	
		

////////////////////////////////////////////////////////////////////////////////
// CTokenLine

struct CTokenLine : public CObject
{
	CTokenLine( ) { };

	CString m_strLine;
	RGTXTB m_rgtxtb;
#ifdef _DEBUG
	virtual void Dump(CDumpContext &dc) const
	{
		dc << "CTokenLine" ; CObject::Dump(dc);
	}
#endif
};


typedef CTypedPtrArray<CObArray, CTokenLine *> CTokenLineArray;

// Given a CTextDoc and a set of lines this class lets you access the various tokens within 
// the corresponding stream from the textdoc. 


////////////////////////////////////////////////////////////////////////////////
// CTokenStream

class CTokenStreamPos
{
	int m_nIndex;
	int m_nToken;
	friend class CTokenStream;
public:
	inline BOOL IsValid () 
	{	return (m_nIndex != -1); }

	inline BOOL operator == (CTokenStreamPos &pos) const
	{	return (m_nIndex == pos.m_nIndex && m_nToken == pos.m_nToken); }

	inline BOOL operator != (CTokenStreamPos &pos) const 
	{	return !(m_nIndex == pos.m_nIndex && m_nToken == pos.m_nToken); }

	inline BOOL IsAtStartOfLine ()
	{	return (IsValid () && m_nToken == 0); }
};


class CTokenStream : public CObject
{
public:
	CTokenStream(LPSOURCEEDIT pISourceEdit, ILINE begLine, ILINE endLine);	// both begLine and endLine are inclusive.
	virtual ~CTokenStream();

	CTokenStreamPos GetHeadPosition() const;
	CTokenStreamPos GetTailPosition() const;

	TXTB* GetAt (CTokenStreamPos& pos);
	TXTB* GetNext (CTokenStreamPos& pos);
	TXTB* GetPrev (CTokenStreamPos& pos);

	CString GetStringAt (CTokenStreamPos &pos);

// Implementation.
protected:
	CTokenLineArray m_rgTokenLine;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
};		

/////////////////////////////////////////////////////////////////////////////
#endif // __TOKENS_H__
