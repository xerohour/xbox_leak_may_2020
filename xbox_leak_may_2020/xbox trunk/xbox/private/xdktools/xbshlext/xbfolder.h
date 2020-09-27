/*++

Copyright (c) Microsoft Corporation

Module Name:

    xbfolder.h

Abstract:

    CXboxFolder implements IShellFolder, IPersistFolder, IBrowserFrameOptions.
    
      
    Beyond implementing these public interfaces, it is the base class for all the "folder" types
    in the Xbox Shell Namespace Extension:  These include: CXboxRoot, CXboxConsole,
    CXboxVolume, and CXboxDirectory.  CXboxVolume and CXboxDirectory have much in common and rely
    on a common intermediate class, CXBoxFileSystemFolder.

    A virtual depcition of the Xbox Namespace might look like this:   

    CXboxRoot
        |___CXboxConsole1___CXboxConsoleN
             |
             |_CXboxVolume(E)_____CXboxVolume(T)__...
               |
               |__CXboxDirectory__CXboxDirectory...
                    |
                    |_CXboxDirectory...
                        |
                        ...
                
    This does not include non-folder items.   Non-folder items are tracked by their container folder.

Selection Clones:
    
    A CXboxFolder is referenced counted like any COM object.  However, it is not always desirable to
    pass around pointers to the same object.  Two cases arises in the shell extension:

    1) IShellFolder::GetUIObjectOf is called to get DataObjects and MenuContexts among other things.
       These objects need a snap shot of a group of selected items.   They do not need to know about
       the other "un-selected" children, nor do they need to necessarily track dynamic changes.  (If
       one of the selected items is deleted out from under it, an appropriate error is generated when
       an operation is attempted.  The shell does the same thing.)

    2) Some operations (display of property pages, copying files, or deleting files) are best performed
       asynchronously in a different thread.  By the far the easiest way to manage thread conflicts is
       to pass a clone (and coincidently this also involves a selection) to the thread.

    Thus CXboxFolder has the concept of selection clone.   A selection clone is created via:
    CXboxFolder::CloneSelection.  It is reference counted like other COM objects

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    02-05-2001 : created (xtree.h)
    06-27-2001 : change caching scheme (xbitem.cpp)

--*/

//-------------------------------------------------------------------------------
// Constants
//-------------------------------------------------------------------------------
const int  MAX_CONSOLE_NAME = 40;
const int  MAX_XBOX_PATH = MAX_PATH+MAX_CONSOLE_NAME;
const char ROOT_GUID_NAME[] = "::{DB15FEDD-96B8-4DA9-97E0-7E5CCA05CC44}";
const WCHAR ROOT_GUID_NAME_WIDE[] = L"::{DB15FEDD-96B8-4DA9-97E0-7E5CCA05CC44}";
const char XBOX_SHELL_EXTENSION_KEY[] = "Software\\Microsoft\\XboxSDK\\xbshlext";
const char XBOX_SHELL_EXTENSION_CONSOLE_KEY[] = "Software\\Microsoft\\XboxSDK\\xbshlext\\Consoles";
const char ADD_NEW_CONSOLE_PIDL_NAME[] = "?Add Xbox";

//---------------------------------------------------------------------------------
// Basic shell attributes for these various item types
//---------------------------------------------------------------------------------
#define ROOT_SHELL_ATTRIBUTES\
        (SFGAO_CANLINK|SFGAO_CANRENAME|SFGAO_FOLDER|SFGAO_HASSUBFOLDER|\
         SFGAO_BROWSABLE)
        
#define CONSOLE_SHELL_ATTRIBUTES\
        (SFGAO_CANLINK|SFGAO_CANDELETE|SFGAO_HASPROPSHEET|SFGAO_FOLDER|SFGAO_HASSUBFOLDER|\
         SFGAO_BROWSABLE)

#define VOLUME_SHELL_ATTRIBUTES\
        (SFGAO_CANLINK|SFGAO_HASPROPSHEET|SFGAO_DROPTARGET|SFGAO_FOLDER|\
         SFGAO_HASSUBFOLDER)

#define DIRECTORY_SHELL_ATTRIBUTES\
        (SFGAO_CANCOPY|SFGAO_CANMOVE|SFGAO_CANLINK|SFGAO_CANRENAME|\
         SFGAO_CANDELETE|SFGAO_HASPROPSHEET|SFGAO_DROPTARGET|SFGAO_FOLDER|\
         SFGAO_HASSUBFOLDER)
  
