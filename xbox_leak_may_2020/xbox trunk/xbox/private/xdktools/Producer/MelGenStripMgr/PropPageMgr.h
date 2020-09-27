// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

#include <StaticPropPageManager.h>

class PropPageMelGen;
class PropPagePlay;
class PropPageConRule;
class PropPageCommand;
class CPropMelGen;

class CMelGenPropPageMgr : CStaticPropPageManager
{
	friend PropPageMelGen;
	friend PropPagePlay;
	friend PropPageConRule;
	friend PropPageCommand;
public:
	CMelGenPropPageMgr(IDMUSProdFramework* pIFramework);
	~CMelGenPropPageMgr();

	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
 
	// IDMUSProdPropPageManager methods
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Additional methods
	void UpdateObjectWithMelGenData();
	template <class T>
	void RefreshPropPageObject(T* pPropPageObject, CPropMelGen* pMelGen)
	{
		if( pPropPageObject )
		{
			pPropPageObject->m_fMultipleMelGensSelected = FALSE;
			pPropPageObject->m_fHaveData = FALSE;

			if( pMelGen )
			{
				if( pMelGen->m_dwMeasure == 0xFFFFFFFF )
				{
					pPropPageObject->m_fMultipleMelGensSelected = TRUE;
				}
				else
				{
					pPropPageObject->m_fHaveData = TRUE;
				}
				pPropPageObject->CopyDataToMelGen( pMelGen );
			}
			else
			{
				CPropMelGen MelGen;
				pPropPageObject->CopyDataToMelGen( &MelGen );
			}

			pPropPageObject->UpdateControls();
		}
	}


private:
	IDMUSProdFramework*		m_pIFramework;
	PropPageMelGen*			m_pPropPageMelGen;
//	PropPagePlay*			m_pPropPagePlay;	Not supported in DX8
	PropPageConRule*		m_pPropPageConRule;
	PropPageCommand*		m_pPropPageCommand;
};

#endif // __PROPPAGEMGR_H_