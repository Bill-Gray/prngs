#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* I think the combsort algorithm may be most effective if
the 'gaps' are relatively prime.  This code looks for such gaps
and produces a table of them,  for a given exponential difference
between adjacent gaps and a starting table.   */

uint64_t gcd( uint64_t a, uint64_t b);
uint64_t integer_square_root( const uint64_t ival);
unsigned find_factors( uint64_t *factors, uint64_t ival,
                                     const unsigned max_factors);

static void show_factors( const uint64_t ival)
{
   uint64_t factors[64];
   uint n_factors, i;

   printf( "%lu = ", ival);
   n_factors = find_factors( factors, ival, 64);
   i = 0;
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

uint64_t atouint64_t( const char *ival)
{
   uint64_t rval = 0;

   while( *ival >= '0' && *ival <= '9')
      rval = rval * 10 + (uint64_t)( *ival++ - '0');
   return( rval);
}

int main( const int argc, const char **argv)
{
   uint64_t array[590];
   int n_filled = argc - 2, i;
   const double exponent = atof( argv[argc - 1]);

   if( argc == 4 && argv[1][0] == 'g')
      {
      const uint64_t a = atouint64_t( argv[2]);
      const uint64_t b = atouint64_t( argv[3]);

      printf( "GCD(%lu,%lu)=%lu\n", a, b, gcd( a, b));
      return( 0);
      }
   if( argc == 2)
      {
      const uint64_t ival = atouint64_t( argv[1]);

      printf( "sqrt = %lu\n", integer_square_root( ival));
      show_factors( ival);
      return( 0);
      }
   for( i = 0; i < n_filled; i++)
      array[i] = (uint64_t)atol( argv[i + 1]);
   while( (int64_t)array[n_filled - 1] > 0)
      {
      int count, got_it;
      uint64_t zval;

      zval = (uint64_t)( (double)array[n_filled - 1] * exponent);
      if( (int64_t)zval < 0)
         return( 0);
      for( count = got_it = 0; !got_it; count++)
         {
         if( count & 1)
            array[n_filled] = zval - (uint64_t)( count / 2);
         else
            array[n_filled] = zval + (uint64_t)( count / 2);
         for( i = 0; i < n_filled && gcd( array[n_filled], array[i]) == 1; i++)
            ;
/*       printf( "Looked at %lu;  failed against %lu\n", array[n_filled],
                     array[i]);
*/       if( i == n_filled)
            got_it = 1;
         }
      printf( "%u: ", n_filled);
      show_factors( array[n_filled]);
/*    printf( "%d: %d (%d)\n", n_filled, array[n_filled], zval); */
      n_filled++;
      }
   return( 0);
}
