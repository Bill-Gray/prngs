// #include "w19937a.h"

#define W 32
#define R 624
#define P 31
#define MASKU (0xffffffffU>>(W-P))
#define MASKL (~MASKU)
#define MAT0NEG(t,v) (v^(v<<(-(t))))
#define M1 70
#define M2 179
#define M3 449
#define STATE(i) (state[(i) <= loc ? loc - (i) : loc - (i) + R])

int well19937a( unsigned int *state)
{
   static int loc = 0;
   const unsigned int m1 = STATE(R-M1);
   const unsigned int m3 = STATE(R-M3);
   const unsigned int sloc = state[loc];
   const unsigned int z1 = sloc ^ (sloc << 25) ^ m1 ^ (m1 >> 27);
   const unsigned int z2 = (STATE( R-M2) >> 9) ^ m3 ^ (m3 >> 1);
   unsigned int rval;

   if( loc >= 2)
      rval = (state[loc - 1] & MASKL) | (state[loc - 2] & MASKU);
   else
      rval = (STATE(1) & MASKL) | (STATE(2) & MASKU);
   state[loc] = z1 ^ z2;
   rval ^= (z1 << 9) ^ (z2 << 21) ^ (state[loc] >> 21);
   if( !loc)
      loc = R - 1;
   else
      loc--;
   state[loc] = rval;
   return( rval);
}

int well19937at( unsigned int *state)
{
   unsigned int rval = well19937a( state);
   const unsigned int TEMPERB = 0xe46e1700U;
   const unsigned int TEMPERC = 0x9b868000U;

   rval ^= (rval << 7) & TEMPERB;
   rval ^= (rval << 15) & TEMPERC;
   return( rval);
}

static void seed_state_array( unsigned int *state)
{
   int i;
            /* Seeds with values from Numerical Recipes, chap 7.1,  page */
            /* 284 ("an even quicker [linear congruential] generator").  */
   state[0] = 314159265;
   for( i = 1; i < 624; i++)
      state[i] = state[i - 1] * 1664525 + 1013904223;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( const int argc, const char **argv)
{
   int i, use_old_version = 0;
   int delays = 0, tempering = 0;
   unsigned int data[624];

   for( i = 1; i < argc; i++)
      if( argv[i][0] == '-')
         switch( argv[i][1])
            {
            case 'v':
               use_old_version = 1;
               break;
            case 't':
               tempering = 1;
               break;
            case 'd':
               delays = atoi( argv[i] + 2);
               printf( "%d delays\n", delays);
               break;
            default:
               printf( "Option '%s' unrecognized\n", argv[i]);
               break;
            }
   seed_state_array( data);
   InitWELLRNG19937a( data);
   if( use_old_version)
      while( delays--)
         WELLRNG19937a( );
   else
      if( tempering)
         while( delays--)
            well19937at( data);
         else
            while( delays--)
               well19937a( data);
   printf( "%.2lf seconds elapsed\n",
                  (double)clock( ) / (double)CLOCKS_PER_SEC);
   for( i = 0; i < 10000; i++)
      printf( "%08x%s",
             use_old_version ? WELLRNG19937a( ) :
             (tempering ? well19937at( data) : well19937a( data)),
                  (i % 8 == 7 ? "\n" : " "));
   return( 0);
}
