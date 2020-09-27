class base1
{
public:
	virtual void Func1() = 0;
	int Num;
};

class base2 
{
public:
	virtual void Func2() = 0;
	long LongNum;
};

base2* func2();
