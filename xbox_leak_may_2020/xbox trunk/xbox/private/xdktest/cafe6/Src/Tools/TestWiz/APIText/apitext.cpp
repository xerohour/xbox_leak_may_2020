/////////////////////////////////////////////////////////////////////////////
// apitext.cpp
//
// email		date		change
// enriquep		10/28/94	created
//
// copyright 1994 Microsoft

// apitext will take a list of files with comments (see specification below)
// and generate a text file that can be read by test wizard and cafehelp.

// comment specification:
// BEGIN_HELP_COMMENT
// Function: <return type> <class name::><function name>(<parameter list>)
// Description: <description to eol>
// Return: <return value description to eol>
// Param: <param name> <param description to eol>
// END_HELP_COMMENT

#include "stdafx.h"
#include "apitext.h"

CParamList paramList;
CStringArray strParamArray;
CStringArray strFinalParamArray;

void main(int argc, char **argv)
{
	CStringList listFiles;
	CStdioFile fTXT;

	// process cmd line args
	if (ProcessCmdLine(argc, argv))
	{

		// read the list of files
		if (ReadFileList(strListFile, listFiles))
		{

			// open the TXT file
			if (fTXT.Open(strTXTFile, CFile::modeCreate | CFile::modeWrite))
			{

				// iterate through each file in the list and read all comment blocks
				for (POSITION pos = listFiles.GetHeadPosition(); pos != NULL; )
				{
					ProcessCommentBlocks(listFiles.GetNext(pos), fTXT);
				}

				// close the TXT file
				fTXT.Close();
			}
			else 
			{
				Error("Unable to open the TXT file (%s).", (LPCTSTR)strTXTFile);
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
	// exit(0);
}

BOOL ReadFileList(CString strFilename, CStringList &listFiles)
{
	CStdioFile fList;
	CString strBuffer;

	// clear the list of files
	listFiles.RemoveAll();

	Debug("Reading file list (%s)", (LPCTSTR)strFilename);

	// open the file list file for reading
	if (fList.Open(strFilename, CFile::modeRead))
	{

		// read each line from the file until eof is reached
		while (fList.ReadString(strBuffer.GetBuffer(512), 511))
		{
			strBuffer.ReleaseBuffer();

			// allow a semicolon (;) as the first character to denote the line as a comment
			if (strBuffer.GetAt(0) != ';')
			{

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

void ProcessCommentBlocks(CString strFilename, CStdioFile &fTXT)
{
	Debug("Processing file (%s)", (LPCTSTR)strFilename);

	CStdioFile fSrc;
	BOOL bComment = FALSE;
	BOOL bReadingComment = FALSE;
	BOOL bLookingForFunctionDecl = FALSE;

	CString strFuncBlock;
	CString strReturnBlock;
	CString strDescBlock;
	CString strParamBlock;

	// open the file
	if (fSrc.Open(strFilename, CFile::modeRead))
	{
		int nLine = 0;
		CString strLine;
		BOOL fFunction = FALSE;
		BOOL fDescription = FALSE;
		BOOL fReturn = FALSE;
		int nParams = 0;	// Counts the number of Param lines in a block

		// read each line from the file until eof
		while (fSrc.ReadString(strLine.GetBuffer(512), 511))
		{
			strLine.ReleaseBuffer();
			nLine++;

			// are we currently reading a comment?
			if (bReadingComment)
			{

				// figure out which description it is
				
				// ClassName or BaseClass or Category?
				if ( (strLine.Find(cstrClassName) != -1) || (strLine.Find(cstrBaseClass) != -1) || (strLine.Find(cstrCategory) != -1) )
				{
					fTXT.WriteString(strLine);
					continue;
				}
				// End of Class Help comment
				if ( strLine.Find(cstrEndClassHelp) != -1 )
				{
					bReadingComment = FALSE;
					// REVIEW: Compare block info with class declaration
					bLookingForFunctionDecl = FALSE; 
					fTXT.WriteString(strLine);
					continue;
				}
					 
		
				// end of API comment?
				if (strLine.Find(cstrEndComment) != -1)
				{
					bReadingComment = FALSE;
					// Compare block info with function declaration
					bLookingForFunctionDecl = TRUE; 
				}
				else 
				{
					// function?
					if (strLine.Find(cstrFunction) != -1)
					{
						// be sure this line isn't entered twice in the same block
						if (!fFunction)
						{
							strFuncBlock = strLine;
							fFunction = TRUE;
						}
						// function line occured more than once in the same block
						else 
						{
							Warning("Multiple %s entries, ignoring this entry (%s, %d).", (LPCTSTR)cstrFunction, (LPCTSTR)strFilename, nLine);
						}
					}
					else 
					{
						// description?
						if (strLine.Find(cstrDescription) != -1) 
						{
							// we must have a function line before a description
							if (fFunction) 
							{
								// be sure this line isn't entered twice in the same block
								if (!fDescription) 
								{
									strDescBlock = strLine;
									fDescription = TRUE;
								}
								// description line occurs more than once in the same block
								else 
								{
									Warning("Multiple %s entries, ignoring this entry (%s, %d).", (LPCTSTR)cstrDescription, (LPCTSTR)strFilename, nLine);
								}
							}
							// must have function line before description
							else 
							{
								Warning("%s line before %s, ignoring this entry (%s, %d).", (LPCTSTR)cstrDescription, (LPCTSTR)cstrFunction, (LPCTSTR)strFilename, nLine);
							}
						}
						else 
						{
							// return?
							if (strLine.Find(cstrReturn) != -1) 
							{
								// we must have a function line before return
								if (fFunction) 
								{
									// be sure this line isn't entered twice in the same block
									if (!fReturn) 
									{
										strReturnBlock = strLine;
										fReturn = TRUE;
									}
									// return line occurs more than once in the same block
									else 
									{
										Warning("Multiple %s entries, ignoring this entry (%s, %d).", (LPCTSTR)cstrReturn, (LPCTSTR)strFilename, nLine);
									}
								}
								// must have function line before return
								else 
								{
									Warning("%s line before %s, ignoring this entry (%s, %d).", (LPCTSTR)cstrReturn, (LPCTSTR)cstrFunction, (LPCTSTR)strFilename, nLine);
								}
							}
							else 
							{
								// parameter?
								if (strLine.Find(cstrParam) != -1) 
								{
									// we must have a function line before a parameter
									if (fFunction) 
									{
										CString str = strLine;
										strParamArray.Add(str);
										nParams++;
									}
									// must have function line before parameter
									else 
									{
										Warning("%s line before %s, ignoring this entry (%s, %d).", (LPCTSTR)cstrParam, (LPCTSTR)cstrFunction, (LPCTSTR)strFilename, nLine);
									}
								}
								else 
								{
									// begin comment?
									if (strLine.Find(cstrBeginComment) != -1) 
									{
										Warning("%s found within comment block (%s, %d).", (LPCTSTR)cstrBeginComment, (LPCTSTR)strFilename, nLine);
									}
									else 
									{
										Warning("Unknown line (%s, %d).", (LPCTSTR)strFilename, nLine);
									}
								}
							}
						}
					}
				}
			}
			// we're not currently reading a comment
			else 
			{
				// Are we looking for a function declaration?
				if ( bLookingForFunctionDecl && ( !strLine.IsEmpty() ) )
				{
					strFuncBlock = VerifyFuncBlockIsUpToDate(strLine, strFuncBlock);
					strReturnBlock = VerifyReturnBlockIsUpToDate(strLine, strReturnBlock);
					VerifyParamBlockIsUpToDate( strLine );

					// write out block
					fTXT.WriteString(strFuncBlock);
					fTXT.WriteString(strDescBlock);
					fTXT.WriteString(strReturnBlock);
					WriteParams(fTXT);
					fTXT.WriteString("// " + cstrEndComment + '\n');

					strFinalParamArray.RemoveAll();
					strParamArray.RemoveAll();


					bLookingForFunctionDecl = FALSE;
				}

						
				// does this line mark the beginning of a comment?
				if ( (strLine.Find(cstrBeginComment) != -1) || (strLine.Find(cstrBeginClassHelp) != -1) )
				{
					// start of a comment block
					// mark that we found at least one comment in this file
					bComment = TRUE;
					// mark that we're now reading a comment
					bReadingComment = TRUE;
					// write out begin comment indicator
					fTXT.WriteString(strLine);
					// mark indicators as false
					fFunction = FALSE;
					fDescription = FALSE;
					fReturn = FALSE;
					nParams = 0;
				}
			}
		}

		// close the file
		fSrc.Close();

		// if we still think we're reading a comment, let the user know
		if (bReadingComment) 
		{
			Warning("No %s found before reaching end of file (%s).", (LPCTSTR)cstrEndComment, (LPCTSTR)strFilename);
		}

		// if we didn't find any comment blocks in the file, let the user know
		if (!bComment) 
		{
			Warning("No comment blocks found in file (%s).", (LPCTSTR)strFilename);
		}
	}
	// unable to open the file
	else 
	{
		Warning("Unable to open file to read comment blocks (%s).", (LPCTSTR)strFilename);
	}
}

void ReadParams(CString strLine)
{
	Debug("Reading Params in function: %s", (LPCSTR)strLine);
	
	ASSERT(!strLine.IsEmpty());
	
	paramList.RemoveAll();

    int iParam = 0;
	int nSeparator = strLine.Find("(");
	strLine = strLine.Mid(nSeparator + 1);   //  skip to open parenthesis
	nSeparator = strLine.SpanIncluding(" ").GetLength();
	strLine = strLine.Mid(nSeparator);   //  skip all spaces 

	if ( (strLine.Find("void") != -1) && (strLine.Find("*") == -1) )	// REVIEW: Will this work with params of type void
		return;			// takes no parameters	or uses PARAM to fill them in
	
	while( strLine[0] != ')' && strLine[0] != '\0' )
	{
		CParam* pParam = new CParam;
		while ( (iParam % 3) < 2)		// Only do this loop 2 times
		{
			nSeparator = strLine.FindOneOf(", \t=/)");
			if (nSeparator == -1)
			{
				Warning("Unexpected end of parameter list in (%s)", strLine);
				goto IncompleteParams;
			}
			switch( (iParam++) & (0x0001) )				  // REVIEW: We assume only 1 param type
			{
				case 0:		// parameter type
					pParam->m_strType = strLine.Left(nSeparator);
					break;
				case 1:		// parameter name
					pParam->m_strName = strLine.Left(nSeparator);
					break;
			}
			
			strLine = strLine.Mid(nSeparator);
			nSeparator = strLine.SpanIncluding(", \t").GetLength();	// advance to next '=' '*' '/' or param type
			strLine = strLine.Mid(nSeparator);

			while (strLine[0] == '*')
			{
				pParam->m_strType += '*';
				strLine = strLine.Mid(1); // Skip '*'
			}

			if ((strLine[0] == '=') || (strLine[0] == '/'))	// Look for default values
			{
				nSeparator = strLine.SpanIncluding("= /*\t").GetLength();	// advance to next '=' '*' '/' or param type
				strLine = strLine.Mid(nSeparator);
				nSeparator = strLine.FindOneOf(" *\t)");		
				pParam->m_strValue = strLine.Left(nSeparator);		// Store Default Value
				strLine = strLine.Mid(nSeparator);
				nSeparator = strLine.SpanIncluding(",*/ \t").GetLength();	// advance to next '=' '*' '/' or param type
				strLine = strLine.Mid(nSeparator);
			}  
		}
		paramList.AddTail(pParam);					// Add Param to list.
		iParam = 0;									// Reset so more parameters can be read.
	}
	IncompleteParams:	iParam = 0;
}

void WriteParams( CStdioFile &fTXT )
{
	Debug("Writing Parameters:");
	
	int nParam = strFinalParamArray.GetSize();
	
	for ( int i=0; i< nParam; i++)
	{
		fTXT.WriteString(strFinalParamArray[i]);
	}	
}

CString VerifyFuncBlockIsUpToDate(CString strLine, CString strFuncBlock)
{
	// Get Function Declaration, assumes it comes after END_HELP_COMMENT

	// Compare Function declarations
	int nStart = 0;

	if( strFuncBlock.Find("Function:") != -1)
	{
		nStart = strFuncBlock.Find(':');
		strFuncBlock = strFuncBlock.Mid( nStart + 1);
		//  skip all spaces to the Function return type
		nStart = strFuncBlock.SpanIncluding(" \t").GetLength();
		strFuncBlock = strFuncBlock.Mid(nStart);
		   
		//  skip all spaces to the Function return type
		nStart = strLine.SpanIncluding(" ").GetLength();
		strLine = strLine.Mid(nStart);   
		
		if (strLine != strFuncBlock)		// If they don't match update block with latest declaration
			Warning("Function block (%s) was out of date", strFuncBlock);
	}
	else
		Warning("Invalid Function block: (%s)", strFuncBlock);  // The key word Function: is not in block
	strFuncBlock = "// Function: " + strLine;  // Update Function block with latest declaration.
	return( strFuncBlock );
}

CString VerifyReturnBlockIsUpToDate(CString strLine, CString strReturnBlock)
{
	// Get Function Declaration, assumes it comes after END_HELP_COMMENT

	int nStart = 0;
		
	//  skip all spaces to the Function return type
	nStart = strLine.SpanIncluding(" \t").GetLength();
	strLine = strLine.Mid(nStart);
	//  get type
	nStart = strLine.FindOneOf(" \t");
	ASSERT( nStart >= 0);
	CString strReturn = strLine.Left( nStart );
	strLine = strLine.Mid ( nStart );
	//  skip spaces and look if the next char is *
	nStart = strLine.SpanIncluding(" \t").GetLength();
	strLine = strLine.Mid(nStart);
	if (strLine[0] == '*')
		strReturn = strReturn + '*';  // It is a * return type
	
	// Search for Return type in Return block
	if (strReturnBlock.Find(strReturn.GetBuffer( strReturn.GetLength() )) == -1)
	{
		Warning("Return type (%s) not found in Return Block", strReturn);
		strReturnBlock = "// Return: " + strReturn + '\n';  // Update Return type
	}

	if( strReturnBlock.Find("Return:") == -1)
	{
		Warning("Invalid Return block in cpp file: (%s)", strReturnBlock);  // The key word Return: is not in block
		strReturnBlock = "// Return: " + strReturn + '\n';  // Correct problem
	}
	
	return (strReturnBlock);
}

void VerifyParamBlockIsUpToDate( CString strLine )
{
	// Get Function Declaration, assumes it comes after END_HELP_COMMENT

	ReadParams(strLine);	// Loads declared params into Param List
	
	POSITION pos = paramList.GetHeadPosition();
	int nParam = strParamArray.GetSize();
	BOOL bIsListed = FALSE;
	int index = 0;

	if (paramList.IsEmpty())
	{
		CString str = "// Param: void, takes no parameters \n";
		strFinalParamArray.Add( str );  // Create new param block
		return;
	}
	
	while (pos != NULL)
	{
		CParam* pParam =  paramList.GetNext(pos);
		for (int i = 0; i < nParam; i++)
		{
			if (strParamArray[i].Find( pParam->m_strName.GetBuffer(pParam->m_strName.GetLength())) != -1 )
			{
				bIsListed = TRUE;
				index = i;
				break;
			}
		}

		if (bIsListed)
		{
			CString str = strParamArray[index];
			strFinalParamArray.Add( str );   // Keep param block
		}
		else
		{
			CString str = "// Param: "+pParam->m_strName + '\n';
			strFinalParamArray.Add( str );  // Create new param block
		}
		
		bIsListed = FALSE;
	}
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


void Error(LPCSTR szMsg, ...)
{
	char szBuf[512];

	va_list marker;
	va_start(marker, szMsg);
	_vsnprintf(szBuf, 512, szMsg, marker);
	va_end(marker);

	fprintf(stderr, "apitext Error: %s\n", szBuf);
	//exit(-2);  This is causing a crash
}

void Warning(LPCSTR szMsg, ...)
{
	char szBuf[512];

	va_list marker;
	va_start(marker, szMsg);
	_vsnprintf(szBuf, 512, szMsg, marker);
	va_end(marker);

	fprintf(stderr, "apitext Warning: %s\n", szBuf);
}

void Debug(LPCSTR szMsg, ...)
{
	#ifdef _DEBUG

		char szBuf[512];

		va_list marker;
		va_start(marker, szMsg);
		_vsnprintf(szBuf, 512, szMsg, marker);
		va_end(marker);

		fprintf(stderr, "apitext Debug: %s\n", szBuf);
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
					strTXTFile = argv[i];
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

	Debug("File list filename = %s", strListFile);
	Debug("RTF filename = %s", strTXTFile);

	return TRUE;
}

void Usage(void)
{
	fprintf(stdout, "\nCAFE API Text File Generation System\n");
	fprintf(stdout, "Copyright 1994 Microsoft\n\n");
	fprintf(stdout, "Usage: apitext [-l <filename> -o <filename> -?]\n");
	fprintf(stdout, "-l <filename> : specify filename of file containing list of files to parse\n");
	fprintf(stdout, "  (defaults to apitext.lst)\n");
	fprintf(stdout, "-o <filename> : specify filename of output file\n");
	fprintf(stdout, "  (defaults to apidata.txt)\n");
	fprintf(stdout, "-? : displays this usage message\n");
	#ifdef _DEBUG
		fprintf(stdout, "\n(debug build)\n");
	#endif // _DEBUG

	exit(-1);
}
