#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define SQRT_TWO_PI 2.506628274631000502415765284811045253006986740609938316299
#define LOG_2       0.693147180559945309417232121458176568075500134360255254120
#define SQRT_LOG_4  1.177410022515474691011569326459699637747385689385820538522

const double b = SQRT_TWO_PI;
const double a = SQRT_LOG_4;
const double phi_a = .5 / SQRT_TWO_PI;

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923

double func( const double x)
{
   return( 2. * exp( -x * x / 2.));
}

int main( const int argc, const char **argv)
{
   unsigned i, n = atoi( argv[1]);

   for( i = 0; i <= n; i++)
      {
      const double s = a / (b-a);    /* = 0.885791344379721116872903480968935 */
      const double x = a + (b - a) * (double)i / (double)n;
      const double f = func( x);
      const double g = 1. + (1. - func( s * (b - x))) * s;
      const double quad = 2.86575 - x * (2.0213 - .3605 * x);

      printf( "%3u: %lf %lf %lf %lf   ", i, quad, f, g, quad + 0.050406);
      printf( ": %lf %lf %lf\n", f - quad, g - f, quad + 0.050406 - g);
      }
   return( 0);
}
