#include <stdio.h>
#include <stdlib.h>

/* NOTE:  Mostly of historical interest only (see comments below as
to why I don't actually use this myself.)

Shamelessly copied (with small modifications) from _Numerical Recipes
in C_,  chapter 7, p. 282.  I ran into problems because the rand( )
function was,  somewhat astonishingly,  not all that random in
Microsoft C or MinGW!  Both use the soi-disant 'random' generator

static long my_seed;

void srand(long s)
{
  seed = s;
}

int rand()
{
  my_seed = my_seed * 214013 + 2531011;
  return ( my_seed >> 16) & 0x7FFF;
}

   ...which isn't even funny.  (In MinGW's defense,  I think they
feel they really have to follow MS closely,  even when that means doing
some really stupid things.)

   For the following,  _Numerical Recipes_ sez:

   "Long period (> 2E+18) random number generator of L'Ecuyer with
Bays-Durham shuffle and added safeguards... Call with idum a negative
integer to initialize;  thereafter,  don't alter idum between successive
deviates in a sequence."

   NOTE something that is (in my opinion) a major problem:  the return
value can't be greater than IM1,  which is 2^31 - 85.  Thus,  some values
at the extreme end won't be reached.  If your project depends on a
uniform distribution,  and in particular if you're sampling and expect
to sample _everything_,  you might miss out some exotic behavior in
the last part of the [0, 1) space.

   Also NOTE that this hasn't been subjected to the same mass scrutiny
as the Mersenne Twister and WELL algorithms.  I'd use those instead,
and essentially bury any concerns that there might be some non-randomness
in your PRNG.         */

static unsigned long rand2( const long seed)
{
   const long IM1 = 2147483563;
   const long IM2 = 2147483399;
   const long IMM1 = (IM1 - 1L);
   const long IA1 = 40014;
   const long IA2 = 40692;
   const long IQ1 = 53668;
   const long IQ2 = 52774;
   const long IR1 = 12211;
   const long IR2 = 3791;
   const int NTAB = 32;
   const long NDIV = 1 + IMM1 / NTAB;
   static long idum = -1L, idum2 = 123456789, iy = 0;
   static long iv[32];
   int j;
   long k;

   if( seed)
      idum = seed;
   if( idum <= 0)       /* initialization */
      {
      if( !idum)
         idum = 1;
      else
         idum = -idum;
      idum2 = idum;
      for( j = NTAB + 7; j >= 0; j--)        /* load shuffle table */
         {                                   /* (after 8 warm-ups) */
         k = idum / IQ1;
         idum = IA1 * (idum - k * IQ1) - k * IR1;
         if( idum < 0)
            idum += IM1;
         if( j < NTAB)
            iv[j] = idum;
         }
      iy = iv[0];
      }
   k = idum / IQ1;      /* We start here if we're not initializing */
   idum = IA1 * (idum - k * IQ1) - k * IR1;
   if( idum < 0)
      idum += IM1;
   k = idum2 / IQ2;
   idum2 = IA2 * (idum2 - k * IQ2) - k * IR2;
   if( idum2 < 0)
      idum2 += IM2;
   j = iy / NDIV;
   iy = iv[j] - idum2;
   iv[j] = idum;
   if( iy < 1)
      iy += IMM1;
   return( iy);
}

#include <stdio.h>

int main( const int argc, const char **argv)
{
   int i;

   for( i = 0; i < 1000; i++)
      printf( "%08lx%s", rand2( 0), (i % 8 == 7 ? "\n" : " "));
   return( 0);
}
