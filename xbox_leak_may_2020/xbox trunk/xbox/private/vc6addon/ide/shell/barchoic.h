///////////////////////////////////////////////////////////////////////////////
//	BARCHOIC.H
//      See description in barchoic.cpp
///////////////////////////////////////////////////////////////////////////////

#ifndef __BARCHOIC_H__
#define __BARCHOIC_H__

#ifndef __BARCUST_H__
#include "barcust.h"
#endif

//	class CToolBar;
//		class CCustomBar;
			class CChoiceBar;

/////////////////////////////////////////////////////////////////////////////
// CChoiceBar window

class CToolCustomizer;

class CChoiceBar : public CCustomBar
{
public:
	CToolCustomizer* m_pCustomizer;
    CObArray m_aUsage;

	CChoiceBar();
	~CChoiceBar();

// Construction.	
public:

	virtual ORIENTATION GetOrient() const;
// Operations.
public:
	CToolCustomizer* GetCustomizer();
	void SetCustomizer(CToolCustomizer *);

// Message map functions.
protected:
    //{{AFX_MSG(CChoiceBar)
	//}}AFX_MSG

	DECLARE_DYNAMIC(CChoiceBar)

#ifdef _DEBUG
public:
	void AssertValid() const;
#endif
};

#endif	// __BARCHOIC_H__
