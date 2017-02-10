#include <stdint.h>
#include <stdio.h>

void find_primes( uint32_t *factors, uint64_t start,
                                    const unsigned array_len);
uint64_t gcd( uint64_t a, uint64_t b);
uint32_t integer_square_root( const uint64_t ival);
unsigned find_factors( uint64_t *factors, uint64_t ival,
                                     const unsigned max_factors);

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

uint32_t integer_square_root( const uint64_t ival)
{
   uint32_t oval = 0, bit_mask = ((uint64_t)1) << 31;

   while( bit_mask)
      {
      const uint32_t new_oval = (oval | bit_mask);

      if( (uint64_t)new_oval * (uint64_t)new_oval <= ival)
         oval = new_oval;
      bit_mask >>= 1;
      }
   return( oval);
}

uint64_t find_a_factor( const uint64_t ival)
{
   uint32_t factor, next_fact;
   const uint32_t mult = 2 * 3 * 5 * 7 * 11 * 13 * 17;    /* = 2310 */
   const uint32_t step = (200000000 / mult) * mult;

   for( factor = 2; factor <= mult; factor++)
      if( !(ival % factor))
         return( factor);

   for( factor = mult; factor * factor <= ival && factor < ival / mult;
                            factor = next_fact)
      {
      uint32_t fact, fact0;

      if( factor < step)
         next_fact = factor * 2;
      else
         next_fact = factor + step;
/*    printf( "fact %u, next %u\n", (unsigned)factor, (unsigned)next_fact);
*/    for( fact0 = factor + 1; fact0 < factor + mult; fact0 += 2)
         if( fact0 % 3 && (fact0 % 5) && fact0 % 7 && fact0 % 11
                     && fact0 % 13 && fact0 % 17)
            {
            for( fact = fact0; fact < next_fact; fact += mult)
               if( !(ival % fact))
                  return( fact);
            }
      }
   return( 0);
}

unsigned find_factors( uint64_t *factors, uint64_t ival,
                                     const unsigned max_factors)
{
   unsigned n_factors = 0;
   uint32_t factor;

   while( n_factors < max_factors &&
                       (factor = find_a_factor( ival)) != 0)
      {
      factors[n_factors++] = factor;
      ival /= factor;
      }
   if( ival > 1)
      factors[n_factors++] = ival;
   return( n_factors);
}

/* Note that in the following,  if you ask for primes < 5,  the code */
/* uses a 'beginnings' array.  Also,  if it looks as if a factor may */
/* be checked that would go into the start of the array,  we split   */
/* the array recursively to avoid it.  In both cases,  we're just    */
/* avoiding problems where the code would decide that N is evenly    */
/* divisible by N.                                                   */

void find_primes( uint32_t *factors, uint64_t start, unsigned array_len)
{
   unsigned i, j;
   const uint64_t end = start + (uint64_t)array_len;
   const uint32_t max_factor = integer_square_root( end);

   while( array_len && start < 5)
      {
      const unsigned char beginning[5] = { 0, 1, 0, 0, 2 };

      *factors++ = beginning[start++];
      array_len--;
      }
   if( !array_len)
      return;
   if( max_factor + 2 >= start)
      {
      const unsigned first_part = max_factor + 3 - start;

      find_primes( factors, start, first_part);
      factors += first_part;
      start += first_part;
      array_len -= first_part;
      }
   for( i = 0; i < array_len; i++)
      factors[i] = (((i ^ start) & 1) ? 0 : 2);
   start--;
   for( i = 2 - (unsigned)( start % 3); i < array_len; i += 3)
      factors[i] = 3;
   for( j = 5; j <= max_factor; j += 4)
      {
#if 0
      for( i = j - 1 - (unsigned)( start % j); i < array_len; i += j)
         factors[i] = j;
      j += 2;
      for( i = j - 1 - (unsigned)( start % j); i < array_len; i += j)
         factors[i] = j;
#endif
      for( i = j - (unsigned)( start % j); i <= array_len; i += j)
         factors[i - 1] = j;
      j += 2;
      for( i = j - (unsigned)( start % j); i <= array_len; i += j)
         factors[i - 1] = j;
      }
}
