/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright (c) Microsoft Corporation.  All rights reserved.
*
* File Comments:
*
*
***********************************************************************/

#pragma once

#pragma pack(push, 1)

struct CVSEG
{
   struct
   {
      WORD  fRead    : 1;	       // Segment is readable
      WORD  fWrite   : 1;	       // Segment is writable
      WORD  fExecute : 1;	       // Segment is executable
      WORD  f32Bit   : 1;	       // Segment is 32 bit
      WORD	     : 4;
      WORD  fSel     : 1;	       //
      WORD  fAbs     : 1;	       // Segment is absolute
      WORD	     : 2;
      WORD  fGroup   : 1;	       // This is a group (not a segment)
      WORD	     : 3;
   }	    wFlags;
   WORD     iovl;		       // Overlay index
   WORD     igr;		       // Group index
   WORD     isegPhysical;	       // Physical segment index
   WORD     ichName;		       // Offset of segment name or 0xFFFF
   WORD     ichClassName;	       // Offset of class name or 0xFFFF
   DWORD    ib; 		       // Offset within physical segment
   DWORD    cb; 		       // Size
};


struct CVSEGMAP
{
   WORD     ccvsegMax;
   WORD     ccvsegLogical;
   CVSEG    rgcvseg[1];
};


/**	CodeView Symbol and Type OMF type information is broken up into two
 *	ranges.  Type indices less than 0x1000 describe type information
 *	that is frequently used.  Type indices above 0x1000 are used to
 *	describe more complex features such as functions, arrays and
 *	structures.
 */

/**	Primitive types have predefined meaning that is encoded in the
 *	values of the various bit fields in the value.
 *
 *	A CodeView primitive type is defined as:
 *
 *	1 1
 *	1 089  7654  3	210
 *	r mode type  r	sub
 *
 *	Where
 *	    mode is the pointer mode
 *	    type is a type indicator
 *	    sub  is a subtype enumeration
 *	    r	 is a reserved field
 *
 *	See Microsoft Symbol and Type OMF (Version 4.0) for more
 *	information.
 */


// --------------------------------------------------------------------
// Encoded Values
// --------------------------------------------------------------------

   // In type descriptions, there are sometimes embedded values.  Where
   // these occur, the next 16 bit word is examined.  If the value of this
   // word is less than 0x8000, then the word contains the desired value.
   // Otherwise, the word contains a value type and the desired value
   // follows this word.

enum CVVALT			       // CodeView Value Type
{
   cvvaltInt8		= 0x8000,      // 8 bit signed integer
   cvvaltInt16		= 0x8001,      // 16 bit signed integer
   cvvaltUInt16 	= 0x8002,      // 16 bit unsigned integer
   cvvaltInt32		= 0x8003,      // 32 bit signed integer
   cvvaltUInt32 	= 0x8004,      // 32 bit unsigned integer
   cvvaltReal32 	= 0x8005,      // 32 bit IEEE floating point
   cvvaltReal64 	= 0x8006,      // 64 bit IEEE floating point
   cvvaltReal80 	= 0x8007,      // 80 bit Intel x87 floating point
   cvvaltReal128	= 0x8008,
   cvvaltInt64		= 0x8009,      // 64 bit signed integer
   cvvaltUInt64 	= 0x800a,      // 64 bit unsigned integer
   cvvaltReal48 	= 0x800b,
   cvvaltComplex32	= 0x800c,      // 32 bit IEEE floating point complex
   cvvaltComplex64	= 0x800d,      // 64 bit IEEE floating point complex
   cvvaltComplex80	= 0x800e,      // 80 bit Intel x87 floating point complex
   cvvaltComplex128	= 0x800f,
   cvvaltString 	= 0x8010,      // Variable length string
};

struct CVVAL
{
   WORD  cvvalt;
};


struct CVVALINT8 : public CVVAL
{
   BYTE  ch;			       // Signed 8-bit integer
};


struct CVVALINT16 : public CVVAL
{
   WORD  w;			       // Signed 16-bit integer
};


struct CVVALUINT16 : public CVVAL
{
   WORD  w;			       // Unsigned 16-bit integer
};


struct CVVALINT32 : public CVVAL
{
   DWORD dw;			       // Signed 32-bit integer
};


struct CVVALUINT32 : public CVVAL
{
   DWORD dw;			       // Unsigned 32-bit integer
};


struct CVVALREAL32 : public CVVAL
{
   DWORD dw;			       // 32 bit IEEE floating point
};


struct CVVALREAL64 : public CVVAL
{
   BYTE  rgb[8];		       // 64 bit IEEE floating point
};


struct CVVALREAL80 : public CVVAL
{
   BYTE  rgb[10];		       // 80 bit Intel x87 floating point
};


struct CVVALREAL128 : public CVVAL
{
   BYTE  rgb[16];		       //
};


struct CVVALINT64 : public CVVAL
{
   BYTE  rgb[8];		       // 64 bit signed integer
};


struct CVVALUINT64 : public CVVAL
{
   BYTE  rgb[8];		       // 64 bit unsigned integer
};


struct CVVALREAL48 : public CVVAL
{
   BYTE  rgb[6];		       // 64 bit unsigned integer
};


struct CVVALCOMPLEX32 : public CVVAL
{
   DWORD dwReal;		       // 32 bit IEEE floating point
   DWORD dwImaginary;		       // "
};


struct CVVALCOMPLEX64 : public CVVAL
{
   BYTE  rgbReal[8];		       // 64 bit IEEE floating point
   BYTE  rgbImaginary[8];	       // "
};


struct CVVALCOMPLEX80 : public CVVAL
{
   BYTE  rgbReal[10];		       // 80 bit Intel x87 floating point
   BYTE  rgbImaginary[10];	       // "
};


struct CVVALCOMPLEX128 : public CVVAL
{
   BYTE  rgbReal[16];		       //
   BYTE  rgbImaginary[16];	       //
};


struct CVVALSTRING : public CVVAL
{
   WORD  cch;			       // Length of value in characters
   char  rgch[1];		       // Value
};


// --------------------------------------------------------------------
// Type Indices
// --------------------------------------------------------------------

   // A CodeView type index is a sixteen bit value.  Values < 0x1000 identify
   // reserved types.  Values 0x1000 and above are used indices into a list
   // of programmer defined types located in the GlobalTypes subsection of
   // a CVPACKed executable or a PreComp or Types subsection of an unpacked
   // executable.

typedef DWORD CVTI32;		       // CodeView Type Index (32 bit)


enum CVTM			       // CodeView Type Mode
{
   cvtmDirect		= 0,	       // Not a pointer
   cvtmNPtr		= 1,	       // Near pointer (0:16)
   cvtmFPtr		= 2,	       // Far pointer (16:16)
   cvtmHPtr		= 3,	       // Huge pointer (16:16)
   cvtmNPtr32		= 4,	       // Near pointer (0:32)
   cvtmFPtr32		= 5,	       // Far pointer (16:32)
   cvtmNPtr64		= 6,	       // Near pointer (0:64)
};


enum CVPT			       // CodeView Primitive Type
{
   // Special Types

   cvptNoType		= 0x0000,      // No type assigned
   cvptAbs		= 0x0001,      // Absolute symbol
   cvptSegment		= 0x0002,      // Segment type
   cvptVoid		= 0x0003,      // void
   cvptCurrency 	= 0x0004,      // BASIC Currency (64 bit integer scaled by 10000)
   cvptNBasicStr	= 0x0005,      // Near BASIC string
   cvptFBasicStr	= 0x0006,      // Far BASIC string
   cvptNotTrans 	= 0x0007,      // Type not translated by cvpack
   cvptHresult		= 0x0008,      // OLE/COM HRESULT

   // Signed integral types

   cvptChar		= 0x0010,      // Signed character
   cvptShort		= 0x0011,      // Signed short
   cvptLong		= 0x0012,      // Signed long
   cvptLongLong 	= 0x0013,      // Signed long long

   // Unsigned integral types

   cvptUChar		= 0x0020,      // Unsigned character
   cvptUShort		= 0x0021,      // Unsigned short
   cvptULong		= 0x0022,      // Unsigned long
   cvptULongLong	= 0x0023,      // Unsigned long long

   // Boolean types

   cvptBool8		= 0x0030,      // 8 bit boolean
   cvptBool16		= 0x0031,      // 16 bit boolean
   cvptBool32		= 0x0032,      // 32 bit boolean
   cvptBool64		= 0x0033,      // 64 bit boolean

   // Floating point types

   cvptReal32		= 0x0040,      // 32 bit real
   cvptReal64		= 0x0041,      // 64 bit real
   cvptReal80		= 0x0042,      // 80 bit real
   cvptReal128		= 0x0043,      // 128 bit real
   cvptReal48		= 0x0044,      // 48 bit real

   // Floating point complex types

   cvptCplx32		= 0x0050,      // 32 bit complex
   cvptCplx64		= 0x0051,      // 64 bit complex
   cvptCplx80		= 0x0052,      // 80 bit complex
   cvptCplx128		= 0x0053,      // 128 bit complex

   // MOre special types

   cvptBit		= 0x0060,      // Bit
   cvptPasChar		= 0x0061,      // Pascal CHAR

   // Integral types

   cvptRChar		= 0x0070,      // Char
   cvptWChar		= 0x0071,      // Wide char
   cvptInt16		= 0x0072,      // 16 bit signed int
   cvptUInt16		= 0x0073,      // 16 bit unsigned int
   cvptInt32		= 0x0074,      // 32 bit signed int
   cvptUInt32		= 0x0075,      // 32 bit unsigned int
   cvptInt64		= 0x0076,      // 64 bit signed int
   cvptUInt64		= 0x0077,      // 64 bit unsigned int

   // CodeView internal types

   cvptCvPtr		= 0x00f0,      // Internal type for created pointers
};


inline CVTM CvtmFromCvti(CVTI32 cvti)
{
   return((CVTM) ((cvti & 0x0F00) >> 8));
}


inline CVPT CvptFromCvti(CVTI32 cvti)
{
   return((CVPT) (cvti & 0x00FF));
}


// --------------------------------------------------------------------
// Type Definition Leaves
// --------------------------------------------------------------------

enum CVLFT			       // CodeView Leaf Type
{
   // Leaf types that start records referenced from symbol records

   cvlftCOBOL1		= 0x000c,
   cvlftLabel		= 0x000e,
   cvlftNull		= 0x000f,
   cvlftNotTran 	= 0x0010,
   cvlftEndPrecomp	= 0x0014,      // Not referenced from symbol records
   cvlftTypeServer	= 0x0016,      // Not referenced from symbol records

   // Leaf types used to indicate pading for alignment

   cvlftPad0		= 0x00f0,
   cvlftPad1		= 0x00f1,
   cvlftPad2		= 0x00f2,
   cvlftPad3		= 0x00f3,
   cvlftPad4		= 0x00f4,
   cvlftPad5		= 0x00f5,
   cvlftPad6		= 0x00f6,
   cvlftPad7		= 0x00f7,
   cvlftPad8		= 0x00f8,
   cvlftPad9		= 0x00f9,
   cvlftPad10		= 0x00fa,
   cvlftPad11		= 0x00fb,
   cvlftPad12		= 0x00fc,
   cvlftPad13		= 0x00fd,
   cvlftPad14		= 0x00fe,
   cvlftPad15		= 0x00ff,

   // Leaf types that start records referenced from other type records

   cvlftList		= 0x0203,
   cvlftRefSym		= 0x020c,

   // Leaf types used in lists

   cvlftEnumerate	= 0x0403,

   cvlftModifier32	= 0x1001,
   cvlftPointer32	= 0x1002,
   cvlftArray32 	= 0x1003,
   cvlftClass32 	= 0x1004,
   cvlftStructure32	= 0x1005,
   cvlftUnion32 	= 0x1006,
   cvlftEnum32		= 0x1007,
   cvlftProcedure32	= 0x1008,
   cvlftMFunction32	= 0x1009,
   cvlftVtShape32	= 0x100a,
   cvlftCOBOL032	= 0x100b,
   cvlftBArray32	= 0x100d,
   cvlftAlias32 	= 0x1010,      // Alias (typedef) type
   cvlftDimArray32	= 0x1011,
   cvlftVftPath32	= 0x1012,
   cvlftPrecomp32	= 0x1013,      // Not referenced from symbol records
   cvlftOEM32		= 0x1015,      // OEM definable type string

   // Leaf types that start records referenced from other type records

   cvlftSkip32		= 0x1200,
   cvlftArgList32	= 0x1201,
   cvlftDefArg32	= 0x1202,
   cvlftFieldList32	= 0x1203,
   cvlftDerived32	= 0x1204,
   cvlftBitField32	= 0x1205,
   cvlftMethodList32	= 0x1206,
   cvlftDimConU32	= 0x1207,
   cvlftDimConLU32	= 0x1208,
   cvlftDimVarU32	= 0x1209,
   cvlftDimVarLU32	= 0x120a,

   // Leaf types used in lists

   cvlftBClass32	= 0x1400,
   cvlftVBClass32	= 0x1401,
   cvlftIVBClass32	= 0x1402,
   cvlftFriendFcn32	= 0x1403,
   cvlftIndex32 	= 0x1404,
   cvlftMember32	= 0x1405,
   cvlftStMember32	= 0x1406,
   cvlftMethod32	= 0x1407,
   cvlftNestType32	= 0x1408,
   cvlftVFuncTab32	= 0x1409,
   cvlftFriendCls32	= 0x140a,
   cvlftOneMethod32	= 0x140b,
   cvlftVFuncOff32	= 0x140c,
   cvlftNestTypeEx32	= 0x140d,
   cvlftMemberModify32	= 0x140e,

   // Leaf types w/zero terminated names

   cvlftTypeServer_Sz	= 0x1501,      // Not referenced from symbol
   cvlftEnumerate_Sz	= 0x1502,
   cvlftArray		= 0x1503,
   cvlftClass		= 0x1504,
   cvlftStructure	= 0x1505,
   cvlftUnion		= 0x1506,
   cvlftEnum		= 0x1507,
   cvlftDimArray	= 0x1508,
   cvlftPrecomp 	= 0x1509,      // Not referenced from symbol
   cvlftAlias		= 0x150a,      // alias (typedef) type
   cvlftDefArg		= 0x150b,
   cvlftFriendFcn	= 0x150c,
   cvlftMember		= 0x150d,
   cvlftStMember	= 0x150e,
   cvlftMethod		= 0x150f,
   cvlftNestType	= 0x1510,
   cvlftOneMethod	= 0x1511,
   cvlftNestTypeEx	= 0x1512,
   cvlftMemberModify	= 0x1513,
   cvlftManaged 	= 0x1514,
};


