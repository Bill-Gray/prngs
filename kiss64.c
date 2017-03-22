/* SUPRKISS64.c, period 5*2^1320480*(2^64-1) */
/* By George Marsaglia,  from http://forums.silverfrost.com/viewtopic.php?t=1480 */
/* Modified to use stdint.h and a few other minor modifications */

#include <stdio.h>
#include <stdint.h>
static uint64_t Q[20635],carry=36243678541LL,
xcng=12367890123456LL,xs=521288629546311LL,indx=20632;

#define CNG ( xcng=6906969069LL*xcng+123 )
#define XS ( xs^=xs<<13,xs^=xs>>17,xs^=xs<<43 )
#define SUPR ( indx<20632 ? Q[indx++] : refill() )
#define KISS SUPR+CNG+XS

uint64_t refill( )
{
   unsigned i;
   uint64_t z, h;

   for( i = 0; i <20635; i++)
      {
      h = (carry & 1);
      z = ((Q[i]<<41)>>1) + ((Q[i]<<39)>>1) + (carry>>1);
      carry = (Q[i]>>23) + (Q[i]>>25) + (z>>63);
      Q[i] = ~((z<<1)+h);
      }
   indx=1;
   return (Q[0]);
}

int main( const int argc, const char **argv)
{
   unsigned i;
   uint64_t x;

   printf( "sizeof( uint64_t) = %zd\n", sizeof( uint64_t));
   for( i = 0; i < 20632; i++)
      Q[i]=CNG+XS;
   for( i = 0; i < 1000000000; i++)
      x=KISS;
   printf( "Does x=-5061657174865032461\n x=%ld.\n",x);
   return( 0);
}
