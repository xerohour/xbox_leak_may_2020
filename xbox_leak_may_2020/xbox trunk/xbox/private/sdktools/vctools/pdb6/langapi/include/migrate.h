/* 
   Migrate.h
  
   The contain of this file should eventually get merged into the NT hearder
   files.

   Copyright (C) 1997, Microsoft Corp.  All Rights Reserved.
*/

//
// Type values for COR methods
//

#define IMAGE_SYM_TYPE_NEW             0x0010  // cor_new
#define IMAGE_SYM_TYPE_INSTANCEOF      0x0020  // cor_instanceof
#define IMAGE_SYM_TYPE_VIRTUAL         0x0030  // cor_virtual
#define IMAGE_SYM_TYPE_METHOD          0x0040  // cor_[static, special, newnew]
