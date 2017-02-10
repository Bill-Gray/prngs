#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include "well.h"

/* NOTE:  to actually test the RNG,  make sure that 'n_to_show' is at least
1000.  The default fits nicely on a console;  but 160 values (or 200,
including those generated with the doubles) doesn't reach toward the end of
the 624-long state array used in WELL19937a.

   The -d(number) flag causes that number of PRNs to be generated,  and the
result is timed.  This is what I used to tune the performance of the new code.
*/

int main( const int argc, const char **argv)
{
   int i, n_to_show = 160, n_doubles_to_show = 20;
   int delays = 0, tempering = 0;
   unsigned int data[WELL19937_ARRAY_LENGTH];
   const time_t t0 = time( NULL);

   printf( "Welltest: compiled %s %s; run at %s\n", __DATE__, __TIME__, ctime( &t0));
   init_well19937a( data, 314159265);
   for( i = 1; i < argc; i++)
      if( argv[i][0] == '-')
         switch( argv[i][1])
            {
            case 't':
               tempering = 1;
               printf( "Showing tempered values\n");
               break;
            case 'd':
               delays = atoi( argv[i] + 2);
               if( strchr( argv[i], 'M'))
                  delays *= 1000000;
               printf( "%d delays\n", delays);
               break;
            case 'n':
               n_to_show = atoi( argv[i] + 2);
               printf( "Showing %d values\n", n_to_show);
               break;
            case 'N':
               n_doubles_to_show = atoi( argv[i] + 2);
               n_to_show = 0;
               printf( "Showing %d doubles\n", n_doubles_to_show);
               break;
            case 's':
               init_well19937a( data, (uint32_t)atoi( argv[i] + 2));
               printf( "Reseeding with %s\n", argv[i] + 2);
               break;
            case 'z':
               {
               int j;

               for( j = 0; j < 624; j++)
                  data[j] = 0;
               data[0] = 1;
               printf( "Re-seeded to zeroes\n");
               }
               break;
            default:
               printf( "Option '%s' unrecognized\n", argv[i]);
               break;
            }
   if( tempering)
      while( delays--)
         well19937at_d( data);
      else
         while( delays--)
            well19937a_d( data);
   printf( "%.2f seconds elapsed\n",
                  (double)clock( ) / (double)CLOCKS_PER_SEC);
   for( i = 0; i < n_doubles_to_show; i++)
      printf( "%.15f%s",
             (tempering ? well19937at_d( data) : well19937a_d( data)),
                  (i % 4 == 3 ? "\n" : " "));
   for( i = 0; i < n_to_show; i++)
      printf( "%08x%s",
             (tempering ? well19937at( data) : well19937a( data)),
                  (i % 8 == 7 ? "\n" : " "));
   return( 0);
}

/* When run as 'welltest -t -d100000000',  I got...

Welltest: compiled Jul  5 2011 11:51:12; run at Sat Aug 27 16:22:45 2011

Showing tempered values
100000000 delays
10.00 seconds elapsed
0.056774108674536 0.922087865309603 0.088409918990713 0.036552525806746
0.892380149700999 0.428476422257541 0.902284155750238 0.124893781117170
0.025347938175661 0.118668865388859 0.984790189283809 0.113767393615478
0.400971727690436 0.267819046338807 0.196379813506557 0.920028814869996
0.744771259250915 0.212556333133887 0.866130453632687 0.258522401119536
a6dd8258 9071d9f4 2380c9b3 a43a1896 12f12617 d8ff99af c0add011 362a2189
492dfb1d 9ee0c514 d01f8e68 abde0ec4 4904b4ef d571af03 da0a0f13 d140e295
ccef51a1 3ba808cb f5b47ced 5c0d405f e3329821 93a10585 cc16441b 946e38b5
563e6c58 15437b54 35be8e5f 35a360c5 fa79bd47 84d2b61d 7db8bca0 17f692d1
77628e45 9a22a61b ace49873 8410eb74 1e6a1888 9efb5174 58739f50 e7eb81fb
5c57644b 5a03346e 750d92fa 70d72a23 0ce271b5 bd4ab37e 9baa1a6a df51ba0f
4a4626e7 dff7c73e 5d427be3 79388e47 26222521 e195749f e1a2a649 d0b4d80e
8a4867ee a4b8b7df ad14c2c0 64740bb7 15faaed3 50109d75 f90dd065 27e4b7ab
2e46003a 43979a6d 929724ec cb89dfaa a940ae75 d3907fd2 8aa54d54 5783c62d
03cd31ec fb442a2d b6fba466 5f7f805f c856f33f 097c54a9 0e774723 932f94ec
e11cba6c e7a62ce5 7964ba97 2d6ee4c8 92f1585a e308bf30 b0336e9f d8de6012
27d5c6ba d8de40b7 e534abd6 11b4f317 af6ee25a 12b79797 53664aeb f587b7de
8d4ef037 e431d4b0 6becbc84 81a2aac0 df7aaa27 6b9c0ffe f2307513 d196f343
aea55444 2190809c e2a90de7 d127243b 80b0c2d1 c81438a9 31c9e07b 0b70d63d
1204e731 78c7b657 ab365438 377b1a36 da2c40d6 bd3fd44c fe7a20c3 a10d88f1
b9140e65 39f1e5cb 6c18e259 533e066d 16f31f4f d45b6e0a fae8e694 f631e6b8
f9269b25 89b780d1 12f671d7 fcc48185 6df982ae 3944ef72 d3cca139 033a5daf
89f11e4e 140ce470 ffb41f86 3cd50527 b47732ee ccb4e491 24621b1c 5b0e979e
dfd34c20 c59abce0 30130c05 29c46886 140313fc 6e4bb1e1 2fd83095 de62142e
782f9e87 084c8fae 2d8a8b35 8792a2b5 c86e1fc6 da8f5e7c 265673c8 77ba45e7
*/
