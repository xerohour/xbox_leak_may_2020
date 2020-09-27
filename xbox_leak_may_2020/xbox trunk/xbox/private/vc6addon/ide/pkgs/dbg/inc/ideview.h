class CIDEDoc;

////////////////////////////////////////////////////////////
// CIDEView

class CIDEView : public CPartView
{
	DECLARE_DYNCREATE(CIDEView)

public:

	// Constructors, destructors

	CIDEView();
	virtual			~CIDEView() {}

	UINT	m_nID;					// save the window ID for use w/ dockables.

	// methods
	CIDEDoc * 	GetDocument(void) const { return (CIDEDoc*)m_pDocument; }
	UINT		GetDocType(void) const { return GetDocument()->GetDocType(); }
};

