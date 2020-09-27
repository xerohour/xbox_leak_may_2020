///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
//
//
///////////////////////////////////////////////////////////////////////////

// xsasm shader assembler
//

#include "stdafx.h"

// Uncomment this if you want to break into the debugger when an error occurs
// #define BREAK_ON_ERROR

#if defined(DBG) && defined(BREAK_ON_ERROR)
#define SETERROR(HR,CODE) \
    do { \
if(FAILED(HR = CODE)){ \
            DebugBreak(); \
        } \
    } while(0)
#else
#define SETERROR(HR,CODE) \
    HR = CODE
#endif

extern "C" const char* XGraphicsBuildVersion();

void stripExtension(char* s)
{
    size_t i = strlen(s);
    while(i){
        if(s[--i]=='.'){
            s[i] = '\0';
            break;
        }
        else if(s[i] == '\\' || s[i] == '/' || s[i] == ':'){
            break;
        }
    }
}

void stripDirectories(char* s)
{
    size_t i = strlen(s);
    while(i){
        if(s[--i]=='/' || s[i] == '\\'){
            i++;
            // Copy in place - it's overlapping, but we're moving in the right direction.
            char* d = &s[i];
            while(*s++ = *d++);
            break;
        }
    }
}

void stripToBaseName(char* s){
    stripDirectories(s);
    stripExtension(s);
}

void ConvertToUnicode(WCHAR* dest, DWORD destSizeBytes, LPCSTR source){
    DWORD destSizeWChars = destSizeBytes / sizeof(WCHAR);
    mbstowcs(dest,source,destSizeWChars);
    dest[destSizeWChars-1] = 0;
}

HRESULT CreateBufferFromFile(const char* path, LPXGBUFFER* ppBuffer){
    HRESULT hr = S_OK;
    if(!path || !ppBuffer){
        SETERROR(hr,E_FAIL);
    }
    HANDLE file = INVALID_HANDLE_VALUE;
    if(SUCCEEDED(hr)){
        file = CreateFile(path,GENERIC_READ,FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,
            NULL);
        if(file == INVALID_HANDLE_VALUE){
            SETERROR(hr,E_FAIL);
        }
    }
    DWORD size = 0;
    if(SUCCEEDED(hr)){
        size = GetFileSize(file,NULL);
        if(size == ~0){
            SETERROR(hr,E_FAIL);
        }
    }
    LPXGBUFFER pBuffer = NULL;
    if(SUCCEEDED(hr)){
        hr = XGBufferCreate(size, &pBuffer);
    }
    if(SUCCEEDED(hr)){
        DWORD sizeRead;
        BOOL result = ReadFile(file, pBuffer->GetBufferPointer(), size, &sizeRead, NULL);
        if(! result || sizeRead != size){
            SETERROR(hr,E_FAIL);
        }
    }
    if(file != INVALID_HANDLE_VALUE){
        CloseHandle(file);
    }
    if(SUCCEEDED(hr)){
        *ppBuffer = pBuffer;
    }
    else {
        if(pBuffer) pBuffer->Release();
    }
    return hr;
}

HRESULT CreateFileFromBuffer(const char* path, LPXGBUFFER pBuffer, BOOL bWritePSHeader){
    HRESULT hr = S_OK;
    if(!path || !pBuffer){
        SETERROR(hr,E_FAIL);
    }
    HANDLE file = INVALID_HANDLE_VALUE;
    if(SUCCEEDED(hr)){
        file = CreateFile(path,GENERIC_WRITE,FILE_SHARE_READ,
            NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,
            NULL);
        if(file == INVALID_HANDLE_VALUE){
            SETERROR(hr,E_FAIL);
        }
    }
    if(SUCCEEDED(hr)){
        DWORD sizeWritten;
        // write pixel shader header DWORD if requested
        DWORD FileID = D3DPIXELSHADERDEF_FILE_ID;
        if(bWritePSHeader)
            WriteFile(file, &FileID, sizeof(DWORD), &sizeWritten, NULL);

        WriteFile(file,pBuffer->GetBufferPointer(),
            pBuffer->GetBufferSize(), &sizeWritten, NULL);
        if(sizeWritten != pBuffer->GetBufferSize()){
            SETERROR(hr,E_FAIL);
            fprintf(stderr, "Could not write file \"%s\".\n", path);
        }
    }
    else {
        fprintf(stderr, "Could not open file \"%s\" for writing.\n",
            path);
    }
    if(file != INVALID_HANDLE_VALUE){
        CloseHandle(file);
    }
    return hr;
}

