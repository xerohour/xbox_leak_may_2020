//
// define the product identifiers and tags used to identify
// which MS tool built any particular object file
//
#pragma once
#if !defined(_prodids_h)
#define _prodids_h

// define the product ids, encodes version + language

enum PRODID {
    prodidUnknown       = 0x0000,
    prodidImport0       = 0x0001,   // Linker generated import object version 0
    prodidLinker510     = 0x0002,   // LINK 5.10 (Visual Studio 97 SP3)
    prodidCvtomf510     = 0x0003,   // LINK 5.10 (Visual Studio 97 SP3) OMF to COFF conversion
    prodidLinker600     = 0x0004,   // LINK 6.00 (Visual Studio 98)
    prodidCvtomf600     = 0x0005,   // LINK 6.00 (Visual Studio 98) OMF to COFF conversion
    prodidCvtres500     = 0x0006,   // CVTRES 5.00
    prodidUtc11_Basic   = 0x0007,   // VB 5.0 native code
    prodidUtc11_C       = 0x0008,   // VC++ 5.0 C/C++
    prodidUtc12_Basic   = 0x0009,   // VB 6.0 native code
    prodidUtc12_C       = 0x000a,   // VC++ 6.0 C
    prodidUtc12_CPP     = 0x000b,   // VC++ 6.0 C++
    prodidAliasObj60    = 0x000c,   // ALIASOBJ.EXE (CRT Tool that builds OLDNAMES.LIB)
    prodidVisualBasic60 = 0x000d,   // VB 6.0 generated object
    prodidMasm613       = 0x000e,   // MASM 6.13
    prodidMasm701       = 0x000f,   // MASM 7.01
    prodidLinker511     = 0x0010,   // LINK 5.11
    prodidCvtomf511     = 0x0011,   // LINK 5.11 OMF to COFF conversion
    prodidMasm614       = 0x0012,   // MASM 6.14 (MMX2 support)
    prodidLinker512     = 0x0013,   // LINK 5.12
    prodidCvtomf512     = 0x0014,   // LINK 5.12 OMF to COFF conversion
    prodidUtc12_C_Std   = 0x0015,   // VC++ 6.0 C standard edition
    prodidUtc12_CPP_Std = 0x0016,   // VC++ 6.0 C++ standard edition
    prodidUtc12_C_Book  = 0x0017,   // VC++ 6.0 C book edition
    prodidUtc12_CPP_Book= 0x0018,   // VC++ 6.0 C++ book edition
    prodidLinker700     = 0x0019,   // LINK 7.00
    prodidCvtomf700     = 0x001a,   // LINK 7.00 OMF to COFF conversion
    prodidUtc13_Basic   = 0x001b,   // VB 7.0 native code
    prodidUtc13_C       = 0x001c,   // VC++ 7.0 C
    prodidUtc13_CPP     = 0x001d,   // VC++ 7.0 C++
    prodidLinker610     = 0x001e,   // LINK 6.10
    prodidCvtomf610     = 0x001f,   // LINK 6.10 OMF to COFF conversion
    prodidLinker601     = 0x0020,   // LINK 6.01
    prodidCvtomf601     = 0x0021,   // LINK 6.01 OMF to COFF conversion
    prodidUtc12_1_Basic = 0x0022,   // VB 6.1 native code
    prodidUtc12_1_C     = 0x0023,   // VC++ 6.1 C
    prodidUtc12_1_CPP   = 0x0024,   // VC++ 6.1 C++
};

#define DwProdidFromProdidWBuild(prodid, wBuild) ((((unsigned long) (prodid)) << 16) | (wBuild))
#define ProdidFromDwProdid(dwProdid)             ((PRODID) ((dwProdid) >> 16))
#define WBuildFromDwProdid(dwProdid)             ((dwProdid) & 0xFFFF)


    // Symbol name and attributes in coff symbol table (requires windows.h)

#define symProdIdentName    "@comp.id"
#define symProdIdentClass   IMAGE_SYM_CLASS_STATIC
#define symProdIdentSection IMAGE_SYM_ABSOLUTE


    // Define the image data format

typedef struct PRODITEM {
    unsigned long   dwProdid;          // Product identity
    unsigned long   dwCount;           // Count of objects built with that product
} PRODITEM;


enum {
    tagEndID    = 0x536e6144,
    tagBegID    = 0x68636952,
};

/*
  Normally, the DOS header and PE header are contiguous.  We place some data
  in between them if we find at least one tagged object file.

  struct {
    IMAGE_DOS_HEADER dosHeader;
    BYTE             rgbDosStub[N];          // MS-DOS stub
    PRODITEM         { tagEndID, 0 };        // start of tallies (Masked with dwMask)
    PRODITEM         { 0, 0 };               // end of tallies   (Masked with dwMask)
    PRODITEM         rgproditem[];           // variable sized   (Masked with dwMask)
    PRODITEM         { tagBegID, dwMask };   // end of tallies
    PRODITEM         { 0, 0 };               // variable sized
    IMAGE_PE_HEADER  peHeader;
    };

*/


#endif
