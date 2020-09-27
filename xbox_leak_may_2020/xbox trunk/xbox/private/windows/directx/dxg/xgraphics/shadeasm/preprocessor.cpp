///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
//
//
///////////////////////////////////////////////////////////////////////////

// Poor man's C Preprocessor, with a few extensions to support vertex shader
// assembly language.
//
//
// Features:
//
// #ifdef FOO
// #ifndef FOO
// #else
// #endif
//
// #include "foo"
// #define FOO BAR
// #define FOO(X) ...
// #pragma FOO
// #undef FOO
// #error message
//
// NVASM style macros

#include "pchshadeasm.h"

// File-specific debugging hooks

#ifdef DBG
// #define DBG_PRINT_PREPROCESSED
#endif

namespace XGRAPHICS {

#define MAXLINE 300
#define MAX_INCLUDE_LEVELS 17
#define MAX_IF_LEVELS 20

#define TOKEN_SYMBOL 0
#define TOKEN_WS 1
#define TOKEN_OTHER 2
#define TOKEN_INT 3
#define TOKEN_EOS 4

static char* Copy(LPCSTR s){
	DWORD size = strlen(s);
	char* result = new char[size+1];
	if(result){
		memcpy(result,s,size+1);
	}
	return result;
}

class PreProcessor {
    // A simple struct for tokens

    class Token {
    public:
        DWORD type;
        const char* token;
        DWORD length;

        bool IsEqual(DWORD type, const char* s, DWORD length){
            return type == this->type && length == this->length &&
                strncmp(s,this->token,length) == 0;
        }
        bool IsEqual(char c){
            return type == TOKEN_OTHER && length == 1 &&
                token[0] == c;
        }
    };

    class Map;
    class Stack;

public:
	PreProcessor(){
        InEnum = false;
	}

	~PreProcessor(){
	}

	HRESULT Initialize(LPCSTR pSourceFileName,
		SASM_ResolverCallback pResolver,
		LPVOID pResolverUserData,
		LPXGBUFFER  pInput,
		LPXGBUFFER* pOutput,
		XD3DXErrorLog* pErrorLog)
	{
		HRESULT hr = S_OK;
		if(SUCCEEDED(hr)){
			hr = defines.Initialize(53);
		}
		if(SUCCEEDED(hr)){
			hr = macros.Initialize(20);
		}
        if(SUCCEEDED(hr)){
            hr = lineBuf.Initialize(MAXLINE);
        }
		if(SUCCEEDED(hr)){
			this->pResolver = pResolver;
			this->pResolverUserData = pResolverUserData;
			this->pBaseInput = pInput;
			this->pOutput = pOutput;
			this->pErrorLog = pErrorLog;
			currentIncludeLevel = 0;
			this->pInput = &fileStack[0];
			this->bAngleBracketsAreQuotes = false;
			strncpy(m_currentOutputFileName, pSourceFileName, sizeof(m_currentOutputFileName));
			m_currentOutputLine = 1;
			hr = this->pInput->Initialize(pBaseInput, pSourceFileName);
		}
		return hr;
	}

	HRESULT Preprocess(){
		HRESULT hr = S_OK;
		if(SUCCEEDED(hr)){
            bool moreWork = true;
			while(moreWork && SUCCEEDED(hr)){
                hr = MoreWorkToDo(moreWork);
                if(SUCCEEDED(hr)){
                    if(moreWork){
				        hr = DoLine(NULL);
                    }
                }
			}
		}
		if(SUCCEEDED(hr)){
			hr = XGBufferCreateFromBuffer(&outputBuffer,pOutput);
		}
		return hr;
	}
private:

	HRESULT MoreWorkToDo(bool& moreWork){
        HRESULT hr = S_OK;
		while( pInput->IsEOF() && SUCCEEDED(hr)) {
            if(pInput->InsideIfElse()){
                Error(true, ERROR_EXPECTEDENDIF, "Expected an #endif");
			    SETERROR(hr, E_FAIL);
                break;
            }
			if(currentIncludeLevel == 0){
				moreWork = false; // We're done
                return hr;
			}
			pInput--;
			currentIncludeLevel--;
		}
        moreWork = true;
		return hr;
	}

	bool InEnum;
    bool gotEnumOpen;
    bool gotEnumClose;
    int  EnumVal;


    HRESULT DoLine(char *pLine){
        HRESULT hr;
        if(pLine != NULL){
            hr = S_OK;
            lineBuf.Clear();
            lineBuf.Append(pLine);
        }
        else
            hr = FindLine(lineBuf);
		
		if(SUCCEEDED(hr)){
			const char* pDirective = FindDirective(lineBuf);
			if(pDirective){
				hr = DoDirective(pDirective);
			}
            else if(!pInput->IsActive()){
                // Do nothing
            }
            else {
                bool foundDecl = false;
                if(SUCCEEDED(hr)){
                    hr = CheckForMacroDeclaration(lineBuf, &foundDecl);
                }
                if(!foundDecl){

                    // keep a copy of lineBuf
                    Buffer oldLineBuf;
                    if(SUCCEEDED(hr)){
                        hr = oldLineBuf.Initialize(0);
                    }
                    if(SUCCEEDED(hr)){
                        hr = oldLineBuf.Append(lineBuf);
                    }

                    // First, define-macro-expand the line.
                    // Then check if it uses NVASM-macros.
                    // If it does, then expand them. As
                    // part of expanding the NVASM-macros,
                    // CheckForMacroUse will define-macro-expand
                    // the expanded NVASM-macro.
                   
                    if(SUCCEEDED(hr)){
				        hr = MacroExpand(lineBuf);
                    }

                    if(SUCCEEDED(hr)){
                        hr = CheckForMacroUse(lineBuf);
                    }
				    
                    if(SUCCEEDED(hr)){
                        if(strcmp(oldLineBuf.GetText(), lineBuf.GetText()) == 0){
                            // line not modified, process it
                            if(SUCCEEDED(hr)){
                                if(InEnum){
                                    // process part of an enum
                                    hr = DoEnumLine(lineBuf);
                                }
                                else{
                                    hr = CheckForEnum(lineBuf, &foundDecl);
                                    if(!foundDecl){
                                        // output if not modified 
                                        if(SUCCEEDED(hr)){
                                            hr = lineBuf.Append("\r\n");
                                        }
                                        if(SUCCEEDED(hr)){
                                            hr = Output(lineBuf);
                                        }
                                    }
                                }
                            }
                        }
                        else{
                            // modified, keep trying until no expansions can be done
                            // process one line at a time
                            char *pStart, *pEnd;
                            oldLineBuf.Clear(); // use as temp storage
                            if(SUCCEEDED(hr)){
                                hr = oldLineBuf.Append(lineBuf);
                            }
                            pEnd = oldLineBuf.GetText();
                            while(1){
                                pStart = pEnd;
                                while((*pEnd != '\0') && (*pEnd != '\n'))
                                    pEnd++;
                                if(*pEnd == '\0'){
                                    DoLine(pStart);
                                    break;
                                }
                                else{
                                    *pEnd = '\0';
                                    DoLine(pStart);
                                    pEnd++;
                                }
                            }
                        }
                    }
                }
			}
		}
		return hr;
	}

