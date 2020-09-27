/*****************************************************************************\
    FILE: newmenu.h
    
    DESCRIPTION:
        The file supports the "New" menu to create new items on the XBOX server.
    This currently only supports Folders but hopefully it will support other
    items later.
\*****************************************************************************/

#ifndef _NEWMENU_H
#define _NEWMENU_H

// For CreateNewFolderCB:
// The following struct is used when recursively downloading
// files/dirs from the XBOX server after a "Download" verb.
typedef struct tagXBOXCREATEFOLDERSTRUCT
{
    LPCWSTR             pszNewFolderName;
    CXboxFolder *        pff;
} XBOXCREATEFOLDERSTRUCT;


// Public APIs (DLL wide)
HRESULT CreateNewFolder(HWND hwnd, CXboxFolder * pff, CXboxDir * pfd, IUnknown * punkSite, BOOL fPosition, POINT point);
HRESULT CreateNewFolderCB(HINTERNET hint, HINTPROCINFO * phpi, LPVOID pvFCFS, BOOL * pfReleaseHint);


#endif // _NEWMENU_H





