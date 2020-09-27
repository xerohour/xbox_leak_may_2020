// property.cpp : Worker routines for property pages.
//

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


//
// This single static buffer is used to format and return a string from
// several routines.  This means that the caller of these routines
// must make a copy of the returned string before calling one of
// the formatting functions again.
//
static CHAR szStringBuffer[128];


LPSTR FormatHex(DWORD dwValue)
{
	sprintf(szStringBuffer, "%08lX", dwValue);
	return szStringBuffer;
}

LPSTR FormatHandle(HANDLE handle)
{
	if (handle)
	{
		return FormatHex((DWORD)handle);
	}
	else
	{
		return ids(IDS_NONE_NOSPACE);
	}
}

LPSTR FormatInt(INT iValue)
{
	_itoa(iValue, szStringBuffer, 10);
	return szStringBuffer;
}

LPSTR FormatULong(ULONG ulValue)
{
	sprintf(szStringBuffer, "%lu", ulValue);
	return szStringBuffer;
}

LPSTR FormatKb(DWORD dwValue)
{
	sprintf(szStringBuffer, "%lu (%lu Kb)", dwValue, dwValue / 1024);
	return szStringBuffer;
}

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

LPSTR FormatKb(LARGE_INTEGER liValue)
{
	double fValue = Li2Double(liValue);

	sprintf(szStringBuffer, "%.0f (%.0f Kb)", fValue, fValue / 1024);
	return szStringBuffer;
}

LPSTR FormatTime(LARGE_INTEGER liTime)
{
	int msec, sec, min, hr;

    double f = Li2Double(liTime)/1.0E7/3600;

    hr = (int)f;

    f = f - hr;
    min = (int)(f = f * 60);

    f = f - min;
    sec = (int)(f = f * 60);

    f = f - sec;
    msec = (int)(f * 1000);

	_stprintf(szStringBuffer, "%d:%02d:%02d.%03d", hr, min, sec, msec);

	return szStringBuffer;
}

LPSTR FormatTime(FILETIME ftTime)
{
	return(ids(IDS_UNAVAILABLE));

	UNREFERENCED_PARAMETER(ftTime);
}

LPSTR FormatTimeDiff(LARGE_INTEGER liTimeBegin, LARGE_INTEGER liTimeEnd)
{
	int msec, sec, min, hr;

    double fBegin = Li2Double(liTimeBegin)/1.0E7/3600;
    double fEnd   = Li2Double(liTimeEnd)/1.0E7/3600;

	double f = fEnd - fBegin;

	if (f == fEnd)
		return(ids(IDS_UNAVAILABLE));

    hr = (int)f;

    f = f - hr;
    min = (int)(f = f * 60);

    f = f - min;
    sec = (int)(f = f * 60);

    f = f - sec;
    msec = (int)(f * 1000);

	_stprintf(szStringBuffer, "%d:%02d:%02d.%03d", hr, min, sec, msec);

	return szStringBuffer;
}

LPSTR FormatTimeDiff(SYSTEMTIME stTime, FILETIME ftTime)
{
	return(ids(IDS_UNAVAILABLE));

	UNREFERENCED_PARAMETER(ftTime);
	UNREFERENCED_PARAMETER(stTime);
}

LPSTR FormatTimeAdd(FILETIME ftTime1, FILETIME ftTime2)
{
	return(ids(IDS_UNAVAILABLE));

	UNREFERENCED_PARAMETER(ftTime1);
	UNREFERENCED_PARAMETER(ftTime2);
}

LPSTR FormatWord(WORD w)
{
	sprintf(szStringBuffer, "%04X", w);
	return szStringBuffer;
}

LPSTR FormatHandleFromTable(HANDLE handle, HANDLESTABLE *pht)
{
	if (!handle)
	{
		return ids(IDS_NONE_NOSPACE);
	}

	while (pht->pszName)
	{
		if (handle == pht->handle)
		{
			return pht->pszName;
		}

		pht++;
	}

	return FormatHex((DWORD)handle);
}

LPSTR FormatULongFromTable(ULONG ulValue, VALUETABLE *pvt)
{
	while (pvt->pszValue)
	{
		if (ulValue == pvt->nValue)
		{
			return pvt->pszValue;
		}

		pvt++;
	}

	return FormatHex(ulValue);
}

