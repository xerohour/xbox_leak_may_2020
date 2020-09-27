class base2
{
protected:
	int base2_var;
public:
	base2(void){base2_var = 2;}
	// inherited, non-overloaded, non-overridden member function
	int base2_func(int i);
	virtual int base2_virtual_func(int i);
};
