#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse d3dxdecl_parse
#define yylex d3dxdecl_lex
#define yyerror d3dxdecl_error
#define yychar d3dxdecl_char
#define yyval d3dxdecl_val
#define yylval d3dxdecl_lval
#define yydebug d3dxdecl_debug
#define yynerrs d3dxdecl_nerrs
#define yyerrflag d3dxdecl_errflag
#define yyss d3dxdecl_ss
#define yyssp d3dxdecl_ssp
#define yyvs d3dxdecl_vs
#define yyvsp d3dxdecl_vsp
#define yylhs d3dxdecl_lhs
#define yylen d3dxdecl_len
#define yydefred d3dxdecl_defred
#define yydgoto d3dxdecl_dgoto
#define yysindex d3dxdecl_sindex
#define yyrindex d3dxdecl_rindex
#define yygindex d3dxdecl_gindex
#define yytable d3dxdecl_table
#define yycheck d3dxdecl_check
#define yyname d3dxdecl_name
#define yyrule d3dxdecl_rule
#define YYPREFIX "d3dxdecl_"

#define isatty _isatty
#define fileno _fileno

#define T_KW_STREAM 257
#define T_KW_SKIP 258
#define T_KW_FVF 259
#define T_KW_UINT8 260
#define T_KW_INT16 261
#define T_KW_FLOAT 262
#define T_KW_D3DCOLOR 263
#define T_NUM 264
#define T_HEX 265
#define T_ID 266
#define T_EOF 267
#define YYERRCODE 256
short d3dxdecl_lhs[] = {                                        -1,
    0,    0,    1,    1,    2,    2,    2,    2,    2,    5,
    5,    5,    5,    4,    4,    3,    3,    6,
};
short d3dxdecl_len[] = {                                         2,
    1,    2,    1,    2,    3,    3,    3,    3,    6,    1,
    1,    1,    1,    1,    3,    1,    1,    1,
};
short d3dxdecl_defred[] = {                                      0,
    0,    0,    0,   10,   11,   12,   13,    1,    0,    0,
    0,    0,   16,   17,    0,    0,   18,    0,    0,    2,
    4,    0,    5,    6,    7,    0,    8,    0,   15,    0,
    0,    9,
};
short d3dxdecl_dgoto[] = {                                       9,
   10,   11,   15,   18,   12,   19,
};
short d3dxdecl_sindex[] = {                                   -255,
 -254, -254, -257,    0,    0,    0,    0,    0,    0, -247,
 -244, -257,    0,    0,  -38,  -37,    0,  -36, -100,    0,
    0,  -59,    0,    0,    0, -257,    0, -254,    0,  -68,
  -33,    0,
};
short d3dxdecl_rindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -239,    0,    0,    0,    0,    0,    0,    0,  -30,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,
};
short d3dxdecl_gindex[] = {                                      0,
   19,    0,   -1,    5,    0,   21,
};
#define YYTABLESIZE 33
short d3dxdecl_table[] = {                                      27,
   16,    1,    2,    3,    4,    5,    6,    7,   17,   13,
   14,    8,    1,    2,    3,    4,    5,    6,    7,   20,
   23,   24,   25,   26,   31,   32,   30,    3,   14,   21,
   29,   28,   22,
};
short d3dxdecl_check[] = {                                      59,
    2,  257,  258,  259,  260,  261,  262,  263,  266,  264,
  265,  267,  257,  258,  259,  260,  261,  262,  263,  267,
   59,   59,   59,  124,   93,   59,   28,  267,   59,   11,
   26,   91,   12,
};
#define YYFINAL 9
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 267
#if YYDEBUG
char *d3dxdecl_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"';'",0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'|'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"T_KW_STREAM",
"T_KW_SKIP","T_KW_FVF","T_KW_UINT8","T_KW_INT16","T_KW_FLOAT","T_KW_D3DCOLOR",
"T_NUM","T_HEX","T_ID","T_EOF",
};
char *d3dxdecl_rule[] = {
"$accept : Declaration",
"Declaration : T_EOF",
"Declaration : Statements T_EOF",
"Statements : Statement",
"Statements : Statement Statements",
"Statement : T_KW_STREAM Num ';'",
"Statement : T_KW_SKIP Num ';'",
"Statement : T_KW_FVF Fvf ';'",
"Statement : Type Id ';'",
"Statement : Type Id '[' Num ']' ';'",
"Type : T_KW_UINT8",
"Type : T_KW_INT16",
"Type : T_KW_FLOAT",
"Type : T_KW_D3DCOLOR",
"Fvf : Id",
"Fvf : Id '|' Fvf",
"Num : T_NUM",
"Num : T_HEX",
"Id : T_ID",
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
{ Prod(P_DECLARATION_0, 0); }
break;
case 2:
{ Prod(P_DECLARATION_1, 1); }
break;
case 3:
{ Prod(P_STATEMENTS_0, 1); }
break;
case 4:
{ Prod(P_STATEMENTS_1, 2); }
break;
case 5:
{ Prod(P_STATEMENT_0, 1); }
break;
case 6:
{ Prod(P_STATEMENT_1, 1); }
break;
case 7:
{ Prod(P_STATEMENT_2, 1); }
break;
case 8:
{ Prod(P_STATEMENT_3, 2); }
break;
case 9:
{ Prod(P_STATEMENT_4, 3); }
break;
case 10:
{ Prod(P_TYPE_UINT8,    0); }
break;
case 11:
{ Prod(P_TYPE_INT16,    0); }
break;
case 12:
{ Prod(P_TYPE_FLOAT,    0); }
break;
case 13:
{ Prod(P_TYPE_D3DCOLOR, 0); }
break;
case 14:
{ Prod(P_FVF_0, 1); }
break;
case 15:
{ Prod(P_FVF_1, 2); }
break;
case 16:
{ Prod(P_NUM_0, 0); }
break;
case 17:
{ Prod(P_NUM_1, 0); }
break;
case 18:
{ Prod(P_ID_0, 0); }
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
