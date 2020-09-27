#include "stdafx.h"
#include "imeutil.h"
#include "cmduiena.h"
#include "cmdcache.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


CObList CKeymap::c_keymaps;
CKeymap* CKeymap::c_pChordKeymap = NULL;
WORD CKeymap::c_kcChordPrefix = 0;
BOOL CKeymap::c_bDisableIME = FALSE;
BOOL CKeymap::c_bEscIsAltPrefix = FALSE;
BOOL CKeymap::c_bImplyAltForNextKey = FALSE;

// These strings are not localized...
const TCHAR CKeymap::c_szCtrl [] = "Ctrl+";
const TCHAR CKeymap::c_szAlt [] = "Alt+";
const TCHAR CKeymap::c_szShift [] = "Shift+";


CKeymap::CKeymap()
	: CMapWordToPtr()
{
	m_pPacket = NULL;
	m_bDirty = FALSE;

	c_keymaps.AddTail(this);
}

CKeymap::~CKeymap()
{
	POSITION pos = c_keymaps.Find(this);
	ASSERT(pos != NULL);
	c_keymaps.RemoveAt(pos);

	if (this == c_pChordKeymap)
		c_pChordKeymap = NULL;
	
	RemoveAll();
}

void CKeymap::RemoveAll()
{
	POSITION pos = GetStartPosition();
	while (pos != NULL)
	{
		WORD kc;
		UINT nCmdID;
		CKeymap* pKeymap;
		GetNextKey(pos, kc, nCmdID, pKeymap);

		if ((kc & KCF_CHORD) != 0)
			delete pKeymap;
	}
	
	CMapWordToPtr::RemoveAll();
}

void CKeymap::LoadAccelerators(HINSTANCE hInstance, LPCTSTR lpResourceID)
{
	HACCEL hAccelTable = ::LoadAccelerators(hInstance, lpResourceID);
	ASSERT(hAccelTable != NULL);
	LoadAccelerators(hAccelTable);
	DestroyAcceleratorTable(hAccelTable);
}

void CKeymap::LoadAccelerators(HACCEL hAccelTable)
{
	int nAccels = CopyAcceleratorTable(hAccelTable, NULL, 0); // get size
	ACCEL* pAccelTable = new ACCEL[nAccels];
	CopyAcceleratorTable(hAccelTable, pAccelTable, nAccels);

	for (int i = 0; i < nAccels; i += 1)
	{
		WORD kc = pAccelTable[i].key;
		ASSERT((kc & 0xff) == kc);
		if ((pAccelTable[i].fVirt & FVIRTKEY) == 0)
			kc |= KCF_ASCII;
		if ((pAccelTable[i].fVirt & FSHIFT) != 0)
			kc |= KCF_SHIFT;
		if ((pAccelTable[i].fVirt & FCONTROL) != 0)
			kc |= KCF_CONTROL;
		if ((pAccelTable[i].fVirt & FALT) != 0)
			kc |= KCF_ALT;

#ifdef _DEBUG
		{
			CKeymap* pKeymap;
			UINT nCmdID;

			CTE *pCTE=theCmdCache.GetCommandEntry(pAccelTable[i].cmd);

			if(pCTE && pCTE->flags & CT_NOKEY)
			{
				TRACE("CKeymap::LoadAccelerators: Questionable: Accelerator attached to NOKEY command: kc=0x%04x cmd=0x%04x,%s\n", kc, nCmdID, pCTE->szCommand);
			}

			if(pCTE && _tcslen(pCTE->szCommand)==0)
			{
				TRACE("CKeymap::LoadAccelerators: Error: Accelerator attached to command without command name: kc=0x%04x cmd=0x%04x\n", kc, nCmdID);
				ASSERT(FALSE);
			}

			if (LookupKey(kc, nCmdID, pKeymap))
			{
				// only report clashes
				if(nCmdID != pAccelTable[i].cmd)
				{
					CTE *pExistingCTE=theCmdCache.GetCommandEntry(nCmdID);
					LPCTSTR pszExistingCommand=pExistingCTE ? pExistingCTE->szCommand : "";
					LPCTSTR pszNewCommand=pCTE ? pCTE->szCommand : "";

					TRACE("CKeymap::LoadAccelerators: Accelerator collision: kc=0x%04x existingcmd=0x%04x,%s newcmd=0x%04x,%s \n", kc, nCmdID, pszExistingCommand, pAccelTable[i].cmd, pszNewCommand);
				}
			}

		}
#endif

		SetKey(kc, pAccelTable[i].cmd);
	}

	delete [] pAccelTable;
}

