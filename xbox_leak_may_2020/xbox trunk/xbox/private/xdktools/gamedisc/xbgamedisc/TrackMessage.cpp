// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      TrackMessage.cpp
// Contents:  
// Revisions: 6-Nov-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- stdafxiled header file
#include "stdafx.h"
#include <zmouse.h>

#define TRACKMSGS

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct 
{
    UINT uMsg;
    char sz[100];
} sMsgMap;

sMsgMap gs_rgmsgmapNotify[] = {
    //genericnotifications
    {NM_OUTOFMEMORY, "NM_OUTOFMEMORY"},
    {NM_CLICK, "NM_CLICK"},
    {NM_DBLCLK, "NM_DBLCLK"},
    {NM_RETURN, "NM_RETURN"},
    {NM_RCLICK, "NM_RCLICK"},
    {NM_RDBLCLK, "NM_RDBLCLK"},
    {NM_SETFOCUS, "NM_SETFOCUS"},
    {NM_KILLFOCUS, "NM_KILLFOCUS"},
    {NM_CUSTOMDRAW, "NM_CUSTOMDRAW"},
    {NM_HOVER, "NM_HOVER"},
    {NM_NCHITTEST, "NM_NCHITTEST"},
    {NM_KEYDOWN, "NM_KEYDOWN"},
    {NM_RELEASEDCAPTURE, "NM_RELEASEDCAPTURE"},
    {NM_SETCURSOR, "NM_SETCURSOR"},
    {NM_CHAR, "NM_CHAR"},
//    {NM_TOOLTIPSCREATED, "NM_TOOLTIPSCREATED"},
//    {NM_LDOWN, "NM_LDOWN"},
//    {NM_RDOWN, "NM_RDOWN"},
//    {NM_THEMECHANGED, "NM_THEMECHANGED"},

    //Treeviewnotifications
    {TVN_KEYDOWN, "TVN_KEYDOWN"},
    {TVN_SINGLEEXPAND, "TVN_SINGLEEXPAND"},
    {TVN_SELCHANGING, "TVN_SELCHANGING"},
    {TVN_SELCHANGED, "TVN_SELCHANGED"},
    {TVN_GETDISPINFO, "TVN_GETDISPINFO"},
    {TVN_SETDISPINFO, "TVN_SETDISPINFO"},
    {TVN_ITEMEXPANDING, "TVN_ITEMEXPANDING"},
    {TVN_ITEMEXPANDED, "TVN_ITEMEXPANDED"},
    {TVN_BEGINDRAG, "TVN_BEGINDRAG"},
    {TVN_BEGINRDRAG, "TVN_BEGINRDRAG"},
    {TVN_DELETEITEM, "TVN_DELETEITEM"},
    {TVN_BEGINLABELEDIT, "TVN_BEGINLABELEDIT"},
    {TVN_ENDLABELEDIT, "TVN_ENDLABELEDIT"},
    {TVN_GETINFOTIP, "TVN_GETINFOTIP"},

    // header notifications
    {HDN_BEGINDRAG, "HDN_BEGINDRAG"},
    {HDN_ENDDRAG, "HDN_ENDDRAG"},
//    {HDN_FILTERCHANGE, "HDN_FILTERCHANGE"},
//    {HDN_FILTERBTNCLICK, "HDN_FILTERBTNCLICK"},
    {HDN_ITEMCHANGING, "HDN_ITEMCHANGING"},
    {HDN_ITEMCHANGED, "HDN_ITEMCHANGED"},
    {HDN_ITEMCLICK, "HDN_ITEMCLICK"},
    {HDN_ITEMDBLCLICK, "HDN_ITEMDBLCLICK"},
    {HDN_DIVIDERDBLCLICK, "HDN_DIVIDERDBLCLICK"},
    {HDN_BEGINTRACK, "HDN_BEGINTRACK"},
    {HDN_ENDTRACK, "HDN_ENDTRACK"},
    {HDN_TRACK, "HDN_TRACK"},
    {HDN_GETDISPINFO, "HDN_GETDISPINFO"},

    // Rebar notifications
    {RBN_HEIGHTCHANGE, "RBN_HEIGHTCHANGE"},
    {RBN_GETOBJECT, "RBN_GETOBJECT"},
    {RBN_LAYOUTCHANGED, "RBN_LAYOUTCHANGED"},
    {RBN_AUTOSIZE, "RBN_AUTOSIZE"},
    {RBN_BEGINDRAG, "RBN_BEGINDRAG"},
    {RBN_ENDDRAG, "RBN_ENDDRAG"},
    {RBN_DELETINGBAND, "RBN_DELETINGBAND"},
    {RBN_DELETEDBAND, "RBN_DELETEDBAND"},
    {RBN_CHILDSIZE, "RBN_CHILDSIZE"},
//    {RBN_CHEVRONPUSHED, "RBN_CHEVRONPUSHED"},
//    {RBN_MINMAX, "RBN_MINMAX"},
    {RBN_FIRST - 22, "RBN_AUTOBREAK"},
};