#define FILE_SHELL_ATTRIBUTES\
        (SFGAO_CANCOPY|SFGAO_CANMOVE|SFGAO_CANRENAME|\
         SFGAO_CANDELETE|SFGAO_HASPROPSHEET)

#define ADDNEWCONSOLE_SHELL_ATTRIBUTES\
        (SFGAO_CANLINK)

//-----------------------------------------------------------------
//  VERB IDs.  These correspond to the verbs in the context menu
//  NOTE: IF YOU CHANGE THESE DEFINITIONS, YOU MUST CHANGE THE MENU
//        TABLE IN MENU.CPP
//-----------------------------------------------------
#define VERB_LAUNCH            0
#define VERB_OPEN              1
#define VERB_EXPLORE           2
#define VERB_REBOOT_WARM       3
#define VERB_REBOOT_SAME_TITLE 4
#define VERB_REBOOT_COLD       5
#define VERB_CAPTURE           6
#define VERB_SECURITY          7
#define VERB_SETDEFAULT        8
#define VERB_CUT               9
#define VERB_COPY             10
#define VERB_PASTE            11
#define VERB_DELETE           12
#define VERB_RENAME           13
#define VERB_NEWFOLDER        14
#define VERB_NEWCONSOLE       15
#define VERB_PROPERTIES       16

#define VERB_REBOOT_BITS    (I2BIT(VERB_REBOOT_WARM)|I2BIT(VERB_REBOOT_SAME_TITLE)|I2BIT(VERB_REBOOT_COLD))


#define ROOT_VERBS       (I2BIT(VERB_OPEN)|I2BIT(VERB_EXPLORE)|I2BIT(VERB_RENAME)|I2BIT(VERB_NEWCONSOLE))
#define CONSOLE_VERBS    (I2BIT(VERB_OPEN)|I2BIT(VERB_EXPLORE)|VERB_REBOOT_BITS|I2BIT(VERB_CAPTURE)|I2BIT(VERB_SETDEFAULT)|I2BIT(VERB_SECURITY)|I2BIT(VERB_DELETE)|I2BIT(VERB_PROPERTIES))
#define VOLUME_VERBS     (I2BIT(VERB_OPEN)|I2BIT(VERB_EXPLORE)|I2BIT(VERB_PASTE)|I2BIT(VERB_NEWFOLDER)|I2BIT(VERB_PROPERTIES))
#define DIRECTORY_VERBS  (I2BIT(VERB_OPEN)|I2BIT(VERB_EXPLORE)|I2BIT(VERB_CUT)|I2BIT(VERB_COPY)|\
                          I2BIT(VERB_PASTE)|I2BIT(VERB_DELETE)|I2BIT(VERB_RENAME)|I2BIT(VERB_NEWFOLDER)|I2BIT(VERB_PROPERTIES))
#define FILE_VERBS       (I2BIT(VERB_CUT)|I2BIT(VERB_COPY)|I2BIT(VERB_DELETE)|I2BIT(VERB_RENAME)|I2BIT(VERB_PROPERTIES))
#define ADDCONSOLE_VERBS (I2BIT(VERB_OPEN))
#define XBE_VERBS        (I2BIT(VERB_LAUNCH)|FILE_VERBS)

//---------------------------------------------------------------------------------
//  Names of columns associated with their index
//---------------------------------------------------------------------------------
#define XBOX_CONSOLE_COLUMN_NAME          0
#define XBOX_CONSOLE_COLUMN_IPADDR        1
#define XBOX_VOLUME_COLUMN_NAME           0
#define XBOX_VOLUME_COLUMN_TYPE           1
#define XBOX_VOLUME_COLUMN_FREE_CAPACITY  2
#define XBOX_VOLUME_COLUMN_TOTAL_CAPACITY 3
#define XBOX_FILE_COLUMN_NAME             0
#define XBOX_FILE_COLUMN_SIZE             1
#define XBOX_FILE_COLUMN_TYPE             2
#define XBOX_FILE_COLUMN_MODIFIED         3
#define XBOX_FILE_COLUMN_ATTRIBUTE        4

