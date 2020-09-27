//**********************************************************************
//  Header for Testgo01 app : Testing of Go/Restart/StopDebugging
//***********************************************************************

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>

void        check();
int         check_int(int a, int b);
float       check_f(float a, float b);
int         check_func(int  a,int  l);
long        checkel_func(long a  ,long b,int  l);
float       checkef_func(float a , float b, int l);
double      checked_func(double a, double b, int l);
long double checkeld_func(long double a, long double b, int l);
char       *check_char(char *s);
char       *check_str(int a);
char       *check_lstr(int a, int b, char *s1, char *s2);

static int foo_Include(int number);

int foo_Include(int number)
{
	number*=2;
	if(number == 3)
	{
		return (-1);
	}
	return number;
}
