
/***************************************************************************

                   Copyright (c) 1994 Microsoft Corporation


Test:           localeco.c

Abstract:       Verify the localconv() function returns the correct values.


===========================================================================
Syntax

===========================================================================
Priority:               1
Products:               WIN NT


Revision History:

    Date        emailname       description
----------------------------------------------------------------------------
    10-05-94    a-timke         created
    11-17-94    a-timke         Fixed n_sign_posn test.
    02-27-95    a-timke         Modified to reflect ANSI implementation
                                of 'grouping' and 'mon_grouping'.
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "test.h"

#ifdef _WIN64

#define CHECK(e)  if (!(e))\
   {printf("  %s(%d): %s (%I64)\n", __FILE__, __LINE__, #e, (__int64)e); return 1;}
#define CHECKE(e, r)  if ((e)!=(r))\
   {printf("  %s(%d): %s != %s (%I64 != %I64)\n", __FILE__, __LINE__, #e, #r, (__int64)e, (__int64)r); return 1;}
#define CHECKNE(e, r) if ((e)==(r))\
   {printf("  %s(%d): %s == %s (%I64 != %I64)\n", __FILE__, __LINE__, #e, #r, (__int64)e, (__int64)r); return 1;}

#else

#define CHECK(e)  if (!(e))\
   {printf("  %s(%d): %s (%d)\n", __FILE__, __LINE__, #e, e); return 1;}
#define CHECKE(e, r)  if ((e)!=(r))\
   {printf("  %s(%d): %s != %s (%d != %d)\n", __FILE__, __LINE__, #e, #r, e, r); return 1;}
#define CHECKNE(e, r) if ((e)==(r))\
   {printf("  %s(%d): %s == %s (%d != %d)\n", __FILE__, __LINE__, #e, #r, e, r); return 1;}

#endif 

static int perfect = 0;
static char test[] = __FILE__;

#if DEBUG
static void ViewLocale(void)
{
  struct lconv *p = localeconv();

  printf("decimal_point     = <%s>\n", p->decimal_point);
  printf("thoushands_sp     = <%s>\n", p->thousands_sep);
  printf("grouping          = <%s>\n", p->grouping);
  printf("curr_symbol       = <%s>\n", p->int_curr_symbol);
  printf("currency_symbol   = <%s>\n", p->currency_symbol);
  printf("mon_decimal_point = <%s>\n", p->mon_decimal_point);
  printf("mon_thousands_sep = <%s>\n", p->mon_thousands_sep);
  printf("mon_grouping      = <%s>\n", p->mon_grouping);
  printf("positive_sign     = <%s>\n", p->positive_sign);
  printf("negative_sign     = <%s>\n", p->negative_sign);
  printf("int_frac_digits   = <%d>\n", p->int_frac_digits&0xff);
  printf("frac_digits       = <%d>\n", p->frac_digits&0xff);
  printf("p_cs_precedes     = <%d>\n", p->p_cs_precedes&0xff);
  printf("p_sep_by_space    = <%d>\n", p->p_sep_by_space&0xff);
  printf("n_cs_precedes     = <%d>\n", p->n_cs_precedes&0xff);
  printf("n_sep_by_space    = <%d>\n", p->n_sep_by_space&0xff);
  printf("p_sign_posn       = <%d>\n", p->p_sign_posn&0xff);
  printf("n_sign_posn       = <%d>\n", p->n_sign_posn&0xff);
}
#endif

//Verify "C" locale values.
static int localeconvV1(void)
{
  struct lconv *pLc = localeconv();

  CHECKE(strcmp(pLc->decimal_point,     "."), 0);
  CHECKE(strcmp(pLc->thousands_sep,     ""), 0);
  CHECKE(strcmp(pLc->grouping,          ""), 0);
  CHECKE(strcmp(pLc->int_curr_symbol,   ""), 0);
  CHECKE(strcmp(pLc->currency_symbol,   ""), 0);
  CHECKE(strcmp(pLc->mon_decimal_point, ""), 0);
  CHECKE(strcmp(pLc->mon_thousands_sep, ""), 0);
  CHECKE(strcmp(pLc->mon_grouping,      ""), 0);
  CHECKE(strcmp(pLc->positive_sign,     ""), 0);
  CHECKE(strcmp(pLc->negative_sign,     ""), 0);
  CHECKE(pLc->int_frac_digits&0xff,      127);
  CHECKE(pLc->frac_digits&0xff,          127);
  CHECKE(pLc->p_cs_precedes&0xff,        127);
  CHECKE(pLc->p_sep_by_space&0xff,       127);
  CHECKE(pLc->n_cs_precedes&0xff,        127);
  CHECKE(pLc->n_sep_by_space&0xff,       127);
  CHECKE(pLc->p_sign_posn&0xff,          127);
  CHECKE(pLc->n_sign_posn&0xff,          127);

  return 0;
}

//Verify US locale values, category=LC_ALL.
static int localeconvV2(void)
{
  struct lconv *pLc;

  CHECKNE(setlocale(LC_ALL, "C"), NULL);
  CHECKE(localeconvV1(), 0);
  CHECKNE(setlocale(LC_ALL,  "English_United States.OCP"), NULL);
  pLc = localeconv();

  CHECKE(strcmp(pLc->decimal_point,     "."),   0);
  CHECKE(strcmp(pLc->thousands_sep,     ","),   0);
  CHECKE(strcmp(pLc->grouping,          "\3"),  0);
  CHECKE(strcmp(pLc->int_curr_symbol,   "USD"), 0);
  CHECKE(strcmp(pLc->currency_symbol,   "$"),   0);
  CHECKE(strcmp(pLc->mon_decimal_point, "."),   0);
  CHECKE(strcmp(pLc->mon_thousands_sep, ","),   0);
  CHECKE(strcmp(pLc->mon_grouping,      "\3"),  0);
  CHECKE(strcmp(pLc->positive_sign,     ""),    0);
  CHECKE(strcmp(pLc->negative_sign,     "-"),   0);
  CHECKE(pLc->int_frac_digits&0xff,      2);
  CHECKE(pLc->frac_digits&0xff,          2);
  CHECKE(pLc->p_cs_precedes&0xff,        1);
  CHECKE(pLc->p_sep_by_space&0xff,       0);
  CHECKE(pLc->n_cs_precedes&0xff,        1);
  CHECKE(pLc->n_sep_by_space&0xff,       0);
  CHECKE(pLc->p_sign_posn&0xff,          3);
  CHECKE(pLc->n_sign_posn&0xff,          0);

  return 0;
}

//Verify US locale values, category=LC_MONETARY.
static int localeconvV3(void)
{
  struct lconv *pLc;

  CHECKNE(setlocale(LC_ALL, "C"), NULL);
  CHECKE(localeconvV1(), 0);
  CHECKNE(setlocale(LC_MONETARY,  "English_United States.OCP"), NULL);
  pLc = localeconv();

  CHECKE(strcmp(pLc->decimal_point,     "."),   0);
  CHECKE(strcmp(pLc->thousands_sep,     ""),    0);
  CHECKE(strcmp(pLc->grouping,          ""),    0);
  CHECKE(strcmp(pLc->int_curr_symbol,   "USD"), 0);
  CHECKE(strcmp(pLc->currency_symbol,   "$"),   0);
  CHECKE(strcmp(pLc->mon_decimal_point, "."),   0);
  CHECKE(strcmp(pLc->mon_thousands_sep, ","),   0);
  CHECKE(strcmp(pLc->mon_grouping,      "\3"),  0);
  CHECKE(strcmp(pLc->positive_sign,     ""),    0);
  CHECKE(strcmp(pLc->negative_sign,     "-"),   0);
  CHECKE(pLc->int_frac_digits&0xff,      2);
  CHECKE(pLc->frac_digits&0xff,          2);
  CHECKE(pLc->p_cs_precedes&0xff,        1);
  CHECKE(pLc->p_sep_by_space&0xff,       0);
  CHECKE(pLc->n_cs_precedes&0xff,        1);
  CHECKE(pLc->n_sep_by_space&0xff,       0);
  CHECKE(pLc->p_sign_posn&0xff,          3);
  CHECKE(pLc->n_sign_posn&0xff,          0);

  return 0;
}

//Verify US locale values, category=LC_NUMERIC.
static int localeconvV4(void)
{
  struct lconv *pLc;

  CHECKNE(setlocale(LC_ALL, "C"), NULL);
  CHECKE(localeconvV1(), 0);
  CHECKNE(setlocale(LC_NUMERIC,  "English_United States.OCP"), NULL);
  pLc = localeconv();

  CHECKE(strcmp(pLc->decimal_point,     "."),   0);
  CHECKE(strcmp(pLc->thousands_sep,     ","),   0);
  CHECKE(strcmp(pLc->grouping,          "\3"),  0);
  CHECKE(strcmp(pLc->int_curr_symbol,   ""),    0);
  CHECKE(strcmp(pLc->currency_symbol,   ""),    0);
  CHECKE(strcmp(pLc->mon_decimal_point, ""),    0);
  CHECKE(strcmp(pLc->mon_thousands_sep, ""),    0);
  CHECKE(strcmp(pLc->mon_grouping,      ""),    0);
  CHECKE(strcmp(pLc->positive_sign,     ""),    0);
  CHECKE(strcmp(pLc->negative_sign,     ""),    0);
  CHECKE(pLc->int_frac_digits&0xff,      127);
  CHECKE(pLc->frac_digits&0xff,          127);
  CHECKE(pLc->p_cs_precedes&0xff,        127);
  CHECKE(pLc->p_sep_by_space&0xff,       127);
  CHECKE(pLc->n_cs_precedes&0xff,        127);
  CHECKE(pLc->n_sep_by_space&0xff,       127);
  CHECKE(pLc->p_sign_posn&0xff,          127);
  CHECKE(pLc->n_sign_posn&0xff,          127);

  return 0;
}

//Verify Swedish locale values, category=LC_ALL.
static int localeconvV5(void)
{
  struct lconv *pLc;

  CHECKNE(setlocale(LC_ALL, "C"), NULL);
  CHECKE(localeconvV1(), 0);
  CHECKNE(setlocale(LC_ALL,  "Swedish"), NULL);
  pLc = localeconv();

  CHECKE(strcmp(pLc->decimal_point,     ","),    0);
  CHECKE(strcmp(pLc->thousands_sep,     "\xa0"), 0);
  CHECKE(strcmp(pLc->grouping,          "\3"),   0);
  CHECKE(strcmp(pLc->int_curr_symbol,   "SEK"),  0);
  CHECKE(strcmp(pLc->currency_symbol,   "kr"),   0);
  CHECKE(strcmp(pLc->mon_decimal_point, ","),    0);
#if 1 /*!defined(TEST_WIN95)*/
  CHECKE(strcmp(pLc->mon_thousands_sep, "."), 0); //QA: "\xa0"
