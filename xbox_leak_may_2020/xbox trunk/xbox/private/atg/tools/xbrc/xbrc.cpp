//-----------------------------------------------------------------------------
//  
//  File: xbrc.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Defines the entry point for the console application.
//  
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "XBContent.h"

// Global content handler for handling error messages from the bundler library.
XBContent *g_pXBContent = NULL;

int _cdecl main(int argc, char* argv[])
{
    if (argc<2) {
        fprintf(stderr, 
            "Xbox Resource Compiler v0.4\n"
            "\txbrc file.xdx\n"
            "Compiles a file in the XML DirectX format (.xdx)\n"
            "to an Xbox resource file (.xbr) which can be loaded\n"
            "directly into an application or previewed using the\n"
            "xbview tool.\n"
            );
        return -1;
    }

    // Create an instance of the XML parser
    CoInitialize(NULL); 
    ISAXXMLReader* pRdr = NULL;
    HRESULT hr = CoCreateInstance( __uuidof(SAXXMLReader40), NULL, CLSCTX_ALL, __uuidof(ISAXXMLReader), (void **)&pRdr);
    if(FAILED(hr)) 
    {
		if (hr == REGDB_E_CLASSNOTREG)
		{
			fprintf(stderr, "xbrc: Failed to create SAXXMLReader40 instance. Please install msxml4.\nTrying previous version.\n");
			hr = CoCreateInstance( __uuidof(SAXXMLReader), NULL, CLSCTX_ALL, __uuidof(ISAXXMLReader), (void **)&pRdr);
		}
		if (FAILED(hr))
		{
			fprintf(stderr, "xbrc: Failed to create XML reader (error %08X).\n", hr);
			CoUninitialize();
			return hr;
		}
    }

    // Set the XML parser to use our callback routines on the tokens
    g_pXBContent = new XBContent;
    if (g_pXBContent == NULL)
        hr = E_OUTOFMEMORY;
    if (FAILED(hr))
    {
        fprintf(stderr, "xbrc: Failed to create Xbox content parser (error %08X).\n", hr);
        CoUninitialize();
        return hr;
    }
    pRdr->putContentHandler(g_pXBContent);  // the XBContent class supports both the content and error handling
    pRdr->putErrorHandler(g_pXBContent);

    // Parse the input files, including wildcards
    for (int i = 1; i < argc; i++)
    {
        WIN32_FIND_DATA FindFileData;
        HANDLE hFind;
        hFind = FindFirstFile(argv[i], &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            fprintf(stderr, "xbrc: Can not find matching files for \"%s\".\n", argv[i]);
            hr = E_FAIL;
            break;
        }
        do {
            const int len = 10000;
            wchar_t URL[len];
            mbstowcs( URL, FindFileData.cFileName, len - 1);
            hr = pRdr->parseURL(URL);
            if (FAILED(hr))
            {
                if (hr == E_ACCESSDENIED)
                    fwprintf(stderr, L"xbrc: Access denied to \"%s\".\n");
                else if (hr == INET_E_OBJECT_NOT_FOUND)
                    fwprintf(stderr, L"xbrc: Object \"%s\" not found.\n", URL);
                else if (hr == INET_E_DOWNLOAD_FAILURE)
                    fwprintf(stderr, L"xbrc: Download failure for \"%s\".\n", URL);
                else
                    fwprintf(stderr, L"xbrc: Parsing document \"%s\" failed with error %08x.\n", URL, hr);
            }
        } while (FindNextFile(hFind, &FindFileData));
        FindClose(hFind);
    }

    // Cleanup
    pRdr->Release();
    CoUninitialize();
    return hr;
}
