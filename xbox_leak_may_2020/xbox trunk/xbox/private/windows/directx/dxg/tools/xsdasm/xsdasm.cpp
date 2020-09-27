///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
//
//
///////////////////////////////////////////////////////////////////////////

// xsdasm shader disassembler
//

#include "stdafx.h"

#ifdef XBOX
#error This is not an Xbox application. It's a command line tool, and there's no command line on Xbox.
#endif

namespace D3D {

void DbgPrint(const char* data,...){
    va_list marker;
    va_start( marker, data);
    char buf[1000];
    _vsnprintf(buf, sizeof(buf)-1, data, marker);
    buf[sizeof(buf)-1] = 0;
    OutputDebugString(buf);
}

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
            fprintf(stderr,"Could not open file: \"%s\".\n", path);
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

HRESULT CreateFileFromBuffer(const char* path, LPXGBUFFER pBuffer){
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
        WriteFile(file,pBuffer->GetBufferPointer(),
            pBuffer->GetBufferSize(), &sizeWritten, NULL);
        if(sizeWritten != pBuffer->GetBufferSize()){
            SETERROR(hr,E_FAIL);
        }
    }
    if(file != INVALID_HANDLE_VALUE){
        CloseHandle(file);
    }
    return hr;
}

BOOL MyDeleteFile(const char* file){
    return DeleteFile(file);
}

    void Banner(){
        fprintf(stdout, "**** XSDASM %s, Copyright (C) 2001 Microsoft Corporation.\n",
            VER_PRODUCTVERSION_STR);
        fprintf(stdout, "****       All Rights Reserved.\n");
        fflush(stdout);
    }

void usage(const char* s){
    Banner();
    if(s){
        fprintf(stderr, "\nError: %s\n", s);
    }
    fprintf(stderr, "\nusage: [-b] ucodeFile <listfile>\n");
    fprintf(stderr, "-b suppress banner.\n");
    fprintf(stderr, "  ucodefile   input file containing vertex shader microcode.\n");
    fprintf(stderr, "  listfile    output file for listing.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "If you don't specify listfile, it defaults to stdout\n");
    exit(-1);
}

// Secret function Imported from xgraphicslib

extern "C" HRESULT DisassembleVertexShader(LPCVOID pUcode, DWORD length, LPXGBUFFER* pDisassembly);

class Disassembler {
public:
    Disassembler(){
        m_pIn = NULL;
        m_out = NULL;
    }

    ~Disassembler(){
    }

    HRESULT Initialize(LPXGBUFFER in, FILE* out){
        HRESULT hr = S_OK;
        m_pInBuffer = in;
        m_out = out;
        return hr;
    }

    HRESULT Disassemble(){
        HRESULT hr = S_OK;
        m_pIn = (const UCHAR*) m_pInBuffer->GetBufferPointer();
        m_pEnd = m_pIn + m_pInBuffer->GetBufferSize();

        hr = ParseHeader();
        if(SUCCEEDED(hr)){
            hr = ParseBody();
        }
        return hr;
    }

private:

    HRESULT ParseHeader(){
        HRESULT hr = S_OK;
        if(m_pEnd - m_pIn < 4){
            fprintf(stderr, "input file too small to contain a shader.\n");
            SETERROR(hr,E_FAIL);
        }
        if(SUCCEEDED(hr)){
            m_shaderType = XGSUCode_GetVertexShaderType(m_pInBuffer->GetBufferPointer());
            if(m_shaderType == SASMT_INVALIDSHADER){
                fprintf(stderr, "invalid shader type %d.\n", m_shaderType);
                SETERROR(hr,E_FAIL);
            }
        }
        if(SUCCEEDED(hr)){
            static const char* kShaderNames[] = {
                "Vertex",
                "Read/Write Vertex",
                "Vertex State"
            };

            fprintf(m_out, "; This is a %s Shader.\n",
                kShaderNames[m_shaderType-SASMT_VERTEXSHADER]);
        }

        if(SUCCEEDED(hr)){
            m_length = XGSUCode_GetVertexShaderLength(m_pInBuffer->GetBufferPointer());
        }
        if(SUCCEEDED(hr)){
            m_pIn += sizeof(DWORD); // Skip header
        }
        return hr;
    }

    HRESULT ParseBody(){
        HRESULT hr = S_OK;
        
        if ( m_length > 136){
            fprintf(stderr, "Not a valid program. Too many instructions.\n");
//            SETERROR(hr,E_FAIL);
        }
        
        if ( m_length * sizeof(DWORD)*4 != (DWORD) (m_pEnd - m_pIn)){
            fprintf(stderr, "Not a valid program. Number of instructions does not match code length.\n");
            SETERROR(hr,E_FAIL);
        }

        LPXGBUFFER pDisassembly;

        if(SUCCEEDED(hr)){
            hr = DisassembleVertexShader(m_pInBuffer->GetBufferPointer(),
                m_pInBuffer->GetBufferSize(), &pDisassembly);
        }
        if(SUCCEEDED(hr)){
            fwrite(pDisassembly->GetBufferPointer(),
                pDisassembly->GetBufferSize(), 1, m_out);
            pDisassembly->Release();
        }
        return hr;
    }

    const UCHAR* m_pIn;
    const UCHAR* m_pEnd;

    LPXGBUFFER m_pInBuffer;
    FILE* m_out;

    DWORD m_shaderType;
    DWORD m_length; // In instructions
};


HRESULT Disassemble(const char* infile, const char* outfile){
    HRESULT hr = S_OK;

    LPXGBUFFER pInput = NULL;
    hr = CreateBufferFromFile(infile,&pInput);

    FILE* output = stdout;

    if(SUCCEEDED(hr)){
        if(outfile){
            output = fopen(outfile, "wt");
            if(!output){
                fprintf(stderr,"Cannot open output file \"%s\"\n", outfile);
                hr = E_FAIL;
            }
        }
    }

    if(SUCCEEDED(hr)){
        Disassembler d;
        hr = d.Initialize(pInput, output);
        if(SUCCEEDED(hr)){
            hr = d.Disassemble();
        }
    }

    RELEASE(pInput);

    if(output != NULL && output != stdout){
        fclose(output);
    }
    return hr;
}

extern "C"
int __cdecl main(int argc, char* argv[])
{
    const char* inFile = NULL;
    const char* listFile = NULL;
    bool suppressBanner = false;

    for(int i = 1; i < argc;i++){
        const char* arg = argv[i];
        if(arg[0] == '-' || arg[0] == '/'){
            switch(arg[1]){
                case 'b':
                    suppressBanner = true;
                    break;
                default:
                    usage("Unknown flag.");
                }
        }
        else if ( ! inFile ) {
            inFile = arg;
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
    if ( ! suppressBanner ) {
        Banner();
    }
    HRESULT hr = Disassemble(inFile,listFile);

    return hr;
}

} // namespace D3D
