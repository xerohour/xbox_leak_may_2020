/*++

Copyright (c) Microsoft Corporation

Module Name:

    visit.h

Abstract:

  1)  Declares the IXboxVisitor class.  This is a callback interface
      implemented by clients that need to "visit" the items of a
      folder.  Visiting can also be done recursively.


  2)  Declares IXboxXXXXVisit interfaces.  These are interfaces
      implemented by simple private helpers of the CXboxFolder
      family of classes.  They basically encapsulate and distill
      what can be done when visiting a particular item type.

Environment:

    Windows 2000 and Later 
    User Mode
    Compiles UNICODE, but explictly most things are ANSI,
    since the Xbox filesystem is all ANSI.

Revision History:
    
    07-03-01 Created by Mitchell Dernis (mitchd)

--*/

//-------------------------------------------------------------------------------
//  Interface to a visit
//-------------------------------------------------------------------------------

class IXboxVisit
/*++
  Interface Description:
    Things you can do when visiting any Xbox Item
--*/
{ 
  public:
   virtual void         GetName(OUT LPSTR pszItemName) = 0;
   virtual LPITEMIDLIST GetPidl(CPidlUtils::PIDLTYPE PidlType) = 0;
   virtual ULONG        GetShellAttributes() = 0;
   virtual void         GetConsoleName(OUT LPSTR pszConsoleName) = 0;
};

class IXboxConsoleVisit : public IXboxVisit
/*++
  Interface Description:
    Things you can do when visiting an Xbox Console
--*/
{
  public:
   virtual DWORD GetIPAddress() = 0;
   virtual void  SetDefault(BOOL fMakeDefault) = 0;
   virtual BOOL  GetDefault() = 0;
};

class IXboxVolumeVisit : public IXboxVisit
/*++
  Interface Description:
    Things you can do when visiting an Xbox Volume.
--*/
{
 public:
   virtual int  GetVolumeType() = 0; // Returns string Resource ID
   virtual void GetDiskCapacity(PULONGLONG pullTotalCapacity, PULONGLONG pullFreeCapacity) = 0;
};

class IXboxFileOrDirVisit : public IXboxVisit
/*++
  Interface Description:
    Things you can do when visiting an Xbox Directory or File.
--*/
{
 public:
  virtual void    GetFileAttributes(PDM_FILE_ATTRIBUTES pDmFileAttributes) = 0;
  virtual HRESULT SetFileAttributes(PDM_FILE_ATTRIBUTES pDmFileAttributes) = 0;
  virtual HRESULT SetName(HWND hWnd, LPSTR pszNewName) = 0;
  virtual HRESULT Delete() = 0;
  virtual void GetWireName(LPSTR pszWireName) = 0;
};

//-------------------------------------------------------------------------------
//  Declare of IXboxVisitor Interface.
//-------------------------------------------------------------------------------
class IXboxVisitor
/*++
  Interface Description:
    Used to call back a visitor for each "selected item" and\or to recurse items.

  Implemented By:
    Clients of CXboxFolder that need to walk a selection of items and possibly
    recurse them.

  How is it used:
    The client calls CXBoxFolder::VisitEach or CXboxFolder::VisitThese.  It is called
    back on the appropriate method for each of the items dictated by the context and
    parameters of VisitEach or VisitThese.

    If recursion is desired the Visit???? method is always called recursing down
    the tree, Visit????Post is called on the way back up.
--*/
{
  public:

    virtual void VisitRoot         (IXboxVisit *pRoot,                DWORD *pdwFlags) = 0;   
    virtual void VisitAddConsole   (IXboxVisit *pAddConsole,          DWORD *pdwFlags) = 0;
    virtual void VisitConsole      (IXboxConsoleVisit   *pConsole,    DWORD *pdwFlags) = 0;   
    virtual void VisitVolume       (IXboxVolumeVisit    *pVolume,     DWORD *pdwFlags) = 0;
    virtual void VisitFileOrDir    (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags) = 0;

    //
    //  Directories may be recursed.  This method is called, when recursing a directory,
    //  after its children have been recursed.
    //
    virtual void VisitDirectoryPost (IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags) = 0;

    enum _Flags
    {
        FlagContinue = 1,
        FlagRecurse  = 2,   //Only applies to directories
        FlagCallPost = 4,   //Only applies to directories (requires FlagRecurse)
        
    };
};