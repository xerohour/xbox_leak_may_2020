// String history Class
//
// 1/20/2000	Shinji Chiba

#include "stdafx.h"

HISTORY::HISTORY()
{
	int i;
	historyRewind = new char*[N_HISTORY];
	historyForward = new char*[N_HISTORY];
	for( i = 0; i < N_HISTORY; i++ )
	{
		historyRewind[i] = (char *) new char[L_HISTORY];
		historyForward[i] = (char *) new char[L_HISTORY];
		ZeroMemory( historyRewind[i], L_HISTORY * sizeof(char) );
		ZeroMemory( historyForward[i], L_HISTORY * sizeof(char) );
	}
}

HISTORY::~HISTORY()
{
	int i;
	for( i = 0; i < N_HISTORY; i++ )
	{
		delete historyForward[i];
		delete historyRewind[i];
	}
	delete historyForward;
	delete historyRewind;
}

void HISTORY::Clear()
{
	historyRewind[0][0] = historyForward[0][0] = '\0';
}

void HISTORY::AddForwardHistory( char *str )
{
	int i;
	for( i = N_HISTORY; i > 1; )
	{
		i--;
		strcpy( historyForward[i], historyForward[i - 1] );
	}
	strcpy( historyForward[0], str );
}

void HISTORY::SaveHistory( char *str )
{
	int i;
	for( i = N_HISTORY; i > 1; )
	{
		i--;
		strcpy( historyRewind[i], historyRewind[i - 1] );
		historyForward[i][0] = '\0';
	}
	strcpy( historyRewind[0], str );
	historyForward[0][0] = '\0';
}

void HISTORY::RestoreRewindHistory( char *str )
{
	if ( historyRewind[0][0] )
	{
		int i;
		AddForwardHistory( str );
		strcpy( str, historyRewind[0] );
		for( i = 0; i < N_HISTORY - 1; i++ )
		{
			strcpy( historyRewind[i], historyRewind[i + 1] );
		}
		historyRewind[N_HISTORY - 1][0] = '\0';
	}
}

void HISTORY::RestoreForwardHistory( char *str )
{
	if ( historyForward[0][0] )
	{
		int i;
		for( i = N_HISTORY; i > 1; )
		{
			i--;
			strcpy( historyRewind[i], historyRewind[i - 1] );
		}
		strcpy( historyRewind[0], str );
		strcpy( str, historyForward[0] );
		for( i = 0; i < N_HISTORY - 1; i++ )
		{
			strcpy( historyForward[i], historyForward[i + 1] );
		}
		historyForward[N_HISTORY - 1][0] = '\0';
	}
}

DWORD HISTORY::GetRewindCount()
{
	int i;
	DWORD count;
	for( i = 0, count = 0; i < N_HISTORY; i++ )
	{
		if ( historyRewind[i][0] ) count++;
		else break;
	}
	return count;
}

DWORD HISTORY::GetForwardCount()
{
	int i;
	DWORD count;
	for( i = 0, count = 0; i < N_HISTORY; i++ )
	{
		if ( historyForward[i][0] ) count++;
		else break;
	}
	return count;
}