BOOL MyDeleteFile(const char* file){
#ifdef XBOX
    return DeleteFile(file);
#else
    wchar_t buf[MAX_PATH];
    ConvertToUnicode(buf, sizeof(buf), file);
    return DeleteFileW(buf);
#endif
}

bool FileExists(const char* fullPath){
    bool fileExists = false;
    DWORD attributes = GetFileAttributes(fullPath);
    if(attributes != 0xFFFFFFFF && ! ( attributes & FILE_ATTRIBUTE_DIRECTORY)){
        fileExists = true;
    }
    return fileExists;
}


class Assembler {
    class Buffer;
    class StringTable;
public:
    Assembler(){
    }

    ~Assembler(){
    }

    int main(int argc, char* argv[])
    {
        CheckXGraphicsVersion();

        const char* inFile = NULL;
        const char* ucodeFile = NULL;
        const char* constFile = NULL;
        const char* listFile = NULL;
        bool suppressBanner = false;
        bool donotoptimize = false;
        bool disableGlobalOptimization = false;
        bool verifyOptimizations = true;
        bool outputTokens = false;
        bool listing = false;
        bool outputAsHeader = false;
        bool inputTokens = false;
        DWORD inputTokensShaderType = 0;
        bool inputMicrocode = false;
        DWORD inputMicrocodeShaderType = 0;
        bool dontValidate = false;
        bool skipPreprocessor = false;
        bool preprocessOnly = false;
        bool generateDebugInfo = false;
        const char* hname = NULL;
        HRESULT hr = S_OK;

        Resolver resolver;

        SETERROR(hr,resolver.Initialize());
        if(FAILED(hr)){
            return hr;
        }

        StringTable defineTable;
        SETERROR(hr,defineTable.Initialize());
        if(FAILED(hr)){
            return hr;
        }

        Buffer base;
        SETERROR(hr,base.Initialize(1024));
        if(FAILED(hr)){
            return hr;
        }

        for(int i = 1; i < argc;i++){
            const char* arg = argv[i];
            if(arg[0] == '-' || arg[0] == '/'){
                switch(arg[1]){
                case 'D':
                    if(arg[2] != '\0') goto unknownFlag;
                    {
                        if(i + 1 >= argc){
                            usage("/D requires an argument");
                            break;
                        }
                        // Is this a -D symbol or a -D symbol=value
                        base.Append("#define ");
                        char* symbol = argv[++i];
                        int offset = strcspn(symbol, "=");
                        // Is this symbol already defined? Use cheap linear search
                        if(defineTable.Exists(symbol, offset)){
                            fprintf(stderr, "This symbol is already defined: -D %s\n", symbol);
                            exit(-2);
                        }
                        defineTable.Add(symbol, offset);
                        base.Append(symbol,offset);
                        if(symbol[offset] == '='){
                            base.Append(' ');
                            base.Append(symbol + offset + 1);
                        }
                        base.Append('\n');
                    }
                    break;

                case 'O':
                    if(arg[2] == 0 || arg[3] != '\0') goto unknownFlag;
                    switch(arg[2]){
                        case 'd':
                            donotoptimize = true;
                            break;
                        case 'g':
                            disableGlobalOptimization = true;
                            break;
                        case 'v':
                            verifyOptimizations = false;
                            break;
                        default:
                            usage("Unrecognized option to /O.");
                            break;
                    }
                    break;
                case 'n':
                    if(strcmp(&arg[1],"nologo") == 0){
                        suppressBanner = true;
                    }
                    else {
                        goto unknownFlag;
                    }
                    break;
                case 'd':
                    if(arg[2] != 0) goto unknownFlag;
                    generateDebugInfo = true;
                    break;
                case 'h':
                    if(strcmp(&arg[1],"help") == 0){
                        goto help;
                    }
                    else if(strcmp(&arg[1],"hname") == 0){
                        if(i + 1 >= argc){
                            usage("/hname requires an argument");
                            break;
                        }
                        hname = argv[++i];
                    }
                    else if(arg[2] != 0) goto unknownFlag;
                    outputAsHeader = true;
                    break;
                case 'l':
                    if(arg[2] != 0) goto unknownFlag;
                    listing = true;
                    break;
                case 'p':
                    if(arg[2] != 0) goto unknownFlag;
                    preprocessOnly = true;
                    break;
                case 'P':
                    if(arg[2] != 0) goto unknownFlag;
                    skipPreprocessor = true;
                    break;
                case 't':
                    if(arg[2] != 0) goto unknownFlag;
                    outputTokens = true;
                    break;
                case 'u':
                    if(arg[2] != 0){
						if(arg[2] == 'u' && arg[3] == 0){
							inputMicrocode = true;
							if(i + 1 >= argc){
								usage("/uu requires an argument");
								break;
							}
							inputMicrocodeShaderType = atoi(argv[++i]);
							break;
						}
						goto unknownFlag;
					}
                    inputTokens = true;
                    if(i + 1 >= argc){
                        usage("/u requires an argument");
                        break;
                    }
                    inputTokensShaderType = atoi(argv[++i]);
                    break;
                case 'v':
                    if(arg[2] != 0) goto unknownFlag;
                    dontValidate = true;
                    break;

                case 'I':
                    if(arg[2] != 0) goto unknownFlag;
                    if(i + 1 >= argc){
                        usage("/I requires an argument");
                        break;
                    }
                    resolver.AddIncludePath(argv[++i]);
                    break;
                case '?':
help:
                    if(arg[2] != 0) goto unknownFlag;
                    usage("Xbox Shader Assembler.");
                    break;
                default:
unknownFlag:
                    {
                        char msgBuf[512];
                        _snprintf(msgBuf,sizeof(msgBuf),"Unknown flag \"%s\"", arg);
                        msgBuf[sizeof(msgBuf)-1]='\0';
                        usage(msgBuf);
                    }
                }
            }
            else if ( ! inFile ) {
                inFile = arg;
            }
            else if ( ! ucodeFile ) {
                ucodeFile = arg;
            }
            else if ( ! constFile ) {
                constFile = arg;
            }
            else if ( ! listFile ) {
                listFile = arg;
            }
            else {
                usage("Unexpected extra file name.");
            }
        }
        if(!inFile){
            usage("Need to specify an input file.");
        }

        if( ! defineTable.IsEmpty() && skipPreprocessor ){
            usage("Can't use /D option and /P option at the same time.");
        }

        if(generateDebugInfo && ! outputTokens){
            usage("/d option requires /t option as well.");
        }

        if(hname && ! outputAsHeader){
            usage("/hname option requires /h option as well.");
        }

        char defaultOutFile[MAX_PATH];
        calculateDefaultOutFile(defaultOutFile, inFile);
        char defaultConstFile[MAX_PATH];
        char defaultListFile[MAX_PATH];
        if(outputAsHeader){
            sprintf(m_defaultUcodeFile,"%s.h",defaultOutFile);
        }
        else {
            sprintf(m_defaultUcodeFile,"%s.x$u",defaultOutFile);
            // We modify the '$' later, once we know whether we've got a pixel shader or a vertex
            // shader
        }

        sprintf(defaultConstFile,"%s.xsc",defaultOutFile);
        sprintf(defaultListFile,"%s.lst",defaultOutFile);
        // fill in default filenames if necessary
        // delete old files (listing file deleted here,
        // other files deleted after assembly is done)
        if(!ucodeFile){
            ucodeFile = m_defaultUcodeFile;
        }
        if(!constFile){
            constFile = defaultConstFile;
        }
        
        if(!listFile && listing){
            listFile = defaultListFile;
            MyDeleteFile(listFile);
        }
        else{
            MyDeleteFile(defaultListFile);
        }

		if(inputTokens || inputMicrocode){
			skipPreprocessor = true;
		}

        DWORD flags = 0;
        if (generateDebugInfo) flags |= SASM_DEBUG; 
        if (donotoptimize) flags |= SASM_DONOTOPTIMIZE; 
        if (disableGlobalOptimization) flags |= SASM_DISABLE_GLOBAL_OPTIMIZATIONS; 
        if (verifyOptimizations) flags |= SASM_VERIFY_OPTIMIZATIONS; 
        if (outputTokens) flags |= SASM_OUTPUTTOKENS;
        if (dontValidate) flags |= SASM_SKIPVALIDATION;
        if (preprocessOnly) flags |= SASM_PREPROCESSONLY;
        if (skipPreprocessor) flags |= SASM_SKIPPREPROCESSOR;
        if (inputTokens){
            switch(inputTokensShaderType){
            case 0: flags |= SASM_INPUT_PIXELSHADER_TOKENS;
                break;
            case 1: flags |= SASM_INPUT_VERTEXSHADER_TOKENS;
                break;
            case 2: flags |= SASM_INPUT_READWRITE_VERTEXSHADER_TOKENS;
                break;
            case 3: flags |= SASM_INPUT_VERTEXSTATESHADER_TOKENS;
                break;
            case 4: flags |= SASM_INPUT_VERTEXSHADER_TOKENS
                          | SASM_INPUT_SCREENSPACE_VERTEXSHADER_TOKENS;
                break;
            case 5: flags |= SASM_INPUT_READWRITE_VERTEXSHADER_TOKENS
                          | SASM_INPUT_SCREENSPACE_VERTEXSHADER_TOKENS;
                break;
            case 6: flags |= SASM_INPUT_VERTEXSHADER_TOKENS
                          | SASM_INPUT_NONXBOX_TOKENS;
                break;
            default:
                usage("unknown numeric argument to /u. Expected (0..5)");
                break;
            }
		}
		if (inputMicrocode){
			switch(inputMicrocodeShaderType){
			case 0: flags |= SASM_INPUT_MICROCODE;
				break;
			case 1: flags |= SASM_INPUT_SCREENSPACE_MICROCODE;
				break;
			default:
				usage("unknown numeric argument to /uu. Expected (0..1)");
				break;
			}
		}

        if ( ! suppressBanner ) {
            Banner();
        }
        hr = Assemble(inFile,constFile,ucodeFile,listFile,
            outputAsHeader, hname,
            flags,
            ResolverCallback, &resolver,
            skipPreprocessor ? NULL : &base);

        return hr;
    }

private:

#define INTERNAL_BUFFER_NAME ":internal buffer"

