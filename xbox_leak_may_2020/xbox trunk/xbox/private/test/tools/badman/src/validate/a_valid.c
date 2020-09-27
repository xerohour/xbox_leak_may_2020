
/*************************************************************************
MODULE: A_Valid.C
AUTHOR: JohnMil
  DATE: 2-5-92

Modification History:

        2-20-92  RamC  Added validate routines for AddAce and
                       AddAccessAllowedAce
                       Added include files parsedef.h and security.h

Copyright (c) 1992 Microsoft Corporation

This module contains API Return value Validation routines for API's
beginning with the letter 'A'.
For more information, please refer to BadMan.Doc.
*************************************************************************/


#include <BadMan.h>
#include <Validate.h>
#include <parsedef.h>


/*********************************************************************/

void AddAccessAllowedAceValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  //
  // The AddAccessAllowed API accepts the following parameters
  //
  // dwParms[0] = PACL  pAcl           supplies ACL being modified
  // dwParms[1] = DWORD dwAceRevision  ACE revision number
  // dwParms[2] = DWORD dwAccessMask   Mask of accesses to be granted to
  //                                   the specified SID
  // dwParms[3] = PSID  pSid           Pointer to the SID being granted
  //                                   access.
  //

  PACCESS_ALLOWED_ACE pAce;
  PACL                pAcl = (PACL)dwParms[0];
  PVOID               pAcePosition;
  USHORT               i;

  //
  // do the validation only if all the parameters to the API are valid
  //

  if((ParmGoodBad[0] == CLOPT_GOOD) &&
     (ParmGoodBad[1] == CLOPT_GOOD) &&
     (ParmGoodBad[2] == CLOPT_GOOD) &&
     (ParmGoodBad[3] == CLOPT_GOOD))   {

    //
    // Check to see if the API call succeeded
    //

    if((BOOL)dwRetVal == TRUE) {


      //
      // get the first ace address
      //

      pAcePosition = (PVOID)((PUCHAR)(pAcl) + sizeof(ACL));

      //
      // locate the position where the AccessAllowedAce was added
      //

      for (i = 0; i < (USHORT)(pAcl->AceCount - 1); i++) {

          pAcePosition = (PVOID)((PUCHAR)pAcePosition +
                        (((PACE_HEADER)pAcePosition)->AceSize));
      }

      //
      // check to see if the ACE was indeed added here
      //

      pAce = (PACCESS_ALLOWED_ACE)pAcePosition;

      if( (pAce->Header.AceType == ACCESS_ALLOWED_ACE_TYPE) &&
          (pAce->Header.AceFlags == 0) &&
          (pAce->Header.AceSize == (USHORT)(sizeof(ACE_HEADER)+
                                   sizeof(ACCESS_MASK) +
                                   0/* BUGBUG GetLengthSid((PSID)dwParms[3])*/ )) &&
          (pAce->Mask == dwParms[2]))  {

         ValPrintf(hConOut, "AddAccessAllowedAceValidate: Success!!\n");
      }
      else
         ErrorPrint(hConOut,hLog,"AddAccessAllowedAceValidate",1,"Failure\n");
    } // end if dwRetVAl
  } // end if ParmGoodBad

  //
  // check to see if the API returned TRUE even when bad parameters were
  // specified.
  //

  else {
    if((BOOL)dwRetVal == TRUE) {
         ErrorPrint(hConOut,hLog,"AddAccessAllowedAceValidate",1,"Failure\n");
    }
  }

  VarInfo;	// -W3 Warning elimination
}



/*********************************************************************/

void AddAccessDeniedAceValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  //
  // The AddAccessDenied API accepts the following parameters
  //
  // dwParms[0] = PACL  pAcl           supplies ACL being modified
  // dwParms[1] = DWORD dwAceRevision  ACE revision number
  // dwParms[2] = DWORD dwAccessMask   Mask of accesses to be granted to
  //                                   the specified SID
  // dwParms[3] = PSID  pSid           Pointer to the SID being granted
  //                                   access.
  //

  PACCESS_DENIED_ACE  pAce;
  PACL                pAcl = (PACL)dwParms[0];
  PVOID               pAcePosition;
  USHORT               i;

  //
  // do the validation only if all the parameters to the API are valid
  //

  if((ParmGoodBad[0] == CLOPT_GOOD) &&
     (ParmGoodBad[1] == CLOPT_GOOD) &&
     (ParmGoodBad[2] == CLOPT_GOOD) &&
     (ParmGoodBad[3] == CLOPT_GOOD))   {

    //
    // Check to see if the API call succeeded
    //

    if((BOOL)dwRetVal == TRUE) {


      //
      // get the first ace address
      //

      pAcePosition = (PVOID)((PUCHAR)(pAcl) + sizeof(ACL));

      //
      // locate the position where the AccessDeniedAce was added
      //

      for (i = 0; i < (USHORT)(pAcl->AceCount - 1); i++) {

          pAcePosition = (PVOID)((PUCHAR)pAcePosition +
                        (((PACE_HEADER)pAcePosition)->AceSize));
      }

      //
      // check to see if the ACE was indeed added here
      //

      pAce = (PACCESS_DENIED_ACE)pAcePosition;

      if( (pAce->Header.AceType == ACCESS_DENIED_ACE_TYPE) &&
          (pAce->Header.AceFlags == 0) &&
          (pAce->Header.AceSize == (USHORT)(sizeof(ACE_HEADER)+
                                   sizeof(ACCESS_MASK) +
                                   0 /* BUGBUG GetLengthSid((PSID)dwParms[3])*/ )) &&
          (pAce->Mask == dwParms[2]))  {

         ValPrintf(hConOut, "AddAccessDeniedAceValidate: Success!!\n");
      }
      else
         ErrorPrint(hConOut,hLog,"AddAccessDeniedAceValidate",1,"Failure\n");
    } // end if dwRetVAl
  } // end if ParmGoodBad

  //
  //  check to see if the API returned TRUE even when bad parameters were
  //  specified.
  //

  else {
    if((BOOL)dwRetVal == TRUE) {
         ErrorPrint(hConOut,hLog,"AddAccessDeniedAceValidate",1,"Failure\n");
    }
  }

  VarInfo;		// -W3 warning elimination
}


/*********************************************************************/

void AddAuditAccessAceValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  //
  // The AddAuditAccessAce API accepts the following parameters
  //
  // dwParms[0] = PACL  pAcl           supplies ACL being modified
  // dwParms[1] = DWORD dwAceRevision  ACE revision number
  // dwParms[2] = DWORD dwAccessMask   Mask of accesses to be granted to
  //                                   the specified SID
  // dwParms[3] = PSID  pSid           Pointer to the SID being granted
  //                                   access.
  // dwParms[4] = BOOL  bAuditSuccess  if TRUE indicates successful access
  //                                   attempts are to be audited
  // dwParms[5] = BOOL  bAuditFailure  if TRUE indicates failed access attempts
  //                                   are to be audited
  //

  PSYSTEM_AUDIT_ACE pAce;
  PACL              pAcl = (PACL)dwParms[0];
  PVOID             pAcePosition;
  USHORT             i;
  UCHAR	            AuditFlags=0;

  //
  // do the validation only if all the parameters to the API are valid
  //

  if((ParmGoodBad[0] == CLOPT_GOOD) &&
     (ParmGoodBad[1] == CLOPT_GOOD) &&
     (ParmGoodBad[2] == CLOPT_GOOD) &&
     (ParmGoodBad[3] == CLOPT_GOOD) &&
     (ParmGoodBad[4] == CLOPT_GOOD) &&
     (ParmGoodBad[5] == CLOPT_GOOD))   {

    //
    // Check to see if the API call succeeded
    //

    if((BOOL)dwRetVal == TRUE) {


      //
      // get the first ace address
      //

      pAcePosition = (PVOID)((PUCHAR)(pAcl) + sizeof(ACL));

      //
      // locate the position where the SystemAuditAce was added
      //

      for (i = 0; i < (USHORT)(pAcl->AceCount - 1); i++) {
          pAcePosition = (PVOID)((PUCHAR)pAcePosition +
                        (((PACE_HEADER)pAcePosition)->AceSize));
      }

      // set the flags

      if(dwParms[4] == (BOOL)TRUE) {
        AuditFlags |= SUCCESSFUL_ACCESS_ACE_FLAG;
      }

      if(dwParms[5] == (BOOL)TRUE) {
        AuditFlags |= FAILED_ACCESS_ACE_FLAG;
      }


      //
      // check to see if the ACE was indeed added here
      //

      pAce = (PSYSTEM_AUDIT_ACE)pAcePosition;

      if( (pAce->Header.AceType == SYSTEM_AUDIT_ACE_TYPE) &&
          (pAce->Header.AceFlags == AuditFlags) &&
          (pAce->Header.AceSize == (USHORT)(sizeof(ACE_HEADER)+
                                   sizeof(ACCESS_MASK) +
                                   0 /* GetLengthSid((PSID)dwParms[3])*/ )) &&
          (pAce->Mask == dwParms[2]))  {

         ValPrintf(hConOut, "AddAuditAccessAceValidate: Success!!\n");
      }
      else
         ErrorPrint(hConOut,hLog,"AddAuditAccessValidate",1,"Failure\n");
    } // end if dwRetVAl
  } // end if ParmGoodBad

  //
  // check to see if the API returned TRUE even when bad parameters were
  // specified.
  //

  else {
    if((BOOL)dwRetVal == TRUE) {
         ErrorPrint(hConOut,hLog,"AddAuditAccessValidate",1,
                    "BadParms but API returned Success\n");
    }
  }

  VarInfo;		// -W3 warning elimination
}

