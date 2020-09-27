//
//	Registry I/O class
//
//	Author: Shinji Chiba 1997-1-23
//

#ifndef __REGISTRY_H__
#define __REGISTRY_H__

#define REG_STRMAX 1024

class REGISTRY
{
private:
	HKEY hMainKey;
	char szSubkey[1024];

public:
	REGISTRY();
	~REGISTRY();
	void Init( HKEY, char* );
	BOOL GetString( char*, char* );
	BOOL GetValue( char*, LPDWORD );
	BOOL SetString( char*, char* );
	BOOL SetValue( char*, DWORD );
	BOOL Delete( char* = NULL, HKEY = NULL );
};

#endif
