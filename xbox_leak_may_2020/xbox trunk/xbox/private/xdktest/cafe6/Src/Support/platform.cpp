/*
    PLATFORM.CPP
    ============

    Copyright 1995 Microsoft Corp.


    History:

    Who      When       What
    ---------------------------
    CFlaat   5-30-95   Created
    CFlaat   6-25-95   Finished creating
    CFlaat   6-27-95   Added Win32s as subtype of Win32Intel


      This file represents the implementation of the CPlatform
	class, a mechanism by which CAFE tests and toolsets can
	indicate their hardware, OS, and language properties.

*/


#include "stdafx.h"
#include "platform.h"





/*

    The following are the currently defined OS constants for Visual C++.
  Each OS has a particular numeric range, such that a more specific
  description falls into the more general range.  For example, Win32MIPS
  falls within the Win32 range, but Mac68K does not.
  
    In other words, we're describing a tree by allocating a certain set
  of values to the leaves, and giving the parent of a set of nodes the
  whole range of values of its children, much like with a B-tree.

    Using a combination of OS & language description, platforms can be
  described in significant detail in a useful form.
*/


/*
     >>> WARNING <<<

Be advised that the arrays must precede the OS & language definitions;
otherwise, you will have construction chaos.
*/


// storage for all platforms & languages

CPlatform::COSArray CPlatform::s_aryOS;
CPlatform::CLangArray CPlatform::s_aryLang;



//   ctor params:                                 NAME             SHORT NMAE      ID's                     ABSTRACT?

  // this includes all OS's
CPlatform::COS const CPlatform::AnyOS                 ("Any OS",         "Any",          0x00000001, UINT_MAX,    TRUE);

  // this includes all Win32 OS's
CPlatform::COS const CPlatform::Win32             ("Win32",          "Win32",        0x10000000, 0x1fffffff,  TRUE);
CPlatform::COS const CPlatform::Win32Mips       ("Win32 MIPS",     "win32_mips",   0x10002000, 0x10002fff,  FALSE);
CPlatform::COS const CPlatform::Win32Alpha      ("Win32 Alpha",    "win32_alpha",  0x10003000, 0x10003fff,  FALSE);
CPlatform::COS const CPlatform::Win32PowerPC    ("Win32 PowerPC",  "win32_ppc",    0x10004000, 0x10004fff,  FALSE);
CPlatform::COS const CPlatform::Win32Intel      ("Win32 Intel",    "win32_x86",    0x10001000, 0x10001fff,  FALSE);
CPlatform::COS const CPlatform::Win32s          ("Win32s",         "win32s_x86",   0x10001010, 0x1000101f,  FALSE);

  // this includes all Mac OS's
CPlatform::COS const CPlatform::Mac             ("Mac",            "Mac",          0x20000000, 0x2fffffff,  TRUE);
CPlatform::COS const CPlatform::Mac68K          ("68K Mac",        "mac_68k",      0x20001000, 0x20001fff,  FALSE);
CPlatform::COS const CPlatform::MacPowerPC      ("PowerMac",       "mac_ppc",      0x20002000, 0x20002fff,  FALSE);
CPlatform::COS const CPlatform::Xbox           ("Xbox",            "xbox",      0x90000000, 0x90001fff,  FALSE);


/*

    The following are the currently defined language constants for Visual 
  C++.  Each language has a unique numeric value.

*/

//   ctor params:                                   NAME			ID          DBCS?   ABSTRACT?

CPlatform::CLanguage const CPlatform::AnyLanguage	("Any",			0x00000001, FALSE,  TRUE);
CPlatform::CLanguage const CPlatform::English		("English",		0x00010001, FALSE,  FALSE);
CPlatform::CLanguage const CPlatform::German		("German",		0x00020001, FALSE,  FALSE);
CPlatform::CLanguage const CPlatform::Japanese		("Japanese",	0x00030001, TRUE,   FALSE);


// CPlatform::COS members

CPlatform::COS::COS(LPCSTR szName, LPCSTR szShortName, unsigned const uLowerLimit, unsigned const uUpperLimit, BOOL bAbstract)
  : m_uLowerLimit(uLowerLimit), 
    m_uUpperLimit(uUpperLimit),
    m_strName(szName),
    m_strShortName(szShortName),
    m_bAbstract(bAbstract)
{
  AssertValid();

  CPlatform::RegisterOS(this);
}

int CPlatform::COS::operator ==(const CPlatform::COS &rOS) const
{
  AssertValid();
  rOS.AssertValid();

  return ((m_uLowerLimit == rOS.m_uLowerLimit) && (m_uUpperLimit == rOS.m_uUpperLimit));
}

int CPlatform::COS::operator !=(const CPlatform::COS &rOS) const
{
  AssertValid();
  rOS.AssertValid();

  return !(this->operator==(rOS));  // we'll define it as the negation of our operator ==
}