void CKeymap::LoadEditorAccelerators()
{
	CPacket* pPacket = m_pPacket;
	ASSERT(pPacket != NULL);
	ASSERT(pPacket->m_lpAccelID != NULL);

	LoadAccelerators(pPacket->HInstance(), pPacket->m_lpAccelID);
}

void CKeymap::GetKeyName(WORD kc1, WORD kc2, CString& str)
{
	const char* GetNameFromVK(WORD n);

	CString str2;
	if (kc2 != 0)
	{
		GetKeyName(kc2, 0, str2);
		str2 = ',' + str2;
	}

	if ((kc1 & KCF_ASCII) != 0)
	{
		if (LOBYTE(kc1) < ' ')
		{
			str += c_szCtrl;
			str += (char)(LOBYTE(kc1) + '@');
		}
		else
		{
			str += (char)LOBYTE(kc1);
		}
	}
	else
	{
		if ((kc1 & KCF_CONTROL) != 0)
			str += c_szCtrl;
		if ((kc1 & KCF_ALT) != 0)
			str += c_szAlt;
		if ((kc1 & KCF_SHIFT) != 0)
			str += c_szShift;
		
		const char* szVK = GetNameFromVK(LOBYTE(kc1));
		if (szVK != NULL)
			str += szVK;
		else
			str += (char)LOBYTE(kc1);
	}

	str += str2;
}

void CKeymap::GetNextKey(POSITION& pos, WORD& kc, UINT& nCmdID, CKeymap*& pKeymap)
{
	void* pv;
	GetNextAssoc(pos, kc, pv);

	if ((kc & KCF_CHORD) != 0)
	{
		pKeymap = (CKeymap*)pv;
		nCmdID = 0;
	}
	else
	{
		pKeymap = NULL;
		nCmdID = (UINT)pv;
	}
}

void CKeymap::SetKey(WORD kc, UINT nCmdID)
{
	kc &= ~KCF_CHORD;
	SetAt(kc, (void*)nCmdID);

	CKeymap* pKeymap;
	if (Lookup(kc | KCF_CHORD, (void*&)pKeymap))
	{
		// If it was a chord before...
		CMapWordToPtr::RemoveKey(kc | KCF_CHORD);
		delete pKeymap;
	}
}

void CKeymap::SetKey(WORD kc1, WORD kc2, UINT nCmdID)
{
	kc1 |= KCF_CHORD;

	CMapWordToPtr::RemoveKey(kc1 & ~KCF_CHORD);

	CKeymap* pKeymap;
	if (!Lookup(kc1, (void*&)pKeymap))
	{
		pKeymap = new CKeymap;
		SetAt(kc1, pKeymap);
	}

	pKeymap->SetKey(kc2, nCmdID);
}

void CKeymap::SetKey(WORD kc, CKeymap* pKeymap)
{
	kc |= KCF_CHORD;

	CMapWordToPtr::RemoveKey(kc & ~KCF_CHORD);

	SetAt(kc, pKeymap);
}

void CKeymap::RemoveKey(WORD kc1, WORD kc2 /* = 0*/)
{
	CMapWordToPtr::RemoveKey(kc1 & ~KCF_CHORD);

	CKeymap* pKeymap;
	if (Lookup(kc1 | KCF_CHORD, (void*&)pKeymap))
	{
		// If it was a chord before...

		if (kc2 != 0)
		{
			pKeymap->RemoveKey(kc2);
			if (pKeymap->IsEmpty())
				kc2 = 0; // delete keymap below...
		}

		if (kc2 == 0)
		{
			CMapWordToPtr::RemoveKey(kc1 | KCF_CHORD);
			delete pKeymap;
		}
	}
}

