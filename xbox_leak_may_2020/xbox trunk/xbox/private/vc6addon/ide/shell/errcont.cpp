//
//	ERRCONT.CPP	
//
//
// History
// =======
// Date			Who			What
// ----			---			----
// 6-19-93		danwh		Created
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "main.h"
#include "utilbld_.h"
#include "msgboxes.h"

IMPLEMENT_DYNAMIC (CErrorContext, CObject)
IMPLEMENT_DYNAMIC (CStorageEC, CErrorContext)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------- CErrorContext ------------------------------
//
///////////////////////////////////////////////////////////////////////////////
CErrorContext g_DummyEC;
///////////////////////////////////////////////////////////////////////////////
//
// --------------------------------- CStorageEC -------------------------------
//
///////////////////////////////////////////////////////////////////////////////

void CStorageEC::AddString ( const TCHAR *pstr ) 
{
	m_StringList.AddTail (pstr); 	
}
void CStorageEC::AddString ( UINT ResID ) 
{
	CString str;
	if (str.LoadString (ResID)) m_StringList.AddTail (str);
	else TRACE ("CStorageEC::AddString failed to load string %i\n", ResID );
}

void CStorageEC::GetString ( CString &rstr ) 
{
	rstr.Empty ();
	POSITION pos;
	for (pos = m_StringList.GetHeadPosition (); pos != NULL; )
		rstr += m_StringList.GetNext (pos);
}

void CStorageEC::GetRevString ( CString &rstr ) 
{
	rstr.Empty ();
	POSITION pos;
	for (pos = m_StringList.GetTailPosition (); pos != NULL; )
		rstr += m_StringList.GetPrev (pos);
}

int CStorageEC::DoMessageBox ( 
	const MsgBoxTypes MsgBoxType, /*= Error 	 */
	UINT nButtonIDs, 	/* = DEFAULT_BUTTONS */
	UINT nHelpContext)  /* = DEFAULT_HELP	 */
{ 
	CString str;
	GetString (str);
	return MsgBox (MsgBoxType, str, nButtonIDs, nHelpContext);
}

void CStorageEC::Reset () 
{
	m_StringList.RemoveAll ();
}

