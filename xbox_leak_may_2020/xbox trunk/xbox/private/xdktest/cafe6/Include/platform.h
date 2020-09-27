
/*
    PLATFORM.H
    ==========

    Copyright 1995 Microsoft Corp.


    History:

    Who      When       What
    ---------------------------
    CFlaat   5-30-95   Created
    CFlaat   6-25-95   Finished creating
    CFlaat   6-27-95   Added Win32s



      This file represents the interface of the CPlatform
	class, a mechanism by which CAFE tests and toolsets can
	indicate their hardware, OS, and language properties.

*/


#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "toolxprt.h"

/*
    CPlatform class

        An instance of this class represents some platform or range of
    platforms.  The OS model is a tree, which currently looks like this:

         Any
		/   \
	  Mac   Win32
     /  |   |   
  PPC 68K   +- Intel
            +- MIPS
			+- Alpha
			\- PowerPC


    Likely future additions are PA-RISC for Win32 and possibly
	the Copland OS for Mac.

	    Languages are not modeled as a tree, but must either match
    exactly or have a test marked as working with language "Any".

	    The most important comparison between CPlatform instances
	is through the "Includes" function.  This returns true if one
	platform description is general enough to "include" another.
	The idea is that a test's set of platform information will be
	compared against a product's, to see if the scope of a test
	"includes" that product.

*/

class TOOLSET_CLASS CPlatform
{
public:

  // available languages

  class CLanguage;

  static CLanguage const AnyLanguage;

  static CLanguage const English;
  static CLanguage const German;
  static CLanguage const Japanese;


  // available operating systems

  class COS;

  static COS const AnyOS;  // this includes all OS's

  static COS const Win32;  // this includes all Win32 OS's
    static COS const Win32Mips;
    static COS const Win32Alpha;
    static COS const Win32PowerPC;
    static COS const Win32Intel;
      static COS const Win32s;   // subtype of Win32Intel

  static COS const Mac;  // this includes all Mac OS's
    static COS const Mac68K;
    static COS const MacPowerPC;

  static COS const Xbox;  // this includes all Xbox OS's

// arrays of OS's & languages

  typedef CArray<COS *, COS *> COSArray;
  typedef CArray<CLanguage *, CLanguage *> CLangArray;


public:

  CPlatform(const COS &rOS, const CLanguage &rLang);  // client ctor
  CPlatform(const CPlatform &rPlatform);  // copy ctor

  int operator ==(const CPlatform &rPlatform) const;  // comparison
  int operator !=(const CPlatform &rPlatform) const;  // comparison
  BOOL Includes(const CPlatform &rPlatform) const;    // asks the question "are you as general or more general than this platform"

  void AssertValid(void) const;

  static const COSArray * GetOSArray(void);
  static const CLangArray * GetLangArray(void);

protected:

  // OS data

  COS const &m_rOS;

  // language data

  CLanguage const &m_rLang;

  // arrays of "real live" OS's & languages, excluding abstract ones

  static COSArray s_aryOS;
  static CLangArray s_aryLang;


public:

  class TOOLSET_CLASS COS
  {
  friend class CPlatform;
  
  protected:

    unsigned const m_uLowerLimit, m_uUpperLimit;
    CString m_strName, m_strShortName;
    BOOL m_bAbstract;

    BOOL IsAbstract(void) const;

  public:

    COS(LPCSTR szName, LPCSTR szShortName, unsigned const uLowerLimit, unsigned const uUpperLimit, BOOL bAbstract);  // ctor for platform.cxx only
    int operator ==(const COS &rOS) const;  // comparison
    int operator !=(const COS &rOS) const;  // comparison
    BOOL Includes(const COS &rOS) const;    // asks the question "are you either as general or more general than rOS?"

    LPCSTR GetName(void) const { return m_strName; }
    LPCSTR GetShortName(void) const { return m_strShortName; }

    void AssertValid(void) const;
  };


  class TOOLSET_CLASS CLanguage
  {
  friend class CPlatform;

  protected:

    unsigned const m_uID;
	  BOOL m_bDBCS;
	  CString m_strName;
    BOOL m_bAbstract;

    BOOL IsAbstract(void) const;

  public:

    CLanguage(LPCSTR szName, unsigned const uID, BOOL bDBCS, BOOL bAbstract);
  	int operator ==(const CLanguage &rLang) const;
  	int operator !=(const CLanguage &rLang) const;
  	BOOL IsDoubleByte(void) const;

    LPCSTR GetName(void) const { return m_strName; }

    void AssertValid(void) const;
  };


protected:

friend class CLanguage;
friend class COS;

  static void RegisterOS(COS *pOS);
  static void RegisterLanguage(CLanguage *pLang);

};

CPlatform::COS TOOLSET_API *GetOSFromShortName(LPCSTR szShortName);
CPlatform::CLanguage TOOLSET_API *GetLangFromName(LPCSTR szName);



#endif
