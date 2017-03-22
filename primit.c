#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Code to check the period of a given polynomial,
in hopes of finding primitive ones.  */

int main( const int argc, const char **argv)
{
   unsigned val, mask[2], count = 0;

   sscanf( argv[1], "%x", mask + 1);
   if( argc > 2)
      mask[1] >>= 1;
   mask[0] = 0;
   val = 1;
   do
      {
      count++;
      val = (val >> 1) ^ mask[val & 1];
/*    val = ((val & 1) ? (val ^ mask[1]) : val) >> 1;
      val = (val ^ mask[val & 1]) >> 1;
*/    }
   while( val != 1);
   printf( "Count = %x (%u)\n", count, count);
   return( 0);
}
