/* ***************************************************************************** */
/* Copyright:      Francois Panneton and Pierre L'Ecuyer, University of Montreal */
/*                 Makoto Matsumoto, Hiroshima University                        */
/*             (The code has been heavily modified by me,  Bill J. Gray,  to     */
/*             store the PRNG state in a user-supplied array for re-entrancy;    */
/*             to provide access to 64-bit and n-bit and floating-point PRNs;    */
/*             to get it to compile correctly for 64-bit systems;  and I've      */
/*             added some comments and a 'seeding' function.)                    */
/* Notice:         This code can be used freely for personal, academic,          */
/*                 or non-commercial purposes. For commercial purposes,          */
/*                 please contact P. L'Ecuyer at: lecuyer at iro.UMontreal.ca    */
/* ***************************************************************************** */

#include <stdint.h>
#include "well1024.h"

#ifndef UINT64_C
   #ifdef _MSC_VER
      #define UINT64_C( a) (a##ui64)
   #else
      #define UINT64_C( a) ((uint64_t)(a##UL))
   #endif
#endif

#define R 32

uint32_t wellrng1024( uint32_t *state)
{
  int state_i = state[R];
  const uint32_t vm1 = state[(state_i +  3) & 0x1fu];
  const uint32_t vm2 = state[(state_i + 24) & 0x1fu];
  const uint32_t vm3 = state[(state_i + 10) & 0x1fu];
  const uint32_t z1    = state[state_i] ^ vm1 ^ (vm1 >> 8);
  const uint32_t z2    = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
  const uint32_t tval = z1 ^ z2;

  state[state_i--] = tval;
  state_i &= 0x1fu;
  state[state_i] ^= (state[state_i] << 11) ^ tval ^ (z1 << 7) ^ (z2 << 13) ;
  state[R] = state_i;
  return (state[state_i]);
}

   /* To initialize Well1024,  I simply initialize the state array      */
   /* with the seed value + i (I add 'i' in case seed_value == 0),      */
   /* then call wellrng1024 32 times.   This appears to work,  even     */
   /* for stupid choices of the seed.  In truth,  I don't know of       */
   /* any "approved" initializer.                                       */

void init_well1024( uint32_t *state, const uint32_t seed)
{
   int i;

   for( i = 0; i < R; i++)
      state[i] = seed + i;
   state[R] = 0;
   for( i = 0; i < R; i++)
      wellrng1024( state);
}

/*   Well1024 produces 32-bit values,  of course.  To turn it into a
64-bit PRNG,  we take two 32-bit values and paste 'em together.
NOTE that Francois Panneton has pointed out to me (private communication)
that there is,  at least in theory,  a problem here.  As a 32-bit PRNG,
the WELL algorithms are "well distributed";  i.e.,  it is proven that they
satisfy a very exact criterion in how their values are distributed.  We
lose that property when treating it in the manner shown below.  (At least,
I'm reasonably sure we do.)

   I'm not sure how much to worry about this fact.  I essentially want
a PRNG that can be treated as a source of random bits;  if there are
dependencies such that they're only random if you take 32 of them at
a time,  then it seems to me that it's no longer really "random".  Sort
of like the comment in _Numerical Recipes_ : "We guarantee that each
number is random individually, but we don't guarantee that more than
one of them is random."

   As far as I know,  nobody has actually shown that there is any
discernible pattern or bias in WELL,  so (at least for the nonce) I am
assuming that tricks such as the following will not lead to trouble. */

uint64_t wellrng1024_64( uint32_t *state)
{
   const uint64_t low_bits  = (uint64_t)wellrng1024( state);
   const uint64_t high_bits = (uint64_t)wellrng1024( state);

   return( low_bits | (high_bits << 32));
}

/* Rather frequently,  one may need something other than 32 or 64 random
bits. For example,  a double-precision floating-point value requires
52 random bits.  A simulated coin flip requires one bit.  In such
situations,  the following function may be useful:  it returns a random
integer with a specified number of bits.

   It does this by calling the above wellrng1024_64( ) function to get
64 bits at a time.  Each time this is done,  it gets 64 random bits.
'n_bits' of this are returned;  the rest are stored and can be used
for subsequent calls.

   NOTE two issues with this.  First,  the code is not re-entrant.
(This could conceivably be fixed by extending the 'state' array to
contain the currently static variables.)  Second,  one runs into the
theoretical problems with loss of the "well equidistributed" criterion
that is discussed in the above wellrng1024_64( ) function.           */

