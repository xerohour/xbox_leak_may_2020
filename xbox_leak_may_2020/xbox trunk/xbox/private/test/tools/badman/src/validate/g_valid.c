
/*************************************************************************
MODULE: G_Valid.C
AUTHOR: JohnMil
  DATE: 2-5-92

Modification History:

        2-20-92  RamC  Added validate routines for AddAce and
                       AddAccessAllowedAce
                       Added include files parsedef.h and security.h

Copyright (c) 1992 Microsoft Corporation

This module contains API Return value Validation routines for API's
beginning with the letter 'G'.
For more information, please refer to BadMan.Doc.
*************************************************************************/


#include <BadMan.h>
#include <Validate.h>
#include <parsedef.h>

void GetGuiResourcesValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
    ValPrintf(hConOut, "GetGuiResources: No Validation done - just bad param testing - so always pass!!\r\n");
}

/******************************************************/

void GetKeyboardTypeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
    switch(dwParms[0])
    {
        case 1:
        {
            // 0 is now considered acceptable - doc bug 181612
            if(dwRetVal>=0 && dwRetVal<8)
            {
                ValPrintf(hConOut, "GetKeyboardTypeValidate: Success!!\r\n");
            }
            else
            {
                ErrorPrint(hConOut,hLog,"GetKeyboardTypeValidate",1,"Failure\r\n");
            }
        }
        case 2:
        {
            // valid numbers between 1 and 7
            if(dwRetVal>0 && dwRetVal<8)
            {
                ValPrintf(hConOut, "GetKeyboardTypeValidate: Success!!\r\n");
            }
            else
            {
                ErrorPrint(hConOut,hLog,"GetKeyboardTypeValidate",1,"Failure\r\n");
            }
        }
        default:
        {
            // should return 0 for error and non-zero for success
            if(dwRetVal!=0)
            {
                ValPrintf(hConOut, "GetKeyboardTypeValidate: Success!!\r\n");
            }
            else
            {
                ErrorPrint(hConOut,hLog,"GetKeyboardTypeValidate",1,"Failure\r\n");
            }
        }

    }
}

/******************************************************/

void GetKBCodePageValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}

/******************************************************/

void GdiFlushValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GdiSetBatchLimitValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}



/******************************************************/

void GetAceValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  //
  // The GetAce API accepts the following parameters
  // 
  // dwParms[0] = PACL  pAcl           pointer to ACL
  // dwParms[1] = DWORD dwAceindex     index indicate which Ace within the
  //                                   ACL to return
  // dwParms[2] = PVOID *pAce          pointer to variable to recieve the
  //                                   address of ACE stored in ACL 
  // 

  PACL        pAcl = (PACL)dwParms[0];
  ULONG       i;
  PACE_HEADER pA, pB; 
  
  //
  // do the validation only if all the parameters to the API are valid
  //

  if((ParmGoodBad[0] == CLOPT_GOOD) &&
     (ParmGoodBad[1] == CLOPT_GOOD) &&
     (ParmGoodBad[2] == CLOPT_GOOD))   {
  
    //
    // Check to see if the API call succeeded
    //

    if((BOOL)dwRetVal == TRUE) {

      //
      // get the first ace address
      //

      pB = (ACE_HEADER *)((PUCHAR)(pAcl) + sizeof(ACL));
  
      //
      // locate the position of the Ace 
      //

      for (i = 0; i < dwParms[1] ; i++) {
         pB = (ACE_HEADER *)((PUCHAR)pB + pB->AceSize);
      }
 
      //
      // this is what we got back from GetAce
      // 

      pA = (PACE_HEADER) * ((PVOID *) dwParms[2]);

      //
      // check to see if we got the correct ACE 
      //

      if ((pA->AceType == pB->AceType) &&
          (pA->AceSize == pB->AceSize) &&
          (pA->AceFlags == pB->AceFlags))  
         ValPrintf(hConOut, "GetAceValidate: Success!!\n");
      else
         ErrorPrint(hConOut,hLog,"GetAceValidate",1,"Failure\n");
    }

  }

  VarInfo;	// -W3 warning elimination
}