enum CVLABT			       // CodeView Label Type
{
   cvlabtNear		= 0,
   cvlabtFar		= 4,
};


//  Structures to access to the type records

enum CVPMT			       // CodeView Pointer to Member Type
{
   cvpmtFnNoVirtSA32	= 0x0B,        // 16:32 method nonvirtual single address point
   cvpmtFnNoVirtMA32	= 0x0C,        // 16:32 method nonvirtual multiple address point
   cvpmtFnVirtBase32	= 0x0D,        // 16:32 method virtual bases
};



// --------------------------------------------------------------------
// Types used in CodeView Leaf Definitions
// --------------------------------------------------------------------

   // CVCP is used by CVLFCLASS, CVLFUNION, and CVLFENUM

struct CVCP			       // CodeView Class Properties
{
   WORD fPacked 	: 1;	       // Structure is packed
   WORD fHasConstructor : 1;	       // Constructors or destructors present
   WORD fOverloadedOps	: 1;	       // Overloaded operators present
   WORD fNested 	: 1;	       // This is a nested class
   WORD fHasNestedTypes : 1;	       // Contains nested types
   WORD fOverloadedAsgn : 1;	       // Overloaded assignment (=)
   WORD fCastOps	: 1;	       // Casting methods
   WORD fForwardRef	: 1;	       // Forward reference (incomplete defn)
   WORD fScoped 	: 1;	       // Scoped definition
   WORD 		: 7;
};


   // CVCALLT is used by CVLFPROC and CVLFMFUNC

enum CVCALLT			       // CodeView Call Type
{
   cvcalltC		= 0x00,        // __cdecl
   cvcalltCFar		= 0x01,        // __cdecl __far
   cvcalltPascal	= 0x02,        // __pascal/__fortran
   cvcalltPascalFar	= 0x03,        // __pascal/__fortran __far
   cvcalltFastcall	= 0x04,        // __fastcall
   cvcalltFastcallFar	= 0x05,        // __fastcall __far

   cvcalltStdcall	= 0x07,        // __stdcall
   cvcalltStdcallFar	= 0x08,        // __stdcall __far
   cvcalltSyscall	= 0x09,        // __syscall
   cvcalltSyscallFar	= 0x0a,        // __syscall __far
   cvcalltThis		= 0x0b,        // Used for member functions
   cvcalltMips		= 0x0c,        // MS MIPS compiler
   cvcalltGeneric	= 0x0d,        // Generic call sequence
   cvcalltAlpha 	= 0x0e,        // MS Alpha compiler
};


   // CVAP is used in CVFP

enum CVAP			       // CodeView Access Protection
{
   cvapPrivate		= 1,
   cvapProtected	= 2,
   cvapPublic		= 3,
};


   // CVMP is used in CVFP

enum CVMP			       // CodeView Method Property
{
   cvmpVanilla		= 0x00,
   cvmpVirtual		= 0x01,
   cvmpStatic		= 0x02,
   cvmpFriend		= 0x03,
   cvmpIntro		= 0x04,
   cvmpPureVirtual	= 0x05,
   cvmpPureIntro	= 0x06,
};


   // CVFP is used in

struct CVFP			       // CodeView Field Properties
{
   WORD cvap		:  2;	       // Access protection (CVAP)
   WORD cvmp		:  3;	       // Method properties (CVMP)
   WORD fPseudo 	:  1;	       // compiler generated fcn and does not exist
   WORD fNoInherit	:  1;	       // Class cannot be inherited
   WORD fNoConstruct	:  1;	       // Class cannot be constructed
   WORD 		:  8;
};


// --------------------------------------------------------------------
// CodeView Leaf Definitions
// --------------------------------------------------------------------

struct CVLF			       // CodeView Leaf
{
   WORD  cvlft; 		       // Leaf type
};


struct CVLFMODIFIER32 : public CVLF
{
   CVTI32 cvti; 		       // Modified type
   WORD   fConst	: 1;
   WORD   fVolatile	: 1;
   WORD   fUnaligned	: 1;
   WORD 		: 13;
};


//	type record for LF_POINTER

//	Type enum for pointer records
//	Pointers can be one of the following types

enum CV_ptrtype_e
{
   CV_PTR_NEAR	       = 0x00, // near pointer
   CV_PTR_FAR	       = 0x01, // far pointer
   CV_PTR_HUGE	       = 0x02, // huge pointer
   CV_PTR_BASE_SEG     = 0x03, // based on segment
   CV_PTR_BASE_VAL     = 0x04, // based on value of base
   CV_PTR_BASE_SEGVAL  = 0x05, // based on segment value of base
   CV_PTR_BASE_ADDR    = 0x06, // based on address of base
   CV_PTR_BASE_SEGADDR = 0x07, // based on segment address of base
   CV_PTR_BASE_TYPE    = 0x08, // based on type
   CV_PTR_BASE_SELF    = 0x09, // based on self
   CV_PTR_NEAR32       = 0x0a, // 16:32 near pointer
   CV_PTR_FAR32        = 0x0b, // 16:32 far pointer
   CV_PTR_64	       = 0x0c, // 64 bit pointer
   CV_PTR_UNUSEDPTR    = 0x0d  // first unused pointer type
};


//	Mode enum for pointers
//	Pointers can have one of the following modes

enum CV_ptrmode_e
{
   CV_PTR_MODE_PTR     = 0x00, // "normal" pointer
   CV_PTR_MODE_REF     = 0x01, // reference
   CV_PTR_MODE_PMEM    = 0x02, // pointer to data member
   CV_PTR_MODE_PMFUNC  = 0x03, // pointer to member function
   CV_PTR_MODE_RESERVED= 0x04  // first unused pointer mode
};


struct CVLFPOINTERBODY32 : public CVLF
{
   CVTI32 cvti; 		       // Type index of the base type

   struct lfPointerAttr32
   {
      DWORD  ptrtype	 :5;	       // ordinal specifying pointer type (ptrtype-t)
      DWORD  ptrmode	 :3;	       // ordinal specifying pointer mode (ptrmode_t)
      DWORD  fFlat32	 :1;	       // true if 0:32 pointer
      DWORD  fVolatile	 :1;	       // TRUE if volatile pointer
      DWORD  fConst	 :1;	       // TRUE if const pointer
      DWORD  fUnaligned  :1;	       // TRUE if unaligned pointer
      DWORD  fRestricted :1;	       // TRUE if restricted pointer
      DWORD		 :4;
   } attr;
};

struct CVLFPOINTER32 : public CVLFPOINTERBODY32
{
   union
   {
      struct
      {
	 CVTI32 pmclass;	       // index of containing class for pointer to member
	 WORD	pmenum; 	       // enumeration specifying pm format
      } pm;

      WORD  bseg;		       // base segment if PTR_BASE_SEG

      BYTE  rgbSym[1];		       // copy of base symbol record (including length)

      struct
      {
	 CVTI32 cvti;		       // type index if CV_PTR_BASE_TYPE
	 char	rgch[1];	       // name of base type
      } btype;
   };
};


struct CVLFARRAY32 : public CVLF
{
   CVTI32 cvti; 		       // Type index of element type
   CVTI32 cvtiIndex;		       // Type index of indexing type
   BYTE   rgb[1];		       // Array size and name (variable length)
};


struct CVLFCLASS32 : public CVLF
{
   WORD   cField;		       // Number of fields
   CVCP   cvcp; 		       // Class properties
   CVTI32 cvtiField;		       // Type index of field list (CVLFFIELD)
   CVTI32 cvtiDerived;		       // Type index of derived from list (or 0)
   CVTI32 cvtiVtShape;		       // Type index of vshape table
   BYTE   rgb[1];		       // Variable length data
};


struct CVLFUNION32 : public CVLF
{
   WORD   cField;		       // Number of fields
   CVCP   cvcp; 		       // Class properties
   CVTI32 cvtiField;		       // Type index of field list (CVLFFIELD)
   BYTE   rgb[1];		       // Variable length data
};


struct CVLFENUM32 : public CVLF
{
   WORD   cField;		       // Number of fields
   CVCP   cvcp; 		       // Class properties
   CVTI32 cvti; 		       // Base type
   CVTI32 cvtiField;		       // Type index of field list (CVLFFIELD)
   char   rgch[1];		       // Length prefixed name
};


struct CVLFPROC32 : public CVLF
{
   CVTI32 cvtiReturn;		       // Type index of return value
   BYTE   cvcallt;		       // Calling convention
   BYTE   bReserved;		       // Reserved
   WORD   cArg; 		       // Number of arguments
   CVTI32 cvtiArg;		       // Type index of arg list (CVLFARG)
};


struct CVLFMFUNC32 : public CVLF
{
   CVTI32 cvtiReturn;		       // Type index of return value
   CVTI32 cvtiClass;		       // Type index of containing class
   CVTI32 cvtiThis;		       // Type index of this pointer
   BYTE   cvcallt;		       // Calling convention
   BYTE   bReserved;		       // Reserved
   WORD   cArg; 		       // Number of arguments
   CVTI32 cvtiArg;		       // Type index of arg list (CVLFARG)
   DWORD  thisadjust;		       // this adjuster (long because pad required anyway)
};


enum CV_VTS_desc_e
{
   CV_VTS_near		= 0x00,
   CV_VTS_far		= 0x01,
   CV_VTS_thin		= 0x02,
   CV_VTS_outer 	= 0x03,
   CV_VTS_meta		= 0x04,
   CV_VTS_near32	= 0x05,
   CV_VTS_far32 	= 0x06,
   CV_VTS_unused	= 0x07,
};

struct CVLFVTSHAPE : public CVLF
{
   WORD  cFunc; 		       // Number of entries in vfunctable
   BYTE  rgb[1];		       // 4 bit (CV_VTS_desc) descriptors
};


struct CVLFCOBOL0_32 : public CVLF
{
   CVTI32 cvti; 		       // Parent type record index
   BYTE   rgb[1];
};


struct CVLFCOBOL1 : public CVLF
{
   BYTE  rgb[1];
};


struct CVLFBARRAY32 : public CVLF
{
   CVTI32 cvti; 		       // Type index of underlying type
};


struct CVLFLABEL : public CVLF
{
   WORD  mode;			       // Addressing mode of label (UNDONE)
};


struct CVLFDIMARRAY32 : public CVLF
{
   CVTI32 cvti; 		       // Type index of array element
   CVTI32 cvtiDim;		       // Type index of dimension (CVLFDIMCON/CVLFDIMVAR)
   char   rgch[1];		       // Length prefixed name
};


struct CVLFCVFPATH32 : public CVLF
{
   WORD   ccvti;		       // Count of number of bases in path
   CVTI32 rgcvtiBase[1];	       // Type index of bases from root to leaf
};


struct CVLFPRECOMP32 : public CVLF
{
   DWORD cvtiStart;		       // Starting type index included
   DWORD ccvti; 		       // Number of types in inclusion
   DWORD dwSignature;		       // Signature
   char  rgch[1];		       // Length prefixed name of header file
};


struct LFENDPRECOMP : public CVLF
{
   DWORD dwSignature;		       // signature
};


struct CVLFOEM32 : public CVLF
{
   WORD   wOEMID;		       // MS assigned OEM identified
   WORD   wOEMTYPE;		       // OEM assigned type identifier
   WORD   ccvti;		       // count of type indices to follow
   CVTI32 rgcvti[1];		       // array of type indices followed
				       // by OEM defined data
};


struct CVLFTYPESERVER : public CVLF
{
   DWORD dwSignature;		       // Signature
   DWORD dwAge; 		       // Age of database used by this module
   char  rgch[1];		       // Length prefixed name of .PDB file
};


struct CVLFSKIP32 : public CVLF
{
   CVTI32 cvti; 		       // Next valid type index
   BYTE   rgb[1];		       // Padding
};


struct CVLFARGLIST32 : public CVLF
{
   WORD   cArg; 		       // Number of arguments
   CVTI32 rgcvti[1];		       // Type index of each argument
};


struct CVLFDERIVED32 : public CVLF
{
   WORD   cClass;		       // Number of derived classes
   CVTI32 rgcvti[1];		       // Type indices of derived classes
};


struct CVLFDEFARG32 : public CVLF
{
   CVTI32 cvti; 		       // Type of resulting expression
   char   rgchExpr[1];		       // Length prefixed expression string
};


struct CVLFFIELDLIST : public CVLF
{
   CVLF  rgcvlf[1];		       // Variable number of leaves
};


//  type record for non-static methods and friends in overloaded method list

struct mlMethod32
{
   CVFP   cvfp; 		       // Field properties
   WORD   wPad;
   CVTI32 cvti; 		       // Index to type record for procedure
   DWORD  vbaseoff[1];		       // Offset in vfunctable if intro virtual
};


struct CVLFMETHODLIST : public CVLF
{
   WORD  cvlft;
   BYTE  mList[1];		       // Variable number of mlMethods
};


struct CVLFBITFIELD32 : public CVLF
{
   CVTI32 cvti; 		       // Base type
   BYTE   cbit; 		       // Number of bits in field
   BYTE   position;		       // Position of field (LSB == 0?)
};


struct CVLFDIMCON32 : public CVLF
{
   CVTI32 cvti; 		       // Type of index
   WORD   rank; 		       // Number of dimensions
   BYTE   rgb[1];		       // array of dimension information with
				       // either upper bounds or lower/upper bound
};


struct CVLFDIMVAR32 : public CVLF
{
   DWORD  rank; 		       // Number of dimensions
   CVTI32 cvti; 		       // Type of index
   CVTI32 rgcvti[1];		       // array of type indices for either
				       // variable upper bound or variable
				       // lower/upper bound.  The referenced
				       // types must be LF_REFSYM or T_VOID
};


struct CVLFREFSYM : public CVLF
{
   BYTE  rgb[1];		       // Copy of referenced symbol record
};



//***********************************************************************


//	index leaf - contains type index of another leaf
//	a major use of this leaf is to allow the compilers to emit a
//	long complex list (LF_FIELD) in smaller pieces.

struct CVLFINDEX32 : public CVLF
{
   WORD   wPad;
   CVTI32 cvti; 		       // Type index of referenced leaf
};


struct CVLFBCLASS32 : public CVLF      // Base Class
{
   CVFP   cvfp; 		       // Field properties
   CVTI32 cvti32;		       // Type index of base class
   CVVAL  cvval;		       // Offset of base within class (CVVAL)
};


struct CVLFVBCLASS32 : public CVLF     // Virtual Base Class
{
   CVFP   cvfp; 		       // Field properties
   CVTI32 cvti; 		       // Type index of direct virtual base class
   CVTI32 cvtiVbptr;		       // Type index of virtual base pointer
   CVVAL  cvval;		       // Virtual base pointer offset from address point
				       // followed by virtual base offset from vbtable
};


