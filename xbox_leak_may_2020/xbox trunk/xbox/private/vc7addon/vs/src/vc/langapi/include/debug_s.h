#ifndef _DEBUG_S_H_
#define _DEBUG_S_H_

#include "unknwn.h"
#include "crefobj.h"
#include "ref.h"

#if defined(MSOBJ_LIBRARY)
#define IMPORT_EXPORT
#else
#if !defined(OBJECTFILE_IMPL)
#define IMPORT_EXPORT   __declspec(dllimport)
#else
#define IMPORT_EXPORT   __declspec(dllexport)
#endif
#endif

struct tagFRAMEDATA;
typedef struct tagFRAMEDATA FRAMEDATA;

// {D8047B7D-1A6F-4bbd-B05C-A67AFBFA25CF}
static const GUID IID_IDebugSSectionWriter = 
{ 0xd8047b7d, 0x1a6f, 0x4bbd, { 0xb0, 0x5c, 0xa6, 0x7a, 0xfb, 0xfa, 0x25, 0xcf } };

class IDebugSSectionWriter: public IUnknown
{
public:
    enum {
        BAD_INDEX = 0xffffffff
    };
	//
	//  FInitWriter - initialize the section writer
    //	fEmitColumns - true iff the line number information will contain column data
    //
    IMPORT_EXPORT static bool __cdecl FCreateWriter( bool fEmitColumns, IDebugSSectionWriter** ppWriter, DWORD sig, bool f1stSection );

    //
    // StartSection
    //  symidSection - symbol id for the associated section. All offsets in line numbers are
    //                  relative to the start of the associated section. If this is the first debug$S section
    //                  then it typically is not assocaited with another section, so symidSection should
    //                  be given as 0.
    // Multiple sections can be written by repeating the sequence:
    //      StartSection()
    //      .. other calls ..
    //      EndSection()
    //      GetSectionBytes()
    //      GetSectionRelocs()
    //
    virtual bool StartSection( DWORD offset, WORD segment, DWORD cb = 0 ) = 0;
    virtual bool StartSectionSymId( DWORD symidSection, DWORD cb = 0 ) = 0;
	//
	// Append bytes to the symbols sub-stream
	//	pb - pointer to bytes to append
	//	cb - number of bytes to append
    //  poffset - if non-null then return section offset of bytes just appended
	//
	virtual bool AppendToSymbols( BYTE* pb, DWORD cb, DWORD* poffset ) = 0;
    //
    // Append a relocation to this section.
    virtual bool AppendRelocation( DWORD offset, DWORD symid, WORD type ) = 0;
    //
    // Add a string to the string table
    //      sz - the string
    // returns the string offset in the table
    //
	virtual DWORD AddString( const wchar_t* sz ) = 0;
	virtual DWORD AddUtf8String( const char* sz ) = 0;      // utf-8 encoded version
	//
	//   Name the file for the next set of line numbers to emit. Succeeding calls to AddLines routines 
    //   will assume this source file.
 	// 	    szFile - filename
    //      pbChksum - pointer to chksum bytes
    //      cbChksum - number of bytes of chksum
    //      chksumType - Chksum type
    //   Returns an index for the file in the source file table
	//
    virtual DWORD AddSourceFile( const wchar_t* szFilename, const BYTE* pbChksum, size_t cbChksum, DWORD chksumType ) = 0;
    //
	// Add a line with no column information
    //  idFile - a return value from AddSourceFile
	//	offset - offset from section start
	//	linenumStart - line number for start of statement
	//	linenumEnd - line number for end of statement, or zero if not emitting start/end pairs
    //  fStatement - true if this is a statement line number, otherwise it is an expression line.
	//
    // AddLine calls must occur in ascending order by offset. 
	virtual bool AddLine( DWORD idFile, DWORD offset, DWORD linenumStart, DWORD linenumEnd = 0, bool fStatement = true ) = 0;
	//
	// Add a line with column information
    //  idFile - a return value from AddSourceFile
	//	offset - offset from section start
	//	linenumStart - line number for start of statement
	//	boColumnStart - byte offset on source line for start of statement
	//	linenumEnd - line number for end of statement, or zero if not emitting start/end pairs
	//	boColumnEnd - byte offset on source line for end of statement, or zero
    //  fStatement - true if this is a statement line number, otherwise it is an expression line.
	//
    // AddLineWithColumn calls must occur in ascending order by offset. 
	virtual bool AddLineWithColumn( DWORD idFile, DWORD offset, DWORD linenumStart, DWORD linenumEnd = 0,
        DWORD offColumnStart = 0, DWORD offColumnEnd = 0, bool fStatement = true ) = 0;

    //
    // Add FRAMEDATA
    //
    //  Any rva fields of the framedata are section relative when added. The linker will adjust them
    //  to true rva's, no reloc's are necessary.
    //
    virtual bool AddFrameData( FRAMEDATA* pData ) = 0;

