//
// Xbox Linker Tool
//
// [colint]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "xbxtooll.h"	// our local header file

IMPLEMENT_DYNAMIC (CLinkerXboxExeTool, CLinkerTool)
#ifdef XBOXDLL
IMPLEMENT_DYNAMIC (CLinkerXboxDllTool, CLinkerTool)
#endif

CLinkerXboxExeTool::CLinkerXboxExeTool () : CLinkerTool()
{
#if 0
	m_nIDName = IDS_XBOXLINKCOFF_TOOL;
	m_nIDUIName = IDS_XBOXLINKCOFF_TOOL_UI;
#endif
}

#ifdef XBOXDLL
CLinkerXboxDllTool::CLinkerXboxDllTool() : CLinkerTool()
{
	m_nIDName = IDS_XBOXLINKCOFF_TOOL;
	m_nIDUIName = IDS_XBOXLINKCOFF_TOOL_UI;
}
#endif

#if 0
BOOL CLinkerXboxExeTool::GetCommandLines(CActionSlobList &lstActions,
    CPtrList &plCommandLines, DWORD attrib, CErrorContext &EC)
{
	CPtrList plLinker;

	/* First we gather the linker's command lines */
    if(!CLinkerTool::GetCommandLines(lstActions, plLinker, attrib, EC))
        return FALSE;

	/* Now we walk through the linker's command lines and strip out those
	 * options belonging to imagebld, forming a new linker command and a new
	 * imagebld command for each one */
	POSITION posCmd = plLinker.GetHeadPosition();
	while(posCmd != (POSITION)NULL)
	{
		CCmdLine *pclLinker = (CCmdLine *)plLinker.GetNext(posCmd);
		CCmdLine *pclXbe = new CCmdLine;
		POSITION posStr = pclLinker->slCommandLines.GetHeadPosition();
		while(posStr != (POSITION)NULL)
		{
			CString *pstr = (CString *)&pclLinker->slCommandLines.GetNext(posStr);
			CString strXbeCmd;
			CString strDescription;
			int ichOpt, ichEnd;

			/* Draconic, but simplistic */
			pstr->MakeLower();

			/* Look for the linker's /out: parameter to use as imagebld's
			 * input */
			ichOpt = pstr->Find("/out:");
			if(ichOpt < 0)
			{
				/* We can't handle this case, so we'll force an error */
				delete pclXbe;
				return FALSE;
			}
			
			/* We don't want the /out: part */
			ichOpt += 5;
			ichEnd = pstr->Find(' ', ichOpt);
			CString strExe = ichEnd < 0 ? pstr->Mid(ichOpt) :
				pstr->Mid(ichOpt, ichEnd - ichOpt);

			strXbeCmd = strExe + ' ';

			/* Build the default xbe name in case we don't see the option */
			CString strXbe;
			GetXbeFromExe(strExe, strXbe);

			/* Now look for all of the /xbe: parameters */
			while((ichOpt = pstr->Find("/xbe:")) >= 0)
			{
				/* Get the / in there */
				strXbeCmd += '/';
				/* And put in the rest of the option */
				ichEnd = pstr->Find(' ', ichOpt);
				if(ichEnd < 0)
					ichEnd = pstr->GetLength();
				/* If this is the /out option, we can throw away our default
				 * out value */
				if(pstr->Mid(ichOpt + 5, 4) == "out:")
					strXbe = "";
				strXbeCmd += pstr->Mid(ichOpt + 5, ichEnd - (ichOpt + 5)) + ' ';
				/* Take the option out of the linker string */
				pstr->Delete(ichOpt, ichEnd - ichOpt);
			}

			/* Tack on our xbe name if we need to */
			if(!strXbe.IsEmpty())
				strXbeCmd += "/out:" + strXbe;

			/* With all that done, put this string on the imagebld cmd list */
			if (!g_buildengine.FormCmdLine(CString("imagebld.exe"), strXbeCmd, EC))
			{
				delete pclXbe;
				return FALSE;
			}

			strDescription.LoadString(IDS_DESC_XBEBUILD);
			pclXbe->slDescriptions.AddTail(strDescription);
			pclXbe->slCommandLines.AddTail(strXbeCmd);
		}

		/* This command item is out of the way, so flesh out the imagebld
		 * structures and add both the linker and imagebld commands to the
		 * complete list */

		plCommandLines.AddTail(pclLinker);
		plCommandLines.AddTail(pclXbe);
	}

	return TRUE;
}

void GetXbeFromExe(CString &strExe, CString &strXbe)
{
	int ich = strExe.Find(".exe");
	if(ich < 0)
	{
		/* No .exe, so we append .xbe -- inside a terminal quote if
		 * necessary */
		if(strExe.Right(1) == "\"")
			strXbe = strExe.Left(strExe.GetLength() - 1) + ".xbe\"";
		else
			strXbe = strExe + ".xbe";
	}
	else
		/* Replace the .exe with .xbe */
		strXbe = strExe.Left(ich) + ".xbe" + strExe.Mid(ich + 4);
}
#endif