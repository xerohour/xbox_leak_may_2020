#include "stdafx.h"
#include "slob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
void CPropBag::Empty () 
{
	WORD w; CProp* pProp;
	POSITION pos = m_props.GetStartPosition ();
	while (pos) 
	{
		m_props.GetNextAssoc (  pos, w, (void*&)pProp );
		VERIFY(m_props.RemoveKey(w));
		CProp::DeleteCProp (pProp);
	}
};

void CPropBag::Serialize ( CArchive &ar, CSlob *pFilterSlob /*= NULL */)
{
	WORD w; CProp* pProp;
	POSITION pos;
	PROP_TYPE pt;
	DWORD dw;

	if (ar.IsStoring())
	{
		pos = m_props.GetStartPosition ();
		while (pos) 
		{
			m_props.GetNextAssoc (  pos, w, (void*&)pProp );
			if (pFilterSlob
				 && 
				!pFilterSlob->SerializePropMapFilter (w)) continue;
			ar <<  (DWORD) pProp->m_nType << w;
			switch (pProp->m_nType)
			{
				case integer:
					ar << (DWORD) ( (CIntProp *) pProp )->m_nVal;
					break;
				case booln:
					ar << (DWORD) ( (CBoolProp *) pProp )->m_bVal;
					break;
				case number:
					ar << ( (CNumberProp *) pProp )->m_numVal;
					break;
				case string:
					ar << ( (CStringProp *) pProp )->m_strVal;
					break;
				default:
					ASSERT(FALSE);		 		
			}   
		}
		// Put in end of records marker:
		ar << (DWORD) null;
	}
	else
	{
		pProp = NULL;
		TRY
		{
			while (1)
			{
				ar >> dw;
				pt = (PROP_TYPE) dw;

				// Check for end of records:			
				if ( pt == null ) break;	
				ar >> w;

				//  Should never be overwriting props:
				#ifdef _DEBUG
				void *pv;
				ASSERT (!m_props.Lookup (w, pv ));
				#endif
				switch (pt)
				{
					case integer:
						pProp = new CIntProp;
						ar >> dw;
						( (CIntProp *) pProp )->m_nVal = (int) dw;
						break;
					case booln:
						pProp = new CBoolProp;
						ar >> dw;
						( (CBoolProp *) pProp )->m_bVal = (int) dw;
						break;
					case number:
						pProp = new CNumberProp;
						ar >> ( (CNumberProp *) pProp )->m_numVal;
						break;
					case string:
						pProp = new CStringProp;
						ar >> ( (CStringProp *) pProp )->m_strVal;
						break;
					default:
						::AfxThrowArchiveException ( 
											CArchiveException::generic 
											);		 		
				}
				if (pFilterSlob
				 	&& 
					!pFilterSlob->SerializePropMapFilter (w)
					)
				{
					CProp::DeleteCProp (pProp); //Discard
				}
				else
				{
					m_props.SetAt ( w, pProp );
				}
				pProp = NULL;	   
			}
		}
		CATCH (CException, e)
		{
			if (pProp) CProp::DeleteCProp (pProp);
			THROW_LAST ();
		}
		END_CATCH
	}

}

void CPropBag::SetIntProp(CSlob* pSlob, UINT nPropID, int val)
{
	CProp* pProp = FindProp(nPropID);
	if (pProp == NULL)
	{
		if (theUndoSlob.IsRecording())
			theUndoSlob.OnAddProp(pSlob, this, nPropID);

		AddProp(nPropID, new CIntProp(val));
	}
	else
	{
		ASSERT(pProp->m_nType == integer);
		
		if (theUndoSlob.IsRecording())
			theUndoSlob.OnSetIntProp(pSlob, nPropID, ((CIntProp*)pProp)->m_nVal, this);
		
		((CIntProp*)pProp)->m_nVal = val;
	}
}

void CPropBag::SetStrProp(CSlob* pSlob, UINT nPropID, CString & str)
{
	CProp* pProp = FindProp(nPropID);
	if (pProp == NULL)
	{
		if (theUndoSlob.IsRecording())
			theUndoSlob.OnAddProp(pSlob, this, nPropID);

		AddProp(nPropID, new CStringProp(str));
	}
	else
	{
		ASSERT(pProp->m_nType == string);
		
		if (theUndoSlob.IsRecording())
			theUndoSlob.OnSetStrProp(pSlob, nPropID, &((CStringProp*)pProp)->m_strVal, this);
		
		((CStringProp*)pProp)->m_strVal = str;
	}
}

void CPropBag::Clone (CSlob * pSlob, CPropBag * pBag, BOOL fEmpty /*=TRUE*/) 
{
	// empty the 'this' property bag?
	if (fEmpty)	Empty();

	ASSERT(pBag != NULL);

	POSITION pos = pBag->m_props.GetStartPosition();
	while (pos != NULL)
	{
		WORD id;
		CProp* pProp;
		pBag->m_props.GetNextAssoc(pos, id, (void*&)pProp);
		
		switch (pProp->m_nType)
		{
		case integer:
			SetIntProp(pSlob, id, ((CIntProp*)pProp)->m_nVal);
			break;

		case string:
			SetStrProp(pSlob, id, ((CStringProp*)pProp)->m_strVal);
			break;
		
		default:
			// FUTURE: other property types...
			ASSERT(FALSE);
			break;
		}
	}
}
