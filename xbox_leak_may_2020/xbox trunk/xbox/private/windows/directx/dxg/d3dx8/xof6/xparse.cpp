#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)

/*==========================================================================
 *
 *  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
 *
 *  File: xparse.y
 *
 *  Content:
 *
 *@@BEGIN_MSINTERNAL
 *
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *   04/19/96   v-paulf This header added
 *
 *@@END_MSINTERNAL
 *
 ***************************************************************************/


#undef XOFDEBUG

#include "precomp.h"

#define YYPREFIX "yy"
#define YYDEBUG 0

CHAR *Strdup(const CHAR *s)
{
    CHAR *d = NULL;

    if (s) {
        int n = (strlen(s) + 1) * sizeof(CHAR);

        if (SUCCEEDED(XMalloc((void **) &d, n)))
            memcpy(d, s, n);
        else
            DPF_ERR("Failed to allocate space for string");
    }

    return d;
}

static int yylex_ascii(XStreamRead *pStream);
static int yylex_binary(XStreamRead *pStream);

typedef union {
        stgInt              stgInt;
        stgFlt              stgFlt;
        GUID                guid;
        char*               string;
        XObject*            xobject;
        XDataPart*          xdatapart;
        XDataRef*           xdataref;
        XDataPartList*      xdatapartList;
        integerList*        intList;
        integerListBinData  intListBinData;
        floatList*          fltList;
        floatListBinData    fltListBinData;
        stringList*         strList;
        XTemplate*          xtemplate;
        XDimension*         elementSize;
        XDimensionList*     dimList;
        XIdentifier*        idInf;
        XTemplateOptionList* toList;
        XTemplateMember*    templMember;
        XTemplateMemberList* tmList;
        XTemplateInfo*      templInfo;
        XBlobData*          binaryData;
} YYSTYPE;
#define YT_END_OF_FILE 0
#define YT_YIELD 0
#define YT_ERROR 255
#define YT_NAME 1
#define YT_STRING 2
#define YT_INTEGER 3
#define YT_REALNUM 4
#define YT_GUID 5
#define YT_INTEGER_LIST 6
#define YT_REALNUM_LIST 7
#define YT_BINARY_DATA 254
#define YT_OBRACE 10
#define YT_CBRACE 11
#define YT_OPAREN 12
#define YT_CPAREN 13
#define YT_OBRACKET 14
#define YT_CBRACKET 15
#define YT_OANGLE 16
#define YT_CANGLE 17
#define YT_DOT 18
#define YT_COMMA 19
#define YT_SEMICOLON 20
#define YT_OBJECT 30
#define YT_TEMPLATE 31
#define YT_WORD 40
#define YT_DWORD 41
#define YT_FLOAT 42
#define YT_DOUBLE 43
#define YT_CHAR 44
#define YT_UCHAR 45
#define YT_SWORD 46
#define YT_SDWORD 47
#define YT_VOID 48
#define YT_LPSTR 49
#define YT_UNICODE 50
#define YT_CSTRING 51
#define YT_ARRAY 52
#define YT_ULONGLONG 53
#define YT_BINARY 54
#define YT_BINARY_RESOURCE 55
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,   36,   36,   18,   18,   20,   20,   20,   19,
   19,   19,   19,   19,   19,   35,   35,   26,   26,   25,
   25,   21,   21,   21,   33,   33,   32,   32,   34,   34,
   23,    6,    6,    7,   29,   29,   27,   28,   28,   22,
    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
    5,    5,    5,   17,   24,    4,    4,    8,    3,    3,
    1,    2,    9,    9,   11,   10,   10,   12,   12,   14,
   13,   13,   15,   16,   16,   31,   31,   30,   38,   38,
   39,   39,   37,   37,   40,
};
short yylen[] = {                                         2,
    0,    1,    1,    2,    6,    6,    0,    1,    2,    1,
    6,    1,    1,    1,    6,    4,    1,    0,    1,    1,
    2,    1,    1,    1,    3,    1,    1,    2,    3,    2,
    5,    1,    1,    1,    1,    2,    3,    1,    1,    3,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    5,    3,    1,    1,    1,    0,    1,
    1,    1,    2,    1,    1,    1,    3,    2,    1,    1,
    1,    3,    2,    1,    3,    0,    1,    1,    1,    2,
    1,    2,    0,    1,    1,
};
short yydefred[] = {                                      0,
   61,    0,   41,   42,   43,   44,   45,   46,   47,   48,
   49,   50,   51,   52,   53,    0,   56,    0,   57,    3,
    0,   58,    0,   60,    0,    4,    0,    0,   78,    0,
   77,    0,    0,    0,    0,   20,   22,   23,   24,    0,
    0,    0,   62,   66,   71,   65,   70,    0,   74,    0,
   12,    0,   64,   13,    0,   69,   14,    0,   10,    8,
    0,   33,   32,    0,    0,    0,   21,    0,    6,    0,
    0,    0,   79,   81,    0,    0,    0,    0,    5,    9,
   34,    0,   55,   40,    0,    0,    0,    0,    0,   27,
    0,    0,    0,   67,   85,   82,   80,   84,   72,   75,
    0,   35,    0,    0,   30,    0,   28,   16,    0,    0,
    0,   38,   39,    0,   31,   36,   25,   29,    0,   54,
    0,   37,   15,   11,
};
short yydgoto[] = {                                      16,
   17,   49,   25,   50,   19,   64,   82,   23,   51,   52,
   53,   54,   55,   56,   57,   58,   59,   20,   60,   61,
   36,   37,   38,   39,   40,   41,  102,  114,  103,   31,
   32,   88,   89,   90,   42,   21,   97,   75,   76,   98,
};
short yysindex[] = {                                    310,
    0,   14,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   14,    0,    0,
  310,    0,   15,    0,   41,    0,   49,   49,    0,  325,
    0,  209,  344,   14,   14,    0,    0,    0,    0,  325,
   46,   54,    0,    0,    0,    0,    0,    4,    0,   14,
    0,   44,    0,    0,   44,    0,    0,   44,    0,    0,
   68,    0,    0,   14,   47,   53,    0,    1,    0,   14,
   49,   70,    0,    0,   78,   57,   79,   80,    0,    0,
    0,   71,    0,    0,   66,   72,   49,    5,   83,    0,
   49,   72,   49,    0,    0,    0,    0,    0,    0,    0,
    9,    0,   16,   76,    0,   72,    0,    0,   84,   89,
  209,    0,    0,   87,    0,    0,    0,    0, -153,    0,
   86,    0,    0,    0,
};
short yyrindex[] = {                                    103,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   94,    0,    0,
  105,    0,    0,    0,    0,    0,    0,  140,    0,   92,
    0,   96,    0,  100,  100,    0,    0,    0,    0,   39,
    0,    0,    0,    0,    0,    0,    0,   30,    0,   94,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   32,
   28,    0,    0,    0,  224,  155,  278,  293,    0,    0,
    0,    0,    0,    0,    0,    3,    2,  101,    0,    0,
  112,  113,  140,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    3,    0,    0,    0,    0,
   96,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
   -2,   56,  -26,   13,   12,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  104,  -60,   27,
  108,    0,    0,    0,    0,    0,   42,    0,    0,  122,
  -64,    0,    0,   64,    0,    0,  -66,  -44,    0,    0,
};
#define YYTABLESIZE 398
short yytable[] = {                                      22,
   80,    1,   76,   83,    1,    1,   92,   65,   66,    1,
   77,  112,   18,   78,    1,   24,   76,   83,   85,  105,
   76,   71,  106,   72,   27,  110,  109,   34,  111,  101,
   62,   24,   24,   18,   59,  115,   59,   34,   76,  118,
   59,   35,   59,   91,   63,   24,   76,   24,   59,   17,
   28,   35,   19,   29,   86,   76,   83,   70,   86,   68,
   80,   81,   73,   74,   69,   87,   83,   24,    1,   43,
   44,   45,   84,   46,   47,   95,   96,   48,   79,   93,
   94,   43,   99,  104,  101,   87,    1,   43,   44,   45,
   95,   46,   47,  117,  119,   48,  124,  108,  113,  120,
  123,  122,    1,   59,    2,   18,    7,    3,    4,    5,
    6,    7,    8,    9,   10,   26,   11,   12,   13,   59,
   14,   15,   76,   83,   26,    3,    4,    5,    6,    7,
    8,    9,   10,  100,   11,   12,   13,  121,   14,   15,
   76,   76,   76,   76,  116,   76,   76,   67,   30,   76,
   76,  107,    0,    0,    0,   83,   83,   83,   83,    0,
   83,   83,    0,    0,   83,   83,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   76,
   76,   76,   76,   76,   76,   76,   76,    0,   76,   76,
   76,    0,   76,   76,   83,   83,   83,   83,   83,   83,
   83,   83,    0,   83,   83,   83,    0,   83,   83,    1,
   43,   44,   45,    0,   46,   47,    0,    0,   48,    0,
    0,    0,    0,    0,   63,   63,    0,   63,    0,   63,
   63,    0,    0,   63,   63,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    3,    4,
    5,    6,    7,    8,    9,   10,    0,   11,   12,   13,
    0,   14,   15,   63,   63,   63,   63,   63,   63,   63,
   63,    0,   63,   63,   63,    0,   63,   63,   68,   68,
   68,    0,    0,   68,   68,    0,    0,   68,   68,    0,
    0,    0,    0,   73,    0,   73,   73,    0,   73,   73,
    0,    0,   73,   73,    0,    0,    0,    0,    0,    0,
    1,    0,    0,    0,    0,    0,    0,   68,   68,   68,
   68,   68,   68,   68,   68,    1,   68,   68,   68,    0,
   68,   68,   73,   73,   73,   73,   73,   73,   73,   73,
    2,   73,   73,   73,    1,   73,   73,    0,    0,    3,
    4,    5,    6,    7,    8,    9,   10,    0,   11,   12,
   13,    0,   14,   15,    3,    4,    5,    6,    7,    8,
    9,   10,    0,   11,   12,   13,   33,   14,   15,    0,
    0,    0,    0,    3,    4,    5,    6,    7,    8,    9,
   10,    0,   11,   12,   13,    0,   14,   15,
};
short yycheck[] = {                                       2,
   61,    1,    1,    1,    1,    1,   71,   34,   35,    1,
   55,    3,    0,   58,    1,   18,   15,   15,   18,   86,
   19,   48,   87,   50,   10,   92,   91,   30,   93,   14,
   33,   34,   35,   21,    5,   20,    5,   40,   11,  106,
   11,   30,   11,   70,   33,   48,   19,   50,   19,   11,
   10,   40,   14,    5,   54,   54,   54,   54,   54,   14,
  121,   64,   19,   20,   11,   68,   20,   70,    1,    2,
    3,    4,   20,    6,    7,   19,   20,   10,   11,   10,
    3,    2,    4,   18,   14,   88,    1,    2,    3,    4,
   19,    6,    7,   18,   11,   10,   11,   15,  101,   11,
  254,   15,    0,   10,    0,   14,   11,   40,   41,   42,
   43,   44,   45,   46,   47,   15,   49,   50,   51,   20,
   53,   54,   11,   11,   21,   40,   41,   42,   43,   44,
   45,   46,   47,   78,   49,   50,   51,  111,   53,   54,
    1,    2,    3,    4,  103,    6,    7,   40,   27,   10,
   11,   88,   -1,   -1,   -1,    1,    2,    3,    4,   -1,
    6,    7,   -1,   -1,   10,   11,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   40,
   41,   42,   43,   44,   45,   46,   47,   -1,   49,   50,
   51,   -1,   53,   54,   40,   41,   42,   43,   44,   45,
   46,   47,   -1,   49,   50,   51,   -1,   53,   54,    1,
    2,    3,    4,   -1,    6,    7,   -1,   -1,   10,   -1,
   -1,   -1,   -1,   -1,    1,    2,   -1,    4,   -1,    6,
    7,   -1,   -1,   10,   11,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   40,   41,
   42,   43,   44,   45,   46,   47,   -1,   49,   50,   51,
   -1,   53,   54,   40,   41,   42,   43,   44,   45,   46,
   47,   -1,   49,   50,   51,   -1,   53,   54,    1,    2,
    3,   -1,   -1,    6,    7,   -1,   -1,   10,   11,   -1,
   -1,   -1,   -1,    1,   -1,    3,    4,   -1,    6,    7,
   -1,   -1,   10,   11,   -1,   -1,   -1,   -1,   -1,   -1,
    1,   -1,   -1,   -1,   -1,   -1,   -1,   40,   41,   42,
   43,   44,   45,   46,   47,    1,   49,   50,   51,   -1,
   53,   54,   40,   41,   42,   43,   44,   45,   46,   47,
   31,   49,   50,   51,    1,   53,   54,   -1,   -1,   40,
   41,   42,   43,   44,   45,   46,   47,   -1,   49,   50,
   51,   -1,   53,   54,   40,   41,   42,   43,   44,   45,
   46,   47,   -1,   49,   50,   51,   52,   53,   54,   -1,
   -1,   -1,   -1,   40,   41,   42,   43,   44,   45,   46,
   47,   -1,   49,   50,   51,   -1,   53,   54,
};
#define YYFINAL 16
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 255
#if YYDEBUG
char *yyname[] = {
"end-of-file","YT_NAME","YT_STRING","YT_INTEGER","YT_REALNUM","YT_GUID",
"YT_INTEGER_LIST","YT_REALNUM_LIST",0,0,"YT_OBRACE","YT_CBRACE","YT_OPAREN",
"YT_CPAREN","YT_OBRACKET","YT_CBRACKET","YT_OANGLE","YT_CANGLE","YT_DOT",
"YT_COMMA","YT_SEMICOLON",0,0,0,0,0,0,0,0,0,"YT_OBJECT","YT_TEMPLATE",0,0,0,0,0,
0,0,0,"YT_WORD","YT_DWORD","YT_FLOAT","YT_DOUBLE","YT_CHAR","YT_UCHAR",
"YT_SWORD","YT_SDWORD","YT_VOID","YT_LPSTR","YT_UNICODE","YT_CSTRING",
"YT_ARRAY","YT_ULONGLONG","YT_BINARY","YT_BINARY_RESOURCE",0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"YT_BINARY_DATA",
"YT_ERROR",
};
char *yyrule[] = {
"$accept : file",
"file :",
"file : object_list",
"object_list : object",
"object_list : object_list object",
"object : identifier opt_name YT_OBRACE opt_clsid data_parts_list YT_CBRACE",
"object : YT_TEMPLATE template_name YT_OBRACE clsid template_parts YT_CBRACE",
"data_parts_list :",
"data_parts_list : data_part",
"data_parts_list : data_parts_list data_part",
"data_part : data_reference",
"data_part : identifier opt_name YT_OBRACE opt_clsid data_parts_list YT_CBRACE",
"data_part : number_list",
"data_part : float_list",
"data_part : string_list",
"data_part : YT_OBRACE YT_BINARY opt_name opt_clsid YT_CBRACE YT_BINARY_DATA",
"template_parts : template_members_part YT_OBRACKET template_option_info YT_CBRACKET",
"template_parts : template_members_list",
"template_members_part :",
"template_members_part : template_members_list",
"template_members_list : template_members",
"template_members_list : template_members_list template_members",
"template_members : primitive",
"template_members : array",
"template_members : template_reference",
"template_option_info : YT_DOT YT_DOT YT_DOT",
"template_option_info : template_option_list",
"template_option_list : template_option_part",
"template_option_list : template_option_list template_option_part",
"template_option_part : name opt_clsid opt_comma",
"template_option_part : YT_BINARY opt_comma",
"array : YT_ARRAY array_data_type array_name dimension_list YT_SEMICOLON",
"array_data_type : primitive_type",
"array_data_type : name",
"array_name : name",
"dimension_list : dimension",
"dimension_list : dimension_list dimension",
"dimension : YT_OBRACKET dimension_size YT_CBRACKET",
"dimension_size : YT_INTEGER",
"dimension_size : name",
"primitive : primitive_type opt_name YT_SEMICOLON",
"primitive_type : YT_WORD",
"primitive_type : YT_DWORD",
"primitive_type : YT_FLOAT",
"primitive_type : YT_DOUBLE",
"primitive_type : YT_CHAR",
"primitive_type : YT_UCHAR",
"primitive_type : YT_SWORD",
"primitive_type : YT_SDWORD",
"primitive_type : YT_LPSTR",
"primitive_type : YT_UNICODE",
"primitive_type : YT_CSTRING",
"primitive_type : YT_ULONGLONG",
"primitive_type : YT_BINARY",
"data_reference : YT_OBRACE opt_name opt_clsid opt_comma YT_CBRACE",
"template_reference : name opt_name YT_SEMICOLON",
"identifier : name",
"identifier : primitive_type",
"template_name : name",
"opt_name :",
"opt_name : name",
"name : YT_NAME",
"string : YT_STRING",
"number_list : number_list_1 list_separator",
"number_list : number_list_2",
"number_list_2 : YT_INTEGER_LIST",
"number_list_1 : YT_INTEGER",
"number_list_1 : number_list_1 list_separator YT_INTEGER",
"float_list : float_list_1 list_separator",
"float_list : float_list_2",
"float_list_2 : YT_REALNUM_LIST",
"float_list_1 : YT_REALNUM",
"float_list_1 : float_list_1 list_separator YT_REALNUM",
"string_list : string_list_1 list_separator",
"string_list_1 : string",
"string_list_1 : string_list_1 list_separator string",
"opt_clsid :",
"opt_clsid : clsid",
"clsid : YT_GUID",
"list_separator : YT_COMMA",
"list_separator : semicolon_list opt_comma",
"semicolon_list : YT_SEMICOLON",
"semicolon_list : semicolon_list YT_SEMICOLON",
"opt_comma :",
"opt_comma : comma",
"comma : YT_COMMA",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE

struct _keyword {
    char    *keyword;
    int  token;
} keywords[] = {
    { "TEMPLATE",       YT_TEMPLATE    },
    { "WORD",           YT_WORD        },
    { "DWORD",          YT_DWORD       },
    { "FLOAT",          YT_FLOAT       },
    { "DOUBLE",         YT_DOUBLE      },
    { "CHAR",           YT_CHAR        },
    { "UCHAR",          YT_UCHAR       },
    { "SWORD",          YT_SWORD       },
    { "SDWORD",         YT_SDWORD      },
    { "STRING",         YT_LPSTR       },
    { "UNICODE",        YT_UNICODE     },
    { "CSTRING",        YT_CSTRING     },
    { "ARRAY",          YT_ARRAY       },
    { "ULONGLONG",      YT_ULONGLONG   },
    { "BINARY",         YT_BINARY      },
    { "BINARY_RESOURCE", YT_BINARY_RESOURCE }
};
static const size_t numKeywords = sizeof(keywords)/sizeof(struct _keyword);


static XStreamRead *yypStream = NULL;

static int lineNum  = 1;
static int flagYield = FALSE;
static int flagEof = FALSE;
static int flagError = FALSE;

#define BUFSIZE 512
static  int  nInBuf = 0;
static  char buf[BUFSIZE];


#ifdef XOFDEBUG
static stgInt scanInt;
static stgFlt scanFlt;
static DWORD scanIntListLen;
static DWORD scanFltListLen;
static char *scanStr = NULL;
static int scanStrLine = 0;
static char scanGuid[37];

static char *tokText(int tok)
{
    static char buf[32];

    switch (tok) {
    case YT_END_OF_FILE:    sprintf(buf, "END_OF_FILE"); break;
    case YT_ERROR:      sprintf(buf, "ERROR");  break;

    case YT_OBRACE:     sprintf(buf, "OBRACE"); break;
    case YT_CBRACE:     sprintf(buf, "CBRACE"); break;
    case YT_OPAREN:     sprintf(buf, "OPAREN"); break;
    case YT_CPAREN:     sprintf(buf, "CPAREN"); break;
    case YT_OBRACKET:   sprintf(buf, "OBRACKET"); break;
    case YT_CBRACKET:   sprintf(buf, "CBRACKET"); break;
    case YT_OANGLE:     sprintf(buf, "OANGLE"); break;
    case YT_CANGLE:     sprintf(buf, "CANGLE"); break;
    case YT_DOT:        sprintf(buf, "DOT");    break;
    case YT_COMMA:      sprintf(buf, "COMMA");  break;
    case YT_SEMICOLON:  sprintf(buf, "SEMICOLON"); break;

    case YT_NAME:       sprintf(buf, "NAME (%s)", scanStr);     break;
    case YT_STRING:     sprintf(buf, "STRING (%s)",scanStr);    break;
    case YT_INTEGER:    sprintf(buf, "INTEGER (0x%x)",scanInt); break;
    case YT_REALNUM:    sprintf(buf, "FLOAT (%f)", scanFlt);    break;
    case YT_GUID:       sprintf(buf, "GUID (%s)", scanGuid); break;
    case YT_INTEGER_LIST: sprintf(buf, "INTEGER LIST (len=0x%x)",
                                  scanIntListLen); break;
    case YT_REALNUM_LIST: sprintf(buf, "FLOAT LIST (len=0x%x)",
                                  scanFltListLen); break;
    case YT_WORD:       sprintf(buf, "WORD");   break;
    case YT_DWORD:      sprintf(buf, "DWORD");  break;
    case YT_FLOAT:      sprintf(buf, "FLOAT");  break;
    case YT_DOUBLE:     sprintf(buf, "DOUBLE"); break;
    case YT_CHAR:       sprintf(buf, "CHAR");   break;
    case YT_UCHAR:      sprintf(buf, "UCHAR");  break;
    case YT_SWORD:      sprintf(buf, "SWORD");  break;
    case YT_SDWORD:     sprintf(buf, "SDWORD"); break;
    case YT_VOID:       sprintf(buf, "VOID");   break;
    case YT_LPSTR:      sprintf(buf, "LPSTR");  break;
    case YT_UNICODE:    sprintf(buf, "UNICODE"); break;
    case YT_CSTRING:    sprintf(buf, "CSTRING"); break;
    case YT_ARRAY:      sprintf(buf, "ARRAY");  break;
    case YT_ULONGLONG:  sprintf(buf, "ULONGLONG"); break;
    case YT_BINARY:         sprintf(buf, "BINARY"); break;
    case YT_BINARY_RESOURCE:sprintf(buf, "BINARY_RESOURCE"); break;

    case YT_TEMPLATE:   sprintf(buf, "TEMPLATE");  break;
    case YT_OBJECT:     sprintf(buf, "OBJECT"); break;
    default:            sprintf(buf, "UNKNOWN"); break;
    }
    return buf;
}
#endif /* XOFDEBUG */

int yyGetUuid()
{
    char ch;
    int rtrn = YT_ERROR;            /* Assume */
    char *p = buf;

    while (((ch = yypStream->GetByte()) != EOF) && (ch != '>') &&
           (ch == '-' || isxdigit(ch)))
        *p++ = ch;

    *p = '\0';
    if (ch == '>') {
        int   data4[8]; /* int, not char coz sscanf may overrun array */

        /* COUNT_GUID_SCANF_FIELDS = 11 */
        /* COUNT_GUID_STRING_LEN = 36 */

        if (strlen(buf) == 36 &&
            sscanf(buf, "%8x-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x",
                   &yylval.guid.Data1, &yylval.guid.Data2,
                   &yylval.guid.Data3, &data4[0], &data4[1],
                   &data4[2], &data4[3], &data4[4], &data4[5],
                   &data4[6], &data4[7]) == 11) {
            yylval.guid.Data4[0] = (BYTE)data4[0];
            yylval.guid.Data4[1] = (BYTE)data4[1];
            yylval.guid.Data4[2] = (BYTE)data4[2];
            yylval.guid.Data4[3] = (BYTE)data4[3];
            yylval.guid.Data4[4] = (BYTE)data4[4];
            yylval.guid.Data4[5] = (BYTE)data4[5];
            yylval.guid.Data4[6] = (BYTE)data4[6];
            yylval.guid.Data4[7] = (BYTE)data4[7];
            rtrn = YT_GUID;
#ifdef XOFDEBUG
        {
            int i;
            for (i = 0; i < sizeof scanGuid; i++)
                scanGuid[i] = buf[i];
        }
#endif /* XOFDEBUG */

        }
    }
    return rtrn;
}

int yyGetString()
{
    char ch;

    nInBuf = 0;
    while (((ch = yypStream->GetByte()) != EOF) && (ch != '"')) {
        if (ch == '\\') {
            if ((ch = yypStream->GetByte())!= EOF) {
                if (ch == 'n')      ch = '\n';
                else if ( ch == 't' )   ch = '\t';
                else if ( ch == 'v' )   ch = '\v';
                else if ( ch == 'b' )   ch = '\b';
                else if ( ch == 'r' )   ch = '\r';
                else if ( ch == 'f' )   ch = '\f';
                else if ( ch == 'e' )   ch = '\033';
                else if ( ch == '0' ) {
                    int tmp, stop;
                    ch = stop = 0;
                    if (((tmp = yypStream->GetByte()) != EOF) &&
                        (isdigit(tmp)) && (tmp!='8') && (tmp!='9')) {
                      ch = (ch * 8) + (tmp - '0');
                    } else {
                        stop = 1;
                        yypStream->UngetByte();
                    }
                    if ((!stop) && ((tmp=yypStream->GetByte())!=EOF) &&
                        (isdigit(tmp)) && (tmp!='8') && (tmp!='9')) {
                        ch= (ch*8)+(tmp-'0');
                    }
                    else {
                        stop= 1;
                        yypStream->UngetByte();
                    }
                    if ((!stop) && ((tmp=yypStream->GetByte())!=EOF) &&
                        (isdigit(tmp)) && (tmp!='8') && (tmp!='9')) {
                        ch= (ch*8)+(tmp-'0');
                    }
                    else {
                        stop= 1;
                        yypStream->UngetByte();
                    }
                }
            }
            else return YT_ERROR;
        }

        if ( nInBuf < BUFSIZE-1 )
            buf[nInBuf++] = ch;
    }
    if ( ch == '"' ) {
        buf[nInBuf++] = '\0';
        yylval.string = Strdup(buf);
#ifdef XOFDEBUG
        scanStr = yylval.string;
        scanStrLine = lineNum;
#endif
        return YT_STRING;
    }
    return YT_ERROR;
}

int
yyGetName(char first)
{
    char ch;
    int i,found;
    int rtrn;

    rtrn = YT_NAME;
    buf[0] = first; nInBuf = 1;
    while (((ch = yypStream->GetByte())!= EOF) &&
            (isalnum(ch) || (ch == '_') || (ch == '-'))) {
        if (nInBuf < BUFSIZE - 1)
            buf[nInBuf++] = ch;
    }
    buf[nInBuf++] = '\0';
    found= 0;

    for (i=0;(!found)&&(i<(int)numKeywords);i++) {
        if (_stricmp(buf,keywords[i].keyword)==0) {
            rtrn = keywords[i].token;
            found= 1;
        }
    }

    if (rtrn != YT_BINARY_RESOURCE)
        yylval.string = Strdup(buf);
#ifdef XOFDEBUG
    scanStr = yylval.string;
    scanStrLine = lineNum;
#endif

    if ((ch != EOF) && (!isspace(ch)))
        yypStream->UngetByte();
    else if (ch == '\n')
        lineNum++;

    return rtrn;
}

int yyGetNumber(char c)
{
    int rtrn = YT_INTEGER;      /* Assume */

    char* p = buf;
    if (c == '-' || c == '+') {
        *p++ = c;
        c = yypStream->GetByte();
    }
    while (c != EOF && isdigit(c)) {
        *p++ = c;
        c = yypStream->GetByte();
    }
    if (c == '.') {
        *p++ = c;
        c = yypStream->GetByte();
        while (c != EOF && isdigit(c)) {
            *p++ = c;
            c = yypStream->GetByte();
        }
        if (c == 'e' || c == 'E') {
            *p++ = c;
            c = yypStream->GetByte();
            if (c == '+' || c == '-') {
                *p++ = c;
                c = yypStream->GetByte();
            }
            while (c != EOF && isdigit(c)) {
                *p++ = c;
                c = yypStream->GetByte();
            }
        }
        rtrn = YT_REALNUM;
    }
    if (c != EOF) yypStream->UngetByte();
    *p = '\0';
    if (rtrn == YT_REALNUM) {
        yylval.stgFlt = (stgFlt)atof(buf);
#ifdef XOFDEBUG
        scanFlt = yylval.stgFlt;
#endif
    } else if (rtrn == YT_INTEGER) {
        yylval.stgInt = (stgInt)atol(buf);
#ifdef XOFDEBUG
        scanInt = yylval.stgInt;
#endif
    }

    return rtrn;
}

int yyGetBinaryData(XStreamRead *pStream)
{
    XBlobData* bd;
    CHAR *mimeType = NULL;
    DWORD i;
    DWORD cbMimeSize;
    DWORD dwOffset;
    DWORD cbSize, dummy; /* XXXXXX DWORD -> ULONGLONG */

    yylval.binaryData = NULL;   /* Assume error */

    /*
     * Read binary blob header. Looks like this:
     * 64-bit offset to real data in resource section
     * 64-bit size field
     * 16-bit length of following mime type string field
     * Mime type stored as ascii. Length specified by previous field
     */

    if (!pStream->ReadDword(&dwOffset))
        return YT_END_OF_FILE;

    /* XXXXXX Ignore high part of offset until ULONGLONG supported... */
    if (!pStream->ReadDword(&dummy))
        return YT_END_OF_FILE;

    if (!pStream->ReadDword(&cbSize))
        return YT_END_OF_FILE;

    /* XXXXXX Ignore high part of size until ULONGLONG supported... */
    if (!pStream->ReadDword(&dummy))
        return YT_END_OF_FILE;

    cbMimeSize = pStream->ReadWord();
    if (cbMimeSize == EOF)
        return YT_END_OF_FILE;

    if (cbMimeSize) {
        if (XMalloc((void **) &mimeType, (cbMimeSize + 1) * sizeof(WCHAR)) != DXFILE_OK)
            return YT_ERROR;

        /* Read the mime type string */
        for (i = 0; i < cbMimeSize; i++) {
            int byte;

            if ((byte = pStream->GetByte()) == EOF)
                return YT_END_OF_FILE;
            mimeType[i] = byte & 0xff;
        }
        mimeType[i] = '\0';
    }

    bd = new XBlobData(dwOffset, mimeType, cbSize);

    if (mimeType)
        XFree(mimeType);

    if (!bd)
        return YT_ERROR;

    yylval.binaryData = bd;

    return YT_BINARY_DATA;
}

int yylex()
{
    int token;

    if (flagError) {
        flagError = FALSE;
        return YT_ERROR;
    }

    if (flagYield) {
        flagEof = flagYield = FALSE;        /* Reset flags */
        return YT_YIELD;
    }

    if (yypStream->mode() == FM_Binary)
        token =  yylex_binary(yypStream);
    else if (yypStream->mode() == FM_Text)
        token = yylex_ascii(yypStream);
    else {
        DPF_ERR("Unknown file mode");
        token = YT_ERROR;
    }

#ifdef XOFDEBUG 
    //DPF(8,"scan: %s\n", tokText(token));
#endif

    if ( token == YT_BINARY_RESOURCE ) {
        flagEof = TRUE;
        token = YT_END_OF_FILE;
    }
    return token;
}

static int yylex_binary(XStreamRead *pStream)
{
    int   token;
    DWORD i, len;

    token = pStream->ReadWord();

    switch (token) {
    case YT_OBRACE:
    case YT_CBRACE:
    case YT_OPAREN:
    case YT_CPAREN:
    case YT_OBRACKET:
    case YT_CBRACKET:
    case YT_DOT:
    case YT_COMMA:
    case YT_SEMICOLON:
        break;

    case YT_TEMPLATE:
    case YT_WORD:
    case YT_DWORD:
    case YT_FLOAT:
    case YT_DOUBLE:
    case YT_CHAR:
    case YT_UCHAR:
    case YT_SWORD:
    case YT_SDWORD:
    case YT_LPSTR:
    case YT_UNICODE:
    case YT_CSTRING:
    case YT_ARRAY:
    case YT_ULONGLONG:
    case YT_BINARY:
        /* The parser expects to find the string in yylval.string... */
        for (i = 0; i < (int)numKeywords; i++) {
            if (token == keywords[i].token) { /* Should _always_ find a match */
                yylval.string = Strdup(keywords[i].keyword);
#ifdef XOFDEBUG
                scanStr = yylval.string;
#endif
                break;
            }
        }
        break;

    case YT_BINARY_RESOURCE:
        break;

    case YT_NAME:
    case YT_STRING:
    /* Names and strings are the same thing apart from the token... */
    {
        char *p;

        /* Length follows the token... */
        if (!pStream->ReadDword(&len)) {
            DPF_ERR("Unexpected EOF");
            return YT_ERROR;      /* Premature EOF */
        }

        for (p = buf, i = 0; i < len; i++) {
            char ch;

            if ((ch = pStream->GetByte()) == EOF) {
                DPF_ERR("Unexpected EOF");
                return YT_ERROR;        /* Premature EOF */
            }
            if (i < BUFSIZE - 1)  /* Don't overrun buffer */
                *p++ = ch;
        }
        *p = '\0';
        yylval.string = Strdup(buf);
#ifdef XOFDEBUG
        scanStr = yylval.string;
#endif
      }
      break;

    case YT_INTEGER_LIST:
    {
        stgInt *data;

        /* Length follows the token... */
        if (!pStream->ReadDword(&len)) {
            DPF_ERR("Unexpected EOF");
            return YT_ERROR;
        } else if (len == 0) {
            DPF_ERR("Zero-length integer list");
            return YT_ERROR;
        }
        if ((XMalloc((void **) &data, len * sizeof(stgInt))) != DXFILE_OK)
            return YT_ERROR;      /* Hmm, out of memory really */

        for(i = 0; i < len; i++) {
            DWORD val;

            if (!pStream->ReadDword(&val)) {
                DPF_ERR("Unexpected EOF");
                return YT_ERROR;
            }
            data[i] = (stgInt)val;
        }
        yylval.intListBinData.data = data;
        yylval.intListBinData.count = len;
#ifdef XOFDEBUG
        scanIntListLen = len;
#endif
    }
    break;

    case YT_REALNUM_LIST:
    {
        stgFlt *data;

        /* Length follows the token... */
        if (!pStream->ReadDword(&len)) {
            DPF_ERR("Unexpected EOF");
            return YT_ERROR;
        } else if (len == 0) {
            DPF_ERR("Zero-length float list");
            return YT_ERROR;
        }
        if ((XMalloc((void **) &data, len * sizeof(stgFlt))) != DXFILE_OK)
            return YT_ERROR;      /* Hmm, out of memory really */

        /*
         * The # of bits used to represent float values is stored in
         * the file header. Check it so we know how much to read now...
         */
        if (pStream->floatSize() == sizeof(FLOAT)) {
            for(i = 0; i < len; i++) {
                float val;

                if (!pStream->ReadFloat(&val)) {
                    DPF_ERR("Unexpected EOF");
                    return YT_ERROR;
                }
                data[i] = (stgFlt)val;
            }
        } else {
            for(i = 0; i < len; i++) {
                double val;

                if (!pStream->ReadDouble(&val)) {
                    DPF_ERR("Unexpected EOF");
                    return YT_ERROR;
                }
                data[i] = (stgFlt)val;
            }
        }
        yylval.fltListBinData.data = data;
        yylval.fltListBinData.count = len;
#ifdef XOFDEBUG
        scanFltListLen = len;
#endif
      }
      break;

    case YT_INTEGER:
    {
        DWORD val;

        if (!pStream->ReadDword(&val)) {
            DPF_ERR("Unexpected EOF");
            return YT_ERROR;
        }
        yylval.stgInt = (stgInt)val;
#ifdef XOFDEBUG
        scanInt = yylval.stgInt;
#endif
    }
    break;

    case YT_REALNUM:
        if (pStream->floatSize() == sizeof(FLOAT)) {
            float val;

            if (!pStream->ReadFloat(&val)) {
                DPF_ERR("Unexpected EOF");
                return YT_ERROR;
            }
            yylval.stgFlt = (stgFlt)val;
#ifdef XOFDEBUG
            scanFlt = yylval.stgFlt;
#endif
        } else {
            double val;

            if (!pStream->ReadDouble(&val)) {
                DPF_ERR("Unexpected EOF");
                return YT_ERROR;
            }
            yylval.stgFlt = (stgFlt)val;
#ifdef XOFDEBUG
            scanFlt = yylval.stgFlt;
#endif
        }
        break;

    case YT_GUID:
        pStream->ReadDword(&(yylval.guid.Data1));
        yylval.guid.Data2 = (unsigned short)pStream->ReadWord();
        yylval.guid.Data3 = (unsigned short)pStream->ReadWord();
        for (i = 0; i < sizeof yylval.guid.Data4; i++) {
            int ch;

            if ((ch = pStream->GetByte()) == EOF) {
                DPF_ERR("Unexpected EOF");
                token = YT_ERROR;
                break;
            } else {
                yylval.guid.Data4[i] = (unsigned char)ch;
            }
        }
        break;

    case YT_BINARY_DATA:
        token = yyGetBinaryData(pStream);
        if (token == YT_END_OF_FILE)
            flagEof = TRUE;
        break;

    case EOF:
        flagEof = TRUE;
        token = YT_END_OF_FILE;
        break;

    default:
        DPF_ERR("Unexpected data in input stream");
        token = YT_ERROR;
    }

    return token;
}

static int yylex_ascii(XStreamRead *pStream)
{
    register char ch;
    int rtrn;

    do {
        ch = pStream->GetByte();
        if (ch == '\n') {
            lineNum++;
        }
        else if (ch == '#') {
            do {
                ch = pStream->GetByte();
            } while ((ch != '\n') && (ch != EOF));
            lineNum++;
        }
        else if (ch == '/') {   /* handle C++ style double-/ comments */
            int newch = pStream->GetByte();
            if (newch == '/') {
                do {
                    ch= pStream->GetByte();
                } while ((ch != '\n') && (ch != EOF));
                lineNum++;
            }
            else if (newch != EOF) {
                pStream->UngetByte();
            }
        }
    } while ((ch != EOF) && (isspace(ch)));
    if      ( ch == '{' )       rtrn = YT_OBRACE;
    else if ( ch == '}' )       rtrn = YT_CBRACE;
    else if ( ch == '(' )       rtrn = YT_OPAREN;
    else if ( ch == ')' )       rtrn = YT_CPAREN;
    else if ( ch == '[' )       rtrn = YT_OBRACKET;
    else if ( ch == ']' )       rtrn = YT_CBRACKET;
    else if ( ch == '.' )       rtrn = YT_DOT;
    else if ( ch == ',' )       rtrn = YT_COMMA;
    else if ( ch == ';' )       rtrn = YT_SEMICOLON;
    else if ( ch == '"' )       rtrn = yyGetString();
    else if ( ch == '<' )       rtrn = yyGetUuid();
    else if ( (isalpha(ch) || (ch=='_')) )
        rtrn = yyGetName(ch);
    else if ( (isdigit(ch) || ch == '-' || ch == '+') )
        rtrn = yyGetNumber(ch);
    else if ( ch == YT_BINARY_DATA ) {
        ch = pStream->GetByte();
        if (ch != ((YT_BINARY_DATA >> 8) & 0xff)) {
            if (ch == EOF) {
                flagEof = TRUE;
                rtrn = YT_END_OF_FILE;
            }
            DPF_ERR("Unexpected character in input stream");
            return YT_ERROR;
        }
        rtrn = yyGetBinaryData(pStream);
        if (rtrn == YT_END_OF_FILE)
            flagEof = TRUE;
    }
    else if ( ch == EOF ) {
        flagEof = TRUE;
        rtrn = YT_END_OF_FILE;
    }
    else {
        DPF_ERR("Unexpected character in input stream");
        rtrn = YT_ERROR;
    }

    return rtrn;
}

yyerror(char* s)
{
#if 0
    if (*buf)
        DPF(0, "%d: %s near \"%s\"\n", lineNum, s, buf);
    else
        DPF(0, "%d: %s\n", lineNum, s);
#endif 0

    return 0;
}

extern int yyparse();

int YaccParseXOF(XStreamRead *pStream)
{
    yypStream = pStream;
    return yyparse();
}

/*
 * Parsed to end of file?
 */
int YaccIsEof(void)
{
    return flagEof;
}

void YaccResetEof(void)
{
    flagEof = FALSE;
}

XStreamRead *YaccGetStream()
{
    return yypStream;
}

void YaccSetStream(XStreamRead *pStream)
{
    yypStream = pStream;
}

void YaccSetError(void)
{
    flagError = TRUE;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;

    if (!XTemplateList::Init())
        goto yyabort;

#if YYDEBUG

    register char *yys;

#if 0
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#else
    yydebug = 9;
#endif

#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            DebugPrint("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            DebugPrint("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
yynewerror:
#endif
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
yyerrlab:
#endif
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    DebugPrint("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    DebugPrint("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            DebugPrint("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        DebugPrint("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 5:
{
                    XDataObj *data;

                    data = XDataObj::Create(yyvsp[-5].string, 
                                            yyvsp[-4].string, 
                                            yyvsp[-2].guid, 
                                            yyvsp[-1].xdatapartList);

                    if (!data) {
                        if (yyvsp[-1].xdatapartList) delete yyvsp[-1].xdatapartList;
                        DPF_ERR("parser: failed to create data object");
                        YaccSetError();
                    } else {
                        if (!yypStream->AddDataObject(data)) {
                            DPF_ERR("parser: failed to add object to list.");
                            YaccSetError();
                        }

                        /* Intrinsic header is special case. Set flags... */
                        const GUID *type;

                        if (SUCCEEDED(data->GetType(&type)) &&
                            *type == TID_DXFILEHeader) {
                            yypStream->SetIntrinsicHeaderInfo(data);
                        }
                    }

                    XFree(yyvsp[-5].string);
                    XFree(yyvsp[-4].string);

                    flagYield = TRUE;
                }
break;
case 6:
{
                    XTemplate *tmplate;

                    tmplate = new XTemplate(yyvsp[-4].string, yyvsp[-2].guid, yyvsp[-1].templInfo);

                    if (!tmplate) {
                        DPF_ERR("parser: failed to create template object");
                        YaccSetError();
                    }

                    XTemplateList::Add(tmplate);

                    XFree(yyvsp[-5].string);
                    XFree(yyvsp[-4].string);
                    delete yyvsp[-1].templInfo;
                }
break;
case 7:
{ yyval.xdatapartList = NULL; }
break;
case 8:
{
                        yyval.xdatapartList = new XDataPartList();

                        if (!yyval.xdatapartList) {
                            DPF_ERR("parser: failed to create data part list.");
                            YaccSetError();
                        } else if (yyvsp[0].xdatapart && !yyval.xdatapartList->Add(yyvsp[0].xdatapart)) {
                            DPF_ERR("parser: failed to add data part to list.");
                            YaccSetError();
                        }
                    }
break;
case 9:
{
                        yyval.xdatapartList = yyvsp[-1].xdatapartList;

                        if (yyvsp[0].xdatapart && !yyval.xdatapartList->Add(yyvsp[0].xdatapart)) {
                            DPF_ERR("parser: failed to add data part to list.");
                            YaccSetError();
                        }
                    }
break;
case 10:
{
                    yyval.xdatapart = yyvsp[0].xdataref;
                }
break;
case 11:
{
                    yyval.xdatapart = XDataObj::Create(yyvsp[-5].string, yyvsp[-4].string, yyvsp[-2].guid, yyvsp[-1].xdatapartList);
                    if (!yyval.xdatapart) {
                        if (yyvsp[-1].xdatapartList) delete yyvsp[-1].xdatapartList;
                        DPF_ERR("parser: failed to create data part object");
                        YaccSetError();
                    }
                    XFree(yyvsp[-5].string);
                    XFree(yyvsp[-4].string);
                }
break;
case 12:
{
                    yyval.xdatapart = yyvsp[0].intList;
                }
break;
case 13:
{
                    yyval.xdatapart = yyvsp[0].fltList;
                }
break;
case 14:
{
                    yyval.xdatapart = yyvsp[0].strList;
                }
break;
case 15:
{
                    yyval.xdatapart = new XBinaryData(yyvsp[-3].string, yyvsp[-2].guid, yyvsp[0].binaryData, yypStream);
                    if (!yyval.xdatapart) {
                        DPF_ERR("parser: failed to create binary object");
                        YaccSetError();
                    }
                    XFree(yyvsp[-4].string);
                    XFree(yyvsp[-3].string);
                    delete yyvsp[0].binaryData;
                }
break;
case 16:
{
                            yyval.templInfo = new XTemplateInfo(yyvsp[-3].tmList, yyvsp[-1].toList,
                                yyvsp[-1].toList? DXFILETEMPLATE_RESTRICTED : DXFILETEMPLATE_OPEN);

                            if (!yyval.templInfo) {
                                DPF_ERR("Failed to create XTemplateInfo.");
                                YaccSetError();
                            }
                        }
break;
case 17:
{
                            yyval.templInfo = new XTemplateInfo(yyvsp[0].tmList);
                            if (!yyval.templInfo) {
                                DPF_ERR("Failed to create XTemplateInfo");
                                YaccSetError();
                            }
                        }
break;
case 18:
{ yyval.tmList = NULL; }
break;
case 19:
{
                            yyval.tmList = yyvsp[0].tmList;
                        }
break;
case 20:
{
                            yyval.tmList = new XTemplateMemberList();
                            if (!yyval.tmList) {
                                DPF_ERR("Failed to create XTemplateMemberList.");
                                YaccSetError();
                            } else if (yyvsp[0].templMember && !yyval.tmList->Add(yyvsp[0].templMember)) {
                                DPF_ERR("Failed to add template member.");
                                YaccSetError();
                            }
                        }
break;
case 21:
{
                            yyval.tmList = yyvsp[-1].tmList;

                            if (yyvsp[0].templMember && !yyval.tmList->Add(yyvsp[0].templMember)) {
                                DPF_ERR("Failed to add template member.");
                                YaccSetError();
                            }
                        }
break;
case 22:
{ yyval.templMember = yyvsp[0].templMember; }
break;
case 23:
{ yyval.templMember = yyvsp[0].templMember; }
break;
case 24:
{ yyval.templMember = yyvsp[0].templMember; }
break;
case 25:
{
                            yyval.toList = NULL;
                       }
break;
case 26:
{
                            yyval.toList = yyvsp[0].toList;
                       }
break;
case 27:
{
                           yyval.toList = new XTemplateOptionList();
                           if (!yyval.toList) {
                               DPF_ERR("Failed to create XTemplateOptionList.");
                               YaccSetError();
                           } else if (!yyval.toList->Add(yyvsp[0].idInf)) {
                               DPF_ERR("Failed to add template option.");
                               YaccSetError();
                           }
                       }
break;
case 28:
{
                           yyval.toList = yyvsp[-1].toList;
                           if (!yyval.toList->Add(yyvsp[0].idInf)) {
                               DPF_ERR("Failed to add template option.");
                               YaccSetError();
                           }
                       }
break;
case 29:
{
                            yyval.idInf = XIdentifier::Create(yyvsp[-2].string, yyvsp[-1].guid);

                            if (!yyval.idInf) {
                                DPF_ERR("Bad template optional member.");
                                YaccSetError();
                            }

                            XFree(yyvsp[-2].string);
                       }
break;
case 30:
{
                            yyval.idInf = XIdentifier::Create(SZBINARY, GUID_NULL);

                            if (!yyval.idInf) {
                                DPF_ERR("Bad template optional member.");
                                YaccSetError();
                            }

                            XFree(yyvsp[-1].string);
                     }
break;
case 31:
{
                        yyval.templMember = XTemplateMember::Create(yyvsp[-3].string, yyvsp[-2].string, yyvsp[-1].dimList);
                        if (!yyval.templMember) {
                            DPF_ERR("parser: failed to create array object");

                            /* Backward compatibility fix:*/
                            /* XOF3 did not verify array_data_type but we do.*/
                            /* See bug 19233:*/
                            /* PolyTrans outputs bad template MeshVertexColors.*/
                            /* It uses indexColor instead of IndextedColor*/
                            /* as the type for vertexColors.*/

                            if (strcmp(yyvsp[-3].string, "indexColor"))
                                YaccSetError();
                            delete yyvsp[-1].dimList;
                        }

                        XFree(yyvsp[-4].string);
                        XFree(yyvsp[-3].string);
                        XFree(yyvsp[-2].string);
                    }
break;
case 32:
{ yyval.string = yyvsp[0].string; }
break;
case 33:
{ yyval.string = yyvsp[0].string; }
break;
case 34:
{ yyval.string = yyvsp[0].string; }
break;
case 35:
{
                        yyval.dimList = new XDimensionList();
                        if (!yyval.dimList) {
                            DPF_ERR("Failed to create XDimensionList.");
                            YaccSetError();
                        } else if (!yyval.dimList->Add(yyvsp[0].elementSize)) {
                            DPF_ERR("Failed to add dimension.");
                            YaccSetError();
                        }
                    }
break;
case 36:
{
                        yyval.dimList = yyvsp[-1].dimList;
                        if (!yyval.dimList->Add(yyvsp[0].elementSize)) {
                            DPF_ERR("Failed to add dimension.");
                            YaccSetError();
                        }
                    }
break;
case 37:
{ yyval.elementSize = yyvsp[-1].elementSize; }
break;
case 38:
{
                       yyval.elementSize = new XDimension(yyvsp[0].stgInt, NULL);

                       if (!yyval.elementSize) {
                           DPF_ERR("Failed to create XDimension");
                           YaccSetError();
                       }
                   }
break;
case 39:
{
                       yyval.elementSize = new XDimension(0, yyvsp[0].string);

                       if (!yyval.elementSize) {
                           DPF_ERR("Failed to create XDimension");
                           YaccSetError();
                       }

                       XFree(yyvsp[0].string);
                   }
break;
case 40:
{
                        yyval.templMember = XTemplateMember::Create(yyvsp[-2].string, yyvsp[-1].string);
                        if (!yyval.templMember) {
                            DPF_ERR("parser: failed to create primitive object");
                            YaccSetError();
                        }

                        XFree(yyvsp[-2].string);
                        XFree(yyvsp[-1].string);
                    }
break;
case 41:
{ yyval.string = yyvsp[0].string; }
break;
case 42:
{ yyval.string = yyvsp[0].string; }
break;
case 43:
{ yyval.string = yyvsp[0].string; }
break;
case 44:
{ yyval.string = yyvsp[0].string; }
break;
case 45:
{ yyval.string = yyvsp[0].string; }
break;
case 46:
{ yyval.string = yyvsp[0].string; }
break;
case 47:
{ yyval.string = yyvsp[0].string; }
break;
case 48:
{ yyval.string = yyvsp[0].string; }
break;
case 49:
{ yyval.string = yyvsp[0].string; }
break;
case 50:
{ yyval.string = yyvsp[0].string; }
break;
case 51:
{ yyval.string = yyvsp[0].string; }
break;
case 52:
{ yyval.string = yyvsp[0].string; }
break;
case 53:
{ yyval.string = yyvsp[0].string; }
break;
case 54:
{
                    if (!yyvsp[-3].string && yyvsp[-2].guid == GUID_NULL) {
                        DPF_ERR("Bad date reference, no name or uuid");
                        YaccSetError();
                    } else {
                        yyval.xdataref = new XDataRef(yyvsp[-3].string, yyvsp[-2].guid, yypStream);
                        if (!yyval.xdataref) {
                            DPF_ERR("Failed to create XDataRef");
                            YaccSetError();
                        }
                        XFree(yyvsp[-3].string);
                    }
                }
break;
case 55:
{
                    yyval.templMember = XTemplateMember::Create(yyvsp[-2].string, yyvsp[-1].string);
                    if (!yyval.templMember) {
                        DPF_ERR("parser: failed to create template reference object");
                        YaccSetError();
                    }

                    XFree(yyvsp[-2].string);
                    XFree(yyvsp[-1].string);
                }
break;
case 56:
{ yyval.string = yyvsp[0].string; }
break;
case 57:
{ yyval.string = yyvsp[0].string; }
break;
case 58:
{ yyval.string = yyvsp[0].string; }
break;
case 59:
{ yyval.string = NULL; }
break;
case 60:
{ yyval.string = yyvsp[0].string; }
break;
case 61:
{ yyval.string = yyvsp[0].string; }
break;
case 62:
{ yyval.string = yyvsp[0].string; }
break;
case 65:
{
                        yyval.intList = new integerList(yyvsp[0].intListBinData);
                        if (!yyval.intList) {
                            DPF_ERR("Failed to create integerList.");
                            YaccSetError();
                        }
                    }
break;
case 66:
{
                        yyval.intList = new integerList();
                        if (!yyval.intList) {
                            DPF_ERR("Failed to create integerList.");
                            YaccSetError();
                        } else if (!yyval.intList->Add(yyvsp[0].stgInt)) {
                            DPF_ERR("Failed to add integer.");
                            YaccSetError();
                        }
                    }
break;
case 67:
{
                        yyval.intList = yyvsp[-2].intList;
                        if (!yyval.intList->Add(yyvsp[0].stgInt)) {
                            DPF_ERR("Failed to add integer.");
                            YaccSetError();
                        }
                    }
break;
case 70:
{
                        yyval.fltList = new floatList(yyvsp[0].fltListBinData);
                        if (!yyval.fltList) {
                            DPF_ERR("Failed to create floatList.");
                            YaccSetError();
                        }
                    }
break;
case 71:
{
                        yyval.fltList = new floatList();

                        if (!yyval.fltList) {
                            DPF_ERR("Failed to create floatList.");
                            YaccSetError();
                        } else if (!yyval.fltList->Add(yyvsp[0].stgFlt)) {
                            DPF_ERR("Failed to add float.");
                            YaccSetError();
                        }
                    }
break;
case 72:
{
                        yyval.fltList = yyvsp[-2].fltList;
                        if (!yyval.fltList->Add(yyvsp[0].stgFlt)) {
                            DPF_ERR("Failed to add float.");
                            YaccSetError();
                        }
                    }
break;
case 74:
{
                        yyval.strList = new stringList();
                        if (!yyval.strList) {
                            DPF_ERR("Failed to create stringList.");
                            YaccSetError();
                        } else if (!yyval.strList->Add(yyvsp[0].string)) {
                            DPF_ERR("Failed to add string.");
                            YaccSetError();
                        }
                    }
break;
case 75:
{
                        yyval.strList = yyvsp[-2].strList;
                        if (!yyval.strList->Add(yyvsp[0].string)) {
                            DPF_ERR("Failed to add string.");
                            YaccSetError();
                        }
                    }
break;
case 76:
{ yyval.guid = GUID_NULL; }
break;
case 77:
{ yyval.guid = yyvsp[0].guid; }
break;
case 78:
{ yyval.guid = yyvsp[0].guid; }
break;
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            DebugPrint("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                DebugPrint("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        DebugPrint("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = (short)yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    XTemplateList::Free();
    return (1);
yyaccept:
    XTemplateList::Free();
    return (0);
}
