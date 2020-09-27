
%{

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

static int yylex_ascii(XStreamRead *pStream);
static int yylex_binary(XStreamRead *pStream);

%}


%token
        YT_END_OF_FILE      0
        YT_YIELD            0   /* Yield control to caller */
        YT_ERROR            255

/* Internals... */
        YT_NAME             1
        YT_STRING           2
        YT_INTEGER          3
        YT_REALNUM          4
        YT_GUID             5
        YT_INTEGER_LIST     6   /* Binary format only */
        YT_REALNUM_LIST     7   /* Binary format only */
        YT_BINARY_DATA      254 /* Must be outside ASCII range */

/* Operators... */
        YT_OBRACE           10
        YT_CBRACE           11
        YT_OPAREN           12
        YT_CPAREN           13
        YT_OBRACKET         14
        YT_CBRACKET         15
        YT_OANGLE           16
        YT_CANGLE           17
        YT_DOT              18
        YT_COMMA            19
        YT_SEMICOLON        20

/* Intrinsics... */
        YT_OBJECT           30
        YT_TEMPLATE         31

/* Primitives (for templates)... */
        YT_WORD             40
        YT_DWORD            41
        YT_FLOAT            42
        YT_DOUBLE           43
        YT_CHAR             44
        YT_UCHAR            45
        YT_SWORD            46
        YT_SDWORD           47
        YT_VOID             48 /* C-style void */
        YT_LPSTR            49 /* Null terminated string */
        YT_UNICODE          50 /* Unicode string */
        YT_CSTRING          51 /* C string */
        YT_ARRAY            52 /* Array */
        YT_ULONGLONG                53
        YT_BINARY           54 /* Binary object */
        YT_BINARY_RESOURCE      55/* Start of binary resource
                           * section.  Stop parsing */

%union {
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
}

%type <string> YT_STRING YT_NAME name
%type <string> string opt_name identifier
%type <string> primitive_type array_data_type
%type <string> array_name template_name YT_UNICODE
%type <string> YT_WORD YT_DWORD YT_FLOAT YT_DOUBLE YT_CHAR YT_UCHAR
%type <string> YT_SWORD YT_SDWORD YT_VOID YT_LPSTR YT_CSTRING
%type <string> YT_TEMPLATE YT_ARRAY YT_ULONGLONG YT_BINARY
%type <stgInt> YT_INTEGER
%type <intListBinData> YT_INTEGER_LIST
%type <stgFlt> YT_REALNUM
%type <fltListBinData> YT_REALNUM_LIST
%type <intList> number_list number_list_1 number_list_2
%type <fltList> float_list float_list_1 float_list_2
%type <strList> string_list string_list_1
%type <xdataref> data_reference
%type <xobject> object
%type <xdatapart> data_part
%type <xdatapartList> data_parts_list
%type <templMember> template_members primitive array template_reference
%type <tmList> template_members_list template_members_part
%type <elementSize> dimension dimension_size
%type <dimList> dimension_list
%type <guid> clsid opt_clsid YT_GUID
%type <toList> template_option_list template_option_info
%type <idInf> template_option_part
%type <templInfo> template_parts
%type <binaryData> YT_BINARY_DATA
%%

file        :   /* Empty */
            |   object_list
            ;

object_list :   object
            |   object_list object
            ;

