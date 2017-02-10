#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

/* See 'blum.c' for a description of this code.  Basically,
it allows one to compute Blum Blum Shub pseudorandom numbers
using 128-bit integers.  However,  you only get those on
64-bit machines.  */

#ifdef USE_128_BIT_INTS
#define uint128_t __uint128_t

         /* Following is fastest method,  involving no */
         /* use of "factored BBS",  but you need 64-bit gcc */
         /* to get the required 128-bit integers : */
uint64_t blum_blum_shub( const uint64_t x)
{
   const uint64_t p = 4294901687;
   const uint64_t q = 4294956167;
   uint128_t x2 = (uint128_t)x * (uint128_t)x;
   const uint64_t pq = p * q;

   return( (uint64_t)( x2 % (uint128_t)pq));
}
#else
uint64_t blum_blum_shub( const uint64_t x)
{
   const uint64_t p = 2479637879;
   const uint64_t q = 2479662359;
   const uint64_t b = x % p;
   const uint64_t d = x % q;
   const uint64_t part1 = (((x / p) * d) % q) * p;
   const uint64_t part2 = (((x / q) * b) % p) * q;
   const uint64_t part3 = b * d;
   uint64_t rval = (part1 + part2 + part3) % (p * q);

   return( rval);
/* return( (x * x) % (p * q));     mathematically right,  but it overflows */
}
#endif

static uint64_t get_bbs_pseudorandom( uint64_t x, uint64_t *rand_val)
{
   unsigned count;
   uint64_t random = x;

   for( count = 16; count; count--)
      {
      x = blum_blum_shub( x);
      random = (random << 4) ^ (uint64_t)x;
      }
   *rand_val = random;
   return( x);
}

int main( const int argc, const char **argv)
{
   uint64_t x = 3;
   unsigned i;
   uint64_t random_value;

   for( i = 0; i < 100; i++)
      {
      x = get_bbs_pseudorandom( x, &random_value);
      printf( "%17" PRIx64, random_value);
      if( i % 4 == 3)
         printf( "\n");
      }
   if( argc > 1)
      {
      i = (unsigned)atoi( argv[1]);
      while( i--)
         x = get_bbs_pseudorandom( x, &random_value);
      printf( "%17" PRIx64, (uint64_t)x);
      }
   return( 0);
}
