#include <xtl.h>

int e_sig1Entry();
int f_sig1Entry();
int f_sig2Entry();
int f_sig3Entry();
int i_sig1Entry();
int i_sig2Entry();


#ifdef __cplusplus
extern "C"
#endif
void __cdecl signalStartTest()
{
	e_sig1Entry();
	f_sig1Entry();
	f_sig2Entry();
	f_sig3Entry();
	i_sig1Entry();
	i_sig2Entry();
}
