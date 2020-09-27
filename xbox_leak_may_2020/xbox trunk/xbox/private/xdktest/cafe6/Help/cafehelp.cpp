/////////////////////////////////////////////////////////////////////////////
// cafehelp.cpp
//
// email	date		change
// briancr	10/21/94	created
//
// copyright 1994 Microsoft

// cafehelp will take a list of files with comments (see specification below)
// and generate an RTF file for compilation with hc31.

// comment specification:
// BEGIN_HELP_COMMENT
// Function: <return type> <class name::><function name>(<parameter list>)
// Description: <description to eol>
// Return: <return value description to eol>
// Param: <param name> <param description to eol>
// END_HELP_COMMENT

#include "stdafx.h"
#include "cafehelp.h"

void main(int argc, char **argv)
{
	CStringList listFiles;
	CStdioFile fRTF;

	// process cmd line args
	if (ProcessCmdLine(argc, argv)) {

		// read the list of files
		if (ReadFileList(strListFile, listFiles)) {

			// open the RTF file
			if (fRTF.Open(strRTFFile, CFile::modeCreate | CFile::modeWrite)) {

				// generate header in RTF file
				RTFHeader(fRTF);

				// iterate through each file in the list and read all comment blocks
				for (POSITION pos = listFiles.GetHeadPosition(); pos != NULL; ) {
					ProcessCommentBlocks(listFiles.GetNext(pos), fRTF);
				}

				// generate end of RTF file
				RTFEnd(fRTF);

				// close the RTF file
				fRTF.Close();
			}
			else {
				Error("Unable to open the RTF file (%s).", (LPCTSTR)strRTFFile);
			}
		}
		else {
			Error("Unable to open the list of files (%s).", (LPCTSTR)strListFile);
		}
	}
	else {
		Usage();
	}

	// success
	exit(0);
}

BOOL ReadFileList(CString strFilename, CStringList &listFiles)
{
	CStdioFile fList;
	CString strBuffer;

	// clear the list of files
	listFiles.RemoveAll();

	Debug("Reading file list (%s)", (LPCTSTR)strFilename);

	// open the file list file for reading
	if (fList.Open(strFilename, CFile::modeRead)) {

		// read each line from the file until eof is reached
		while (fList.ReadString(strBuffer.GetBuffer(512), 511)) {
			strBuffer.ReleaseBuffer();

			// allow a semicolon (;) as the first character to denote the line as a comment
			if (strBuffer.GetAt(0) != ';') {

				// remove trailing \n
				strBuffer = EliminateTrailingChars(strBuffer, "\n");

				// add this filename to the list of files
				listFiles.AddTail(strBuffer);
			}
		}

		// close the file list file
		fList.Close();

		return TRUE;
	}

	return FALSE;
}

void RTFHeader(CStdioFile &fRTF)
{
	Debug("Writing RTF header.");

	for (int i = 0; !cstrRTFHeader[i].IsEmpty(); i++) {
		fRTF.WriteString(cstrRTFHeader[i]);
	}
}

