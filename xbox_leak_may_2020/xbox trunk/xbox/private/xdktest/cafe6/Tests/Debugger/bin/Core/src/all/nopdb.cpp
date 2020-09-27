__declspec(dllexport) void no_pdb_dll_func(void)
	{
	__asm
		{
		int 3
		}
	}