/******************************************************/

void AddAceValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  //
  // The AddAce API accepts the following parameters
  //
  // dwParms[0] = PACL  pAcl           pointer to ACL
  // dwParms[1] = DWORD dwAceRevision  ACE revision number
  // dwParms[2] = DWORD dwAceindex     where to insert ACE list
  // dwParms[3] = PVOID pAceList       list of aces
  // dwParms[4] = DWORD dwAceListLen   length of ace list
  //

  PVOID  pAcePosition, Ace;
  ULONG  i, AceCount;
  PACL   pAcl = (PACL)dwParms[0];

  //
  // do the validation only if all the parameters to the API are valid
  //

  if((ParmGoodBad[0] == CLOPT_GOOD) &&
     (ParmGoodBad[1] == CLOPT_GOOD) &&
     (ParmGoodBad[2] == CLOPT_GOOD) &&
     (ParmGoodBad[3] == CLOPT_GOOD) &&
     (ParmGoodBad[4] == CLOPT_GOOD))   {

    //
    // Check to see if the API call succeeded
    //

    if((BOOL)dwRetVal == TRUE) {

      //
      // get the first ace address
      //

      pAcePosition = (PVOID)((PUCHAR)(pAcl) + sizeof(ACL));

      //
      // if the AceIndex is specified as any value other than zero,
      // we will have to figure out where the AceList was inserted
      // (depending on the number of aces added).
      // We will do this by going down the list of Aces and finding
      // out how many aces were added. We then use that value to
      // decrement our notion of the number of Aces before the
      // AddAce API was invoked.
      //

      Ace = (PVOID)dwParms[3];
      AceCount = pAcl->AceCount;

      if(dwParms[2]) {
        for (i = 0; Ace < (PVOID)((PUCHAR)dwParms[3]+dwParms[4]) ; i++)
             Ace = (PVOID)((PUCHAR)Ace + ((PACE_HEADER)Ace)->AceSize);

        //
        // Decrement the AceCount by this number
        //

        AceCount -= i;
      }


      //
      // locate the position where the Acelist was inserted
      //

      for (i = 0; i < dwParms[2] && i < AceCount ; i++) {

          pAcePosition = (PVOID)((PUCHAR)pAcePosition +
                        (((PACE_HEADER)pAcePosition)->AceSize));
      }

      //
      // check to see if the ACEs were indeed inserted here
      //

      if(!memcmp(pAcePosition, (PVOID)dwParms[3], dwParms[4]))
         ValPrintf(hConOut, "AddAceValidate: Success!!\n");
      else
         ErrorPrint(hConOut,hLog,"AddAceValidate",1,"Failure\n");
    }

  }

  VarInfo;		// -W3 warning elimination
}


/******************************************************/

void AddAliasValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AddAtomAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AddAtomWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AddFontModuleValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AddFontResourceAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AddFontResourceWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AddFontResourceExAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AddFontResourceExWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AddFontMemResourceExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AdjustWindowRectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AdjustWindowRectExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AllocConsoleValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AngleArcValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AnimatePaletteValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AnyPopupValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AppendMenuAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void AppendMenuWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void ArcValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void ArcToValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
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

void ArrangeIconicWindowsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}