void FillLBFromStrList(CListBox* pListBox, CStringList* pStrList)
{
	POSITION pos;

	pListBox->ResetContent();
	pos = pStrList->GetHeadPosition();
	while (pos)
	{
		pListBox->AddString(pStrList->GetNext(pos));
	}
}

void FillCBFromStrList(CComboBox* pComboBox, CStringList* pStrList)
{
	POSITION pos;

	pComboBox->ResetContent();
	pos = pStrList->GetHeadPosition();
	while (pos)
	{
		pComboBox->AddString(pStrList->GetNext(pos));
	}

	pComboBox->SetCurSel(0);
}

MSGTYPE GetMsgType(UINT msg)
{
	//
	// Is this message in the reserved system range?
	//
	if (msg < MAX_MESSAGES)
	{
		//
		// Do we find it in our lookup table?
		//
		if (CMsgDoc::m_apmdLT[msg])
		{
			return MT_NORMAL;
		}
		else
		{
			//
			// Not found in our table.  Must not be documented.
			//
			return MT_UNDOCUMENTED;
		}
	}
	else
	{
		//
		// Is it a registered message (0xC000 - 0xFFFF)?
		//
		if (msg >= MIN_REGISTEREDMSG)
		{
			return MT_REGISTERED;
		}
		else
		{
			//
			// Must be a WM_USER message.
			//
			return MT_USER;
		}
	}
}