object      :   identifier opt_name YT_OBRACE
                                      opt_clsid
                                      data_parts_list
                                    YT_CBRACE
                {
                    XDataObj *data;

                    data = XDataObj::Create($1, $2, $4, $5);

                    if (!data) {
                        if ($5) delete $5;
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

                    XFree($1);
                    XFree($2);

                    flagYield = TRUE;
                }
            |   YT_TEMPLATE template_name YT_OBRACE
                                            clsid
                                            template_parts
                                          YT_CBRACE
                {
                    XTemplate *tmplate;

                    tmplate = new XTemplate($2, $4, $5);

                    if (!tmplate) {
                        DPF_ERR("parser: failed to create template object");
                        YaccSetError();
                    }

                    XTemplateList::Add(tmplate);

                    XFree($1);
                    XFree($2);
                    delete $5;
                }

data_parts_list :   /* Empty */ { $$ = NULL; }
                |   data_part
                    {
                        $$ = new XDataPartList();

                        if (!$$) {
                            DPF_ERR("parser: failed to create data part list.");
                            YaccSetError();
                        } else if ($1 && !$$->Add($1)) {
                            DPF_ERR("parser: failed to add data part to list.");
                            YaccSetError();
                        }
                    }
                |   data_parts_list data_part
                    {
                        $$ = $1;

                        if ($2 && !$$->Add($2)) {
                            DPF_ERR("parser: failed to add data part to list.");
                            YaccSetError();
                        }
                    }
                ;

data_part   :   data_reference
                {
                    $$ = $1;
                }
            |   identifier opt_name YT_OBRACE
                                      opt_clsid
                                      data_parts_list
                                    YT_CBRACE
                {
                    $$ = XDataObj::Create($1, $2, $4, $5);
                    if (!$$) {
                        if ($5) delete $5;
                        DPF_ERR("parser: failed to create data part object");
                        YaccSetError();
                    }
                    XFree($1);
                    XFree($2);
                }
            |   number_list
                {
                    $$ = $1;
                }
            |   float_list
                {
                    $$ = $1;
                }
            |   string_list
                {
                    $$ = $1;
                }
            |   YT_OBRACE YT_BINARY opt_name opt_clsid YT_CBRACE YT_BINARY_DATA
                {
                    $$ = new XBinaryData($3, $4, $6, yypStream);
                    if (!$$) {
                        DPF_ERR("parser: failed to create binary object");
                        YaccSetError();
                    }
                    XFree($2);
                    XFree($3);
                    delete $6;
                }
            ;

template_parts        : template_members_part
                        YT_OBRACKET template_option_info YT_CBRACKET
                        {
                            $$ = new XTemplateInfo($1, $3,
                                $3? DXFILETEMPLATE_RESTRICTED : DXFILETEMPLATE_OPEN);

                            if (!$$) {
                                DPF_ERR("Failed to create XTemplateInfo.");
                                YaccSetError();
                            }
                        }
                      | template_members_list
                        {
                            $$ = new XTemplateInfo($1);
                            if (!$$) {
                                DPF_ERR("Failed to create XTemplateInfo");
                                YaccSetError();
                            }
                        }
                      ;

template_members_part : /* Empty */ { $$ = NULL; }
                      | template_members_list
                        {
                            $$ = $1;
                        }
                      ;

template_members_list : template_members
                        {
                            $$ = new XTemplateMemberList();
                            if (!$$) {
                                DPF_ERR("Failed to create XTemplateMemberList.");
                                YaccSetError();
                            } else if ($1 && !$$->Add($1)) {
                                DPF_ERR("Failed to add template member.");
                                YaccSetError();
                            }
                        }
                    |   template_members_list template_members
                        {
                            $$ = $1;

                            if ($2 && !$$->Add($2)) {
                                DPF_ERR("Failed to add template member.");
                                YaccSetError();
                            }
                        }
                    ;

template_members    :   primitive { $$ = $1; }
                    |   array { $$ = $1; }
                    |   template_reference { $$ = $1; }
                    ;

template_option_info : YT_DOT YT_DOT YT_DOT
                       {
                            $$ = NULL;
                       }
                     | template_option_list
                       {
                            $$ = $1;
                       }
                     ;

template_option_list : template_option_part
                       {
                           $$ = new XTemplateOptionList();
                           if (!$$) {
                               DPF_ERR("Failed to create XTemplateOptionList.");
                               YaccSetError();
                           } else if (!$$->Add($1)) {
                               DPF_ERR("Failed to add template option.");
                               YaccSetError();
                           }
                       }
                     | template_option_list template_option_part
                       {
                           $$ = $1;
                           if (!$$->Add($2)) {
                               DPF_ERR("Failed to add template option.");
                               YaccSetError();
                           }
                       }
                     ;

template_option_part : name opt_clsid opt_comma
                       {
                            $$ = XIdentifier::Create($1, $2);

                            if (!$$) {
                                DPF_ERR("Bad template optional member.");
                                YaccSetError();
                            }

                            XFree($1);
                       }
                     | YT_BINARY opt_comma
                     {
                            $$ = XIdentifier::Create(SZBINARY, GUID_NULL);

                            if (!$$) {
                                DPF_ERR("Bad template optional member.");
                                YaccSetError();
                            }

                            XFree($1);
                     }
                     ;

array           :   YT_ARRAY array_data_type array_name dimension_list
                    YT_SEMICOLON
                    {
                        $$ = XTemplateMember::Create($2, $3, $4);
                        if (!$$) {
                            DPF_ERR("parser: failed to create array object");

                            // Backward compatibility fix:
                            // XOF3 did not verify array_data_type but we do.
                            // See bug 19233:
                            // PolyTrans outputs bad template MeshVertexColors.
                            // It uses indexColor instead of IndextedColor
                            // as the type for vertexColors.

                            if (xstrcmp($2, "indexColor"))
                                YaccSetError();
                            delete $4;
                        }

                        XFree($1);
                        XFree($2);
                        XFree($3);
                    }
                ;

array_data_type  :  primitive_type { $$ = $1; }
                 |  name { $$ = $1; }
                 ;

array_name       :  name { $$ = $1; }
                 ;

dimension_list   :  dimension
                    {
                        $$ = new XDimensionList();
                        if (!$$) {
                            DPF_ERR("Failed to create XDimensionList.");
                            YaccSetError();
                        } else if (!$$->Add($1)) {
                            DPF_ERR("Failed to add dimension.");
                            YaccSetError();
                        }
                    }
                 |  dimension_list dimension
                    {
                        $$ = $1;
                        if (!$$->Add($2)) {
                            DPF_ERR("Failed to add dimension.");
                            YaccSetError();
                        }
                    }
                 ;

dimension        : YT_OBRACKET dimension_size YT_CBRACKET { $$ = $2; }

dimension_size   : YT_INTEGER
                   {
                       $$ = new XDimension($1, NULL);

                       if (!$$) {
                           DPF_ERR("Failed to create XDimension");
                           YaccSetError();
                       }
                   }
                 | name
                   {
                       $$ = new XDimension(0, $1);

                       if (!$$) {
                           DPF_ERR("Failed to create XDimension");
                           YaccSetError();
                       }

                       XFree($1);
                   }
                 ;

primitive        :  primitive_type opt_name YT_SEMICOLON
                    {
                        $$ = XTemplateMember::Create($1, $2);
                        if (!$$) {
                            DPF_ERR("parser: failed to create primitive object");
                            YaccSetError();
                        }

                        XFree($1);
                        XFree($2);
                    }
                 ;

primitive_type   :  YT_WORD      { $$ = $1; }
                 |  YT_DWORD     { $$ = $1; }
                 |  YT_FLOAT     { $$ = $1; }
                 |  YT_DOUBLE    { $$ = $1; }
                 |  YT_CHAR      { $$ = $1; }
                 |  YT_UCHAR     { $$ = $1; }
                 |  YT_SWORD     { $$ = $1; }
                 |  YT_SDWORD    { $$ = $1; }
                 |  YT_LPSTR     { $$ = $1; } /* Null terminated string */
                 |  YT_UNICODE   { $$ = $1; } /* Unicode */
                 |  YT_CSTRING   { $$ = $1; } /* C string */
                 |  YT_ULONGLONG { $$ = $1; } /* 64-bit uint */
                 |  YT_BINARY    { $$ = $1; } /* Binary data object */
                 ;

data_reference   :  YT_OBRACE opt_name opt_clsid opt_comma YT_CBRACE
                {
                    if (!$2 && $3 == GUID_NULL) {
                        DPF_ERR("Bad date reference, no name or uuid");
                        YaccSetError();
                    } else {
                        $$ = new XDataRef($2, $3, yypStream);
                        if (!$$) {
                            DPF_ERR("Failed to create XDataRef");
                            YaccSetError();
                        }
                        XFree($2);
                    }
                }
                ;

template_reference  :   name opt_name YT_SEMICOLON
                {
                    $$ = XTemplateMember::Create($1, $2);
                    if (!$$) {
                        DPF_ERR("parser: failed to create template reference object");
                        YaccSetError();
                    }

                    XFree($1);
                    XFree($2);
                }
                ;

identifier  :   name { $$ = $1; }
            |   primitive_type { $$ = $1; }
            ;

template_name    :  name { $$ = $1; }

opt_name    :   /* Empty */ { $$ = NULL; }
            |   name { $$ = $1; }
            ;

name        :   YT_NAME { $$ = $1; }
            ;

string      :   YT_STRING { $$ = $1; }
            ;

number_list     :   number_list_1 list_separator
                |   number_list_2
                ;

number_list_2   :   YT_INTEGER_LIST /* Binary integer handler */
                    {
                        $$ = new integerList($1);
                        if (!$$) {
                            DPF_ERR("Failed to create integerList.");
                            YaccSetError();
                        }
                    }

number_list_1   :   YT_INTEGER  /* Text integer handler */
                    {
                        $$ = new integerList();
                        if (!$$) {
                            DPF_ERR("Failed to create integerList.");
                            YaccSetError();
                        } else if (!$$->Add($1)) {
                            DPF_ERR("Failed to add integer.");
                            YaccSetError();
                        }
                    }
                |   number_list_1 list_separator YT_INTEGER
                    {
                        $$ = $1;
                        if (!$$->Add($3)) {
                            DPF_ERR("Failed to add integer.");
                            YaccSetError();
                        }
                    }
                ;

float_list      :   float_list_1 list_separator
                |   float_list_2
                ;

float_list_2    :   YT_REALNUM_LIST /* Binary float handler */
                    {
                        $$ = new floatList($1);
                        if (!$$) {
                            DPF_ERR("Failed to create floatList.");
                            YaccSetError();
                        }
                    }

float_list_1    :   YT_REALNUM  /* Text float handler */
                    {
                        $$ = new floatList();

                        if (!$$) {
                            DPF_ERR("Failed to create floatList.");
                            YaccSetError();
                        } else if (!$$->Add($1)) {
                            DPF_ERR("Failed to add float.");
                            YaccSetError();
                        }
                    }
                |   float_list_1 list_separator YT_REALNUM
                    {
                        $$ = $1;
                        if (!$$->Add($3)) {
                            DPF_ERR("Failed to add float.");
                            YaccSetError();
                        }
                    }
                ;

string_list     :   string_list_1 list_separator
                ;

string_list_1   :   string
                    {
                        $$ = new stringList();
                        if (!$$) {
                            DPF_ERR("Failed to create stringList.");
                            YaccSetError();
                        } else if (!$$->Add($1)) {
                            DPF_ERR("Failed to add string.");
                            YaccSetError();
                        }
                    }
                |   string_list_1 list_separator string
                    {
                        $$ = $1;
                        if (!$$->Add($3)) {
                            DPF_ERR("Failed to add string.");
                            YaccSetError();
                        }
                    }
                ;

opt_clsid       :   /* Empty */ { $$ = GUID_NULL; }
                |   clsid { $$ = $1; }
                ;

clsid           :  YT_GUID { $$ = $1; }

list_separator  :  YT_COMMA
                |  semicolon_list opt_comma

semicolon_list  :  YT_SEMICOLON
                |  semicolon_list YT_SEMICOLON

opt_comma       :  /* Empty */
                |  comma
                ;

comma           :  YT_COMMA
                ;
%%

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
    int ch;
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

        if (xstrlen(buf) == 36 &&
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
    int ch;

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
        yylval.string = xstrdup(buf);
#ifdef XOFDEBUG
        scanStr = yylval.string;
        scanStrLine = lineNum;
#endif
        return YT_STRING;
    }
    return YT_ERROR;
}

