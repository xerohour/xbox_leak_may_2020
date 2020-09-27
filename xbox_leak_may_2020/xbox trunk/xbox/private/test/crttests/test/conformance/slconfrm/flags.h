/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-1998 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as	agreed in writing with
 *  Dinkumware, Ltd.
 */

/* flags.h  */

#ifndef FLAGS_H
#define FLAGS_H

#define BIG_OBJECT	((size_t)(-1) / 2)	/* or smaller, as needed */

#if _MSC_VER < 1200
 #define PAIR_DEFINITION	typedef pair<Char, int> Pair
#endif

#define THROW(x)	/* VC++ ignores exception specifications */
#define THROW0()

#define DISALLOW_ACTUAL_DESTRUCTOR_NAME	0
#define DISALLOW_ARRAY_NEW_DELETE	0
#define DISALLOW_MEMBER_TEMPLATES	0
#define DISALLOW_PARTIAL_SPECIALIZATION	1
#define DISALLOW_PLACEMENT_DELETE	0
#define DISALLOW_TOLERANT_TEMPLATES	0
#define DISALLOW_TYPENAME	0
#define DISALLOW_USER_NAME_PROTECTION	1

/* #define DISALLOW_ACTUAL_DESTRUCTOR_NAME */
/* #define DISALLOW_ARRAY_NEW_DELETE */
/* #define DISALLOW_BINARY_AND_UNARY_AMPERSAND */
/* #define DISALLOW_BOOLEANS */
/* #define DISALLOW_CONDITIONS */
/* #define DISALLOW_DIGRAPHS */
/* #define DISALLOW_EMPTY_BRACKETS_IN_DELETE */
/* #define DISALLOW_EXCEPTIONS */
/* #define DISALLOW_IMPLICIT_INSTANTIATION */
/* #define DISALLOW_MEMBER_TEMPLATES */
/* #define DISALLOW_MUTABLE */
/* #define DISALLOW_NAMESPACES */
/* #define DISALLOW_NEW_CASTS */
/* #define DISALLOW_OVERLOADED_POSTFIX_INCREMENT */
/* #define DISALLOW_PARTIAL_SPECIALIZATION */
/* #define DISALLOW_PLACEMENT_DELETE */
/* #define DISALLOW_QUALIFIED_CLASS_NAMES */
/* #define DISALLOW_REDUNDANT_PARENS_IN_DECLARATOR */
/* #define DISALLOW_RETURN_TEMPORARY_WITH_DESTRUCTOR */
/* #define DISALLOW_RTTI */
/* #define DISALLOW_TEMPLATES */
/* #define DISALLOW_TOLERANT_POINTER_OPERATOR */
/* #define DISALLOW_TOLERANT_TEMPLATES */
/* #define DISALLOW_TYPENAME */
/* #define DISALLOW_USER_NAME_PROTECTION */
/* #define SKIP_PENDING_CASES */

		/* SKIP INDIVIDUAL TESTS */
#define SKIP_18614T11  /* unexpected() called by exception-specification failure */

#define SKIP_265T41a  /* abs(long), div(long, long) clashes w/valarray */
#define SKIP_2782T21  /* <cwchar> synopsis */

 #if (defined(_DLL) && !defined(_STATIC_CPPLIB)) || defined(_DEBUG)
  /*
   * These tests expect replacement new/delete to be called for internal
   * Library operations, which doesn't happen in the DLL or Debug CRTs
   */
  #define SKIP_XD313T65  /* streambuf::overflow calls operator new[] */
  #define SKIP_XD313T73  /* streambuf::overflow calls operator delete */
 #endif

#endif  /* of FLAGS_H */
