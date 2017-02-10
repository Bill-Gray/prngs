/*
------------------------------------------------------------------------------
isaac64.c: My random number generator for 64-bit machines.
By Bob Jenkins, 1996.  Public Domain.
------------------------------------------------------------------------------
*/
#include "isaac64.h"

#define RANDSIZL   (8)
#define RANDSIZ    (1<<RANDSIZL)

static    uint64_t mm[RANDSIZ];
static    uint64_t aa=0, bb=0, cc=0;
static    uint64_t randrsl[RANDSIZ];
static    int randcnt;

#define ind(mm,x)  (*(uint64_t *)((uint8_t *)(mm) + ((x) & ((RANDSIZ-1)<<3))))
#define rngstep(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (mix) + *(m2++); \
  *(m++) = y = ind(mm,x) + a + b; \
  *(r++) = b = ind(mm,y>>RANDSIZL) + x; \
}

static void isaac64_update( void)
{
  register uint64_t a,b,x,y,*m,*m2,*r;
  int pass, i;

  m=mm; r=randrsl;
  a = aa; b = bb + (++cc);
  m2 = m + RANDSIZ / 2;
  for( pass = 2; pass; pass--, m2 = mm)
     for( i = RANDSIZ / 8; i; i--)
        {
        rngstep(~(a^(a<<21)), a, b, mm, m, m2, r, x);
        rngstep(  a^(a>>5)  , a, b, mm, m, m2, r, x);
        rngstep(  a^(a<<12) , a, b, mm, m, m2, r, x);
        rngstep(  a^(a>>33) , a, b, mm, m, m2, r, x);
        }
  bb = b; aa = a;
}

#define mix(a,b,c,d,e,f,g,h) \
{ \
   a-=e; f^=h>>9;  h+=a; \
   b-=f; g^=a<<9;  a+=b; \
   c-=g; h^=b>>23; b+=c; \
   d-=h; a^=c<<15; c+=d; \
   e-=a; b^=d>>14; d+=e; \
   f-=b; c^=e<<20; e+=f; \
   g-=c; d^=f>>17; f+=g; \
   h-=d; e^=g<<14; g+=h; \
}

#ifndef UINT64_C
#define UINT64_C(a) (a##ui64)
#endif

void isaac64_init( const int flag)
{
   int i;
   uint64_t a,b,c,d,e,f,g,h;
   aa=bb=cc=(uint64_t)0;
   a=b=c=d=e=f=g=h= UINT64_C( 0x9e3779b97f4a7c13);  /* the golden ratio */

   for (i=0; i<RANDSIZ; ++i)
      mm[i]=(uint64_t)0;
   for (i=0; i<4; ++i)                    /* scramble it */
   {
     mix(a,b,c,d,e,f,g,h);
   }

   for (i=0; i<RANDSIZ; i+=8)   /* fill in mm[] with messy stuff */
   {
     if (flag)                  /* use all the information in the seed */
     {
       a+=randrsl[i  ]; b+=randrsl[i+1]; c+=randrsl[i+2]; d+=randrsl[i+3];
       e+=randrsl[i+4]; f+=randrsl[i+5]; g+=randrsl[i+6]; h+=randrsl[i+7];
     }
     mix(a,b,c,d,e,f,g,h);
     mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
     mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
   }

   if (flag)
   {        /* do a second pass to make all of the seed affect all of mm */
     for (i=0; i<RANDSIZ; i+=8)
     {
       a+=mm[i  ]; b+=mm[i+1]; c+=mm[i+2]; d+=mm[i+3];
       e+=mm[i+4]; f+=mm[i+5]; g+=mm[i+6]; h+=mm[i+7];
       mix(a,b,c,d,e,f,g,h);
       mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
       mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
     }
   }

   isaac64_update( );          /* fill in the first set of results...  */
   isaac64_update( );          /* repeat is not _absolutely_ necessary */
   randcnt = RANDSIZ;    /* prepare to use the first set of results */
}

/*
------------------------------------------------------------------------------
 Call isaac64( ) to retrieve a single 64-bit random value
------------------------------------------------------------------------------
*/
uint64_t isaac64( void)
{
   const uint64_t rval=
        (!randcnt-- ? (isaac64_update( ), randcnt=RANDSIZ-1, randrsl[randcnt]) :
                 randrsl[randcnt]);

   return( rval);
}

