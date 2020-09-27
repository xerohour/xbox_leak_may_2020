// Utilities.cpp
//

#include "stdafx.h"
#include "Utilities.h"

TCENT Mils2TimeCents(DWORD dwMils)
{
	TCENT	tcTimeCent;
	double		dTime;

	if (dwMils < 1) dwMils = 1;
	dTime = dwMils;
	dTime /= 1000.0;
	dTime = log10(dTime) / 0.301;
	dTime *= 1200;
	tcTimeCent = (TCENT) (dTime + 0.5);
	return tcTimeCent;
}

DWORD TimeCents2Mils(TCENT tcTime)
{
    DWORD dwMils;
    double dTime = tcTime;
    dTime /= 1200;
    dTime = pow(2.0,dTime);
    dTime *= 1000;
    dwMils = (DWORD) (dTime + 0.5);
	if (dwMils > 40000) dwMils = 40000;
    return(dwMils);
}