    HRESULT CreateHeaderFile(const char* headerFile, LPXGBUFFER ucodeBuffer,
                             LPXGBUFFER constBuffer, bool isTokens, DWORD shaderType,
                             const char* hname){
        HRESULT hr = S_OK;
        // Use stdio because it's nice to be able to use printf
        FILE* out = fopen(headerFile, "wt");
        if(!out){
            SETERROR(hr,E_FAIL);
        }

        char baseName[MAX_PATH];
        strncpy(baseName, headerFile, MAX_PATH);
        baseName[MAX_PATH-1] = '\0';
        stripToBaseName(baseName);

        // Capitalize first letter of base name for neatness

        if(baseName[0] >= 'a' && baseName[0] <= 'z'){
            baseName[0] -= 'a' - 'A';
        }

        const char* kShaderTypes[] = {"Pixel", "Vertex", "ReadWriteVertex", "VertexState"};

        char nameBuf[MAX_PATH];
        if(!hname){
            _snprintf(nameBuf, sizeof(nameBuf),"dw%s%sShader", baseName,
                kShaderTypes[SASMT_SHADERTYPE(shaderType)]);
            nameBuf[MAX_PATH-1] = 0;
            hname = nameBuf;
        }

        if(SUCCEEDED(hr) && ucodeBuffer && ucodeBuffer->GetBufferSize()){
            fprintf(out,"DWORD %s[] = {\n", hname);
            DWORD numDWORDS = ucodeBuffer->GetBufferSize() / sizeof(DWORD);
            DWORD* code = (DWORD*) ucodeBuffer->GetBufferPointer();
            for(DWORD i = 0; i < numDWORDS; i++){
                if((i == 0) || (((i-1) % 4) == 0) ){
                    fprintf(out, "\t");
                }
                fprintf(out, "0x%08x", code[i]);
                if(i < numDWORDS-1){
                    fprintf(out, ",");
                    if((i != 0) && (((i-1) % 4) < 3)){
                        fprintf(out, " ");
                    }
                    else {
                        fprintf(out, "\n");
                    }
                }
            }
            fprintf(out, "\n};\n");
        }
        // Now, write constant data

        if(SUCCEEDED(hr) && constBuffer && constBuffer->GetBufferSize()){
            // Write a seperating space if there's both ucode data and const data
            if(ucodeBuffer && ucodeBuffer->GetBufferSize()){
                fprintf(out, "\n");
            }
            fprintf(out,"DWORD dw%s%sShaderConstantData[] = {\n", baseName, kShaderTypes[shaderType]);
            DWORD numDWORDS = constBuffer->GetBufferSize() / sizeof(DWORD);
            DWORD* code = (DWORD*) constBuffer->GetBufferPointer();
            for(DWORD i = 0; i < numDWORDS; i++){
                if(i % 5 == 0 ){
                    fprintf(out, "\t");
                }
                fprintf(out, "0x%08x", code[i]);
                if(i < numDWORDS-1){
                    fprintf(out, ",");
                    if(i % 5 < 4){
                        fprintf(out, " ");
                    }
                    else {
                        fprintf(out, "\n");
                    }
                }
            }
            fprintf(out, "\n};\n");
        }

        if(out){
            fclose(out);
        }
        return hr;
    }

