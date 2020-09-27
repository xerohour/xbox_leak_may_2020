////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// File: Library/Support/Shell/ShellToolKit.h
// Purpose: General Purpose Functions for Shell Utilities
// Author: Sam Frantz
// Date: 08-OKT-2000
//---------------------------------------------------------------------------

#ifndef SHELLTOOLKIT_H
#define SHELLTOOLKIT_H

#include <library/common/prelude.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAXPATHLEN		(2048)
#define MAX_INSTR			(1024)
#define IDENTIFIERLEN	(256)
#define FAIL				(-1)
#define SUCCEED			(0)

#define MIN(x,y)        (((x) < (y)) ? (x) : (y))
#define EOS						((char) 0)
#define PATHDELIM               '\\'
#define PATH_SEPARATOR          ';'
#define simple_name(filename)   (strchr(filename, (int) PATHDELIM) == (char *) NULL)

#define MAX_OPTIONS	52
#define PRINT_BYTE(x)	((x >= ' ' && x <= '~') ? x : '.')	// used by PrintDump()

//---------------------------------------------------------------------------

class ShellToolKit
	{
	public:
		char	legalOptionChars[IDENTIFIERLEN];
		char	optionSet[MAX_OPTIONS];
		char	*optionArg[MAX_OPTIONS];
		int	optionCount;

		BOOL	verbose;
		BOOL	interactive;
		int	startArg;
		FILE	*outFp;
		FILE	*inFp;
		FILE	*screenFp;
		char	dirName[MAXPATHLEN];
		char	pgmName[MAXPATHLEN];
		char	pgmExt[IDENTIFIERLEN];
		char	inputFileName[MAXPATHLEN];
		char	outputFileName[MAXPATHLEN];
		char	logFileBaseName[MAXPATHLEN];
		int	logFileNumber;
		ShellToolKit(void)
			{
			startArg = 1;
			verbose = FALSE;
			outFp = (FILE *) NULL;
			screenFp = stdout;
			inFp = stdin;
			interactive = TRUE;
			outputFileName[0] = (char) NULL;
			inputFileName[0] = (char) NULL;
			logFileBaseName[0] = (char) NULL;
			logFileNumber = 0;
			}
		~ShellToolKit(void) { Shut(); }
		int Init(int argc, char *argv[], char *legalOptionChars = (char *) NULL);
		void Shut(void)
			{
			if (outFp != (FILE *) NULL)
				{
				printf("\nOutput is in \"%s\".\n", outputFileName);
				fclose(outFp);
				}
			if (inFp != stdin)
				fclose(inFp);
			}
		void OpenNextLogFile(void)
			{
			logFileNumber++;
			if (outFp != (FILE *) NULL)
				fclose(outFp);
			sprintf(outputFileName,"%s%d.log", logFileBaseName, logFileNumber);
			outFp = fopen(outputFileName,"w");
			}
		void CloseLogFile(void)
			{
			if (outFp != (FILE *) NULL)
				{
				fclose(outFp);
				outFp = (FILE *) NULL;
				}
			}
		void Print(char *format, ...);
		void VerbosePrint(char *format, ...);
		void PrintDump(unsigned char *bp, int len, int maxlen=0, BOOL dumpASCII=TRUE);
		void VerbosePrintDump(unsigned char *bp, int len, int maxlen=0, BOOL dumpASCII=TRUE);
		void PrintSetup(char *filename = (char *) NULL, FILE *screenFp = stdout);
		int StripTrailingWhitespace(char *instr);
		int SubstrIndex(char *instr, char *substr);
		int TranslateChars(char *instr, char *outstr, char from, char to);;
		int BaseName(char *instr, char *dir, char *bname, char *ext, char newSlash);
		int AnalyzeFileSpec(char *instr, char &driveLetter, char *dir, char *bname, char *ext);
		int GetYorNwithDefault(int defaultBool);
		int GetPositiveInt(void);

#ifdef WIN32
		int CheckPath( char *pBaseName,		// base name of file to find
						   char *pRelDir,			// relative directory path
						   char *pSearchPath,	// NULL terminated search paths
							int lSearchPath,		// length of search path string
							char *pReturn,			// return path buffer pointer
							int sReturn	);			// size of return path buffer
		int FindFileInPath( char *pBaseName,	// base name of file to find
						        char *pRelDir,		// relative directory path
							  	  char *pPathStr,	// NULL terminated search paths
								  char path_delim,	// path delimiter character
								  char *pReturn,		// return path buffer pointer
								  int sReturn );		// size of return path buffer
		int FindFileInEnvPath(char *defaultFname, char *envPathVar, char *resultFname);
#endif // WIN32

		int GetStandardProgramOptions(int argc, char *argv[],	char *legalOptionChars);
		BOOL CheckProgramFlag(int argc, char *argv[], char flagChar);
		BOOL CheckProgramWordFlag(int argc, char *argv[], char *flagStr);
		BOOL GetProgramOptionWithParameter(int argc, char *argv[],
													  char flagChar, char *paramStr);
		BOOL GetProgramWordOptionWithParameter(int argc, char *argv[],
															char *flagStr, char *&paramStr);
		void InitOptionChars(char *optionChars)
			{
			int	i;

			if (optionChars != (char *) NULL)
				strcpy(legalOptionChars,optionChars);
			for (i=0; i<MAX_OPTIONS; i++)
				{
				optionSet[i] = (char) NULL;
				optionArg[i] = (char *) NULL;
				}
			optionCount = 0;
			}
		void SetOptionFlag(char optChar)
			{
			BOOL	done = FALSE;
			for (int i=0; i < optionCount && done == FALSE; i++)
				{
				if (optionSet[i] == optChar)
					done = TRUE;
				}
			if (!done)
				{
				if (optionCount < MAX_OPTIONS)
					optionSet[optionCount++] = optChar;
				}
			}
		BOOL GetOptionFlag(char optChar)
			{
			BOOL	done = FALSE;
			for (int i=0; i < optionCount && done == FALSE; i++)
				{
				if (optionSet[i] == optChar)
					done = TRUE;
				}
			return done;
			}
		void SetOptionParameter(char optChar, char *optArg)
			{
			BOOL	done = FALSE;
			for (int i=0; i < optionCount && done == FALSE; i++)
				{
				if (optionSet[i] == optChar)
					{
					optionArg[i] = optArg;
					done = TRUE;
					}
				}
			if (!done)
				{
				if (optionCount < MAX_OPTIONS)
					{
					optionArg[optionCount] = optArg;
					optionSet[optionCount++] = optChar;
					}
				}
			}
		char * GetOptionParameter(char optChar)
			{
			char *done = (char *) NULL;
			for (int i=0; i < optionCount && done == (char *) NULL; i++)
				{
				if (optionSet[i] == optChar)
					done = optionArg[i];
				}
			return done;
			}
	};

#endif
