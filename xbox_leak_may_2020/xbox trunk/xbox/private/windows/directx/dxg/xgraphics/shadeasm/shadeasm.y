%{

#define isatty _isatty
#define fileno _fileno

%}

%token T_VS
%token T_XVS
%token T_XVSW
%token T_XVSS
%token T_PS
%token T_XPS
%token T_DEF
%token T_OP0
%token T_OP1
%token T_OP2
%token T_OP3
%token T_OP4
%token T_OP5
%token T_OP6
%token T_OP7
%token T_ID
%token T_NUM
%token T_FLT
%token T_EOF



%start Shader

%%
Shader          : Version T_EOF                             { Prod(P_SHADER_0); }
                | Version Body T_EOF                        { Prod(P_SHADER_1); }
                ;

Version         : T_VS '.' Num '.' Num                      { Prod(P_VERSION_0); }
                | T_PS '.' Num '.' Num                      { Prod(P_VERSION_1); }
                | T_XPS '.' Num '.' Num                     { Prod(P_VERSION_XPS); }
                | T_XVS '.' Num '.' Num                     { Prod(P_VERSION_XVS); }
                | T_XVSW '.' Num '.' Num                    { Prod(P_VERSION_XVSW); }
                | T_XVSS '.' Num '.' Num                    { Prod(P_VERSION_XVSS); }
                ;

Body            : Statements                                { Prod(P_BODY_0); }
                | Constants Statements                      { Prod(P_BODY_1); }
                ;

Constants       : Constant                                  { Prod(P_CONSTANTS_0); }
                | Constant Constants                        { Prod(P_CONSTANTS_1); }
                ;

Constant        : T_DEF Reg ',' Val ',' Val ',' Val ',' Val { Prod(P_CONSTANT_0); }
                ;

Statements      : Statement                                 { Prod(P_STATEMENTS_0); }
                | Statement Statements                      { Prod(P_STATEMENTS_1); }
                ;

Statement       : Op0                                       { Prod(P_STATEMENT_0); }
                | Op1 Dst                                   { Prod(P_STATEMENT_1); }
                | Op2 Dst ',' Src                           { Prod(P_STATEMENT_2); }
                | Op3 Dst ',' Src ',' Src                   { Prod(P_STATEMENT_3); }
                | Op4 Dst ',' Src ',' Src ',' Src           { Prod(P_STATEMENT_4); }
                | Op5 Src ',' Src ',' Src ',' Src ',' Src ',' Src ',' Src  { Prod(P_STATEMENT_5); }
                | Op6 Dst ',' Dst ',' Src ',' Src ',' Src ',' Src          { Prod(P_STATEMENT_6); }
                | Op7 Dst ',' Dst ',' Dst ',' Src ',' Src ',' Src ',' Src  { Prod(P_STATEMENT_7); }
                ;

Dst             : Reg                                       { Prod(P_DST_0); }
                | Reg '.' Id                                { Prod(P_DST_1); }
                ;

Src             : Src2                                      { Prod(P_SRC_0); }
                | '-' Src2                                  { Prod(P_SRC_1); }
                | Num '-' Src2                              { Prod(P_SRC_2); }
                ;

Src2            : Reg                                       { Prod(P_SRC2_0); }
                | Reg '.' Id                                { Prod(P_SRC2_1); }
                ;

Reg             : Id                                        { Prod(P_REG_0); }
                | Id '[' Offset ']'                         { Prod(P_REG_1); }
                | Id '[' Offset ']' '_' Id                  { Prod(P_REG_2); }
                ;


Offset          : '(' Offset ')' '+' Offset                 { Prod(P_OFFSET_7); }
                | '(' Offset ')' '-' Offset                 { Prod(P_OFFSET_8); }
                | '(' Offset ')'                            { Prod(P_OFFSET_6); }
                | Src2                                      { Prod(P_OFFSET_0); }
                | '-' Num                                   { Prod(P_OFFSET_5); }
                | Num                                       { Prod(P_OFFSET_1); }
                | Src2 '+' Offset                           { Prod(P_OFFSET_2); }
                | '-' Num '+' Offset                        { Prod(P_OFFSET_10); }
                | Num '+' Offset                            { Prod(P_OFFSET_3); }
                | Src2 '-' Offset                           { Prod(P_OFFSET_4); }
                | '-' Num '-' Offset                        { Prod(P_OFFSET_11); }
                | Num '-' Offset                            { Prod(P_OFFSET_9); }
                ;

Op0             : T_OP0                                     { Prod(P_OP0_0); }
                | '+' T_OP0                                 { Prod(P_OP0_1); }
                ;

Op1             : T_OP1                                     { Prod(P_OP1_0); }
                | '+' T_OP1                                 { Prod(P_OP1_1); }
                ;

Op2             : T_OP2                                     { Prod(P_OP2_0); }
                | '+' T_OP2                                 { Prod(P_OP2_1); }
                ;

Op3             : T_OP3                                     { Prod(P_OP3_0); }
                | '+' T_OP3                                 { Prod(P_OP3_1); }
                ;

Op4             : T_OP4                                     { Prod(P_OP4_0); }
                | '+' T_OP4                                 { Prod(P_OP4_1); }
                ;

Op5             : T_OP5                                     { Prod(P_OP5_0); }
                | '+' T_OP5                                 { Prod(P_OP5_1); }
                ;

Op6             : T_OP6                                     { Prod(P_OP6_0); }
                | '+' T_OP6                                 { Prod(P_OP6_1); }
                ;

Op7             : T_OP7                                     { Prod(P_OP7_0); }
                | '+' T_OP7                                 { Prod(P_OP7_1); }
                ;

Val             : Num                                       { Prod(P_VAL_0); }
                | '+' Num                                   { Prod(P_VAL_1); }
                | '-' Num                                   { Prod(P_VAL_2); }
                | Flt                                       { Prod(P_VAL_3); }
                | '+' Flt                                   { Prod(P_VAL_4); }
                | '-' Flt                                   { Prod(P_VAL_5); }
                ;

Id              : T_ID                                      { Prod(P_ID); };
Num             : T_NUM                                     { Prod(P_NUM); };
Flt             : T_FLT                                     { Prod(P_FLT); };

%%