struct CVLFFRIENDCLS32 : public CVLF   // Friend Class
{
   WORD   wPad;
   CVTI32 cvti; 		       // Type index of friend class record
};


struct CVLFFRIENDFCN32 : public CVLF   // Friend Function
{
   WORD   wPad;
   CVTI32 cvti; 		       // Type index
   char   rgchName[1];		       // Length prefixed name
};


struct CVLFMEMBER32 : public CVLF      // Member Function
{
   CVFP   cvfp; 		       // Field properties
   CVTI32 cvti; 		       // Type index of member function
   CVVAL  cvval;		       // Offset of field (CVVAL) followed by name
};


struct CVLFSTMEMBER32 : public CVLF    // Static Member Function
{
   CVFP   cvfp; 		       // Field properties
   CVTI32 cvti; 		       // Type index of member function
   char   rgchName[1];		       // Length prefixed name
};


struct CVLFVFUNCTAB32 : public CVLF    // Virtual Function Table
{
   WORD   wPad;
   CVTI32 cvti; 		       // Type index of pointer
};


struct CVLFVFUNCOFF32 : public CVLF
{
   WORD   wPad;
   CVTI32 cvti; 		       // type index of pointer
   DWORD  ib;			       // offset of virtual function table pointer
};


struct CVLFMETHOD32 : public CVLF
{
   WORD   count;		       // number of occurances of function
   CVTI32 mList;		       // index to LF_METHODLIST record
   char   rgchName[1];		       // Length prefixed name
};


struct CVLFONEMETHOD32 : public CVLF
{
   CVFP   cvfp; 		       // Field properties
   CVTI32 cvti; 		       // index to type record for procedure
   DWORD  vbaseoff[1];		       // offset in vfunctable if
				       // intro virtual followed by
				       // length prefixed name of method
};


struct CVLFENUMERATE : public CVLF
{
   CVFP  cvfp;			       // Field properties
   CVVAL cvval; 		       // Value
// char  rgchName[1];		       // Length prefixed name
};


struct CVLFNESTTYPE32 : public CVLF    // Nested Type
{
   WORD   wPad;
   CVTI32 index;		       // Type index
   char   rgchName[1];		       // Length prefixed name
};


struct CVLFNESTTYPEEX : public CVLF    // Nested Type
{
   CVFP   cvfp; 		       // Field properties
   CVTI32 index;		       // Type index
   char   rgchName[1];		       // Length prefixed name
};


struct CVLFMEMBERMODIFY : public CVLF
{
   CVFP   cvfp; 		       // Field properties
   CVTI32 index;		       // Type index
   char   rgchName[1];		       // Length prefixed name
};


struct CVTYP
{
   WORD  cb;			       // Record size excluding this word
   CVLF  cvlf;			       // Leaf record (variable sized)
};


// --------------------------------------------------------------------
//
// --------------------------------------------------------------------

enum CVSYMT
{
   cvsymtCompile	= 0x0001,      // Compile flags symbol
   cvsymtSSearch	= 0x0005,      // Start Search
   cvsymtEnd		= 0x0006,      // Block, procedure, "with" or thunk end
   cvsymtSkip		= 0x0007,      // Reserve symbol space in $$Symbols table
   cvsymtCvReserved	= 0x0008,      // Reserved symbol for CV internal use
   cvsymtObjName	= 0x0009,      // path to object file name
   cvsymtEndArg 	= 0x000a,      // end of argument/return list
   cvsymtReturn 	= 0x000d,      // return description symbol
   cvsymtEntryThis	= 0x000e,      // description of this pointer on entry

   cvsymtThunk32	= 0x0206,      // Thunk Start
   cvsymtBlock32	= 0x0207,      // block start
   cvsymtWith32 	= 0x0208,      // with start
   cvsymtLabel32	= 0x0209,      // code label
   cvsymtCExModel32	= 0x020a,      // change execution model

   cvsymtProcRef	= 0x0400,      // Reference to a procedure
   cvsymtDataRef	= 0x0401,      // Reference to data
   cvsymtAlign		= 0x0402,      // Used for page alignment of symbols
   cvsymtLProcRef	= 0x0403,      // Local Reference to a procedure

   cvsymtRegister_32	= 0x1001,      // Register variable (32 bit type index)
   cvsymtConstant_32	= 0x1002,      // constant symbol (32 bit type index)
   cvsymtUDT_32 	= 0x1003,      // User defined type (32 bit type index)
   cvsymtCOBOLUDT_32	= 0x1004,      // special UDT for cobol that does not symbol pack (32 bit type index)
   cvsymtManyReg_32	= 0x1005,      // multiple register variable (32 bit type index)
   cvsymtBPRel32_32	= 0x1006,      // BP-relative (32 bit type index)
   cvsymtLData32_32	= 0x1007,      // Module-local symbol (32 bit type index)
   cvsymtGData32_32	= 0x1008,      // Global data symbol (32 bit type index)
   cvsymtPub32_32	= 0x1009,      // A public symbol (CV internal reserved) (32 bit type index)
   cvsymtLProc32_32	= 0x100a,      // Local procedure start (32 bit type index)
   cvsymtGProc32_32	= 0x100b,      // Global procedure start (32 bit type index)
   cvsymtVFTable32_32	= 0x100c,      // address of virtual function table (32 bit type index)
   cvsymtRegRel32_32	= 0x100d,      // register relative address (32 bit type index)
   cvsymtLThread32_32	= 0x100e,      // local thread storage (32 bit type index)
   cvsymtGThread32_32	= 0x100f,      // global thread storage (32 bit type index)
   cvsymtLProcMips_32	= 0x1010,      // Local procedure start (32 bit type index)
   cvsymtGProcMips_32	= 0x1011,      // Global procedure start (32 bit type index)
   cvsymtFrameInfo	= 0x1012,      // Information regarding stack frame
   cvsymtCompile2	= 0x1013,      // Extended compile flags and info
   cvsymtManyReg2	= 0x1014,      // Compiler dependent per-procedure information
   cvsymtLProcIa64	= 0x1015,      // Local procedure start (32 bit type index)
   cvsymtGProcIa64	= 0x1016,      // Global procedure start (32 bit type index)
   cvsymtLocalSlot	= 0x1017,      // local IL sym with field for local slot index
   cvsymtParamSlot	= 0x1018,      // local IL sym with field for parameter slot index
   cvsymtAnnotation	= 0x1019,      // Annotation string literals
   cvsymtGManProc	= 0x101a,      // Global proc
   cvsymtLManProc	= 0x101b,      // Local proc
   cvsymtLManData	= 0x1020,      // Static data
   cvsymtGManData	= 0x1021,      // Global data
   cvsymtManFrameRel	= 0x1022,      // Frame relative local var or param
   cvsymtManRegister	= 0x1023,      // Register local var or param
   cvsymtManSlot	= 0x1024,      // Slot local var or param
   cvsymtManManyReg	= 0x1025,      // Multiple register local var or param
   cvsymtManRegRel	= 0x1026,      // Register relative local var or param
   cvsymtManManyReg2	= 0x1027,      // Multiple register local var or param
   cvsymtManTypRef	= 0x1028,      // Index for type referenced by name from metadata
   cvsymtUNamespace	= 0x1029,      // Using namespace

   cvsymtObjName_Sz	= 0x1101,      // path to object file name
   cvsymtThunk32_Sz	= 0x1102,      // Thunk Start
   cvsymtBlock32_Sz	= 0x1103,      // block start
   cvsymtWith32_Sz	= 0x1104,      // with start
   cvsymtLabel32_Sz	= 0x1105,      // code label
   cvsymtRegister_Sz	= 0x1106,      // Register variable
   cvsymtConstant_Sz	= 0x1107,      // constant symbol
   cvsymtUDT_Sz 	= 0x1108,      // User defined type
   cvsymtCOBOLUDT_Sz	= 0x1109,      // special UDT for cobol that does not symbol pack
   cvsymtManyReg_Sz	= 0x110a,      // multiple register variable
   cvsymtBPRel32_Sz	= 0x110b,      // BP-relative
   cvsymtLData32_Sz	= 0x110c,      // Module-local symbol
   cvsymtGData32_Sz	= 0x110d,      // Global data symbol
   cvsymtPub32_Sz	= 0x110e,      // a public symbol (CV internal reserved)
   cvsymtLProc32_Sz	= 0x110f,      // Local procedure start
   cvsymtGProc32_Sz	= 0x1110,      // Global procedure start
   cvsymtRegRel32_Sz	= 0x1111,      // register relative address
   cvsymtLThread32_Sz	= 0x1112,      // local thread storage
   cvsymtGThread32_Sz	= 0x1113,      // global thread storage
   cvsymtLProcMips_Sz	= 0x1114,      // Local procedure start
   cvsymtGProcMips_Sz	= 0x1115,      // Global procedure start
   cvsymtCompile2_Sz	= 0x1116,      // extended compile flags and info
   cvsymtManyReg2_Sz	= 0x1117,      // multiple register variable
   cvsymtLProcIa64_Sz	= 0x1118,      // Local procedure start (IA64)
   cvsymtGProcIa64_Sz	= 0x1119,      // Global procedure start (IA64)
   cvsymtLocalSlot_Sz	= 0x111a,      // local IL sym with field for local slot index
   cvsymtParamSlot_Sz	= 0x111b,      // local IL sym with field for parameter slot index
   cvsymtLManData_Sz	= 0x111c,
   cvsymtGManData_Sz	= 0x111d,
   cvsymtManFrameRel_Sz = 0x111e,
   cvsymtManRegister_Sz = 0x111f,
   cvsymtManSlot_Sz	= 0x1120,
   cvsymtManManyReg_Sz	= 0x1121,
   cvsymtManRegRel_Sz	= 0x1122,
   cvsymtManManyReg2_Sz = 0x1123,
   cvsymtUNamespace_Sz	= 0x1124,      // Using namespace
   cvsymtProcRef_Sz	= 0x1125,      // Reference to a procedure
   cvsymtDataRef_Sz	= 0x1126,      // Reference to data
   cvsymtLProcRef_Sz	= 0x1127,      // Local Reference to a procedure
   cvsymtAnnotationRef	= 0x1128,      // Reference to an S_ANNOTATION symbol
   cvsymtTokenRef	= 0x1129,      // Reference to one of the many MANPROCSYM's
   cvsymtGManProc_Sz	= 0x112a,      // Global proc
   cvsymtLManProc_Sz	= 0x112b,      // Local proc
   cvsymtTrampoline	= 0x112c,      // Trampoline thunks
   cvsymtManConstant	= 0x112d,      // Constants with metadata type info
};


enum CVLANG			       // CodeView Language
{
   cvlangC		= 0x00,
   cvlangCxx		= 0x01,
   cvlangFORTRAN	= 0x02,
   cvlangMasm		= 0x03,
   cvlangPascal 	= 0x04,
   cvlangBasic		= 0x05,
   cvlangCOBOL		= 0x06,
   cvlangLink		= 0x07,
   cvlangCvtres 	= 0x08,
};


enum CVCPU			       // CodeView CPU
{
   cvcpu8080		= 0x00,
   cvcpu8086		= 0x01,
   cvcpu80286		= 0x02,
   cvcpu80386		= 0x03,
   cvcpu80486		= 0x04,
   cvcpuPentium 	= 0x05,
   cvcpuPentiumII	= 0x06,
   cvcpuPentiumIII	= 0x07,
   cvcpuMips		= 0x10,
   cvcpuMips16		= 0x11,
   cvcpuMips32		= 0x12,
   cvcpuMips64		= 0x13,
   cvcpuMipsI		= 0x14,
   cvcpuMipsII		= 0x15,
   cvcpuMipsIII 	= 0x16,
   cvcpuMipsIV		= 0x17,
   cvcpuMipsV		= 0x18,
   cvcpuM68000		= 0x20,
   cvcpuM68010		= 0x21,
   cvcpuM68020		= 0x22,
   cvcpuM68030		= 0x23,
   cvcpuM68040		= 0x24,
   cvcpuAlpha21064	= 0x30,
   cvcpuAlpha21164	= 0x31,
   cvcpuAlpha21164A	= 0x32,
   cvcpuAlpha21264	= 0x33,
   cvcpuAlpha21364	= 0x34,
   cvcpuPpc601		= 0x40,
   cvcpuPpc603		= 0x41,
   cvcpuPpc604		= 0x42,
   cvcpuPpc620		= 0x43,
   cvcpuSh3		= 0x50,
   cvcpuSh3E		= 0x51,
   cvcpuSh3Dsp		= 0x52,
   cvcpuSh4		= 0x53,
   cvcpuArm3		= 0x60,
   cvcpuArm4		= 0x61,
   cvcpuArm4T		= 0x62,
   cvcpuIA64		= 0x80,
   cvcpuCee		= 0x90,
   cvcpuAM33		= 0xA0,
   cvcpuM32R		= 0xB0,
   cvcpuTriCore 	= 0xC0,
   cvcpuX86_64		= 0xD0,
};


//  enum describing compile flag ambiant data model

enum CV_CFL_DATA
{
   CV_CFL_DNEAR 	= 0x00,
   CV_CFL_DFAR		= 0x01,
   CV_CFL_DHUGE 	= 0x02,
};


// enum describing compile flag ambiant code model

enum CV_CFL_CODE_e
{
   CV_CFL_CNEAR 	= 0x00,
   CV_CFL_CFAR		= 0x01,
   CV_CFL_CHUGE 	= 0x02,
};


// enum describing compile flag target floating point package

enum CV_CFL_FPKG_e
{
   CV_CFL_NDP		= 0x00,
   CV_CFL_EMU		= 0x01,
   CV_CFL_ALT		= 0x02,
};


// enum describing function return method


struct CV_PROCFLAGS
{
   BYTE  fFPO		: 1;	    // Frame pointer omitted
   BYTE  fInterrupt	: 1;	    // Interrupt return
   BYTE  fFar		: 1;	    // Far return
   BYTE  fNoReturn	: 1;	    // Function does not return
   BYTE  fNoFallThrough : 1;	    // Label isn't fallen into
   BYTE 		: 3;	    // Unused
};


// enum describing function data return method

enum CV_GENERIC_STYLE_e
{
   CV_GENERIC_VOID   = 0x00,	   // void return type
   CV_GENERIC_REG    = 0x01,	   // return data is in registers
   CV_GENERIC_ICAN   = 0x02,	   // indirect caller allocated near
   CV_GENERIC_ICAF   = 0x03,	   // indirect caller allocated far
   CV_GENERIC_IRAN   = 0x04,	   // indirect returnee allocated near
   CV_GENERIC_IRAF   = 0x05,	   // indirect returnee allocated far
   CV_GENERIC_UNUSED = 0x06	   // first unused
};


