/* The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1986-1991, Chiron Systems Inc and Plum Hall Inc.
 * VERSION: 3
 * DATE:    91/07/01
 * The "ANSI" mode of this suite corresponds to official ANSI C, X3.159-1989.
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

CHAR O_c = 0;
SCHAR O_sc = 0;
SHORT O_s = 0;
INT O_i = 0;
UCHAR O_uc = 0;
USHORT O_us = 0;
UINT O_ui = 0;
LONG O_l = 0;
ULONG O_ul = 0;
FLOAT O_f = 0;
DOUBLE O_d = 0;
LDOUBLE O_ld = 0;

	struct XX {
		CHAR c;
		SCHAR sc;
		SHORT s;
		INT i;
		UCHAR uc;
		USHORT us;
		UINT ui;
		LONG l;
		ULONG ul;
		FLOAT f;
		DOUBLE d;
		LDOUBLE ld;
		};
	static struct XX x1;
	static struct XX x2 = {
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	}; 
struct XX Oc() { struct XX x1; x1=x2; if (O_c)x1.c=O_c; O_c=0; return(x1);}
struct XX Osc() { struct XX x1; x1=x2; if (O_sc)x1.sc=O_sc; O_sc=0; return(x1);}
struct XX Os() { struct XX x1; x1=x2; if (O_s)x1.s=O_s; O_s=0; return(x1);}
struct XX Oi() { struct XX x1; x1=x2; if (O_i)x1.i=O_i; O_i=0; return(x1);}
struct XX Ouc() { struct XX x1; x1=x2; if (O_uc)x1.uc=O_uc; O_uc=0; return(x1);}
struct XX Ous() { struct XX x1; x1=x2; if (O_us)x1.us=O_us; O_us=0; return(x1);}
struct XX Oui() { struct XX x1; x1=x2; if (O_ui)x1.ui=O_ui; O_ui=0; return(x1);}
struct XX Ol() { struct XX x1; x1=x2; if (O_l)x1.l=O_l; O_l=0; return(x1);}
struct XX Oul() { struct XX x1; x1=x2; if (O_ul)x1.ul=O_ul; O_ul=0; return(x1);}
struct XX Of() { struct XX x1; x1=x2; if (O_f)x1.f=O_f; O_f=0; return(x1);}
struct XX Od() { struct XX x1; x1=x2; if (O_d)x1.d=O_d; O_d=0; return(x1);}
struct XX Old() { struct XX x1; x1=x2; if (O_ld)x1.ld=O_ld; O_ld=0; return(x1);}