BOOL CKeymap::LookupKey(WORD kc, UINT& nCmdID, CKeymap*& pKeymap)
{
	void* pv;
	if (Lookup(kc & ~KCF_CHORD, pv))
	{
		nCmdID = (UINT)pv;
		pKeymap = NULL;
		return TRUE;
	}

	if (Lookup(kc | KCF_CHORD, pv))
	{
		nCmdID = 0;
		pKeymap = (CKeymap*)pv;
		return TRUE;
	}

	return FALSE;
}

// This hack is to try to keep the old-style key bindings off of the
// menu...  Bindings found in the following table will only appear on
// the menu if the command has no other binding.
//
struct OLDKEY
{
	UINT nCmdID;
	WORD kc;
};

OLDKEY oldkeys [] =
{
	ID_FILE_OPEN,	VK_F12 | KCF_CONTROL,
	ID_FILE_SAVE,	VK_F12 | KCF_SHIFT,
	ID_FILE_SAVE_AS,VK_F12,
	ID_FILE_PRINT,	VK_F12 | KCF_SHIFT | KCF_CONTROL,
	ID_EDIT_CUT,	VK_DELETE | KCF_SHIFT,
	ID_EDIT_COPY,	VK_INSERT | KCF_CONTROL,
	ID_EDIT_PASTE,	VK_INSERT | KCF_SHIFT,
	ID_EDIT_UNDO,	VK_BACK | KCF_ALT,
	ID_EDIT_FIND_DLG, VK_F3 | KCF_ALT,
};

BOOL CKeymap::LookupCommand(UINT nCmdID, WORD& kcOne, WORD& kcTwo, CKeymap* pOverride)
{
	WORD kcFound = 0;
	POSITION pos = GetStartPosition();
	while (pos != NULL)
	{
		WORD kc;
		void* pv;
		GetNextAssoc(pos, kc, pv);

		if ((kc & KCF_CHORD) != 0)
		{
			WORD kcOneTry, kcTwoTry;
			if (((CKeymap*)pv)->LookupCommand(nCmdID, kcOneTry, kcTwoTry))
			{
				UINT nFoo;
				CKeymap* pFoo;
				// we're looking for before ignoring the binding here...
				if (pOverride != NULL && pOverride->LookupKey(kc, nFoo, pFoo))
					continue;

				ASSERT(kcTwoTry == 0);
				kcTwo = kcOneTry;
				kcOne = kc;
				return TRUE;
			}
		}
		else
		{
			if ((UINT)pv == nCmdID)
			{
				if (pOverride != NULL)
				{
					// Found a match, now make sure it's not being overriden...
					UINT nFoo;
					CKeymap* pFoo;
					if (pOverride != NULL && pOverride->LookupKey(kc, nFoo, pFoo) && nFoo != nCmdID)
						continue;
				}
				
				// Make sure we don't use any of the old (CUA-style) key
				// bindings unless we have to...
				for (int i = 0; i < sizeof (oldkeys) / sizeof (OLDKEY); i += 1)
				{
					if (oldkeys[i].nCmdID == nCmdID && oldkeys[i].kc == kc ||
						(kc & KCF_ALT) != 0 && ((kc & 0x00ff) == VK_F1 || (kc & 0x00ff) == VK_F2))
					{
						kcFound = kc;
						kc = 0;
						break;
					}
				}
				
				if (kc != 0)
				{
					kcOne = kc;
					kcTwo = 0;
					return TRUE;
				}
			}
		}
	}
	
	if (kcFound != 0)
	{
		kcOne = kcFound;
		kcTwo = 0;
		return TRUE;
	}

	return FALSE;
}

