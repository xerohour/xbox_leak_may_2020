typedef int FAR PASCAL CallBackFCN();

__declspec(dllexport) int FAR callback(CallBackFCN lpfnCallBack, int x);
__declspec(dllexport) int  FAR  foo( int paramA, int paramB );
__declspec(dllexport) int  FAR  recursive_dll(int lim , int code);
