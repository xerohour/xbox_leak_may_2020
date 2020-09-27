#pragma once

//
// Microsoft (R) 32-bit C/C++ Optimizing Compiler Front-End
// Copyright (C) Microsoft Corp 1984-1999. All rights reserved.
//
// IDE_parseinfo.h
//
// Type declarations for parse info for IDE use throughout the compiler.  
//

// Typedefs used for entrypoints into FEACP
typedef void (WINAPI * PYCBCALLBACK)(char *pszFilename, bool fNewHeader);
typedef void * (__cdecl * PFNALLOC)(size_t);
typedef void (WINAPI * PFNREPORTERROR)(DWORD dwErrId, DWORD dwErrType, SZ szFileName, DWORD begLine, DWORD endLine, WORD begCol, WORD endCol, SZ szErrTxt);
typedef void (WINAPI *PFNREPORTINC)(const char * strHeaderFile);


// struct definition of struct where column info is maintained
typedef struct IDE_ColumnInfo_s {
	__int32 lineBegin;
	unsigned __int32	colEnd		: 12,
						colBegin	: 12,
						lineEnd		: 8;	// line number for colEnd is 
											// lineBegin + lineEnd
} IDE_ColumnInfo_t;



// The kinds of things that we may produce column info for
__declspec(selectany) char * IDE_kind_str[] = { "Object", "Function", "Class", "Struct", "Union", "Namespace", "Enum", "Template", "unknown" };

// NOTE: If you add any types, please add them before max_e. Also, update 
// IDE_kind_str[][] accordingly if you add or rearrange members of the enum
// declared below
typedef struct IDE_Decl_s {
	enum kind_t { object_e, function_e,  class_e, struct_e, union_e, namespace_e, enum_e, template_e, max_e };
	
	kind_t kind; // what "kind" of declaration is it?

	IDE_ColumnInfo_t declarator;	// eg: "int *(*foo)()" = 0; between quotes 
									// is the declarator

	IDE_ColumnInfo_t name;			// "foo" in the above example

	IDE_ColumnInfo_t init;			// "= 0" in the above example
									// note: "init" is "default arg" for a 
									// formal_e

	struct IDE_Decl_s *next;		// the "next" node at this same lexical 
									// level (eg, formal list, list of mbr 
									// funcs, etc)

	union {
		// function_e
		struct {
			IDE_Decl_s *formalList;	// list of formal arguments for a function_e
			IDE_Decl_s *cv_modifier;// cv_modifier col info for a function_e
		};
		
		struct {
			IDE_Decl_s *memberList;		// class_e, struct_e, union_e, 
										// namespace_e, enum_e, template_e
			IDE_Decl_s *baseClassList;	// class_e, struct_e, template_e
			IDE_Decl_s *templateLink;	// template_e
		};
    };

} IDE_Decl_t;


