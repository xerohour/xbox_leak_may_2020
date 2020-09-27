#ifndef MD5_HXX
#define MD5_HXX

#include "bspf.h"

/**
  Get the MD5 Message-Digest of the specified message with the 
  given length.  The digest consists of 32 hexadecimal digits.

  @param buffer The message to compute the digest of
  @param length The length of the message
  @return The message-digest
*/
string MD5(const uInt8* buffer, uInt32 length);

#endif