BOOL CKeymap::TranslateMessage(CWnd* pWnd, MSG* pMsg)
{
	static CKeymap* pChordSource;
	if (c_pChordKeymap != NULL && c_pChordKeymap != this)
	{
		pChordSource = this;
		return c_pChordKeymap->TranslateMessage(pWnd, pMsg);
	}

	WORD kc;

	if (!pWnd->IsWindowEnabled() || !pWnd->IsWindowVisible() || pWnd->IsIconic())
		return FALSE;

	switch (pMsg->message)
	{
	default:
		return FALSE;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (pMsg->wParam == VK_MENU ||
			pMsg->wParam == VK_CONTROL ||
			pMsg->wParam == VK_SHIFT)
		{
			return FALSE;
		}

		kc = (WORD)pMsg->wParam;
		ASSERT((kc & 0xff) == kc);
		if (GetKeyState(VK_SHIFT) < 0)
			kc |= KCF_SHIFT;
		if (GetKeyState(VK_CONTROL) < 0)
			kc |= KCF_CONTROL;
		if (GetKeyState(VK_MENU) < 0)
			kc |= KCF_ALT;
		break;

	case WM_CHAR:
	case WM_SYSCHAR:
		kc = (WORD)pMsg->wParam;
		ASSERT((kc & 0xff) == kc);
		kc |= KCF_ASCII;
		if (pMsg->message == WM_SYSCHAR)
			kc |= KCF_ALT;
		break;
	}

	if (c_bEscIsAltPrefix && kc == VK_ESCAPE) // Epsilon emulation...
	{
		if (c_bImplyAltForNextKey)
		{
			CancelChord();
		}
		else
		{
			((CMainFrame*)theApp.m_pMainWnd)->SetTimer(200, 1000 * 1, NULL);
			c_bImplyAltForNextKey = TRUE;
		}
		return TRUE;
	}

	if (c_bImplyAltForNextKey)
	{
		kc |= KCF_ALT;
		CancelChord();
	}

	UINT nCmdID;
	CKeymap* pKeymap;
	if (!LookupKey(kc, nCmdID, pKeymap))
	{
		if (this == c_pChordKeymap)
		{
			if (kc != VK_ESCAPE)
			{
				CMainFrame *pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
				if (pChordSource == &pMainFrame->m_keymap)
				{
					MessageBeep(0);
				}
				else
				{
					// Try the same chord in the main keymap
					if (!pMainFrame->m_keymap.LookupKey(c_kcChordPrefix, nCmdID, pKeymap))
					{
						MessageBeep(0);
					}
					else
					{
						pChordSource = &pMainFrame->m_keymap;
						c_pChordKeymap = pKeymap;
						return c_pChordKeymap->TranslateMessage(pWnd, pMsg);
					}
				}
			}
			CancelChord();
			return TRUE;
		}

		return FALSE;
	}

	if (nCmdID != 0)
	{
		CancelChord();

		// Check to see if the command has been disabled before calling its update handler
		CTE* pCTE = theCmdCache.GetCommandEntry(nCmdID);
		BOOL bEnabled = (pCTE == NULL || (pCTE->flags & CT_NOUI) == 0);
		
		if (bEnabled)
		{
			CCmdUIEnable state;
			state.m_nID = nCmdID;
			state.DoUpdate(AfxGetApp()->m_pMainWnd, TRUE);
			
			bEnabled = state.m_bEnabled;
		}

		if (bEnabled)
		{
			AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND,
				MAKEWPARAM((WORD)nCmdID, 1));
		}
		else
		{
			MessageBeep(0);
		}
	}
	else
	{
		ASSERT(pKeymap != NULL);
		c_pChordKeymap = pKeymap;
		c_kcChordPrefix = kc;

		((CMainFrame*)theApp.m_pMainWnd)->SetTimer(200, 1000 * 1, NULL);

		// Disable the IME so it doesn't eat the last half of the chord
		if (theIME.IsOpen() && theIME.IsEnabled())
		{
			imeEnableIME( NULL, FALSE );
			c_bDisableIME = TRUE;
		}
	}

	return TRUE;
}

void CKeymap::CancelChord()
{
	if (!c_bImplyAltForNextKey && c_pChordKeymap == NULL)
		return;

	c_bImplyAltForNextKey = FALSE;
	c_pChordKeymap = NULL;
	((CMainFrame*)theApp.m_pMainWnd)->KillTimer(200);
	((CMainFrame*)theApp.m_pMainWnd)->KillTimer(201);
	SetPrompt();

	// If we disabled the IME when we started this chord, enable it.
	if (c_bDisableIME)
	{
		imeEnableIME( NULL, TRUE );
		c_bDisableIME = FALSE;
	}
}

