/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    sectoken.h

Abstract:

    Security Testing Utilites -- token include file.

    These routines are primarily designed to check and/or set token
    info used by the security system in determining object access.

    See token.c for documentation on routines.

Author:

    Jim Gilroy (jamesg) 3-19-91

Revision History:

--*/


#ifndef  _SECTOKEN_
#define  _SECTOKEN_


/*   VOID */
/*   SecPrintTokenUser ( */
/*       IN  PTOKEN_USER  ptr */
/*       IN  PSZ   Name */
/*       ) */

/*   Prints TokenUser SID */

#define SecPrintTokenUser(ptr, Name) \
            SecPrintSid (((TOKEN_USER *)ptr)->User.Sid, Name)

/*   VOID */
/*   SecPrintTokenOwner ( */
/*       IN  PTOKEN_OWNER  ptr, */
/*       IN  PSZ   Name */
/*       ) */

/*   Prints TokenOwner SID */

#define SecPrintTokenOwner(ptr, Name) \
            SecPrintSid (((TOKEN_OWNER *)ptr)->Owner, Name)

/*   VOID */
/*   SecPrintTokenPrimaryGroup ( */
/*       IN  PTOKEN_PRIMARY_GROUP  ptr, */
/*       IN  PSZ   Name */
/*       ) */

/*   Prints TokenPrimaryGroup SID */

#define SecPrintTokenPrimaryGroup(ptr, Name) \
            SecPrintSid (((TOKEN_PRIMARY_GROUP *)ptr)->PrimaryGroup, Name)





/*  Token Group Routines */


VOID
SecPrintTokenGroups (
    IN  PTOKEN_GROUPS  PTokenGroups,
    IN  PSZ Name
    );


/*  Token Privileges */


VOID
SecPrintTokenPrivileges (
    IN  PTOKEN_PRIVILEGES  PTokenPrivileges,
    IN  PSZ  Name
    );

VOID
SecPrintTokenStatistics (
    IN  PTOKEN_STATISTICS  ptr,
    IN  PSZ  Name
    );

VOID
SecPrintTokenControl (
    IN  PTOKEN_CONTROL  ptr,
    IN  PSZ  Name
    );

NTSTATUS    SecPrintTokenInfo(
    IN  HANDLE  token_to_query
    );

#endif  /*  _SECTOKEN_ */


/*  End of sectoken.h */