    HRESULT CheckForMacroDeclaration(Buffer& buffer, bool* pFoundDecl){
        HRESULT hr = S_OK;
        const char* p = buffer.GetText();
        Token token;
        bool foundDecl = false;
        if(SUCCEEDED(hr)){
            hr = GetToken(token, p, true);
        }
        if(SUCCEEDED(hr)){
            static const char kMacro[] = "macro";
            static const char kEndM[] = "endm";
            
            if(token.type == TOKEN_SYMBOL &&
                token.length == sizeof(kMacro)-1 &&
                strncmp(token.token, kMacro, sizeof(kMacro)-1) == 0){
                // This is a macro declaration.
                foundDecl = true;
                if(SUCCEEDED(hr)){
                    Token name;
                    hr = GetToken(name, p, true);
                    if(SUCCEEDED(hr)){
                        if(token.type == TOKEN_SYMBOL){
                            // Is this already defined?
                            if(macros.Lookup(name.token, name.length)){
                                // Already defined. Issue a warning
                                Error(false, ERROR_SYMBOLALREADYDEFINED,"This macro is already defined");
                                macros.Remove(name.token, name.length);
                            }
                            Buffer body;
                            bool dontDefineMacro = false;
                            if(SUCCEEDED(hr)){
                                hr = body.Initialize(400);
                            }
                            // After the macro name comes the argument list
                            if(SUCCEEDED(hr)){
                                hr = SkipWS(p); 
                            }
                            // Check for syntactic correctness
                            bool isLegal = false;
                            if(SUCCEEDED(hr)){
                                hr = IsLegalMacroDeclaration(p, NULL, &isLegal);
                            }
                            if(!isLegal){
                                dontDefineMacro = true;
                            }
                            if(SUCCEEDED(hr)){
                                hr = body.Append(p);
                            }
                            if(SUCCEEDED(hr)){ // Use a null to seperate the argument list from the body
                                hr = body.Append('\0');
                            }
                            // Now collect lines of input until we get an endm
                            Buffer tempBuf;
                            if(SUCCEEDED(hr)){
                                hr = tempBuf.Initialize(400);
                            }
                            bool endOfMacro = false;
                            if(SUCCEEDED(hr)){
                                bool firstLine = true;
                                Token endm;
                                do {
                                    if (pInput->IsEOF()){
                                        Error(true, ERROR_EXPECTEDENDM, "Expected endm before end of file.");
                                        break;
                                    }
                                    hr = FindLine(tempBuf);
                                    if(SUCCEEDED(hr)){
                                        const char* p = tempBuf.GetText();
                                        hr = GetToken(endm, p, true);
                                    }
                                    if(SUCCEEDED(hr)){
                                        if(endm.IsEqual(TOKEN_SYMBOL, kEndM, sizeof(kEndM)-1)){
                                            endOfMacro = true;
                                        }
                                    }
                                    if(SUCCEEDED(hr)){
                                        if(!endOfMacro){
                                            if(firstLine){
                                                firstLine = false;
                                            }
                                            else {
                                                body.Append("\n"); // Just newline, not \r\n
                                            }
                                            body.Append(tempBuf);
                                        }
                                    }
                                } while(SUCCEEDED(hr) && ! endOfMacro);
                            }
                            if(SUCCEEDED(hr) && endOfMacro && ! dontDefineMacro){
                                bool isLegal;
                                hr = IsLegalMacroDeclaration(p, body.GetText(), &isLegal);
                                if(SUCCEEDED(hr) && isLegal){
                                    hr = macros.Add(name.token, name.length, body.GetText(), body.GetUsed());
                                }
                            }
                        }
                        else {
                            Error(true,ERROR_EXPECTEDMACRONAME, "Expected macro name");
                        }
                        if(SUCCEEDED(hr)){
                            // We have parsed the macro, so output
                        }
                    }
                }
            }
        }
        
        *pFoundDecl = foundDecl;
        return hr;
    }

    
    HRESULT CheckForEnum(Buffer& buffer, bool* pFoundEnum){
        HRESULT hr;
        Token token;
        const char* p = buffer.GetText();
        hr = GetToken(token, p, true);
        if(SUCCEEDED(hr)){
            static const char kEnum[] = "enum";
            
            if(token.IsEqual(TOKEN_SYMBOL, kEnum, sizeof(kEnum)-1)){
                // This is an enum.
                InEnum = true;
                gotEnumOpen = false;
                gotEnumClose = false;
                EnumVal = -1;  // first entry defaults to 0 (enumval incremented before use)
                *pFoundEnum = true;
            }
        }
        return S_OK;
    }

    HRESULT DoEnumLine(Buffer& buffer){
        Token token;
        HRESULT hr;

        // Process each line of the enum
        const char* p;
        p = buffer.GetText();
        hr = GetToken(token, p, true);
        if(SUCCEEDED(hr)){
            if(token.IsEqual('{'))
                gotEnumOpen = true;
            else{
                if(!gotEnumOpen){
                    Error(true, ERROR_EXPECTEDOPENBRACE, "Missing open brace in enum.");
                    InEnum=false;
                    return E_FAIL;
                }
                if(token.IsEqual('}')){
                    gotEnumClose = true;
                    InEnum = false;
                }
                else{
                    // must be a symbol definition
                    // make sure we got a symbol
                    if(token.type != TOKEN_SYMBOL){
                        Error(true, ERROR_BADENUM, "Syntax error in enum.");
                        InEnum=false;
                        return E_FAIL;
                    }
                    Token nextTok;
                    hr = GetToken(nextTok, p, true);
                    if(SUCCEEDED(hr)){
                        if(nextTok.IsEqual('=')){
                            hr = GetToken(nextTok, p, true);
                            if(SUCCEEDED(hr)){
                                if(nextTok.type != TOKEN_INT){
                                    Error(true, ERROR_BADENUM, "Syntax error in enum.");
                                    InEnum=false;
                                    return E_FAIL;
                                }
                                EnumVal = atoi(nextTok.token);
                                hr = GetToken(nextTok, p, true);
                            }
                        }
                        else{
                            if(!nextTok.IsEqual(',') && (nextTok.type != TOKEN_EOS)){
                                Error(true, ERROR_BADENUM, "Syntax error in enum.");
                                InEnum=false;
                                return E_FAIL;
                            }
                            EnumVal++;
                        }
                    }
                    // add the define
                    char buf[256];
                    sprintf(buf+1, "%d", EnumVal);
                    buf[0] = '\0';
                    hr = defines.Add(token.token, token.length, buf, strlen(buf+1)+1);
                }
            }
        }
        return hr;
    }

    HRESULT IsLegalDefineArgList(const char* argList, bool* pIsLegal){
        bool isLegal = true;
        Token token;
        HRESULT hr = S_OK;
        Map args;
        hr = args.Initialize(10);
        if(*argList == '('){
            argList++;
        }
        else {
            Error(true, ERROR_MACROARGUMENTSYNTAX, "Expected '('.");
            isLegal = false;
        }
        if(SUCCEEDED(hr) && isLegal){
            do {
                hr = GetToken(token, argList, true);
                if(SUCCEEDED(hr)){
                    if(token.type == TOKEN_SYMBOL){
                        if(args.Lookup(token.token, token.length)){
                            Error(true, ERROR_MACROARGUMENTSYNTAX, "Duplicate argument name.");
                            isLegal = false;
                            break;
                        }
                        hr = args.Add(token.token, token.length, "*", 1);
                    }
                    else if(token.type == TOKEN_EOS){
                        break;
                    }
                    else if(token.IsEqual(')')){
                        break;
                    }
                    else {
                        Error(true, ERROR_MACROARGUMENTSYNTAX, "Expected argument name.");
                        isLegal = false;
                        break;
                    }
                }
                hr = GetToken(token, argList, true);
                if(SUCCEEDED(hr)){
                    if(token.IsEqual(',')){
                        // don't have to do anything
                    }
                    else if(token.IsEqual(')')){
                        break;
                    }
                    else {
                         Error(true, ERROR_MACROARGUMENTSYNTAX, "Expected ',' or ')'.");
                        isLegal = false;
                        break;
                    }
                }
            } while(SUCCEEDED(hr));
        }
        if(pIsLegal){
            *pIsLegal = isLegal;
        }
        return hr;
    }

    HRESULT IsLegalMacroDeclaration(const char* argList, const char* body, bool* pIsLegal){
        bool isLegal = true;
        Token token;
        HRESULT hr = S_OK;
        Map args;
        hr = args.Initialize(10);
        if(SUCCEEDED(hr)){
            do {
                hr = GetToken(token, argList, true);
                if(SUCCEEDED(hr)){
                    if(token.type == TOKEN_SYMBOL){
                        if(args.Lookup(token.token, token.length)){
                            Error(true, ERROR_MACROARGUMENTSYNTAX, "Duplicate argument name.");
                            isLegal = false;
                            break;
                        }
                        hr = args.Add(token.token, token.length, "*", 1);
                    }
                    else if(token.type == TOKEN_EOS){
                        break;
                    }
                    else {
                        Error(true, ERROR_MACROARGUMENTSYNTAX, "Expected argument name.");
                        isLegal = false;
                        break;
                    }
                }
                hr = GetToken(token, argList, true);
                if(SUCCEEDED(hr)){
                    if(token.IsEqual(',')){
                        // don't have to do anything
                    }
                    else if(token.type == TOKEN_EOS){
                        break;
                    }
                    else {
                        Error(true, ERROR_MACROARGUMENTSYNTAX, "Expected comma.");
                        isLegal = false;
                        break;
                    }
                }
            } while(SUCCEEDED(hr));
        }
        
        // Now check the body to see that it has legal parmeters
        if(isLegal && body){
            const char* bodyp = body;
            do {
                Token bodyToken;
                hr = GetToken(bodyToken, bodyp, false);
                if(SUCCEEDED(hr)){
                    if(bodyToken.type == TOKEN_EOS){
                        break;
                    }
                    if(bodyToken.IsEqual('%')){
                        // It's a macro argument use.
                        hr = GetToken(bodyToken, bodyp, false);
                        if(SUCCEEDED(hr)){
                            if(bodyToken.type != TOKEN_SYMBOL){
                                Error(true,ERROR_EXPECTEDMACROPARAMETER, "Expected macro parameter.");
                                isLegal = false;
                                continue;
                            }
                            bool isIncDec = false;
                            if(bodyToken.IsEqual(TOKEN_SYMBOL, "inc", 3)){
                                isIncDec = true;
                            }
                            else if(bodyToken.IsEqual(TOKEN_SYMBOL, "dec", 3)){
                                isIncDec = true;
                            }
                            if(isIncDec){
                                bool isLegalIncDec = false;
                                hr = CheckLegalIncDecUse(args, bodyp, &isLegalIncDec);
                                if(SUCCEEDED(hr)){
                                    if(!isLegalIncDec){
                                        isLegal = false;
                                    }
                                }
                            }
                            else{
                                const char* val = args.Lookup(bodyToken.token, bodyToken.length);
                                if(val){
                                    // Nothing to do
                                }
                                else {
                                    Error(true,ERROR_EXPECTEDMACROPARAMETER, "Expected macro parameter name.");
                                    isLegal = false;
                                }
                            }
                        }
                    }
                    else {
                        // nothing to do
                    }
                }
            } while(SUCCEEDED(hr) && isLegal);
        }
        
        if(SUCCEEDED(hr)){
            *pIsLegal = isLegal;
        }
        return hr;
    }

