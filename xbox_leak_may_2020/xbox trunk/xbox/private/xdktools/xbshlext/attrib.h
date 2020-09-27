/*++

Copyright (c) Microsoft Corporation

Module Name:

    attrib.h

Abstract:

    Declaration  of CGetAttributes and CSetAttributes.

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    06-11-2001 : created (mitchd)

--*/

//
//  Message sent by CGetAttributes to update the progress of retrieving the attributes
//

struct XBOX_MULTIFILE_ATTRIBUTES
{
    DWORD     dwAttributes;      //Attribute Flags
    DWORD     dwValidAttributes;
    DWORD     dwFileCount;
    DWORD     dwFolderCount;
    ULONGLONG ullTotalSize;
    char      szTypeName[MAX_PATH];  //Get Type name
    FILETIME  ChangeTime;            //If one file
    FILETIME  CreationTime;          //If one file.
};

typedef bool (*GET_ATTRIBUTES_CB)(PVOID pv, XBOX_MULTIFILE_ATTRIBUTES *pAttributes);
class CGetAttributes : public IXboxVisitor
{
  public:
      
    CGetAttributes(XBOX_MULTIFILE_ATTRIBUTES *pAttributes, bool fGetTypeName = true, GET_ATTRIBUTES_CB pfnCallback = NULL, PVOID pvCallback = NULL);

    //
    //  These methods should never be hit.
    //
    void VisitRoot          (IXboxVisit *pRoot,               DWORD *pdwFlags){};
    void VisitAddConsole    (IXboxVisit *pAddConsole,         DWORD *pdwFlags){};
    void VisitConsole       (IXboxConsoleVisit   *pConsole,   DWORD *pdwFlags){};
    void VisitVolume        (IXboxVolumeVisit    *pVolume,    DWORD *pdwFlags){};
    void VisitDirectoryPost (IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags){};

    //
    //  Get or set the attributes.
    //

    void VisitFileOrDir    (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags);
    
    private:
      XBOX_MULTIFILE_ATTRIBUTES *m_pAttributes;
      GET_ATTRIBUTES_CB          m_pfnCallback;
      PVOID                      m_pvCallback;
      bool                       m_fGetTypeName;
      bool                       m_fVariousFileTypes;
      bool                       m_fFirstItem;
};


class CSetAttributes : public IXboxVisitor
{
  public:
      
    CSetAttributes(DWORD dwSetAttributes, DWORD dwClearAttributes) :
        m_dwSetAttributes(dwSetAttributes), m_dwClearAttributes(dwClearAttributes)
        {}

    //
    //  These methods should never be hit.
    //
    void VisitRoot          (IXboxVisit *pRoot,               DWORD *pdwFlags){};
    void VisitAddConsole    (IXboxVisit *pAddConsole,         DWORD *pdwFlags){};
    void VisitConsole       (IXboxConsoleVisit   *pConsole,   DWORD *pdwFlags){};
    void VisitVolume        (IXboxVolumeVisit    *pVolume,    DWORD *pdwFlags){};
    void VisitDirectoryPost (IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags){};

    //
    //  Get or set the attributes.
    //

    void VisitFileOrDir    (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags);
    
    private:
     DWORD m_dwSetAttributes;
     DWORD m_dwClearAttributes;
};

            
  