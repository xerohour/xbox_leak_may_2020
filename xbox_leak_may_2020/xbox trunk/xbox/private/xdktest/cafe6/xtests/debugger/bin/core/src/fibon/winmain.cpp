#include <xtl.h> //xbox #include <windows.h>

extern void fiboncases(int argc, char *argv[]);


int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
{
	char *argv[]= {"damn executable", "5", "3"};
	for(int i=0;i<10;i++)
		fiboncases(3,argv);
	return 0;
}