void ProcessCommentBlocks(CString strFilename, CStdioFile &fRTF)
{
	Debug("Processing file (%s)", (LPCTSTR)strFilename);

	CStdioFile fSrc;
	BOOL bComment = FALSE;
	BOOL bReadingComment = FALSE;

	// open the file
	if (fSrc.Open(strFilename, CFile::modeRead)) {
		int nLine = 0;
		CString strLine;
		BOOL fFunction;
		BOOL fDescription;
		BOOL fReturn;
		BOOL fParam;

		// read each line from the file until eof
		while (fSrc.ReadString(strLine.GetBuffer(512), 511)) {
			strLine.ReleaseBuffer();
			nLine++;

			// are we currently reading a comment?
			if (bReadingComment) {

				// figure out which description it is

				// end of comment?
				if (strLine.Find(cstrEndComment) != -1) {
					// write out end of comment
					RTFEndOfFunction(strLine, strFilename, nLine, fRTF);
					// mark that we're done reading the comment
					bReadingComment = FALSE;
				}
				else {
					// function?
					if (strLine.Find(cstrFunction) != -1) {
						// be sure this line isn't entered twice in the same block
						if (!fFunction) {
							RTFFunction(strLine, strFilename, nLine, fRTF);
							fFunction = TRUE;
						}
						// function line occured more than once in the same block
						else {
							Warning("Multiple %s entries, ignoring this entry (%s, %d).", (LPCTSTR)cstrFunction, (LPCTSTR)strFilename, nLine);
						}
					}
					else {
						// description?
						if (strLine.Find(cstrDescription) != -1) {
							// we must have a function line before a description
							if (fFunction) {
								// be sure this line isn't entered twice in the same block
								if (!fDescription) {
									RTFDescription(strLine, strFilename, nLine, fRTF);
									fDescription = TRUE;
								}
								// description line occurs more than once in the same block
								else {
									Warning("Multiple %s entries, ignoring this entry (%s, %d).", (LPCTSTR)cstrDescription, (LPCTSTR)strFilename, nLine);
								}
							}
							// must have function line before description
							else {
								Warning("%s line before %s, ignoring this entry (%s, %d).", (LPCTSTR)cstrDescription, (LPCTSTR)cstrFunction, (LPCTSTR)strFilename, nLine);
							}
						}
						else {
							// return?
							if (strLine.Find(cstrReturn) != -1) {
								// we must have a function line before return
								if (fFunction) {
									// be sure this line isn't entered twice in the same block
									if (!fReturn) {
										RTFReturn(strLine, strFilename, nLine, fRTF);
										fReturn = TRUE;
									}
									// return line occurs more than once in the same block
									else {
										Warning("Multiple %s entries, ignoring this entry (%s, %d).", (LPCTSTR)cstrReturn, (LPCTSTR)strFilename, nLine);
									}
								}
								// must have function line before return
								else {
									Warning("%s line before %s, ignoring this entry (%s, %d).", (LPCTSTR)cstrReturn, (LPCTSTR)cstrFunction, (LPCTSTR)strFilename, nLine);
								}
							}
							else {
								// parameter?
								if (strLine.Find(cstrParam) != -1) {
									// we must have a function line before a parameter
									if (fFunction) {
										// write out the parameter header if this is the first parameter for this block
										if (!fParam) {
											RTFParamHdr(strLine, strFilename, nLine, fRTF);
										}
										RTFParam(strLine, strFilename, nLine, fRTF);
										fParam = TRUE;
									}
									// must have function line before parameter
									else {
										Warning("%s line before %s, ignoring this entry (%s, %d).", (LPCTSTR)cstrParam, (LPCTSTR)cstrFunction, (LPCTSTR)strFilename, nLine);
									}
								}
								else {
									// begin comment?
									if (strLine.Find(cstrBeginComment) != -1) {
										Warning("%s found within comment block (%s, %d).", (LPCTSTR)cstrBeginComment, (LPCTSTR)strFilename, nLine);
									}
									else {
										Warning("Unknown line (%s, %d).", (LPCTSTR)strFilename, nLine);
									}
								}
							}
						}
					}
				}
			}
			// we're not reading a comment
			else {
				// does this line mark the beginning of a comment?
				if (strLine.Find(cstrBeginComment) != -1) {
					// start of a comment block
					// mark that we found at least one comment in this file
					bComment = TRUE;
					// mark that we're now reading a comment
					bReadingComment = TRUE;
					// mark indicators as false
					fFunction = FALSE;
					fDescription = FALSE;
					fReturn = FALSE;
					fParam = FALSE;
				}
			}
		}

		// close the file
		fSrc.Close();

		// if we still think we're reading a comment, let the user know
		if (bReadingComment) {
			Warning("No %s found before reaching end of file (%s).", (LPCTSTR)cstrEndComment, (LPCTSTR)strFilename);
		}

		// if we didn't find any comment blocks in the file, let the user know
		if (!bComment) {
			Warning("No comment blocks found in file (%s).", (LPCTSTR)strFilename);
		}
	}
	// unable to open the file
	else {
		Warning("Unable to open file to read comment blocks (%s).", (LPCTSTR)strFilename);
	}
}

