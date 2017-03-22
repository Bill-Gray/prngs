#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Feistel network test.  See https://en.wikipedia.org/wiki/Feistel_network */

/* "func" must hash the input value and the key to create something */
/* that,  ideally,  ensures that any individual bit flipped in 'ival' */
/* causes about half of the bits in the output to be flipped,  in a   */
/* "random" fashion.  This is the avalanche condition.  In practice,  */
/* we just try to flip a lot of bits and rely on the fact that we're  */
/* doing four (or many) rounds.                                       */

uint32_t func( uint32_t ival, const uint32_t key)
{
   ival += (ival << 15);
   ival ^= (ival >> 11);
   return( ival + key);
}

int main( const int argc, const char **argv)
{
   uint32_t r, l;
   uint32_t keys[4] = { 0x3141592f, 0x53a897e3, 0x238b626d, 0x32785028 };
   int round;

   if( argc < 3)
      {
      printf( "Need r and l values\n");
      return( -1);
      }
   sscanf( argv[1], "%x", &r);
   sscanf( argv[2], "%x", &l);
   for( round = 0; round < 4; round++)
      {
      const uint32_t l1 = r;

      r = l ^ func( r, keys[round]);
      l = l1;
      }
   printf( "Hashed:   %08x %08x\n", r, l);
   for( round = 3; round >= 0; round--)
      {
      const uint32_t r1 = l;

      l = r ^ func( l, keys[round]);
      r = r1;
      }
   printf( "Unhashed: %08x %08x\n", r, l);
   return( 0);
}
