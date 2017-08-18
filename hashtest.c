#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <search.h>
#include <time.h>
#include <math.h>    /* for sqrt() */

/* Test code for 'hash.c' (q.v.) */

uint64_t SuperFastHash64( const char * data, size_t len);

int shift_amt = 0;

static uint32_t SuperFastHash (const char * data, size_t len)
{
   return( (uint32_t)( SuperFastHash64( data, len) >> shift_amt));
}

struct hashword {
   uint32_t hash;
   char *text;
   };

#define MAX_WORDS 700000

static unsigned count_bits( uint32_t ival)
{
   unsigned rval = 0;

   while( ival)
      {
      if( ival & 1)
         rval++;
      ival >>= 1;
      }
   return( rval);
}

static const char *comment_text[] = {
   "Shown at left are data about the number of",
   "bits flipped in the hash output when bit N",
   "of a 32-bit input is flipped.  The mean",
   "number of bits flipped,  sigma,  and max",
   "and min (as measured on 150000 pseudo-",
   "random values with bit N flipped) is shown.",
   "Ideally, flipping a given input bit should",
   "flip half the output bits on average.",
   "", "", "", "", "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "", "", "", "", "" };

static void test_bit_flipping( const unsigned bit)
{
   unsigned i, total_bits = 0, total_bits2 = 0;
   unsigned most_flipped = 0, least_flipped = 999;
   double avg_bits, avg_bits2;
   const unsigned count = 2500000;

   for( i = 0; i < count; i++)
      {
      const uint32_t ival1 = rand( );
      const uint32_t ival2 = ival1 ^ ((uint32_t)1 << bit);
      const uint32_t hash1 = SuperFastHash( (const char *)&ival1, sizeof( uint32_t));
      const uint32_t hash2 = SuperFastHash( (const char *)&ival2, sizeof( uint32_t));
      const unsigned bits_flipped = count_bits( hash1 ^ hash2);

      total_bits += bits_flipped;
      total_bits2 += bits_flipped * bits_flipped;
      if( most_flipped < bits_flipped)
         most_flipped = bits_flipped;
      if( least_flipped > bits_flipped)
         least_flipped = bits_flipped;
      }
   avg_bits = (double)total_bits / (double)count;
   avg_bits2 = (double)total_bits2 / (double)count;
   printf( "%2u: %7.4f +/- %.4f (%2u to %2u)  %s\n", bit, avg_bits,
                                   sqrt( avg_bits2 - avg_bits * avg_bits),
                                   least_flipped, most_flipped,
                                   comment_text[bit]);
}


int hashword_compare( const void *a, const void *b)
{
   uint32_t hash_a = ((const struct hashword *)a)->hash;
   uint32_t hash_b = ((const struct hashword *)b)->hash;

   return( hash_a > hash_b ? 1 : -1);
}

int main( const int argc, const char **argv)
{
   unsigned i, n_delays = 0;
   clock_t t0;
   FILE *ifile = fopen( "american-english-insane", "rb");

   for( i = 1; i < (unsigned)argc; i++)
      if( argv[i][0] == '-')
         switch( argv[i][1])
            {
            case 's':
               shift_amt = atoi( argv[i] + 2);
               break;
            case 'd':
               n_delays = (unsigned)atoi( argv[i] + 2);
               break;
            }

                  /* Show hashes of 160 integers,  basically just so we  */
                  /* can eyeball them and say "yeah,  they look random". */
                  /* Not a very good test,  I realize!                   */
   for( i = 0; i < 160; i++)
      printf( "%08x%s", SuperFastHash( (const char *)&i, sizeof( uint32_t)),
               (i & 7) == 7 ? "\n" : " ");

   if( ifile)
      {
      char tbuff[100];
      struct hashword *words = (struct hashword *)calloc( MAX_WORDS, sizeof( struct hashword));
      unsigned n_words = 0, n_collisions = 0;

      while( fgets( tbuff, sizeof( tbuff), ifile))
         {
         for( i = 0; (unsigned)tbuff[i] >= ' '; i++)
            ;
         tbuff[i] = '\0';
         words[n_words].hash = SuperFastHash( tbuff, i);
         words[n_words].text = (char *)malloc( i + 1);
         strcpy( words[n_words].text, tbuff);
         n_words++;
         }
      fclose( ifile);
      printf( "%u words read\n", n_words);
      qsort( words, n_words, sizeof( struct hashword), hashword_compare);
      for( i = 1; i < n_words; i++)
         if( words[i].hash == words[i - 1].hash)
             {
             n_collisions++;
             printf( "%08x: \"%s\" \"%s\"\n", words[i].hash, words[i - 1].text, words[i].text);
             }
      printf( "%u collisions found\n", n_collisions);
      printf( "Roughly %.0f collisions expected if the hash were 'perfect'\n",
                  (double)n_words * (double)n_words / (2. * 65536. * 65536.));
      }

   t0 = clock( );
   for( i = 0; i < 32; i++)
      test_bit_flipping( i);
   t0 = clock( ) - t0;
   printf( "%.3f seconds\n", (double)t0 / (double)CLOCKS_PER_SEC);
   if( n_delays)
      {
      t0 = clock( );
      for( i = n_delays; i; i--)
         SuperFastHash( (const char *)&i, sizeof( i));
      t0 = clock( ) - t0;
      printf( "%u hashes in %.3f seconds\n", n_delays,
                    (double)t0 / (double)CLOCKS_PER_SEC);
      }
   return( 0);
}

/* Example output (you should get the same thing,  except probably
a different time at the end)

00000000 bbc9aeec 77935dd9 335d0cc6 ef26bbb3 aaf06aa0 66ba198d 2283c87a
de4d7767 9a172654 55e0d541 11aa842e cd74331b 893de208 450790f5 00d13fe1
bc9aeece 78649dbb 342e4ca8 eff7fb95 abc1aa82 678b596f 2355085c df1eb749
9ae86636 56b21523 127bc410 ce4572fd 8a0f21ea 45d8d0d7 01a27fc3 bd6c2eb0
7935dd9d 34ff8c8a f0c93b77 ac92ea64 685c9951 2426483e dfeff72b 9bb9a618
57835505 134d03f2 cf16b2df 8ae061cc 46aa10b9 0273bfa5 be3d6e92 7a071d7f
35d0cc6c f19a7b59 ad642a46 692dd933 24f78820 e0c1370d 9c8ae5fa 585494e7
141e43d4 cfe7f2c1 8bb1a1ae 477b509b 0344ff87 bf0eae74 7ad85d61 36a20c4e
f26bbb3b ae356a28 69ff1915 25c8c802 e19276ef 9d5c25dc 5925d4c9 14ef83b6
d0b932a3 8c82e190 484c907c 04163f69 bfdfee56 7ba99d43 37734c30 f33cfb1d
af06aa0a 6ad058f7 269a07e4 e263b6d1 9e2d65be 59f714ab 15c0c398 d18a7285
8d542172 491dd05e 04e77f4b c0b12e38 7c7add25 38448c12 f40e3aff afd7e9ec
6ba198d9 276b47c6 e334f6b3 9efea5a0 5ac8548d 1692037a d25bb267 8e256154
49ef1040 05b8bf2d c1826e1a 7d4c1d07 3915cbf4 f4df7ae1 b0a929ce 6c72d8bb
283c87a8 e4063695 9fcfe582 5b99946f 1763435c d32cf249 8ef6a136 4ac05022
0689ff0f c253adfc 7e1d5ce9 39e70bd6 f5b0bac3 b17a69b0 6d44189d 290dc78a
e4d77677 a0a12564 5c6ad451 1834833e d3fe322b 8fc7e117 4b919004 075b3ef1
c324edde 7eee9ccb 3ab84bb8 f681faa5 b24ba992 6e15587f 29df076c e5a8b659
a1726546 5d3c1433 1905c320 d4cf720d 909920f9 4c62cfe6 082c7ed3 c3f62dc0
7fbfdcad 3b898b9a f7533a87 b31ce974 6ee69861 2ab0474e e679f63b a243a528
 0: 17.3036 +/- 2.3993 (10 to 28)  Shown at left are data about the number of
 1: 16.9066 +/- 2.3261 (11 to 28)  bits flipped in the hash output when bit N
 2: 16.2459 +/- 2.4299 (10 to 28)  of a 32-bit input is flipped.  The mean
 3: 16.6466 +/- 2.4521 (10 to 29)  number of bits flipped,  sigma,  and max
 4: 17.3011 +/- 2.3467 (11 to 29)  and min (as measured on 150000 pseudo-
 5: 16.9441 +/- 2.2959 (11 to 28)  random values with bit N flipped) is shown.
 6: 16.3261 +/- 2.4251 (10 to 29)  Ideally, flipping a given input bit should
 7: 16.8831 +/- 2.4600 (10 to 28)  flip half the output bits on average.
 8: 17.3471 +/- 2.3434 (11 to 29)
 9: 17.5839 +/- 2.3420 (11 to 29)
10: 17.4153 +/- 2.3870 (11 to 29)
11: 17.7586 +/- 2.3177 (11 to 29)
12: 17.8534 +/- 2.2862 (12 to 28)
13: 17.6570 +/- 2.3202 (11 to 29)
14: 18.0519 +/- 2.2830 (12 to 29)
15: 17.5884 +/- 2.3408 (11 to 29)
16: 18.0165 +/- 2.3187 (11 to 30)
17: 17.8828 +/- 2.2607 (12 to 29)
18: 18.1445 +/- 2.2797 (11 to 29)
19: 17.6167 +/- 2.2074 (12 to 28)
20: 16.6988 +/- 2.2650 (11 to 28)
21: 16.5821 +/- 2.1663 (11 to 27)
22: 16.3433 +/- 2.0741 (11 to 27)
23: 15.8843 +/- 2.0441 (11 to 28)
24: 15.1006 +/- 2.1812 (10 to 27)
25: 15.2372 +/- 2.2200 (10 to 30)
26: 15.5231 +/- 2.2820 (10 to 28)
27: 15.7712 +/- 2.2064 (11 to 29)
28: 15.2764 +/- 2.2773 (10 to 29)
29: 15.5144 +/- 2.3076 (10 to 30)
30: 15.4924 +/- 2.2960 (10 to 28)
31: 15.5340 +/- 2.3011 (10 to 28)
0.843 seconds
*/