void RTFFunction(CString strLine, CString strFilename, int nLine, CStdioFile &fRTF)
{
	Debug("Processing function: %s", (LPCTSTR)strLine);

	CString strReturnType;
	CString strClassName;
	CString strFunction;
	CString strParamList;
	int nPos;

	// find the function keyword and move to the end of it
	nPos = strLine.Find(cstrFunction);
	strReturnType = strLine.Mid(nPos+cstrFunction.GetLength());

	// find the begining of the return type (eliminate white space)
	strReturnType = EliminateLeadingChars(strReturnType, " ");

	// find the end of the return type
	nPos = strReturnType.FindOneOf(" \t");
	if (nPos == -1) {
		nPos = 0;
	}
	// if the first space is after the first paren, then there is no return type
	// i.e. spaces separating parameters
	if (nPos > strReturnType.FindOneOf("(")) {
		nPos = 0;
	}

	// the class name starts after the return type
	strClassName = strReturnType.Mid(nPos);
	strReturnType = strReturnType.Left(nPos);

	// find the beginning of the class name
	strClassName = EliminateLeadingChars(strClassName, " ");

	// find the end of the class name
	nPos = strClassName.FindOneOf(":");
	if (nPos == -1) {
		nPos = 0;
	}
	// if the first : is after the first paren, then there is no class name
	// i.e. class::name is used in the parameter list
	if (nPos > strClassName.FindOneOf("(")) {
		nPos = 0;
	}

	// the function starts after the class name
	strFunction = strClassName.Mid(nPos);
	strClassName = strClassName.Left(nPos);

	// find the beginning of the function (skip :: if it exists)
	strFunction = EliminateLeadingChars(strFunction, " :");

	// find the end of the function
	nPos = strFunction.FindOneOf("(");
	if (nPos == -1) {
		nPos = 0;
	}

	// the parameter list starts after the function
	strParamList = strFunction.Mid(nPos);
	strFunction = strFunction.Left(nPos);

	// remove trailing \n
	strParamList = EliminateTrailingChars(strParamList, "\n");

	// write the function to the RTF file
	fRTF.WriteString(cstrBeginFunc);
	fRTF.WriteString(cstrBeginIDFunc);
	fRTF.WriteString("id_"+strClassName+"_"+strFunction);
	fRTF.WriteString(cstrEndIDFunc);
	fRTF.WriteString(cstrBeginIndexFunc);
	if (!strClassName.IsEmpty()) {
		fRTF.WriteString(strClassName+"; ");
	}
	fRTF.WriteString(strFunction);
	fRTF.WriteString(cstrEndIndexFunc);
	fRTF.WriteString(cstrBeginTopicFunc);
	if (!strClassName.IsEmpty()) {
		fRTF.WriteString(strClassName+"::");
	}
	fRTF.WriteString(strFunction);
	fRTF.WriteString(cstrEndTopicFunc);
	fRTF.WriteString(cstrBeginTextFunc);
	if (!strReturnType.IsEmpty()) {
		fRTF.WriteString(strReturnType+" ");
	}
	if (!strClassName.IsEmpty()) {
		fRTF.WriteString(strClassName+"::");
	}
	fRTF.WriteString(strFunction+strParamList);
	fRTF.WriteString(cstrEndTextFunc);

	UNREFERENCED_PARAMETER(strFilename);
	UNREFERENCED_PARAMETER(nLine);
}

