int check_func(int  a,int  l);

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
