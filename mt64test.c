#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include "mt64.h"

#ifndef UINT64_C
#define UINT64_C(a) (a##ui64)
#endif

int main( const int argc, const char **argv)
{
    int i;
    int delays = (argc > 1 ? atoi( argv[1]) : 0);
    const uint64_t init[4]={ UINT64_C( 0x12345), UINT64_C( 0x23456),
                             UINT64_C( 0x34567), UINT64_C( 0x45678) };
    const uint64_t length=4;
    uint64_t mt[MT_STATE_SIZE], test_value;

    init_mt64_by_array( init, length, mt);
    printf( "1000 outputs of mt64()\n");
    for( i = 0; i < 1000; i++) {
      printf( "%20" PRIx64 " ", mt64( mt));
      if (i%3==2) printf("\n");
    }
    printf("\n1000 outputs of mt64_double()\n");
    for (i=0; i<1000; i++) {
      printf( "%19.17f ", mt64_double( mt));
      if( i % 3 == 2) printf("\n");
    }
    test_value = mt64( mt);
    if( test_value != UINT64_C( 16139946649673210444))
      printf( "FAILURE TO MATCH!: %20" PRIx64 "\n",  test_value);
    else
      printf( "Last value passes test\n");
    if( delays)
       {
       clock_t t0;

       printf( "Computing %d ", delays);
       printf( argc == 2 ? "64-bit randoms\n"
               : "64-bit floating-point randoms\n");
       t0 = clock( );
       if( argc == 2)
          for( i = delays; i; i--)
             mt64( mt);
       else
          for( i = delays; i; i--)
             mt64_double( mt);
       printf( "%.2f seconds elapsed\n",
                  (double)(clock( ) - t0) / (double)CLOCKS_PER_SEC);
       }
    return 0;
}

