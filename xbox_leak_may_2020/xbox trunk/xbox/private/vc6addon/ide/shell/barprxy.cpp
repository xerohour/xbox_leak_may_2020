#include "stdafx.h"

#include "barprxy.h"

#include "barglob.h"
#include "commapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// For Orion, the data storage format is going to have to be radically different. It will
// to accomodates menus and toolbars, including items such as  toolbar buttons, dropdown menus, 
// textual buttons, as well as everything that was supported in 4.0. I guess that we might also 
// like to support migration of toolbars, though given that we've changed the IDs of all the 
// commands, this could be difficult.

// The original version 4.0 code always emitted data blocks that were an exact multiple 
// of 3 WORDs long (x%3 always == 0). [This calculation excludes the size word stored before
// the block, which is not included in the transmitted value] We can use this fact to identify 
// ourselves (and future versions) in an extensible way compatible with the 4.0 scheme by 
// never writing blocks whose length%3 is 1. To make things somewhat simpler for future 
// upgrade schemes, I am going to add a version word to the front of the data block.

// At this point, code to migrate v4x toolbars is not being implemented, but the opening is here
// to add migration code simply by detecting the size of the output data.

// The original (v4x) format was:
// size	  - size of the block in bytes, excluding the size word
// repeat  
//   nID    - command id
//   nStyle - type of toolbar button
//   wExtra - only used for width of sizeable hwnd controls
// end repeat

// The Orion format is under development, and will be amended as more features are added.
// The current version is:
// size         - size of the block, in bytes, excluding the size word
// version word - always cBarDataVersion50
// count    - number of buttons on the bar
// offset to start of string table (*1) from start of version word, in words
// offset to start of bitmap table (*2) from start of version word, in words
// offset to start of addin/macro table (*3) from start of version word, in words
// repeat
//   nID    - command id or menu id - 0 implies separator, or addin info
//   nStyle - type of toolbar button
//   wExtra - used for width of sizeable hwnd controls
//			           textLabel table index of textual controls
// end repeat
// *1
// string table (ordered as they appear in the bar)
// repeat
//   string in wide character form
// end repeat
// *2
// DIB table (ordered as they appear in the bar)
// repeat
//   WORD: dib data size
//   DIB data
// end repeat
// *3
// addin info table
// repeat
//   WORD: id of package responsible for command
//   WORD: addin info size
//   addin info
// end repeat
// 00 padding to ensure block size criteria listed above

// FUTURE: Perhaps consider using MFC serialisation for this in next major version. We just
// extended the same storage format when moving from v4 to v5; this allowed us to easily
// play version detection tricks with the size of blocks, for example. But clearly, this kind of
// pointer/access code is neither very reliable nor very readable; probably next time round
// we'll need to replace it.

// martynl 10May96, revised 25Jun96

// These formats were used in some earlier checked in code
const WORD cBarDataVersionPre50a=1; // before DIB saving, and re-org
const WORD cBarDataVersionPre50b=2; // before TBBS_GLYPH
const WORD cBarDataVersionPre50c=3; // before addins and macros
const WORD cBarDataVersionPre50d=4; // before TBBS_CUSTOMTEXT
// The current format
const WORD cBarDataVersion50=5;
const WORD cBarDataVersionCurrent=cBarDataVersion50;

