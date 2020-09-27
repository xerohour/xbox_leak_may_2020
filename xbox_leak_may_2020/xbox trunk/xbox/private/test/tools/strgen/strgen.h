/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    strgen.h

Abstract:

    Routine to generate random string

Environment:

    Xbox

Revision History:

--*/


#define SG_FORMAT_ASCII			0
#define SG_FORMAT_UNICODE		1

#define SG_MAXLENGTH_256		0
#define SG_MAXLENGTH_32768		-1

#define SG_STYLE_NORMAL			0
#define SG_STYLE_NETBIOS		1

#define SG_PRESUDO_YES          1
#define SG_PRESUDO_NO           0

#define SG_RISK_NO              0
#define SG_RISK_YES             1

int strgen(LCID ,int ,int ,	int ,unsigned int,int,int,void *);
int PassString(char *,int ,void **,BOOL *,LCID );
int  ConvertLCIDtoCodePage(LCID );
void GenerateFERiskString(char **,WORD **,int ,LCID,int);
void GenerateEuroRiskString(char **,WORD **,int , LCID,int);
void GenerateMixRiskString(LCID*,WCHAR **,int,int);
BOOL ISFELocale(LCID);

extern LCID  g_lcID[MAX_PATH] ;

