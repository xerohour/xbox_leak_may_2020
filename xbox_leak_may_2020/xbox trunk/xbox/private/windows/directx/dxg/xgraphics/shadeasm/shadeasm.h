#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse d3dxasm_parse
#define yylex d3dxasm_lex
#define yyerror d3dxasm_error
#define yychar d3dxasm_char
#define yyval d3dxasm_val
#define yylval d3dxasm_lval
#define yydebug d3dxasm_debug
#define yynerrs d3dxasm_nerrs
#define yyerrflag d3dxasm_errflag
#define yyss d3dxasm_ss
#define yyssp d3dxasm_ssp
#define yyvs d3dxasm_vs
#define yyvsp d3dxasm_vsp
#define yylhs d3dxasm_lhs
#define yylen d3dxasm_len
#define yydefred d3dxasm_defred
#define yydgoto d3dxasm_dgoto
#define yysindex d3dxasm_sindex
#define yyrindex d3dxasm_rindex
#define yygindex d3dxasm_gindex
#define yytable d3dxasm_table
#define yycheck d3dxasm_check
#define yyname d3dxasm_name
#define yyrule d3dxasm_rule
#define YYPREFIX "d3dxasm_"

#define isatty _isatty
#define fileno _fileno

#define T_VS 257
#define T_XVS 258
#define T_XVSW 259
#define T_XVSS 260
#define T_PS 261
#define T_XPS 262
#define T_DEF 263
#define T_OP0 264
#define T_OP1 265
#define T_OP2 266
#define T_OP3 267
#define T_OP4 268
#define T_OP5 269
#define T_OP6 270
#define T_OP7 271
#define T_ID 272
#define T_NUM 273
#define T_FLT 274
#define T_EOF 275
#define YYERRCODE 256
short d3dxasm_lhs[] = {                                        -1,
    0,    0,    1,    1,    1,    1,    1,    1,    2,    2,
    5,    5,    6,    4,    4,    9,    9,    9,    9,    9,
    9,    9,    9,   12,   12,   14,   14,   14,   21,   21,
    7,    7,    7,   22,   22,   22,   22,   22,   22,   22,
   22,   22,   22,   22,   22,   10,   10,   11,   11,   13,
   13,   15,   15,   16,   16,   17,   17,   18,   18,   19,
   19,    8,    8,    8,    8,    8,    8,   20,    3,   23,
};
short d3dxasm_len[] = {                                         2,
    2,    3,    5,    5,    5,    5,    5,    5,    1,    2,
    1,    2,   10,    1,    2,    1,    2,    4,    6,    8,
   14,   12,   14,    1,    3,    1,    2,    3,    1,    3,
    1,    4,    6,    5,    5,    3,    1,    2,    1,    3,
    4,    3,    3,    4,    3,    1,    2,    1,    2,    1,
    2,    1,    2,    1,    2,    1,    2,    1,    2,    1,
    2,    1,    2,    2,    1,    2,    2,    1,    1,    1,
};
short d3dxasm_defred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   46,   48,   50,   52,   54,
   56,   58,   60,    1,    0,    0,    9,    0,    0,    0,
   16,    0,    0,    0,    0,    0,    0,    0,   69,    0,
    0,    0,    0,    0,    0,   68,    0,    0,   47,   49,
   51,   53,   55,   57,   59,   61,    2,   10,   12,   15,
    0,   17,    0,    0,    0,    0,    0,    0,    0,   26,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   27,    0,    0,    0,    0,    0,
    3,    6,    7,    8,    4,    5,   70,    0,    0,   62,
    0,   65,    0,    0,    0,    0,    0,   25,   18,    0,
    0,   28,   30,    0,    0,    0,   64,   67,   63,   66,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   45,   42,   43,
   40,    0,   19,    0,    0,    0,    0,    0,   44,   41,
    0,    0,   33,    0,    0,    0,    0,    0,   35,   34,
   20,    0,    0,    0,    0,    0,    0,    0,   13,    0,
    0,    0,    0,    0,    0,    0,   22,    0,    0,    0,
   21,   23,
};
short d3dxasm_dgoto[] = {                                       7,
    8,   26,   67,   27,   28,   29,   68,  101,   30,   31,
   32,   62,   33,   69,   34,   35,   36,   37,   38,   48,
   70,  107,  102,
};
short d3dxasm_sindex[] = {                                   -165,
  -37,   14,   28,   52,   76,   79,    0,    1, -227, -227,
 -227, -227, -227, -227, -224,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -150, -202,    0,   27, -183,   27,
    0, -224, -224, -224, -224,  -13, -224, -224,    0,   82,
   83,   85,   86,   91,   92,    0,   37,   48,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   94,    0,   97,   98,   99, -224,  101,  102,  103,    0,
  105,  110, -227, -227, -227, -227, -227, -227,   16,  -11,
 -224,  -13,  -13,  -13,    0, -224, -224,  -13, -224, -224,
    0,    0,    0,    0,    0,    0,    0, -223, -223,    0,
  111,    0, -227,  -11,  -10,   -6,   63,    0,    0,  114,
  115,    0,    0,  118,  119,  120,    0,    0,    0,    0,
   16,   -2,  124,  -11,  -11,  -11,  -11,   71,  -13,  -13,
  -13,  -13, -224,  125,  -11,  -11,    2,    0,    0,    0,
    0, -224,    0,  126,  127,  128,  129,   16,    0,    0,
  -11,  -11,    0,  -13,  -13,  -13,  -13,  135,    0,    0,
    0,  136,  137,  138,   16,  -13,  -13,  -13,    0,  139,
  140,  141,  -13,  -13,  -13,  142,    0,  146,  -13,  -13,
    0,    0,
};
short d3dxasm_rindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   35, -107,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -41,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   13,    0,    0,    0,    0,    0,    0,  -17,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -40,  -28,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -16,    0,    0,    0,    0,    0,  -29,    0,    0,
    0,    0,    0,    0,    0,    0,   -3,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,
};
short d3dxasm_gindex[] = {                                      0,
    0,    0,    9,   12,  162,    0,   34,  -90,    0,    0,
    0,  -27,    0,   21,    0,    0,    0,    0,    0,  -51,
  241,  -25,  -44,
};
#define YYTABLESIZE 393
short d3dxasm_table[] = {                                      31,
   39,   31,   31,   31,   31,   63,   64,   65,    9,   71,
   72,   32,   37,   32,   32,   32,   32,   40,   41,   42,
   43,   44,   45,   29,   38,   29,   29,   29,  104,  108,
  134,   66,  125,  103,  124,  113,  127,   36,  126,   58,
  136,   60,  135,   25,  152,   39,  151,   46,   47,   39,
   97,   31,   39,  118,  120,   24,   24,  158,   99,   10,
   98,  115,  116,   32,   37,   61,   61,   61,   61,   25,
   61,   61,   57,   11,  169,   29,   38,   11,  123,   15,
   79,   91,   92,   93,   94,   95,   96,  100,  105,   36,
  153,    1,    2,    3,    4,    5,    6,   12,  138,  139,
  140,  141,  109,  110,  111,  147,  117,  119,  114,  149,
  150,  122,  105,   49,   50,   51,   52,   53,   54,   55,
   56,   13,   61,   61,   14,  159,  160,   73,   74,  100,
   75,   76,  105,  105,  105,  105,   77,   78,   80,   81,
   82,   83,   84,  105,  105,   86,   88,   87,   89,  143,
  144,  145,  146,   90,  121,  128,  100,  129,  130,  105,
  105,  131,  132,  133,  137,  142,   61,   14,  148,  154,
  155,  156,  157,  100,  161,  162,  163,  164,  165,  166,
  167,  168,  173,  174,  175,  179,  170,  171,  172,  180,
   59,    0,    0,  176,  177,  178,    0,    0,    0,  181,
  182,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   31,   31,   31,   31,   31,   31,   31,   31,
    0,    0,    0,   31,   32,   32,   32,   32,   32,   32,
   32,   32,    0,    0,    0,   32,   29,   29,   29,   29,
   29,   29,   29,   29,    0,    0,    0,   29,   46,   39,
   46,   39,    0,   15,   16,   17,   18,   19,   20,   21,
   22,   23,    0,    0,    0,   24,   24,   24,   24,   24,
   24,   24,   24,   24,    0,    0,    0,   24,   39,   97,
   16,   17,   18,   19,   20,   21,   22,   23,   11,   11,
   11,   11,   11,   11,   11,   11,   85,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  106,    0,    0,    0,    0,    0,  112,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  106,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  106,  106,  106,  106,    0,    0,
    0,    0,    0,    0,    0,  106,  106,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  106,  106,
};
short d3dxasm_check[] = {                                      41,
   41,   43,   44,   45,   46,   33,   34,   35,   46,   37,
   38,   41,   41,   43,   44,   45,   46,    9,   10,   11,
   12,   13,   14,   41,   41,   43,   44,   45,   40,   81,
  121,   45,   43,   45,   45,   87,   43,   41,   45,   28,
   43,   30,   45,   43,   43,  273,   45,  272,   15,  273,
  274,   93,   93,   98,   99,   43,   44,  148,   43,   46,
   45,   89,   90,   93,   93,   32,   33,   34,   35,   43,
   37,   38,  275,   46,  165,   93,   93,   43,  104,  263,
   44,   73,   74,   75,   76,   77,   78,   79,   80,   93,
  142,  257,  258,  259,  260,  261,  262,   46,  124,  125,
  126,  127,   82,   83,   84,  133,   98,   99,   88,  135,
  136,  103,  104,  264,  265,  266,  267,  268,  269,  270,
  271,   46,   89,   90,   46,  151,  152,   46,   46,  121,
   46,   46,  124,  125,  126,  127,   46,   46,   91,   46,
   44,   44,   44,  135,  136,   45,   44,   46,   44,  129,
  130,  131,  132,   44,   44,   93,  148,   44,   44,  151,
  152,   44,   44,   44,   41,   95,  133,  275,   44,   44,
   44,   44,   44,  165,  154,  155,  156,  157,   44,   44,
   44,   44,   44,   44,   44,   44,  166,  167,  168,   44,
   29,   -1,   -1,  173,  174,  175,   -1,   -1,   -1,  179,
  180,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  264,  265,  266,  267,  268,  269,  270,  271,
   -1,   -1,   -1,  275,  264,  265,  266,  267,  268,  269,
  270,  271,   -1,   -1,   -1,  275,  264,  265,  266,  267,
  268,  269,  270,  271,   -1,   -1,   -1,  275,  272,  273,
  272,  273,   -1,  263,  264,  265,  266,  267,  268,  269,
  270,  271,   -1,   -1,   -1,  275,  264,  265,  266,  267,
  268,  269,  270,  271,   -1,   -1,   -1,  275,  273,  274,
  264,  265,  266,  267,  268,  269,  270,  271,  264,  265,
  266,  267,  268,  269,  270,  271,   66,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   80,   -1,   -1,   -1,   -1,   -1,   86,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  104,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  124,  125,  126,  127,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  135,  136,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  151,  152,
};
#define YYFINAL 7
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 275
#if YYDEBUG
char *d3dxasm_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,"'+'","','","'-'","'.'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,"'_'",
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,"T_VS","T_XVS","T_XVSW","T_XVSS","T_PS","T_XPS","T_DEF","T_OP0","T_OP1",
"T_OP2","T_OP3","T_OP4","T_OP5","T_OP6","T_OP7","T_ID","T_NUM","T_FLT","T_EOF",
};
char *d3dxasm_rule[] = {
"$accept : Shader",
"Shader : Version T_EOF",
"Shader : Version Body T_EOF",
"Version : T_VS '.' Num '.' Num",
"Version : T_PS '.' Num '.' Num",
"Version : T_XPS '.' Num '.' Num",
"Version : T_XVS '.' Num '.' Num",
"Version : T_XVSW '.' Num '.' Num",
"Version : T_XVSS '.' Num '.' Num",
"Body : Statements",
"Body : Constants Statements",
"Constants : Constant",
"Constants : Constant Constants",
"Constant : T_DEF Reg ',' Val ',' Val ',' Val ',' Val",
"Statements : Statement",
"Statements : Statement Statements",
"Statement : Op0",
"Statement : Op1 Dst",
"Statement : Op2 Dst ',' Src",
"Statement : Op3 Dst ',' Src ',' Src",
"Statement : Op4 Dst ',' Src ',' Src ',' Src",
"Statement : Op5 Src ',' Src ',' Src ',' Src ',' Src ',' Src ',' Src",
"Statement : Op6 Dst ',' Dst ',' Src ',' Src ',' Src ',' Src",
"Statement : Op7 Dst ',' Dst ',' Dst ',' Src ',' Src ',' Src ',' Src",
"Dst : Reg",
"Dst : Reg '.' Id",
"Src : Src2",
"Src : '-' Src2",
"Src : Num '-' Src2",
"Src2 : Reg",
"Src2 : Reg '.' Id",
"Reg : Id",
"Reg : Id '[' Offset ']'",
"Reg : Id '[' Offset ']' '_' Id",
"Offset : '(' Offset ')' '+' Offset",
"Offset : '(' Offset ')' '-' Offset",
"Offset : '(' Offset ')'",
"Offset : Src2",
"Offset : '-' Num",
"Offset : Num",
"Offset : Src2 '+' Offset",
"Offset : '-' Num '+' Offset",
"Offset : Num '+' Offset",
"Offset : Src2 '-' Offset",
"Offset : '-' Num '-' Offset",
"Offset : Num '-' Offset",
"Op0 : T_OP0",
"Op0 : '+' T_OP0",
"Op1 : T_OP1",
"Op1 : '+' T_OP1",
"Op2 : T_OP2",
"Op2 : '+' T_OP2",
"Op3 : T_OP3",
"Op3 : '+' T_OP3",
"Op4 : T_OP4",
"Op4 : '+' T_OP4",
"Op5 : T_OP5",
"Op5 : '+' T_OP5",
"Op6 : T_OP6",
"Op6 : '+' T_OP6",
"Op7 : T_OP7",
"Op7 : '+' T_OP7",
"Val : Num",
"Val : '+' Num",
"Val : '-' Num",
"Val : Flt",
"Val : '+' Flt",
"Val : '-' Flt",
"Id : T_ID",
"Num : T_NUM",
"Flt : T_FLT",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
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
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
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
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
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
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
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
                    printf("%sdebug: state %d, error recovery shifting\
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
                    printf("%sdebug: error recovery discarding state %d\n",
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
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
{ Prod(P_SHADER_0); }
break;
case 2:
{ Prod(P_SHADER_1); }
break;
case 3:
{ Prod(P_VERSION_0); }
break;
case 4:
{ Prod(P_VERSION_1); }
break;
case 5:
{ Prod(P_VERSION_XPS); }
break;
case 6:
{ Prod(P_VERSION_XVS); }
break;
case 7:
{ Prod(P_VERSION_XVSW); }
break;
case 8:
{ Prod(P_VERSION_XVSS); }
break;
case 9:
{ Prod(P_BODY_0); }
break;
case 10:
{ Prod(P_BODY_1); }
break;
case 11:
{ Prod(P_CONSTANTS_0); }
break;
case 12:
{ Prod(P_CONSTANTS_1); }
break;
case 13:
{ Prod(P_CONSTANT_0); }
break;
case 14:
{ Prod(P_STATEMENTS_0); }
break;
case 15:
{ Prod(P_STATEMENTS_1); }
break;
case 16:
{ Prod(P_STATEMENT_0); }
break;
case 17:
{ Prod(P_STATEMENT_1); }
break;
case 18:
{ Prod(P_STATEMENT_2); }
break;
case 19:
{ Prod(P_STATEMENT_3); }
break;
case 20:
{ Prod(P_STATEMENT_4); }
break;
case 21:
{ Prod(P_STATEMENT_5); }
break;
case 22:
{ Prod(P_STATEMENT_6); }
break;
case 23:
{ Prod(P_STATEMENT_7); }
break;
case 24:
{ Prod(P_DST_0); }
break;
case 25:
{ Prod(P_DST_1); }
break;
case 26:
{ Prod(P_SRC_0); }
break;
case 27:
{ Prod(P_SRC_1); }
break;
case 28:
{ Prod(P_SRC_2); }
break;
case 29:
{ Prod(P_SRC2_0); }
break;
case 30:
{ Prod(P_SRC2_1); }
break;
case 31:
{ Prod(P_REG_0); }
break;
case 32:
{ Prod(P_REG_1); }
break;
case 33:
{ Prod(P_REG_2); }
break;
case 34:
{ Prod(P_OFFSET_7); }
break;
case 35:
{ Prod(P_OFFSET_8); }
break;
case 36:
{ Prod(P_OFFSET_6); }
break;
case 37:
{ Prod(P_OFFSET_0); }
break;
case 38:
{ Prod(P_OFFSET_5); }
break;
case 39:
{ Prod(P_OFFSET_1); }
break;
case 40:
{ Prod(P_OFFSET_2); }
break;
case 41:
{ Prod(P_OFFSET_10); }
break;
case 42:
{ Prod(P_OFFSET_3); }
break;
case 43:
{ Prod(P_OFFSET_4); }
break;
case 44:
{ Prod(P_OFFSET_11); }
break;
case 45:
{ Prod(P_OFFSET_9); }
break;
case 46:
{ Prod(P_OP0_0); }
break;
case 47:
{ Prod(P_OP0_1); }
break;
case 48:
{ Prod(P_OP1_0); }
break;
case 49:
{ Prod(P_OP1_1); }
break;
case 50:
{ Prod(P_OP2_0); }
break;
case 51:
{ Prod(P_OP2_1); }
break;
case 52:
{ Prod(P_OP3_0); }
break;
case 53:
{ Prod(P_OP3_1); }
break;
case 54:
{ Prod(P_OP4_0); }
break;
case 55:
{ Prod(P_OP4_1); }
break;
case 56:
{ Prod(P_OP5_0); }
break;
case 57:
{ Prod(P_OP5_1); }
break;
case 58:
{ Prod(P_OP6_0); }
break;
case 59:
{ Prod(P_OP6_1); }
break;
case 60:
{ Prod(P_OP7_0); }
break;
case 61:
{ Prod(P_OP7_1); }
break;
case 62:
{ Prod(P_VAL_0); }
break;
case 63:
{ Prod(P_VAL_1); }
break;
case 64:
{ Prod(P_VAL_2); }
break;
case 65:
{ Prod(P_VAL_3); }
break;
case 66:
{ Prod(P_VAL_4); }
break;
case 67:
{ Prod(P_VAL_5); }
break;
case 68:
{ Prod(P_ID); }
break;
case 69:
{ Prod(P_NUM); }
break;
case 70:
{ Prod(P_FLT); }
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
            printf("%sdebug: after reduction, shifting from state 0 to\
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
                printf("%sdebug: state %d, reading %d (%s)\n",
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
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
