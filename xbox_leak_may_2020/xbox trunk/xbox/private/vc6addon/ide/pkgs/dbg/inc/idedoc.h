class CIDEView;


////////////////////////////////////////////////////////////
//	CIDEDoc

class CIDEDoc : public CPartDoc
{
	DECLARE_DYNCREATE(CIDEDoc);

public:
	CIDEDoc() {};
	~CIDEDoc() {};

	// data
public:
	CIDEView *	m_pFirstView;

private:
	UINT		m_docType;

	// methods
public:
	UINT 	GetDocType(void) { return m_docType; }
	void	SetDocType(UINT docType) { m_docType = docType; }

#ifndef NO_VBA
	DECLARE_DISPATCH_MAP();
#endif	// NO_VBA
};


