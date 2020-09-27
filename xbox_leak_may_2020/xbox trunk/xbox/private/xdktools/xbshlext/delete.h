/*++

Copyright (c) Microsoft Corporation

Module Name:

    delete.h

Abstract:

    Declaration of CXboxDelete.

    Pass an instance to VisitEach or VisitThese to visit death
    and deletion upon such items.

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    07-18-2001 : created (mitchd)

--*/

#include "stdafx.h"

class CXboxDelete : public IXboxVisitor
{
  public:
 
    CXboxDelete(HWND hWnd, UINT uItemCount, bool fSilent);
    ~CXboxDelete();

    virtual void VisitRoot         (IXboxVisit *pRoot,                DWORD *pdwFlags);   
    virtual void VisitAddConsole   (IXboxVisit *pAddConsole,          DWORD *pdwFlags);
    virtual void VisitConsole      (IXboxConsoleVisit   *pConsole,    DWORD *pdwFlags);   
    virtual void VisitVolume       (IXboxVolumeVisit    *pVolume,     DWORD *pdwFlags);
    virtual void VisitFileOrDir    (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags);
    virtual void VisitDirectoryPost (IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags);
    
  private:

    //
    //  Internal Helpers Methods
    //

    void StartProgressDialog();

    //
    //  arguments from C'tor
    //

    HWND                m_hWnd;
    bool                m_fSilent;

    //
    //  Internal Variables
    //

    bool             m_fFirstItem;
    bool             m_fYesToAllReadOnly;
    bool             m_fNoToAllReadOnly;
    UINT             m_uRecurseDepth;
    UINT             m_uItemCount;
    IProgressDialog *m_pProgressDialog;
    

    // These are really part of us.  We need to make them separate
    // classes and members that point back to us for the CXboxFunctionObject
    // stuff to work.

};