    HRESULT CheckLegalIncDecUse(Map& args, const char*& bodyp, bool* pIsLegal){
        HRESULT hr = S_OK;
        // Syntax is '(' lots of stuff, nested correctly,  ')'
        Token token;
        bool isLegal = true;
        hr = GetIncDecArgList(token,bodyp);
        if(SUCCEEDED(hr)){
            if(token.type == TOKEN_SYMBOL){
                // now search through the contents, checking that all
                // the %foo symbols are known.
                do {
                    const char* p = token.token;
                    Token element;
                    hr = GetToken(element, p, true);
                    if(SUCCEEDED(hr)){
                        if(element.type == TOKEN_EOS){
                            break;
                        }
                        else if (element.IsEqual('%')){
                            hr = GetToken(token,bodyp, true);
                            if(SUCCEEDED(hr)){
                                if(token.type != TOKEN_SYMBOL){
                                    Error(true,ERROR_MACROARGUMENTSYNTAX,"Expected macro parameter");
                                    isLegal = false;
                                }
                                else {
                                    if(args.Lookup(token.token, token.length) == NULL){
                                        Error(true,ERROR_MACROARGUMENTSYNTAX,"Expected macro parameter");
                                        isLegal = false;
                                    }
                                }
                            }
                            
                        }
                        else {
                            // nothing to do
                        }
                    }
                } while(SUCCEEDED(hr) && isLegal);
            }
            else {
                Error(true,ERROR_MACROARGUMENTSYNTAX,"Expected an argument enclosed in parentheses");
                isLegal = false;
            }
        }
        if(pIsLegal){
            *pIsLegal = isLegal;
        }
        return hr;
    }

    HRESULT CheckForMacroUse(Buffer& buffer){
        HRESULT hr = S_OK;
        const char* p = buffer.GetText();
        Token token;
        if(SUCCEEDED(hr)){
            hr = GetToken(token, p, true);
        }
        if(SUCCEEDED(hr)){
            if(token.type == TOKEN_SYMBOL){
                const char* body = macros.Lookup(token.token, token.length);
                if(body){
                    Map args;
                    hr = args.Initialize(10);
                    const char* argp = body;
                    const char* bodyp = argp + strlen(argp) + 1;
                    bool argParseOK = true;
                    if(SUCCEEDED(hr)){
                        do {
                            Token argToken;
                            hr = GetToken(argToken, argp, true);
                            if(SUCCEEDED(hr)){
                                hr = GetMacroArgToken(token, p);
                            }
                            if(SUCCEEDED(hr)){
                                if(argToken.type == TOKEN_EOS && token.type != TOKEN_EOS){
                                    Error(true, ERROR_MACROARGUMENTSYNTAX, "Too many arguments");
                                    argParseOK = false;
                                }
                                else if(argToken.type != TOKEN_EOS && token.type == TOKEN_EOS){
                                    Error(true, ERROR_MACROARGUMENTSYNTAX, "Too few arguments");
                                    argParseOK = false;
                                }
                                else if(argToken.type == TOKEN_OTHER){
                                    if(token.type == TOKEN_OTHER
                                        && token.type == TOKEN_OTHER && argToken.token[0] == token.token[0]){
                                        // Good, we matched a comma
                                    }
                                    else {
                                        Error(true, ERROR_MACROARGUMENTSYNTAX, "Expected a comma.");
                                        argParseOK = false;
                                    }
                                }
                                else if(argToken.type == TOKEN_EOS && token.type == TOKEN_EOS){
                                    break; // We're done with parsing the args
                                }
                                else if(argToken.type == TOKEN_SYMBOL && token.type == TOKEN_SYMBOL){
                                    hr = args.Add(argToken.token, argToken.length, token.token, token.length);
                                }
                                else {
                                    Error(true, ERROR_MACROARGUMENTSYNTAX, "Unexpected symbol.");
                                    argParseOK = false;
                                }
                            }
                        } while(SUCCEEDED(hr) && argParseOK);
                    }
                    if(SUCCEEDED(hr)){
                        if(argParseOK){
                            buffer.Clear();
                            hr = ProcessMacroBody(buffer, bodyp, args);
                            if(SUCCEEDED(hr)){
				                hr = MacroExpand(lineBuf);
                            }
                        }
                    }
                }
            }
        }
        return hr;
    }

    HRESULT ProcessMacroBody(Buffer& buffer, const char*& bodyp, Map& args){
        HRESULT hr = S_OK;
        // Output the macro body, substituting symbols as we go
        do {
            Token bodyToken;
            hr = GetToken(bodyToken, bodyp, false);
            if(SUCCEEDED(hr)){
                if(bodyToken.type == TOKEN_EOS){
                    break;
                }
                if(bodyToken.IsEqual('%')){
                    // It's a macro argument use.
                    hr = GetToken(bodyToken, bodyp, false);
                    if(SUCCEEDED(hr)){
                        if(bodyToken.type != TOKEN_SYMBOL){
                            Error(true,ERROR_EXPECTEDMACROPARAMETER, "Expected macro parameter.");
                            buffer.Append("%");
                            buffer.Append(bodyToken.token, bodyToken.length);
                            continue;
                        }
                        if(bodyToken.IsEqual(TOKEN_SYMBOL, "inc", 3)){
                            hr = ProcessIncDec(buffer, bodyp, args, 1);
                        }
                        else if(bodyToken.IsEqual(TOKEN_SYMBOL, "dec", 3)){
                            hr = ProcessIncDec(buffer, bodyp, args, -1);
                        }
                        else{
                            const char* val = args.Lookup(bodyToken.token, bodyToken.length);
                            if(val){
                                buffer.Append(val);
                            }
                            else {
                                Error(true,ERROR_EXPECTEDMACROPARAMETER, "Expected macro parameter.");
                            }
                        }
                    }
                }
                else {
                    buffer.Append(bodyToken.token, bodyToken.length);
                }
            }
        } while(SUCCEEDED(hr));
        return hr;
    }

    HRESULT ProcessIncDec(Buffer& buffer, const char*& bodyp, Map& args, int sign){
        HRESULT hr = S_OK;
        Token argList;
        hr = GetIncDecArgList(argList, bodyp);
        bool goodUse = true;
        Buffer incDecBuf;
        if(SUCCEEDED(hr)){
            hr = incDecBuf.Initialize(100);
        }
        if(SUCCEEDED(hr)){
            if(argList.type != TOKEN_SYMBOL){
                Error(true, ERROR_MACROARGUMENTSYNTAX, "Expected parenthesized expression.");
                goodUse = false;
            }
            else {
                Buffer incDecInputBuf;
                hr = incDecInputBuf.Initialize(100);
                if(SUCCEEDED(hr)){
                    hr = incDecInputBuf.Append(argList.token, argList.length);
                }
                if(SUCCEEDED(hr)){
                    const char* inP = incDecInputBuf.GetText();
                    hr = ProcessMacroBody(incDecBuf, inP, args);
                }
            }
        }
        if(SUCCEEDED(hr) && goodUse){
            // Now we inc/dec

            const char* incDecBufP = incDecBuf.GetText(); 
            // If this is an explicit register number, increment it or decrement it
            Token prefix;
            Token op;
            Token number;
            hr = FindNumber(prefix, op, number, incDecBufP);
            if(SUCCEEDED(hr)){
                if(number.type == TOKEN_SYMBOL && number.length > 0){
                    // OK, we found a number.
                    int num = atoi(number.token);
                    char opc = op.token ? *op.token : '+';
                    // Normalize: subtracting becomes adding.
                    if(opc == '-'){
                        opc = '+';
                        num = -num;
                    }

                    int newNum = num + sign;

                    // At this point opc is '+'
                    // Normalize: adding a negative number becomes subtracting a positive number.
                    if(newNum < 0){
                        opc = '-';
                        newNum = -newNum;
                    }

                    char newNumber[12]; // enough space for -2000000000 
                    _itoa(newNum, newNumber, 10);
                    if(SUCCEEDED(hr)){
                        hr = buffer.Append(prefix.token, prefix.length); // Stuff before number
                    }

                    if(SUCCEEDED(hr)){
                        if(op.token ) { // the a0.x +/- # case
                            if ( newNum == 0) {
                            // output nothing
                            }
                            else {
                                hr = buffer.Append(opc);
                                if(SUCCEEDED(hr)){
                                    buffer.Append(newNumber);
                                }
                            }
                        }
                        else {
                            // The c# case
                            if( opc == '-'){
                                buffer.Append(opc);
                            }
                            buffer.Append(newNumber);
                        }
                    }

                    if(SUCCEEDED(hr)){
                        hr = buffer.Append(incDecBufP);
                    }
                }
                else {
                    // Wacky innermost constant mode.
                    const char* incDecBufStart = incDecBuf.GetText();
                    incDecBufP = incDecBufStart;
                    hr = FindInnermostItem(number, incDecBufP);
                    if(SUCCEEDED(hr)){
                        if(number.type == TOKEN_SYMBOL){
                            // OK, we found an innermost item.
                            hr = buffer.Append(incDecBufStart,incDecBufP - incDecBufStart); // Stuff before end of inner
                            if(SUCCEEDED(hr)){
                                hr = buffer.Append(sign >= 0 ? '+' : '-');
                            }
                            if(SUCCEEDED(hr)){
                                hr = buffer.Append('1');
                            }
                            if(SUCCEEDED(hr)){
                                hr = buffer.Append(incDecBufP);
                            }
                        }
                        else {
                            // Give up
                            Error(true, ERROR_MACROARGUMENTSYNTAX, "Couldn't find object to increment/decrement");
                            goodUse = false;
                        }
                    }
                }
            }
        }
        return S_OK;
    }
    
