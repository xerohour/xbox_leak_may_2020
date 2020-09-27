#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int foo_Include(int number);
int TestG(void);
int func(int);
int recur_func();
void nest_func(int);
typedef int My_typedef;
int main(void);

int foo_Include(int number)
{
	number*=2;
	if(number == 3)
	{
		return (-1);
	}
	return number;
}
