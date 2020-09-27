
/* IFld.h -*- C++ -*- */

//******************************************************************************
//
// COPYRIGHT (C) 2000 APPLIED MICROSYSTEMS CORPORATION.  ALL RIGHTS RESERVED.
// UNPUBLISHED -- RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF THE UNITED
// STATES.  USE OF A COPYRIGHT NOTICE IS PRECAUTIONARY ONLY AND DOES NOT
// IMPLY PUBLICATION OR DISCLOSURE.
//
// THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE SECRETS OF
// APPLIED MICROSYSTEMS CORPORATION.  USE, DISCLOSURE, OR REPRODUCTION IS
// PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF APPLIED
// MICROSYSTEMS CORPORATION.
//
//                 RESTRICTED RIGHTS LEGEND
//
// Use, duplication, or disclosure by the Government is subject to
// restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
// in Technical Data and Computer Software clause at DFARS 252.227-7013
// or subparagraphs (c)(1) and (2) of Commercial Computer Software --
// Restricted Rights at 48 CFR 52.227-19, as applicable.
//
// Applied Microsystems Corporation
// 5020 148th Ave NE
// Redmond, WA 98052
//
//******************************************************************************
//
// Contents:
//
//      Interface to access the FLD files.
//
// Main authors:
//
//      03/14/01,pwl - initial version
//
// Target restrictions:
//
//      Identify target builds not supported by this header file.
//
// Tool restrictions:
//
//      Identify build tool limitations for this header file.
//
// Things to do:
//
//      Identify must-do, should-do, and wish-list items.
//
//******************************************************************************

#ifndef IFLD_H
#define IFLD_H

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#if defined (WIN32)
#pragma warning (disable : 4251) // X needs to have dll-interface to be used by client
#endif  // WIN32

// Uses other components
class IDiscLayout;  // from "IDiscLayout.h"

// Uses windows libraries
#include <windows.h>

// Uses standard libraries
#include <wchar.h>
#include <string>

#ifdef _MSC_VER
#ifdef LAYOUTENGINE_EXPORTS
#define LAYOUTENGINE_API __declspec(dllexport)
#else
#define LAYOUTENGINE_API __declspec(dllimport)
#endif
#else
#define LAYOUTENGINE_API
#endif

class CFldImpl;

/* ********************************************************** */
class LAYOUTENGINE_API IFld 
{
// --- public interface
public:
    typedef unsigned long tNodeIdx;

    friend class CFldImpl;

    enum EMaxErrorLength
    {
        MAX_LENGTH = 255
    };

    // Create, Open and Close a File Layout Descriptor file 
    // (new binary format only)
    bool IsFileReadOnly( const TCHAR * fname );
    bool CreateFld( const char* fname );
    bool CreateFld( const wchar_t* fname );
    bool OpenFld( const char* fname, bool bReadOnly=true );
    bool OpenFld( const wchar_t* fname, bool bReadOnly=true );
    void CloseFld();

    // Save and Load of disc layout specified by IDiscLayout
    bool SaveDiscLayout( const IDiscLayout& discLayout );
    bool LoadDiscLayout( IDiscLayout& discLayout );

    // General information
    bool GetVolName( const char** name );
    bool GetVolSetName( const char** name );
    bool SetVolName( const char* name );
    bool SetVolSetName( const char* name );
    bool GetVolName( const wchar_t** name );
    bool GetVolSetName( const wchar_t** name );
    bool SetVolName( const wchar_t* name );
    bool SetVolSetName( const wchar_t* name );

    // Error Handling
    const wchar_t * const GetLastErrorString( void )
    {
        return m_wszErrorMessage.c_str();
    }

    // file tree API
    tNodeIdx GetNumElem();
    tNodeIdx GetRootDir();
    tNodeIdx GetParentDir( tNodeIdx idx );
    tNodeIdx GetFirstElemInDir( tNodeIdx idx );
    tNodeIdx GetNextSiblingElem( tNodeIdx idx );
    
    // node info API - the life time of the retrun
    // strings are only between calls to another
    // call that returns strings.
    bool GetName( tNodeIdx idx, const char** name );
    bool GetName( tNodeIdx idx, const wchar_t** name );
    bool GetPath( tNodeIdx idx, const char** name );
    bool GetPath( tNodeIdx idx, const wchar_t** name );
    bool GetSrcPath( tNodeIdx idx, const char** name );
    bool GetSrcPath( tNodeIdx idx, const wchar_t** name );
    bool GetSize(    tNodeIdx idx,   unsigned long& size );
    bool GetLayer(   tNodeIdx idx,   unsigned int&  layer );
    bool GetBlockSize( tNodeIdx idx, unsigned long& blocks );
    bool GetStartLsn(  tNodeIdx idx, unsigned long& lsn );
    bool SetLayer(     tNodeIdx idx, unsigned int   layer );
    bool SetBlockSize( tNodeIdx idx, unsigned long  blocks );
    bool SetStartLsn(  tNodeIdx idx, unsigned long  lsn );
    bool IsDirectory(   tNodeIdx idx );
    bool IsFile(   tNodeIdx idx );
    bool IsPlaceHolder( tNodeIdx idx );
    bool IsXbe( tNodeIdx idx );
    bool IsEmpty( tNodeIdx idx );

    // Data blocks API
    unsigned long GetNumDataBlock();
    tNodeIdx GetDataBlockIndex( int placement );

// --- class specfic
public: IFld();
public: ~IFld();
// compiler generated...
private: IFld(const IFld& rhs); 
private: IFld& operator=(const IFld& rhs);
private: IFld* operator&();
private: const IFld* operator&() const;
    
// --- private data members
private:
    CFldImpl *mImpl;


// --- Private methods
private:
    void SetFileErrorMessage( DWORD nMessageId, bool bPrintStrerror = false );
    void SetNoArgErrorMessage( DWORD nMessageId );

    // For error messages only!!   
    std::wstring m_wszErrorMessage;
    std::wstring m_wszFileName;
};


#endif // IFLD_H
/* **************************** End of File ******************************* */