BOOL CPlatform::COS::Includes(const CPlatform::COS &rOS) const
{
  AssertValid();
  rOS.AssertValid();

  return ((m_uLowerLimit <= rOS.m_uLowerLimit) && (m_uUpperLimit >= rOS.m_uUpperLimit));
}

BOOL CPlatform::COS::IsAbstract(void) const
{
   return m_bAbstract; 
}

void CPlatform::COS::AssertValid(void) const
{
  ASSERT(!m_strName.IsEmpty());

  ASSERT(m_uLowerLimit && m_uUpperLimit && (m_uLowerLimit < m_uUpperLimit));

//    the following line causes infinite assertion checks!!
//  ASSERT(AnyOS.Includes(*this));
}


// CPlatform::CLanguage members

CPlatform::CLanguage::CLanguage(LPCSTR szName, unsigned const uID, BOOL bDBCS, BOOL bAbstract)
  : m_strName(szName),
    m_uID(uID),
    m_bDBCS(bDBCS),
    m_bAbstract(bAbstract)
{
  AssertValid();

  CPlatform::RegisterLanguage(this);
}

int CPlatform::CLanguage::operator ==(const CLanguage &rLang) const
{
  AssertValid();
  rLang.AssertValid();

  return (m_uID == rLang.m_uID);
}

int CPlatform::CLanguage::operator !=(const CLanguage &rLang) const
{
  AssertValid();
  rLang.AssertValid();

  return !(this->operator==(rLang));  // we'll define it as the negation of our operator ==
}

BOOL CPlatform::CLanguage::IsDoubleByte(void) const
{
  AssertValid();

  return m_bDBCS;
}

BOOL CPlatform::CLanguage::IsAbstract(void) const
{
   return m_bAbstract; 
}


void CPlatform::CLanguage::AssertValid(void) const
{
  ASSERT(m_uID);
}




// CPlatform members

CPlatform::CPlatform(const COS &rOS, const CLanguage &rLang)
  : m_rOS(rOS), m_rLang(rLang)
{
  AssertValid();
  rLang.AssertValid();
}


CPlatform::CPlatform(const CPlatform &rPlatform)  // copy ctor
  : m_rOS(rPlatform.m_rOS), m_rLang(rPlatform.m_rLang)
{
  rPlatform.AssertValid();
  AssertValid();
}


int CPlatform::operator ==(const CPlatform &rP) const
{
  AssertValid();
  rP.AssertValid();

  return ((m_rLang == rP.m_rLang) && (m_rOS == rP.m_rOS));
}

int CPlatform::operator !=(const CPlatform &rP) const
{
  AssertValid();
  rP.AssertValid();

  return !(this->operator==(rP));  // we'll define it as the negation of our operator ==
}

BOOL CPlatform::Includes(const CPlatform &rP) const
{
  AssertValid();
  rP.AssertValid();

  return (((m_rLang == rP.m_rLang) || m_rLang == AnyLanguage)
	   && (m_rOS.Includes(rP.m_rOS)));
}

void CPlatform::AssertValid(void) const
{
  m_rOS.AssertValid();
  m_rLang.AssertValid();

  s_aryOS.AssertValid();
}


// static member:

const CPlatform::COSArray *CPlatform::GetOSArray(void)
{
  return &s_aryOS;
}

// static member:

const CPlatform::CLangArray *CPlatform::GetLangArray(void)
{
  return &s_aryLang;
}


// static member:

void CPlatform::RegisterOS(COS *pOS)
{
  if (!pOS->IsAbstract())
    s_aryOS.Add(pOS);
}

// static member:

void CPlatform::RegisterLanguage(CLanguage *pLang)
{
  if (!pLang->IsAbstract())
    s_aryLang.Add(pLang);
}



// misc. helpers

CPlatform::COS TOOLSET_API *GetOSFromShortName(LPCSTR szShortName)
{
    const CPlatform::COSArray *paryOS = CPlatform::GetOSArray();
    int nSize = paryOS -> GetSize();
    CString str(szShortName);

    int i;
    CPlatform::COS *pOS;

    for (i = 0; i < nSize; i++)
    {
      pOS = paryOS->GetAt(i);
      if (!str.CompareNoCase(pOS->GetShortName()))
	return pOS;
    }

    return 0;
}

CPlatform::CLanguage TOOLSET_API *GetLangFromName(LPCSTR szName)
{
    const CPlatform::CLangArray *paryLang = CPlatform::GetLangArray();
    int nSize = paryLang -> GetSize();
    CString str(szName);

    int i;
    CPlatform::CLanguage *pLang;

    for (i = 0; i < nSize; i++)
    {
      pLang = paryLang->GetAt(i);
      if (!str.CompareNoCase(pLang->GetName()))
	return pLang;
    }

    return 0;
}


