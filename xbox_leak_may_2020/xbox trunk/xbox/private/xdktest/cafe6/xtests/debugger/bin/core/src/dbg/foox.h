#define hasPARAMS
#define FUNCTYPE _stdcall
#define returnsVALUE


#ifdef hasPARAMS
#define  fooParams int paramA, int paramB
#else
#define  fooParams
#endif

#ifdef returnsVALUE
#define  fooReturn  int
#else
#define  fooReturn  void
#endif

fooReturn FUNCTYPE foo( fooParams );
fooReturn FUNCTYPE foo2( fooParams );
