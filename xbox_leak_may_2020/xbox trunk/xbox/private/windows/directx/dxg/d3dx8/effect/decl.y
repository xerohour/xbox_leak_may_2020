%{

#define isatty _isatty
#define fileno _fileno

%}

%token T_KW_STREAM
%token T_KW_SKIP
%token T_KW_FVF
%token T_KW_UINT8
%token T_KW_INT16
%token T_KW_FLOAT
%token T_KW_D3DCOLOR
%token T_NUM
%token T_HEX
%token T_ID
%token T_EOF

%start Declaration

%%

Declaration     : T_EOF                                     { Prod(P_DECLARATION_0, 0); }
                | Statements T_EOF                          { Prod(P_DECLARATION_1, 1); }
                ;

Statements      : Statement                                 { Prod(P_STATEMENTS_0, 1); }
                | Statement Statements                      { Prod(P_STATEMENTS_1, 2); }
                ;

Statement       : T_KW_STREAM Num ';'                       { Prod(P_STATEMENT_0, 1); }
                | T_KW_SKIP   Num ';'                       { Prod(P_STATEMENT_1, 1); }
                | T_KW_FVF    Fvf ';'                       { Prod(P_STATEMENT_2, 1); }
                | Type        Id ';'                        { Prod(P_STATEMENT_3, 2); }
                | Type        Id '[' Num ']' ';'            { Prod(P_STATEMENT_4, 3); }
                ;

Type            : T_KW_UINT8                                { Prod(P_TYPE_UINT8,    0); }
                | T_KW_INT16                                { Prod(P_TYPE_INT16,    0); }
                | T_KW_FLOAT                                { Prod(P_TYPE_FLOAT,    0); }
                | T_KW_D3DCOLOR                             { Prod(P_TYPE_D3DCOLOR, 0); }
                ;

Fvf             : Id                                        { Prod(P_FVF_0, 1); }
                | Id '|' Fvf                                { Prod(P_FVF_1, 2); }
                ;

Num             : T_NUM                                     { Prod(P_NUM_0, 0); }
                | T_HEX                                     { Prod(P_NUM_1, 0); }
                ;

Id              : T_ID                                      { Prod(P_ID_0, 0); }
                ;


%%