//---------------------------------------------------------------------------------
// Return Values For CompareIDs
//---------------------------------------------------------------------------------
#define S_EQUAL     0x00000000
#define S_GREATER   0x00000001
#define S_LESS      0x0000FFFF
#define E_EQUAL     0x80000000
#define E_GREATER   0x80000001
#define E_LESS      0x8000FFFF
inline HRESULT ResultFromCompare(int iCompare, bool fFail = false)
{
   HRESULT hr;
   hr = (((iCompare) > 0) ? S_GREATER : (((iCompare) < 0) ? S_LESS : S_EQUAL));
   if(fFail)
   {
        hr |= 0x80000000;
   }
   return hr;
}

//-------------------------------------------------------------------------------
//  Forward Declare All The CXboxFolder Classes.
//-------------------------------------------------------------------------------
class CXboxFolder;
class CXboxRoot;            
class CXboxConsole;         
class CXboxFileSystemFolder;
class CXboxVolume;
class CXboxDirectory;

//-------------------------------------------------------------------------------
//  Forward Declaration of visitor classes needed by CXboxFolder (as pointers)
//-------------------------------------------------------------------------------
class IXboxConsoleVisit;
class IXboxVolumeVisit;
class IXboxDirectoryVisit;
class IXboxFileVisit;
class IXboxVisitor;