MSGTYPE GetMsgType(UINT msg, HWND hwnd)
{
	//
	// Is this message in the reserved system range?
	//
	if (msg < MAX_MESSAGES)
	{
		//
		// Do we find it in our lookup table?
		//
		if (CMsgDoc::m_apmdLT[msg])
		{
			return MT_NORMAL;
		}
		else
		{
			//
			// Not found in our table.  Must not be documented.
			//
			return MT_UNDOCUMENTED;
		}
	}
	else
	{
		//
		// Is it a registered message (0xC000 - 0xFFFF)?
		//
		if (msg >= MIN_REGISTEREDMSG)
		{
			return MT_REGISTERED;
		}
		else
		{
			CHAR szClass[128];

			GetClassName(hwnd, szClass, sizeof(szClass));

			if (_tcscmp(szClass, "#32770") == 0)
			{
				switch(msg)
				{
					case DM_GETDEFID:
					case DM_SETDEFID:
						return MT_DIALOG;

					default:
						return MT_USER;
				}
			}
#ifndef DISABLE_WIN95_MESSAGES
			else if (_tcsicmp(szClass, "SysHeader32") == 0)
			{
				switch(msg)
				{
					case HDM_GETITEMCOUNT:
					case HDM_INSERTITEM:
					case HDM_DELETEITEM:
					case HDM_GETITEM:
					case HDM_SETITEM:
					case HDM_LAYOUT:
					case HDM_HITTEST:
						return MT_HEADER;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "ToolbarWindow32") == 0)
			{
				switch(msg)
				{
					case TB_ENABLEBUTTON:
					case TB_CHECKBUTTON:
					case TB_PRESSBUTTON:
					case TB_HIDEBUTTON:
					case TB_INDETERMINATE:
					case TB_ISBUTTONENABLED:
					case TB_ISBUTTONCHECKED:
					case TB_ISBUTTONPRESSED:
					case TB_ISBUTTONHIDDEN:
					case TB_ISBUTTONINDETERMINATE:
					case TB_SETSTATE:
					case TB_GETSTATE:
					case TB_ADDBITMAP:
					case TB_ADDBUTTONS:
					case TB_INSERTBUTTON:
					case TB_DELETEBUTTON:
					case TB_GETBUTTON:
					case TB_BUTTONCOUNT:
					case TB_COMMANDTOINDEX:
					case TB_SAVERESTORE:
					case TB_CUSTOMIZE:
					case TB_ADDSTRING:
					case TB_GETITEMRECT:
					case TB_BUTTONSTRUCTSIZE:
					case TB_SETBUTTONSIZE:
					case TB_SETBITMAPSIZE:
					case TB_AUTOSIZE:
					case TB_SETBUTTONTYPE:
					case TB_GETTOOLTIPS:
					case TB_SETTOOLTIPS:
					case TB_SETPARENT:
					case TB_SETROWS:
					case TB_GETROWS:
					case TB_SETCMDID:
					case TB_CHANGEBITMAP:
					case TB_GETBITMAP:
					case TB_GETBUTTONTEXT:
						return MT_TOOLBAR;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "tooltips_class32") == 0)
			{
				switch(msg)
				{
					case TTM_ACTIVATE:
					case TTM_SETDELAYTIME:
					case TTM_ADDTOOL:
					case TTM_DELTOOL:
					case TTM_NEWTOOLRECT:
					case TTM_RELAYEVENT:
					case TTM_GETTOOLINFO:
					case TTM_SETTOOLINFO:
					case TTM_HITTEST:
					case TTM_GETTEXT:
					case TTM_UPDATETIPTEXT:
					case TTM_GETTOOLCOUNT:
					case TTM_ENUMTOOLS:
					case TTM_GETCURRENTTOOL:
						return MT_TOOLTIP;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "msctls_statusbar32") == 0)
			{
				switch(msg)
				{
					case SB_SETTEXT:
					case SB_GETTEXT:
					case SB_GETTEXTLENGTH:
					case SB_SETPARTS:
					case SB_GETPARTS:
					case SB_GETBORDERS:
					case SB_SETMINHEIGHT:
					case SB_SIMPLE:
					case SB_GETRECT:
						return MT_STATUSBAR;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "msctls_trackbar32") == 0)
			{
				switch(msg)
				{
					case TBM_GETPOS:
					case TBM_GETRANGEMIN:
					case TBM_GETRANGEMAX:
					case TBM_GETTIC:
					case TBM_SETTIC:
					case TBM_SETPOS:
					case TBM_SETRANGE:
					case TBM_SETRANGEMIN:
					case TBM_SETRANGEMAX:
					case TBM_CLEARTICS:
					case TBM_SETSEL:
					case TBM_SETSELSTART:
					case TBM_SETSELEND:
					case TBM_GETPTICS:
					case TBM_GETTICPOS:
					case TBM_GETNUMTICS:
					case TBM_GETSELSTART:
					case TBM_GETSELEND:
					case TBM_CLEARSEL:
					case TBM_SETTICFREQ:
					case TBM_SETPAGESIZE:
					case TBM_GETPAGESIZE:
					case TBM_SETLINESIZE:
					case TBM_GETLINESIZE:
					case TBM_GETTHUMBRECT:
					case TBM_GETCHANNELRECT:
					case TBM_SETTHUMBLENGTH:
					case TBM_GETTHUMBLENGTH:
						return MT_TRACKBAR;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "msctls_updown") == 0)
			{
				switch(msg)
				{
					case UDM_SETRANGE:
					case UDM_GETRANGE:
					case UDM_SETPOS:
					case UDM_GETPOS:
					case UDM_SETBUDDY:
					case UDM_GETBUDDY:
					case UDM_SETACCEL:
					case UDM_GETACCEL:
					case UDM_SETBASE:
					case UDM_GETBASE:
						return MT_UPDOWN;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "msctls_progress32") == 0)
			{
				switch(msg)
				{
					case PBM_SETRANGE:
					case PBM_SETPOS:
					case PBM_DELTAPOS:
					case PBM_SETSTEP:
					case PBM_STEPIT:
						return MT_PROGRESS;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "msctls_hotkey32") == 0)
			{
				switch(msg)
				{
					case HKM_SETHOTKEY:
					case HKM_GETHOTKEY:
					case HKM_SETRULES:
						return MT_HOTKEY;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "SysListView32") == 0)
			{
				switch(msg)
				{
					case LVM_GETBKCOLOR:
					case LVM_SETBKCOLOR:
					case LVM_GETIMAGELIST:
					case LVM_SETIMAGELIST:
					case LVM_GETITEMCOUNT:
					case LVM_GETITEM:
					case LVM_SETITEM:
					case LVM_INSERTITEM:
					case LVM_DELETEITEM:
					case LVM_DELETEALLITEMS:
					case LVM_GETCALLBACKMASK:
					case LVM_SETCALLBACKMASK:
					case LVM_GETNEXTITEM:
					case LVM_FINDITEM:
					case LVM_GETITEMRECT:
					case LVM_SETITEMPOSITION:
					case LVM_GETITEMPOSITION:
					case LVM_GETSTRINGWIDTH:
					case LVM_HITTEST:
					case LVM_ENSUREVISIBLE:
					case LVM_SCROLL:
					case LVM_REDRAWITEMS:
					case LVM_ARRANGE:
					case LVM_EDITLABEL:
					case LVM_GETEDITCONTROL:
					case LVM_GETCOLUMN:
					case LVM_SETCOLUMN:
					case LVM_INSERTCOLUMN:
					case LVM_DELETECOLUMN:
					case LVM_GETCOLUMNWIDTH:
					case LVM_SETCOLUMNWIDTH:
					case LVM_CREATEDRAGIMAGE:
					case LVM_GETVIEWRECT:
					case LVM_GETTEXTCOLOR:
					case LVM_SETTEXTCOLOR:
					case LVM_GETTEXTBKCOLOR:
					case LVM_SETTEXTBKCOLOR:
					case LVM_GETTOPINDEX:
					case LVM_GETCOUNTPERPAGE:
					case LVM_GETORIGIN:
					case LVM_UPDATE:
					case LVM_SETITEMSTATE:
					case LVM_GETITEMSTATE:
					case LVM_GETITEMTEXT:
					case LVM_SETITEMTEXT:
					case LVM_SETITEMCOUNT:
					case LVM_SORTITEMS:
					case LVM_SETITEMPOSITION32:
					case LVM_GETSELECTEDCOUNT:
					case LVM_GETITEMSPACING:
					case LVM_GETISEARCHSTRING:
						return MT_LISTVIEW;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "SysTreeView32") == 0)
			{
				switch(msg)
				{
					case TVM_INSERTITEM:
					case TVM_DELETEITEM:
					case TVM_EXPAND:
					case TVM_GETITEMRECT:
					case TVM_GETCOUNT:
					case TVM_GETINDENT:
					case TVM_SETINDENT:
					case TVM_GETIMAGELIST:
					case TVM_SETIMAGELIST:
					case TVM_GETNEXTITEM:
					case TVM_SELECTITEM:
					case TVM_GETITEM:
					case TVM_SETITEM:
					case TVM_EDITLABEL:
					case TVM_GETEDITCONTROL:
					case TVM_GETVISIBLECOUNT:
					case TVM_HITTEST:
					case TVM_CREATEDRAGIMAGE:
					case TVM_SORTCHILDREN:
					case TVM_ENSUREVISIBLE:
					case TVM_SORTCHILDRENCB:
					case TVM_ENDEDITLABELNOW:
					case TVM_GETISEARCHSTRING:
						return MT_TREEVIEW;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "SysTabControl32") == 0)
			{
				switch(msg)
				{
					case TCM_GETBKCOLOR:
					case TCM_SETBKCOLOR:
					case TCM_GETIMAGELIST:
					case TCM_SETIMAGELIST:
					case TCM_GETITEMCOUNT:
					case TCM_GETITEM:
					case TCM_SETITEM:
					case TCM_INSERTITEM:
					case TCM_DELETEITEM:
					case TCM_DELETEALLITEMS:
					case TCM_GETITEMRECT:
					case TCM_GETCURSEL:
					case TCM_SETCURSEL:
					case TCM_HITTEST:
					case TCM_SETITEMEXTRA:
					case TCM_ADJUSTRECT:
					case TCM_SETITEMSIZE:
					case TCM_REMOVEIMAGE:
					case TCM_SETPADDING:
					case TCM_GETROWCOUNT:
					case TCM_GETTOOLTIPS:
					case TCM_SETTOOLTIPS:
					case TCM_GETCURFOCUS:
					case TCM_SETCURFOCUS:
						return MT_TABCTRL;

					default:
						return MT_USER;
				}
			}
			else if (_tcsicmp(szClass, "SysAnimate32") == 0)
			{
				switch(msg)
				{
					case ACM_OPEN:
					case ACM_PLAY:
					case ACM_STOP:
						return MT_ANIMATE;

					default:
						return MT_USER;
				}
			}
#endif	// DISABLE_WIN95_MESSAGES
			else
				return MT_USER;
		}
	}
}
