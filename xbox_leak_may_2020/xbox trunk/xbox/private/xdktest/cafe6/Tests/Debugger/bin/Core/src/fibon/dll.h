enum CALLTYPE{ __INT64=0, UNSIGNED, SIGNEDLONG};

class __declspec(dllexport) dll_class
	{
		private:
			int dll_class_var;

			__int64 fibon(__int64 n);
			long fibon(unsigned int u);
			long fibon(long m);
			long fibon(float f);
			__int64 fibontype(__int64 n, CALLTYPE type = UNSIGNED);
			int average(int first, ...);
			double factorial(double nn);
			void fiboncases(int argc, char *argv[]);

		public:
			/*__declspec(dllexport)*/ dll_class(void);
			/*__declspec(dllexport)*/ int dll_class_func(int i);
	};
