#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

uint64_t gcd( uint64_t a, uint64_t b);
unsigned find_factors( uint64_t *factors, uint64_t ival,
                                     const unsigned max_factors);

/* Code to find good p and q values for a Blum-Blum-Shub generator.  p
and q should be primes congruent with 3 modulo 4,  and the greatest
common denominator of the Euler totient function (also known as
'phi') of p-1 and q-1 should be two (to get maximum cycle length).

   Finding such pairs gets to be difficult as p and q approach the
maximum values for 32-bit integers.  (And even more so as one goes
past that... but that's not my goal here;  see 'blum.c' for an
explanation of what's going on.)  The following code looks for
suitable large primes and stores an array of them, along with the
Euler totient function of (prime-1).  As new large primes are found,
the GCD of each possible pair (phi(p-1), phi(q-1)) is computed.
Eventually,  we find that the GCD in question is two,  and we're done.

   It appears to me that it's necessary for one of p or q to be a
safe prime;  i.e.,  not only is (say) p a prime number,  but so is
(p-1)/2.  The other number in the pair can be an "ordinary" prime.
*/

static uint64_t euler_totient_function( const uint64_t ival, const bool output_factors)
{
   uint64_t rval = ival, factors[64];
   unsigned i, n_factors = find_factors( factors, ival, 64);

   if( output_factors)
      {
      i = 0;
      printf( "%lu = ", ival);
      while( i < n_factors)
         {
         unsigned j;

         for( j = i + 1; j < n_factors && factors[j] == factors[i]; j++)
            ;
         if( j > i + 1)
            printf( "(%lu^%u)", (unsigned long)factors[i], j - i);
         else
            printf( "%lu", (unsigned long)factors[i]);
         i = j;
         if( i < n_factors)
            printf( " * ");
         }
      printf( "\n");
      }
   rval -= rval / factors[0];
   for( i = 1; i < n_factors; i++)
      if( factors[i] != factors[i - 1])
         rval -= rval / factors[i];
   return( rval);
}

#define uint128_t __uint128_t

static uint64_t quick_primality_test( const uint64_t ival)
{
   uint64_t pow_2 = 2, result = 1;
   uint64_t zval = ival;

   while( zval)
      {
      if( zval & 1)
         result = (uint64_t)(((uint128_t)result * (uint128_t)pow_2) % (uint128_t)ival);
      pow_2 = (uint64_t)(((uint128_t)pow_2 * (uint128_t)pow_2) % (uint128_t)ival);
      zval >>= 1;
      }
   return( result);
}

#define MAX_RUN 10000

int main( const int argc, const char **argv)
{
   uint64_t array[MAX_RUN], phi[MAX_RUN];
   int i, j;

   if( argc == 2)
      {
      uint64_t ival = (uint64_t)atoi( argv[1]);

      printf( "%lu\n", euler_totient_function( ival, true));
      printf( "%lu\n", quick_primality_test( ival));
      return( 0);
      }
   for( i = 0; i < MAX_RUN; i++)
      {
      uint64_t min_factor;

      if( i < argc - 1)
         array[i] = (uint64_t)( atol( argv[i + 1]) | 3L);
      else           /* extrapolate from previous values */
         array[i] = (array[i - 1] * 2 - array[i - 2]) | 3L;
                 /* following is 2^64 / sqrt(3), rounded down to be */
                 /* congruent with 3 modulo 4: */
      array[i] = 10650232656628343499UL - 100000000000 * i;
/*    array[i] = 2479700523 - 10000 * i;           ...similar for 2^32 / sqrt(3) */
/*    array[i] = 4294967295 - 10000 * i;           ...similar for 2^32 - 1  */
      while( quick_primality_test( array[i]) != 2)
         array[i] -= (int64_t)4;
      array[i] += (int64_t)4;
      do
         {                               /* Decrement array[i] by four  */
         array[i] -= (int64_t)4;         /* until it's a prime          */
         find_factors( &min_factor, array[i], 1);
         }
         while( array[i] != min_factor);
/*    printf( "%d: %lu\n", i, array[i]);     */
      phi[i] = euler_totient_function( array[i] - (uint64_t)1, false);
      for( j = 0; j < i; j++)
         if( array[i] && array[j])
            {
            const uint64_t zval = gcd( phi[i], phi[j]);

            if( zval == 2)   /* show only our successes */
               printf( "   using %lu, %lu: %lu\n", array[i], array[j],
                          zval);
            if( zval == 2)
               {
               printf( "  phi( %lu) = %lu; phi( %lu) = %lu\n",
                     array[i] - 1, phi[i],
                     array[j] - 1, phi[j]);
               array[i] = array[j] = 0;      /* mark so they don't get used again */
/*             return( 0);    */
               }
            }
      }
   return( 0);
}

