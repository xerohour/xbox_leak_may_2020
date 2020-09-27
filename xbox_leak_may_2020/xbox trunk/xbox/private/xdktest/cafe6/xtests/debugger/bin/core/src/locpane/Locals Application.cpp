#include <xtl.h> //xbox #include <windows.h>

// Locals Test

void FuncNoArg(void)
{
	return;
}

char FuncWithArg(char chArg)
{
	int		nJustLocalInt = 1;
	double	dJustLocalDouble = 3.14;
	return chArg + 2;
}										/* Last FuncWithArg line */
int FAR PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	char	chJustChar = 'a'; 
	int		nJustInt = 1965;
	int		&nJustIntRef =  nJustInt;
	double	dJustDouble = 2.92;

	nJustInt++;								/* First line for tests */				
	chJustChar++;

	chJustChar = FuncWithArg(chJustChar);	/* Second line for tests */
	FuncNoArg();							/* Third line for tests */

	return 0;

}