    HRESULT FindInnermostItem(Token& token, const char*& p){
        HRESULT hr = S_OK;
        char c;
        bool foundInner = false;
        while((c = *p) != '\0'){
            if(c == '(' || c == '['){
                token.token = p+1;
            }
            else if(c == ')' || c == ']'){
                token.type = TOKEN_SYMBOL;
                token.length = (p-token.token)-1;
                foundInner = true;
                break;
            }
            p++;
        }
        if(!foundInner){
            token.token = 0;
            token.length = 0;
            token.type = TOKEN_OTHER;
        }
        return hr;
    }

    HRESULT FindNumber(Token& prefix, Token& op, Token& number, const char*& p){
        HRESULT hr = S_OK;

        // Looking for <prefix> {+ | -} <num> <text>
        // Where text can contain a0.x
        // and num can start with a -

        char c;
        bool foundNum = false;
        op.type = 0;
        op.token = 0;
        op.length = 0;

        prefix.type = 0;
        prefix.token = p;
        prefix.length = 0;

        number.type = 0;
        number.token = 0;
        number.length = 0;

        enum State { ePrefix, ePlus, eMinus, eNumber, eDone };
        State state = ePrefix;
        
        bool foundA0x = false;

        while((c = *p) != '\0' && state != eDone){
            switch(state) {
            case ePrefix:
                {
                    if(c == 'a' && p[1] == '0' && p[2] == '.' && p[3] == 'x'){
                        // Skip over a0.x string
                        p += 3; // depends upon the p++ below.
                        foundA0x = true;
                    }
                    else if ( c == '+' && foundA0x ){
                        state = ePlus;
                        op.type = TOKEN_OTHER;
                        op.token = p;
                        op.length = 1;
                    }
                    else if ( c == '-'){
                        if(!foundA0x){
                            state = eNumber;
                            number.type = TOKEN_SYMBOL;
                            number.token = p;
                            number.length = 1;
                        }
                        else {
                            state = eMinus;
                            op.type = TOKEN_OTHER;
                            op.token = p;
                            op.length = 1;
                        }
                    }
                    else if ( c >= '0' && c <= '9' ){
                        state = eNumber;
                        number.type = TOKEN_SYMBOL;
                        number.token = p;
                        number.length = 1;
                    }
                    p++;
                    if(state == ePrefix){
                        prefix.length = p - prefix.token;
                    }
                }
                break;
            case eMinus:
            case ePlus:
                {
                    if(c == ' ' || c == '\t' || c == '\x1a'){
                        // do nothing
                    }
                    else if ( c == '-' || c >= '0' && c <= '9' ){
                        state = eNumber;
                        number.type = TOKEN_SYMBOL;
                        number.token = p;
                        number.length = 1;
                    }
                    else {
                        state = ePrefix;
                    }
                    p++;
                    if(state == ePrefix){
                        prefix.length = p - prefix.token;
                    }
                }
                break;
            case eNumber:
                {
                    if(c >= '0' && c <= '9'){
                       p++;
                       number.length++;
                    }
                    else {
                        state = eDone;
                    }
                }
                break;
            }
        }
        return hr;
    }

    const char* FindDirective(Buffer& buffer){
        const char* s = buffer.GetText();
        const char* sEnd = buffer.GetTextEnd();
        for(;s < sEnd; s++){
            if( *s == '#'){
                return s+1;
            }
            if( *s == ' ' || *s == '\t' || *s == '\x1a'){
                continue;
            }
            return NULL;
        }
        return NULL;
    }

	HRESULT DoDirective(const char* pDirective){
		HRESULT hr = S_OK;
		// Which directive is it?
		const char kInclude[] = "include";
		const char kDefine[] = "define";
        const char kPragma[] = "pragma";
        const char kUndef[] = "undef";
        const char kError[] = "error";
        const char kIf[] = "if";
        const char kIfdef[] = "ifdef";
        const char kIfndef[] = "ifndef";
        const char kElse[] = "else";
        const char kElif[] = "elif";
        const char kEndif[] = "endif";

        Token token;
        hr = GetToken(token, pDirective, true);
        if(SUCCEEDED(hr)){
            if(token.type != TOKEN_SYMBOL){
                Error(true, ERROR_UNKNOWNPREPROCESSORDIRECTIVE,"Unknown preprocessor directive");
 			    SETERROR(hr, E_FAIL);
		    }
        }

        if(SUCCEEDED(hr)){
            if( pInput->IsActive() ) {
		        if(token.length == sizeof(kInclude)-1
                    && strncmp(token.token, kInclude, sizeof(kInclude)-1) == 0){
			        hr = DoInclude(pDirective);
		        }
		        else if(token.length == sizeof(kDefine)-1
                    && strncmp(token.token, kDefine, sizeof(kDefine)-1) == 0){
			        hr = DoDefine(pDirective);
		        }
		        else if(token.length == sizeof(kPragma)-1
                    && strncmp(token.token, kPragma, sizeof(kPragma)-1) == 0){
			        hr = DoPragma(pDirective);
		        }
		        else if(token.length == sizeof(kUndef)-1
                    && strncmp(token.token, kUndef, sizeof(kUndef)-1) == 0){
			        hr = DoUndef(pDirective);
		        }
		        else if(token.length == sizeof(kError)-1
                    && strncmp(token.token, kError, sizeof(kError)-1) == 0){
			        hr = DoError(pDirective);
		        }
		        else if(token.length == sizeof(kIf)-1
                    && strncmp(token.token, kIf, sizeof(kIf)-1) == 0){
			        hr = DoIf(pDirective);
		        }
		        else if(token.length == sizeof(kIfdef)-1
                    && strncmp(token.token, kIfdef, sizeof(kIfdef)-1) == 0){
			        hr = DoIfdef(pDirective, true);
		        }
		        else if(token.length == sizeof(kIfndef)-1
                    && strncmp(token.token, kIfndef, sizeof(kIfndef)-1) == 0){
			        hr = DoIfdef(pDirective, false);
		        }
		        else if(token.length == sizeof(kElif)-1
                    && strncmp(token.token, kElif, sizeof(kElif)-1) == 0){
			        hr = DoElif(pDirective);
		        }
		        else if(token.length == sizeof(kElse)-1
                    && strncmp(token.token, kElse, sizeof(kElse)-1) == 0){
			        hr = DoElse(pDirective);
		        }
		        else if(token.length == sizeof(kEndif)-1
                    && strncmp(token.token, kEndif, sizeof(kEndif)-1) == 0){
			        hr = DoEndif(pDirective);
		        }
		        else {
			        // Unknown directive
			        Error(true, ERROR_UNKNOWNPREPROCESSORDIRECTIVE,"Unknown preprocessor directive"); 
			        SETERROR(hr, E_FAIL);
		        }
            }
            else {
                // inactive - just handle ifdefs
		        if(token.length == sizeof(kIf)-1
                    && strncmp(token.token, kIf, sizeof(kIf)-1) == 0){
			        hr = DoIf(pDirective);
		        }
		        else if(token.length == sizeof(kIfdef)-1
                    && strncmp(token.token, kIfdef, sizeof(kIfdef)-1) == 0){
			        hr = DoIfdef(pDirective, true);
		        }
		        else if(token.length == sizeof(kIfndef)-1
                    && strncmp(token.token, kIfndef, sizeof(kIfndef)-1) == 0){
			        hr = DoIfdef(pDirective, false);
		        }
		        else if(token.length == sizeof(kElif)-1
                    && strncmp(token.token, kElif, sizeof(kElif)-1) == 0){
			        hr = DoElif(pDirective);
		        }
		        else if(token.length == sizeof(kElse)-1
                    && strncmp(token.token, kElse, sizeof(kElse)-1) == 0){
			        hr = DoElse(pDirective);
		        }
		        else if(token.length == sizeof(kEndif)-1
                    && strncmp(token.token, kEndif, sizeof(kEndif)-1) == 0){
			        hr = DoEndif(pDirective);
		        }
            }
        }
		return hr;
	}

    HRESULT DoIf(const char* args){
        HRESULT hr = S_OK;
        if(pInput->IsActive()){
            Error(true, ERROR_DONTHANDLEIF, "#if is not supported.");
        }
        else {
            // allow this if it's #ifdefed out.
            hr = pInput->PushActiveLevel(false);
        }
        return hr;
    }

    HRESULT DoElif(const char* args){
        HRESULT hr = S_OK;
        if(pInput->IsActive()){
            Error(true, ERROR_DONTHANDLEIF, "#elif is not supported.");
        }
        else {
            // allow this if it's #ifdefed out. Doesn't change state.
        }
        return hr;
    }

    HRESULT DoIfdef(const char* args, bool ifdef /* true == ifdef, false == ifndef */){
		HRESULT hr = S_OK;
  		const char* p = args;
		Token token;
        bool defined = false;
		if(SUCCEEDED(hr)){
			hr = GetToken(token, p, true);
		}
		if(SUCCEEDED(hr)){
			if(token.type != TOKEN_SYMBOL){
				// Expected a symbol
				Error(true, ERROR_SYMBOLEXPECTED,"symbol expected");
				SETERROR(hr, E_FAIL);
			}
		}
		if(SUCCEEDED(hr)){
			if(defines.Lookup(token.token, token.length)){
				defined = true;
			}
		}
		if(SUCCEEDED(hr)){
            hr = pInput->PushActiveLevel(defined == ifdef);
            if(FAILED(hr)){
                Error(true, ERROR_TOOMANYLEVELSOFIF, "Too many nested levels of #ifdef/#ifndef in this file.");
            }
        }
 		return hr;
    }