void AddGlobalAcceleratorTable(HACCEL hAccel, WORD kcPrefix /*= 0*/)
{
	if (kcPrefix == 0)
	{
		((CMainFrame*)theApp.m_pMainWnd)->m_keymap.LoadAccelerators(hAccel);
	}
	else
	{
		// Chords
		CKeymap* pKeymap = new CKeymap;

		pKeymap->LoadAccelerators(hAccel);
		((CMainFrame*)theApp.m_pMainWnd)->m_keymap.SetKey(kcPrefix, pKeymap);
	}
}

void AddGlobalAcceleratorTable(LPCTSTR lpAccelID, WORD kcPrefix /*= 0*/)
{
	HINSTANCE hInst = AfxFindResourceHandle(lpAccelID, RT_ACCELERATOR);
	ASSERT(hInst != NULL);

	HACCEL hAccelTable = LoadAccelerators(hInst, lpAccelID);
	ASSERT(hAccelTable != NULL);

	AddGlobalAcceleratorTable(hAccelTable, kcPrefix);
	DestroyAcceleratorTable(hAccelTable);
}


BOOL LookupGlobalCommand(UINT nCmdID, WORD& kcOne, WORD& kcTwo)
{
	return ((CMainFrame*)theApp.m_pMainWnd)->m_keymap.LookupCommand(
		nCmdID, kcOne, kcTwo);
}

////////////////////////////////////////////////////////////////////////////
// In the registry, a keymap is represented by a WORD count of bytes
// followed by as many REGKEYMAPPING structures as will fit.

struct REGKEYMAPPING
{
	WORD kc1, kc2;
	UINT nCmdID;
};

const TCHAR szRegKeyboard [] = _T("Keyboard");

void CKeymap::WriteReg(const TCHAR* szKey)
{
	if (!m_bDirty)
		return;
	
	if (szKey == NULL)
		szKey = m_strName;

	CMemFile memRegData;

	// save 2 bytes for size ... will fill in later
	USHORT wZero = 0;
	memRegData.Write(&wZero, sizeof(wZero));

	POSITION pos = GetStartPosition();
	while (pos != NULL)
	{
		WORD kc;
		UINT nCmdID = 0;
		CKeymap* pChordKeymap = NULL;
		GetNextKey(pos, kc, nCmdID, pChordKeymap);

		if ((kc & KCF_CHORD) != 0)
		{
			ASSERT(pChordKeymap != NULL);

			POSITION pos2 = pChordKeymap->GetStartPosition();
			while (pos2 != NULL)
			{
				WORD kc2;
				CKeymap* pFoo;
				pChordKeymap->GetNextKey(pos2, kc2, nCmdID, pFoo);

				REGKEYMAPPING key;
				key.kc1 = kc;
				key.kc2 = kc2;
				key.nCmdID = nCmdID;
				memRegData.Write(&key, sizeof(key));
			}
		}
		else
		{
			REGKEYMAPPING key;
			key.kc1 = kc;
			key.kc2 = 0;
			key.nCmdID = nCmdID;
			memRegData.Write(&key, sizeof(key));
		}
	}

	HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, memRegData.GetLength());
	LPVOID pGlob = GlobalLock(hGlob);

	{
		// get a pointer to memRegData contents and copy to pGlob.
		void *pvRegData, *pvRegDataLim;
		VERIFY(memRegData.GetBufferPtr(CMemFile::bufferCheck, 0, NULL, NULL));	// confirm supported
		memRegData.Seek(0, CFile::begin);
		VERIFY(memRegData.GetBufferPtr(CMemFile::bufferRead, memRegData.GetLength(),
									   &pvRegData, &pvRegDataLim));
		memcpy(pGlob, pvRegData, memRegData.GetLength());
		// no need to release buffer pointers
	}
	ASSERT(memRegData.GetLength() <= 0xffff);		// will never happen -- limited # of keys
	*(WORD *)pGlob = (WORD)memRegData.GetLength();	// fill in size which we left as 0 earlier

	WriteRegData(szRegKeyboard, szKey, hGlob);

	GlobalUnlock(hGlob);
	GlobalFree(hGlob);
}