    //
    // EndSection - complete the section
    //
    virtual bool EndSection( DWORD cb = 0 ) pure;    
    //
    // GetSection[Bytes, Relocs] -
    //      Get the raw bytes and relocations for this debug$S section
    //      Pointer's returned are to memory managed by the IDebugSSectionWriter, they are
    //      good while the object exists and no calls to StartSection have been executed.
    // Must first call EndSection.
    //
    virtual size_t GetSectionBytes( BYTE** ppbSection ) = 0;
    virtual size_t GetSectionRelocs( BYTE** ppbRelocs ) = 0;
};

class IDebugSSubSectionEnum;
class IDebugSStringEnum;
class IDebugSLineBlockEnum;
class IDebugSLineEnum;
class IDebugSFileEnum;
class IDebugSStringTable;
class IDebugSFileInfo;

interface MapStrs
{
    virtual bool mapStrId( DWORD oldId, DWORD* newId ) = 0;
};

//
// QI the IDebugSSubSection based on Type() results for the correct SubSection interface
//
// {7DEF5160-D85A-472c-8644-1C5479AF1385}
static const GUID IID_IDebugSSubSection = 
{ 0x7def5160, 0xd85a, 0x472c, { 0x86, 0x44, 0x1c, 0x54, 0x79, 0xaf, 0x13, 0x85 } };

class IDebugSSubSection: public IUnknown
{
public:
    virtual DWORD Type() = 0;   // one of DEBUG_S_SUBSECTION_TYPE
    //
    // Get a pointer to the data, return the size of the data
    //
    virtual size_t GetData( BYTE** ppb ) = 0;       // bytes of the data part of the subsection
    virtual size_t GetRawBytes( BYTE** ppb ) = 0;   // raw bytes of entire subsection (incl's header)
    virtual void Dump() = 0;                        // dump to std out
    virtual bool remap( MapStrs* pRemaper ) = 0;    // remap string table ids to a new string table
};

// {F9C7819B-4FE7-43cc-A337-D2B93807EBF2}
static const GUID IID_IDebugSSubSectionEnum = 
{ 0xf9c7819b, 0x4fe7, 0x43cc, { 0xa3, 0x37, 0xd2, 0xb9, 0x38, 0x7, 0xeb, 0xf2 } };

class IDebugSSubSectionEnum: public IUnknown
{
public:
    virtual void Reset() = 0;
    virtual bool Next() = 0;
    virtual void Get( IDebugSSubSection** ppSubSection ) = 0;
    virtual bool clone( IDebugSSubSectionEnum **ppClone ) = 0;
};

// {C03407A4-245F-416c-B465-F9EE9209C1D8}
static const GUID IID_IDebugSSectionReader = 
{ 0xc03407a4, 0x245f, 0x416c, { 0xb4, 0x65, 0xf9, 0xee, 0x92, 0x9, 0xc1, 0xd8 } };

class IDebugSSectionReader: public IUnknown
{
public:
    IMPORT_EXPORT static bool __cdecl FCreateReader( BYTE* pb, size_t cb, IDebugSSectionReader** ppReader, DWORD sig );
    
    virtual bool GetSectionEnum( IDebugSSubSectionEnum** ppEnum ) = 0;
    virtual bool GetStringTable( IDebugSStringTable** ppSection ) = 0;
    virtual bool GetFileInfo( IDebugSFileInfo** ppSection ) = 0;
};

// {E9B6D889-CC8B-46c9-9B3C-57643BB8680E}
static const GUID IID_IDebugSSymbols = 
{ 0xe9b6d889, 0xcc8b, 0x46c9, { 0x9b, 0x3c, 0x57, 0x64, 0x3b, 0xb8, 0x68, 0xe } };

class IDebugSSymbols: public IUnknown
{
public:
    //
    // Get a pointer to the buffer of symbol bytes, return the size of the buffer
    //
    virtual size_t GetData( BYTE** ppb ) = 0;
};

// {75A7CDBA-5FC4-41c1-B4BC-294090508A03}
static const GUID IID_IDebugSStringEnum = 
{ 0x75a7cdba, 0x5fc4, 0x41c1, { 0xb4, 0xbc, 0x29, 0x40, 0x90, 0x50, 0x8a, 0x3 } };

class IDebugSStringEnum: public IUnknown
{
public:
    virtual void Reset() = 0;
    virtual bool Next() = 0;
    //
    // Get the string and its offset in the section
    //
    virtual void GetUtf8( char** ppUtf8Str, DWORD* pOff ) = 0;   
    virtual void Get( wchar_t* szStr, DWORD* pccStr, DWORD* pOff ) = 0;   
};

// {B0C076E0-F631-465e-9945-8D083FFEE95E}
static const GUID IID_IDebugSStringTable = 
{ 0xb0c076e0, 0xf631, 0x465e, { 0x99, 0x45, 0x8d, 0x8, 0x3f, 0xfe, 0xe9, 0x5e } };

class IDebugSStringTable: public IUnknown
{
public:
    virtual bool GetStringEnum( IDebugSStringEnum** ppEnum ) = 0;
    virtual bool GetStringByOff( DWORD off, char** ppUtf8Str ) = 0;
};