/******************************************************/

void GetAclInformationValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  if((ParmGoodBad[0] == CLOPT_GOOD) &&
     (ParmGoodBad[1] == CLOPT_GOOD) &&
     (ParmGoodBad[2] == CLOPT_GOOD) &&
     (ParmGoodBad[3] == CLOPT_GOOD))  {
    if(dwParms[3] == AclRevisionInformation) {
       if(((PACL_REVISION_INFORMATION)dwParms[1]) -> AclRevision == ACL_REVISION)
          ValPrintf(hConOut,"GetAclInformationValidate: Success!!\n");
       else
			    // Modified to correct # of parms (JohnMil)
	  ErrorPrint(hConOut,hLog,"GetAclInformationValidate:",0,"Failure\n");
    }
    else { // this is a size information request
        // not much we can test here
    }
  }

  VarInfo;	// -W3 warning elimination
  dwRetVal;	// -W3 warning elimination
}


/******************************************************/

void GetActiveWindowValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetAliasValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetArcDirectionValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
                        HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetAspectRatioFilterExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}

/******************************************************/

void GetAsyncKeyStateValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetAtomNameAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetAtomNameWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetBitmapBitsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetBitmapDimensionExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetBkColorValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetBkModeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetBrushOrgExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCaptureValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCaretBlinkTimeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCaretPosValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCharTypeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCharWidthAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCharWidthWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClassInfoAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClassInfoWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClassLongValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClassNameAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClassNameWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClassWordValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClientRectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClipBoxValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClipCursorValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClipboardDataValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClipboardFormatNameAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClipboardFormatNameWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClipboardOwnerValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetClipboardViewerValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetColorAdjustmentValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void GetColorSpaceValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}

/******************************************************/

void GetCommConfigValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCommMaskValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCommStateValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCommTimeoutsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCommandLineAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCommandLineWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetConsoleCurrentFontValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetConsoleCursorInfoValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetConsoleFontInfoValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetConsoleFontSizeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetConsoleModeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetConsoleNumberOfFontsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetConsoleNumberOfInputEventsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetConsoleNumberOfMouseButtonsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetConsoleScreenBufferInfoValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetConsoleTitleValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetContextThreadValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCurrentDirectoryAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCurrentDirectoryWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCurrentObjectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCurrentPositionExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCurrentProcessValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCurrentProcessIdValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCurrentThreadValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCurrentThreadIdValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCurrentTimeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCursorPosValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDCBrushColorValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
    if (dwRetVal)
        ;//BUGBUG DeleteDC((HDC)dwRetVal);

	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDCPenColorValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
    if (dwRetVal)
        ;//BUGBUG DeleteDC((HDC)dwRetVal);

	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDCValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
    if (dwRetVal)
        ;//BUGBUG DeleteDC((HDC)dwRetVal);

	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDCExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
    if (dwRetVal)
        ;//BUGBUG DeleteDC((HDC)dwRetVal);

	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDCOrgExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	if (dwRetVal)
        	;//BUGBUG DeleteDC((HDC)dwRetVal);

	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDIBitsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDesktopAttrsAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDesktopAttrsWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDesktopTypesAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDesktopTypesWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDesktopWindowValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDeviceCapsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}

/******************************************************/

void GetDeviceGammaRampValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void GetDialogBaseUnitsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDIBColorTableValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDiskFreeSpaceAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDiskFreeSpaceWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDlgCtrlIDValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDlgItemValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDlgItemIntValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDlgItemTextAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDlgItemTextWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDoubleClickTimeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDriveTypeAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetDriveTypeWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetEnhMetaFileAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetEnhMetaFileWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetEnvironmentStringsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetEnvironmentVariableAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetEnvironmentVariableWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetExitCodeProcessValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetExitCodeThreadValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetFileAttributesAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetFileAttributesWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetFileSizeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetFileTimeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetFileTypeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetFocusValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetFontMapperControlsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetFullPathNameAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetFullPathNameWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetGraphicsModeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetIconInfoValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetInputDesktopValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetInputStateValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetKeyNameTextAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetKeyNameTextWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetKeyStateValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetKeyboardStateValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetLastActivePopupValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetLastErrorValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetLocaleInfoValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}
/******************************************************/

void GetICMProfileAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}

/******************************************************/

void GetICMProfileWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}

/******************************************************/

void GetLogColorSpaceAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}

/******************************************************/

void GetLogColorSpaceWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}




/******************************************************/

void GetLogicalDriveStringsAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetLogicalDriveStringsWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetLogicalDrivesValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMailslotInfoValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMapModeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMenuValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMenuCheckMarkDimensionsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMenuItemCountValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMenuItemIDValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMenuStateValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMenuStringAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMenuStringWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMessageValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMessageExtraInfoValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMessagePosValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMessageTimeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMetaFileValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetMetaFileBitsExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetModuleFileNameAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetModuleFileNameWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetModuleHandleAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetModuleHandleWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetNamedPipeHandleStateAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetNamedPipeHandleStateWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetNamedPipeInfoValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetNearestColorValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetNearestPaletteIndexValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetNextDlgGroupItemValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetNextDlgTabItemValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetNextLocaleValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetObjectAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetObjectSecurityValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetObjectTypeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetObjectWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetOverlappedResultValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPaletteEntriesValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetParentValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPathValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPixelValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPolyFillModeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPriorityClipboardFormatValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPriorityThreadValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPrivateProfileIntAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPrivateProfileIntWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPrivateProfileSectionAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPrivateProfileSectionWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPrivateProfileStringAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPrivateProfileStringWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetProcAddressValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetProcessWindowStationValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetProfileIntAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetProfileIntWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetProfileSectionAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetProfileSectionWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetProfileStringAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetProfileStringWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPropAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetPropWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetQualifiedLocaleValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetQueueStatusValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetROP2Validate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetRgnBoxValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetScrollPosValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetScrollRangeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSidIdentifierAuthorityValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  // point to opaque SID datastructure

  PISID piSid = (PISID)dwParms[0];
  PSID_IDENTIFIER_AUTHORITY pSidAuthority = (PSID_IDENTIFIER_AUTHORITY)dwRetVal;

  if(ParmGoodBad[0] == CLOPT_GOOD) {
   if((piSid->IdentifierAuthority.Value[0] == pSidAuthority->Value[0]) && 
      (piSid->IdentifierAuthority.Value[1] == pSidAuthority->Value[1]) && 
      (piSid->IdentifierAuthority.Value[2] == pSidAuthority->Value[2]) && 
      (piSid->IdentifierAuthority.Value[3] == pSidAuthority->Value[3]) && 
      (piSid->IdentifierAuthority.Value[4] == pSidAuthority->Value[4]) && 
      (piSid->IdentifierAuthority.Value[5] == pSidAuthority->Value[5]))
    ErrorPrint(hConOut, hLog,"GetSidIdentiferAuthorityValidate",1,"Success!!");
   else 
    ErrorPrint(hConOut, hLog,"GetSidIdentiferAuthorityValidate",1,"Failure");
  }

  VarInfo;	// -W3 warning elimination
}


/******************************************************/

void GetLengthSidValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  /* The default SID created is with one sub-authority and this length
     is equal to the size of the SID datastructure.
  */

  if(ParmGoodBad[0] == CLOPT_GOOD) {
    if(dwRetVal == sizeof(SID))
      ErrorPrint(hConOut, hLog,"GetLengthSidValidate",1,"Success!!");
    else
      ErrorPrint(hConOut, hLog,"GetLengthSidValidate",1,"Failure");
  }

  VarInfo;	// -W3 warning elimination
  dwParms;	// -W3 warning elimination
}


/******************************************************/

void GetSidLengthRequiredValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  /* calculate actual length required for a SID
     and compare against returned value

     the calculation is based on the fact that

     size of SID with 1 SubAuthority	 = sizeof(SID)
     size of SID with 2 SubAuthorities = sizeof(SID) + (2-1) * sizeof(ULONG)
     size of SID with 3 SubAuthorities = sizeof(SID) + (3-1) * sizeof(ULONG)
     and so on.
     Each subauthority takes up the size of a ULONG in the SID array

     This is dependent on the fact that ANYSIZE_ARRAY used in the
     definition of SID is 1.	If this value changes in the future,
     our assumptions here will not be valid anymore.

  */

  if(ParmGoodBad[0] == CLOPT_GOOD) {
    if(dwRetVal == (sizeof(SID) + ((dwParms[0]-1) * sizeof(ULONG))))
      ErrorPrint(hConOut, hLog,"GetSidLengthRequiredValidate",1,"Success!!");
    else
      ErrorPrint(hConOut, hLog,"GetSidLengthRequiredValidate",1,"Failure");
  }

  VarInfo;	// -W3 warning elimination
}


/******************************************************/

void GetSidSubAuthorityValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  PISID piSid = (PISID)dwParms[0];

  if(ParmGoodBad[0] == CLOPT_GOOD && ParmGoodBad[1] == CLOPT_GOOD) {
     if(piSid->SubAuthority[dwParms[1]] == *((PULONG)dwRetVal))
      ErrorPrint(hConOut, hLog,"GetSidSubAuthorityValidate",1,"Success!!");
    else
      ErrorPrint(hConOut, hLog,"GetSidSubAuthorityValidate",1,"Failure");
  }

  VarInfo;	// -W3 warning elimination
}


/******************************************************/

void GetSidSubAuthorityCountValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  PISID piSid = (PISID)dwParms[0];

  if(ParmGoodBad[0] == CLOPT_GOOD) {
     if(piSid->SubAuthorityCount == *((PUCHAR)dwRetVal))
      ErrorPrint(hConOut, hLog,"GetSidSubAuthorityCountValidate",1,"Success!!");
    else
      ErrorPrint(hConOut, hLog,"GetSidSubAuthorityCountValidate",1,"Failure");
  }

  VarInfo;	// -W3 warning elimination
}


/******************************************************/

void GetStartupInfoAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetStartupInfoWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetStdHandleValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetStockObjectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetStretchBltModeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSubMenuValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSysColorValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSysInputModeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSystemDirectoryAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSystemDirectoryWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSystemInfoValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSystemMenuValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSystemMetricsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSystemPaletteEntriesValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSystemPaletteUseValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetSystemTimeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTabbedTextExtentAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTabbedTextExtentWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTempFileNameAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTempFileNameWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTempPathAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTempPathWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextAlignValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextCharacterExtraValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextColorValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextExtentPointAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextExtentPointWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextFaceAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextFaceWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextMetricsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextMetricsWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetThreadDesktopValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetThreadSelectorEntryValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetThresholdEventValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetThresholdStatusValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTickCountValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTokenInformationValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  /*
   * The GetTokenInformation API accepts the following parameters
   * 
   * dwParms[0] = HANDLE TokenHandle
   * dwParms[1] = TOKEN_INFORMATION_CLASS TokenInformationClass
   * dwParms[2] = PVOID TokenInformation
   * dwParms[3] = ULONG TokenInformationLength 
   * dwParms[4] = PULONG ReturnLength 
   */
  
   PSID pSid;
   PACL pAcl;

  /*
   * do the validation only if all the parameters to the API are valid
   */

  if((ParmGoodBad[0] == CLOPT_GOOD) &&
     (ParmGoodBad[1] == CLOPT_GOOD) &&
     (ParmGoodBad[2] == CLOPT_GOOD) &&
     (ParmGoodBad[3] == CLOPT_GOOD) &&
     (ParmGoodBad[4] == CLOPT_GOOD))   {
  
    /*
     * Check to see if the API call succeeded
     */

    if((BOOL)dwRetVal == TRUE) {
      switch((TOKEN_INFORMATION_CLASS)dwParms[1]) {
        case TokenUser:
          pSid = (PSID) ((PTOKEN_USER)(PVOID *)dwParms[2])->User.Sid;

          //BUGBUG 
          //if(!IsValidSid(pSid))
            //ValPrintf(hConOut,"GetTokenInformationValidate Failure\n");
          //else
            ValPrintf(hConOut,"GetTokenInformationValidate Success!!\n");
          break;
        case TokenGroups:
          pSid = (PSID) ((PTOKEN_GROUPS)(PVOID *)dwParms[2])->Groups[0].Sid;

          //BUGBUG
          //if(!IsValidSid(pSid))
            //ValPrintf(hConOut,"GetTokenInformationValidate Failure\n");
          //else
            ValPrintf(hConOut,"GetTokenInformationValidate Success!!\n");
          break;
        case TokenOwner:
          pSid = (PSID) ((PTOKEN_OWNER)(PVOID *)dwParms[2])->Owner;

          //BUGBUG
          //if(!IsValidSid(pSid))
            //ValPrintf(hConOut,"GetTokenInformationValidate Failure\n");
          //else
            ValPrintf(hConOut,"GetTokenInformationValidate Success!!\n");
          break;
        case TokenPrimaryGroup:
          pSid = (PSID) 
                 ((PTOKEN_PRIMARY_GROUP)(PVOID *)dwParms[2])->PrimaryGroup;

          //BUGBUG
          //if(!IsValidSid(pSid))
            //ValPrintf(hConOut,"GetTokenInformationValidate Failure\n");
          //else
            ValPrintf(hConOut,"GetTokenInformationValidate Success!!\n");
          break;
        case TokenDefaultDacl:
          pAcl = (PACL) 
                 ((PTOKEN_DEFAULT_DACL)(PVOID *)dwParms[2])->DefaultDacl;

          if(pAcl)
            //BUGBUG
            //if(!IsValidAcl(pAcl))
              //ValPrintf(hConOut,"GetTokenInformationValidate Failure\n");
            //else
              ValPrintf(hConOut,"GetTokenInformationValidate Success!!\n");
          else
              ValPrintf(hConOut,"GetTokenInformationValidate NullDACL\n");
          break;
        case TokenPrivileges:
        case TokenSource:
        case TokenType:
        case TokenImpersonationLevel:
        case TokenStatistics:
          ValPrintf(hConOut,"GetTokenInformationValidate NOTIMPLEMENTED\n");
          break;
        default:
            ValPrintf(hConOut,"GetTokenInformationValidate Unknown Case\n");
          break;
      } // end switch
    } // end if dwRetVal
  } // end if valid params

  VarInfo;	// -W3 warning elimination
  hLog;		// -W3 warning elimination
}

/******************************************************/

void GetTopWindowValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetUpdateRectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetUpdateRgnValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetVersionValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetViewportExtExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetViewportOrgExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetVolumeInformationAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetVolumeInformationWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowDCValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowExtExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowLongValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowOrgExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowRectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowStationAttrsAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowStationAttrsWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowTextAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowTextLengthValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowTextWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowThreadProcessIdValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowWordValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowsDirectoryAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWindowsDirectoryWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetWorldTransformValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalAddAtomAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalAddAtomWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalAllocValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalDeleteAtomValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalFindAtomAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalFindAtomWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalFlagsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalFreeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalGetAtomNameAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalGetAtomNameWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalLockValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalMemoryStatusValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalReAllocValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalSizeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GlobalUnlockValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GrayStringAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}
/******************************************************/

void GrayStringWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}

/******************************************************/

void GetFontLanguageInfoValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCharWidthIValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetCharABCWidthsIValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextExtentPointIValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetTextExtentExPointIValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetFontUnicodeRangesValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void GetGlyphIndicesAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}

/******************************************************/

void GetGlyphIndicesWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}



/******************************************************/

void GetFontAssocStatus(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}

