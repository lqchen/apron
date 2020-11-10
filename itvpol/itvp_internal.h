/*
 * itvp_internal.h
 *
 * Private definitions, access to internal structures and algorithms.
 * Use with care.
 *
 * APRON Library / itvpol Domain
 *
 * Copyright (C) Liqian CHEN' 2009
 *
 */

/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#ifndef __ITVP_INTERNAL_H
#define __ITVP_INTERNAL_H

#include "num.h"
#include "bound.h"
#include "itvp_fun.h"
#include "itvp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ********************************************************************** */
/* I. Manager */
/* ********************************************************************** */

/* manager-local data specific to itvpol */
struct _itvp_internal_t {
  ap_funid_t funid;  /* current function */
  ap_funopt_t* funopt; /* local parameters for current function */  
  ap_manager_t* man; /* back-pointer */
  int  newdims;   /* number of new dimensions added right now */
  bool conv; /* raised when a conversion from/to a user type resulted in an overapproximation */
};

/* called by each function to setup and get manager-local data */
static inline itvp_internal_t* itvp_init_from_manager(ap_manager_t* man, ap_funid_t id , size_t size )
{
  itvp_internal_t* pr = (itvp_internal_t*) man->internal;
  pr->funid = id;
  pr->funopt = man->option.funopt+id;
  man->result.flag_exact = man->result.flag_best = true;
  pr->conv = false; 
  return pr;
};

/* ============================================================ */
/* II. itvpols Internal Representation */
/* ============================================================ */
struct _itvp_t {
  size_t dim;     /* total number of variables */ 
  size_t intdim;  /* the first intdim variables are integer ones */ 
  size_t ncons;      /* number of interval linear constraints*/
  numdbl_t *cons;    /* interval linear constraints; size of NbConstraints*(Dimension+1) */
  numdbl_t *bnds;    /* bounds of variables */
  unsigned flag;  /* EMPTY_POL: empty polyhedron; UNIVERSE_POL: universe polyhedron; GENERAL_POL: otherwise*/  
};

struct _fpp_t0 {
  size_t dim;     /* total number of variables */ 
  size_t intdim;  /* the first intdim variables are integer ones */ 
  size_t ncons;      /* number of linear constraints*/
  numdbl_t *cons;    /* linear constraints; size of NbConstraints*(Dimension+1) */
  numdbl_t *bnds;    /* bounds of variables */
  unsigned flag;  /* EMPTY_POL: empty polyhedron; UNIVERSE_POL: universe polyhedron; GENERAL_POL: otherwise*/  
};

/* ============================================================ */
/* II.1 Management */
/* ============================================================ */

itvp_t* itvp_alloc_internal (itvp_internal_t* pr, size_t dim, size_t intdim);
void    itvp_free_internal  (itvp_internal_t* pr, itvp_t* o);
itvp_t* itvp_copy_internal  (itvp_internal_t* pr, itvp_t* o);
itvp_t* itvp_alloc_top      (itvp_internal_t* pr, size_t dim, size_t intdim);
itvp_t* itvp_alloc_urgent   (itvp_internal_t* pr, size_t dim, size_t intdim, size_t ncons);
/* ============================================================ */
/* III. Properties on num_t */
/* ============================================================ */

/*
  num_name              unique type description
  num_incomplete        does the type make algorithms incomplete
  num_safe              is the type safe in case of overflow
  num_fpu               num requires init_fpu to be called
  num_export_double     constraints are output using double instead of mpq
*/
/* 
  Note: at present itvpol only suppport NUM_DOUBLE.  
*/

#if   defined ( NUM_LONGINT )
/* overflows produce unsound results, type not closed by / 2 */
#define num_name             "NUM_LONGINT"
#define num_incomplete       1    
#define num_safe             0
#define num_fpu              0
#define num_export_double    0
#elif defined ( NUM_LONGLONGINT )
#define num_name             "NUM_LONGLONGINT"
#define num_incomplete       1
#define num_safe             0
#define num_fpu              0
#define num_export_double    0

#elif defined ( NUM_MPZ )
/* no overflow, type not closed by / 2 */
#define num_name             "NUM_MPZ"
#define num_incomplete       1
#define num_safe             1
#define num_fpu              0
#define num_export_double    0

#elif defined ( NUM_LONGRAT )
/* complete algorithms, but overflows produce unsound results */
#define num_name             "NUM_LONGRAT"
#define num_incomplete       0
#define num_safe             0
#define num_fpu              0
#define num_export_double    0
#elif defined ( NUM_LONGLONGRAT )
#define num_name             "NUM_LONGLONGRAT"
#define num_incomplete       0
#define num_safe             0
#define num_fpu              0
#define num_export_double    0

#elif defined ( NUM_MPQ )
/* the "perfect" type */
#define num_name             "NUM_MPQ"
#define num_incomplete       0
#define num_safe             1
#define num_fpu              0
#define num_export_double    0

#elif defined ( NUM_DOUBLE )
/* overflow are ok (stick to +oo), type not closed by + and / 2 */
#define num_name            "NUM_DOUBLE"
#define num_incomplete       1
#define num_safe             1
#define num_fpu              1
#define num_export_double    1
#elif defined ( NUM_LONGDOUBLE )
#define num_name             "NUM_LONGDOUBLE"
#define num_incomplete       1
#define num_safe             1
#define num_fpu              1
#define num_export_double    1

/* duh */
#else
#error "No numerical type defined"
#endif


/* ============================================================ */
/* IV. MACROS */
/* ============================================================ */

  /* invalid argument exception */
#define arg_assert(cond,action)						\
  do { if (!(cond)) {							\
      char buf_[1024];							\
      snprintf(buf_,sizeof(buf_),					\
	       "assertion (%s) failed in %s at %s:%i",			\
	       #cond, __func__, __FILE__, __LINE__);		\
      ap_manager_raise_exception(pr->man,AP_EXC_INVALID_ARGUMENT,	\
				 pr->funid,buf_);			\
      action }								\
  } while(0)

  /* malloc with safe-guard */
#define checked_malloc(itvp,ptr,t,nb,action)					\
  do {									\
    (ptr) = (t*)malloc(sizeof(t)*(nb));					\
    if (!(ptr)) {							\
      char buf_[1024];							\
      snprintf(buf_,sizeof(buf_),					\
	       "cannot allocate %s[%lu] for %s in %s at %s:%i",		\
	       #t, (long unsigned)(nb), #ptr,				\
	       __func__, __FILE__, __LINE__);				\
      ap_manager_raise_exception(itvp->man,AP_EXC_OUT_OF_SPACE,		\
				 itvp->funid,buf_);			\
      action }								\
  } while(0)

  /* free with safe-guard */
#define checked_free(pr)					\
      if(pr!=NULL) { free(pr);pr=NULL;}

#ifdef __cplusplus
}
#endif

#endif /* __ITVP_INTERNAL_H */
