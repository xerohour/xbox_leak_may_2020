#include <windows.h>

// Memory Window Test

void FuncNoArg(void)
{
	return;
}

int FuncWithArg(int nArg)
{
	return nArg;
}


char FuncWithArg(char chArg)
{
	int		nJustLocalInt = 1;
	double	dJustLocalDouble = 3.14;
	return chArg + 2;
}

										/* Last FuncWithArg line */
int FAR PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	char			chJustChar = 'M'; 
	char			sJustString[] =  "Mamba-hara-mambu-ru"; /* 19 chars in the string */
	unsigned char	uchJustUnsignedChar = 'u';
	short int		nsJustShort = -123;
	unsigned short	unsJustUnsignedShort = 22987;
	int				nJustInt = -65555;
	unsigned int	unJustUnsignedInt = 655551;
	long int		lJustLongInt = -77777;
	unsigned long	ulJustUnsignedLongInt  = 999999;	
	float			fJustFloat = (float) 123.14;
	double			dJustDouble = -777e50;
	long double		dlJustLongDouble = -999e70;
	__int64			sfJustSixtyFourInt = -1234567890987654909;
	unsigned __int64 sfJustSixtyFourUnsignedInt = 9876543212345678123;

	// Arrays for the Column test
	const int number_of_columns_to_be_tested = 3;

	char			asJustString [number_of_columns_to_be_tested + 2] = "Mamb";
	char			asLongString[8*number_of_columns_to_be_tested + 2] = "Mamba-Tumba-Rumba-Shurum";
	short int		ansJustShort[number_of_columns_to_be_tested + 1] = { -123, 12, -34, 45};
	unsigned short	aunsJustUnsignedShort[number_of_columns_to_be_tested + 1] = { 22987, 126, 334, 645};
	int				anJustInt[number_of_columns_to_be_tested + 1] = { -65555, 112, -354, 4451};
	unsigned int	aunJustUnsignedInt[number_of_columns_to_be_tested + 1] = { 655551, 2112, 9834, 45008};
	long int		alJustLongInt[number_of_columns_to_be_tested + 1] = { -77777, 120087, 3477, 99045};
	unsigned long	aulJustUnsignedLongInt[number_of_columns_to_be_tested + 1]  = { 999999, 77712, 36544, 22245};
	float			afJustFloat[number_of_columns_to_be_tested + 1] = { (float) 123.14, (float) 23.16, (float) 60.70, (float) 2.93};
	double			adJustDouble[number_of_columns_to_be_tested + 1] = { -777e50,  77e22,  12e5,  54e10};
	long double		adlJustLongDouble[number_of_columns_to_be_tested + 1] = { -999e70, 89e3,  18e2,  -4e17};
	__int64			asfJustSixtyFourInt[number_of_columns_to_be_tested + 1] = { -1234567890987654909, 1, -2, -3 };
	unsigned __int64 asfJustSixtyFourUnsignedInt[number_of_columns_to_be_tested + 1] = { 9876543212345678123, 12, 45, 1};

	// Pointers
	int		*pnJustIntPtr = &nJustInt;
	void	*pvJustVoidPtrStr = (void*) &sJustString;
	void	*pvJustVoidPtrShort = (void*) &ansJustShort;
	void	*pvJustVoidPtrInt = (void*) &anJustInt;
	void	*pvJustVoidPtrFloat = (void*) &afJustFloat;
	void	*pvJustVoidPtrDouble = (void*) &adJustDouble;
	void	*pvJustVoidPtrInt64 = (void*) &asfJustSixtyFourUnsignedInt;

	nJustInt++;								/* First line for tests */				
	chJustChar++;

	chJustChar = FuncWithArg(chJustChar);	/* Second line for tests */
	FuncNoArg();							/* Third line for tests */

	return 0;

}

