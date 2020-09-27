#include "stat_lib.h"

class Inherited : public base1, public base2 
{
public:
	void Func2(void); // Implementation of above
	void Func1(void); // Implementation of above

private:
	int m_int; 
	void Helper(int); // Helper function for implementation
};

void Inherited::Func2 (void)
{
	int i = 2;
	m_int =3;
	Helper(i);
	Helper(m_int);
}

void Inherited::Func1 (void)
{
	int i = 0;	 
}

void Inherited::Helper(int par)
{
	par++;
}

Inherited inh2;

base2* func2()
{
	return (base2*)(&inh2);
}