//-------------------------------------------------------------------------------
//  Declaration of CXboxFolder
//-------------------------------------------------------------------------------
class ATL_NO_VTABLE CXboxFolder : 
	public CComObjectRootEx<CComMultiThreadModel>,
    public IShellFolder,
    public IPersistFolder
    CTRACKABLEOBJECT
{

 public:

  /*
  ** ATL COM INTERFACE MAP
  */
  DECLARE_PROTECT_FINAL_CONSTRUCT()
  BEGIN_COM_MAP(CXboxFolder)
    COM_INTERFACE_ENTRY_IID(IID_IPersistFolder, IPersistFolder)
    COM_INTERFACE_ENTRY_IID(IID_IShellFolder, IShellFolder)
  END_COM_MAP()
 
  /*
  ** ATL let's us customize registration of our object,
  ** if we just declare an UpdateRegistry method.
  */
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister);

 public:

  /*
  ** IShellFolder Methods
  */
  STDMETHOD(BindToObject)(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv);
  STDMETHOD(BindToStorage)(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv);
  STDMETHOD(CompareIDs)(LPARAM lParam, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
  STDMETHOD(CreateViewObject)(HWND hwnd, REFIID riid, void **ppv);
  STDMETHOD(EnumObjects)(HWND hwnd, DWORD grfFlags, IEnumIDList **ppenumIDList);
  STDMETHOD(GetAttributesOf)(UINT cidl, LPCITEMIDLIST * apidl, ULONG * drgfInOut);
  STDMETHOD(GetDisplayNameOf)(LPCITEMIDLIST pidl, DWORD uFlags, STRRET *psr);
  STDMETHOD(GetUIObjectOf)(HWND hwnd, UINT cidl, LPCITEMIDLIST * apidl, REFIID riid, UINT * prgfInOut, void **ppv);
  STDMETHOD(ParseDisplayName)(HWND hwnd, LPBC pbc, LPOLESTR pszDisplayName, ULONG *pchEaten, LPITEMIDLIST *ppidl, ULONG *pdwAttributes);
  STDMETHOD(SetNameOf)(HWND hwnd, LPCITEMIDLIST pidl, LPCOLESTR pszName, DWORD uFlags, LPITEMIDLIST *ppidl);
  
  /*
  ** IPersistFolder Methods
  */

  STDMETHOD(GetClassID)(LPCLSID lpClassID);   //IPersist really
  STDMETHOD(Initialize)(LPCITEMIDLIST pidl);

  /*
  ** Construction
  */
  HRESULT CXboxFolder::CloneSelection(UINT cidl, LPCITEMIDLIST *apidl, CXboxFolder **ppSelectionClone);
  HRESULT Clone(CXboxFolder **ppSelectionClone);

  /*
  ** Get Name and Attribute of Self
  */

  inline LPCSTR GetNamePtr() {return m_pszPathName;}
  inline LPCSTR GetPathNamePtr() {return m_pszName;}
  inline void   GetName(OUT LPSTR pszName) {strcpy(pszName, m_pszName);}
  inline void   GetPath(OUT LPSTR pszPath) {strcpy(pszPath, m_pszPathName);}
  inline void   GetName(OUT LPWSTR pwszName) {Utils::CopyAtoW(pwszName, m_pszName);}
  inline void   GetPath(OUT LPWSTR pwszPath) {Utils::CopyAtoW(pwszPath, m_pszPathName);}
  inline UINT   GetPathLen() {return m_uPathLen;}
  inline ULONG  GetShellAttributes() {return m_ulShellAttributes;}
  
  /*
  ** Item Identifiers
  */
  UINT          GetPidlLen(CPidlUtils::PIDLTYPE PidlType);
  LPITEMIDLIST  GetPidl(CPidlUtils::PIDLTYPE PidlType, UINT uExtraAllocation=0);

  /*
  ** Get Name and Attribute of Children
  */
  inline  void InvalidateChildList() {m_fChildrenValid=FALSE;}
  void    GetSelectShellAttributes(IN OUT ULONG *pulShellAttributes);
  inline  DWORD GetVerbsForSelection() {return m_fSelectionClone ? m_dwSelectionVerbs : 0;}
  UINT    GetChildCount(BOOL fGuesstimate = FALSE);
  
  /*
  ** Column Details
  */
  virtual HRESULT GetColumnHeaderDetails(IN UINT iColumn, OUT SHELLDETAILS *pShellDetails){return E_NOTIMPL;}
  virtual HRESULT GetDetails(IN LPCITEMIDLIST pidl, UINT iColumn, OUT SHELLDETAILS *pShellDetails){return E_NOTIMPL;}
  virtual HRESULT SetStatusBarText(IShellBrowser *pShellBrowser, LPCITEMIDLIST pidlSelect){}
  
  /*
  ** Visitors
  */
  void VisitEach(IXboxVisitor *pVisitor, DWORD dwFlags);
  void VisitThese(UINT cidl, LPCITEMIDLIST *apidl, IXboxVisitor *pVisitor, DWORD dwFlags);
    
  /*
  ** Utility
  */
  static UINT PathFromPidl(LPCITEMIDLIST pidl, LPSTR pszPathName);
  static LPITEMIDLIST PidlFromPath(LPCWSTR pwszPathName, UINT *puPathLength, UINT *puPathDepth,
                                   BOOL *pfLeadingSlash, BOOL *pfTrailingSlash);
  
  static void GetWireName(LPSTR pszWireName, LPCSTR pszInitialPath, LPCSTR pszFinalPath);
  static BOOL IsXbeFile(LPCSTR pszFileName);
  static  BOOL AreItemsIDsIdentical(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, BOOL *pfSameMachine);

  /*
  ** File Transfer
  */
  virtual HRESULT ReceiveFile(LPSTR pszRelativeRemoteFile, LPSTR szLocalFile){return E_FAIL;}
  virtual LPITEMIDLIST GetSourcePidl(LPCSTR pszFolder, LPCSTR pszItem);

  virtual void GetConsoleName(LPSTR pszConsole);
  virtual void GetTargetWireName(LPSTR pszWireName, LPSTR pszRelativeRemoteFile);
  virtual LPITEMIDLIST GetTargetPidl(LPSTR pszRelativeRemoteFile);
  virtual IXboxConnection *GetXboxConnection(){return NULL;}

 protected:

  /*
  ** Construction, Destruction and Cloning
  */
  CXboxFolder();
  virtual void Destroy();

  HRESULT 
  InitBaseClass(
    LPCSTR          pszPathName,
    ULONG           ulShellAttributes,
    LPCITEMIDLIST   pidlRoot
    );

  HRESULT DuplicateBaseMembers(CXboxFolder *pClone);
  virtual HRESULT CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFolder **ppSelectionClone){return E_NOTIMPL;}
  
  /*
  ** Children
  */
  HRESULT GetChildIndex(LPCSTR pszChildName, UINT *puIndex);
  HRESULT BuildSelectionIndexList(UINT cidl, LPCITEMIDLIST *apidl, UINT **ppuIndexList);
  LPITEMIDLIST GetChildPidl(UINT uIndex, CPidlUtils::PIDLTYPE PidlType);
  virtual HRESULT RefreshChildren(){return E_NOTIMPL;}
  virtual HRESULT ReallocateChildren(UINT uChildCount){return E_NOTIMPL;}
  virtual void FreeChildren(){}
  virtual HRESULT ValidateItem(LPCSTR pszPathName, UINT uRelativeDepth, DWORD *pdwAttributes){return E_NOTIMPL;}
  virtual void Visit(UINT uIndexCount, const UINT *puIndexList, IXboxVisitor *pVisitor, DWORD dwFlags){}

  /*
  ** Item Identifiers
  */

  HRESULT CompareItemIDs(UINT iColumn, int iNameCmp, const SHITEMID *pItem1, const SHITEMID *pItem2);
  virtual HRESULT CompareItemIDs(UINT iColumn, int iNameCmp, UINT uChildIndex1, UINT uChildIndex2){return E_NOTIMPL;}


  /*
  ** Variables pertaining to self
  */

  LPSTR         m_pszPathName;       // Pointer to full path
  LPSTR         m_pszName;           // Pointer to item name
  ULONG         m_ulShellAttributes; // The shell's SFGAO_XXXX flags.
  UINT          m_uPathDepth;        // How many ancestor's up to root, e.g. root is 0.
  UINT          m_uPathLen;          // Stored Length of the full path
  LPITEMIDLIST  m_pidlRoot;          // Pidl of our root (e.g. desktop to Xbox root)
  UINT          m_uRootPidlLen;      // Length of our pidl root.

  /*
  ** Variables pertaining to children
  */
  BOOL  m_fSelectionClone;           // If TRUE indicates that this instance is a clone of a selection.
  BOOL  m_fChildrenValid;            // Clear on creation, cleared by 
  UINT  m_uAllocatedChildCount;      // Length of Arrays of children
  UINT  m_uChildCount;               // Number of children
  LPSTR *m_rgpszChildNames;          // Array of name strings for each child
  ULONG *m_rgulChildShellAttributes; // Array of SFGAO_XXXX flags for each child
  ULONG m_dwSelectionVerbs;          // Verbs that apply to the current selection, only set if m_fSelectionClone.
  UINT  m_uLastChildIndex;           // Used for optimizing GetChildIndex.  See GetChildIndex implementation
                                     // for details.

 
 protected:
  ~CXboxFolder(){}              //force destroy 
 private:
  
  /*
  ** Stuff we outlaw, good C++ to prevent abuse.
  */
  
  CXboxFolder(CXboxFolder&);            //no implicit copying
  CXboxFolder& operator=(CXboxFolder&); //no assigning

  friend class CFolderVisit;
  friend class CFolderVisitFromSelf;
  friend class CDirectoryVisitFromSelf;
  friend class CConsoleVisit;
  friend class CConsoleVisitFromSelf;
      
};

