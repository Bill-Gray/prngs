/* Based on code and ideas from

http://www.iro.umontreal.ca/~panneton/WELLRNG.html

   ...implementing the WELL19937a algorithm,  due to Francois
Panneton and Pierre L'Ecuyer, University of Montreal;  Makoto
Matsumoto, Hiroshima University.                        */

#define W 32
#define R 624
#define P 31
#define MASKU (0xffffffffU>>(W-P))
#define MASKL (~MASKU)
// #define MAT0NEG(t,v) (v^(v<<(-(t))))         No longer used
#define M1 70
#define M2 179
#define M3 449
#define STATE(i) (sptr[(i) <= loc ? -(i) : R - (i)])

#include <stdint.h>
#include "well.h"

            /* initialization routine for Mersenne Twister:    */
            /* slightly modified linear conguential generator. */
            /* Dunno if there is an "approved" init routine    */
            /* for the WELL generators?                        */
void init_well19937a( uint32_t *state, const uint32_t seed)
{
   uint32_t i;

   state[0] = seed;
   for( i = 1; i < R; i++)      /* 0x6c078965 = 1812433253 */
      state[i] = 1812433253 * (state[i - 1] ^ (state[i - 1] >> 30)) + i;
   state[R] = 0;
}

/* The last value in the 'state' array holds our current location within */
/* that array.  Which is why WELL19937_ARRAY_LENGTH is 625,  instead of  */
/* equal to R=624.  By doing this,  the entire state of the generator is */
/* held in the array.  You could have multiple generators going at any   */
/* given time (i.e.,  the code is re-entrant/thread-safe).               */

uint32_t well19937a( uint32_t *state)
{
   int loc = (int)state[R];
   uint32_t *sptr = state + loc;
   const uint32_t m1 = STATE(R-M1);
   const uint32_t m3 = STATE(R-M3);
   const uint32_t sloc = *sptr;
   const uint32_t z1 = sloc ^ (sloc << 25) ^ m1 ^ (m1 >> 27);
   const uint32_t z2 = (STATE( R-M2) >> 9) ^ m3 ^ (m3 >> 1);
   uint32_t rval;

   if( loc >= 2)
      rval = (sptr[-1] & MASKL) | (sptr[-2] & MASKU);
   else
      rval = (STATE(1) & MASKL) | (STATE(2) & MASKU);
   *sptr = z1 ^ z2;
   rval ^= (z1 << 9) ^ (z2 << 21) ^ (*sptr >> 21);
   if( !loc)
      loc = R - 1;
   else
      loc--;
   state[loc] = rval;
   state[R] = (uint32_t)loc;
   return( rval);
}

uint32_t well19937at( uint32_t *state)
{
   uint32_t rval = well19937a( state);
   const uint32_t TEMPERB = 0xe46e1700U;
   const uint32_t TEMPERC = 0x9b868000U;

   rval ^= (rval << 7) & TEMPERB;
   rval ^= (rval << 15) & TEMPERC;
   return( rval);
}

/* We get a 64-bit random number by smushing together two 32-bit ones: */

uint64_t well19937a_64( uint32_t *state)
{
   return( (uint64_t)well19937a( state)
        | ((uint64_t)well19937a( state) << 32));
}

uint64_t well19937at_64( uint32_t *state)
{
   return( (uint64_t)well19937at( state)
        | ((uint64_t)well19937at( state) << 32));
}

/* The above functions allow one to get 32 or 64 random bits.  But  */
/* suppose one wants N random bits?  N is frequently one (coin-flip */
/* situation) or 52 (computing a double-precision value,  which has */
/* 52 bits of precision).  If one only wants,  say,  one bit,  it's */
/* wasteful to compute a full 32 or 64 bits each time.  And slightly */
/* wasteful to discard twelve bits when computing a double.          */
/*   The following function attempts to evade this.  Random values   */
/* are computed 64 bits at a time.  If fewer bits than this are      */
/* needed, the remaining bits are stored (in the static variable     */
/* 'bits') for the next call.  At each call,  there may be enough    */
/* bits stored to answer the call without needing to compute 64      */
/* more.  Or not,  in which case another 64 bits are generated,      */
/* with some of them possibly saved for the next call.               */
/*   NOTE THAT THIS IS ALMOST COMPLETELY UNTESTED.  Logic appears    */
/* sound,  but dunno yet if the code is!                             */
/*   Also,  the code is not thread-safe,  given the static           */
/* variables.  These should be stored at the end of the 'state'      */
/* array,  making it three 32-bit values longer (i.e.,  increasing   */
/* WELL19937_ARRAY_LENGTH from 625 to 628).                          */

