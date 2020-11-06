/* Mostly cut & pasted from http://www.pcg-random.org/download.html */
/* Permuted Congruential Generator */

#include <stdint.h>

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

uint32_t pcg32_random_r( pcg32_random_t* rng)
{
    const uint32_t xorshifted = (uint32_t)( ((rng->state >> 18u) ^ rng->state) >> 27u);
    const int rot = (int)( rng->state >> 59u);
    const uint64_t multiplier = 6364136223846793005ULL;

    /* Advance internal state                                         */
    rng->state = rng->state * multiplier + (rng->inc | 1);
    /* Calculate output function (XSH RR), uses old state for max ILP */
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

uint64_t pcg32_random_r_64( pcg32_random_t* rng)
{
    uint32_t xorshifted = (uint32_t)( ((rng->state >> 18u) ^ rng->state) >> 27u);
    int rot = (int)( rng->state >> 59u);
    const uint64_t multiplier = 6364136223846793005ULL;
    uint64_t rval;

    /* Advance internal state                                         */
    rng->state = rng->state * multiplier + (rng->inc | 1);
    /* Calculate output function (XSH RR), uses old state for max ILP */
    rval = ((uint64_t)xorshifted << (32 - rot)) | ((uint64_t)xorshifted << (32 | ((-rot) & 31)));
    rval &= ~((uint64_t)0xffffffff);
    /* Advance internal state                                         */
    xorshifted = (uint32_t)( ((rng->state >> 18u) ^ rng->state) >> 27u);
    rot = (int)( rng->state >> 59u);
    rng->state = rng->state * multiplier + (rng->inc | 1);
    /* Calculate output function (XSH RR), uses old state for max ILP */
    rval |= (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    return( rval);
}

uint32_t pcg32_random_rev_r( pcg32_random_t* rng)
{
    uint32_t xorshifted, rot;
    const uint64_t inverse = 13877824140714322085ULL;

    /* Reverse internal state                                         */
    rng->state = (rng->state - (rng->inc | 1)) * inverse;
    xorshifted = ((rng->state >> 18u) ^ rng->state) >> 27u;
    rot = rng->state >> 59u;
    /* Calculate output function (XSH RR), uses old state for max ILP */
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

#include <stdio.h>
#include <stdlib.h>

/* In the above code,  'multiplier' and 'inverse' are inverses modulo
2^64;  i.e.,  multiplier * inverse = 1 (mod 2^64).  The following
function was used to compute 'inverse',  and could be used to recompute
it if 'multiplier' were changed. */

static uint64_t multiplicative_inverse( uint64_t ival)
{
   uint64_t rval = ival;

   while( ival != 1)
      {
      printf( "%llu %llu\n", (unsigned long long)ival,
                             (unsigned long long)rval);
      ival *= ival;
      rval *= ival;
      }
   return( rval);
}

int main( const int argc, const char **argv)
{
   pcg32_random_t rng;
   int i;
   uint64_t multiplier = 6364136223846793005ULL;

   printf( "Inverse = %llu\n", (unsigned long long)
            multiplicative_inverse( multiplier));
   rng.inc = (uint64_t)atol( argv[1]);
   if( argc < 3)
      rng.state = 0;
   else
      rng.state = (uint64_t)atol( argv[2]);
   for( i = 0; i < 160; i++)
      printf( "%08x %s", pcg32_random_r(  &rng), (i % 8 == 7) ? "\n" : "");
   printf( "\nRunning backward :\n");
   for( i = 0; i < 160; i++)
      printf( "%08x %s", pcg32_random_rev_r( &rng), (i % 8 == 7) ? "\n" : "");
   printf( "\n64-bit version :\n");
   for( i = 0; i < 80; i++)
      printf( "%016lx %s", pcg32_random_r_64(  &rng), (i % 4 == 3) ? "\n" : "");
   return( 0);
}