// CASBar:SetData
// The SetData function transfers data from the hglobal to the bar, by decoding it. It's used
// for loading. Note that loading of toolbars is a 2 phase process with this and DynSetButtons
BOOL CToolBarProxy::SetData(HGLOBAL hglob)
{
	BOOL bSomeDeleted=FALSE;	// true if something was deleted during loading
	BOOL bSomeAdded=FALSE;		// true if something was loaded

	// empty toolbar, to ensure that what goes
	// in here is not confused
	Empty();
	
	WORD FAR* lpWord = (WORD FAR*) ::GlobalLock(hglob); // this is the offset into the decoded data
	
	// extract size of decoded data
	WORD wSize	= *lpWord++;

	// real block starts here
	WORD FAR* lpBase = lpWord;
	WORD nVersion = *lpWord++;

	// ensure it's a VC 5 toolbar
	ASSERT(((wSize/sizeof(WORD)) % 3)!=0);
	if(nVersion != cBarDataVersionCurrent || ((wSize/sizeof(WORD)) % 3)==0)
	{
		// V4 migration code would go here, if we were motivated.
		return FALSE;
	}

	int nCount = *lpWord++;

	// extract pointers to middle of table for bitmaps, strings, and ICommandProvider stuff
	WORD FAR* lpString= lpBase+(*lpWord++);
	int nString=0;
	WORD FAR* lpBitmap= lpBase+(*lpWord++);
	WORD FAR* lpExternal= lpBase+(*lpWord++);
	
	ASSERT((lpString-lpBase)<=wSize);
	ASSERT((lpBitmap-lpBase)<=wSize);
	ASSERT((lpExternal-lpBase)<=wSize);

	BatchBegin(nCount);

    for (int iButton = 0; iButton < nCount; iButton++)
    {
		int nID = *lpWord++;
		int nStyle = *lpWord++;
		int nExtra = *lpWord++;
		CString text;
		HBITMAP hbmGlyph=NULL;
		if(nStyle & TBBS_CUSTOMTEXT)
		{
			text=((LPCTSTR)lpString);
			lpString=(WORD*)(((LPCTSTR)lpString)+text.GetLength()+1);
		}
		if(nStyle & TBBS_CUSTOMBITMAP)
		{
			WORD wBmpSize=*lpBitmap++;
			hbmGlyph=GLOBAL_DATA::LoadSysColorBitmap((LPBITMAPINFOHEADER)lpBitmap);
			lpBitmap=(LPWORD)(((LPBYTE)lpBitmap)+wBmpSize);
		}
		if(	nID==0 &&						// not a regular command 
			(nStyle & TBBS_SEPARATOR)==0)	// not a separator
		{
			// need to get ICommandProvider to decode data
			WORD wPackage=*lpExternal++;
			ASSERT(wPackage>0);

			if(wPackage>0)
			{
				WORD wDataSize=*lpExternal++;
				// find the package for this command
				CPackage *pPackage=theApp.GetPackage(wPackage);
				if(pPackage!=NULL)
				{
					
					ICommandProvider *pCP=pPackage->GetCommandProvider();

					if(pCP!=NULL)
					{
						// decode the saved data into a command id
						nID=pCP->LoadCommand(lpExternal, wDataSize);
						if(nID==0)
						{
							bSomeDeleted=TRUE;
							nStyle=TBBS_INVISIBLE;
						}

						pCP->Release();
					}
					else
					{
						// package is refusing to provide ICP interface, which it should do since data was saved that way
						ASSERT(FALSE);
						bSomeDeleted=TRUE;
						nID=0;
						nStyle=TBBS_INVISIBLE;
					}
				}
				else
				{
					bSomeDeleted=TRUE;
					nID=0;
					nStyle=TBBS_INVISIBLE;
				}

				// skip the data
				lpExternal=(LPWORD)(((LPBYTE)lpExternal)+wDataSize);
			}
			else
			{
				// if the package was 0, this is proabably a corrupted data stream, so quit
				break;
			}

		}

		if(nID!=0)
		{
			bSomeAdded=TRUE;
		}
		
		BatchAdd(iButton, nID, nStyle, nExtra, hbmGlyph, text);
    }

	BatchEnd();

	// At this point, all of the custom bitmaps have glyphs in the extra data. Some of these
	// should be transferred to the well and destroyed later on.

	ASSERT(lpString-lpBase<=wSize);
	ASSERT(lpBitmap-lpBase<=wSize);
	ASSERT(lpExternal-lpBase<=wSize);

	// ignore any potential padding
	ASSERT(lpWord-lpBase<=wSize);

	::GlobalUnlock(hglob);

	if(!bSomeAdded && bSomeDeleted)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

// CASBar::GetData
// This is for saving the bar to the registry. It's quite memory intensive, unfortunately, 
// as each custom bar glyph must be converted to a DIB.
HGLOBAL CToolBarProxy::GetData()
{
	if (GetCount() == 0)
		return NULL;

	// We need to create the dib we're going to save from the custom bitmaps in the bar. Sadly, 
	// Since GetDIBits can only subsegment by scanlines, it isn't possible to act on a toolbar image well
	// and cut out the middle step and go straight from bitmaps within the toolbar image well and DIBs.
	// Instead we have to start by acquiring an individual button glyph to ensure that we have one
	// ddb for each custom button

	HGLOBAL *rgDibs=NULL;

	rgDibs=new HGLOBAL[GetCount()];
	ASSERT(rgDibs);

	for(int iButton=0; iButton < GetCount(); ++iButton) 
	{
		UINT nStyle;
		int iImage;
		UINT nID;

		GetButtonInfo(iButton, nID, nStyle, iImage);

		rgDibs[iButton]=NULL;

		if((nStyle & TBBS_CUSTOMBITMAP)!=0)
		{
			HBITMAP hbmSave=CreateSingleBitmap(iButton);

			rgDibs[iButton]=GLOBAL_DATA::SaveSysColorBitmap(hbmSave);

			ASSERT(GlobalSize(rgDibs[iButton])<USHRT_MAX);

			DestroySingleBitmap(iButton,hbmSave);
		}
	}

	// calculate size of block
	DWORD dwSize;						// start out by calculating size in a dword, so
										// we can spot overflows.
	dwSize=5*sizeof(WORD);				// version and count, plus 3 offsets
	DWORD dwButtonSize=GetCount()*sizeof(WORD)*3;	// the actual buttons
	dwSize+=dwButtonSize;

	// size of all the textual button labels
	BITMAP bmpData;

	DWORD dwStringSize=0;
	DWORD dwDIBSize=0;
	DWORD dwExternalSize=0;
	for(iButton=0; iButton < GetCount(); ++iButton)
	{
		UINT nStyle;
		int iImage;
		UINT nID;

		GetButtonInfo(iButton, nID, nStyle, iImage);

		if(nStyle & TBBS_CUSTOMTEXT)
		{
			dwStringSize+=(GetButtonText(iButton).GetLength()+1)*sizeof(TCHAR);
		}

		if(nStyle & TBBS_CUSTOMBITMAP)
		{
			// size of the following info
			dwDIBSize+=sizeof(WORD);

			// work out size of bits
			ASSERT(rgDibs[iButton]!=NULL);

			dwDIBSize+=GlobalSize(rgDibs[iButton]);
		}

		// if it's not a separator, it might have information of its own to save
		if((nStyle & TBBS_SEPARATOR)==0)
		{
			CTE *pCTE=theCmdCache.GetCommandEntry(nID);

			// find the package
			CPackage *pPackage=NULL;

			if(	pCTE &&
				pCTE->GetFirstPack() &&
				(pPackage=pCTE->GetFirstPack()->GetPackage())!=NULL)
			{
				// now get the appropriate interface
				ICommandProvider *pCP=pPackage->GetCommandProvider();
				if(pCP)
				{
					WORD wExternalSize=pCP->SaveCommand((WORD)nID, NULL);
					
					// always word align external blocks
					if(wExternalSize & 1)
					{
						wExternalSize+=1;
					}

					if(wExternalSize>0)
					{
						dwExternalSize+=sizeof(WORD); // package id
						dwExternalSize+=sizeof(WORD); // size of info
						dwExternalSize+=wExternalSize;
					}

					pCP->Release();
				}
			}
		}
	}
	// word align after strings
	if(dwStringSize & 1)
	{
		dwStringSize++;
	}

	WORD wStringOffset=(WORD)((dwSize)/sizeof(WORD));
	WORD wBitmapOffset=(WORD)((dwSize+dwStringSize)/sizeof(WORD));
	WORD wExternalOffset=(WORD)((dwSize+dwStringSize+dwDIBSize)/sizeof(WORD));

	dwSize+=dwStringSize+dwDIBSize+dwExternalSize;

	// ensure size is a multiple of WORD
	if(dwSize%2 == 1)
	{
		dwSize+=1;
	}
	
	// ensure the size couldn't be the size of a version 4.0 block
	if(((dwSize/sizeof(WORD)) % 3)==0)
	{
		dwSize+=sizeof(WORD);
	}

	if(dwSize>USHRT_MAX)
	{
		// Ugh! More than 64k of data expected.

		// clean up all the spare DIBs
		for (int iButton = 0; iButton < GetCount(); iButton++)
		{
			if(rgDibs[iButton]!=NULL)
			{
				GlobalFree(rgDibs[iButton]);
			}
		}
		delete rgDibs;

		return NULL;
	}

	WORD wSize=(WORD)dwSize;

	HGLOBAL hglob = ::GlobalAlloc(GMEM_SHARE, wSize + sizeof(WORD));
	WORD FAR* lpWord = (WORD FAR*) ::GlobalLock(hglob);
	
	*lpWord++ = wSize;
	LPWORD lpBase=lpWord;
	*lpWord++ = cBarDataVersionCurrent;
	*lpWord++ = (WORD)GetCount();

	*lpWord++ = wStringOffset;	
	WORD FAR *lpString = lpBase+wStringOffset; // where to store the offset to string
	*lpWord++ = wBitmapOffset;	
	WORD FAR *lpBitmap = lpBase+wBitmapOffset; // where to store the offset to bitmap
	*lpWord++ = wExternalOffset;	
	WORD FAR *lpExternal = lpBase+wExternalOffset; // where to store the offset to bitmap

	// write basic button data
    for (iButton = 0; iButton < GetCount(); iButton++)
    {
		UINT nStyle;
		int iImage;
		UINT nID;

		GetButtonInfo(iButton, nID, nStyle, iImage);

		// if it's not a separator, it might have information of its own to save
		if((nStyle & TBBS_SEPARATOR)==0)
		{
			CTE *pCTE=theCmdCache.GetCommandEntry(nID);

			// find the package
			CPackage *pPackage=NULL;

			if(	pCTE &&
				pCTE->GetFirstPack() &&
				(pPackage=pCTE->GetFirstPack()->GetPackage())!=NULL)
			{
				// now get the appropriate interface
				ICommandProvider *pCP=pPackage->GetCommandProvider();
				if(pCP)
				{
					// write external data past header stuff
					WORD wExternalSize=pCP->SaveCommand((WORD)nID, lpExternal+2);
					
					// always word align external blocks
					if(wExternalSize & 1)
					{
						wExternalSize+=1;
					}

					if(wExternalSize>0)
					{
						// write external data header
						*lpExternal++=(WORD)pPackage->PackageID();
						*lpExternal++=wExternalSize;
						lpExternal=(LPWORD)(((LPBYTE)lpExternal)+wExternalSize);

						nID=0;
					}

					pCP->Release();
				}
			}
		}

		*lpWord++ = (WORD) nID;
		// Never save invisible style
		*lpWord++ = (WORD) (nStyle & ~TBBS_INVISIBLE);

		WORD wExtra = 0; // extra is non-zero for windows, where it contains width
    	if (nStyle & TBBS_HWND)
    	{
    		CRect rect;
    		::GetWindowRect(GetButtonControl(iButton), rect);
		
    		wExtra = (WORD) rect.Width();
    	}
		*lpWord++ = wExtra;

		// save strings for each textual button
		if(nStyle & TBBS_CUSTOMTEXT)
		{
			_tcscpy((LPTSTR)lpString,GetButtonText(iButton));
			// nasty casts- advance word pointer in bytes
			lpString=(WORD*)(((LPCTSTR)lpString)+(GetButtonText(iButton).GetLength()+1));
		}

		// save all the DIBs.
		if(nStyle & TBBS_CUSTOMBITMAP)
		{
			ASSERT(rgDibs[iButton]!=NULL);

			*lpBitmap++=(WORD)GlobalSize(rgDibs[iButton]);

			memcpy(lpBitmap, rgDibs[iButton], GlobalSize(rgDibs[iButton]));

			lpBitmap=(LPWORD)(((LPBYTE)lpBitmap)+GlobalSize(rgDibs[iButton]));

			GlobalFree(rgDibs[iButton]);
		}

    }

	delete rgDibs;
	
	ASSERT(((LPBYTE)lpWord)-((LPBYTE)lpBase)<=wSize);
	ASSERT(((LPBYTE)lpString)-((LPBYTE)lpBase)<=wSize);
	ASSERT(((LPBYTE)lpBitmap)-((LPBYTE)lpBase)<=wSize);
	ASSERT(((LPBYTE)lpExternal)-((LPBYTE)lpBase)<=wSize);

	// pad with 0s
	while(((LPBYTE)lpExternal)-((LPBYTE)lpBase)<wSize)
	{
		*lpExternal++=0;
	}
	
	::GlobalUnlock(hglob);

	return hglob;
}