void RTFDescription(CString strLine, CString strFilename, int nLine, CStdioFile &fRTF)
{
	Debug("Processing description: %s", (LPCSTR)strLine);

	CString strDescription;
	int nPos;

	// find the description keyword and move to the end of it
	nPos = strLine.Find(cstrDescription);
	strDescription = strLine.Mid(nPos+cstrDescription.GetLength());

	// find the begining of the description (eliminate white space)
	strDescription = EliminateLeadingChars(strDescription, " ");

	// remove trailing \n
	strDescription = EliminateTrailingChars(strDescription, "\n");

	// write the description to the RTF file
	fRTF.WriteString(cstrBeginDesc);
	fRTF.WriteString(strDescription);
	fRTF.WriteString(cstrEndDesc);

	UNREFERENCED_PARAMETER(strFilename);
	UNREFERENCED_PARAMETER(nLine);
}

void RTFReturn(CString strLine, CString strFilename, int nLine, CStdioFile &fRTF)
{
	Debug("Processing return: %s", (LPCSTR)strLine);

	CString strReturn;
	int nPos;

	// find the description keyword and move to the end of it
	nPos = strLine.Find(cstrReturn);
	strReturn = strLine.Mid(nPos+cstrReturn.GetLength());

	// find the begining of the return (eliminate white space)
	strReturn = EliminateLeadingChars(strReturn, " ");

	// remove trailing \n
	strReturn = EliminateTrailingChars(strReturn, "\n");

	// write the return desc to the RTF file
	fRTF.WriteString(cstrBeginReturn);
	fRTF.WriteString(strReturn);
	fRTF.WriteString(cstrEndReturn);

	UNREFERENCED_PARAMETER(strFilename);
	UNREFERENCED_PARAMETER(nLine);
}

void RTFParamHdr(CString strLine, CString strFilename, int nLine, CStdioFile &fRTF)
{
	Debug("Processing param hdr: %s", (LPCSTR)strLine);

	// write the parameter header to the RTF file
	fRTF.WriteString(cstrParamHdr);

	UNREFERENCED_PARAMETER(strFilename);
	UNREFERENCED_PARAMETER(nLine);
}

void RTFParam(CString strLine, CString strFilename, int nLine, CStdioFile &fRTF)
{
	Debug("Processing param: %s", (LPCSTR)strLine);

	CString strParam;
	CString strParamDesc;
	int nPos;

	// find the parameter keyword and move to the end of it
	nPos = strLine.Find(cstrParam);
	strParam = strLine.Mid(nPos+cstrParam.GetLength());

	// find the begining of the parameter (eliminate white space)
	strParam = EliminateLeadingChars(strParam, " ");

	// find the end of the parameter
	nPos = strParam.FindOneOf(" \t");
	if (nPos == -1) {
		nPos = 0;
	}

	// the parameter description starts after the parameter
	strParamDesc = strParam.Mid(nPos);
	strParam = strParam.Left(nPos);

	// find the beginning of the parameter description
	strParamDesc = EliminateLeadingChars(strParamDesc, " ");

	// remove trailing \n
	strParamDesc = EliminateTrailingChars(strParamDesc, "\n");

	// write the parameter to the RTF file
	fRTF.WriteString(cstrBeginParamName);
	fRTF.WriteString(strParam);
	fRTF.WriteString(cstrEndParamName);
	fRTF.WriteString(cstrBeginParamDesc);
	fRTF.WriteString(strParamDesc);
	fRTF.WriteString(cstrEndParamDesc);

	UNREFERENCED_PARAMETER(strFilename);
	UNREFERENCED_PARAMETER(nLine);
}

void RTFEndOfFunction(CString strLine, CString strFilename, int nLine, CStdioFile &fRTF)
{
	Debug("Processing end of function: %s", (LPCSTR)strLine);

	CString strLineNum;
	int nPos;

	// write the reference for this function to the RTF file
	fRTF.WriteString(cstrBeginFuncRef);
	// only include filename
	nPos = strFilename.ReverseFind('\\');
	if (nPos != -1) {
		strFilename = strFilename.Mid(nPos+1);
	}
	fRTF.WriteString(strFilename);
	strLineNum.Format(", %d", nLine+1);
	fRTF.WriteString(strLineNum);
	fRTF.WriteString(cstrEndFuncRef);

	// write the end of function to the RTF file
	fRTF.WriteString(cstrEndFunc);

	UNREFERENCED_PARAMETER(nLine);
}