    HRESULT Assemble(const char* shaderFile,
        const char* constFile,
        const char* ucodeFile,
        const char* listFile,
        bool outputAsHeader,
        const char* hname,
        DWORD flags,
        SASM_ResolverCallback pResolver,
        LPVOID pResolverUserData,
        Buffer* pBase)
    {
        LPXGBUFFER shaderBuffer = NULL;
        LPXGBUFFER ucodeBuffer = NULL;
        LPXGBUFFER constBuffer = NULL;
        LPXGBUFFER errorBuffer = NULL;
        LPXGBUFFER listBuffer = NULL;

        HRESULT hr = S_OK;
        if( pBase ) {
            // Check if file exists...
            if(! FileExists(shaderFile)){
                fprintf(stderr, "Could not open file \"%s\".\n", shaderFile);
                hr = E_FAIL;
            }
            else {
                pBase->Printf("#include \"%s\"\n", shaderFile);
            }
        }
        else {
            hr = CreateBufferFromFile(shaderFile, &shaderBuffer);
			if(FAILED(hr)){
                fprintf(stderr, "Could not read file \"%s\".\n", shaderFile);
			}
        }

        DWORD shaderType = SASMT_VERTEXSHADER;

        if(SUCCEEDED(hr)){

            hr = AssembleShader(
                shaderFile,
                shaderBuffer ? shaderBuffer->GetBufferPointer() : pBase->GetText(),
                shaderBuffer ? shaderBuffer->GetBufferSize() : pBase->GetUsed(),
                flags,
                constFile ? &constBuffer : NULL,
                ucodeFile ? &ucodeBuffer : NULL,
                &errorBuffer,
                listFile ? &listBuffer : NULL,
                pResolver, pResolverUserData,
                &shaderType);
        }
        // Delete the old output files no matter what
        if(! outputAsHeader ) {
            if(constFile && (SASMT_SHADERTYPE(shaderType) != SASMT_PIXELSHADER)){
                MyDeleteFile(constFile);
            }
            // Fill in default file name extension, now that we know what kind of shader
            // we've got.
            if(ucodeFile == m_defaultUcodeFile){
                char c = 'v';
                if(SASMT_SHADERTYPE(shaderType) == SASMT_PIXELSHADER){
                    c = 'p';
                }
                DWORD offset = strlen(m_defaultUcodeFile) - 2; // Location of '$'
                m_defaultUcodeFile[offset] = c;
            }
        }

        if(ucodeFile){
            MyDeleteFile(ucodeFile);
        }
        if(SUCCEEDED(hr)){
            if(outputAsHeader){
                if(SUCCEEDED(hr)){
                    if(ucodeFile && (ucodeBuffer && ucodeBuffer->GetBufferSize())
                        || (constBuffer && constBuffer->GetBufferSize())){
                        bool isTokens = (flags & SASM_OUTPUTTOKENS) != 0;
                        hr = CreateHeaderFile(ucodeFile, ucodeBuffer, constBuffer, isTokens, shaderType, hname);
                    }
                }
            }
            else {
                if(SUCCEEDED(hr)){
                    if(constFile && constBuffer && constBuffer->GetBufferSize() ){
                        hr = CreateFileFromBuffer(constFile, constBuffer, FALSE);
                    }
                }
                if(SUCCEEDED(hr)){
                    if(ucodeFile && ucodeBuffer && ucodeBuffer->GetBufferSize()){
                        bool isTokens = (flags & SASM_OUTPUTTOKENS) != 0;
                        bool writePixelShaderHeader = (!isTokens) && (SASMT_SHADERTYPE(shaderType) == SASMT_PIXELSHADER);
                        hr = CreateFileFromBuffer(ucodeFile, 
                                                  ucodeBuffer, 
                                                  writePixelShaderHeader );
                    }
                }
            }
        }
        if(SUCCEEDED(hr)){
            if(listFile && listBuffer && listBuffer->GetBufferSize()){
                hr = CreateFileFromBuffer(listFile, listBuffer, FALSE);
            }
        }

        if(errorBuffer && errorBuffer->GetBufferSize()){
            fwrite(errorBuffer->GetBufferPointer(), errorBuffer->GetBufferSize(), 1, stderr);
        }

        RELEASE(shaderBuffer);
        RELEASE(listBuffer);
        RELEASE(constBuffer);
        RELEASE(ucodeBuffer);
        RELEASE(errorBuffer);
        return hr;
    }