class CXboxRoot : public CXboxFolder, public CComCoClass<CXboxRoot, &CLSID_XboxFolder>
{

 public:

  /*
  **  IPersistFolder (overrides CXboxFolder implementatio)
  */
  STDMETHOD(Initialize)(LPCITEMIDLIST pidl);

  /*
  ** IShellFolder Methods
  */
  STDMETHOD(BindToObject)(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv);

  /*
  ** Construction
  */
  CXboxRoot();

  /*
  ** Disallow file transfer
  */
  virtual void GetTargetWireName(LPSTR pszWireName, LPSTR pszRelativeRemoteFile){_ASSERT(FALSE);}
  virtual void GetConsoleName(LPSTR pszConsole) {*pszConsole='\0';}
  
  /*
  ** Column Details
  */

  virtual HRESULT GetColumnHeaderDetails(UINT iColumn, SHELLDETAILS *pShellDetails);
  virtual HRESULT GetDetails(LPCITEMIDLIST pidl, UINT iColumn, SHELLDETAILS *pShellDetails);
  virtual HRESULT SetStatusBarText(IShellBrowser *pShellBrowser, LPCITEMIDLIST pidlSelect);

 protected:
 
  /*
  **  Cloning
  */
  virtual HRESULT CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFolder **ppSelectionClone);

  /*
  ** Children
  */
  virtual HRESULT RefreshChildren();
  virtual void FreeChildren();
  virtual HRESULT ReallocateChildren(UINT uChildCount);
  virtual HRESULT ValidateItem(LPCSTR pszPathName, UINT uRelativeDepth, DWORD *pdwAttributes);
  virtual void Visit(UINT uIndexCount, const UINT *puIndexList, IXboxVisitor *pVisitor, DWORD dwFlags);

  /*
  ** Item Identifiers
  */

  virtual HRESULT CompareItemIDs(UINT iColumn, int iNameCmp, UINT uChildIndex1, UINT uChildIndex2);
  
  /*
  ** Root specific information about children.
  */
  DWORD  GetChildIpAddress(UINT uChildIndex, BOOL fRetry = FALSE);
  LPCSTR GetActualName(UINT uChildIndex);
  
  DWORD *m_rgdwChildIPAddresses; //Array of IP address for each of the children.
  UINT  m_uDefaultConsole;       //Index into child array. 
  LPSTR *m_rgpszActualNames;     //Array of actual Xbox names, fetched on demand.
  #define XBOX_USE_USER_PROVIDED_NAME ((LPSTR)(0xFFFFFFFF))

  /*
  ** Stuff we outlaw, good C++ to prevent abuse.
  */
