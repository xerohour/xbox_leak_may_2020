//
//	cpptest.h
//

typedef int FOOTYPE;

class A {
public:
	virtual ~A();
	virtual void vfoo1();
	virtual void vfoo2();

private:
	int m_i;
};

class B {
public:
	virtual ~B();
	virtual void vfoo2();
	virtual void vfoo3();

private:
	int m_i;
};

class MI : public A, public B {
public:
	virtual void vfoo2();
	int InlineFoo() { return m_i; }
	static FOOTYPE StaticFoo(MI,MI);

private:
	int m_i;
	static m_si;
};

void cpptest();
void Afoos( A& );

#define FOOMAX(a,b) ( ((a)>(b))?(a):(b) )