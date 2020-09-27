// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     compstra.c
// Contents: CompareStringA functionality.  Contains CompareString table initilization functions
//           and includes the C file that has the actual CompareString function definition (we set
//           a define so that the 'A' functionality gets compiled rather than 'W').
// Author:	 Jeff Simon (jeffsim) 16-May-2001
//
// Copyright Microsoft Corporation
// 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "basedll.h"
#pragma hdrstop


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Internal Defines - specific to this file
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Sort order (lower letters are sorted before upper letters; symbols are sorted before numbers, ...)
// UNDONE-ERR: Just guessing on the sort order of the non-ascii chars (kanji and kanas).  Guessing
//             that the sort order (first->last) would be "ascii", "kata", "hira", "kanji".
#define COMP_INVALID    0x80000000
#define COMP_KANJI      0x00800000
#define COMP_HIRAGANA   0x00400000
#define COMP_KATAKANA   0x00200000
#define COMP_ULETTER    0x00100000
#define COMP_LLETTER    0x00080000
#define COMP_NUMBER     0x00040000
#define COMP_SYMBOL     0x00020000
#define COMP_WORDSORT   0x00010000


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Global variables
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Conversion table.  This will be filled in algorithmically by InitLocTable at startup.
// UNDONE-ERR: Currently only big enough to hold the 256 ASCII chars.  Not sure how to handle
//             Kanji and the kanas -- will I add them to the table, or handle them algorithmically?
DWORD g_rgdwLoc[256] = {
    COMP_INVALID |   0, COMP_INVALID |   1, COMP_INVALID |   2, COMP_INVALID |   3, // 
    COMP_INVALID |   4, COMP_INVALID |   5, COMP_INVALID |   6, COMP_INVALID |   7, // 
    COMP_INVALID |   8, COMP_INVALID |   9, COMP_INVALID |  10, COMP_INVALID |  11, // 
    COMP_INVALID |  12, COMP_INVALID |  13, COMP_INVALID |  14, COMP_INVALID |  15, // 
    COMP_INVALID |  16, COMP_INVALID |  17, COMP_INVALID |  18, COMP_INVALID |  19, // 
    COMP_INVALID |  20, COMP_INVALID |  21, COMP_INVALID |  22, COMP_INVALID |  23, // 
    COMP_INVALID |  24, COMP_INVALID |  25, COMP_INVALID |  26, COMP_INVALID |  27, // 
    COMP_INVALID |  28, COMP_INVALID |  29, COMP_INVALID |  30, COMP_INVALID |  31, // 

    COMP_SYMBOL  |  32, COMP_SYMBOL  |  33, COMP_SYMBOL  |  34, COMP_SYMBOL  |  35, //  !"#
    COMP_SYMBOL  |  36, COMP_SYMBOL  |  37, COMP_SYMBOL  |  38, COMP_SYMBOL  |  39, // $%&'
    COMP_SYMBOL  |  40, COMP_SYMBOL  |  41, COMP_SYMBOL  |  42, COMP_SYMBOL  |  43, // ()*+
    COMP_SYMBOL  |  44, COMP_SYMBOL  |  45, COMP_SYMBOL  |  45, COMP_SYMBOL  |  47, // ,-./
    COMP_NUMBER  |  48, COMP_NUMBER  |  49, COMP_NUMBER  |  50, COMP_NUMBER  |  51, // 0123
    COMP_NUMBER  |  52, COMP_NUMBER  |  53, COMP_NUMBER  |  54, COMP_NUMBER  |  55, // 4567
    COMP_NUMBER  |  56, COMP_NUMBER  |  57, COMP_SYMBOL  |  58, COMP_SYMBOL  |  59, // 89:;
    COMP_SYMBOL  |  60, COMP_SYMBOL  |  61, COMP_SYMBOL  |  62, COMP_SYMBOL  |  63, // <=>?
    COMP_SYMBOL  |  64, COMP_ULETTER |  65, COMP_ULETTER |  66, COMP_ULETTER |  67, // @ABC
	
    COMP_ULETTER |  68, COMP_ULETTER |  69, COMP_ULETTER |  70, COMP_ULETTER |  71, // DEFG
    COMP_ULETTER |  72, COMP_ULETTER |  73, COMP_ULETTER |  74, COMP_ULETTER |  75, // HIJK
    COMP_ULETTER |  76, COMP_ULETTER |  77, COMP_ULETTER |  78, COMP_ULETTER |  79, // LMNO
    COMP_ULETTER |  80, COMP_ULETTER |  81, COMP_ULETTER |  82, COMP_ULETTER |  83, // PQRS
    COMP_ULETTER |  84, COMP_ULETTER |  85, COMP_ULETTER |  86, COMP_ULETTER |  87, // TUVW
    COMP_ULETTER |  88, COMP_ULETTER |  89, COMP_ULETTER |  90, COMP_SYMBOL  |  91, // XYZ[
    COMP_SYMBOL  |  92, COMP_SYMBOL  |  93, COMP_SYMBOL  |  94, COMP_SYMBOL  |  95, // \]^_
    COMP_SYMBOL  |  96, COMP_LLETTER |  97, COMP_LLETTER |  98, COMP_LLETTER |  99, // `abc

    COMP_LLETTER | 100, COMP_LLETTER | 101, COMP_LLETTER | 102, COMP_LLETTER | 103, // defg
    COMP_LLETTER | 104, COMP_LLETTER | 105, COMP_LLETTER | 106, COMP_LLETTER | 107, // hijk
    COMP_LLETTER | 108, COMP_LLETTER | 109, COMP_LLETTER | 110, COMP_LLETTER | 111, // lmno
    COMP_LLETTER | 112, COMP_LLETTER | 113, COMP_LLETTER | 114, COMP_LLETTER | 115, // pqrs
    COMP_LLETTER | 116, COMP_LLETTER | 117, COMP_LLETTER | 118, COMP_LLETTER | 119, // tuvw
    COMP_LLETTER | 120, COMP_LLETTER | 121, COMP_LLETTER | 122, COMP_SYMBOL  | 123, // xyz{
    COMP_SYMBOL  | 124, COMP_SYMBOL  | 125, COMP_SYMBOL  | 126, COMP_INVALID | 127, // |}~
    COMP_INVALID | 128, COMP_INVALID | 129, COMP_INVALID | 130, COMP_INVALID | 131, // 

    COMP_INVALID | 132, COMP_INVALID | 133, COMP_INVALID | 134, COMP_INVALID | 135, // 
    COMP_INVALID | 136, COMP_INVALID | 137, COMP_INVALID | 138, COMP_INVALID | 139, // 
    COMP_INVALID | 140, COMP_INVALID | 141, COMP_INVALID | 142, COMP_INVALID | 143, // 
    COMP_INVALID | 144, COMP_INVALID | 145, COMP_INVALID | 145, COMP_INVALID | 147, // 
    COMP_INVALID | 148, COMP_INVALID | 149, COMP_INVALID | 150, COMP_INVALID | 151, // 
    COMP_INVALID | 152, COMP_INVALID | 153, COMP_INVALID | 154, COMP_INVALID | 155, // 
    COMP_INVALID | 156, COMP_INVALID | 157, COMP_INVALID | 158, COMP_INVALID | 159, // 
    COMP_INVALID | 160, COMP_SYMBOL  | 161, COMP_SYMBOL  | 162, COMP_SYMBOL  | 163, //  ¡¢£
    COMP_SYMBOL  | 164, COMP_SYMBOL  | 165, COMP_SYMBOL  | 166, COMP_SYMBOL  | 167, // ¤¥¦§
	
    COMP_SYMBOL  | 168, COMP_SYMBOL  | 169, COMP_SYMBOL  | 170, COMP_SYMBOL  | 171, // ¨©ª«
    COMP_SYMBOL  | 172, COMP_SYMBOL  | 173, COMP_SYMBOL  | 174, COMP_SYMBOL  | 175, // ¬­®¯
    COMP_SYMBOL  | 176, COMP_SYMBOL  | 177, COMP_SYMBOL  | 178, COMP_SYMBOL  | 179, // °±²³
    COMP_SYMBOL  | 180, COMP_SYMBOL  | 181, COMP_SYMBOL  | 182, COMP_SYMBOL  | 183, // ´µ¶·
    COMP_SYMBOL  | 184, COMP_SYMBOL  | 185, COMP_SYMBOL  | 186, COMP_SYMBOL  | 187, // ¸¹º»
    COMP_SYMBOL  | 188, COMP_SYMBOL  | 189, COMP_SYMBOL  | 190, COMP_SYMBOL  | 191, // ¼½¾¿
    COMP_ULETTER | 192, COMP_ULETTER | 193, COMP_ULETTER | 194, COMP_ULETTER | 195, // ÀÁÂÃ
    COMP_ULETTER | 196, COMP_ULETTER | 197, COMP_ULETTER | 198, COMP_ULETTER | 199, // ÄÅÆÇ

    COMP_ULETTER | 200, COMP_ULETTER | 201, COMP_ULETTER | 202, COMP_ULETTER | 203, // ÈÉÊË
    COMP_ULETTER | 204, COMP_ULETTER | 205, COMP_ULETTER | 206, COMP_ULETTER | 207, // ÌÍÎÏ
    COMP_ULETTER | 208, COMP_ULETTER | 209, COMP_ULETTER | 210, COMP_ULETTER | 211, // ÐÑÒÓ
    COMP_ULETTER | 212, COMP_ULETTER | 213, COMP_ULETTER | 214, COMP_SYMBOL  | 215, // ÔÕÖ×
    COMP_ULETTER | 216, COMP_ULETTER | 217, COMP_ULETTER | 218, COMP_ULETTER | 219, // ØÙÚÛ
    COMP_ULETTER | 220, COMP_ULETTER | 221, COMP_ULETTER | 222, COMP_ULETTER | 223, // ÜÝÞß
    COMP_LLETTER | 224, COMP_LLETTER | 225, COMP_LLETTER | 226, COMP_LLETTER | 227, // àáâã
    COMP_LLETTER | 228, COMP_LLETTER | 229, COMP_LLETTER | 230, COMP_LLETTER | 231, // äåæç

    COMP_LLETTER | 232, COMP_LLETTER | 233, COMP_LLETTER | 234, COMP_LLETTER | 235, // èéêë
    COMP_LLETTER | 236, COMP_LLETTER | 237, COMP_LLETTER | 238, COMP_LLETTER | 239, // ìíîï
    COMP_LLETTER | 240, COMP_LLETTER | 241, COMP_LLETTER | 242, COMP_LLETTER | 243, // ðñòó
    COMP_LLETTER | 244, COMP_LLETTER | 245, COMP_LLETTER | 245, COMP_SYMBOL  | 247, // ôõö÷
    COMP_LLETTER | 248, COMP_LLETTER | 249, COMP_LLETTER | 250, COMP_LLETTER | 251, // øùúû
    COMP_LLETTER | 252, COMP_LLETTER | 253, COMP_LLETTER | 254, COMP_LLETTER | 255, // üýþ
};

// Now include the C file that actually defines CompareStringA.  Define 'COMPSTRA' first so that
// the file knows that we want CompareStringA, and not CompareStringW
#define COMPSTRA
#include "compstr.c"