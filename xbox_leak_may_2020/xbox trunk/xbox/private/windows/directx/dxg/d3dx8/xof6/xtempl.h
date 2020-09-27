//**************************************************************************
//
// Copyright (C) 1997 - 1998 Microsoft Corporation. All Rights Reserved.
//
// FileName:    xtempl.h
//
// Description: Template related classes header file
//
// History:
//      01/09/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XTEMPL_H_
#define _XTEMPL_H_

#define SZBINARY "BINARY"

#define DIMENSION_LIST_CHUNK_SIZE       2
#define TEMPLATE_OPTION_LIST_CHUNK_SIZE 5
#define TEMPLATE_MEMBER_LIST_CHUNK_SIZE 10

typedef enum _DXFILETEMPLATETYPE {
    DXFILETEMPLATE_OPEN,
    DXFILETEMPLATE_CLOSED,
    DXFILETEMPLATE_RESTRICTED
} DXFILETEMPLATETYPE;

class XTemplate;

////////////////////////////////////////////////////////////////////////////
//
//  XIdentifier: Class for identifiers. name | uuid | name uuid.
//
////////////////////////////////////////////////////////////////////////////

class XIdentifier
{
    const XString m_szName;
    const GUID    m_id;
    XIdentifier(LPCSTR szName, REFGUID id) : m_szName(szName), m_id(id) {}

public:
    static XIdentifier *Create(LPCSTR szName, REFGUID id);

    LPCSTR name() { return m_szName; }
    REFGUID id() { return m_id; }
};

typedef XPtrArray<XIdentifier, TEMPLATE_OPTION_LIST_CHUNK_SIZE> \
XTemplateOptionList;

////////////////////////////////////////////////////////////////////////////
//
//  XDimension: Class for describing array dimension.
//
////////////////////////////////////////////////////////////////////////////

class XDimension
{
public:
    DWORD   m_cElements;
    XString m_szElements;

    XDimension(DWORD cElements, LPCSTR szElements)
      : m_cElements(cElements), m_szElements(szElements) {}
};

typedef XPtrArray<XDimension, DIMENSION_LIST_CHUNK_SIZE> XDimensionList;

////////////////////////////////////////////////////////////////////////////
//
//  XTemplateMember: Class for template members.
//
////////////////////////////////////////////////////////////////////////////

class XTemplateMember
{
    XString           m_szName;
    const XPrimitive *m_pPrim;
    XTemplate        *m_pTemplate;
    XDimensionList   *m_plDim;

    XTemplateMember(LPCSTR szName,
                    const XPrimitive *pPrim,
                    XTemplate *pTemplate,
                    XDimensionList *plDim);

    HRESULT ResolveArraySize(XTemplate *pParent, DWORD *pcArraySize);

    HRESULT ModulateSingleData(LPVOID *ppvData, DWORD *pcbSize, LPVOID *ppvRet);

public:
    LPVOID m_pData;

    ~XTemplateMember();

    static XTemplateMember *Create(LPCSTR szType,
                                   LPCSTR szName,
                                   XDimensionList *plDim = NULL);

    LPCSTR name() { return m_szName; }
    XDimensionList *dl() { return m_plDim; }

    LPCSTR type();

    BOOL NeedRepack();

    HRESULT ModulateData(LPVOID *ppvData, DWORD *pcbSize, LPVOID *ppvRet,
                         XTemplate *pParent);

    DWORD GetSize(XTemplate *pParent);
};

typedef XPtrArray<XTemplateMember, TEMPLATE_MEMBER_LIST_CHUNK_SIZE> \
XTemplateMemberList;

////////////////////////////////////////////////////////////////////////////
//
//  XTemplateInfo: Class for definding template.
//
////////////////////////////////////////////////////////////////////////////

class XTemplateInfo
{
public:
    XTemplateMemberList *m_plMembers;
    XTemplateOptionList *m_plOptions;
    DXFILETEMPLATETYPE   m_type;

    XTemplateInfo(XTemplateMemberList *plMembers,
                  XTemplateOptionList *plOptions = NULL,
                  DXFILETEMPLATETYPE   type = DXFILETEMPLATE_CLOSED)
      : m_plMembers(plMembers), m_plOptions(plOptions), m_type(type) {}
};

////////////////////////////////////////////////////////////////////////////
//
//  XTemplate: Class for templates.
//
////////////////////////////////////////////////////////////////////////////

class XTemplate
{
    const XString m_szName;
    const GUID    m_id;

    XTemplateMemberList *m_plMembers;     // Template member list.
    XTemplateOptionList *m_plOptions;     // restricted member identifiers.
    DXFILETEMPLATETYPE   m_type;          // Open, closed or restricted.

    // All numbers are stored in 4 bytes from the parser.
    // If the size of any member is not 4. We need to repack data.

    BOOL                  m_fRepack;

public:
    XTemplate(LPCSTR szName, REFGUID id, XTemplateInfo *pTemplInfo);
    virtual ~XTemplate();

    const XString &name() { return m_szName; }
    REFGUID id() { return m_id; }

    XTemplateMemberList *ml() { return m_plMembers; }
    XTemplateOptionList *ol() { return m_plOptions; }
    BOOL IsOpen() { return m_type == DXFILETEMPLATE_OPEN; }
    BOOL IsClosed() { return m_type == DXFILETEMPLATE_CLOSED; }

    BOOL NeedRepack() { return m_fRepack; }

    HRESULT GetMemberData(XTemplateMember *pMemberCur,
                          LPCSTR szMember,
                          LPVOID *ppvData);

    HRESULT ModulateData(LPVOID *ppvData, DWORD *pcbSize, LPVOID *ppvRet);

    DWORD GetSize(LPVOID pvSrc);

    HRESULT GetSubData(LPVOID pData, LPCSTR szMember, DWORD *pcbSize,
                       LPVOID *ppvRet);
};


////////////////////////////////////////////////////////////////////////////
//
//  XTemplateList: Class for template list.
//
////////////////////////////////////////////////////////////////////////////

class XTemplateList
{
public:
    static BOOL Init();

    static int Add(XTemplate *pSrc);

    static BOOL Find(LPCSTR szName,
                     XTemplate **ppTmplate = NULL);

    static BOOL Find(REFGUID id,
                     XTemplate **ppTmplate = NULL);

    static void Free();
};

#endif // _XTEMPL_H_