struct CV_GENERIC_FLAG
{
   WORD  cstyle  :1;	 // true push varargs right to left
   WORD  rsclean :1;	 // true if returnee stack cleanup
   WORD  unused  :14;	 // unused
};





struct CVSYM
{
   WORD  cb;			       // Record size excluding this word
   WORD  cvsymt;		       // Symbol type
};


struct CVSYMSEARCH : public CVSYM
{
   DWORD startsym;		       // Offset of the procedure
   WORD  lsn;			       // Logical segment/section number
};


struct CVSYMCFLAG : public CVSYM
{
   unsigned char   machine;    // target processor
   struct CVSYM
   {
      unsigned char   language	  :8; // language index
      unsigned char   pcode	  :1; // true if pcode present
      unsigned char   floatprec   :2; // floating precision
      unsigned char   floatpkg	  :2; // float package
      unsigned char   ambdata	  :3; // ambiant data model
      unsigned char   ambcode	  :3; // ambiant code model
      unsigned char   mode32	  :1; // true if compiled 32 bit mode
      unsigned char   pad	  :4; // reserved
   } flags;
   unsigned char       ver[1];	   // Length-prefixed compiler version string
};


struct CVSYMOBJNAME : public CVSYM
{
   DWORD dwSignature;		       // signature
   char  rgchName[1];		       // Length prefixed name
};


struct CVSYMENDARG : public CVSYM
{
};


struct CVSYMRETURN : public CVSYM
{
   CV_GENERIC_FLAG flags;      // flags
   CV_GENERIC_STYLE_e style;   // return style
			       // followed by return method data
};


struct CVSYMENTRYTHIS : public CVSYM
{
   unsigned char   thissym;    // symbol describing this pointer on entry
};


enum THUNK_ORDINAL
{
   THUNK_ORDINAL_NOTYPE,
   THUNK_ORDINAL_ADJUSTOR,
   THUNK_ORDINAL_VCALL,
   THUNK_ORDINAL_PCODE
};


enum CEXM_MODEL_e
{
   CEXM_MDL_table	  = 0x00,      // not executable
   CEXM_MDL_jumptable	  = 0x01,      // Compiler generated jump table
   CEXM_MDL_datapad	  = 0x02,      // Data padding for alignment
   CEXM_MDL_native	  = 0x20,      // native (actually not-pcode)
   CEXM_MDL_cobol	  = 0x21,      // cobol
   CEXM_MDL_codepad	  = 0x22,      // Code padding for alignment
   CEXM_MDL_code	  = 0x23,      // code
   CEXM_MDL_pcode	  = 0x40,      // pcode
   CEXM_MDL_pcode32Mac	  = 0x41,      // pcode 32-bit
   CEXM_MDL_pcode32MacNep = 0x42       // pcode 32-bit Native Entry Point
};


enum CV_COBOL_e
{
   CV_COBOL_dontstop,
   CV_COBOL_pfm,
   CV_COBOL_false,
   CV_COBOL_extcall
};


enum CV_PUBSYMFLAGS_e
{
   cvpsfNone	  = 0x00000000,
   cvpsfCode	  = 0x00000001,
   cvpsfFunction  = 0x00000002,
   cvpsfManaged   = 0x00000004,
};


struct CV_PUBSYMFLAGS
{
   DWORD fCode	     : 1;	       // Set if public symbol refers to a code address
   DWORD fFunction   : 1;	       // Set if public symbol is a function
   DWORD fManaged    : 1;	       // Set if managed code
   DWORD	     : 29;	       // Must be zero
};