// {D9F9F9A5-3C35-4c7d-95B1-AF55F899EF0F}
static const GUID IID_IDebugSFileEnum = 
{ 0xd9f9f9a5, 0x3c35, 0x4c7d, { 0x95, 0xb1, 0xaf, 0x55, 0xf8, 0x99, 0xef, 0xf } };

class IDebugSFileEnum: public IUnknown
{
public:
    virtual void Reset() = 0;
    virtual bool Next() = 0;
    //
    // Get the string offset, and its checksum info
    //
    virtual void Get( DWORD* idFile, DWORD* pOffString, DWORD* pChksumType, BYTE** ppChksum, DWORD* pcbChksum ) = 0;   
};
// {F71E95AA-95E9-4549-B72D-A0F4E1C576BA}
static const GUID IID_IDebugSFileInfo = 
{ 0xf71e95aa, 0x95e9, 0x4549, { 0xb7, 0x2d, 0xa0, 0xf4, 0xe1, 0xc5, 0x76, 0xba } };

class IDebugSFileInfo: public IUnknown
{
public:
    virtual bool GetFileEnum( IDebugSFileEnum** ppEnum ) = 0;
    virtual bool GetFileById( DWORD index, DWORD* pOffString, DWORD* pChksumType, BYTE** ppChksum, DWORD* pcbChksum ) = 0;
};

// {B3D97B8F-ADAC-49c9-8C04-B773F34DACFE}
static const GUID IID_IDebugSLineEnum = 
{ 0xb3d97b8f, 0xadac, 0x49c9, { 0x8c, 0x4, 0xb7, 0x73, 0xf3, 0x4d, 0xac, 0xfe } };

class IDebugSLineEnum: public IUnknown
{
public:
    virtual void Reset() = 0;
    virtual bool Next() = 0;
    //
    // Get the line details
    //
    //virtual void Get( DWORD* pindex) = 0;
    virtual void Get( CV_Line_t* pLine, CV_Column_t* pColumn ) = 0;
};

// {2BBCDB3E-D4AC-4076-8F0A-FACF3E6132A6}
static const GUID IID_IDebugSLineBlockEnum = 
{ 0x2bbcdb3e, 0xd4ac, 0x4076, { 0x8f, 0xa, 0xfa, 0xcf, 0x3e, 0x61, 0x32, 0xa6 } };

class IDebugSLineBlockEnum: public IUnknown
{
public:
    virtual void Reset() = 0;
    virtual bool Next() = 0;
    //
    // Get the file id
    //
    virtual void GetFileId( DWORD* pidFile ) = 0;
    virtual bool GetLineEnum( IDebugSLineEnum** ppEnum ) = 0;
    virtual void Get( DWORD* cLines, CV_Line_t** ppLines, CV_Column_t** ppColumns ) = 0;
    virtual bool GetSegOffset( WORD* pSeg, DWORD* pOffset, DWORD* pcb ) = 0;
    virtual bool clone( IDebugSLineBlockEnum **ppClone ) = 0;
};

// {A55C0A25-5402-4419-830D-903068898E13}
static const GUID IID_IDebugSLines = 
{ 0xa55c0a25, 0x5402, 0x4419, { 0x83, 0xd, 0x90, 0x30, 0x68, 0x89, 0x8e, 0x13 } };

class IDebugSLines: public IUnknown
{
public:
    virtual DWORD GetFlags() = 0;
    virtual bool GetSegOffset( WORD* pSeg, DWORD* pOffset, DWORD* pcb ) = 0;
    //virtual void GetLineEnum( IDebugSLineEnum** ppEnum ) = 0;
    virtual bool GetBlockEnum( IDebugSLineBlockEnum** ppEnum ) = 0;
};

// {6260200D-7F1E-44a1-8998-FCC8A0EEB4A0}
static const GUID IID_IDebugSFrameDataEnum = 
{ 0x6260200d, 0x7f1e, 0x44a1, { 0x89, 0x98, 0xfc, 0xc8, 0xa0, 0xee, 0xb4, 0xa0 } };

class IDebugSFrameDataEnum: public IUnknown
{
public:
    virtual void Reset() = 0;
    virtual bool Next() = 0;
    virtual void Get( FRAMEDATA* pData ) = 0;   
};

// {9BB21FD2-1F4B-46dc-8E03-AC7BED2730B4}
static const GUID IID_IDebugSFrameData = 
{ 0x9bb21fd2, 0x1f4b, 0x46dc, { 0x8e, 0x3, 0xac, 0x7b, 0xed, 0x27, 0x30, 0xb4 } };


class IDebugSFrameData: public IUnknown
{
public:
    virtual bool GetFrameDataEnum( IDebugSFrameDataEnum** ppEnum ) = 0;
    virtual bool GetFrameDataByRva( DWORD rva, FRAMEDATA* pData ) = 0;
    virtual bool GetSectionRva( DWORD* prva ) = 0;
    virtual bool ApplyReloc() = 0;
};
#endif
