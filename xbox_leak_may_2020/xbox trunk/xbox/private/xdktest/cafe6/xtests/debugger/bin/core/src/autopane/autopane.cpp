#include <xtl.h> //#include <windows.h>

char array[4] = "abc";
int global;
int i;
int *pi = &i;


class base
	{
	private:
		int m_private;
	public:
		virtual void func(int i);
		int m_public;
		char *m_pchar;
	} b;


void base::func(int i)

	// verify that function arguments appear when prolog reached
	{
	// verify that no locals appear after executing prolog
	while(0);

	// verify that only expressions from current statement and previous statement are added
	m_private = i;
	m_public = i;
	}


int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
	
	{
	i = 0;
	*pi = 1;
	while(0);

	int *pint = &i;
	while(0);

	// navigate the stack to another function and back
	// verify that function calls aren’t added
	// step through prolog with this pane active, then select auto pane
	b.func(0);
	while(0);

	// verify that multi-line statements get scanned back 10 lines
	int j[60];

	for(int k = 0; k < 60; k++)
		j[k] = 1;
	
	k = j[1] +
		j[2] +
		j[3] +
		j[4] +
		j[5] + 
		j[6] +
		j[7] +
		j[8] +
		j[9] +
		j[10] +
		j[11] +
		j[12] +
		j[13] +
		j[14] +
		j[15];

	while(0);
	while(0);

	// verify that the unsupported operators are not included
	b.func(j[0]);
	i = (int)(char)j[1];
	j[2]++;
	j[3]--;
	i = !j[4];
	i = ~j[5];
	++j[6];
	--j[7];
	i = sizeof(j[8]);
	j[9] = 1, j[10] = 1;
	i = j[11] * j[12];
	i = j[13] / j[14];
	i = j[15] % j[16];
	i = j[17] + j[18];
	i = j[19] - j[20];
	i = j[21] << j[22];
	i = j[23] >> j[24];
	i = j[25] < j[26];
	i = j[27] <= j[28];
	i = j[29] > j[30];
	i = j[31] >= j[32];
	i = j[33] == j[34];
	i = j[35] != j[36];
	i = j[37] & j[38];
	i = j[39] ^ j[40];
	i = j[41] | j[42];
	i = j[43] && j[44];
	i = j[45] || j[46];
	i = j[47];
	i += j[49];
	i -= j[50];
	i *= j[51];
	i /= j[52];
	i %= j[53];
	i <<= j[54];
	i >>= j[55];
	i &= j[56];
	i ^= j[57];
	i |= j[58];
	i = j[59] ? j[60] : 0;

	array[0] = 'x';
	while(0);
	
	array[(5 + 15) / 10 * 2 - (4 + (int)'\x00')] = '1';
	while(0);

	array[array[4]] = 'a';
	while(0);

	base *pb2;
	base *pb = new base;
	pb2 = pb;

	*pb = *pb;

	//b.*m_pchar = 'a';
	//pb->*m_pchar = 'b';
	//char **ppchar = (char**)&array;
	//*ppchar[0] = 'c';
	//**ppchar = '1';

	pb->m_public = 0;
	pb->func(0);
	while(0);

	b.m_public = 1;
	b.func(1);
	while(0);

	while(0);
	while(0); // no expressions

	int before_comments = 0;
	// pi = 1;
	/* 
	global = 2;
	*/
	while(0);

	int i2 = 0;
	i2 = 1;
	while(0);

	::global = 1;
	while(0);

	// verify that no expressions with error values are added
	i = 1;
	while(1);
	return 0;
	}
