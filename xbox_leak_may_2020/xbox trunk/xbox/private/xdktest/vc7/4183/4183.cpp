// Repro code:

struct kkk
{
	operator const float &()
	{
		static const float r = 0.F;
		return r;
	}
};

void __cdecl main()
{
	float kk;
	kk = kkk();

	class Foo2
	{
	public:
		virtual ~Foo2() {}
	};
}
