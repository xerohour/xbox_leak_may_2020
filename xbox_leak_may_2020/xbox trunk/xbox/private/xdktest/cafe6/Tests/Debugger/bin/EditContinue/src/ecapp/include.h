class baseInclude
{
private:
	int m_private;

protected:
	int m_protected;

public: 
	baseInclude(void) {m_private = 1; m_protected = 2;}
	virtual ~baseInclude(void){};

	int m_public;
	char *m_pchar;
	virtual void virtual_func(int i);
	virtual void virtual_func1() { int i = 5; };
};

class derived1_Include : public baseInclude
{
private:
	char ch;

public:
	derived1_Include(void){ch = 'a';}
};

class derived2_Include : public derived1_Include
{
private:
	float f;

public:
	derived2_Include(void){f = 1.0f;}
};


	