#else
  CHECKE(strcmp(pLc->mon_thousands_sep, "\xa0"), 0); //QA: "\xa0"
#endif
  CHECKE(strcmp(pLc->mon_grouping,      "\3"),   0);
  CHECKE(strcmp(pLc->positive_sign,     ""),     0);
  CHECKE(strcmp(pLc->negative_sign,     "-"),    0);
  CHECKE(pLc->int_frac_digits&0xff,      2);
  CHECKE(pLc->frac_digits&0xff,          2);
  CHECKE(pLc->p_cs_precedes&0xff,        0);
  CHECKE(pLc->p_sep_by_space&0xff,       1);
  CHECKE(pLc->n_cs_precedes&0xff,        0);
  CHECKE(pLc->n_sep_by_space&0xff,       1);
  CHECKE(pLc->p_sign_posn&0xff,          1);
  CHECKE(pLc->n_sign_posn&0xff,          1);

  return 0;
}

#if defined(_M_M68K) || defined(_M_MPPC)
int localecoEntry(void){
  startest();
  checke(localeconvV1(), 0);
  checke(localeconvV2(), 1);
  checke(localeconvV3(), 1);
  checke(localeconvV4(), 1);
  checke(localeconvV5(), 1);
  finish();
}
#else
int localecoEntry(void){
  startest();
  checke(localeconvV1(), 0);
  checke(localeconvV2(), 0);
  checke(localeconvV3(), 0);
  checke(localeconvV4(), 0);
  checke(localeconvV5(), 0);
  finish();
}
#endif
