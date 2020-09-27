
/*************************************************************************
MODULE: I_Valid.C
AUTHOR: JohnMil
  DATE: 2-5-92

Copyright (c) 1992 Microsoft Corporation

This module contains API Return value Validation routines for API's
beginning with the letter 'I'.
For more information, please refer to BadMan.Doc.
*************************************************************************/
#include <BadMan.h>


#include <Validate.h>
#include <parsedef.h>

/******************************************************/

void InSendMessageValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void InflateRectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void InitAtomTableValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}

/******************************************************/

void InitializeAclValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
   PACL pAcl = (PACL)dwParms[0];

   if((ParmGoodBad[0] == CLOPT_GOOD) &&
      (ParmGoodBad[1] == CLOPT_GOOD) &&
      (ParmGoodBad[2] == CLOPT_GOOD)) {

      // Verify ACL creation

      if((pAcl->AclRevision == ACL_REVISION)  &&
	 (pAcl->AclSize == dwParms[1]))
	 ErrorPrint(hConOut, hLog,"InitializeAclValidate",1,"Success!!");
      else

	 ErrorPrint(hConOut, hLog,"InitializeAclValidate",1,"Failure");
   }
}
/******************************************************/

void InitializeCriticalSectionValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}

/******************************************************/

void InitializeSidValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
     // point to Opaque SID datastructure

     PISID piSid = (PISID)dwParms[0];
     PSID_IDENTIFIER_AUTHORITY pSidAuthority = (PSID_IDENTIFIER_AUTHORITY)
					       dwParms[1];
     /*++

	Let us verify the SID initialization

	The SID structure is defined in ntseapi.h as

	typedef struct _SID {
	    UCHAR Revision;
	    UCHAR SubAuthorityCount;
	    SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
	    ULONG SubAuthority[ANYSIZE_ARRAY];
	} SID, *PISID;

	and the SID_IDENTIFIER_AUTHORITY structure as

	typedef struct _SID_IDENTIFIER_AUTHORITY {
	    UCHAR Value[6];
	} SID_IDENTIFIER_AUTHORITY, *PSID_IDENTIFIER_AUTHORITY;

	We will use a pointer to the opaque datastructure to
	verify if the Revision number,Subauthority count and
	Identifier authority are initialized properly.

     --*/
   // check to see if the parameters are good 

   if((ParmGoodBad[0] == CLOPT_GOOD) &&
      (ParmGoodBad[1] == CLOPT_GOOD) &&
      (ParmGoodBad[2] == CLOPT_GOOD))	{

      if((piSid->Revision == SID_REVISION)	  &&  // default revision
	(piSid->SubAuthorityCount == (ULONG)dwParms[2])  &&  // check SA count
	(piSid->IdentifierAuthority.Value[0] ==       // check Identifier
	 pSidAuthority->Value[0])		  &&  // authority value
	(piSid->IdentifierAuthority.Value[1] ==
	 pSidAuthority->Value[1])		  &&
	(piSid->IdentifierAuthority.Value[2] ==
	 pSidAuthority->Value[2])		  &&
	(piSid->IdentifierAuthority.Value[3] ==
	 pSidAuthority->Value[3])		  &&
	(piSid->IdentifierAuthority.Value[4] ==
	 pSidAuthority->Value[4])		  &&
	(piSid->IdentifierAuthority.Value[5] ==
	 pSidAuthority->Value[5]))
       ErrorPrint(hConOut,hLog,"InitializeSidValidate",1, "Success!!");
     else
       ErrorPrint(hConOut,hLog,"InitializeSidValidate",1, "Failure");
   }

}



/******************************************************/

void InsertMenuAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void InsertMenuWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IntersectClipRectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IntersectRectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void InvalidateConsoleDIBitsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void InvalidateRectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void InvalidateRgnValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void InvertRectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void InvertRgnValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsCharAlphaAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsCharAlphaNumericAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsCharAlphaNumericWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsCharAlphaWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsCharLowerAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsCharLowerWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsCharUpperAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsCharUpperWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsChildValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsClipboardFormatAvailableValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsDialogMessageValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsDlgButtonCheckedValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsIconicValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsRectEmptyValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsTwoByteCharPrefixValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}

/******************************************************/

void IsValidAclValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
   if(ParmGoodBad[0] == CLOPT_GOOD) {

      // Verify ACL creation

      if(((PACL)dwParms[0]) -> AclRevision == ACL_REVISION)
	ErrorPrint(hConOut, hLog,"IsValidAclValidate",1,"Success!!");
      else

	ErrorPrint(hConOut, hLog,"IsValidAclValidate",1,"Failure");
   }
}


/******************************************************/

void IsValidSidValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
   if(ParmGoodBad[0] == CLOPT_GOOD) {
      // note that here we are accessing the Opaque SID structure PISID
      if(((PISID)dwParms[0]) -> Revision == SID_REVISION)
	ErrorPrint(hConOut, hLog,"IsValidSidValidate",1,"Success!!");
      else
	ErrorPrint(hConOut, hLog,"IsValidSidValidate",1,"Failure");
   }
}


/******************************************************/

void IsWindowValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsWindowEnabledValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsWindowVisibleValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void IsZoomedValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}

/******************************************************/

void InternalExtractIconListAValidate(DefineType VarInfo[],DWORD dwParms[],
                        int ParmGoodBad[],DWORD dwRetVal,
                        HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void InternalExtractIconListWValidate(DefineType VarInfo[],DWORD dwParms[],
                        int ParmGoodBad[],DWORD dwRetVal,
                        HANDLE hLog,HANDLE hConOut)
{
  ;
}
