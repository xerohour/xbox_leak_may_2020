#include <xtl.h>

class vec4
{
public :
	float x,y,z,w;

	vec4() {}
	vec4(const vec4 &bug):x(bug.x), y(bug.y), z(bug.z), w(bug.w) {}
};

class mat
{
public:
	vec4 vec[4];
};

class mat2
{
public:
	mat a, b;

	mat2() {}
	mat2(const mat &y):a(y), b(y) {}
};

mat2 *yy1, *yy2;

void FooBar(void)
{
	*yy1 = mat2(yy1->a);
}

void __cdecl main()
{
}
