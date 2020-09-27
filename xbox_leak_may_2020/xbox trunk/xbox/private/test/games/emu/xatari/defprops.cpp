
#include "stdafx.h"

/**
  The default properties file is generated from the 'stella.pro' file 
  using a sed script
*/
static const char* theScript[] = {
//  #include "DefProps.def"
  0
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char** defaultPropertiesFile()
{
  return theScript;
}

