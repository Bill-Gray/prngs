#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923
#define SQRT_2 1.4142135623730950488016887242096980785696718753769480731766797379907325

/* "Gaussian" is doubled so that the integral from zero to infinity
   is one,  instead of 1/2.                                          */

static double gaussian( const double x)
{
   return( 2. * exp( -x * x / 2.) / sqrt( 2. * PI));
}

static double area_within_cap( const double x)
{
   return( erf( x / SQRT_2)  - x * gaussian( x));
}

/* Figure out the ziggurat divisions.  Given n divisions,  we need the
(x, y) points along the Gaussian curve such that 1/n,  2/n,  ... (n-1)/n
of the area is _above_ the line (-x, y) to (x, y).  That is to say,  we
are slicing the area under the Gaussian curve horizontally,  into n equal
areas.  (Hence the above 'area_within_cap'(). )  We do this using a binary
search.  (It's cheap,  easy,  and we only have to do it once.)  A little
experiment showed that a starting increment of 1,  i.e.,  an assumption
that x[i] - x[i-1] < 2 in all cases,  is correct for all n. */

static void fill_ziggurat_indices( double *x, double *y, const unsigned n)
{
   unsigned i;

   x[0] = 0.;
   for( i = 1; i < n; i++)
      {
      const double fraction = (double)i / (double)n;
      double increment = 1.;

      x[i] = x[i - 1];
      while( increment > 1e-20)
         {
         const double new_x = x[i] + increment;

         if( area_within_cap( new_x) < fraction)
            x[i] = new_x;
         increment /= 2.;
         }
      }
   for( i = 0; i < n; i++)
      y[i] = gaussian( x[i]);
}

int main( const int argc, const char **argv)
{
   if( argc == 2)
      {
      const double x = atof( argv[1]);
      const double y = gaussian( x);

      printf( "%.10f\n", area_within_cap( atof( argv[1])));
      printf( "x = %.10f; y = %.10f; box area = %.10f\n",
               x, y, x * y);
      printf( "Area in tail: %.10f\n", 1. - erf( x / SQRT_2));
      }
   else
      {
      double x[1024], y[1024];
      const unsigned n = atoi( argv[1]);
      unsigned i;
      double probability_failure = 0.;

      fill_ziggurat_indices( x, y, n);
      for( i = 0; i < n / 4; i++)
         printf( "%3u %.10f   %3u %.10f   %3u %.10f   %3u %.10f\n",
                  i, x[i],       i + n / 4, x[i + n / 4],
                  i + n / 2, x[i + n / 2],
                  i + 3 * n / 4, x[i + 3 * n / 4]);
      for( i = 1; i < n; i++)
         probability_failure += 1. - x[i - 1] / x[i];
      printf( "Computing more than one PRN will be required %.2f%% of the time\n",
                  100. * probability_failure / (double)n);
      }
   return( 0);
}
