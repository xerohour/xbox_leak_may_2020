%union {
	char *tokval;
}


%token <tokval> T_EOF
%token <tokval> T_INT
%token <tokval> T_HEX
%token <tokval> T_FLOAT
%token <tokval> T_ID
%token <tokval> T_DECL
%token <tokval> T_ASM
%token <tokval> T_KW_TECHNIQUE
%token <tokval> T_KW_PASS

%start EffectBody

%%


//
// Effect
//

EffectBody      : Techniques T_EOF                                          { Prod(P_EFFECT_BODY0, 1); }
                | Parameters Techniques T_EOF                               { Prod(P_EFFECT_BODY1, 2); }
                ;


//
// Parameter
//

Parameters      : Parameter                                                 { Prod(P_PARAMETERS0, 1); }
                | Parameter Parameters                                      { Prod(P_PARAMETERS1, 2); }
                ;

Parameter       : Id FourCC ';'                                             { Prod(P_PARAMETER0, 2); }
                | Id FourCC '=' Const ';'                                   { Prod(P_PARAMETER1, 3); }
                ;

                
//
// Technique
//

Techniques      : Technique                                                 { Prod(P_TECHNIQUES0, 1); }
                | Technique Techniques                                      { Prod(P_TECHNIQUES1, 2); }
                ;

Technique       : T_KW_TECHNIQUE '{' TechniqueBody                          { Prod(P_TECHNIQUE0, 1); }
                | T_KW_TECHNIQUE FourCC '{' TechniqueBody                   { Prod(P_TECHNIQUE1, 2); }
                ;

TechniqueBody   : '}'                                                       { Prod(P_TECHNIQUE_BODY0, 0); }
                | Passes '}'                                                { Prod(P_TECHNIQUE_BODY1, 1); }
                | Assignments '}'                                           { Prod(P_TECHNIQUE_BODY2, 1); }
                | Assignments Passes '}'                                    { Prod(P_TECHNIQUE_BODY3, 2); }
                ;


//
// Passes
//

Passes          : Pass                                                      { Prod(P_PASSES0, 1); }
                | Pass Passes                                               { Prod(P_PASSES1, 2); }
                ;

Pass            : T_KW_PASS '{' PassBody                                    { Prod(P_PASS0, 1); }
                | T_KW_PASS FourCC '{' PassBody                             { Prod(P_PASS1, 2); }
                ;

PassBody        : '}'                                                       { Prod(P_PASS_BODY0, 0); }
                | Assignments '}'                                           { Prod(P_PASS_BODY1, 1); }
                ;




//
// Assignments
//

Assignments     : Assignment                                                { Prod(P_ASSIGNMENTS0, 1); }
                | Assignment Assignments                                    { Prod(P_ASSIGNMENTS1, 2); }
                ;

Assignment      : LValue '=' Expression ';'                                 { Prod(P_ASSIGNMENT0, 2); }
                | LValue '=' Reference ';'                                  { Prod(P_ASSIGNMENT1, 2); }
                ;

LValue          : Id                                                        { Prod(P_LVALUE0, 1); }
                | Id '[' Dword ']'                                          { Prod(P_LVALUE1, 2); }
                ;

Expression      : Value                                                     { Prod(P_EXPRESSION0, 1); }
                | Value '|' Expression                                      { Prod(P_EXPRESSION1, 2); }
                ;

Reference       : '<' FourCC '>'                                            { Prod(P_REFERENCE0, 1); }
                ;




//
// Misc
//

Const           : Dword                                                     { Prod(P_CONST_DWORD, 1); }
                | Float                                                     { Prod(P_CONST_FLOAT, 1); }
                | Vector                                                    { Prod(P_CONST_VECTOR, 1); }
                | Matrix                                                    { Prod(P_CONST_MATRIX, 1); }
                | VertexShader                                              { Prod(P_CONST_VERTEXSHADER, 1); }
                | PixelShader                                               { Prod(P_CONST_PIXELSHADER, 1); }
                ;

Value           : Const                                                     { Prod(P_VALUE0, 1); }
                | Id                                                        { Prod(P_VALUE1, 1); }
                ;


Dword           : T_INT                                                     { Prod(P_DWORD0, 0); }
                | '-' T_INT                                                 { Prod(P_DWORD1, 0); }
                | T_HEX                                                     { Prod(P_DWORD2, 0); }
                | '-' T_HEX                                                 { Prod(P_DWORD3, 0); }
                ;

Float           : T_FLOAT                                                   { Prod(P_FLOAT0, 0); }
                | '-' T_FLOAT                                               { Prod(P_FLOAT1, 0); }
                ;

Vector          : '(' Float ')'                                             { Prod(P_VECTOR0, 1); }
                | '(' Float ',' Float ')'                                   { Prod(P_VECTOR1, 2); }
                | '(' Float ',' Float ',' Float ')'                         { Prod(P_VECTOR2, 3); }
                | '(' Float ',' Float ',' Float ',' Float ')'               { Prod(P_VECTOR3, 4); }
                ;

Matrix          : '[' Float ',' Float ',' Float ',' Float ','
                      Float ',' Float ',' Float ',' Float ','
                      Float ',' Float ',' Float ',' Float ','
                      Float ',' Float ',' Float ',' Float ']'               { Prod(P_MATRIX0, 16); }
                ;

VertexShader    : Decl                                                      { Prod(P_VERTEXSHADER0, 1); }
                | Decl Asm                                                  { Prod(P_VERTEXSHADER1, 2); }
                ;

PixelShader     : Asm                                                       { Prod(P_PIXELSHADER0, 1); }
                ;

FourCC          : Id                                                        { Prod(P_FOURCC0, 1); }
                ;

Id              : T_ID                                                      { Prod(P_ID0, 0); }
                ;

Decl            : T_DECL                                                    { Prod(P_DECL0, 0); }
                ;

Asm             : T_ASM                                                     { Prod(P_ASM0, 0); }
                ;