    HRESULT DoElse(const char* args){
        HRESULT hr = S_OK;
        // ANSI compatability weenies would check to make sure that there are no tokens here.
        hr = pInput->ElseActiveLevel();
        if(FAILED(hr)){
            Error(true, ERROR_UNEXPECTEDELSE, "Unexpected #else");
        }
        return hr;
    }

    HRESULT DoEndif(const char* args){
        HRESULT hr = S_OK;
        // ANSI compatability weenies would check to make sure that there are no tokens here.
        hr = pInput->PopActiveLevel();
        if(FAILED(hr)){
            Error(true, ERROR_UNEXPECTEDENDIF, "Unexpected #endif");
        }
        return hr;
    }

    HRESULT DoError(const char* errorArgs){
        Error(true, ERROR_ERRORDIRECTIVE, errorArgs);
 		return E_FAIL;
    }

    HRESULT DoPragma(const char* pragmaArgs){
        // Just macro-expand and write out as-is
		HRESULT hr = MacroExpand(lineBuf);
		if(SUCCEEDED(hr)){
 			hr = Output(lineBuf);
		}
		return hr;
    }

	HRESULT DoDefine(const char* defineArgs){
		HRESULT hr = S_OK;
		const char* p = defineArgs;

		// Looking for a token, some white space, and then the rest of the line
		Token name;
		if(SUCCEEDED(hr)){
			hr = GetToken(name, p, true);
		}
		if(SUCCEEDED(hr)){
			if(name.type != TOKEN_SYMBOL){
				// Expected a symbol
				Error(true, ERROR_SYMBOLEXPECTED,"symbol expected");
				SETERROR(hr, E_FAIL);
			}
		}
		if(SUCCEEDED(hr)){
			// Is this already defined?
			if(defines.Lookup(name.token, name.length)){
				// Already defined. Just a warning (or should it be an error?)
				Error(false, ERROR_SYMBOLALREADYDEFINED,"This symbol is already defined");
				hr = DoUndef2(name.token, name.length);
			}
		}

        Buffer body;
        if(SUCCEEDED(hr)){
            hr = body.Initialize(400);
        }
		if(*p == '('){
			// #define with arguments
            const char* argListStart = p;
            Token t;
            do {
                hr = GetToken(t, p, true);
            } while ( SUCCEEDED(hr) &&
                ! (t.IsEqual(')') || t.type == TOKEN_EOS));
            if(SUCCEEDED(hr)){
                if(t.type == TOKEN_EOS){
                    Error(true,ERROR_EXPECTED_CLOSE_PAREN, "Expected ')'.");
				    SETERROR(hr, E_FAIL);
                }
            }
            if(SUCCEEDED(hr)){
                hr = body.Append(argListStart, p - argListStart);
            }
            if(SUCCEEDED(hr)){ // Use a null to seperate the argument list from the body
                hr = body.Append('\0');
            }
            // Append the rest of the macro
            if(SUCCEEDED(hr)){
                hr = SkipWS(p);
            }
            if(SUCCEEDED(hr)){
                hr = body.Append(p);
            }
            if(SUCCEEDED(hr)){
                bool isLegal;
                hr = IsLegalDefineArgList(body.GetText(), &isLegal);
                if(SUCCEEDED(hr) && isLegal){
                    hr = defines.Add(name.token, name.length, body.GetText(), body.GetUsed());
                }
            }
 		}
        else {
            // ordinary #define without arguments
            if(SUCCEEDED(hr)){
                hr = body.Append('\0'); // means no args
            }
            if(SUCCEEDED(hr)){
                hr = SkipWS(p);
            }
            if(SUCCEEDED(hr)){
                hr = body.Append(p);
            }
		    if(SUCCEEDED(hr)){
			    hr = defines.Add(name.token, name.length, body.GetText(), body.GetUsed());
		    }
        }
		return hr;
	}

	HRESULT DoUndef(const char* defineArgs){
		// Looking for a token
		const char* p = defineArgs;
		HRESULT hr = S_OK;
		Token key;
		if(SUCCEEDED(hr)){
			hr = GetToken(key, p, true);
		}
		if(SUCCEEDED(hr)){
			if(key.type != TOKEN_SYMBOL){
				// Expected a symbol
				Error(true, ERROR_SYMBOLEXPECTED,"symbol expected");
				SETERROR(hr, E_FAIL);
			}
		}
		if(SUCCEEDED(hr)){
            hr = DoUndef2(key.token, key.length);
		}
        return hr;
    }

    HRESULT DoUndef2(const char* key, DWORD keySize){
        defines.Remove(key, keySize);
        return S_OK;
    }

	static HRESULT SkipWS(const char*& p){
		HRESULT hr = S_OK;
		char c;
		while((c = *p) != '\0'){
            if(c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\x1a'){
				p++;
			}
			else {
				break;
			}
		}
		return hr;
	}

	// Returns start and length of token.
	// Skips any preceeding white space

	static HRESULT GetToken(Token& token, const char*& p, bool skipWS){
		HRESULT hr = S_OK;
		if(skipWS){
			hr = SkipWS(p);
		}
		if(SUCCEEDED(hr)){
			token.token = p;
			// Find end of token
			char c = *p;
			if(c != '\0') {
				if ((c >= 'A' && c <= 'Z')
					|| (c >= 'a' && c <= 'z')
					|| (c == '_'))
				{
					// It's a symbol
					token.type = TOKEN_SYMBOL;
					while((c = *p) != '\0'){
						if((c >= 'A' && c <= 'Z')
							|| (c >= 'a' && c <= 'z')
							|| (c >= '0' && c <= '9')
							|| (c == '_'))
						{
							p++;
						}
						else {
							break;
						}
					}
				}
                else if ( c == ' ' || c == '\t' || c == '\x1a'){
					token.type = TOKEN_WS;
					while((c = *p) != '\0'){
                        if(c == ' ' || c == '\t' || c == '\x1a')
						{
							p++;
						}
						else {
							break;
						}
					}
				}
                else if ((c == '-') ||
                         (c == '+') ||
                         ((c >= '0') && (c <= '9')))
                {
                    // It's an integer
                    p++;
                    token.type = TOKEN_INT;
					while((c = *p) != '\0'){
						if((c >= '0' && c <= '9'))
							p++;
						else
							break;
					}
                }
                else {
					token.type = TOKEN_OTHER;
					p++; // Everything else is just a single character
				}
			}
            else {
                token.type = TOKEN_EOS;
            }
		}
		token.length = p - token.token;
		return hr;
	}

    static HRESULT GetDefineMacroArgToken(Token& token, const char*& p){
		HRESULT hr = S_OK;
		hr = SkipWS(p);
		if(SUCCEEDED(hr)){
			token.token = p;
			// Find end of token
			char c = *p;
			if(c != '\0') {
				if (c == ',' || c == ')')
				{
                    token.type = TOKEN_OTHER;
                    p++;
                }
                else {
					// It's a symbol
					token.type = TOKEN_SYMBOL;
                    int nestingLevel = 0;
					while((c = *p) != '\0' && (nestingLevel > 0 || !(c == ',' || c == ')'))){
                        switch(c){
                        case '(':
                            nestingLevel++;
                            break;
                        case ')':
                            nestingLevel--;
                            break;
                        default:
                            break;
                        }
                        p++;
                    }
				}
			}
            else {
                token.type = TOKEN_EOS;
            }
		}
		token.length = p - token.token;
		return hr;
	}

    static HRESULT GetMacroArgToken(Token& token, const char*& p){
		HRESULT hr = S_OK;
		hr = SkipWS(p);
		if(SUCCEEDED(hr)){
			token.token = p;
			// Find end of token
			char c = *p;
			if(c != '\0') {
				if (c == ',')
				{
                    token.type = TOKEN_OTHER;
                    p++;
                }
                else {
					// It's a symbol
					token.type = TOKEN_SYMBOL;
                    int nestingLevel = 0;
					while((c = *p) != '\0' && (nestingLevel > 0 || c != ',')){
                        switch(c){
                        case '(':
                            nestingLevel++;
                            break;
                        case ')':
                            nestingLevel--;
                            break;
                        default:
                            break;
                        }
                        p++;
                    }
				}
			}
            else {
                token.type = TOKEN_EOS;
            }
		}
		token.length = p - token.token;
		return hr;
	}

    static HRESULT GetIncDecArgList(Token& token, const char*& p){
        HRESULT hr = S_OK;
        hr = SkipWS(p);
        if(SUCCEEDED(hr)){
            token.token = p;
            // Find end of token
            char c = *p;
            if(c != '\0') {
                if (c == '('){
                    // It's a symbol
                    token.type = TOKEN_SYMBOL;
                    token.token = ++p;
                    int nestingLevel = 1;
                    while((c = *p) != '\0'){
                        switch(c){
                        case '(':
                            nestingLevel++;
                            break;
                        case ')':
                            nestingLevel--;
                            break;
                        default:
                            break;
                        }
                        if(nestingLevel == 0){
                            token.length = p - token.token;
                            p++; // eat closing paren too.
                            break;
                        }
                        p++;
                    }
                    if(nestingLevel){
                        token.type = TOKEN_OTHER;
                        token.length = 0;
                        token.token = 0;
                    }
                }
                else {
                    {
                        token.type = TOKEN_OTHER;
                        token.length = 0;
                        token.token = 0;
                    }
                }
            }
            else {
                token.type = TOKEN_EOS;
                token.length = 0;
                token.token = 0;
            }
        }
        return hr;
    }