int
yyGetName(int first)
{
    int ch,i,found;
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
        if (xstricmp(buf,keywords[i].keyword)==0) {
            rtrn = keywords[i].token;
            found= 1;
        }
    }

    if (rtrn != YT_BINARY_RESOURCE)
        yylval.string = xstrdup(buf);
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

int yyGetNumber(int c)
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

#undef DPF_MODNAME
#define DPF_MODNAME "xparse::yyGetBinaryData"

int yyGetBinaryData(XStreamRead *pStream)
{
    XBlobData* bd;
    char *mimeType = NULL;
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
        if (XMalloc((void **) &mimeType, cbMimeSize + 1) != DXFILE_OK)
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

#undef DPF_MODNAME
#define DPF_MODNAME "xparse::yylex"

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
    DPF(8,"scan: %s\n", tokText(token));
#endif

    if ( token == YT_BINARY_RESOURCE ) {
        flagEof = TRUE;
        token = YT_END_OF_FILE;
    }
    return token;
}

#undef DPF_MODNAME
#define DPF_MODNAME "xparse::yylex_binary"

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
                yylval.string = xstrdup(keywords[i].keyword);
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
            int ch;

            if ((ch = pStream->GetByte()) == EOF) {
                DPF_ERR("Unexpected EOF");
                return YT_ERROR;        /* Premature EOF */
            }
            if (i < BUFSIZE - 1)  /* Don't overrun buffer */
                *p++ = ch;
        }
        *p = '\0';
        yylval.string = xstrdup(buf);
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
        yylval.guid.Data2 = pStream->ReadWord();
        yylval.guid.Data3 = pStream->ReadWord();
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
        DPF(0, "Unexpected data %d in input stream\n", token);
        token = YT_ERROR;
    }

    return token;
}

#undef DPF_MODNAME
#define DPF_MODNAME "xparse::yylex_ascii"

static int yylex_ascii(XStreamRead *pStream)
{
    register int ch;
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
            DPF(1,"Unexpected character 0x%x (%d) in input stream\n", ch, ch);
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
        DPF(1,"Unexpected character %c (%d) in input stream\n", ch, ch);
        rtrn = YT_ERROR;
    }

    return rtrn;
}

yyerror(char* s)
{
    if (*buf)
        DPF(0, "%d: %s near \"%s\"\n", lineNum, s, buf);
    else
        DPF(0, "%d: %s\n", lineNum, s);
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

