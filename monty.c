#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "mt64.h"

/* Returns Gaussian-distributed random numbers,  using the
Monty Python algorithm (Marsaglia and Tsang,  _ACM Transactions
on Mathematical Software_,  Vol 24, No 3, September 1998,
pages 341-350).  I found it at :

http://www.cparity.com/projects/AcmClassification/samples/292453.pdf
https://dieharder.googlecode.com/svn/trunk/Exclude/monty_python.pdf

   The following mostly just copies the C code provided at the end
of the above paper,  with some changes:

   -- Switched from 32-bit floats to 64-bit floats.

   -- The following uses the 64-bit version of the Mersenne Twister
(MT-64),  rather than George Marsaglia's "multiply with carry"
routine.  I realize that MWC is a really good,  well-tested PRNG.
But MT-64 has been subject to even more testing -- especially for
Monte Carlo applications,  which are my main interest -- and it
produces slightly more "random" numbers (admittedly,  at a small
cost in speed... but PRN generation isn't a bottleneck for my
purposes. Or even,  I'd wager, most MC projects.)  In any case,
the switch to 64-bit floats meant that a 64-bit PRNG was de rigeur.

   -- I explicitly named the "mystery constants";  e.g,
"SQRT_TWO_PI" instead of 2.506628.  This also allowed me to
provide double-precision constants instead of the single-precision
ones given in the paper (with enough digits to suffice for when we
go to 128-bit floats someday!  Though revision will be necessary
for 256-bit floats...)

   -- x is left positive until the very end,  when we look at the
least significant bit and flip sign accordingly.

   -- I renamed 'v' to 'quadratic'.  That part of the code is intended
to quickly reject/accept many cases without having to compute a
logarithm,  and really speeds up the code (by a factor of two on my
machine).  However, adding it resulted in changed output if you ran
thousands of numbers.  That shouldn't happen; the only effect of
filtering on the basis of 'quadratic' should be improved speed.  A
little testing showed that the problem was that the constraints given
were a little too tight; you could get quick accept/rejects that
shouldn't have been so quickly accepted or rejected (but you had to run
things a while before hitting such cases).  This is probably either
because of the use of more exact constants,  or perhaps the use of
double precision instead of single.  I wrote a little code to compute
those constants,  changing M&T's 2.8658 (constant term in computing
'quadratic') to 2.86575, and 0.0506 to 0.050406.  Presumably,  some
benefit might accrue from adjusting the linear and quadratic
coefficients as well... but I'll bet we'd be talking about a 0.0001%
improvement in performance;  the real reason I tweaked the constant
coeffs was to ensure correct results,  not to improve performance.  */

#define MAX_RANDOM UINT64_MAX
#define SQRT_TWO_PI 2.506628274631000502415765284811045253006986740609938316299
#define LOG_2       0.693147180559945309417232121458176568075500134360255254120
#define SQRT_LOG_4  1.177410022515474691011569326459699637747385689385820538522
#define LOG_TWO_S   0.571873321490287788037338249589259207921774650944520110838
/* for reference: s=0.885791344379721116872903480968935327028394513567170138597
*/

double gaussian_random( uint64_t *mt_state);

double gaussian_random( uint64_t *mt_state)
{
   const double b = SQRT_TWO_PI;
   const double a = SQRT_LOG_4;   /* phi(a) = .5 / b */
   const double s = a / (b-a);    /* = 0.885791344379721116872903480968935 */
   const double random_scale = (double)MAX_RANDOM + 1.;
   const uint64_t random_value = mt64( mt_state);
#ifdef _MSC_VER    /* MSVC isn't good about uint64_t to double conversions */
   double x = (double)( (int64_t)( random_value >> 8)) * (256. * b) / random_scale;
#else
   double x = (double)random_value * b / random_scale;
#endif

   if( x > a)     /* occurs with probability 1-a/b = 53% */
      {
      double y = mt64_double( mt_state);
      const double quadratic = 2.86575 - x * (2.0213 - .3605 * x);

      if( y > quadratic)
         {
         if( y > quadratic + 0.050406)  /* safely in the upper right region: */
            x = s * (b - x);          /* rotate and scale into upper cap   */
         else if( log( y) > LOG_2 - x * x / 2.)
            {
            x = s * (b - x);
            if( log( 1. + s - y) > LOG_TWO_S - x * x / 2.)
               {
               do          /* find x in the tails */
                  {        /* we have to do this about 1.2% of the time. */
                  x = -log( 1. - mt64_double( mt_state)) / b;
                  y = -log( 1. - mt64_double( mt_state));
                  }
               while( y + y < x * x);
               x += b;
               }
            }
         }
      }
   return( random_value & 1 ? x : -x);
}

#include <stdio.h>
#include <time.h>

int main( const int argc, const char **argv)
{
   uint64_t mt_state[MT_STATE_SIZE];
   int i, n = (argc >= 2 ? atoi( argv[1]) : 60);

   init_mt64( 3, mt_state);
   if( n > 100000)         /* testing for speed */
      {
      clock_t t0 = clock( );
      double sum = 0.;

      while( n--)
         sum += gaussian_random( mt_state);
      t0 = clock( ) - t0;
      printf( "%f seconds elapsed\n", (double)t0 / (double)CLOCKS_PER_SEC);
      printf( "Sum = %.11f\n", sum);
      n = 60;     /* now show sixty values */
      }
   for( i = 0; i < n; i++)
      printf( "%+20.15f%s", gaussian_random( mt_state),
               (i % 3 == 2 || i == n - 1 ? "\n" : " "));
   return( 0);
}

/* Sample output from './monty 100000000' (that's "a hundred million",
so 6.38 seconds means I got about 16 million Gaussian deviates per
second.)  With the quadratic tests removed,  I got the same result,
except that it took 13.6 seconds.

6.380000 seconds elapsed
Sum = -3717.88811932500
  -0.330540261784166   -0.415681997570423   +0.807406797341604
  -0.736333968494029   +0.996205279333591   -1.451237196601617
  -0.764591510525268   -0.453795740736628   -1.464367435511462
  +0.319301140059287   +0.975139030349232   +1.148026043474682
  +0.409033410779756   -0.813689366962272   +0.610361094160448
  +1.390390121562538   -0.749475138520735   -0.815904628871601
  +0.504688613784047   +0.697927344080394   -1.866463359043295
  +1.566425623668398   -1.349937216972288   +0.663407304154426
  +2.397899132561670   -0.415066756577601   -1.616117449769316
  -0.268837424323505   -0.041937048758938   -1.489543137115773
  -0.120405385113855   +0.592475349723788   -0.893237075247945
  -0.135217888415135   -1.172108234455719   +0.250356638611022
  -0.115354814189050   +1.562119144841063   -0.869156623358561
  -0.031097704384295   -0.413591289565795   +1.788265701277386
  +0.003248158839398   +1.596986111544246   +0.840459253637472
  -1.851937652391193   -0.908507448957161   +1.118033972389155
  -1.668206218661314   -1.322629603754597   +0.254109498016942
  -0.922686738466723   -0.812915963456494   +0.029634460606542
  +0.107980931573167   -1.061800702753364   -0.443707334021938
  +0.947073008672029   -0.434789457816677   +0.918233783666353
*/