    void Banner(){
        fprintf(stdout, "Microsoft (R) Xbox Shader Assembler Version %s\n", VER_PRODUCTVERSION_STR);
        fprintf(stdout, "Copyright (C) 2001 Microsoft Corporation. All rights reserved.\n");
        fflush(stdout);
    }

    void CheckXGraphicsVersion(){
        // Make sure we're linked with the corresponding version of XGraphics.
        const char* xgbv =  XGraphicsBuildVersion();
        if(strcmp(VER_PRODUCTVERSION_STR, xgbv) != 0){
            fprintf(stderr, "Error: Build version mismatch.\n");
            fprintf(stderr, "xsasm.exe     is from build %s\n",VER_PRODUCTVERSION_STR);
            fprintf(stderr, "xgraphics.lib is from build %s\n", xgbv);
            exit(3);
        }
    }

    void usage(const char* s){
        Banner();
        if(s){
            fprintf(stderr, "\nError: %s\n", s);
        }
		//               01234567890123456789012345678901234567890123456789012345678901234567890123456789
        fprintf(stderr, "\nusage: [/O[dgv]][/D symbol][/b][/d][/h][/l][/p][/P][/t][/u num][/v]\n");
        fprintf(stderr, "           [/I path] sourcefile <ucodefile> <constfile> <listfile>\n");
        fprintf(stderr, "  /D symbol   Define symbol.\n");
        fprintf(stderr, "                /D symbol=value   Define symbol to have a specific value.\n");
        fprintf(stderr, "  /Od         Turn off all optimization.\n");
        fprintf(stderr, "  /Og         Turn off just global optimization.\n");
        fprintf(stderr, "  /Ov         Turn off optimization verification.\n");
        fprintf(stderr, "  /d          Generate debugging information for DX8 token stream.\n");
        fprintf(stderr, "                Requires /t option.\n");
        fprintf(stderr, "  /h          Output as a .h file.\n");
        fprintf(stderr, "  /hname sym  Use sym as the name of the variable defined in the .h file.\n");
        fprintf(stderr, "                Requires /h option.\n");
        fprintf(stderr, "  /l          Generate a listing.\n");
        fprintf(stderr, "  /nologo     Suppress copyright message.\n");
        fprintf(stderr, "  /p          Only run preprocessor. Put preprocessor output in ucodeFile.\n");
        fprintf(stderr, "  /P          Skip preprocessor. (If you specify both /p and /P you\n");
        fprintf(stderr, "                copy sourcefile to ucodeFile.)\n");
        fprintf(stderr, "  /t          Generate D3D8 tokens instead of NV2A microcode.\n");
        fprintf(stderr, "  /u num      Input is D3D8 tokens instead of assembly source.\n");
        fprintf(stderr, "              num is: 0 pixel shader\n");
        fprintf(stderr, "                      1 vertex shader\n");
        fprintf(stderr, "                      2 read/write vertex shader\n");
        fprintf(stderr, "                      3 vertex state shader\n");
        fprintf(stderr, "                      4 screenspace vertex shader\n");
        fprintf(stderr, "                      5 screenspace read/write vertex shader\n");
        fprintf(stderr, "                      6 ordinary non-Xbox vertex shader\n");
		fprintf(stderr, "  /uu num     Input is microcode instead of assembly source.\n");
        fprintf(stderr, "              num is: 0 normal microcode\n");
        fprintf(stderr, "                      1 screenspace microcode\n");
        fprintf(stderr, "  /v          Do not validate the shader.\n");
        fprintf(stderr, "  /I path     Set Include search path.\n");
        fprintf(stderr, "  shaderfile  source file for shader.\n");
        fprintf(stderr, "  ucodefile   output file for microcode.\n");
        fprintf(stderr, "  constfile   output file for shader constants file.\n");
        fprintf(stderr, "  listfile    output file for listing.\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "If you don't specify ucodefile, it defaults to file.xpu for pixel shaders,\n");
        fprintf(stderr, "                                            or file.xvu for vertex shaders.\n");
        fprintf(stderr, "If you don't specify constfile, it defaults to file.xsc\n");
        fprintf(stderr, "If you don't specify listfile,  it defaults to file.lst\n");
        fprintf(stderr, "(where 'file' is the name of the shader file, not including the extension.)\n");
    #ifdef XBOX
        _asm int 3 // Breakpoint
    #else
        exit(-1);
    #endif
    }

    void calculateDefaultOutFile(char* out, const char* in){
        strcpy(out, in);
        stripExtension(out);
    }

    static HRESULT ResolverCallback(LPVOID pResolverUserData,
            BOOL isSystemInclude, LPCSTR sourceFilePath,
            LPCSTR includeFileName,
            LPSTR resolvedFilePath, DWORD resolvedFilePathSize,
            LPXGBUFFER* ppResolvedFile)
    {
        return ((Resolver*) pResolverUserData)->Resolve(isSystemInclude, sourceFilePath,
            includeFileName, resolvedFilePath, resolvedFilePathSize, ppResolvedFile);
    }


private:
    char m_defaultUcodeFile[MAX_PATH];

    class Buffer {
        DWORD m_size;
        char* m_buf;
        DWORD m_used;
    public:
        Buffer(){
            m_size = 0;
            m_buf = 0;
            m_used = 0;
        }

        ~Buffer(){
            delete [] m_buf;
        }

        HRESULT Initialize(DWORD size){
            m_used = 0;
            return GrowTo(size);
        }

        HRESULT GrowTo(DWORD size){
            HRESULT hr = S_OK;
            if(size + 1 > m_size){
                size += 4*1024;
                char* newBuf = new char[size];
                if(!newBuf){
                    hr = E_OUTOFMEMORY;
                }
                else{
                    memcpy(newBuf,m_buf, m_used);
                    delete [] m_buf;
                    m_buf = newBuf;
                    m_size = size;
                    m_buf[m_used] = 0;
                }
            }
            return hr;
        }

        void Printf(const char* msg,...){
            if(m_size - m_used < 1000){
                GrowTo(m_size + 2000);
            }
            va_list list;
            va_start(list, msg );
            m_used += _vsnprintf(m_buf + m_used, m_size - m_used, msg, list);
            va_end(list);
        }

        HRESULT Append(LPCVOID data, DWORD length){
            HRESULT hr = GrowTo(m_used + length);
            if(SUCCEEDED(hr)){
                memcpy(m_buf + m_used, data, length);
                m_used += length;
                m_buf[m_used] = '\0';
            }
            return hr;
        }

        HRESULT Append(char c){
            return Append(&c, sizeof(char));
        }

        HRESULT Append(const char* c){
            return Append(c, strlen(c));
        }

        HRESULT Append(Buffer& buffer){
            return Append(buffer.GetText(), buffer.GetUsed());
        }


        char* GetText(){
            return m_buf;
        }

        char* GetTextEnd(){
            return m_buf + m_used;
        }

        DWORD GetUsed(){
            return m_used;
        }

        void Clear(){
            m_used = 0;
            if(m_buf){
                m_buf[0] = 0;
            }
        }
    };

    class StringTable {
        Buffer m_buf;
    public:
        HRESULT Initialize(){
            return m_buf.Initialize(100);
        }

        bool IsEmpty(){
            return m_buf.GetUsed() == 0;
        }

        bool Exists(const char* symbol, DWORD length){
            // Search for symbol in a buffer.
            // Organization of buffer is packed null terminated strings
            for(const char* word = First(); word; word = Next(word)){
                if( strncmp(word, symbol, length) == 0 && word[length] == '\0'){
                    return true; // Found name
                }
            }
            // Not found
            return false;
        }

        HRESULT Add(const char* symbol, DWORD length){
            HRESULT hr = S_OK;
            hr = m_buf.Append(symbol,length);
            if(SUCCEEDED(hr)){
                hr = m_buf.Append('\0');
            }
            return hr;
        }
        const char* First(){
            if(m_buf.GetUsed() == 0){
                return NULL;
            }
            return m_buf.GetText();
        }
        const char* Next(const char* current){
            const char* next = current + strlen(current) + 1;
            if(next >= m_buf.GetTextEnd()){
                return NULL;
            }
            return next;
        }
    };

    /* The rules for #include files
    
        If it's a system include, search in the list of directories
        specified with AddIncludePath

        If it's not a system include, first search in the directory
        of the sourceFile, then in the list of directories specified
        with AddIncludePath.

        Include directories are specified with or without the trailing
        slash.
    */
    class Resolver {
    public:
        Resolver(){
        }

        HRESULT Initialize(){
            m_bisFirstInclude = TRUE;
            return m_includePaths.Initialize();
        }

        HRESULT Resolve(BOOL isSystemInclude, LPCSTR sourceFilePath,
            LPCSTR includeFileName,
            LPSTR resolvedFilePath, DWORD resolvedFilePathSize,
            LPXGBUFFER* ppResolvedFile)
        {
            HRESULT hr = S_OK;
            char fullPath[MAX_PATH];
            bool foundFile = false;
            if(SUCCEEDED(hr)){

                // We know that the very first time we're called
                // it's from the internal bufer that we use to
                // define symbols that were defined from the command
                // line using the -D flag.

                if(m_bisFirstInclude){
                    m_bisFirstInclude = FALSE;
                    strncpy(fullPath, includeFileName, sizeof(fullPath));
                    fullPath[sizeof(fullPath)-1] = '\0';
                    foundFile = FileExists(fullPath);
                    if(!foundFile){
                        SETERROR(hr,E_FAIL);
                    }
               }
            }
            if(SUCCEEDED(hr) && ! foundFile){
                if(! isSystemInclude ) {
                    // First, search relative to the source file path

                    // Strip off the file name
                    char sourceDir[MAX_PATH];
                    CalculateDirectoryPath(sourceDir, sizeof(sourceDir), sourceFilePath);
                    if(sourceDir[0]){
                        sprintf(fullPath, "%s\\%s", sourceDir, includeFileName);
                        foundFile = FileExists(fullPath);
                    }
                    else {
                        strncpy(fullPath, includeFileName, sizeof(fullPath));
                        fullPath[sizeof(fullPath)-1] = '\0';
                        foundFile = FileExists(fullPath);
                    }
                }
            }
            if(SUCCEEDED(hr) && ! foundFile){
                for(const char* includePath = m_includePaths.First();
                    includePath && SUCCEEDED(hr);
                    includePath = m_includePaths.Next(includePath) )
                {
                    // Allow backslashes at the end of include paths.
                    // Required to handle the edge cases of "\" and "C:\"

                    DWORD includePathLength = strlen(includePath);
                    if(includePathLength && includePath[includePathLength-1] == '\\'){
                        sprintf(fullPath, "%s%s", includePath, includeFileName);
                    }
                    else {
                        sprintf(fullPath, "%s\\%s", includePath, includeFileName);
                    }
                    foundFile = FileExists(fullPath);
                    if(foundFile){
                        break;
                    }
                }
            }
            if(SUCCEEDED(hr)){
                if( !foundFile ){ 
                    strcpy(fullPath, includeFileName);
                    fullPath[sizeof(fullPath)-1] = '\0';
                    foundFile = FileExists(fullPath);
                }
            }

            if(SUCCEEDED(hr)){
                if(foundFile){
                    hr = CreateBufferFromFile(fullPath, ppResolvedFile);
                }
                else {
                    SETERROR(hr,E_FAIL);
                }
            }

            if(SUCCEEDED(hr)){
                strncpy(resolvedFilePath, fullPath, resolvedFilePathSize);
            }

            return hr;
        }

        HRESULT AddIncludePath(LPSTR includePath){
            return m_includePaths.Add(includePath,strlen(includePath));
        }
    private:

        void CalculateDirectoryPath(char* dirPath, DWORD dirPathSize, const char* sourceFileName){
            // Find last slash
            if(dirPathSize == 0){
                return;
            }
            const char* pLastSlash = 0;
            for(const char* s = sourceFileName; *s; s++){
                if(*s == '\\'){
                    pLastSlash = s;
                }
            }
            DWORD size = 0;
            if(pLastSlash != 0){
                size = pLastSlash - sourceFileName; // Includes slash
            }
            if(size > dirPathSize-1){
                size = dirPathSize-1;
            }
            if(size > 0){
                memcpy(dirPath, sourceFileName, size);
            }
            dirPath[size] = '\0';
        }

        bool m_bisFirstInclude;

        StringTable m_includePaths;
    };

};

#ifdef XBOX

void __cdecl main()
{
    // TODO: Parse the command line into an argv argc pair.
    Assembler assembler;
    assembler.main(0,0);
}
#else
int __cdecl main(int argc, char* argv[])
{
    Assembler assembler;
    return assembler.main(argc,argv);
}
#endif