sMsgMap gs_rgmmWPCFlags[] = {
    {SWP_DRAWFRAME, "SWP_DRAWFRAME"},
    {SWP_FRAMECHANGED, "SWP_FRAMECHANGED"},
    {SWP_HIDEWINDOW, "SWP_HIDEWINDOW"},
    {SWP_NOACTIVATE, "SWP_NOACTIVATE"},
    {SWP_NOCOPYBITS, "SWP_NOCOPYBITS"},
    {SWP_NOMOVE, "SWP_NOMOVE"},
    {SWP_NOOWNERZORDER, "SWP_NOOWNERZORDER"},
    {SWP_NOREDRAW, "SWP_NOREDRAW"},
//    {SWP_NOREPOSITION, "SWP_NOREPOSITION"},
    {SWP_NOSENDCHANGING, "SWP_NOSENDCHANGING"},
    {SWP_NOSIZE, "SWP_NOSIZE"},
    {SWP_NOZORDER, "SWP_NOZORDER"},
    {SWP_SHOWWINDOW, "SWP_SHOWWINDOW"},
};

sMsgMap gs_rgmsgmap[] = {
    {WM_NULL, "WM_NULL"},
    {WM_CREATE, "WM_CREATE"},
    {WM_DESTROY, "WM_DESTROY"},
    {WM_MOVE, "WM_MOVE"},
    {WM_SIZE, "WM_SIZE"},
    {WM_ACTIVATE, "WM_ACTIVATE"},
    {WM_SETFOCUS, "WM_SETFOCUS"},
    {WM_KILLFOCUS, "WM_KILLFOCUS"},
    {WM_ENABLE, "WM_ENABLE"},
    {WM_SETREDRAW, "WM_SETREDRAW"},
    {WM_SETTEXT, "WM_SETTEXT"},
    {WM_GETTEXT, "WM_GETTEXT"},
    {WM_GETTEXTLENGTH, "WM_GETTEXTLENGTH"},
    {WM_PAINT, "WM_PAINT"},
    {WM_CLOSE, "WM_CLOSE"},
    {WM_QUERYENDSESSION, "WM_QUERYENDSESSION"},
    {WM_QUERYOPEN, "WM_QUERYOPEN"},
    {WM_ENDSESSION, "WM_ENDSESSION"},
    {WM_QUIT, "WM_QUIT"},
    {WM_ERASEBKGND, "WM_ERASEBKGND"},
    {WM_SYSCOLORCHANGE, "WM_SYSCOLORCHANGE"},
    {WM_SHOWWINDOW, "WM_SHOWWINDOW"},
    {WM_WININICHANGE, "WM_WININICHANGE"},
    {WM_SETTINGCHANGE, "WM_SETTINGCHANGE"},
    {WM_DEVMODECHANGE, "WM_DEVMODECHANGE"},
    {WM_ACTIVATEAPP, "WM_ACTIVATEAPP"},
    {WM_FONTCHANGE, "WM_FONTCHANGE"},
    {WM_TIMECHANGE, "WM_TIMECHANGE"},
    {WM_CANCELMODE, "WM_CANCELMODE"},
    {WM_SETCURSOR, "WM_SETCURSOR"},
    {WM_MOUSEACTIVATE, "WM_MOUSEACTIVATE"},
    {WM_CHILDACTIVATE, "WM_CHILDACTIVATE"},
    {WM_QUEUESYNC, "WM_QUEUESYNC"},
    {WM_GETMINMAXINFO, "WM_GETMINMAXINFO"},
    {WM_PAINTICON, "WM_PAINTICON"},
    {WM_ICONERASEBKGND, "WM_ICONERASEBKGND"},
    {WM_NEXTDLGCTL, "WM_NEXTDLGCTL"},
    {WM_SPOOLERSTATUS, "WM_SPOOLERSTATUS"},
    {WM_DRAWITEM, "WM_DRAWITEM"},
    {WM_MEASUREITEM, "WM_MEASUREITEM"},
    {WM_DELETEITEM, "WM_DELETEITEM"},
    {WM_VKEYTOITEM, "WM_VKEYTOITEM"},
    {WM_CHARTOITEM, "WM_CHARTOITEM"},
    {WM_SETFONT, "WM_SETFONT"},
    {WM_GETFONT, "WM_GETFONT"},
    {WM_SETHOTKEY, "WM_SETHOTKEY"},
    {WM_GETHOTKEY, "WM_GETHOTKEY"},
    {WM_QUERYDRAGICON, "WM_QUERYDRAGICON"},
    {WM_COMPAREITEM, "WM_COMPAREITEM"},
//    {WM_GETOBJECT, "WM_GETOBJECT"},
    {WM_COMPACTING, "WM_COMPACTING"},
    {WM_COMMNOTIFY, "WM_COMMNOTIFY"},
    {WM_WINDOWPOSCHANGING, "WM_WINDOWPOSCHANGING"},
    {WM_WINDOWPOSCHANGED, "WM_WINDOWPOSCHANGED"},
    {WM_POWER, "WM_POWER"},
    {WM_COPYDATA, "WM_COPYDATA"},
    {WM_CANCELJOURNAL, "WM_CANCELJOURNAL"},
    {WM_NOTIFY, "WM_NOTIFY"},
    {WM_INPUTLANGCHANGEREQUEST, "WM_INPUTLANGCHANGEREQUEST"},
    {WM_INPUTLANGCHANGE, "WM_INPUTLANGCHANGE"},
    {WM_TCARD, "WM_TCARD"},
    {WM_HELP, "WM_HELP"},
    {WM_USERCHANGED, "WM_USERCHANGED"},
    {WM_NOTIFYFORMAT, "WM_NOTIFYFORMAT"},
    {WM_CONTEXTMENU, "WM_CONTEXTMENU"},
    {WM_STYLECHANGING, "WM_STYLECHANGING"},
    {WM_STYLECHANGED, "WM_STYLECHANGED"},
    {WM_DISPLAYCHANGE, "WM_DISPLAYCHANGE"},
    {WM_GETICON, "WM_GETICON"},
    {WM_SETICON, "WM_SETICON"},
    {WM_NCCREATE, "WM_NCCREATE"},
    {WM_NCDESTROY, "WM_NCDESTROY"},
    {WM_NCCALCSIZE, "WM_NCCALCSIZE"},
    {WM_NCHITTEST, "WM_NCHITTEST"},
    {WM_NCPAINT, "WM_NCPAINT"},
    {WM_NCACTIVATE, "WM_NCACTIVATE"},
    {WM_GETDLGCODE, "WM_GETDLGCODE"},
    {WM_SYNCPAINT, "WM_SYNCPAINT"},
    {WM_NCMOUSEMOVE, "WM_NCMOUSEMOVE"},
    {WM_NCLBUTTONDOWN, "WM_NCLBUTTONDOWN"},
    {WM_NCLBUTTONUP, "WM_NCLBUTTONUP"},
    {WM_NCLBUTTONDBLCLK, "WM_NCLBUTTONDBLCLK"},
    {WM_NCRBUTTONDOWN, "WM_NCRBUTTONDOWN"},
    {WM_NCRBUTTONUP, "WM_NCRBUTTONUP"},
    {WM_NCRBUTTONDBLCLK, "WM_NCRBUTTONDBLCLK"},
    {WM_NCMBUTTONDOWN, "WM_NCMBUTTONDOWN"},
    {WM_NCMBUTTONUP, "WM_NCMBUTTONUP"},
    {WM_NCMBUTTONDBLCLK, "WM_NCMBUTTONDBLCLK"},
    {0x00AB, "WM_NCXBUTTONDOWN"},
    {0x00AC, "WM_NCXBUTTONUP"},
    {0x00AD, "WM_NCXBUTTONDBLCLK"},
    {0x00FF, "WM_INPUT"},
    {WM_KEYFIRST, "WM_KEYFIRST"},
    {WM_KEYDOWN, "WM_KEYDOWN"},
    {WM_KEYUP, "WM_KEYUP"},
    {WM_CHAR, "WM_CHAR"},
    {WM_DEADCHAR, "WM_DEADCHAR"},
    {WM_SYSKEYDOWN, "WM_SYSKEYDOWN"},
    {WM_SYSKEYUP, "WM_SYSKEYUP"},
    {WM_SYSCHAR, "WM_SYSCHAR"},
    {WM_SYSDEADCHAR, "WM_SYSDEADCHAR"},
    {WM_KEYLAST, "WM_KEYLAST"},
    {WM_IME_STARTCOMPOSITION, "WM_IME_STARTCOMPOSITION"},
    {WM_IME_ENDCOMPOSITION, "WM_IME_ENDCOMPOSITION"},
    {WM_IME_COMPOSITION, "WM_IME_COMPOSITION"},
    {WM_IME_KEYLAST, "WM_IME_KEYLAST"},
    {WM_INITDIALOG, "WM_INITDIALOG"},
    {WM_COMMAND, "WM_COMMAND"},
    {WM_SYSCOMMAND, "WM_SYSCOMMAND"},
    {WM_TIMER, "WM_TIMER"},
    {WM_HSCROLL, "WM_HSCROLL"},
    {WM_VSCROLL, "WM_VSCROLL"},
    {WM_INITMENU, "WM_INITMENU"},
    {WM_INITMENUPOPUP, "WM_INITMENUPOPUP"},
    {WM_MENUSELECT, "WM_MENUSELECT"},
    {WM_MENUCHAR, "WM_MENUCHAR"},
    {WM_ENTERIDLE, "WM_ENTERIDLE"},
//    {WM_MENURBUTTONUP, "WM_MENURBUTTONUP"},
//    {WM_MENUDRAG, "WM_MENUDRAG"},
//    {WM_MENUGETOBJECT, "WM_MENUGETOBJECT"},
//    {WM_UNINITMENUPOPUP, "WM_UNINITMENUPOPUP"},
//    {WM_MENUCOMMAND, "WM_MENUCOMMAND"},
    {0x0127, "WM_CHANGEUISTATE"},
    {0x0128, "WM_UPDATEUISTATE"},
    {0x0129, "WM_QUERYUISTATE"},
    {WM_CTLCOLORMSGBOX, "WM_CTLCOLORMSGBOX"},
    {WM_CTLCOLOREDIT, "WM_CTLCOLOREDIT"},
    {WM_CTLCOLORLISTBOX, "WM_CTLCOLORLISTBOX"},
    {WM_CTLCOLORBTN, "WM_CTLCOLORBTN"},
    {WM_CTLCOLORDLG, "WM_CTLCOLORDLG"},
    {WM_CTLCOLORSCROLLBAR, "WM_CTLCOLORSCROLLBAR"},
    {WM_CTLCOLORSTATIC, "WM_CTLCOLORSTATIC"},
    {WM_MOUSEFIRST, "WM_MOUSEFIRST"},
    {WM_MOUSEMOVE, "WM_MOUSEMOVE"},
    {WM_LBUTTONDOWN, "WM_LBUTTONDOWN"},
    {WM_LBUTTONUP, "WM_LBUTTONUP"},
    {WM_LBUTTONDBLCLK, "WM_LBUTTONDBLCLK"},
    {WM_RBUTTONDOWN, "WM_RBUTTONDOWN"},
    {WM_RBUTTONUP, "WM_RBUTTONUP"},
    {WM_RBUTTONDBLCLK, "WM_RBUTTONDBLCLK"},
    {WM_MBUTTONDOWN, "WM_MBUTTONDOWN"},
    {WM_MBUTTONUP, "WM_MBUTTONUP"},
    {WM_MBUTTONDBLCLK, "WM_MBUTTONDBLCLK"},
    {WM_MOUSEWHEEL, "WM_MOUSEWHEEL"},
    {0x020B, "WM_XBUTTONDOWN"},
    {0x020C, "WM_XBUTTONUP"},
    {0x020D, "WM_XBUTTONDBLCLK"},
    {WM_MOUSELAST, "WM_MOUSELAST"},
    {WM_PARENTNOTIFY, "WM_PARENTNOTIFY"},
    {WM_ENTERMENULOOP, "WM_ENTERMENULOOP"},
    {WM_EXITMENULOOP, "WM_EXITMENULOOP"},
    {WM_NEXTMENU, "WM_NEXTMENU"},
    {WM_SIZING, "WM_SIZING"},
    {WM_CAPTURECHANGED, "WM_CAPTURECHANGED"},
    {WM_MOVING, "WM_MOVING"},
    {WM_POWERBROADCAST, "WM_POWERBROADCAST"},
    {WM_DEVICECHANGE, "WM_DEVICECHANGE"},
    {WM_MDICREATE, "WM_MDICREATE"},
    {WM_MDIDESTROY, "WM_MDIDESTROY"},
    {WM_MDIACTIVATE, "WM_MDIACTIVATE"},
    {WM_MDIRESTORE, "WM_MDIRESTORE"},
    {WM_MDINEXT, "WM_MDINEXT"},
    {WM_MDIMAXIMIZE, "WM_MDIMAXIMIZE"},
    {WM_MDITILE, "WM_MDITILE"},
    {WM_MDICASCADE, "WM_MDICASCADE"},
    {WM_MDIICONARRANGE, "WM_MDIICONARRANGE"},
    {WM_MDIGETACTIVE, "WM_MDIGETACTIVE"},
    {WM_MDISETMENU, "WM_MDISETMENU"},
    {WM_ENTERSIZEMOVE, "WM_ENTERSIZEMOVE"},
    {WM_EXITSIZEMOVE, "WM_EXITSIZEMOVE"},
    {WM_DROPFILES, "WM_DROPFILES"},
    {WM_MDIREFRESHMENU, "WM_MDIREFRESHMENU"},
    {WM_IME_SETCONTEXT, "WM_IME_SETCONTEXT"},
    {WM_IME_NOTIFY, "WM_IME_NOTIFY"},
    {WM_IME_CONTROL, "WM_IME_CONTROL"},
    {WM_IME_COMPOSITIONFULL, "WM_IME_COMPOSITIONFULL"},
    {WM_IME_SELECT, "WM_IME_SELECT"},
    {WM_IME_CHAR, "WM_IME_CHAR"},
//    {WM_IME_REQUEST, "WM_IME_REQUEST"},
    {WM_IME_KEYDOWN, "WM_IME_KEYDOWN"},
    {WM_IME_KEYUP, "WM_IME_KEYUP"},
    {WM_MOUSEHOVER, "WM_MOUSEHOVER"},
    {WM_MOUSELEAVE, "WM_MOUSELEAVE"},
//    {WM_NCMOUSEHOVER, "WM_NCMOUSEHOVER"},
//    {WM_NCMOUSELEAVE, "WM_NCMOUSELEAVE"},
//    {WM_WTSSESSION_CHANGE, "WM_WTSSESSION_CHANGE"},
//    {WM_TABLET_FIRST, "WM_TABLET_FIRST"},
//    {WM_TABLET_LAST, "WM_TABLET_LAST"},
    {WM_CUT, "WM_CUT"},
    {WM_COPY, "WM_COPY"},
    {WM_PASTE, "WM_PASTE"},
    {WM_CLEAR, "WM_CLEAR"},
    {WM_UNDO, "WM_UNDO"},
    {WM_RENDERFORMAT, "WM_RENDERFORMAT"},
    {WM_RENDERALLFORMATS, "WM_RENDERALLFORMATS"},
    {WM_DESTROYCLIPBOARD, "WM_DESTROYCLIPBOARD"},
    {WM_DRAWCLIPBOARD, "WM_DRAWCLIPBOARD"},
    {WM_PAINTCLIPBOARD, "WM_PAINTCLIPBOARD"},
    {WM_VSCROLLCLIPBOARD, "WM_VSCROLLCLIPBOARD"},
    {WM_SIZECLIPBOARD, "WM_SIZECLIPBOARD"},
    {WM_ASKCBFORMATNAME, "WM_ASKCBFORMATNAME"},
    {WM_CHANGECBCHAIN, "WM_CHANGECBCHAIN"},
    {WM_HSCROLLCLIPBOARD, "WM_HSCROLLCLIPBOARD"},
    {WM_QUERYNEWPALETTE, "WM_QUERYNEWPALETTE"},
    {WM_PALETTEISCHANGING, "WM_PALETTEISCHANGING"},
    {WM_PALETTECHANGED, "WM_PALETTECHANGED"},
    {WM_HOTKEY, "WM_HOTKEY"},
    {WM_PRINT, "WM_PRINT"},
    {WM_PRINTCLIENT, "WM_PRINTCLIENT"},
    {0x0319, "WM_APPCOMMAND"},
    {0x031A, "WM_THEMECHANGED"},
    {WM_HANDHELDFIRST, "WM_HANDHELDFIRST"},
    {WM_HANDHELDLAST, "WM_HANDHELDLAST"},
    {WM_AFXFIRST, "WM_AFXFIRST"},
    {WM_AFXLAST, "WM_AFXLAST"},
    {WM_PENWINFIRST, "WM_PENWINFIRST"},
    {WM_PENWINLAST, "WM_PENWINLAST"},

    // TreeView messages
    {TVM_INSERTITEM, "TVM_INSERTITEM"},
    {TVM_DELETEITEM, "TVM_DELETEITEM"},
    {TVM_EXPAND, "TVM_EXPAND"},
    {TVM_GETITEMRECT, "TVM_GETITEMRECT"},
    {TVM_GETCOUNT, "TVM_GETCOUNT"},
    {TVM_GETINDENT, "TVM_GETINDENT"},
    {TVM_SETINDENT, "TVM_SETINDENT"},
    {TVM_GETIMAGELIST, "TVM_GETIMAGELIST"},
    {TVM_SETIMAGELIST, "TVM_SETIMAGELIST"},
    {TVM_GETNEXTITEM, "TVM_GETNEXTITEM"},
    {TVM_SELECTITEM, "TVM_SELECTITEM"},
    {TVM_GETITEM, "TVM_GETITEM"},
    {TVM_SETITEM, "TVM_SETITEM"},
    {TVM_EDITLABEL, "TVM_EDITLABEL"},
    {TVM_GETEDITCONTROL, "TVM_GETEDITCONTROL"},
    {TVM_GETVISIBLECOUNT, "TVM_GETVISIBLECOUNT"},
    {TVM_HITTEST, "TVM_HITTEST"},
    {TVM_CREATEDRAGIMAGE, "TVM_CREATEDRAGIMAGE"},
    {TVM_SORTCHILDREN, "TVM_SORTCHILDREN"},
    {TVM_ENSUREVISIBLE, "TVM_ENSUREVISIBLE"},
    {TVM_SORTCHILDRENCB, "TVM_SORTCHILDRENCB"},
    {TVM_ENDEDITLABELNOW, "TVM_ENDEDITLABELNOW"},
    {TVM_GETISEARCHSTRING, "TVM_GETISEARCHSTRING"},
    {TVM_SETTOOLTIPS, "TVM_SETTOOLTIPS"},
    {TVM_GETTOOLTIPS, "TVM_GETTOOLTIPS"},
    {TVM_SETINSERTMARK, "TVM_SETINSERTMARK"},
    {TVM_SETUNICODEFORMAT, "TVM_SETUNICODEFORMAT"},
    {TVM_GETUNICODEFORMAT, "TVM_GETUNICODEFORMAT"},
    {TVM_SETITEMHEIGHT, "TVM_SETITEMHEIGHT"},
    {TVM_GETITEMHEIGHT, "TVM_GETITEMHEIGHT"},
    {TVM_SETBKCOLOR, "TVM_SETBKCOLOR"},
    {TVM_SETTEXTCOLOR, "TVM_SETTEXTCOLOR"},
    {TVM_GETBKCOLOR, "TVM_GETBKCOLOR"},
    {TVM_GETTEXTCOLOR, "TVM_GETTEXTCOLOR"},
    {TVM_SETSCROLLTIME, "TVM_SETSCROLLTIME"},
    {TVM_GETSCROLLTIME, "TVM_GETSCROLLTIME"},
    {TVM_SETINSERTMARKCOLOR, "TVM_SETINSERTMARKCOLOR"},
    {TVM_GETINSERTMARKCOLOR, "TVM_GETINSERTMARKCOLOR"},
//    {TVM_GETITEMSTATE, "TVM_GETITEMSTATE"},
//    {TVM_SETLINECOLOR, "TVM_SETLINECOLOR"},
//    {TVM_GETLINECOLOR, "TVM_GETLINECOLOR"},
//    {TVM_MAPACCIDTOHTREEITEM, "TVM_MAPACCIDTOHTREEITEM"},
//    {TVM_MAPHTREEITEMTOACCID, "TVM_MAPHTREEITEMTOACCID"},
};