	HRESULT DoInclude(const char* includeArgs0){
		HRESULT hr = S_OK;
		if(currentIncludeLevel >= MAX_INCLUDE_LEVELS-1){
			// Nested too deeply
			Error(true, ERROR_TOOMANYLEVELSOFINCLUDE,"Too many levels of include directives. Maximum of %d levels allowed.", MAX_INCLUDE_LEVELS);
			SETERROR(hr,E_FAIL);
		}

		if(SUCCEEDED(hr) && ! pResolver ) {
			// No file factory, so can't include anything.
			Error(true, ERROR_MISSINGRESOLVER,"Can't process #include because no resolver object was supplied to preprocessor function");
			SETERROR(hr,E_FAIL);
		}

		// Macroexpand the arguments

		Buffer argBuf;
		const char* includeArgs = 0;

		DWORD argLength = strlen(includeArgs0);
		hr = argBuf.Initialize(argLength);
		if(SUCCEEDED(hr)){
			hr = argBuf.Append(includeArgs0,argLength);
		}
		if(SUCCEEDED(hr)){
			bAngleBracketsAreQuotes = true;
			hr = MacroExpand(argBuf);
			bAngleBracketsAreQuotes = false;
		}
		if(SUCCEEDED(hr)){
			includeArgs = argBuf.GetText();
		}

		char filePath[MAX_PATH];
		BOOL isSystem;
		if(SUCCEEDED(hr)){
			hr = GetFilePath(includeArgs,filePath, sizeof(filePath), &isSystem);
		}
		if(SUCCEEDED(hr)){
			XGBuffer* pIncludeFile;
			char resolvedPath[MAX_PATH];
			hr = (*pResolver)(pResolverUserData, isSystem, pInput->m_fileName, filePath,
				resolvedPath, sizeof(resolvedPath), &pIncludeFile);
			if(SUCCEEDED(hr)){
				currentIncludeLevel++;
				pInput++;
				hr = pInput->Initialize(pIncludeFile, resolvedPath);
				pIncludeFile->Release(); // pInput is holding onto it now.
			}
			else {
				Error(true, ERROR_CANTOPENFILE, "Couldn't open include file.");
			}
		}
		return hr;
	}

	HRESULT GetFilePath(const char* includeArgs, char* filePath, DWORD filePathLength,
		BOOL* isSystem){
		// skip white space, looking for '"' or "<"
		const char* s = includeArgs;
		const char* filePathStart = 0;
		char c;
		while((c = *s++)){
            if(c == ' ' || c == '\t' || c == '\x1a'){
				continue;
			}
			else if(c == '"'){
				// Match double quotes
				*isSystem = FALSE;
				filePathStart = s;
				while((c = *s++)){
					if(c == '"'){
						goto maybeGood;
					}
				}
			}
			else if(c == '<'){
				*isSystem = TRUE;
				// Match angle brackets
				filePathStart = s;
				while((c = *s++)){
					if(c == '>'){
						goto maybeGood;
					}
				}
			}
			else {
				// Bad character, expected '<' or '"'
				Error(true, ERROR_FILENAMENOTQUOTED,"File names have to be quoted with <...> or \"...\"");
				return E_FAIL;
			}
		}
		Error(true, ERROR_FILENAMEMISSING,"Expected file name");
		return E_FAIL;

maybeGood:
		DWORD length = s - filePathStart;
		if(length > filePathLength){
			// Too long.
			Error(true, ERROR_INCLUDENAMETOOLONG,"Include file name too long.");
			return E_FAIL;
		}
		// OK
		memcpy(filePath,filePathStart,length-1);
		filePath[length-1] = '\0';
		return S_OK;
	}

	HRESULT MacroExpand(Buffer& b){
        HRESULT hr = S_OK;

        Map alreadyUsedMap;

        if(SUCCEEDED(hr)){
            hr = alreadyUsedMap.Initialize(10);
        }
        if(SUCCEEDED(hr)){
            hr = MacroExpand(b, alreadyUsedMap);
        }
        return hr;
     }

    HRESULT MacroExpand(Buffer& buf, Map& alreadyUsed){
        HRESULT hr = S_OK;
        Buffer out;
        if(SUCCEEDED(hr)){
            hr = out.Initialize(buf.GetUsed());
        }
        const char* in = buf.GetText();
        const char* inEnd = in + strlen(in);
        Buffer argExpandBuf;
        if(SUCCEEDED(hr)){
            hr = argExpandBuf.Initialize(10);
        }
		enum QuoteStateMachine {
			eNormal,
			eInsideQuotedString,
			eBackslash
		};
		QuoteStateMachine state = eNormal;
		char endQuote;
        while(SUCCEEDED(hr) && in < inEnd){
            Token token;
            hr = GetToken(token, in, false);
            if ( SUCCEEDED(hr)){
				switch(state){
				case eNormal:
					{
						if(token.type == TOKEN_SYMBOL){
							const char* value = defines.Lookup(token.token, token.length);
							if(value){
								if(! alreadyUsed.Lookup(token.token, token.length)){
									// It's a macro. Expand it here
									alreadyUsed.Add(token.token, token.length,"*",1);
									argExpandBuf.Clear();
									if(value[0] == '\0'){
										// A #define macro without arguments
										hr = argExpandBuf.Append(value+1);
									}
									else {
										hr = ExpandDefineMacroWithArgs(in, argExpandBuf, value);
									}
									if(SUCCEEDED(hr)){
										hr = MacroExpand(argExpandBuf,alreadyUsed);
									}
									alreadyUsed.Remove(token.token, token.length);
									if(SUCCEEDED(hr)){
										hr = out.Append(argExpandBuf);
									}
									if(FAILED(hr)){
										// Expanded line is too big for buffer.
										Error(true, ERROR_EXPANDEDMACROTOOLONG,"Expanded macro too long.");
									}
									continue;
								}
							}
						}
						else if(token.IsEqual('\'')){
							state = eInsideQuotedString;
							endQuote = '\'';
						}
						else if(token.IsEqual('"')){
							state = eInsideQuotedString;
							endQuote = '\"';
						}
						else if(bAngleBracketsAreQuotes &&
							token.IsEqual('<')){
							state = eInsideQuotedString;
							endQuote = '>';
						}
					}
					break;
				case eInsideQuotedString:
					{
						if(token.IsEqual('\\')){
							state = eBackslash;
						}
						else if(token.IsEqual(endQuote)){
							state = eNormal;
							endQuote = 0;
						}
					}
					break;
				case eBackslash:
					{
						state = eInsideQuotedString;
					}
					break;
				}
			}
			if(SUCCEEDED(hr)){
				// copy token to output
				hr = out.Append(token.token, token.length);
				if(FAILED(hr)) {
					// Expanded line is too big for buffer.
					Error(true, ERROR_EXPANDEDMACROTOOLONG,"Expanded macro too long.");
				}
				continue;
			}
		}
		if(SUCCEEDED(hr)){
			buf.Clear();
			buf.Append(out);
		}
		return hr;
	}

    HRESULT ExpandDefineMacroWithArgs(const char*& p, Buffer& buffer, const char* body){
        HRESULT hr = S_OK;
        // Match ( foo, bar, baz ) against ( sdfsdfs , sdfsdfs, sdfsdfs )
        Map args;
        hr = args.Initialize(10);
        const char* argp = body;
        const char* bodyp = argp + strlen(argp) + 1;
        bool argParseOK = true;
        bool bLookingForOpenParen = true;
        bool bMacroArgsComplete = false;
        if(SUCCEEDED(hr)){
            do {
                Token argToken; // from argument list
                Token token; // from input
                hr = GetToken(argToken, argp, true);
                if(SUCCEEDED(hr)){
                    if(bLookingForOpenParen){
                        // Handle first token, which is an open paren,
                        // specially. Otherwise GetMacroArgToken
                        // will grab the whole argument list as a single
                        // token.
                        bLookingForOpenParen = false;
                        hr = GetToken(token, p, true);
                        if(! token.IsEqual('(') ){
                            Error(true, ERROR_MACROARGUMENTSYNTAX, "Expected '('");
                            argParseOK = false;
                        }
                    }
                    else {
                        if(bMacroArgsComplete){
                            token.type = TOKEN_EOS;
                        }
                        else {
                            hr = GetDefineMacroArgToken(token, p);
                        }
                    }
                }
                if(SUCCEEDED(hr) && argParseOK){
                    if(argToken.type == TOKEN_EOS && token.type != TOKEN_EOS){
                        Error(true, ERROR_MACROARGUMENTSYNTAX, "Too many arguments");
                        argParseOK = false;
                    }
                    else if(argToken.type != TOKEN_EOS && token.type == TOKEN_EOS){
                        Error(true, ERROR_MACROARGUMENTSYNTAX, "Too few arguments");
                        argParseOK = false;
                    }
                    else if(argToken.type == TOKEN_OTHER){
                        if(token.type == TOKEN_OTHER && argToken.token[0] == token.token[0]){
                            // Good, we matched an open-paren, comma or a close-paren
                            if(argToken.IsEqual(')')){
                                bMacroArgsComplete = true;
                            }
                        }
                        else {
                            Error(true, ERROR_MACROARGUMENTSYNTAX, "Expected a comma or a close paren.");
                            argParseOK = false;
                        }
                    }
                    else if(argToken.type == TOKEN_EOS && token.type == TOKEN_EOS){
                        break; // We're done with parsing the args
                    }
                    else if(argToken.type == TOKEN_SYMBOL && token.type == TOKEN_SYMBOL){
                        hr = args.Add(argToken.token, argToken.length, token.token, token.length);
                    }
                    else {
                        Error(true, ERROR_MACROARGUMENTSYNTAX, "Unexpected symbol.");
                        argParseOK = false;
                    }
                }
            } while(SUCCEEDED(hr) && argParseOK);
        }
        if(SUCCEEDED(hr)){
            if(argParseOK){
                buffer.Clear();
                hr = ProcessDefineMacroBody(buffer, bodyp, args);
            }
        }
        return hr;
    }