BOOL CKeymap::GetReg(const TCHAR* szKey)
{
	if (szKey == NULL)
		szKey = m_strName;
	
	HGLOBAL hGlob = GetRegData(szRegKeyboard, szKey, NULL);
	if (hGlob == NULL)
		return FALSE;
	
	// Delete all the old entries...
	RemoveAll();
	
	LPVOID pGlob = GlobalLock(hGlob);
	UNALIGNED REGKEYMAPPING FAR* pKey = (REGKEYMAPPING*)((CHAR*)pGlob + sizeof (WORD));
	UNALIGNED REGKEYMAPPING FAR* pKeyEnd = (REGKEYMAPPING *)((CHAR *)pGlob + *(WORD FAR*)pGlob);
	while (pKey < pKeyEnd)
	{
		UNALIGNED REGKEYMAPPING FAR* pKeyNew;

		pKeyNew = pKey + 1;

		if (pKey->kc2 == 0)
			SetKey(pKey->kc1, pKey->nCmdID);
		else
			SetKey(pKey->kc1, pKey->kc2, pKey->nCmdID);
		
		pKey = pKeyNew;
	}
	GlobalUnlock(hGlob);
	GlobalFree(hGlob);
	return TRUE;
}

void CKeymap::SetEscIsAltPrefix(BOOL bSetEscIsAltPrefix)
{
	c_bEscIsAltPrefix = bSetEscIsAltPrefix;
}


// Active key string for the command
BOOL GetCmdKeyString(UINT nCmdID, CString& str)
{
	WORD kc1, kc2;

	CPartView* pView = (CPartView*)CWnd::FromHandle(theApp.GetActiveView());
	CKeymap* pViewKeymap = NULL;
	if (pView != NULL && pView->IsKindOf(RUNTIME_CLASS(CPartView)))
	{
		pViewKeymap = pView->GetKeymap();
		if (pViewKeymap != NULL && pViewKeymap->LookupCommand(nCmdID, kc1, kc2))
		{
			CKeymap::GetKeyName(kc1, kc2, str);
			return TRUE;
		}
	}

	if (((CMainFrame*)theApp.m_pMainWnd)->m_keymap.LookupCommand(nCmdID, kc1, kc2, pViewKeymap))
	{
		CKeymap::GetKeyName(kc1, kc2, str);
		return TRUE;
	}

	return FALSE;
}

// Any key string for the command (for use in Tips)
// Gets the first one found, which may not be active at any given time.
BOOL GetCmdKeyStringAll(UINT nCmdID, CString& str)
{
	CKeymap *pkm;
	POSITION pos;
	pos = CKeymap::c_keymaps.GetHeadPosition();
	while (pos)
	{
		if ((pkm = (CKeymap*)CKeymap::c_keymaps.GetNext(pos)) != NULL)
		{
			WORD kcOne, kcTwo;
			if (pkm->LookupCommand(nCmdID, kcOne, kcTwo, 0))
			{
				CKeymap::GetKeyName(kcOne, kcTwo, str);
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL COverridingKeymap::LookupKey(WORD kc, UINT& nCmdID, CKeymap*& pKeymap)
{
	if (m_fOverrideActive && CKeymap::LookupKey(kc, nCmdID, pKeymap))
		return TRUE;
	else
		return m_pkeymapBase->LookupKey(kc, nCmdID, pKeymap);
}

BOOL COverridingKeymap::LookupCommand(UINT nCmdID, WORD& kcOne, WORD& kcTwo, CKeymap* pOverride)
{	
	if (m_fOverrideActive && CKeymap::LookupCommand(nCmdID, kcOne, kcTwo, pOverride))
		return TRUE;
	else
		return m_pkeymapBase->LookupCommand(nCmdID, kcOne, kcTwo, pOverride);
}
