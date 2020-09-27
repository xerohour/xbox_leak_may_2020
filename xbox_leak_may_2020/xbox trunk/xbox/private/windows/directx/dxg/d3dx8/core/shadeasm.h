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
#define T_PS 258
#define T_DEF 259
#define T_OP0 260
#define T_OP1 261
#define T_OP2 262
#define T_OP3 263
#define T_OP4 264
#define T_ID 265
#define T_NUM 266
#define T_FLT 267
#define T_EOF 268
#define YYERRCODE 256
short d3dxasm_lhs[] = {                                        -1,
    0,    0,    1,    1,    2,    2,    5,    5,    6,    4,
    4,    9,    9,    9,    9,    9,   12,   12,   14,   14,
   14,   18,   18,    7,    7,   19,   19,   19,   19,   10,
   10,   11,   11,   13,   13,   15,   15,   16,   16,    8,
    8,    8,    8,    8,    8,   17,    3,   20,
};
short d3dxasm_len[] = {                                         2,
    2,    3,    5,    5,    1,    2,    1,    2,   10,    1,
    2,    1,    2,    4,    6,    8,    1,    3,    1,    2,
    3,    1,    3,    1,    4,    1,    1,    3,    3,    1,
    2,    1,    2,    1,    2,    1,    2,    1,    2,    1,
    2,    2,    1,    2,    2,    1,    1,    1,
};
short d3dxasm_defred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,   30,   32,   34,
   36,   38,    1,    0,    0,    5,    0,    0,    0,   12,
    0,    0,    0,    0,   47,    0,    0,   46,    0,    0,
   31,   33,   35,   37,   39,    2,    6,    8,   11,    0,
   13,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    3,    4,   48,    0,    0,   40,    0,   43,
    0,    0,    0,    0,   18,    0,    0,   14,   19,    0,
    0,   42,   45,   41,   44,    0,    0,    0,    0,   25,
   20,    0,    0,    0,    0,   29,   23,   28,   21,   15,
    0,    0,    0,    0,   16,    0,    9,
};
short d3dxasm_dgoto[] = {                                       3,
    4,   15,   67,   16,   17,   18,   62,   59,   19,   20,
   21,   41,   22,   68,   23,   24,   30,   69,   64,   60,
};
short d3dxasm_sindex[] = {                                   -253,
  -38,  -14,    0,  -24, -230, -230, -224,    0,    0,    0,
    0,    0,    0, -237, -213,    0,   -6, -214,   -6,    0,
 -224, -224, -224, -224,    0,   14,   15,    0,   18,  -27,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   19,
    0,   22,   26,   27, -230, -230,  -25, -226, -224,  -33,
  -33,  -33,    0,    0,    0, -223, -223,    0,   29,    0,
   31,   30,   32,  -12,    0, -224,   34,    0,    0,   42,
   43,    0,    0,    0,    0,  -25, -226, -224, -226,    0,
    0, -224,  -33,  -33,   44,    0,    0,    0,    0,    0,
   45,  -25,  -33,   46,    0,  -25,    0,
};
short d3dxasm_rindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   -1, -177,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -43,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -15,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    2,  -34,    3,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,
};
short d3dxasm_gindex[] = {                                      0,
    0,    0,    1,   16,   74,    0,   61,  -62,    0,    0,
    0,   -7,    0,  -30,    0,    0,  -47,  -10,  -66,   -5,
};
#define YYTABLESIZE 263
short d3dxasm_table[] = {                                      24,
   24,   65,   24,    1,    2,   26,   27,    5,   22,   22,
   86,   66,   88,   85,   42,   43,   44,   57,   14,   56,
   70,   71,   31,   32,   33,   34,   35,   17,   17,   94,
   87,    6,   37,   97,   39,   25,   14,   63,   28,   25,
   28,    7,   25,   55,    7,   53,   54,   58,   61,   24,
   73,   75,   90,   91,   36,   81,   72,   74,   22,   45,
   46,   47,   95,   48,   49,   50,   63,   29,   63,   51,
   52,   89,   76,   77,   79,   78,   58,   61,   82,   61,
   80,   40,   40,   40,   40,   83,   84,   92,   93,   96,
   10,   38,   58,    0,   27,   26,   58,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   24,   24,   24,   24,
   24,    0,    0,    0,   24,   22,   22,   22,   22,   22,
    0,   28,   25,   22,    7,    8,    9,   10,   11,   12,
   25,   55,    0,   13,   17,   17,   17,   17,   17,    0,
    0,    0,   17,    8,    9,   10,   11,   12,    7,    7,
    7,    7,    7,
};
short d3dxasm_check[] = {                                      43,
   44,   49,   46,  257,  258,    5,    6,   46,   43,   44,
   77,   45,   79,   76,   22,   23,   24,   43,   43,   45,
   51,   52,  260,  261,  262,  263,  264,   43,   44,   92,
   78,   46,   17,   96,   19,  266,   43,   48,  265,  266,
  265,   43,  266,  267,  259,   45,   46,   47,   48,   93,
   56,   57,   83,   84,  268,   66,   56,   57,   93,   46,
   46,   44,   93,   91,   46,   44,   77,    7,   79,   44,
   44,   82,   44,   43,   43,   46,   76,   77,   45,   79,
   93,   21,   22,   23,   24,   44,   44,   44,   44,   44,
  268,   18,   92,   -1,   93,   93,   96,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  260,  261,  262,  263,
  264,   -1,   -1,   -1,  268,  260,  261,  262,  263,  264,
   -1,  265,  266,  268,  259,  260,  261,  262,  263,  264,
  266,  267,   -1,  268,  260,  261,  262,  263,  264,   -1,
   -1,   -1,  268,  260,  261,  262,  263,  264,  260,  261,
  262,  263,  264,
};
#define YYFINAL 3
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 268
#if YYDEBUG
char *d3dxasm_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,"'+'","','","'-'","'.'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"T_VS",
"T_PS","T_DEF","T_OP0","T_OP1","T_OP2","T_OP3","T_OP4","T_ID","T_NUM","T_FLT",
"T_EOF",
};
char *d3dxasm_rule[] = {
"$accept : Shader",
"Shader : Version T_EOF",
"Shader : Version Body T_EOF",
"Version : T_VS '.' Num '.' Num",
"Version : T_PS '.' Num '.' Num",
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
"Dst : Reg",
"Dst : Reg '.' Id",
"Src : Src2",
"Src : '-' Src2",
"Src : Num '-' Src2",
"Src2 : Reg",
"Src2 : Reg '.' Id",
"Reg : Id",
"Reg : Id '[' Offset ']'",
"Offset : Src2",
"Offset : Num",
"Offset : Src2 '+' Offset",
"Offset : Num '+' Offset",
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
{ Prod(P_BODY_0); }
break;
case 6:
{ Prod(P_BODY_1); }
break;
case 7:
{ Prod(P_CONSTANTS_0); }
break;
case 8:
{ Prod(P_CONSTANTS_1); }
break;
case 9:
{ Prod(P_CONSTANT_0); }
break;
case 10:
{ Prod(P_STATEMENTS_0); }
break;
case 11:
{ Prod(P_STATEMENTS_1); }
break;
case 12:
{ Prod(P_STATEMENT_0); }
break;
case 13:
{ Prod(P_STATEMENT_1); }
break;
case 14:
{ Prod(P_STATEMENT_2); }
break;
case 15:
{ Prod(P_STATEMENT_3); }
break;
case 16:
{ Prod(P_STATEMENT_4); }
break;
case 17:
{ Prod(P_DST_0); }
break;
case 18:
{ Prod(P_DST_1); }
break;
case 19:
{ Prod(P_SRC_0); }
break;
case 20:
{ Prod(P_SRC_1); }
break;
case 21:
{ Prod(P_SRC_2); }
break;
case 22:
{ Prod(P_SRC2_0); }
break;
case 23:
{ Prod(P_SRC2_1); }
break;
case 24:
{ Prod(P_REG_0); }
break;
case 25:
{ Prod(P_REG_1); }
break;
case 26:
{ Prod(P_OFFSET_0); }
break;
case 27:
{ Prod(P_OFFSET_1); }
break;
case 28:
{ Prod(P_OFFSET_2); }
break;
case 29:
{ Prod(P_OFFSET_3); }
break;
case 30:
{ Prod(P_OP0_0); }
break;
case 31:
{ Prod(P_OP0_1); }
break;
case 32:
{ Prod(P_OP1_0); }
break;
case 33:
{ Prod(P_OP1_1); }
break;
case 34:
{ Prod(P_OP2_0); }
break;
case 35:
{ Prod(P_OP2_1); }
break;
case 36:
{ Prod(P_OP3_0); }
break;
case 37:
{ Prod(P_OP3_1); }
break;
case 38:
{ Prod(P_OP4_0); }
break;
case 39:
{ Prod(P_OP4_1); }
break;
case 40:
{ Prod(P_VAL_0); }
break;
case 41:
{ Prod(P_VAL_1); }
break;
case 42:
{ Prod(P_VAL_2); }
break;
case 43:
{ Prod(P_VAL_3); }
break;
case 44:
{ Prod(P_VAL_4); }
break;
case 45:
{ Prod(P_VAL_5); }
break;
case 46:
{ Prod(P_ID); }
break;
case 47:
{ Prod(P_NUM); }
break;
case 48:
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
