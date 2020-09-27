#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse d3dxeffect_parse
#define yylex d3dxeffect_lex
#define yyerror d3dxeffect_error
#define yychar d3dxeffect_char
#define yyval d3dxeffect_val
#define yylval d3dxeffect_lval
#define yydebug d3dxeffect_debug
#define yynerrs d3dxeffect_nerrs
#define yyerrflag d3dxeffect_errflag
#define yyss d3dxeffect_ss
#define yyssp d3dxeffect_ssp
#define yyvs d3dxeffect_vs
#define yyvsp d3dxeffect_vsp
#define yylhs d3dxeffect_lhs
#define yylen d3dxeffect_len
#define yydefred d3dxeffect_defred
#define yydgoto d3dxeffect_dgoto
#define yysindex d3dxeffect_sindex
#define yyrindex d3dxeffect_rindex
#define yygindex d3dxeffect_gindex
#define yytable d3dxeffect_table
#define yycheck d3dxeffect_check
#define yyname d3dxeffect_name
#define yyrule d3dxeffect_rule
#define YYPREFIX "d3dxeffect_"
typedef union {
	char *tokval;
} YYSTYPE;
#define T_EOF 257
#define T_INT 258
#define T_HEX 259
#define T_FLOAT 260
#define T_ID 261
#define T_DECL 262
#define T_ASM 263
#define T_KW_TECHNIQUE 264
#define T_KW_PASS 265
#define YYERRCODE 256
short d3dxeffect_lhs[] = {                                        -1,
    0,    0,    2,    2,    3,    3,    1,    1,    7,    7,
    8,    8,    8,    8,    9,    9,   11,   11,   12,   12,
   10,   10,   13,   13,   14,   14,   15,   15,   16,    6,
    6,    6,    6,    6,    6,   18,   18,   17,   17,   17,
   17,   19,   19,   20,   20,   20,   20,   21,   22,   22,
   23,    5,    4,   24,   25,
};
short d3dxeffect_len[] = {                                         2,
    2,    3,    1,    2,    3,    5,    1,    2,    3,    4,
    1,    2,    2,    3,    1,    2,    3,    4,    1,    2,
    1,    2,    4,    4,    1,    4,    1,    3,    3,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    2,    1,
    2,    1,    2,    3,    5,    7,    9,   33,    1,    2,
    1,    1,    1,    1,    1,
};
short d3dxeffect_defred[] = {                                      0,
   53,    0,    0,    0,    0,    0,    0,    0,    0,   52,
    0,    1,    0,    4,    0,    8,    0,   11,    0,    9,
    0,    0,    0,    0,    0,    0,    2,    5,    0,    0,
    0,    0,   12,   13,    0,   16,   22,    0,   10,   38,
   40,   42,   54,   55,    0,    0,    0,    0,   30,   31,
   32,   33,   34,   35,    0,   51,   19,    0,   17,    0,
    0,    0,   14,    0,   37,   36,    0,    0,    0,    0,
    0,   39,   41,   43,    0,    6,   50,   20,   18,   26,
    0,   23,   24,    0,    0,   44,    0,   29,   28,    0,
    0,    0,   45,    0,    0,    0,    0,   46,    0,    0,
    0,    0,   47,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   48,
};
short d3dxeffect_dgoto[] = {                                       3,
    4,    5,    6,   19,   11,   66,    8,   20,   21,   22,
   23,   59,   24,   25,   67,   68,   49,   69,   50,   51,
   52,   53,   54,   55,   56,
};
short d3dxeffect_sindex[] = {                                   -234,
    0, -116,    0, -243, -246, -235, -235, -246, -123,    0,
  -85,    0, -209,    0,  -12,    0, -115,    0,  -41,    0,
  -71, -122, -210, -235,   -5, -123,    0,    0,  -28, -112,
  -65,  -22,    0,    0,  -63,    0,    0,  -40,    0,    0,
    0,    0,    0,    0,  -44, -217,  -44,    5,    0,    0,
    0,    0,    0,    0, -198,    0,    0,  -58,    0, -112,
 -214,  -24,    0, -235,    0,    0,   11,   16,  -47, -182,
   36,    0,    0,    0,  -10,    0,    0,    0,    0,    0,
   20,    0,    0,  -34,  -44,    0,  -44,    0,    0,   40,
   -9,  -44,    0,  -44,   44,   -8,  -44,    0,  -44,   46,
   51,  -44,    0,   50,  -44,   52,  -44,   54,  -44,   56,
  -44,   57,  -44,   58,  -44,   59,  -44,   60,  -44,   61,
  -44,   63,  -44,   64,  -44,   17,    0,
};
short d3dxeffect_rindex[] = {                                      0,
    0,    0,    0,    0,    0, -155,    0, -146,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   53,    0,
    0,    0,  -13, -121,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -50,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   62,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,
};
short d3dxeffect_gindex[] = {                                      0,
   32,  107,    0,   22,    8,   86,    0,   90,   30,  -14,
    0,   65,    0,    0,   33,    0,   87,    0,  -26,    0,
    0,    0,    0,    0,   67,
};
#define YYTABLESIZE 237
short d3dxeffect_table[] = {                                      47,
   70,   18,   34,   21,   46,   47,    9,   30,   49,   37,
   46,   47,   57,   12,   15,   58,   46,    2,   71,   64,
   75,    7,   61,   10,   31,    1,    1,    7,   10,    2,
   86,   93,   98,   87,   94,   99,   13,   26,   10,   16,
   72,   73,   74,   72,   73,   58,   28,   27,   29,   32,
   45,   35,   36,   33,   17,   38,   45,   60,   90,   65,
   91,   63,   45,   76,   44,   95,   78,   96,   80,   82,
  100,   81,  101,   49,   83,  104,   84,   74,  106,   85,
  108,   88,  110,   92,  112,   10,  114,   97,  116,  102,
  118,  103,  120,  105,  122,  107,  124,  109,  126,  111,
  113,  115,  117,  119,  121,   65,  123,  125,    3,  127,
    7,   15,   14,   25,   48,   39,   89,    0,   62,    0,
   27,   77,    0,    0,   79,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    1,    0,    0,
    0,   17,   17,   21,    1,    1,    0,    0,    1,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   42,    0,   40,   41,   42,
    1,   43,   44,   40,   41,   42,    1,   43,   44,   40,
   41,   42,    0,   43,   44,   40,   41,
};
short d3dxeffect_check[] = {                                      40,
   45,  125,  125,  125,   45,   40,  123,  123,   59,   24,
   45,   40,  125,  257,    7,   30,   45,  264,   45,   60,
   47,    0,   45,    2,   17,  261,  261,    6,    7,  264,
   41,   41,   41,   44,   44,   44,    5,  123,   17,    8,
  258,  259,  260,  258,  259,   60,   59,  257,   61,   91,
   91,   22,   23,  125,  265,   61,   91,  123,   85,   38,
   87,  125,   91,   59,  263,   92,  125,   94,   93,   59,
   97,   64,   99,  124,   59,  102,  124,  260,  105,   44,
  107,   62,  109,   44,  111,   64,  113,   44,  115,   44,
  117,   41,  119,   44,  121,   44,  123,   44,  125,   44,
   44,   44,   44,   44,   44,   84,   44,   44,  264,   93,
  257,  125,    6,   61,   29,   26,   84,   -1,   32,   -1,
   59,   55,   -1,   -1,   60,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  261,   -1,   -1,
   -1,  265,  265,  265,  261,  261,   -1,   -1,  261,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  260,   -1,  258,  259,  260,
  261,  262,  263,  258,  259,  260,  261,  262,  263,  258,
  259,  260,   -1,  262,  263,  258,  259,
};
#define YYFINAL 3
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 265
#if YYDEBUG
char *d3dxeffect_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','","'-'",0,0,0,0,0,0,0,0,0,0,0,0,0,"';'","'<'",
"'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,
"']'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'","'|'",
"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,"T_EOF","T_INT","T_HEX","T_FLOAT","T_ID","T_DECL",
"T_ASM","T_KW_TECHNIQUE","T_KW_PASS",
};
char *d3dxeffect_rule[] = {
"$accept : EffectBody",
"EffectBody : Techniques T_EOF",
"EffectBody : Parameters Techniques T_EOF",
"Parameters : Parameter",
"Parameters : Parameter Parameters",
"Parameter : Id FourCC ';'",
"Parameter : Id FourCC '=' Const ';'",
"Techniques : Technique",
"Techniques : Technique Techniques",
"Technique : T_KW_TECHNIQUE '{' TechniqueBody",
"Technique : T_KW_TECHNIQUE FourCC '{' TechniqueBody",
"TechniqueBody : '}'",
"TechniqueBody : Passes '}'",
"TechniqueBody : Assignments '}'",
"TechniqueBody : Assignments Passes '}'",
"Passes : Pass",
"Passes : Pass Passes",
"Pass : T_KW_PASS '{' PassBody",
"Pass : T_KW_PASS FourCC '{' PassBody",
"PassBody : '}'",
"PassBody : Assignments '}'",
"Assignments : Assignment",
"Assignments : Assignment Assignments",
"Assignment : LValue '=' Expression ';'",
"Assignment : LValue '=' Reference ';'",
"LValue : Id",
"LValue : Id '[' Dword ']'",
"Expression : Value",
"Expression : Value '|' Expression",
"Reference : '<' FourCC '>'",
"Const : Dword",
"Const : Float",
"Const : Vector",
"Const : Matrix",
"Const : VertexShader",
"Const : PixelShader",
"Value : Const",
"Value : Id",
"Dword : T_INT",
"Dword : '-' T_INT",
"Dword : T_HEX",
"Dword : '-' T_HEX",
"Float : T_FLOAT",
"Float : '-' T_FLOAT",
"Vector : '(' Float ')'",
"Vector : '(' Float ',' Float ')'",
"Vector : '(' Float ',' Float ',' Float ')'",
"Vector : '(' Float ',' Float ',' Float ',' Float ')'",
"Matrix : '[' Float ',' Float ',' Float ',' Float ',' Float ',' Float ',' Float ',' Float ',' Float ',' Float ',' Float ',' Float ',' Float ',' Float ',' Float ',' Float ']'",
"VertexShader : Decl",
"VertexShader : Decl Asm",
"PixelShader : Asm",
"FourCC : Id",
"Id : T_ID",
"Decl : T_DECL",
"Asm : T_ASM",
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
{ Prod(P_EFFECT_BODY0, 1); }
break;
case 2:
{ Prod(P_EFFECT_BODY1, 2); }
break;
case 3:
{ Prod(P_PARAMETERS0, 1); }
break;
case 4:
{ Prod(P_PARAMETERS1, 2); }
break;
case 5:
{ Prod(P_PARAMETER0, 2); }
break;
case 6:
{ Prod(P_PARAMETER1, 3); }
break;
case 7:
{ Prod(P_TECHNIQUES0, 1); }
break;
case 8:
{ Prod(P_TECHNIQUES1, 2); }
break;
case 9:
{ Prod(P_TECHNIQUE0, 1); }
break;
case 10:
{ Prod(P_TECHNIQUE1, 2); }
break;
case 11:
{ Prod(P_TECHNIQUE_BODY0, 0); }
break;
case 12:
{ Prod(P_TECHNIQUE_BODY1, 1); }
break;
case 13:
{ Prod(P_TECHNIQUE_BODY2, 1); }
break;
case 14:
{ Prod(P_TECHNIQUE_BODY3, 2); }
break;
case 15:
{ Prod(P_PASSES0, 1); }
break;
case 16:
{ Prod(P_PASSES1, 2); }
break;
case 17:
{ Prod(P_PASS0, 1); }
break;
case 18:
{ Prod(P_PASS1, 2); }
break;
case 19:
{ Prod(P_PASS_BODY0, 0); }
break;
case 20:
{ Prod(P_PASS_BODY1, 1); }
break;
case 21:
{ Prod(P_ASSIGNMENTS0, 1); }
break;
case 22:
{ Prod(P_ASSIGNMENTS1, 2); }
break;
case 23:
{ Prod(P_ASSIGNMENT0, 2); }
break;
case 24:
{ Prod(P_ASSIGNMENT1, 2); }
break;
case 25:
{ Prod(P_LVALUE0, 1); }
break;
case 26:
{ Prod(P_LVALUE1, 2); }
break;
case 27:
{ Prod(P_EXPRESSION0, 1); }
break;
case 28:
{ Prod(P_EXPRESSION1, 2); }
break;
case 29:
{ Prod(P_REFERENCE0, 1); }
break;
case 30:
{ Prod(P_CONST_DWORD, 1); }
break;
case 31:
{ Prod(P_CONST_FLOAT, 1); }
break;
case 32:
{ Prod(P_CONST_VECTOR, 1); }
break;
case 33:
{ Prod(P_CONST_MATRIX, 1); }
break;
case 34:
{ Prod(P_CONST_VERTEXSHADER, 1); }
break;
case 35:
{ Prod(P_CONST_PIXELSHADER, 1); }
break;
case 36:
{ Prod(P_VALUE0, 1); }
break;
case 37:
{ Prod(P_VALUE1, 1); }
break;
case 38:
{ Prod(P_DWORD0, 0); }
break;
case 39:
{ Prod(P_DWORD1, 0); }
break;
case 40:
{ Prod(P_DWORD2, 0); }
break;
case 41:
{ Prod(P_DWORD3, 0); }
break;
case 42:
{ Prod(P_FLOAT0, 0); }
break;
case 43:
{ Prod(P_FLOAT1, 0); }
break;
case 44:
{ Prod(P_VECTOR0, 1); }
break;
case 45:
{ Prod(P_VECTOR1, 2); }
break;
case 46:
{ Prod(P_VECTOR2, 3); }
break;
case 47:
{ Prod(P_VECTOR3, 4); }
break;
case 48:
{ Prod(P_MATRIX0, 16); }
break;
case 49:
{ Prod(P_VERTEXSHADER0, 1); }
break;
case 50:
{ Prod(P_VERTEXSHADER1, 2); }
break;
case 51:
{ Prod(P_PIXELSHADER0, 1); }
break;
case 52:
{ Prod(P_FOURCC0, 1); }
break;
case 53:
{ Prod(P_ID0, 0); }
break;
case 54:
{ Prod(P_DECL0, 0); }
break;
case 55:
{ Prod(P_ASM0, 0); }
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