    HRESULT ProcessDefineMacroBody(Buffer& buffer, const char*& bodyp, Map& args){
        HRESULT hr = S_OK;
        // Output the define macro body, substituting symbols as we go
        do {
            Token bodyToken;
            hr = GetToken(bodyToken, bodyp, false);
            if(SUCCEEDED(hr)){
                if(bodyToken.type == TOKEN_EOS){
                    break;
                }
                if(bodyToken.type == TOKEN_SYMBOL){
                    const char* val = args.Lookup(bodyToken.token, bodyToken.length);
                    if(val){
                        hr = buffer.Append(val);
                        continue;
                    }
                }
                hr = buffer.Append(bodyToken.token, bodyToken.length);
            }
        } while(SUCCEEDED(hr));
        return hr;
    }

    void FormatAsStringConstant(LPSTR out, DWORD outSize, LPCSTR in){
		if(outSize == 0){
			return;
		}
		LPSTR outEnd = out + outSize;
		while(out < outEnd){
			char c = *in++;
			if(!c) {
				break;
			}
			if(c == '\\' || c == '"'){
				if(out + 1 >= outEnd) {
					break; // No room
				}
				*out++ = '\\';
			}
			*out++ = c;
		}
		if(out < outEnd){
			*out++ = 0;
		}
	}

	HRESULT Output(Buffer& buf){
		HRESULT hr = S_OK;
		
		// Do we need to emit "#line" information?

		bool needLine = false;
		bool needFile = false;

		if(strcmp(m_currentOutputFileName, pInput->m_fileName) != 0){
			strncpy(m_currentOutputFileName, pInput->m_fileName, sizeof(m_currentOutputFileName));
			needFile = true;
			needLine = true;
		}
		if(m_currentOutputLine != pInput->m_currentLine){
			m_currentOutputLine = pInput->m_currentLine;
			needLine = true;
		}
		if(SUCCEEDED(hr)) {
			if(needLine ) {
				if( needFile){
					char fileBuf[MAX_PATH * 2];
					FormatAsStringConstant(fileBuf, sizeof(fileBuf), m_currentOutputFileName);
					hr = OutputF("#line %d \"%s\"\n", m_currentOutputLine, m_currentOutputFileName);
				}
				else {
					hr = OutputF("#line %d\n", m_currentOutputLine);
				}
			}
		}
        if(SUCCEEDED(hr)){
            // Output the line. If this is a multi-line line, output a
            // #line directive every line, to make error reporting work correctly
            const char* pStart = buf.GetText();
            const char* p = pStart;
            bool firstLine = true;
            while(SUCCEEDED(hr) && *p){
                while(*p && *p != '\n'){
                    p++;
                }
                if(*p == '\n'){
                    ++p;
                    if(firstLine){
                        firstLine = false;
                    }
                    else{
                        hr = OutputF("#line %d\n", m_currentOutputLine);
                    }
                    hr = Output3(pStart, p-pStart);
                    pStart = p;
                }
            }
            if(SUCCEEDED(hr)){
                m_currentOutputLine++;
            }
            if(SUCCEEDED(hr)){
                if(pStart != p){ // Some text at the end that wasn't terminated with a \r\n
                    hr = Output3(pStart, p-pStart);
                    if(SUCCEEDED(hr)){
                        hr = Output2("\r\n");
                    }
                    if(SUCCEEDED(hr)){
                        m_currentOutputLine++;
                    }
                }
            }
        }
        return hr;
    }

	HRESULT OutputF(const char* szFormat, ...){
		char buf[256];
		va_list list;
		va_start(list, szFormat);
		_vsnprintf(buf, sizeof(buf), szFormat, list);
		buf[sizeof(buf)-1] = '\0';
		va_end(list);
		return Output2(buf);
	}

	HRESULT Output2(const char* s){
		return Output3(s, strlen(s));
	}

	HRESULT Output3(const char* s, size_t size){
#if !defined(XBOX) && defined(DBG_PRINT_PREPROCESSED)
        char buf[1000];
        memcpy(buf,s,size);
        buf[size] = 0;
		DPF(1,"%s", buf);
#endif
		return outputBuffer.Append(s, size);
	}

	HRESULT FindLine(Buffer& lineBuf){
		HRESULT hr = S_OK;
        lineBuf.Clear();
        if(! pInput->IsEOF()){
            hr = FindSingleLine(lineBuf);
        }
        return hr;
    }

	// A GetC that filters out backslash-escaped end-of-lines
	int FilteredGetC(){
		int c;
		for(;;) {
			c = pInput->GetC();
			if(c == '\\'){
				if(pInput->GuessC('\r')){
					pInput->GuessC('\n');
					continue;
				}
				else if(pInput->GuessC('\n')){
					continue;
				}
			}
			break;
		}
		return c;
	}

	bool FilteredGuessC(int guess){
		int c = FilteredGetC();
		bool result = c == guess;
		if(!result){
			pInput->UnGetC(c);
		}
		return result;
	}
		
	HRESULT FindSingleLine(Buffer& lineBuf){
		HRESULT hr = S_OK;
		while(! pInput->IsEOF() && SUCCEEDED(hr)){
			int c = FilteredGetC();
			if ( c == -1 ) {
				// EOF
				break;
			}
			switch(c){
			case '\r':
				{
					pInput->GuessC('\n'); // DOS style eol
					return S_OK;
				}
			case '\n':
				{
					// Unix style eol
					return S_OK;
				}
			case ';':
				{	// Assembler style comment
					// (If we could disable this, then
					// this preprocessor would handle C code, too.)
					// Skip until end of line
					SkipToEndOfLine();
					// that's a whole line
					return S_OK;
				}
				break;
			case '/':
				{
					int c2 = FilteredGetC();
					switch(c2){
					case '/': // C++ style comment
						{
					        SkipToEndOfLine();
							return S_OK;
						}
					case '*':
						{
							// Start of C style comment
							for(;;){
								if(pInput->IsEOF()){
									// This comment ran off the end of the file.
									Error(true, ERROR_UNTERMINATED_C_STYLE_COMMENT,
                                        "Un-terminated C-style comment. Expected \"*/\".");
									return E_FAIL;
								}
                                if(FilteredGetC() == '*'){
                                    if(FilteredGuessC('/')){
									    break;
                                    }
								}
							}
			                hr = lineBuf.Append(' '); // C-style comments turn into single spaces.
						}
						break;
					default:
						{
							pInput->UnGetC(c2); // Not a comment after all
							hr = lineBuf.Append((char) c);
						}
					}
				} // End of forward slash processing
				break;
			default:
				{
					hr = lineBuf.Append((char) c);
				}
			}
		}
        if(FAILED(hr)){
            Error(true, ERROR_SOURCELINETOOLONG, "Source line too long.");
        }
		// Ran off the end of the input buffer, treat as normal end-of-line
		return hr;
	}

    void SkipToEndOfLine(){
        int c2;
		while((c2 = FilteredGetC()) != -1){
            if(c2 == '\r' || c2 == '\n'){
                if(c2 == '\r'){
			        FilteredGuessC('\n');
		        }
                break;
            }
		}
    }

	void Error(bool error, DWORD code, const char* szFormat,...){
		if(pErrorLog){
			char szA[512];

			va_list ap;
			va_start(ap, szFormat);
			_vsnprintf(szA, sizeof(szA), szFormat, ap);
			szA[sizeof(szA)-1] = '\0';
			va_end(ap);
			
			pErrorLog->Log(error, code + 2000, pInput->m_fileName, pInput->m_currentLine, szA);
		}
	}

	// Nested classes

	// Holds macro definitions

	class Map {
	public:
		Map(){
			table = 0;
			tableSize = 0;
		}
		~Map(){
			Cleanup();
		}

		void Cleanup(){
			for(DWORD i = 0; i < tableSize; i++){
				Entry* pEntry = table[i];
				delete pEntry;
			}
			delete [] table;
			table = 0;
			tableSize = 0;
		}

		HRESULT Initialize(DWORD initialTableSize){
			Cleanup();
            tableSize = initialTableSize;
			table = new PENTRY[tableSize];
			if(!table){
                tableSize = 0;
				return E_OUTOFMEMORY;
			}
			memset(table,0, sizeof(Entry*) * tableSize);
			return S_OK;
		}

