#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

/* See 'blum.c' for a description of this code.  Basically,
it allows one to compute Blum Blum Shub pseudorandom numbers
using 128-bit integers.  However,  you only get those on
64-bit machines.  */

#define uint128_t __uint128_t

static uint128_t blum_blum_shub( const uint128_t x)
{
   const uint64_t p = UINT64_C( 10650232656610726727);
   const uint64_t q = UINT64_C( 10650232656628286999);
   const uint128_t pq = (uint128_t)p * (uint128_t)q;
   const uint64_t b = (uint64_t)(x % (uint128_t)p);
   const uint64_t d = (uint64_t)(x % (uint128_t)q);
   const uint128_t part1 = (((x / (uint128_t)p) * (uint128_t)d) % (uint128_t)q) * (uint128_t)p;
   const uint128_t part2 = (((x / (uint128_t)q) * (uint128_t)b) % (uint128_t)p) * (uint128_t)q;
   const uint128_t part3 = (uint128_t)b * (uint128_t)d;
   uint128_t rval = (part1 + part2 + part3) % pq;

   return( rval);
/* return( (x * x) % (p * q));     mathematically right,  but it overflows */
}

static uint128_t get_bbs_pseudorandom( uint128_t x, uint64_t *rand_val)
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
   uint128_t x = 3;
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
