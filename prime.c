#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

/* Finds and displays factors for a given range of numbers.  Lists
the primes,  then lists those primes which are 'safe' and '2-safe',
as well as those which are good for Blum-Blum-Shub PRNGs. */

void find_primes( uint32_t *factors, uint64_t start, const unsigned array_len);

uint64_t atouint64_t( const char *ival)
{
   uint64_t rval = 0;

   while( *ival >= '0' && *ival <= '9')
      rval = rval * 10 + (uint64_t)( *ival++ - '0');
   return( rval);
}

int main( const int argc, const char **argv)
{
   uint64_t start = (argc > 1 ? atouint64_t( argv[1]) : 0);
   unsigned array_len = (argc > 2 ? (unsigned)atoi( argv[2]) : 1);
   uint32_t *factors = (uint32_t *)calloc( array_len, sizeof( uint32_t));
   uint32_t *half_factors = (uint32_t *)calloc( array_len / 2, sizeof( uint32_t));
   uint32_t *quarter_factors = (uint32_t *)calloc( array_len / 4, sizeof( uint32_t));
   unsigned i, j;
   int n_digits;
   uint64_t tval = start + array_len - 1;
   unsigned n_per_line;

   if( argc < 3 || start <= 0 || array_len <= 1)
      {
      printf( "Need a starting number and a number of values\n");
      return( -1);
      }
   for( n_digits = 0; tval; tval /= 10)
      n_digits++;
   n_per_line = 80 / (int)( n_digits + 1);
   find_primes( factors, start, array_len);
   for( i = 0; i < array_len && i < 10000; i++)
      printf( "%*" PRIu64 "  %u\n", n_digits, (uint64_t)i + start, factors[i]);
   printf( "\nPrimes are:\n");
   for( i = j = 0; j < 10000 && i < array_len; i++)
      if( !factors[i])
         {
         j++;
         printf( "%*" PRIu64 "%s", n_digits, (uint64_t)i + start,
                     (j % n_per_line) ? " " : "\n");
         }
   printf( "\n");

   find_primes( half_factors, start / 2, array_len / 2);
   printf( "\nSafe primes (p and (p-1)/2 are both prime) are:\n");
   for( i = j = 0; j < 10000 && i < array_len; i++)
      if( !factors[i] && !half_factors[i / 2])
         {
         j++;
         printf( "%*" PRIu64 "%s", n_digits, (uint64_t)i + start,
                     (j % n_per_line) ? " " : "\n");
         }
   printf( "\n");

   find_primes( quarter_factors, start / 4, array_len / 4);
   n_per_line = 80 / (int)( n_digits + 2);
   printf( "\n2-Safe primes (p, (p-1)/2, (p-3)/4 are all prime); p %% 16 == 7 cases *ed, are:\n");
   for( i = j = 0; i < array_len; i++)
      if( !factors[i] && !half_factors[i / 2] && !quarter_factors[i / 4])
         {
         j++;
         printf( "%*" PRIu64 "%c%s", n_digits, (uint64_t)i + start,
                     (i + start) % 16 == 7 ? '*' : ' ',
                     (j % n_per_line) ? " " : "\n");
         }
   printf( "\n");
   return( 0);
}