		HRESULT Add(const char* key, DWORD keySize,
			const char* value, DWORD valueSize){
			if(Lookup(key,keySize) || ! table){
				return E_FAIL;
			}
			DWORD hash = Hash(key, keySize);
			Entry** ppEntry = &table[hash % tableSize];
			PENTRY entry = new Entry();
			HRESULT hr = entry->Initialize(key, keySize, value, valueSize);
			if(SUCCEEDED(hr)){
				if(! *ppEntry){
					*ppEntry = entry;
				}
				else{
					entry->next = *ppEntry;
					*ppEntry = entry;
				}
			}
			return hr;
		}

		const char* Lookup(const char* key, DWORD keySize){
			if(!table){
				return NULL;
			}
            DWORD hash = Hash(key, keySize);
			Entry* pEntry = table[hash % tableSize];
			while(pEntry){
				if(pEntry->keySize == keySize){
					if(strncmp(key, pEntry->key, keySize)==0){
						return pEntry->value;
					}
				}
				pEntry = pEntry->next;
			}
			return NULL;
		}

        void Remove(const char* key, DWORD keySize){
			if(!table){
				return;
			}
			DWORD hash = Hash(key, keySize);
            DWORD hashIndex = hash % tableSize;
			Entry* pEntry = table[hashIndex];
            Entry* pPrev = NULL;
			while(pEntry){
				if(pEntry->keySize == keySize){
					if(strncmp(key, pEntry->key, keySize)==0){
                        if(pPrev){
                            pPrev->next = pEntry->next;
                        }
                        else {
                            table[hashIndex] = pEntry->next;
                        }
                        pEntry->next = NULL; // Break association so pEntry's destructor
                                             // doesn't delete the rest of the chain
                        delete pEntry;
						return;
					}
				}
                pPrev = pEntry;
				pEntry = pEntry->next;
			}
		}

	private:


		// This hash function was originally developed for the Visual Basic 7
		// StringPool class.

		DWORD Hash(const char* key, DWORD keySize)
		{
		    const BYTE *pb = (const BYTE *)key;
		    const BYTE *pbEnd = pb + keySize;

		    DWORD hash = 0;

		    while (pb != pbEnd)
		    {
		        BYTE b = *pb++;
		        hash = _lrotl(hash, 1) + b * 0x10204081;
		    }

		    return hash;
		}

		class Entry {
		public:
			Entry(){
				next = 0;
				key = 0;
				value = 0;
			}

			~Entry(){
				Clean();
			}

			void Clean(){
				delete next; next = 0;
				delete [] key; key = 0;
				delete [] value; value = 0;
			}

			HRESULT Initialize(const char* key, DWORD keySize,
				const char* value, DWORD valueSize){
				Clean();
				HRESULT hr = Copy(this->key, key, keySize);
				this->keySize = keySize;
				if(SUCCEEDED(hr)){
					hr = Copy(this->value, value, valueSize);
				}
				return hr;
			}

			HRESULT Copy(char*& dest, const char* source, DWORD sourceSize){
				dest = new char[sourceSize+1];
				if(!dest){
					return E_OUTOFMEMORY;
				}
				memcpy(dest,source,sourceSize);
				dest[sourceSize] = '\0';
				return S_OK;
			}

			Entry* next;
			char* key;
			DWORD keySize;
			char* value;
		};

		typedef Entry* PENTRY;

		PENTRY* table;
		DWORD tableSize;
	};

	// Makes it easy to handle input.

#define kNoLevel (~0)

	class InputBuffer {
	public:
		InputBuffer(){
			this->pReadFile = 0;
			isEOF = true;
			validSize = 0;
			readIndex = 0;
			unGetCMode = false;
			m_fileName = NULL;
		}

		// Can be called multiple times. Closes old file each time
		HRESULT Initialize(LPXGBUFFER pReadFile, LPCSTR fileName){

			// Close old file if it's still open
			Close();

			this->pReadFile = pReadFile;
			pReadFile->AddRef();
			buffer = pReadFile->GetBufferPointer();
			validSize = pReadFile->GetBufferSize();
			readIndex = 0;
			isEOF = false;
			unGetCMode = false;
			m_fileName = Copy(fileName);
			m_currentLine = 1;
			isEOL = false;
			wasEOL = false;
            ifLevel = kNoLevel;
            isActive = true;
			return S_OK;
		}

		~InputBuffer(){
			Close();
			delete [] m_fileName;
		}

		int GetC() {
			int c;
			wasEOL = isEOL;
			if(isEOL){
				m_currentLine++;
				isEOL = false;
			}
			if(! unGetCMode && readIndex < validSize){
				c = ((unsigned char*) buffer)[readIndex++];
			}
			else {
				c = FullGetC();
			}
			if(c == '\n'){
				isEOL = true;
			}
			return c;
		}

		int PeekC(){
			int c = GetC();
			if(c != -1){
				UnGetC(c);
			}
			return c;
		}

		bool GuessC(char guess){
			int c = GetC();
			bool result = c == guess;
			if( ! result ) {
				UnGetC(c);
			}
			return result;
		}

		void UnGetC(int c){
			if(c != -1){
				unGetCMode = true;
				unGetCChar = (char) c;
				isEOL = wasEOL;
				if(wasEOL){
					m_currentLine--;
				}
			}
		}

		inline bool IsEOF() {
			return ! unGetCMode && isEOF;
		}

        bool IsActive(){
            return isActive;
        }

        void ComputeActive(){
            bool active = true;
            if(ifLevel != kNoLevel){
                for(DWORD i = 0; i <= ifLevel; i++){
                    if(!ifActive[i]){
                        active = false;
                        break;
                    }
                }
            }
            isActive = active;
        }

        HRESULT PushActiveLevel(bool isActive){
            HRESULT hr = S_OK;
            if(ifLevel == kNoLevel){
                ifLevel = 0;
            }
            else if(ifLevel >= MAX_IF_LEVELS-1){
                hr = E_FAIL;
            }
            else {
                ifLevel++;
            }
            if(SUCCEEDED(hr)){
                ifActive[ifLevel] = isActive;
                seenElse[ifLevel] = false;
                ComputeActive();
            }
            return hr;
        }

        HRESULT ElseActiveLevel(){
            HRESULT hr = S_OK;
            if(ifLevel == kNoLevel){
                hr = E_FAIL;
            }
            if(SUCCEEDED(hr)){
                if(seenElse[ifLevel]){
                    hr = E_FAIL;
                }
            }
            if(SUCCEEDED(hr)){
                seenElse[ifLevel] = true;
                ifActive[ifLevel] = ! ifActive[ifLevel];
                ComputeActive();
            }
            return hr;
        }

        HRESULT PopActiveLevel(){
            HRESULT hr = S_OK;
            if(ifLevel == kNoLevel){
                hr = E_FAIL;
            }
            else if(ifLevel == 0){
                ifLevel = kNoLevel;
            }
            else {
                ifLevel--;
            }
            if(SUCCEEDED(hr)){
                ComputeActive();
            }
            return hr;
        }

        DWORD InsideIfElse(){
            return ifLevel != kNoLevel;
        }

	private:
		void Close(){
			if(!isEOF){
				if(pReadFile){
					pReadFile->Release();
                    pReadFile = 0;
				}
			}
			isEOF = true;
		}

		int FullGetC(){
			if(unGetCMode){
				unGetCMode = false;
				return unGetCChar;
			}
			if(readIndex < validSize){
				return ((unsigned char*) buffer)[readIndex++];
			}
			if(isEOF){
				return -1;
			}
			Close();
			return -1;
		}

		LPXGBUFFER pReadFile;
		LPCVOID buffer;
		DWORD validSize;
		DWORD readIndex;
		bool isEOF;
		bool unGetCMode;
		char unGetCChar;
		bool isEOL; // Current character is EOL
		bool wasEOL; // Last character read was EOL character

        bool isActive; // true means we should process input
        DWORD ifLevel; // how deeply we're nested into ifdefs
        bool ifActive[MAX_IF_LEVELS];
        bool seenElse[MAX_IF_LEVELS];
	public:
		DWORD m_currentLine;
		char* m_fileName;
	};

	// Instance variables

	Buffer lineBuf;

	SASM_ResolverCallback pResolver;
	LPVOID pResolverUserData;
	LPXGBUFFER  pBaseInput;
	Buffer outputBuffer;
	LPXGBUFFER* pOutput;
	XD3DXErrorLog* pErrorLog;

	char* pDst;

	InputBuffer* pInput;
	DWORD currentIncludeLevel;
	InputBuffer fileStack[MAX_INCLUDE_LEVELS];

	Map defines;
    Map macros; // NVASM style macros

	// For #file and #line directive generation
	char m_currentOutputFileName[MAX_PATH];
	DWORD m_currentOutputLine;

	bool bAngleBracketsAreQuotes; // For processing #include directives
};

HRESULT
XGPreprocess(LPCSTR pSourceFileName,
	SASM_ResolverCallback pResolver,
	LPVOID pResolverUserData,
	LPXGBUFFER  pInput,
	LPXGBUFFER* pOutput,
	XD3DXErrorLog* pErrorLog)
{
	HRESULT hr = S_OK;
	PreProcessor* p = new PreProcessor();
	if ( ! p ){
		hr = E_OUTOFMEMORY;
	}
	if ( SUCCEEDED(hr) ){
		hr = p->Initialize(pSourceFileName, pResolver, pResolverUserData,
			pInput, pOutput, pErrorLog);
	}
	if ( SUCCEEDED(hr) ){
		hr = p->Preprocess();
	}
	delete p;
	return hr;
}

} // namespace XGRAPHICS
