int foo_Include(int number);

int foo_Include(int number)
{
	number *= 4;
	if(number == 3)
	{
		return (-1);
	}
	number += 5;
	return number;
}
