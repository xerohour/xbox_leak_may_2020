#include <iostream.h>

int main()
{
 	char Char = 's';
 	int  Array1d[3] = {111, 222, 333};
 	long Array2d[3][3] = {11, 22, 33, 44, 55, 66, 77, 88, 99};
 	int Local=17;
 	float Real=(float)2.17;
 	double Double=1.303;
 	long double Ldouble=1.303;
 	char String[28] = "this is a string";
 	long Long=99;
	
	cout << Char;
	cout << Array1d[0] << Array1d[1] << Array1d[2] ;
	int i,j;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			cout << Array2d[i][j] ;
	cout << String;

	cin >> Char;
	cin >> Array1d[0] >> Array1d[1] >> Array1d[2] ;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			cin >> Array2d[i][j] ;
	cin >> String;

 	return 0;
}