uint64_t well19937at_get_n_bits( uint32_t *state, const int n_bits)
{
   static int n_bits_available = 0;
   static uint64_t bits;
   uint64_t rval;

   n_bits_available -= n_bits;
   if( n_bits_available >= 0)     /* we can return that many bits  */
      {                           /* without having to compute any */
      rval = bits >> n_bits_available;
      }
   else        /* Wups!  We have a negative number of random bits */
      {        /* cached,  and need to generate 64 more of them:  */
      rval = well19937at_64( state);
      bits ^= rval >> (-n_bits_available);
      n_bits_available += 64;
      }
   return( rval & (((uint64_t)1 << n_bits) - (uint64_t)1));
}

/* One can take a 64-bit integer,  mask off the upper twelve bits, */
/* OR in a suitable constant,  and get the IEEE equivalent of a    */
/* floating-point value from 1 to 2.  Subtract 1 from it,  and     */
/* you can get uniform random numbers [0, 1).  A trick grabbed     */
/* from _Numerical Recipes in C_,  p. 285,  and updated from the   */
/* 32-bit world to the 64-bit one.                                 */
/*   The authors of _NR_ note that this is a really dirty trick,   */
/* highly machine-dependent.  It'll break on a non-IEEE machine.   */
/* But I don't know if those are even made anymore.                */
/*   NOTE that it's important to use the 64-bit integer versions   */
/* of the above functions.  If you just grab a 32-bit int and      */
/* scale it to [0, 1),  then 19 bits of the 51-bit precision of a  */
/* double-precision floating-point value will be left at zero.     */
/*   For comparison,  I tried the 'conventional way' of getting a  */
/* 64-bit value and dividing by 2^64 (as shown below). This was    */
/* about 25% slower.                                               */

#ifndef UINT64_C
#define UINT64_C(a) (a##ui64)
#endif

double well19937a_d( uint32_t *state)
{
#ifdef CONVENTIONAL_WAY
   const double two_to_the_minus_64 =
                1. / (65536. * 65536. * 65536. * 65536.);

   return( (double)well19937a_64( state) * two_to_the_minus_64);
#else
   const uint64_t jflone = UINT64_C( 0x3ff0000000000000);
   const uint64_t jflmsk = UINT64_C( 0x000fffffffffffff);
   const uint64_t rval = jflone | (jflmsk & well19937a_64( state));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
   return( *((double *)&rval) - 1.);
#pragma GCC diagnostic pop
#endif
}

#ifdef SLIGHTLY_FASTER
/*   A slight speed improvement can be had by noting that we're    */
/* generating 64 random bits,  then using only 52 of them.  The    */
/* #ifdef SLIGHTLY_FASTER version saves those twelve remaining     */
/* bits;  when 60 have been accumulated (every fifth time),  we    */
/* have accumulated enough to make a new double value,  saving     */
/* a call to our RNG.  This cuts down the RNG workload by 17%,     */
/* but because of the overhead of keeping track of those bits, the */
/* actual speedup is only about 8%.  It doesn't really seem to be  */
/* at all worthwhile,  and is currently #ifdeffed out.             */
/*    Note that this was written before the above get_n_bits( )    */
/* function.  Logically,  we should just use that function to get  */
/* 52 bits.  That would recycle an existing function (saving code) */
/* and further decrease the RNG workload by about 2%.              */

double well19937a_d( uint32_t *state)
{
   const uint64_t jflone = UINT64_C( 0x3ff0000000000000);
   const uint64_t jflmsk = UINT64_C( 0x000fffffffffffff);

   uint64_t rval;
   static uint64_t remains;
   static int bits_avail = 0;

   if( bits_avail >= 52)      /* should happen every 5th call */
      {
      rval = remains & jflmsk;
      bits_avail = 0;
      }
   else
      {
      rval = well19937a_64( state);
      remains = (remains << 12) | (rval & 0xfff);
      rval >>= 12;
      bits_avail += 12;
      }
   rval |= jflone;
   return( *((double *)&rval) - 1.);
}
#endif

double well19937at_d( uint32_t *state)
{
   const uint64_t jflone = UINT64_C( 0x3ff0000000000000);
   const uint64_t jflmsk = UINT64_C( 0x000fffffffffffff);
   const uint64_t rval = jflone | (jflmsk & well19937at_64( state));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
   return( *((double *)&rval) - 1.);
#pragma GCC diagnostic pop
}