private:
  CXboxRoot(CXboxRoot&);             //no implicit copying
  CXboxRoot& operator =(CXboxRoot&); //no assigning

  friend class CConsoleVisit;

};

class CXboxConsole : public CXboxFolder
{

 public:

  /*
  ** IShellFolder Methods
  */
  STDMETHOD(BindToObject)(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv);
  
  /*
  ** Construction
  */

  static  HRESULT Create(
                    LPCSTR          pszResolveName,
                    LPCITEMIDLIST   pidlRoot,
                    IXboxConnection *pConnection,
                    REFIID          riid,
                    void            **ppv
                    );

  /*
  ** Column Details
  */

  virtual HRESULT GetColumnHeaderDetails(UINT iColumn, SHELLDETAILS *pShellDetails);
  virtual HRESULT GetDetails(LPCITEMIDLIST pidl, UINT iColumn, SHELLDETAILS *pShellDetails);
  virtual HRESULT SetStatusBarText(IShellBrowser *pShellBrowser, LPCITEMIDLIST pidlSelect);
  
  /*
  ** File Transfer
  */
  virtual IXboxConnection *GetXboxConnection(){m_pConnection->AddRef();return m_pConnection;}
  
 protected:
  
  /*
  **  Cloning
  */
  virtual HRESULT CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFolder **ppSelectionClone);
  
  /*
  ** Children
  */
  virtual HRESULT RefreshChildren();
  virtual HRESULT ReallocateChildren(UINT uChildCount);
  virtual void FreeChildren();
  virtual HRESULT ValidateItem(LPCSTR pszPathName, UINT uRelativeDepth, DWORD *pdwAttributes);
  virtual void Visit(UINT uIndexCount, const UINT *puIndexList, IXboxVisitor *pVisitor, DWORD dwFlags);

  /*
  ** Item Identifiers
  */

  virtual HRESULT CompareItemIDs(UINT iColumn, int iNameCmp, UINT uChildIndex1, UINT uChildIndex2);

 protected:

  /*
  ** c'tor and d'tor
  */
  CXboxConsole();   
  virtual void Destroy();
    
  /*
  **  Information about the console
  */
  IXboxConnection *m_pConnection;
  LPSTR            m_pszDisplayName;
  DWORD            m_dwIPAddress;

  /*
  **  Console Specific Information About 
  **  Information about children   
  */
  
  ULONGLONG *m_rgullChildFreeSpace;  // In bytes.
  ULONGLONG *m_rgullChildTotalSpace; // In bytes.
  UINT      *m_rguChildVolumeType;   // IDS_DRIVETYPE_FIXEDDISK or IDS_DRIVETYPE_MEMORY_UNIT
  
  /*
  ** Stuff we outlaw, good C++ to prevent abuse.
  */
 private:
  CXboxConsole(CXboxConsole&);             //no implicit copying
  CXboxConsole& operator =(CXboxConsole&); //no assigning

  friend class CConsoleVisitFromSelf;
  friend class CVolumeVisit;

};

class CXboxFileSystemFolder : public CXboxFolder
{

 public:

  /*
  ** IShellFolder Methods
  */
  STDMETHOD(BindToObject)(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv);
  STDMETHOD(SetNameOf)(HWND hwnd, LPCITEMIDLIST pidl, LPCOLESTR pszName, DWORD uFlags, LPITEMIDLIST *ppidl);

  /*
  ** Column Details
  */

  virtual HRESULT GetColumnHeaderDetails(UINT iColumn, SHELLDETAILS *pShellDetails);
  virtual HRESULT GetDetails(LPCITEMIDLIST pidl, UINT iColumn, SHELLDETAILS *pShellDetails);
  virtual HRESULT SetStatusBarText(IShellBrowser *pShellBrowser, LPCITEMIDLIST pidlSelect);
  
