/*******************************************************************
MODULE: Validate.h
AUTHOR: JohnMil
DATE:	2-5-92

Copyright (c) 1992 Microsoft Corporation


This is a general-purpose header file for all validation routines.

*******************************************************************/

#include <windows.h>
#include <stdio.h>

void	    ErrorPrint(HANDLE hConOut,HANDLE hLog,char *Module,int CaseNo, char *Message);
int	    ValPrintf(HANDLE hConOut,char *Format, ...);
