/* ********************************************************************** */
/* numintIll_def.h */
/* ********************************************************************** */

#ifndef _NUMINTILL_DEF_H_
#define _NUMINTILL_DEF_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "gmp.h"
#include "mpfr.h"

#include "numConfig.h"
/* Require C99 compliant compiler */

#ifdef __cplusplus
extern "C" {
#endif

typedef long long int numintIll_native;
typedef numintIll_native numintIll_t[1];

#define NUMINTILL_ZERO 0LL
#define NUMINTILL_ONE 1LL
#define NUMINTILL_MAX LLONG_MAX
#define NUMILL_MAX LLONG_MAX
#define NUMINTILL_MIN LLONG_MIN

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */

static inline void numintIll_set(numintIll_t a, numintIll_t b)
{ *a = *b; }
static inline void numintIll_set_array(numintIll_t* a, numintIll_t* b, size_t size)
{ memcpy(a,b,size*sizeof(numintIll_t)); }
static inline void numintIll_set_int(numintIll_t a, long int i)
{ *a = (numintIll_native)i; }

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numintIll_init(numintIll_t a)
{ *a = NUMINTILL_ZERO; }
static inline void numintIll_init_array(numintIll_t* a, size_t size)
{
  size_t i;
  for (i=0; i<size; i++) *(a[i]) = NUMINTILL_ZERO;
}
static inline void numintIll_init_set(numintIll_t a, numintIll_t b)
{ numintIll_set(a,b); }
static inline void numintIll_init_set_int(numintIll_t a, long int i)
{ numintIll_set_int(a,i); }

static inline void numintIll_clear(numintIll_t a)
{}
static inline void numintIll_clear_array(numintIll_t* a, size_t size)
{}
static inline void numintIll_swap(numintIll_t a, numintIll_t b)
{ numintIll_t t; *t=*a;*a=*b;*b=*t; }

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void numintIll_neg(numintIll_t a, numintIll_t b)
{ *a = -(*b); }
static inline void numintIll_abs(numintIll_t a, numintIll_t b)
{ *a = llabs(*b); }
static inline void numintIll_add(numintIll_t a, numintIll_t b, numintIll_t c)
{ *a = *b + *c; }
static inline void numintIll_add_uint(numintIll_t a, numintIll_t b, unsigned long int c)
{ *a = *b + c; }
static inline void numintIll_sub(numintIll_t a, numintIll_t b, numintIll_t c)
{ *a = *b - *c; }
static inline void numintIll_sub_uint(numintIll_t a, numintIll_t b, unsigned long int c)
{ *a = *b - c; }
static inline void numintIll_mul(numintIll_t a, numintIll_t b, numintIll_t c)
{ *a = *b * *c; }
static inline void numintIll_mul_2(numintIll_t a, numintIll_t b)
{ *a = *b << 1; }

static inline void numintIll_tdiv_q(numintIll_t q, numintIll_t a, numintIll_t b)
{ *q = *a / *b; }

static inline int numintIll_sgn(numintIll_t a)
{ return (*a==NUMINTILL_ZERO ? 0 : (*a>NUMINTILL_ZERO ? 1 : -1)); }

static inline void numintIll_fdiv_q(numintIll_t q, numintIll_t a, numintIll_t b)
{
  if (numintIll_sgn(a)*numintIll_sgn(b)<0 && *a % *b) *q = *a / *b - 1;
  else *q = *a / *b;
}
static inline void numintIll_cdiv_q(numintIll_t q, numintIll_t a, numintIll_t b)
{
  if (numintIll_sgn(a)*numintIll_sgn(b)>0 && *a % *b) *q = *a / *b + 1;
  else *q = *a / *b;
}
static inline void numintIll_cdiv_qr(numintIll_t q, numintIll_t r, numintIll_t a, numintIll_t b)
{
  numintIll_t qq,rr;
  *qq = *a / *b;
  *rr = *a % *b;
  if (numintIll_sgn(a)*numintIll_sgn(b)>0 && *rr) {
    *q = *qq + 1;
    *r = *rr - *b;
  }
  else {
    *q = *qq;
    *r = *rr;
  }
}

static inline void numintIll_cdiv_2(numintIll_t a, numintIll_t b)
{ *a = (*b>=NUMINTILL_ZERO) ? (*b+1)/2 : *b/2; }
static inline void numintIll_cdiv_q_2exp(numintIll_t a, numintIll_t b, unsigned long int c)
{ *a = (*b >> c)+(*b & ((NUMINTILL_ONE<<c)-NUMINTILL_ONE) ? 1 : 0); }
static inline void numintIll_fdiv_q_2exp(numintIll_t a, numintIll_t b, unsigned long int c)
{ *a = (*b >> c); }
static inline void numintIll_min(numintIll_t a, numintIll_t b, numintIll_t c)
{ *a = (*b<=*c) ? *b : *c; }
static inline void numintIll_max(numintIll_t a, numintIll_t b, numintIll_t c)
{ *a = (*b>=*c) ? *b : *c; }

static const long long numintIll_max_exact_double = 1LL << 52;

static inline void numintIll_sqrt(numintIll_t up, numintIll_t down, numintIll_t b)
{
  double f = sqrt(*b);
  assert(*b>=0);
  if (*b<numintIll_max_exact_double) {
    /* ceil(sqrt(b)) can be exactly represented as a double */
    *up = ceil(f);
    *down = floor(f);
  }
  else {
    /* (unlikely) case where ulp(sqrt(b)) might  be greater than 1 */
    *up = ceil(nextafter(f,+1/0.));
    *down = floor(nextafter(f,0.));
  }
}

static inline void numintIll_mul_2exp(numintIll_t a, numintIll_t b, int c)
{
  if (c>=0) *a = *b << c;
  else numintIll_cdiv_q_2exp(a,b,-c);
}


/* ====================================================================== */
/* Arithmetic Integer Operations */
/* ====================================================================== */

static inline void numintIll_divexact(numintIll_t a, numintIll_t b, numintIll_t c)
{ *a = *b / *c; }
static inline void numintIll_mod(numintIll_t a, numintIll_t b, numintIll_t c)
{ *a = *b % *c; }
static inline numintIll_native _gcd_aux2(numintIll_native a, numintIll_native b)
{ /* a is supposed to be greater than b */
  numintIll_native t;
  while (b!=NUMINTILL_ZERO && a!=b) {
    t = b;
    b = a % b;
    a = t;
  }
  return a;
}
static inline numintIll_native _gcd_aux(numintIll_native a, numintIll_native b)
{
  numintIll_abs(&a,&a);
  numintIll_abs(&b,&b);
  return (a>=b) ? _gcd_aux2(a,b) : _gcd_aux2(b,a);
}
static inline void numintIll_gcd(numintIll_t a, numintIll_t b,  numintIll_t c)
{ *a = _gcd_aux(*b,*c); }

static inline numintIll_native _lcm_aux(numintIll_native a, numintIll_native b)
{
  numintIll_abs(&a,&a);
  numintIll_abs(&b,&b);
  return a / _gcd_aux(a,b) * b;
}
static inline void numintIll_lcm(numintIll_t a, numintIll_t b,  numintIll_t c)
{ *a = _lcm_aux(*b,*c); }

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numintIll_cmp(numintIll_t a, numintIll_t b)
{ return (*a==*b ? 0 : (*a>*b ? 1 : -1)); }
static inline int numintIll_cmp_int(numintIll_t a, long int b)
{ return (*a==b ? 0 : (*a>b ? 1 : -1)); }
static inline bool numintIll_equal(numintIll_t a, numintIll_t b)
{ return *a==*b; }
static inline bool numintIll_integer(numintIll_t a)
{ return true; }

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void numintIll_print(numintIll_t a)
{ printf("%lli",*a); }
static inline void numintIll_fprint(FILE* stream, numintIll_t a)
{ fprintf(stream,"%lli",*a); }
static inline int numintIll_snprint(char* s, size_t size, numintIll_t a)
{ return snprintf(s,size,"%lli",*a); }

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

/* frac -> numintIll */
static inline bool numintIll_set_frac(numintIll_t a, long int i, long int j)
{
  assert(j>0);
  if (i>=0) *a = (i+j-1)/j;
  else *a = i/j;
  return (i%j) ? false : true;
}

/* mpz -> numintIll */
static inline bool numintIll_set_mpz(numintIll_t a, mpz_t b)
{
  int sgn;
  size_t count;
  unsigned long int tab[2];
  
  sgn = mpz_sgn(b);
  mpz_export(&tab,&count,1,sizeof(long int),0,0,b);
  if (count==0){
    *a = 0;
  }
  else {
    *a = tab[0];
    if (count==2){
      *a = *a << (sizeof(long int)*8);
      *a = *a + (long long int)(tab[1]);
      assert(*a>=0LL);
    }
    if (sgn<0) *a = -(*a);
  }
  return true;
}

/* mpq -> numintIll */
static inline bool numintIll_set_mpq_tmp(numintIll_t a, mpq_t b,
				      mpz_t q, mpz_t r)
{
  mpz_cdiv_qr(q,r, mpq_numref(b),mpq_denref(b));
  numintIll_set_mpz(a,q);
  bool res = (mpz_sgn(r)==0);
  return res;
}
static inline bool numintIll_set_mpq(numintIll_t a, mpq_t b)
{
  mpz_t q,r;
  mpz_init(q);mpz_init(r);
  bool res = numintIll_set_mpq_tmp(a,b,q,r);
  mpz_clear(q); mpz_clear(r);
  return res;
}
/* double -> numintIll */
static inline bool numintIll_set_double(numintIll_t a, double b)
{
  double c = ceil(b);
  if (!isfinite(c)) { DEBUG_SPECIAL; *a = 0; return false; }
  *a = c;
  return (b==c);
}
/* mpfr -> numintIll */
static inline bool numintIll_set_mpfr(numintIll_t a, mpfr_t b)
{
  if (!mpfr_number_p(b)) { DEBUG_SPECIAL; numintIll_set_int(a,0); return false; }
  *a = mpfr_get_sj(b,GMP_RNDU);
  return mpfr_integer_p(b);
}
/* numintIll -> int */
static inline bool int_set_numintIll(long int* a, numintIll_t b)
{ *a = (long int)(*b); return true; }

/* numintIll -> mpz */
static inline bool mpz_set_numintIll(mpz_t a, numintIll_t b)
{
  unsigned long long int n;
  unsigned long int rep[2];
  
  n = llabs(*b);
  rep[1] = n & ULONG_MAX;
  rep[0] = n >> (sizeof(long int)*8);
  mpz_import(a,2,1,sizeof(unsigned long int),0,0,rep);
  if (*b<0)
    mpz_neg(a,a);
  return true;
}

/* numintIll -> mpq */
static inline bool mpq_set_numintIll(mpq_t a, numintIll_t b)
{
  mpz_set_ui(mpq_denref(a),1);
  return mpz_set_numintIll(mpq_numref(a),b);
}

/* numintIll -> double */
static inline bool double_set_numintIll(double* a, numintIll_t b)
{
  *a = (double)(*b);
  double aa = -((double)(-(*b)));
  return (*a==aa);
}

/* numintIll -> mpfr */
static inline bool mpfr_set_numintIll(mpfr_t a, numintIll_t b)
{
  return !mpfr_set_sj(a,*b,GMP_RNDU);
}

static inline bool mpz_fits_numintIll(mpz_t a)
{
  size_t size = mpz_sizeinbase(a,2);
  return (size <= sizeof(numintIll_t)*8-1);
}

static inline bool mpq_fits_numintIll_tmp(mpq_t a, mpz_t mpz)
{
  mpz_cdiv_q(mpz,mpq_numref(a),mpq_denref(a));
  return mpz_fits_numintIll(mpz);
}
static inline bool mpq_fits_numintIll(mpq_t a)
{
  mpz_t mpz;
  mpz_init(mpz);
  bool res = mpq_fits_numintIll_tmp(a,mpz);
  mpz_clear(mpz);
  return res;
}
static inline bool double_fits_numintIll(double a)
{
  return isfinite(a) && a>=(double)(-NUMINTILL_MAX) && a<=(double)NUMINTILL_MAX;
}
static inline bool mpfr_fits_numintIll(mpfr_t a)
{
  return mpfr_number_p(a) && mpfr_fits_intmax_p(a,GMP_RNDU);
}
static inline bool numintIll_fits_int(numintIll_t a)
{ return (*a>=-LONG_MAX && *a<=LONG_MAX); }
static inline bool numintIll_fits_float(numintIll_t a)
{ return true; }
static inline bool numintIll_fits_double(numintIll_t a)
{ return true; }
static inline bool numintIll_fits_mpfr(numintIll_t a)
{ return true; }


/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

static inline unsigned char numintIll_serialize_id(void)
{ return sizeof(numintIll_t)/4; }

static inline size_t numintIll_serialize(void* dst, numintIll_t src)
{
  num_store_words8(dst,src,sizeof(numintIll_t));
  return sizeof(numintIll_t);
}

static inline size_t numintIll_deserialize(numintIll_t dst, const void* src)
{
  num_store_words8(dst,src,sizeof(numintIll_t));
  return sizeof(numintIll_t);
}

static inline size_t numintIll_serialized_size(numintIll_t a)
{ return sizeof(numintIll_t); }

#ifdef __cplusplus
}
#endif

#endif