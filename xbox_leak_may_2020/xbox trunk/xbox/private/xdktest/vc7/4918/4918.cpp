#ifndef _XBOX
#include <stdio.h>
#define PRINTF printf
#else
#include <xtl.h>
extern "C" void __cdecl DbgPrint(const char *, ...);
#define PRINTF DbgPrint
#endif

#define BUGGY_RESULTS

class StringPtr
{
public:
   StringPtr() : Data(NULL)   {};
   StringPtr(char *data) : Data(data)   {};

   char *Data;
};

template<typename KEY, typename VALUE>
class Resource
{
public:

   Resource() {};
   ~Resource(){ };
   bool  Find(KEY key, VALUE *value);
};

template<typename KEY, typename VALUE>
bool Resource<KEY, VALUE>::Find(KEY key, VALUE *value)
{
   PRINTF("Address %p\n", key.Data);
   return false;
}

Resource<StringPtr, int> Function;

char *Names[] = {{"Name1"}, {"Name2"}, {"Name3"}, {NULL}};

void __cdecl main()
{
   char **AnimNames = Names;
   int thing;

#ifdef BUGGY_RESULTS
   /* demonstrate that key.Data is wrong inside the Find function */
   while( *AnimNames )
   {
      PRINTF("%s, Address pointer: %p\n", *AnimNames, *AnimNames);
      Function.Find(*AnimNames++, &thing );
   }
#else
  /* produces correct output */
   while( *AnimNames )
   {
      PRINTF("%s, Address pointer: %p\n", *AnimNames, *AnimNames);
      Function.Find(*AnimNames, &thing );
      AnimNames++;
   }
#endif

#ifdef _XBOX
   while (1)
	   ;
#endif
}