uint64_t wellrng1024_bits( uint32_t *state, const int n_bits)
{
   static int bits_stored = 0;
   static uint64_t saved_bits;
   uint64_t rval;

   if( n_bits < bits_stored)
      {
      rval = saved_bits;
      saved_bits >>= n_bits;
      }
   else
      {
      rval = wellrng1024_64( state);
      saved_bits |= (rval >> n_bits) << bits_stored;
      bits_stored += 64;
      }
   bits_stored -= n_bits;
   return( rval & ((UINT64_C( 1) << n_bits) - 1));
}

/* One can take a 64-bit integer,  mask off the upper twelve bits,
OR in a suitable constant,  and get the IEEE equivalent of a
floating-point value from 1 to 2.  Subtract 1 from it,  and you can
get uniform random numbers [0, 1).  A trick grabbed from _Numerical
Recipes in C_,  p. 285,  and updated from the 32-bit world to the
64-bit one.

  The authors of _NR_ note,  correctly,  that this is a really dirty
trick, highly machine-dependent.  Your mother may cry if she learns
you did this.  It won't work on a non-IEEE machine. But I don't know
if those are even made anymore.

  NOTE that it's important to use the 64-bit integer versions of the
above functions.  If you just grab a 32-bit int and scale it to [0,
1),  then 19 bits of the 51-bit precision of a double-precision
floating-point value will be left at zero.

  For comparison,  I tried the 'conventional way' of getting a
64-bit value and dividing by 2^64 (as shown below). This was about
25% slower.  (But is needed on non-IEEE machines,  and conceivably
on some machines with alignment issues.  There is also another
_very_ small advantage: the _Numerical Recipes_ "dirty trick" cannot
generate PRNs smaller than 2^-52.  (And,  in fact,  all results will
be multiples of 2^-52,  even for values where extra precision would
be quite possible.)

   If,  instead,  one does the division,  you'll get full precision
all the way down to 2^-64.  A small benefit,  but I suppose there may
be those who would want it.               */

double wellrng1024_d( uint32_t *state)
{
#ifdef CONVENTIONAL_WAY
   const double two_to_the_minus_64 =
                1. / (65536. * 65536. * 65536. * 65536.);

   return( (double)wellrng1024_64( state) * two_to_the_minus_64);
#else
   const uint64_t jflone = UINT64_C( 0x3ff0000000000000);
   const uint64_t jflmsk = UINT64_C( 0x000fffffffffffff);
   const uint64_t rval = jflone | (jflmsk & wellrng1024_64( state));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
   return( *((double *)&rval) - 1.);
#pragma GCC diagnostic pop
#endif
}

/* Similar trickery can be done with long doubles.  This is even
less portable.  On my Intel(R) machine,  GCC and Clang treat a long
double as a 16-byte entity,  with six bytes left as zeroes.  However,
'long doubles' can be 12 bytes with two bytes of padding,  or ten
bytes with no padding.  Or a 'long double' can just be a double (this
is how Microsoft Visual C++ (TM) handles it).  A long double can just
be two doubles (the "double-double" method);  PowerPC has used this.
Or it can be a 128-bit quad-precision value (GCC's __float128 type).

   Your mother might cry at the above function.  The following code
might cause her to disown you.  You've been warned.      */

long double wellrng1024_ld( uint32_t *state)
{
#ifdef CONVENTIONAL_WAY
   const double two_to_the_minus_64 =
                1. / (65536. * 65536. * 65536. * 65536.);

   return( (double)wellrng1024_64( state) * two_to_the_minus_64);
#else
   const uint64_t mask = UINT64_C( 0x8000000000000000);
   const uint64_t exponent_and_zeroes = UINT64_C( 0x03fff);
   uint64_t array[2];

   array[0] = mask | wellrng1024_64( state);
   array[1] = exponent_and_zeroes;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
   return( *((long double *)array) - 1.L);
#pragma GCC diagnostic pop
#endif
}

