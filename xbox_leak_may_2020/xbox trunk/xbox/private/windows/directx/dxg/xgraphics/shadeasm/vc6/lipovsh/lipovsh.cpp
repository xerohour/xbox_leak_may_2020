#include "stdafx.h"

#ifdef XBOX
#error lipovsh doesn't work on xbox!
#endif

void syntax() {
	printf("LipoVSH: Copyright (c) Microsoft Corperation.\n");
	printf("debugging tool to simplify a vertex shader that breaks an optimizer\n");
	printf("Syntax: LipoVsh In Out\n");
	printf("In: file that breaks the shader assembler optimizer\n");
	printf("Out: location to place the smallest version of 'In' that still breaks.\n");
}

int __cdecl main(int argc, char* argv[])
{
	HANDLE in = INVALID_HANDLE_VALUE;
	HANDLE out = INVALID_HANDLE_VALUE;
	HRESULT hr = 0;
	char*buf = 0;
	DWORD size = 0;

	if(argc != 3) {
		syntax();
		return E_FAIL;
	}

	in = CreateFile(argv[1], GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
	if(in == INVALID_HANDLE_VALUE) {
		hr = E_FAIL;
	}
	if(SUCCEEDED(hr)) {
		out = CreateFile(argv[2], GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, 0);
		if(out == INVALID_HANDLE_VALUE) {
			hr = E_FAIL;
		}
	}
	if(SUCCEEDED(hr)) {
		size = GetFileSize(in,0);
		buf = (char*)malloc(size + 1);
		if(buf == 0) {
			hr = E_OUTOFMEMORY;
		}
	}
	if(SUCCEEDED(hr)) {
		DWORD size2;
		ReadFile(in, buf, size, &size2, 0);
		buf[size] = '\0';
		if(size2 != size) {
			hr = E_UNEXPECTED;
		}
	}

//the loop
	if(SUCCEEDED(hr)) {
		int i;
		char b;
		bool br = false;
		LPXGBUFFER blah = 0, blah2 = 0;
		LPXGBUFFER err = 0;


		if(SUCCEEDED(AssembleShader(argv[1], buf, size, SASM_VERIFY_OPTIMIZATIONS, NULL, &blah2, NULL, NULL, NULL, NULL, 0))) {
			printf("shader doesn't break with optimizer on!\n");
			hr = E_FAIL;
		}
		if(SUCCEEDED(hr) && FAILED(AssembleShader(argv[1], buf, size, SASM_DONOTOPTIMIZE, NULL, &blah, &err, NULL, NULL, NULL, 0))) {
			if(strstr((char*)XGBuffer_GetBufferPointer(err), "Too many microcode instructions")) {
			} else {
				printf("shader doesn't assemble with no optimizations !\n");
				hr = E_FAIL;
			}
			RELEASE(err);
		}
		if(SUCCEEDED(hr)) {
			for(i = size - 1; i >= 0; i--) {
				if(buf[i] == '\n') {
					b = buf[i+1];
					if(b == '\0') continue;
					buf[i+1] = ';'; //comment this line
					
					if(FAILED(AssembleShader(argv[1], buf, size, SASM_DONOTOPTIMIZE, NULL, &blah, &err, NULL, NULL, NULL, 0))) {
						if(strstr((char*)XGBuffer_GetBufferPointer(err), "Too many microcode instructions")) {
						} else {
							printf("doesn't assemble\n");
							buf[i+1] = b;
						}
					} 
					if(SUCCEEDED(AssembleShader(argv[1], buf, size, SASM_VERIFY_OPTIMIZATIONS, NULL, &blah2, NULL, NULL, NULL, NULL, 0))) {
						buf[i+1] = b;
					}

					RELEASE(err);
					RELEASE(blah);
					RELEASE(blah2);
				}
			}
		}
	}

	if(SUCCEEDED(hr)) {
		DWORD head;
		bool dontwrite = false;
		DWORD blah;
		for(head = 0; head < size; head++) {
			if(buf[head] == ';') dontwrite=true;
			if(!dontwrite) WriteFile(out, &buf[head], 1, &blah, 0);
			if(buf[head] == '\n') dontwrite=false;
		}
	}

	if(buf) free(buf);
	if(in != INVALID_HANDLE_VALUE) CloseHandle(in);
	if(out != INVALID_HANDLE_VALUE) CloseHandle(out);

	_asm {int 3};
	return hr;
}