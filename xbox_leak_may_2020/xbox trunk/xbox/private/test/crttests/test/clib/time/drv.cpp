#include <xtl.h>

int gmtime1Entry();
int localtime1Entry();
int mktime1Entry();
int mktime2Entry();
//int mktime3Entry();
int futime1Entry();
int futime2Entry();
int utime1Entry();
int utime2Entry();
int ftime1Entry();
int time1Entry();
int ctime1Entry();
//int strftime1Entry();
int testcrtEntry();


#ifdef __cplusplus
extern "C"
#endif
void __cdecl timeStartTest()
{
	gmtime1Entry();
	localtime1Entry();
	mktime1Entry();
	mktime2Entry();
	//mktime3Entry();
	futime1Entry();
	futime2Entry();
	utime1Entry();
	utime2Entry();
	ftime1Entry();
	time1Entry();
	ctime1Entry();
	//strftime1Entry();
	testcrtEntry();
}
