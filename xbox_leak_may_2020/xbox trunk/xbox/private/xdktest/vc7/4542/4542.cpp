#include <xtl.h>
#include <stdlib.h>

extern "C" int ___mb_cur_max_func();
extern "C" int ___lc_handle_func();
extern "C" int ___lc_collate_cp_func();
extern "C" int ___lc_codepage_func();

void __cdecl main()
{
	___mb_cur_max_func();
	___lc_handle_func();
	___lc_collate_cp_func();
	___lc_codepage_func();
}