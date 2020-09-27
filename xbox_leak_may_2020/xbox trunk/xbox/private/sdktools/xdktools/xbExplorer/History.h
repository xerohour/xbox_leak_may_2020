// String history Class
//
// 1/20/2000	Shinji Chiba

#ifndef __HISTORY_H__
#define __HISTORY_H__

#include <windows.h>

#define N_HISTORY 64
#define L_HISTORY MAX_PATH

class HISTORY
{
private:
	char **historyRewind;
	char **historyForward;

	void AddForwardHistory( char* );

public:
	HISTORY();
	~HISTORY();
	void Clear();
	void SaveHistory( char* );
	void RestoreRewindHistory( char* );
	void RestoreForwardHistory( char* );
	DWORD GetRewindCount();
	DWORD GetForwardCount();
};

#endif
