class base1
{
protected:
	// inherited member variable.
	int base1_var;
public:
	base1(void){base1_var = 1;}
	int base1_func(int i);
	virtual int base1_virtual_func(int i);
	int base1_overloaded_func(int i);
	int base1_overridden_func(int i);
};
