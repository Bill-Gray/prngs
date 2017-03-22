#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define uint128_t __uint128_t

uint64_t atouint64_t( const char *ival)
{
   uint64_t rval = 0;

   while( *ival >= '0' && *ival <= '9')
      rval = rval * 10 + (uint64_t)( *ival++ - '0');
   return( rval);
}

uint64_t gcd( uint64_t a, uint64_t b)
{
   uint64_t tval;

   if( a < b)
      {
      tval = a;
      a = b;
      b = tval;
      }
   while( b)
      {
      tval = a % b;
      a = b;
      b = tval;
      }
   return( a);
}

int main( const int argc, const char **argv)
{
   const uint64_t p = atouint64_t( argv[1]);
   const uint64_t q = atouint64_t( argv[2]);
   uint128_t pq = (uint128_t)p * (uint128_t)q;
   uint64_t z = (argc > 3 ? atouint64_t( argv[3]) : 0);
   uint64_t z2 = (uint64_t)(((uint128_t)z * (uint128_t)z) % (uint128_t)pq);
   uint64_t count = 0, seed;
   unsigned n_shorts_found = 0;
   const uint64_t phi_p = (p - 3) / 2;
   const uint64_t phi_q = (q - 3) / 2;
   const uint64_t divisor = gcd( phi_p, phi_q);
   const uint64_t max_cycle = phi_p * (phi_q / divisor);

   printf( "Theoretical maximum cycle length: %" PRIu64 "; divisor %" PRIu64 "\n",
            max_cycle, divisor);
   if( z)
      {
      z = z2;
      do
         {
         z = (uint64_t)(((uint128_t)z * (uint128_t)z) % (uint128_t)pq);
         count++;
         if( argc > 4)
            {
            printf( "%20" PRIu64, z);
            if( count % 4 == 3)
               printf( "\n");
            }
         }
         while( z != z2);
      printf( "\nCount = %" PRIu64 "\n", count);
      }
   else     /* with only p and q given on command line,  check _all_ seeds: */
      for( seed = 2; seed < pq - 1; seed++)
         if( seed % p > 1 && seed % q > 1 && seed % p < p - 1 && seed % q < q - 1)
            {
            z = seed;
            z2 = (uint64_t)(((uint128_t)z * (uint128_t)z) % (uint128_t)pq);
            z = z2;
            count = 0;
            do
               {
               if( count == max_cycle)
                  {
                  printf( "Timed out\n");
                  return( -1);
                  }
               z = (uint64_t)(((uint128_t)z * (uint128_t)z) % (uint128_t)pq);
               count++;
               }
               while( z != z2);
            if( count != max_cycle)
               {
               printf( "Short cycle: length %" PRIu64 ", seed %" PRIu64 "\n",
                           count, seed);
               n_shorts_found++;
               if( n_shorts_found == 20)
                  return( -1);
               }
            }
   return( 0);
}