struct CVSYMPUB32 : public CVSYM
{
   CV_PUBSYMFLAGS  flags;
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMTHUNK32 : public CVSYM
{
   DWORD    pParent;		       // pointer to the parent
   DWORD    pEnd;		       // pointer to this blocks end
   DWORD    pNext;		       // pointer to next symbol
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   WORD     cb; 		       // length of thunk
   BYTE     ord;		       // ordinal specifying type of thunk
   char     rgchName[1];	       // Length prefixed name
   BYTE     variant[1]; 	       // variant portion of thunk
};


struct CVSYMLABEL32 : public CVSYM
{
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   CV_PROCFLAGS    flags;	       // flags
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMBLOCK32 : public CVSYM
{
   DWORD    pParent;		       // pointer to the parent
   DWORD    pEnd;		       // pointer to this blocks end
   DWORD    cb; 		       // Block length
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMWITH32 : public CVSYM
{
   DWORD    pParent;		       // pointer to the parent
   DWORD    pEnd;		       // pointer to this blocks end
   DWORD    cb; 		       // Block length
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   char     rgchExpr[1];	       // Length prefixed expression string
};


struct CVSYMCEXM32 : public CVSYM
{
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   WORD     model;		       // execution model
   union var32
   {
      struct _PCODE
      {
	 DWORD pcdtable;	       // Offset to pcode function table
	 DWORD pcdspi;		       // Offset to segment pcode information
      } pcode;

      struct _PCODE32MAC
      {
	  DWORD calltableOff;	       // offset to function table
	  unsigned short calltableSeg; // segment of function table
      } pcode32Mac;

      struct _COBOL
      {
	 WORD  subtype; 	       // See CV_COBOL_e
	 WORD  flag;
      } cobol;
   };
};


struct CVSYMREG_32 : public CVSYM
{
   CVTI32   cvti;		       // Type index
   WORD     wReg;		       // Register enumerate
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMCONST_32 : public CVSYM
{
   CVTI32   cvti;		       // Type index
   CVVAL    cvval;		       // Value
// char     rgchName[1];	       // Length prefixed name
};


struct CVSYMUDT_32 : public CVSYM
{
   CVTI32   cvti;		       // Type index
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMMANYREG_32 : public CVSYM
{
   CVTI32   cvti;		       // Type index
   BYTE     creg;		       // Count of number of registers
   BYTE     rgbReg[1];		       // Register enumerates
				       // Registers are most significant first.
// char     rgchName[1];	       // Length prefixed name
};


struct CVSYMBPREL32_32 : public CVSYM
{
   DWORD    ib; 		       // BP-relative offset
   CVTI32   cvti;		       // Type index (32 bit)
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMDATA32_32 : public CVSYM
{
   CVTI32   cvti;		       // Type index (32 bit)
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMPROC32_32 : public CVSYM
{
   DWORD    pParent;		       // pointer to the parent
   DWORD    pEnd;		       // pointer to this blocks end
   DWORD    pNext;		       // pointer to next symbol
   DWORD    cb; 		       // Proc length
   DWORD    DbgStart;		       // Debug start offset
   DWORD    DbgEnd;		       // Debug end offset
   CVTI32   cvti;		       // Type index (32 bit)
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   CV_PROCFLAGS    flags;	       // Proc flags
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMREGREL32_32 : public CVSYM
{
   DWORD    ib; 		       // Offset of symbol
   CVTI32   cvti;		       // Type index (32 bit)
   WORD     wReg;		       // Register enumerate
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMTHREAD32_32 : public CVSYM
{
   CVTI32   cvti;		       // Type index (32 bit)
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMPROCMIPS_32 : public CVSYM
{
   DWORD    pParent;		       // pointer to the parent
   DWORD    pEnd;		       // pointer to this blocks end
   DWORD    pNext;		       // pointer to next symbol
   DWORD    cb; 		       // Proc length
   DWORD    DbgStart;		       // Debug start offset
   DWORD    DbgEnd;		       // Debug end offset
   DWORD    regSave;		       // int register save mask
   DWORD    fpSave;		       // fp register save mask
   DWORD    intib;		       // int register save offset
   DWORD    fpib;		       // fp register save offset
   CVTI32   cvti;		       // Type index (32 bit)
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   BYTE     retReg;		       // Register return value is in
   BYTE     frameReg;		       // Frame pointer register
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMFRAMEINFO : public CVSYM
{
   DWORD    cbFrame;		       // Count of bytes of total frame of procedure
   DWORD    cbPad;		       // Count of bytes of padding in the frame
   DWORD    ibPad;		       // Offset in the frame of start of the padding
   DWORD    cbSaveRegs;
   DWORD    ibHandler;		       // Segment/section relative offset of handler
   WORD     lsnHandler; 	       // Logical segment/section number of handler
   union
   {
      struct
      {
	 DWORD	  fHasAlloca  : 1;     // Function uses _alloca()
	 DWORD	  fHasSetJmp  : 1;     // Function uses setjmp()
	 DWORD	  fHasLongJmp : 1;     // Function uses longjmp()
	 DWORD	  fHasInlAsm  : 1;     // Function uses inline asm
	 DWORD	  fHasEH      : 1;     // Function has EH states
	 DWORD	  fInlSpec    : 1;     // Function has declated as inline
	 DWORD	  fHasSEH     : 1;     // Function has SEH
	 DWORD	  Reserved    : 25;    // Must be zero
      } s;

      DWORD	  dwFlags;
   } u;
};


struct CVSYMCOMPILE2  : public CVSYM
{
   union
   {
      struct
      {
	 DWORD	  iLanguage	  : 8;
	 DWORD	  fEC		  : 1; // Compiled for edit and continue
	 DWORD	  fNoDbgInfo	  : 1; // not compiled with debug info
	 DWORD	  fLTCG 	  : 1; // compiled with LTCG
	 DWORD	  fNoDataAlign	  : 1; // compiled with -Bzalign
	 DWORD	  fManagedPresent : 1; // managed code/data present
	 DWORD	  Reserved : 19;       // Must be zero
      } s;

      DWORD	  dwFlags;
   } u;
   WORD     wMachine;		       // Target processor (same as COFF/PE machine type)
   WORD     verFEMajor; 	       // Front end major version #
   WORD     verFEMinor; 	       // Front end minor version #
   WORD     verFEBuild; 	       // Front end build version #
   WORD     verMajor;		       // Back end major version #
   WORD     verMinor;		       // Back end minor version #
   WORD     verBuild;		       // Back end build version #
   char     rgchVer[1]; 	       // Length-prefixed compiler version string

   // What follows is an optional block of zero terminated strings terminated by two zeros
};


struct CVSYMPROCIA64 : public CVSYM
{
   DWORD    pParent;		       // pointer to the parent
   DWORD    pEnd;		       // pointer to this blocks end
   DWORD    pNext;		       // pointer to next symbol
   DWORD    cb; 		       // Proc length
   DWORD    DbgStart;		       // Debug start offset
   DWORD    DbgEnd;		       // Debug end offset
   CVTI32   cvti;		       // Type index (32 bit)
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   WORD     retReg;		       // Register return value is in
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMREF : public CVSYM
{
   DWORD    sumName;		       // SUC of the name
   DWORD    ibSym;		       // Offset of actual symbol in $$Symbols
   WORD     imod;		       // Module containing the actual symbol
   WORD     usFill;		       // align this record
};


struct CVSYMALIGN : public CVSYM
{
};


struct CVSYMSLOT : public CVSYM
{
   DWORD    iSlot;		       // slot index
   CVTI32   cvti;		       // Type index (32 bit)
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMMANPROC : public CVSYM
{
   DWORD    pParent;		       // pointer to the parent
   DWORD    pEnd;		       // pointer to this blocks end
   DWORD    pNext;		       // pointer to next symbol
   DWORD    cb; 		       // Proc length
   DWORD    DbgStart;		       // Debug start offset
   DWORD    DbgEnd;		       // Debug end offset
   DWORD    token;		       // COM+ metadata token for method
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   CV_PROCFLAGS    flags;	       // Proc flags
   WORD     retReg;		       // Register return value is in
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMMANPROCMIPS : public CVSYM
{
   DWORD    pParent;		       // pointer to the parent
   DWORD    pEnd;		       // pointer to this blocks end
   DWORD    pNext;		       // pointer to next symbol
   DWORD    cb; 		       // Proc length
   DWORD    DbgStart;		       // Debug start offset
   DWORD    DbgEnd;		       // Debug end offset
   DWORD    regSave;		       // int register save mask
   DWORD    fpSave;		       // fp register save mask
   DWORD    intib;		       // int register save offset
   DWORD    fpib;		       // fp register save offset
   DWORD    token;		       // COM+ metadata token for method
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   BYTE     retReg;		       // Register return value is in
   BYTE     frameReg;		       // Frame pointer register
   char     rgchName[1];	       // Length prefixed name
};


struct CVSYMANNOTATION : public CVSYM
{
   DWORD    ib; 		       // Segment/section relative offset
   WORD     lsn;		       // Logical segment/section number
   WORD     csz;		       // Count of zero terminated annotation strings
   char     rgsz[1];		       // Sequence of zero terminated annotation strings
};


struct CVSYMUNAMESPACE : public CVSYM
{
   char     rgchName[1];	       // Length prefixed name
};


enum CVTRAMPT			       // Trampoline subtype
{
   cvtramptIncremental, 	       // Incremental thunks
   cvtramptBranchIsland,	       // Branch island thunks
};


struct CVSYMTRAMPOLINE : public CVSYM
{
   WORD     cvtrampt;		       // Trampoline sym subtype
   WORD     cb; 		       // Trampoline length
   DWORD    ib; 		       // Segment/section relative offset
   DWORD    ibTarget;		       // Segment/section relative offset of target
   WORD     lsn;		       // Logical segment/section number
   WORD     lsnTarget;		       // Logical segment/section number of target
};

// UNDONE: For srcline stuff - will turn them into CVSYM later.


   // Source line to address mapping table.
   // This table is generated by the link/ilink utility from line number
   // information contained in the object file OMF data.  This table contains
   // only the code contribution for one segment from one source file.

struct CVSRCLINE
{
   WORD     Seg;		       // linker segment index
   WORD     cLnOff;		       // count of line/offset pairs
   DWORD    offset[1];		       // array of offsets in segment
   WORD     lineNbr[1]; 	       // array of line lumber in source
};


   // Source file description
   // This table is generated by the linker

struct CVSRCFILE
{
   WORD     cSeg;		       // number of segments from source file
   WORD     reserved;		       // reserved
   DWORD    baseSrcLn[1];	       // base of OMFSourceLine tables
				       // this array is followed by array
				       // of segment start/end pairs followed by
				       // an array of linker indices
				       // for each segment in the file
   WORD     cFName;		       // length of source file name
   char     Name;		       // name of file padded to long boundary
};


   // Source line to address mapping header structure
   // This structure describes the number and location of the
   // OMFAddrLine tables for a module.	The offSrcLine entries are
   // relative to the beginning of this structure.

struct CVSRC
{
   WORD     cFile;		    // number of OMFSourceTables
   WORD     cSeg;		    // number of segments in module
   DWORD    baseSrcFile[1];	    // base of OMFSourceFile table
                                    // this array is followed by array
                                    // of segment start/end pairs followed
                                    // by an array of linker indices
                                    // for each segment in the module
};

// UNDONE: End


enum CV_HREG_e
{
   // Register subset shared by all processor types,
   // must not overlap with any of the ranges below and yet must fit
   // in a byte

   CV_ALLREG_ERR     = 250,
   CV_ALLREG_TEB     = 251,

   // Register set for the x86 processor series (plus PCODE registers)

   CV_REG_NONE	     =	 0,
   CV_REG_AL	     =	 1,
   CV_REG_CL	     =	 2,
   CV_REG_DL	     =	 3,
   CV_REG_BL	     =	 4,
   CV_REG_AH	     =	 5,
   CV_REG_CH	     =	 6,
   CV_REG_DH	     =	 7,
   CV_REG_BH	     =	 8,
   CV_REG_AX	     =	 9,
   CV_REG_CX	     =	10,
   CV_REG_DX	     =	11,
   CV_REG_BX	     =	12,
   CV_REG_SP	     =	13,
   CV_REG_BP	     =	14,
   CV_REG_SI	     =	15,
   CV_REG_DI	     =	16,
   CV_REG_EAX	     =	17,
   CV_REG_ECX	     =	18,
   CV_REG_EDX	     =	19,
   CV_REG_EBX	     =	20,
   CV_REG_ESP	     =	21,
   CV_REG_EBP	     =	22,
   CV_REG_ESI	     =	23,
   CV_REG_EDI	     =	24,
   CV_REG_ES	     =	25,
   CV_REG_CS	     =	26,
   CV_REG_SS	     =	27,
   CV_REG_DS	     =	28,
   CV_REG_FS	     =	29,
   CV_REG_GS	     =	30,
   CV_REG_IP	     =	31,
   CV_REG_FLAGS      =	32,
   CV_REG_EIP	     =	33,
   CV_REG_EFLAGS     =	34,
   CV_REG_TEMP	     =	40,	       // PCODE Temp
   CV_REG_TEMPH      =	41,	       // PCODE TempH
   CV_REG_QUOTE      =	42,	       // PCODE Quote
   CV_REG_PCDR3      =	43,	       // PCODE reserved
   CV_REG_PCDR4      =	44,	       // PCODE reserved
   CV_REG_PCDR5      =	45,	       // PCODE reserved
   CV_REG_PCDR6      =	46,	       // PCODE reserved
   CV_REG_PCDR7      =	47,	       // PCODE reserved
   CV_REG_CR0	     =	80,	       // CR0 -- control registers
   CV_REG_CR1	     =	81,
   CV_REG_CR2	     =	82,
   CV_REG_CR3	     =	83,
   CV_REG_DR0	     =	90,	       // Debug register
   CV_REG_DR1	     =	91,
   CV_REG_DR2	     =	92,
   CV_REG_DR3	     =	93,
   CV_REG_DR4	     =	94,
   CV_REG_DR5	     =	95,
   CV_REG_DR6	     =	96,
   CV_REG_DR7	     =	97,
   CV_REG_ST0	     = 128,
   CV_REG_ST1	     = 129,
   CV_REG_ST2	     = 130,
   CV_REG_ST3	     = 131,
   CV_REG_ST4	     = 132,
   CV_REG_ST5	     = 133,
   CV_REG_ST6	     = 134,
   CV_REG_ST7	     = 135,
   CV_REG_CTRL	     = 136,
   CV_REG_STAT	     = 137,
   CV_REG_TAG	     = 138,
   CV_REG_FPIP	     = 139,
   CV_REG_FPCS	     = 140,
   CV_REG_FPDO	     = 141,
   CV_REG_FPDS	     = 142,
   CV_REG_ISEM	     = 143,
   CV_REG_FPEIP      = 144,
   CV_REG_FPEDO      = 145,
   CV_REG_MM0	     = 146,
   CV_REG_MM1	     = 147,
   CV_REG_MM2	     = 148,
   CV_REG_MM3	     = 149,
   CV_REG_MM4	     = 150,
   CV_REG_MM5	     = 151,
   CV_REG_MM6	     = 152,
   CV_REG_MM7	     = 153,

   // Registers for the 68K processors

   CV_R68_D0	     =	0,
   CV_R68_D1	     =	1,
   CV_R68_D2	     =	2,
   CV_R68_D3	     =	3,
   CV_R68_D4	     =	4,
   CV_R68_D5	     =	5,
   CV_R68_D6	     =	6,
   CV_R68_D7	     =	7,
   CV_R68_A0	     =	8,
   CV_R68_A1	     =	9,
   CV_R68_A2	     = 10,
   CV_R68_A3	     = 11,
   CV_R68_A4	     = 12,
   CV_R68_A5	     = 13,
   CV_R68_A6	     = 14,
   CV_R68_A7	     = 15,
   CV_R68_CCR	     = 16,
   CV_R68_SR	     = 17,
   CV_R68_USP	     = 18,
   CV_R68_MSP	     = 19,
   CV_R68_SFC	     = 20,
   CV_R68_DFC	     = 21,
   CV_R68_CACR	     = 22,
   CV_R68_VBR	     = 23,
   CV_R68_CAAR	     = 24,
   CV_R68_ISP	     = 25,
   CV_R68_PC	     = 26,

   // Reserved 27

   CV_R68_FPCR	     = 28,
   CV_R68_FPSR	     = 29,
   CV_R68_FPIAR      = 30,

   // Reserved 31

   CV_R68_FP0	     = 32,
   CV_R68_FP1	     = 33,
   CV_R68_FP2	     = 34,
   CV_R68_FP3	     = 35,
   CV_R68_FP4	     = 36,
   CV_R68_FP5	     = 37,
   CV_R68_FP6	     = 38,
   CV_R68_FP7	     = 39,

   // Reserved 40-50

   CV_R68_PSR	     = 51,
   CV_R68_PCSR	     = 52,
   CV_R68_VAL	     = 53,
   CV_R68_CRP	     = 54,
   CV_R68_SRP	     = 55,
   CV_R68_DRP	     = 56,
   CV_R68_TC	     = 57,
   CV_R68_AC	     = 58,
   CV_R68_SCC	     = 59,
   CV_R68_CAL	     = 60,
   CV_R68_TT0	     = 61,
   CV_R68_TT1	     = 62,

   // Reserved 63

   CV_R68_BAD0	     = 64,
   CV_R68_BAD1	     = 65,
   CV_R68_BAD2	     = 66,
   CV_R68_BAD3	     = 67,
   CV_R68_BAD4	     = 68,
   CV_R68_BAD5	     = 69,
   CV_R68_BAD6	     = 70,
   CV_R68_BAD7	     = 71,
   CV_R68_BAC0	     = 72,
   CV_R68_BAC1	     = 73,
   CV_R68_BAC2	     = 74,
   CV_R68_BAC3	     = 75,
   CV_R68_BAC4	     = 76,
   CV_R68_BAC5	     = 77,
   CV_R68_BAC6	     = 78,
   CV_R68_BAC7	     = 79,

   // Register set for the MIPS R4000

   CV_M4_NOREG	     = CV_REG_NONE,

   CV_M4_IntZERO     = 10,	       // General purpose registers
   CV_M4_IntAT	     = 11,
   CV_M4_IntV0	     = 12,
   CV_M4_IntV1	     = 13,
   CV_M4_IntA0	     = 14,
   CV_M4_IntA1	     = 15,
   CV_M4_IntA2	     = 16,
   CV_M4_IntA3	     = 17,
   CV_M4_IntT0	     = 18,
   CV_M4_IntT1	     = 19,
   CV_M4_IntT2	     = 20,
   CV_M4_IntT3	     = 21,
   CV_M4_IntT4	     = 22,
   CV_M4_IntT5	     = 23,
   CV_M4_IntT6	     = 24,
   CV_M4_IntT7	     = 25,
   CV_M4_IntS0	     = 26,
   CV_M4_IntS1	     = 27,
   CV_M4_IntS2	     = 28,
   CV_M4_IntS3	     = 29,
   CV_M4_IntS4	     = 30,
   CV_M4_IntS5	     = 31,
   CV_M4_IntS6	     = 32,
   CV_M4_IntS7	     = 33,
   CV_M4_IntT8	     = 34,
   CV_M4_IntT9	     = 35,
   CV_M4_IntKT0      = 36,
   CV_M4_IntKT1      = 37,
   CV_M4_IntGP	     = 38,
   CV_M4_IntSP	     = 39,
   CV_M4_IntS8	     = 40,
   CV_M4_IntRA	     = 41,
   CV_M4_IntLO	     = 42,
   CV_M4_IntHI	     = 43,

   CV_M4_Fir	     = 50,
   CV_M4_Psr	     = 51,

   CV_M4_FltF0	     = 60,	       // Floating point registers
   CV_M4_FltF1	     = 61,
   CV_M4_FltF2	     = 62,
   CV_M4_FltF3	     = 63,
   CV_M4_FltF4	     = 64,
   CV_M4_FltF5	     = 65,
   CV_M4_FltF6	     = 66,
   CV_M4_FltF7	     = 67,
   CV_M4_FltF8	     = 68,
   CV_M4_FltF9	     = 69,
   CV_M4_FltF10      = 70,
   CV_M4_FltF11      = 71,
   CV_M4_FltF12      = 72,
   CV_M4_FltF13      = 73,
   CV_M4_FltF14      = 74,
   CV_M4_FltF15      = 75,
   CV_M4_FltF16      = 76,
   CV_M4_FltF17      = 77,
   CV_M4_FltF18      = 78,
   CV_M4_FltF19      = 79,
   CV_M4_FltF20      = 80,
   CV_M4_FltF21      = 81,
   CV_M4_FltF22      = 82,
   CV_M4_FltF23      = 83,
   CV_M4_FltF24      = 84,
   CV_M4_FltF25      = 85,
   CV_M4_FltF26      = 86,
   CV_M4_FltF27      = 87,
   CV_M4_FltF28      = 88,
   CV_M4_FltF29      = 89,
   CV_M4_FltF30      = 90,
   CV_M4_FltF31      = 91,
   CV_M4_FltFsr      = 92,

   // Register set for the ALPHA AXP

   CV_ALPHA_NOREG    = CV_REG_NONE,

   CV_ALPHA_FltF0 = 10, 	       // Floating point registers
   CV_ALPHA_FltF1,     // 11
   CV_ALPHA_FltF2,     // 12
   CV_ALPHA_FltF3,     // 13
   CV_ALPHA_FltF4,     // 14
   CV_ALPHA_FltF5,     // 15
   CV_ALPHA_FltF6,     // 16
   CV_ALPHA_FltF7,     // 17
   CV_ALPHA_FltF8,     // 18
   CV_ALPHA_FltF9,     // 19
   CV_ALPHA_FltF10,    // 20
   CV_ALPHA_FltF11,    // 21
   CV_ALPHA_FltF12,    // 22
   CV_ALPHA_FltF13,    // 23
   CV_ALPHA_FltF14,    // 24
   CV_ALPHA_FltF15,    // 25
   CV_ALPHA_FltF16,    // 26
   CV_ALPHA_FltF17,    // 27
   CV_ALPHA_FltF18,    // 28
   CV_ALPHA_FltF19,    // 29
   CV_ALPHA_FltF20,    // 30
   CV_ALPHA_FltF21,    // 31
   CV_ALPHA_FltF22,    // 32
   CV_ALPHA_FltF23,    // 33
   CV_ALPHA_FltF24,    // 34
   CV_ALPHA_FltF25,    // 35
   CV_ALPHA_FltF26,    // 36
   CV_ALPHA_FltF27,    // 37
   CV_ALPHA_FltF28,    // 38
   CV_ALPHA_FltF29,    // 39
   CV_ALPHA_FltF30,    // 30
   CV_ALPHA_FltF31,    // 41

   CV_ALPHA_IntV0,     // 42   Integer registers
   CV_ALPHA_IntT0,     // 43
   CV_ALPHA_IntT1,     // 44
   CV_ALPHA_IntT2,     // 45
   CV_ALPHA_IntT3,     // 46
   CV_ALPHA_IntT4,     // 47
   CV_ALPHA_IntT5,     // 48
   CV_ALPHA_IntT6,     // 49
   CV_ALPHA_IntT7,     // 50
   CV_ALPHA_IntS0,     // 51
   CV_ALPHA_IntS1,     // 52
   CV_ALPHA_IntS2,     // 53
   CV_ALPHA_IntS3,     // 54
   CV_ALPHA_IntS4,     // 55
   CV_ALPHA_IntS5,     // 56
   CV_ALPHA_IntFP,     // 57
   CV_ALPHA_IntA0,     // 58
   CV_ALPHA_IntA1,     // 59
   CV_ALPHA_IntA2,     // 60
   CV_ALPHA_IntA3,     // 61
   CV_ALPHA_IntA4,     // 62
   CV_ALPHA_IntA5,     // 63
   CV_ALPHA_IntT8,     // 64
   CV_ALPHA_IntT9,     // 65
   CV_ALPHA_IntT10,    // 66
   CV_ALPHA_IntT11,    // 67
   CV_ALPHA_IntRA,     // 68
   CV_ALPHA_IntT12,    // 69
   CV_ALPHA_IntAT,     // 70
   CV_ALPHA_IntGP,     // 71
   CV_ALPHA_IntSP,     // 72
   CV_ALPHA_IntZERO,   // 73

   CV_ALPHA_Fpcr,      // 74   Control registers
   CV_ALPHA_Fir,       // 75
   CV_ALPHA_Psr,       // 76
   CV_ALPHA_FltFsr,    // 77
   CV_ALPHA_SoftFpcr,  // 78

   // Register set for the PowerPC

   CV_PPC_GPR0	     =	1,	       // General Registers ( User Level )
   CV_PPC_GPR1	     =	2,
   CV_PPC_GPR2	     =	3,
   CV_PPC_GPR3	     =	4,
   CV_PPC_GPR4	     =	5,
   CV_PPC_GPR5	     =	6,
   CV_PPC_GPR6	     =	7,
   CV_PPC_GPR7	     =	8,
   CV_PPC_GPR8	     =	9,
   CV_PPC_GPR9	     = 10,
   CV_PPC_GPR10      = 11,
   CV_PPC_GPR11      = 12,
   CV_PPC_GPR12      = 13,
   CV_PPC_GPR13      = 14,
   CV_PPC_GPR14      = 15,
   CV_PPC_GPR15      = 16,
   CV_PPC_GPR16      = 17,
   CV_PPC_GPR17      = 18,
   CV_PPC_GPR18      = 19,
   CV_PPC_GPR19      = 20,
   CV_PPC_GPR20      = 21,
   CV_PPC_GPR21      = 22,
   CV_PPC_GPR22      = 23,
   CV_PPC_GPR23      = 24,
   CV_PPC_GPR24      = 25,
   CV_PPC_GPR25      = 26,
   CV_PPC_GPR26      = 27,
   CV_PPC_GPR27      = 28,
   CV_PPC_GPR28      = 29,
   CV_PPC_GPR29      = 30,
   CV_PPC_GPR30      = 31,
   CV_PPC_GPR31      = 32,

   CV_PPC_CR	     = 33,	       // Condition Register ( User Level )
   CV_PPC_CR0	     = 34,
   CV_PPC_CR1	     = 35,
   CV_PPC_CR2	     = 36,
   CV_PPC_CR3	     = 37,
   CV_PPC_CR4	     = 38,
   CV_PPC_CR5	     = 39,
   CV_PPC_CR6	     = 40,
   CV_PPC_CR7	     = 41,

   CV_PPC_FPR0	     = 42,	       // Floating Point Registers ( User Level )
   CV_PPC_FPR1	     = 43,
   CV_PPC_FPR2	     = 44,
   CV_PPC_FPR3	     = 45,
   CV_PPC_FPR4	     = 46,
   CV_PPC_FPR5	     = 47,
   CV_PPC_FPR6	     = 48,
   CV_PPC_FPR7	     = 49,
   CV_PPC_FPR8	     = 50,
   CV_PPC_FPR9	     = 51,
   CV_PPC_FPR10      = 52,
   CV_PPC_FPR11      = 53,
   CV_PPC_FPR12      = 54,
   CV_PPC_FPR13      = 55,
   CV_PPC_FPR14      = 56,
   CV_PPC_FPR15      = 57,
   CV_PPC_FPR16      = 58,
   CV_PPC_FPR17      = 59,
   CV_PPC_FPR18      = 60,
   CV_PPC_FPR19      = 61,
   CV_PPC_FPR20      = 62,
   CV_PPC_FPR21      = 63,
   CV_PPC_FPR22      = 64,
   CV_PPC_FPR23      = 65,
   CV_PPC_FPR24      = 66,
   CV_PPC_FPR25      = 67,
   CV_PPC_FPR26      = 68,
   CV_PPC_FPR27      = 69,
   CV_PPC_FPR28      = 70,
   CV_PPC_FPR29      = 71,
   CV_PPC_FPR30      = 72,
   CV_PPC_FPR31      = 73,

   CV_PPC_FPSCR      = 74,	       // Floating Point Status and Control Register ( User Level )

   CV_PPC_MSR	     = 75,	       // Machine State Register ( Supervisor Level )

   CV_PPC_SR0	     = 76,	       // Segment Registers ( Supervisor Level )
   CV_PPC_SR1	     = 77,
   CV_PPC_SR2	     = 78,
   CV_PPC_SR3	     = 79,
   CV_PPC_SR4	     = 80,
   CV_PPC_SR5	     = 81,
   CV_PPC_SR6	     = 82,
   CV_PPC_SR7	     = 83,
   CV_PPC_SR8	     = 84,
   CV_PPC_SR9	     = 85,
   CV_PPC_SR10	     = 86,
   CV_PPC_SR11	     = 87,
   CV_PPC_SR12	     = 88,
   CV_PPC_SR13	     = 89,
   CV_PPC_SR14	     = 90,
   CV_PPC_SR15	     = 91,

   /*
   ** For all of the special purpose registers add 100 to the SPR# that the
   ** Motorola/IBM documentation gives with the exception of any imaginary
   ** registers.
   */

   CV_PPC_PC	     = 99,	       // PC (imaginary register)

   CV_PPC_MQ	     = 100,	       // MPC601
   CV_PPC_XER	     = 101,
   CV_PPC_RTCU	     = 104,	       // MPC601
   CV_PPC_RTCL	     = 105,	       // MPC601
   CV_PPC_LR	     = 108,
   CV_PPC_CTR	     = 109,

   CV_PPC_COMPARE    = 110,	       // part of XER (internal to the debugger only)
   CV_PPC_COUNT      = 111,	       // part of XER (internal to the debugger only)

   CV_PPC_DSISR      = 118,	       // Special Purpose Registers ( Supervisor Level )
   CV_PPC_DAR	     = 119,
   CV_PPC_DEC	     = 122,
   CV_PPC_SDR1	     = 125,
   CV_PPC_SRR0	     = 126,
   CV_PPC_SRR1	     = 127,
   CV_PPC_SPRG0      = 372,
   CV_PPC_SPRG1      = 373,
   CV_PPC_SPRG2      = 374,
   CV_PPC_SPRG3      = 375,
   CV_PPC_ASR	     = 280,	       // 64-bit implementations only
   CV_PPC_EAR	     = 382,
   CV_PPC_PVR	     = 287,
   CV_PPC_BAT0U      = 628,
   CV_PPC_BAT0L      = 629,
   CV_PPC_BAT1U      = 630,
   CV_PPC_BAT1L      = 631,
   CV_PPC_BAT2U      = 632,
   CV_PPC_BAT2L      = 633,
   CV_PPC_BAT3U      = 634,
   CV_PPC_BAT3L      = 635,
   CV_PPC_DBAT0U     = 636,
   CV_PPC_DBAT0L     = 637,
   CV_PPC_DBAT1U     = 638,
   CV_PPC_DBAT1L     = 639,
   CV_PPC_DBAT2U     = 640,
   CV_PPC_DBAT2L     = 641,
   CV_PPC_DBAT3U     = 642,
   CV_PPC_DBAT3L     = 643,

   CV_PPC_PMR0	     = 1044,	       // MPC620,
   CV_PPC_PMR1	     = 1045,	       // MPC620,
   CV_PPC_PMR2	     = 1046,	       // MPC620,
   CV_PPC_PMR3	     = 1047,	       // MPC620,
   CV_PPC_PMR4	     = 1048,	       // MPC620,
   CV_PPC_PMR5	     = 1049,	       // MPC620,
   CV_PPC_PMR6	     = 1050,	       // MPC620,
   CV_PPC_PMR7	     = 1051,	       // MPC620,
   CV_PPC_PMR8	     = 1052,	       // MPC620,
   CV_PPC_PMR9	     = 1053,	       // MPC620,
   CV_PPC_PMR10      = 1054,	       // MPC620,
   CV_PPC_PMR11      = 1055,	       // MPC620,
   CV_PPC_PMR12      = 1056,	       // MPC620,
   CV_PPC_PMR13      = 1057,	       // MPC620,
   CV_PPC_PMR14      = 1058,	       // MPC620,
   CV_PPC_PMR15      = 1059,	       // MPC620,

   CV_PPC_DMISS      = 1076,	       // MPC603
   CV_PPC_DCMP	     = 1077,	       // MPC603
   CV_PPC_HASH1      = 1078,	       // MPC603
   CV_PPC_HASH2      = 1079,	       // MPC603
   CV_PPC_IMISS      = 1080,	       // MPC603
   CV_PPC_ICMP	     = 1081,	       // MPC603
   CV_PPC_RPA	     = 1082,	       // MPC603

   CV_PPC_HID0	     = 1108,	       // MPC601, MPC603, MPC620
   CV_PPC_HID1	     = 1109,	       // MPC601
   CV_PPC_HID2	     = 1110,	       // MPC601, MPC603, MPC620 ( IABR )
   CV_PPC_HID3	     = 1111,	       // Not Defined
   CV_PPC_HID4	     = 1112,	       // Not Defined
   CV_PPC_HID5	     = 1113,	       // MPC601, MPC604, MPC620 ( DABR )
   CV_PPC_HID6	     = 1114,	       // Not Defined
   CV_PPC_HID7	     = 1115,	       // Not Defined
   CV_PPC_HID8	     = 1116,	       // MPC620 ( BUSCSR )
   CV_PPC_HID9	     = 1117,	       // MPC620 ( L2CSR )
   CV_PPC_HID10      = 1118,	       // Not Defined
   CV_PPC_HID11      = 1119,	       // Not Defined
   CV_PPC_HID12      = 1120,	       // Not Defined
   CV_PPC_HID13      = 1121,	       // MPC604 ( HCR )
   CV_PPC_HID14      = 1122,	       // Not Defined
   CV_PPC_HID15      = 1123,	       // MPC601, MPC604, MPC620 ( PIR )

   // JAVA VM registers

   CV_JAVA_PC	     =	1,

   // Register set for the Hitachi SH3/SH4

   CV_SH3_NOREG      = CV_REG_NONE,

   CV_SH3_IntR0      = 10,	       // CPU REGISTER
   CV_SH3_IntR1      = 11,
   CV_SH3_IntR2      = 12,
   CV_SH3_IntR3      = 13,
   CV_SH3_IntR4      = 14,
   CV_SH3_IntR5      = 15,
   CV_SH3_IntR6      = 16,
   CV_SH3_IntR7      = 17,
   CV_SH3_IntR8      = 18,
   CV_SH3_IntR9      = 19,
   CV_SH3_IntR10     = 20,
   CV_SH3_IntR11     = 21,
   CV_SH3_IntR12     = 22,
   CV_SH3_IntR13     = 23,
   CV_SH3_IntFp      = 24,
   CV_SH3_IntSp      = 25,
   CV_SH3_Gbr	     = 38,
   CV_SH3_Pr	     = 39,
   CV_SH3_Mach	     = 40,
   CV_SH3_Macl	     = 41,

   CV_SH3_Pc	     = 50,
   CV_SH3_Sr	     = 51,

   CV_SH3_BarA	     = 60,
   CV_SH3_BasrA      = 61,
   CV_SH3_BamrA      = 62,
   CV_SH3_BbrA	     = 63,
   CV_SH3_BarB	     = 64,
   CV_SH3_BasrB      = 65,
   CV_SH3_BamrB      = 66,
   CV_SH3_BbrB	     = 67,
   CV_SH3_BdrB	     = 68,
   CV_SH3_BdmrB      = 69,
   CV_SH3_Brcr	     = 70,

   // Register set for the ARM processor.

   CV_ARM_NOREG      = CV_REG_NONE,

   CV_ARM_R0	     = 10,
   CV_ARM_R1	     = 11,
   CV_ARM_R2	     = 12,
   CV_ARM_R3	     = 13,
   CV_ARM_R4	     = 14,
   CV_ARM_R5	     = 15,
   CV_ARM_R6	     = 16,
   CV_ARM_R7	     = 17,
   CV_ARM_R8	     = 18,
   CV_ARM_R9	     = 19,
   CV_ARM_R10	     = 20,
   CV_ARM_R11	     = 21,	       // Frame pointer, if allocated
   CV_ARM_R12	     = 22,
   CV_ARM_SP	     = 23,	       // Stack pointer
   CV_ARM_LR	     = 24,	       // Link Register
   CV_ARM_PC	     = 25,	       // Program counter
   CV_ARM_CPSR	     = 26,	       // Current program status register

   // Register set for Intel IA64

   // Branch Registers

   CV_IA64_Br0	     = 512,
   CV_IA64_Br1	     = 513,
   CV_IA64_Br2	     = 514,
   CV_IA64_Br3	     = 515,
   CV_IA64_Br4	     = 516,
   CV_IA64_Br5	     = 517,
   CV_IA64_Br6	     = 518,
   CV_IA64_Br7	     = 519,

    // Predicate Registers

   CV_IA64_P0	     = 704,
   CV_IA64_P1	     = 705,
   CV_IA64_P2	     = 706,
   CV_IA64_P3	     = 707,
   CV_IA64_P4	     = 708,
   CV_IA64_P5	     = 709,
   CV_IA64_P6	     = 710,
   CV_IA64_P7	     = 711,
   CV_IA64_P8	     = 712,
   CV_IA64_P9	     = 713,
   CV_IA64_P10	     = 714,
   CV_IA64_P11	     = 715,
   CV_IA64_P12	     = 716,
   CV_IA64_P13	     = 717,
   CV_IA64_P14	     = 718,
   CV_IA64_P15	     = 719,
   CV_IA64_P16	     = 720,
   CV_IA64_P17	     = 721,
   CV_IA64_P18	     = 722,
   CV_IA64_P19	     = 723,
   CV_IA64_P20	     = 724,
   CV_IA64_P21	     = 725,
   CV_IA64_P22	     = 726,
   CV_IA64_P23	     = 727,
   CV_IA64_P24	     = 728,
   CV_IA64_P25	     = 729,
   CV_IA64_P26	     = 730,
   CV_IA64_P27	     = 731,
   CV_IA64_P28	     = 732,
   CV_IA64_P29	     = 733,
   CV_IA64_P30	     = 734,
   CV_IA64_P31	     = 735,
   CV_IA64_P32	     = 736,
   CV_IA64_P33	     = 737,
   CV_IA64_P34	     = 738,
   CV_IA64_P35	     = 739,
   CV_IA64_P36	     = 740,
   CV_IA64_P37	     = 741,
   CV_IA64_P38	     = 742,
   CV_IA64_P39	     = 743,
   CV_IA64_P40	     = 744,
   CV_IA64_P41	     = 745,
   CV_IA64_P42	     = 746,
   CV_IA64_P43	     = 747,
   CV_IA64_P44	     = 748,
   CV_IA64_P45	     = 749,
   CV_IA64_P46	     = 750,
   CV_IA64_P47	     = 751,
   CV_IA64_P48	     = 752,
   CV_IA64_P49	     = 753,
   CV_IA64_P50	     = 754,
   CV_IA64_P51	     = 755,
   CV_IA64_P52	     = 756,
   CV_IA64_P53	     = 757,
   CV_IA64_P54	     = 758,
   CV_IA64_P55	     = 759,
   CV_IA64_P56	     = 760,
   CV_IA64_P57	     = 761,
   CV_IA64_P58	     = 762,
   CV_IA64_P59	     = 763,
   CV_IA64_P60	     = 764,
   CV_IA64_P61	     = 765,
   CV_IA64_P62	     = 766,
   CV_IA64_P63	     = 767,

   // Predicate Registers

   CV_IA64_Preds     = 768,

   // Banked General Registers

   CV_IA64_IntH0     = 832,
   CV_IA64_IntH1     = 833,
   CV_IA64_IntH2     = 834,
   CV_IA64_IntH3     = 835,
   CV_IA64_IntH4     = 836,
   CV_IA64_IntH5     = 837,
   CV_IA64_IntH6     = 838,
   CV_IA64_IntH7     = 839,
   CV_IA64_IntH8     = 840,
   CV_IA64_IntH9     = 841,
   CV_IA64_IntH10    = 842,
   CV_IA64_IntH11    = 843,
   CV_IA64_IntH12    = 844,
   CV_IA64_IntH13    = 845,
   CV_IA64_IntH14    = 846,
   CV_IA64_IntH15    = 847,

   // Special Registers

   CV_IA64_Ip	     = 1016,
   CV_IA64_Umask     = 1017,
   CV_IA64_Cfm	     = 1018,
   CV_IA64_Psr	     = 1019,

   // Banked General Registers

   CV_IA64_Nats      = 1020,
   CV_IA64_Nats2     = 1021,
   CV_IA64_Nats3     = 1022,

   // General-Purpose Registers

   // Integer Registers

   CV_IA64_IntR0     = 1024,
   CV_IA64_IntR1     = 1025,
   CV_IA64_IntR2     = 1026,
   CV_IA64_IntR3     = 1027,
   CV_IA64_IntR4     = 1028,
   CV_IA64_IntR5     = 1029,
   CV_IA64_IntR6     = 1030,
   CV_IA64_IntR7     = 1031,
   CV_IA64_IntR8     = 1032,
   CV_IA64_IntR9     = 1033,
   CV_IA64_IntR10    = 1034,
   CV_IA64_IntR11    = 1035,
   CV_IA64_IntR12    = 1036,
   CV_IA64_IntR13    = 1037,
   CV_IA64_IntR14    = 1038,
   CV_IA64_IntR15    = 1039,
   CV_IA64_IntR16    = 1040,
   CV_IA64_IntR17    = 1041,
   CV_IA64_IntR18    = 1042,
   CV_IA64_IntR19    = 1043,
   CV_IA64_IntR20    = 1044,
   CV_IA64_IntR21    = 1045,
   CV_IA64_IntR22    = 1046,
   CV_IA64_IntR23    = 1047,
   CV_IA64_IntR24    = 1048,
   CV_IA64_IntR25    = 1049,
   CV_IA64_IntR26    = 1050,
   CV_IA64_IntR27    = 1051,
   CV_IA64_IntR28    = 1052,
   CV_IA64_IntR29    = 1053,
   CV_IA64_IntR30    = 1054,
   CV_IA64_IntR31    = 1055,

   // Register Stack

   CV_IA64_IntR32    = 1056,
   CV_IA64_IntR33    = 1057,
   CV_IA64_IntR34    = 1058,
   CV_IA64_IntR35    = 1059,
   CV_IA64_IntR36    = 1060,
   CV_IA64_IntR37    = 1061,
   CV_IA64_IntR38    = 1062,
   CV_IA64_IntR39    = 1063,
   CV_IA64_IntR40    = 1064,
   CV_IA64_IntR41    = 1065,
   CV_IA64_IntR42    = 1066,
   CV_IA64_IntR43    = 1067,
   CV_IA64_IntR44    = 1068,
   CV_IA64_IntR45    = 1069,
   CV_IA64_IntR46    = 1070,
   CV_IA64_IntR47    = 1071,
   CV_IA64_IntR48    = 1072,
   CV_IA64_IntR49    = 1073,
   CV_IA64_IntR50    = 1074,
   CV_IA64_IntR51    = 1075,
   CV_IA64_IntR52    = 1076,
   CV_IA64_IntR53    = 1077,
   CV_IA64_IntR54    = 1078,
   CV_IA64_IntR55    = 1079,
   CV_IA64_IntR56    = 1080,
   CV_IA64_IntR57    = 1081,
   CV_IA64_IntR58    = 1082,
   CV_IA64_IntR59    = 1083,
   CV_IA64_IntR60    = 1084,
   CV_IA64_IntR61    = 1085,
   CV_IA64_IntR62    = 1086,
   CV_IA64_IntR63    = 1087,
   CV_IA64_IntR64    = 1088,
   CV_IA64_IntR65    = 1089,
   CV_IA64_IntR66    = 1090,
   CV_IA64_IntR67    = 1091,
   CV_IA64_IntR68    = 1092,
   CV_IA64_IntR69    = 1093,
   CV_IA64_IntR70    = 1094,
   CV_IA64_IntR71    = 1095,
   CV_IA64_IntR72    = 1096,
   CV_IA64_IntR73    = 1097,
   CV_IA64_IntR74    = 1098,
   CV_IA64_IntR75    = 1099,
   CV_IA64_IntR76    = 1100,
   CV_IA64_IntR77    = 1101,
   CV_IA64_IntR78    = 1102,
   CV_IA64_IntR79    = 1103,
   CV_IA64_IntR80    = 1104,
   CV_IA64_IntR81    = 1105,
   CV_IA64_IntR82    = 1106,
   CV_IA64_IntR83    = 1107,
   CV_IA64_IntR84    = 1108,
   CV_IA64_IntR85    = 1109,
   CV_IA64_IntR86    = 1110,
   CV_IA64_IntR87    = 1111,
   CV_IA64_IntR88    = 1112,
   CV_IA64_IntR89    = 1113,
   CV_IA64_IntR90    = 1114,
   CV_IA64_IntR91    = 1115,
   CV_IA64_IntR92    = 1116,
   CV_IA64_IntR93    = 1117,
   CV_IA64_IntR94    = 1118,
   CV_IA64_IntR95    = 1119,
   CV_IA64_IntR96    = 1120,
   CV_IA64_IntR97    = 1121,
   CV_IA64_IntR98    = 1122,
   CV_IA64_IntR99    = 1123,
   CV_IA64_IntR100   = 1124,
   CV_IA64_IntR101   = 1125,
   CV_IA64_IntR102   = 1126,
   CV_IA64_IntR103   = 1127,
   CV_IA64_IntR104   = 1128,
   CV_IA64_IntR105   = 1129,
   CV_IA64_IntR106   = 1130,
   CV_IA64_IntR107   = 1131,
   CV_IA64_IntR108   = 1132,
   CV_IA64_IntR109   = 1133,
   CV_IA64_IntR110   = 1134,
   CV_IA64_IntR111   = 1135,
   CV_IA64_IntR112   = 1136,
   CV_IA64_IntR113   = 1137,
   CV_IA64_IntR114   = 1138,
   CV_IA64_IntR115   = 1139,
   CV_IA64_IntR116   = 1140,
   CV_IA64_IntR117   = 1141,
   CV_IA64_IntR118   = 1142,
   CV_IA64_IntR119   = 1143,
   CV_IA64_IntR120   = 1144,
   CV_IA64_IntR121   = 1145,
   CV_IA64_IntR122   = 1146,
   CV_IA64_IntR123   = 1147,
   CV_IA64_IntR124   = 1148,
   CV_IA64_IntR125   = 1149,
   CV_IA64_IntR126   = 1150,
   CV_IA64_IntR127   = 1151,

   // Floating-Point Registers

   // Low Floating Point Registers

   CV_IA64_FltF0     = 2048,
   CV_IA64_FltF1     = 2049,
   CV_IA64_FltF2     = 2050,
   CV_IA64_FltF3     = 2051,
   CV_IA64_FltF4     = 2052,
   CV_IA64_FltF5     = 2053,
   CV_IA64_FltF6     = 2054,
   CV_IA64_FltF7     = 2055,
   CV_IA64_FltF8     = 2056,
   CV_IA64_FltF9     = 2057,
   CV_IA64_FltF10    = 2058,
   CV_IA64_FltF11    = 2059,
   CV_IA64_FltF12    = 2060,
   CV_IA64_FltF13    = 2061,
   CV_IA64_FltF14    = 2062,
   CV_IA64_FltF15    = 2063,
   CV_IA64_FltF16    = 2064,
   CV_IA64_FltF17    = 2065,
   CV_IA64_FltF18    = 2066,
   CV_IA64_FltF19    = 2067,
   CV_IA64_FltF20    = 2068,
   CV_IA64_FltF21    = 2069,
   CV_IA64_FltF22    = 2070,
   CV_IA64_FltF23    = 2071,
   CV_IA64_FltF24    = 2072,
   CV_IA64_FltF25    = 2073,
   CV_IA64_FltF26    = 2074,
   CV_IA64_FltF27    = 2075,
   CV_IA64_FltF28    = 2076,
   CV_IA64_FltF29    = 2077,
   CV_IA64_FltF30    = 2078,
   CV_IA64_FltF31    = 2079,

   // High Floating Point Registers

   CV_IA64_FltF32    = 2080,
   CV_IA64_FltF33    = 2081,
   CV_IA64_FltF34    = 2082,
   CV_IA64_FltF35    = 2083,
   CV_IA64_FltF36    = 2084,
   CV_IA64_FltF37    = 2085,
   CV_IA64_FltF38    = 2086,
   CV_IA64_FltF39    = 2087,
   CV_IA64_FltF40    = 2088,
   CV_IA64_FltF41    = 2089,
   CV_IA64_FltF42    = 2090,
   CV_IA64_FltF43    = 2091,
   CV_IA64_FltF44    = 2092,
   CV_IA64_FltF45    = 2093,
   CV_IA64_FltF46    = 2094,
   CV_IA64_FltF47    = 2095,
   CV_IA64_FltF48    = 2096,
   CV_IA64_FltF49    = 2097,
   CV_IA64_FltF50    = 2098,
   CV_IA64_FltF51    = 2099,
   CV_IA64_FltF52    = 2100,
   CV_IA64_FltF53    = 2101,
   CV_IA64_FltF54    = 2102,
   CV_IA64_FltF55    = 2103,
   CV_IA64_FltF56    = 2104,
   CV_IA64_FltF57    = 2105,
   CV_IA64_FltF58    = 2106,
   CV_IA64_FltF59    = 2107,
   CV_IA64_FltF60    = 2108,
   CV_IA64_FltF61    = 2109,
   CV_IA64_FltF62    = 2110,
   CV_IA64_FltF63    = 2111,
   CV_IA64_FltF64    = 2112,
   CV_IA64_FltF65    = 2113,
   CV_IA64_FltF66    = 2114,
   CV_IA64_FltF67    = 2115,
   CV_IA64_FltF68    = 2116,
   CV_IA64_FltF69    = 2117,
   CV_IA64_FltF70    = 2118,
   CV_IA64_FltF71    = 2119,
   CV_IA64_FltF72    = 2120,
   CV_IA64_FltF73    = 2121,
   CV_IA64_FltF74    = 2122,
   CV_IA64_FltF75    = 2123,
   CV_IA64_FltF76    = 2124,
   CV_IA64_FltF77    = 2125,
   CV_IA64_FltF78    = 2126,
   CV_IA64_FltF79    = 2127,
   CV_IA64_FltF80    = 2128,
   CV_IA64_FltF81    = 2129,
   CV_IA64_FltF82    = 2130,
   CV_IA64_FltF83    = 2131,
   CV_IA64_FltF84    = 2132,
   CV_IA64_FltF85    = 2133,
   CV_IA64_FltF86    = 2134,
   CV_IA64_FltF87    = 2135,
   CV_IA64_FltF88    = 2136,
   CV_IA64_FltF89    = 2137,
   CV_IA64_FltF90    = 2138,
   CV_IA64_FltF91    = 2139,
   CV_IA64_FltF92    = 2140,
   CV_IA64_FltF93    = 2141,
   CV_IA64_FltF94    = 2142,
   CV_IA64_FltF95    = 2143,
   CV_IA64_FltF96    = 2144,
   CV_IA64_FltF97    = 2145,
   CV_IA64_FltF98    = 2146,
   CV_IA64_FltF99    = 2147,
   CV_IA64_FltF100   = 2148,
   CV_IA64_FltF101   = 2149,
   CV_IA64_FltF102   = 2150,
   CV_IA64_FltF103   = 2151,
   CV_IA64_FltF104   = 2152,
   CV_IA64_FltF105   = 2153,
   CV_IA64_FltF106   = 2154,
   CV_IA64_FltF107   = 2155,
   CV_IA64_FltF108   = 2156,
   CV_IA64_FltF109   = 2157,
   CV_IA64_FltF110   = 2158,
   CV_IA64_FltF111   = 2159,
   CV_IA64_FltF112   = 2160,
   CV_IA64_FltF113   = 2161,
   CV_IA64_FltF114   = 2162,
   CV_IA64_FltF115   = 2163,
   CV_IA64_FltF116   = 2164,
   CV_IA64_FltF117   = 2165,
   CV_IA64_FltF118   = 2166,
   CV_IA64_FltF119   = 2167,
   CV_IA64_FltF120   = 2168,
   CV_IA64_FltF121   = 2169,
   CV_IA64_FltF122   = 2170,
   CV_IA64_FltF123   = 2171,
   CV_IA64_FltF124   = 2172,
   CV_IA64_FltF125   = 2173,
   CV_IA64_FltF126   = 2174,
   CV_IA64_FltF127   = 2175,

   // Application Registers

   CV_IA64_ApKR0     = 3072,
   CV_IA64_ApKR1     = 3073,
   CV_IA64_ApKR2     = 3074,
   CV_IA64_ApKR3     = 3075,
   CV_IA64_ApKR4     = 3076,
   CV_IA64_ApKR5     = 3077,
   CV_IA64_ApKR6     = 3078,
   CV_IA64_ApKR7     = 3079,
   CV_IA64_AR8	     = 3080,
   CV_IA64_AR9	     = 3081,
   CV_IA64_AR10      = 3082,
   CV_IA64_AR11      = 3083,
   CV_IA64_AR12      = 3084,
   CV_IA64_AR13      = 3085,
   CV_IA64_AR14      = 3086,
   CV_IA64_AR15      = 3087,
   CV_IA64_RsRSC     = 3088,
   CV_IA64_RsBSP     = 3089,
   CV_IA64_RsBSPSTORE = 3090,
   CV_IA64_RsRNAT    = 3091,
   CV_IA64_AR20      = 3092,
   CV_IA64_AR21      = 3093,
   CV_IA64_AR22      = 3094,
   CV_IA64_AR23      = 3095,
   CV_IA64_AR24      = 3096,
   CV_IA64_AR25      = 3097,
   CV_IA64_AR26      = 3098,
   CV_IA64_AR27      = 3099,
   CV_IA64_AR28      = 3100,
   CV_IA64_AR29      = 3101,
   CV_IA64_AR30      = 3102,
   CV_IA64_AR31      = 3103,
   CV_IA64_ApCCV     = 3104,
   CV_IA64_AR33      = 3105,
   CV_IA64_AR34      = 3106,
   CV_IA64_AR35      = 3107,
   CV_IA64_ApUNAT    = 3108,
   CV_IA64_AR37      = 3109,
   CV_IA64_AR38      = 3110,
   CV_IA64_AR39      = 3111,
   CV_IA64_StFPSR    = 3112,
   CV_IA64_AR41      = 3113,
   CV_IA64_AR42      = 3114,
   CV_IA64_AR43      = 3115,
   CV_IA64_ApITC     = 3116,
   CV_IA64_AR45      = 3117,
   CV_IA64_AR46      = 3118,
   CV_IA64_AR47      = 3119,
   CV_IA64_AR48      = 3120,
   CV_IA64_AR49      = 3121,
   CV_IA64_AR50      = 3122,
   CV_IA64_AR51      = 3123,
   CV_IA64_AR52      = 3124,
   CV_IA64_AR53      = 3125,
   CV_IA64_AR54      = 3126,
   CV_IA64_AR55      = 3127,
   CV_IA64_AR56      = 3128,
   CV_IA64_AR57      = 3129,
   CV_IA64_AR58      = 3130,
   CV_IA64_AR59      = 3131,
   CV_IA64_AR60      = 3132,
   CV_IA64_AR61      = 3133,
   CV_IA64_AR62      = 3134,
   CV_IA64_AR63      = 3135,
   CV_IA64_RsPFS     = 3136,
   CV_IA64_ApLC      = 3137,
   CV_IA64_ApEC      = 3138,
   CV_IA64_AR67      = 3139,
   CV_IA64_AR68      = 3140,
   CV_IA64_AR69      = 3141,
   CV_IA64_AR70      = 3142,
   CV_IA64_AR71      = 3143,
   CV_IA64_AR72      = 3144,
   CV_IA64_AR73      = 3145,
   CV_IA64_AR74      = 3146,
   CV_IA64_AR75      = 3147,
   CV_IA64_AR76      = 3148,
   CV_IA64_AR77      = 3149,
   CV_IA64_AR78      = 3150,
   CV_IA64_AR79      = 3151,
   CV_IA64_AR80      = 3152,
   CV_IA64_AR81      = 3153,
   CV_IA64_AR82      = 3154,
   CV_IA64_AR83      = 3155,
   CV_IA64_AR84      = 3156,
   CV_IA64_AR85      = 3157,
   CV_IA64_AR86      = 3158,
   CV_IA64_AR87      = 3159,
   CV_IA64_AR88      = 3160,
   CV_IA64_AR89      = 3161,
   CV_IA64_AR90      = 3162,
   CV_IA64_AR91      = 3163,
   CV_IA64_AR92      = 3164,
   CV_IA64_AR93      = 3165,
   CV_IA64_AR94      = 3166,
   CV_IA64_AR95      = 3167,
   CV_IA64_AR96      = 3168,
   CV_IA64_AR97      = 3169,
   CV_IA64_AR98      = 3170,
   CV_IA64_AR99      = 3171,
   CV_IA64_AR100     = 3172,
   CV_IA64_AR101     = 3173,
   CV_IA64_AR102     = 3174,
   CV_IA64_AR103     = 3175,
   CV_IA64_AR104     = 3176,
   CV_IA64_AR105     = 3177,
   CV_IA64_AR106     = 3178,
   CV_IA64_AR107     = 3179,
   CV_IA64_AR108     = 3180,
   CV_IA64_AR109     = 3181,
   CV_IA64_AR110     = 3182,
   CV_IA64_AR111     = 3183,
   CV_IA64_AR112     = 3184,
   CV_IA64_AR113     = 3185,
   CV_IA64_AR114     = 3186,
   CV_IA64_AR115     = 3187,
   CV_IA64_AR116     = 3188,
   CV_IA64_AR117     = 3189,
   CV_IA64_AR118     = 3190,
   CV_IA64_AR119     = 3191,
   CV_IA64_AR120     = 3192,
   CV_IA64_AR121     = 3193,
   CV_IA64_AR122     = 3194,
   CV_IA64_AR123     = 3195,
   CV_IA64_AR124     = 3196,
   CV_IA64_AR125     = 3197,
   CV_IA64_AR126     = 3198,
   CV_IA64_AR127     = 3199,

   // Control Registers

   CV_IA64_ApDCR     = 4096,
   CV_IA64_ApITM     = 4097,
   CV_IA64_ApIVA     = 4098,
   CV_IA64_CR3	     = 4099,
   CV_IA64_CR4	     = 4100,
   CV_IA64_CR5	     = 4101,
   CV_IA64_CR6	     = 4102,
   CV_IA64_CR7	     = 4103,
   CV_IA64_ApPTA     = 4104,
   CV_IA64_CR9	     = 4105,
   CV_IA64_CR10      = 4106,
   CV_IA64_CR11      = 4107,
   CV_IA64_CR12      = 4108,
   CV_IA64_CR13      = 4109,
   CV_IA64_CR14      = 4110,
   CV_IA64_CR15      = 4111,
   CV_IA64_StIPSR    = 4112,
   CV_IA64_StISR     = 4113,
   CV_IA64_StIDA     = 4114,
   CV_IA64_StIIP     = 4115,
   CV_IA64_StIDTR    = 4116,
   CV_IA64_StIITR    = 4117,
   CV_IA64_StIIPA    = 4118,
   CV_IA64_StIFS     = 4119,
   CV_IA64_StIIM     = 4120,
   CV_IA64_StIHA     = 4121,
   CV_IA64_CR26      = 4122,
   CV_IA64_CR27      = 4123,
   CV_IA64_CR28      = 4124,
   CV_IA64_CR29      = 4125,
   CV_IA64_CR30      = 4126,
   CV_IA64_CR31      = 4127,
   CV_IA64_CR32      = 4128,
   CV_IA64_CR33      = 4129,
   CV_IA64_CR34      = 4130,
   CV_IA64_CR35      = 4131,
   CV_IA64_CR36      = 4132,
   CV_IA64_CR37      = 4133,
   CV_IA64_CR38      = 4134,
   CV_IA64_CR39      = 4135,
   CV_IA64_CR40      = 4136,
   CV_IA64_CR41      = 4137,
   CV_IA64_CR42      = 4138,
   CV_IA64_CR43      = 4139,
   CV_IA64_CR44      = 4140,
   CV_IA64_CR45      = 4141,
   CV_IA64_CR46      = 4142,
   CV_IA64_CR47      = 4143,
   CV_IA64_CR48      = 4144,
   CV_IA64_CR49      = 4145,
   CV_IA64_CR50      = 4146,
   CV_IA64_CR51      = 4147,
   CV_IA64_CR52      = 4148,
   CV_IA64_CR53      = 4149,
   CV_IA64_CR54      = 4150,
   CV_IA64_CR55      = 4151,
   CV_IA64_CR56      = 4152,
   CV_IA64_CR57      = 4153,
   CV_IA64_CR58      = 4154,
   CV_IA64_CR59      = 4155,
   CV_IA64_CR60      = 4156,
   CV_IA64_CR61      = 4157,
   CV_IA64_CR62      = 4158,
   CV_IA64_CR63      = 4159,
   CV_IA64_CR64      = 4160,
   CV_IA64_CR65      = 4161,
   CV_IA64_SaLID     = 4162,
   CV_IA64_CR67      = 4163,
   CV_IA64_CR68      = 4164,
   CV_IA64_CR69      = 4165,
   CV_IA64_CR70      = 4166,
   CV_IA64_SaIVR     = 4167,
   CV_IA64_SaTPR     = 4168,
   CV_IA64_CR73      = 4169,
   CV_IA64_CR74      = 4170,
   CV_IA64_SaEOI     = 4171,
   CV_IA64_CR76      = 4172,
   CV_IA64_CR77      = 4173,
   CV_IA64_CR78      = 4174,
   CV_IA64_CR79      = 4175,
   CV_IA64_CR80      = 4176,
   CV_IA64_CR81      = 4177,
   CV_IA64_CR82      = 4178,
   CV_IA64_CR83      = 4179,
   CV_IA64_CR84      = 4180,
   CV_IA64_CR85      = 4181,
   CV_IA64_CR86      = 4182,
   CV_IA64_CR87      = 4183,
   CV_IA64_CR88      = 4184,
   CV_IA64_CR89      = 4185,
   CV_IA64_CR90      = 4186,
   CV_IA64_CR91      = 4187,
   CV_IA64_CR92      = 4188,
   CV_IA64_CR93      = 4189,
   CV_IA64_CR94      = 4190,
   CV_IA64_CR95      = 4191,
   CV_IA64_SaIRR0    = 4192,
   CV_IA64_CR97      = 4193,
   CV_IA64_SaIRR1    = 4194,
   CV_IA64_CR99      = 4195,
   CV_IA64_SaIRR2    = 4196,
   CV_IA64_CR101     = 4197,
   CV_IA64_SaIRR3    = 4198,
   CV_IA64_CR103     = 4199,
   CV_IA64_CR104     = 4200,
   CV_IA64_CR105     = 4201,
   CV_IA64_CR106     = 4202,
   CV_IA64_CR107     = 4203,
   CV_IA64_CR108     = 4204,
   CV_IA64_CR109     = 4205,
   CV_IA64_CR110     = 4206,
   CV_IA64_CR111     = 4207,
   CV_IA64_CR112     = 4208,
   CV_IA64_CR113     = 4209,
   CV_IA64_SaITV     = 4210,
   CV_IA64_CR115     = 4211,
   CV_IA64_SaPMV     = 4212,
   CV_IA64_SaLRR0    = 4213,
   CV_IA64_SaLRR1    = 4214,
   CV_IA64_SaCMCV    = 4215,
   CV_IA64_CR120     = 4216,
   CV_IA64_CR121     = 4217,
   CV_IA64_CR122     = 4218,
   CV_IA64_CR123     = 4219,
   CV_IA64_CR124     = 4220,
   CV_IA64_CR125     = 4221,
   CV_IA64_CR126     = 4222,
   CV_IA64_CR127     = 4223,

   // Protection Key Registers

   CV_IA64_Pkr0      = 5120,
   CV_IA64_Pkr1      = 5121,
   CV_IA64_Pkr2      = 5122,
   CV_IA64_Pkr3      = 5123,
   CV_IA64_Pkr4      = 5124,
   CV_IA64_Pkr5      = 5125,
   CV_IA64_Pkr6      = 5126,
   CV_IA64_Pkr7      = 5127,
   CV_IA64_Pkr8      = 5128,
   CV_IA64_Pkr9      = 5129,
   CV_IA64_Pkr10     = 5130,
   CV_IA64_Pkr11     = 5131,
   CV_IA64_Pkr12     = 5132,
   CV_IA64_Pkr13     = 5133,
   CV_IA64_Pkr14     = 5134,
   CV_IA64_Pkr15     = 5135,

   // Region Registers

   CV_IA64_Rr0	     = 6144,
   CV_IA64_Rr1	     = 6145,
   CV_IA64_Rr2	     = 6146,
   CV_IA64_Rr3	     = 6147,
   CV_IA64_Rr4	     = 6148,
   CV_IA64_Rr5	     = 6149,
   CV_IA64_Rr6	     = 6150,
   CV_IA64_Rr7	     = 6151,

   // Performance Monitor Data Registers

   CV_IA64_PFD0      = 7168,
   CV_IA64_PFD1      = 7169,
   CV_IA64_PFD2      = 7170,
   CV_IA64_PFD3      = 7171,
   CV_IA64_PFD4      = 7172,
   CV_IA64_PFD5      = 7173,
   CV_IA64_PFD6      = 7174,
   CV_IA64_PFD7      = 7175,

   // Performance Monitor Config Registers

   CV_IA64_PFC0      = 7424,
   CV_IA64_PFC1      = 7425,
   CV_IA64_PFC2      = 7426,
   CV_IA64_PFC3      = 7427,
   CV_IA64_PFC4      = 7428,
   CV_IA64_PFC5      = 7429,
   CV_IA64_PFC6      = 7430,
   CV_IA64_PFC7      = 7431,

   // Instruction Translation Registers

   CV_IA64_TrI0      = 8192,
   CV_IA64_TrI1      = 8193,
   CV_IA64_TrI2      = 8194,
   CV_IA64_TrI3      = 8195,
   CV_IA64_TrI4      = 8196,
   CV_IA64_TrI5      = 8197,
   CV_IA64_TrI6      = 8198,
   CV_IA64_TrI7      = 8199,

   // Data Translation Registers

   CV_IA64_TrD0      = 8320,
   CV_IA64_TrD1      = 8321,
   CV_IA64_TrD2      = 8322,
   CV_IA64_TrD3      = 8323,
   CV_IA64_TrD4      = 8324,
   CV_IA64_TrD5      = 8325,
   CV_IA64_TrD6      = 8326,
   CV_IA64_TrD7      = 8327,

   // Instruction Breakpoint Registers

   CV_IA64_DbI0      = 8448,
   CV_IA64_DbI1      = 8449,
   CV_IA64_DbI2      = 8450,
   CV_IA64_DbI3      = 8451,
   CV_IA64_DbI4      = 8452,
   CV_IA64_DbI5      = 8453,
   CV_IA64_DbI6      = 8454,
   CV_IA64_DbI7      = 8455,

   // Data Breakpoint Registers

   CV_IA64_DbD0      = 8576,
   CV_IA64_DbD1      = 8577,
   CV_IA64_DbD2      = 8578,
   CV_IA64_DbD3      = 8579,
   CV_IA64_DbD4      = 8580,
   CV_IA64_DbD5      = 8581,
   CV_IA64_DbD6      = 8582,
   CV_IA64_DbD7      = 8583,
};



// --------------------------------------------------------------------
// Pointer to Member Representations
// --------------------------------------------------------------------

//  representation of a 16:32 near pointer to data for a class with
//  or without virtual functions and no virtual bases

struct CV_PDMR32_NVVFCN
{
   // NULL = (0x00000000)

   DWORD ibData;		       // Displacement to data
};


//  representation of a 16:32 near pointer to data for a class
//  with virtual bases

struct CV_PDMR32_VBASE
{
   // NULL = (?, ?, 0xFFFFFFFF)

   DWORD  ibData;		       // Displacement to data
   DWORD  ibThis;		       // This pointer displacement to vbptr
   DWORD  ibVbase;		       // Displacement within vbase table
};


//  representation of a 16:32 near pointer to member function for a
//  class with no virtual bases and a single address point

struct CV_PMFR32_NVSA
{
   // NULL = (0x00000000)

   DWORD pfn;			       // Address of function
};


//  representation of a 16:32 near pointer to member function for a
//  class with no virtual bases and multiple address points

struct CV_PMFR32_NVMA
{
   // NULL = (0x00000000, ?)

   DWORD pfn;			       // Address of function
   DWORD disp;			       // UNDONE: ?
};


//  representation of a 16:32 near pointer to member function for a
//  class with virtual bases

struct CV_PMFR32_VBASE
{
   // NULL = (0x00000000, ?, ?, ?)

   DWORD pfn;			       // Address of function
   WORD  ibData;		       // Displacement to data
   WORD  ibThis;		       // This pointer displacement to vbptr
   WORD  ibVbase;		       // Displacement within vbase table
};


#pragma pack(pop)