CString EliminateLeadingChars(CString str, CString strSet)
{
	while (str.FindOneOf(strSet) == 0) {
		str = str.Mid(1);
	}
	return str;
}

CString EliminateTrailingChars(CString str, CString strSet)
{
	while (str.FindOneOf(strSet) == str.GetLength()-1) {
		str = str.Left(str.GetLength()-1);
	}
	return str;
}

void RTFEnd(CStdioFile &fRTF)
{
	Debug("Writing RTF end.");

	// write the end of the RTF file
	fRTF.WriteString(cstrRTFEnd);
}

void Error(LPCSTR szMsg, ...)
{
	char szBuf[512];

	va_list marker;
	va_start(marker, szMsg);
	_vsnprintf(szBuf, 512, szMsg, marker);
	va_end(marker);

	fprintf(stderr, "CAFEHelp Error: %s\n", szBuf);
	exit(-2);
}

void Warning(LPCSTR szMsg, ...)
{
	char szBuf[512];

	va_list marker;
	va_start(marker, szMsg);
	_vsnprintf(szBuf, 512, szMsg, marker);
	va_end(marker);

	fprintf(stderr, "CAFEHelp Warning: %s\n", szBuf);
}

void Debug(LPCSTR szMsg, ...)
{
	#ifdef _DEBUG

		char szBuf[512];

		va_list marker;
		va_start(marker, szMsg);
		_vsnprintf(szBuf, 512, szMsg, marker);
		va_end(marker);

		fprintf(stderr, "CAFEHelp Debug: %s\n", szBuf);
	#else
		UNREFERENCED_PARAMETER(szMsg);
	#endif // _DEBUG
}

BOOL ProcessCmdLine(int argc, char **argv)
{
	Debug("Processing command line args.");

	int i;

	// loop through args
	for (i = 1; i < argc; i++) {

		// ignore preceeding - or / by looking at second char
		switch (argv[i][1]) {
			// set the list file
			case 'l': {
				// parameter following this switch is the filename
				i++;
				if (i < argc) {
					strListFile = argv[i];
				}
				else {
					Usage();
					return FALSE;
				}
				break;
			}
			// set the output file
			case 'o': {
				// parameter following this switch is the filename
				i++;
				if (i < argc) {
					strRTFFile = argv[i];
				}
				else {
					Usage();
					return FALSE;
				}
				break;
			}
			case '?': {
				// display usage information
				Usage();
				return FALSE;
			}
			default: {
				Warning("Unrecognized switch: %s", argv[i]);
				Usage();
				return FALSE;
			}
		}
	}

	Debug("File list filename = %s", (LPCSTR)strListFile);
	Debug("RTF filename = %s", (LPCSTR)strRTFFile);

	return TRUE;
}

void Usage(void)
{
	fprintf(stdout, "\nCAFE Help File Generation System\n");
	fprintf(stdout, "Copyright 1994 Microsoft\n\n");
	fprintf(stdout, "Usage: cafehelp [-l <filename> -o <filename> -?]\n");
	fprintf(stdout, "-l <filename> : specify filename of file containing list of files to parse\n");
	fprintf(stdout, "  (defaults to cafehelp.lst)\n");
	fprintf(stdout, "-o <filename> : specify filename of output file\n");
	fprintf(stdout, "  (defaults to cafehelp.rtf)\n");
	fprintf(stdout, "-? : displays this usage message\n");
	#ifdef _DEBUG
		fprintf(stdout, "\n(debug build)\n");
	#endif // _DEBUG

	exit(-1);
}