  /*
  ** File Transfer - as source
  */
  virtual HRESULT ReceiveFile(LPSTR pszRelativeRemoteFile, LPSTR szLocalFile);
  
  /*
  ** File Transfer - as target
  */
  virtual IXboxConnection *GetXboxConnection(){m_pConnection->AddRef();return m_pConnection;}

 protected:
  
  /*
  ** c'tor and d'tor
  */
  CXboxFileSystemFolder();
  virtual void Destroy();
  HRESULT CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFileSystemFolder *pClone);

  /*
  ** Children
  */
  virtual HRESULT RefreshChildren();
  virtual HRESULT ReallocateChildren(UINT uChildCount);
  virtual void FreeChildren();
  virtual HRESULT ValidateItem(LPCSTR pszPathName, UINT uRelativeDepth, DWORD *pdwAttributes);
  HRESULT SetName(HWND hWnd, LPSTR szWireNameOld, LPSTR szWireNameNew);

  /*
  ** Item Identifiers
  */

  virtual HRESULT CompareItemIDs(UINT iColumn, int iNameCmp, UINT uChildIndex1, UINT uChildIndex2);

  /*
  **  File Specific Information about children
  */

  IXboxConnection    *m_pConnection;
  DM_FILE_ATTRIBUTES *m_rgChildFileAttributes;  //Array of File Attributes for Children
  LPSTR              *m_rgszChildTypeNames;     //Array of Type Names (as returned by SHGetFileInfo)

  /*
  ** utility
  */
  static LPSTR GetFileTypeName(PDM_FILE_ATTRIBUTES pdmFileAttributes);

  /*
  ** Stuff we outlaw, good C++ to prevent abuse.
  */
 
 private:
  CXboxFileSystemFolder(CXboxFileSystemFolder&);             //no implicit copying
  CXboxFileSystemFolder& operator =(CXboxFileSystemFolder&); //no assigning

  friend class CFileOrDirVisit;
  friend class CFileOrDirVisitRecurse;
  friend class CFileOrDirVisitFromSelf;
  friend class CDirectoryVisitFromSelf;
};

class CXboxVolume : public CXboxFileSystemFolder
{
 public:
  
  /*
  ** Construction
  */
  CXboxVolume::CXboxVolume(){INIT_TRACKABLEOBJECT_NAME(CXboxVolume);m_dwSelectionVerbs = VOLUME_VERBS;}
  static HRESULT Create(
                    LPCSTR          pszPathName,              
                    LPCITEMIDLIST   pidlRoot,
                    IXboxConnection *pConnection,
                    REFIID          riid,
                    void            **ppv
                    );

  static UINT GetVolumeType(char cDriveLetter);

 protected:

  /*
  **  Operations on selections
  */
  virtual HRESULT CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFolder **ppSelectionClone);
  virtual void Visit(UINT uIndexCount, const UINT *puIndexList, IXboxVisitor *pVisitor, DWORD dwFlags);

  /*
  ** Information about this volume
  */
  UINT m_uVolumeType;

  friend class CVolumeVisitFromSelf;
};

class CXboxDirectory : public CXboxFileSystemFolder
{
 public:
  /*
  ** Construction
  */
  CXboxDirectory::CXboxDirectory(){INIT_TRACKABLEOBJECT_NAME(CXboxDirectory);m_dwSelectionVerbs = DIRECTORY_VERBS;}
  static HRESULT Create(
                    LPCSTR          pszPathName,
                    LPCITEMIDLIST   pidlRoot,
                    IXboxConnection *pConnection,
                    REFIID          riid,
                    void            **ppv
                    );
 protected:
  /*
  **  Operations on selections
  */
  virtual HRESULT CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFolder **ppSelectionClone);
  virtual void Visit(UINT uIndexCount, const UINT *puIndexList, IXboxVisitor *pVisitor, DWORD dwFlags);

  
};

//-------------------------------------------------------------------------------
//  Specialize ATL's CComEnum to do IEnumIDList
//-------------------------------------------------------------------------------
typedef CComEnum<IEnumIDList,&IID_IEnumIDList,LPITEMIDLIST,CPidlUtils,CComMultiThreadModel> CEnumIdList;