sMsgMap gs_rgmmNMCUSTOMDRAWFLAGS[] = 
{
    {CDDS_POSTERASE, "CDDS_POSTERASE"},
    {CDDS_POSTPAINT, "CDDS_POSTPAINT"},
    {CDDS_PREERASE, "CDDS_PREERASE"},
    {CDDS_PREPAINT, "CDDS_PREPAINT"},
    {CDDS_ITEM, "CDDS_ITEM"},
    {CDDS_ITEMPOSTERASE, "CDDS_ITEMPOSTERASE"},
    {CDDS_ITEMPOSTPAINT, "CDDS_ITEMPOSTPAINT"},
    {CDDS_ITEMPREERASE, "CDDS_ITEMPREERASE"},
    {CDDS_ITEMPREPAINT, "CDDS_ITEMPREPAINT"},
    {CDDS_SUBITEM, "CDDS_SUBITEM"},
};
    
void DumpFlags(DWORD dwFlags, sMsgMap *pmm, int cItems)
{
    bool fDumped = false;
    for (int j = 0; j < cItems; j++)
        if (dwFlags & pmm[j].uMsg)
        {
            if (fDumped)
                DebugOutput(" | ");
            DebugOutput("%s", pmm[j].sz);
            fDumped = true;
        }
}

void TrackMessage(UINT uMsg, WPARAM wparam, LPARAM lparam, char *szText)
{
    NMHDR *pnmhdr;
    int j;

#ifndef TRACKMSGS
    return;
#endif
    for (int i = 0; i < (sizeof gs_rgmsgmap) / sizeof sMsgMap; i++)
        if (uMsg == gs_rgmsgmap[i].uMsg)
        {
            DebugOutput("%s:  %s ", szText, gs_rgmsgmap[i].sz);

            // Special-case parameters
            switch(uMsg)
            {
            case WM_WINDOWPOSCHANGING:
            case WM_WINDOWPOSCHANGED:
                DebugOutput("((%d, %d) - (%d, %d) flags = ", ((WINDOWPOS*)lparam)->x, ((WINDOWPOS*)lparam)->y,
                            ((WINDOWPOS*)lparam)->cx, ((WINDOWPOS*)lparam)->cy);
                DumpFlags(((WINDOWPOS*)lparam)->flags, gs_rgmmWPCFlags, sizeof gs_rgmmWPCFlags / sizeof sMsgMap);
                DebugOutput(")\n");
                break;

            case WM_SIZE:
                DebugOutput("(Size = (%d,%d))\n", LOWORD(lparam), HIWORD(lparam));
                break;
            case WM_SIZING:
                DebugOutput("(Rect = (%d,%d)-(%d,%d))\n", ((LPRECT)lparam)->left, ((LPRECT)lparam)->top,
                    ((LPRECT)lparam)->right, ((LPRECT)lparam)->bottom);
                break;
            case WM_MOVE:
                DebugOutput("(Pos = (%d,%d))\n", LOWORD(lparam), HIWORD(lparam));
                break;
            case WM_NOTIFY:
                pnmhdr = (NMHDR*)lparam;
                for (j = 0; j < (sizeof gs_rgmsgmapNotify) / sizeof sMsgMap; j++)
                    if (pnmhdr->code == gs_rgmsgmapNotify[j].uMsg)
                    {
                        switch(pnmhdr->code)
                        {
                        case NM_CUSTOMDRAW:
                            DebugOutput("(NM_CUSTOMDRAW item=%d (drawstage = ",wparam);
                            DumpFlags(((LPNMCUSTOMDRAW)lparam)->dwDrawStage,
                                      gs_rgmmNMCUSTOMDRAWFLAGS,
                                      sizeof gs_rgmmNMCUSTOMDRAWFLAGS / sizeof sMsgMap);
                            DebugOutput(")\n");
                            break;
                        case HDN_TRACK:
                            {
                                LPNMHEADER pnmhdr = (LPNMHEADER)lparam;
                                LPHDITEM phdi = pnmhdr->pitem;
                                DebugOutput("(HDN_TRACK (item %d width %d)\n", pnmhdr->iItem, phdi->cxy);
                            }
                            break;
                        default:
                            DebugOutput("(%s item=%d (lparam = 0x%08x)\n", gs_rgmsgmapNotify[j].sz, wparam, lparam);
                            break;
                        }
                        return;
                    }
                    DebugOutput("(Unknown Notify Code: 0x%08x)\n", pnmhdr->code);
                break;

            default:
                 DebugOutput("(wparam = 0x%08x, lparam = 0x%08x)\n", wparam, lparam);
                 break;
            }
            return;
        }
    DebugOutput("%s: Unknown Message '%d'\n", szText, uMsg);
}
