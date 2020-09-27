/* Suite++: The Plum Hall Validation Suite for C++
 * VERSION: 1
 * AND      The Plum Hall Validation Suite for C
 * VERSION: 3
 * Unpublished copyright (c) 1991, Plum Hall Inc (Editor)
 * DATE:    91/07/01
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

#if HAS_PROTOTYPES
#define PARMS(x) x
#else
#define PARMS(X) ()
#endif	/* HAS_PROTOTYPES */

int ivalue PARMS((int));
long lvalue PARMS((long));
float fvalue PARMS((double));
double dvalue PARMS((double));

#if NEW_STYLE_FN_DEF
#define FN_DEF1(a1, d1) (d1)
#else /* !NEW_STYLE_FN_DEF */
#define FN_DEF1(a1, d1) (a1) d1;
#endif /* NEW_STYLE_FN_DEF */

