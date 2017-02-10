/* ***************************************************************************** */
/* Copyright:      Francois Panneton and Pierre L'Ecuyer, University of Montreal */
/*                 Makoto Matsumoto, Hiroshima University                        */
/* Notice:         This code can be used freely for personal, academic,          */
/*                 or non-commercial purposes. For commercial purposes,          */
/*                 please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca       */
/* ***************************************************************************** */

#include <stdint.h>

#define R 16

static uint32_t state_i = 0;
static uint32_t STATE[R];

void InitWELLRNG512a( const uint32_t *init);
uint32_t WELLRNG512a( void);

void InitWELLRNG512a( const uint32_t *init){
   int j;
   state_i = 0;
   for (j = 0; j < R; j++)
     STATE[j] = init[j];
}

uint32_t WELLRNG512a (void){
  uint32_t z1, z2, tval;
  uint32_t a, c;

  a = STATE[state_i];
  c = STATE[(state_i + 13) & 0xf];
  z1    = a ^ (a << 16) ^ c ^ (c << 15);
  a = STATE[(state_i + 9) & 0xf];
  z2 = a ^ (a >> 11);
  tval = STATE[state_i] = z1 ^ z2;
  state_i = (state_i + 15) & 0xfU;
  STATE[state_i] ^= (STATE[state_i] << 2) ^ (z1 << 18) ^ z2
                       ^ (z2 << 28) ^ ((tval << 5) & 0xda442d24U);
  return (STATE[state_i]);
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( const int argc, const char **argv)
{
   int delays = (argc == 1 ? 0 : atoi( argv[1]));
   int i, n_to_show = 160;
   uint32_t init[16] = {
      3141592653u, 589793238u, 462643383u, 2795028841u, 971693993u, 751058209u,
      749445923u, 781640628u, 620899862u, 803482534u, 2117067982u, 1480865132u,
      823066470u, 938446095u, 505822317u, 2535940812u };

   InitWELLRNG512a( init);

   while( delays--)
      WELLRNG512a( );
   printf( "%.2f seconds elapsed\n",
                  (double)clock( ) / (double)CLOCKS_PER_SEC);
   for( i = 0; i < n_to_show; i++)
      printf( "%08x%s", WELLRNG512a( ),
                  (i % 8 == 7 ? "\n" : " "));
   return( 0);
}

/* Output (with zero delays) should be:

0.00 seconds elapsed
295732ff 479c6a8a 331e895b 4f8f1ca9 33a9405a 240d9004 703cbf6f 71aa84b5
b9be8a42 e07d4269 43c98c07 97fae94e 963fef44 ced6ebd0 1fcd1ee1 8bec1508
c8ced044 312aab1f aaa46a34 db213327 73f035b0 c02c39d8 fc0cc9ba 4716020b
96e7270e f652bd19 e0a505a9 d266fa44 e7cde7d0 6c755150 524a69b7 2c2c3fb7
1c8a5b5b 885561a7 7830fa69 ec7cc104 fa9bcc9c 828f7341 05d4881b 31a7216e
1bed432b 6d8077ba 1d150744 29e66b2d 5935c58a e55ef800 6dd69052 da4b6d63
415df2c5 e9f832bb 694dbbf9 124f13ee ac3b4fb1 bdb1570a 5b05bab2 ecd0f25c
add7b746 0a1c2038 147c21b7 d85e6218 7b88e814 64dfde20 49439e3c 7b778d33
acda889d 0f2dce31 f5f083ad 2e425868 cb58eeea 01db313f 0b3b44d5 3c636aeb
2868b81b c7077cc0 a325734b 4d2e0176 002609bc 6c5132ea c6c122d7 4921e9c2
8f02974d f51eedd2 e29492a0 03d015b2 ffd8503b baddc0b1 30a46796 9edfc4e7
b115826b e9ae771e a943a0bd 09d79d24 6a96ee9c 59df851d cd1998f0 60a3bfb9
5095117e 5f69e935 0b502e7d 75e140b2 acfbd40f f435553d 637e69b8 39eb233a
2852dc0f 69778f78 5cacbaa4 c64bad17 61b67c34 9f2732ee cdf18c95 8458f482
e69434dd 48863c33 5a6d2e1d 6613698d 7d35d629 71bb1f0d 59b50a83 7d45758d
3e381ae8 68b8d51e 1a38c5d2 6b00dfe3 c0cc1c3a 5a3e256f 901c7e2a 1c1e47d8
c0b67a26 65f9cd88 e7ed1bb8 f4704a70 4e82fd80 370769c6 6a62c2f4 92b3bdf1
c3d82ffb ce8ead86 407cfdcf 646f84a9 d1dd87f8 df625df1 e0180998 09048453
048c4f37 5e62f642 8660a494 41b7a757 76d032ca 5e5b8340 55e75f07 d20bd33c
e9233222 734d61c9 b68c3fa5 b6b8cc18 d4653ff8 c7e2ed69 2ed406e7 f4b19dd4

...and with a billion delays :

9.42 seconds elapsed
15176074 a63e57d4 ae71abf5 8367c6d1 e6265c35 6a3da436 d4ac017b dfda85d2
043f314d 11444621 3fb9731e 73d3cd4d 59c21259 55c65905 771b2b1b 96857937
d049976c 64816d9a 64cd56f4 cb7fdc85 8950d8f3 9943105e c4984c97 89c5ddc3
d6bc1dc6 8d136c9d 96ed25bf 1490b7af a77d480d cccf34dc 07320c1b 947c44d9
e77533be 964f022b 60246b88 5da459dd a6faf0bf 6b0f7c9b 071e5baa b5303950
785d4c5e 2a74776c 02379ebb fc6e2211 13cf2552 8b03f661 ddc326ac 53229090
c3658ee9 9731409b ca356eb2 516df8fe 74c4d2da 022199cd e5a5fb50 aae8b95b
c158bff8 b89fac73 317d874a 0359a933 758e53b1 d31d116c a0b793a8 87803c83
7170b834 57cecded eb020f5f b11d827a e4df6681 4d116b29 a41ce122 8b553fb9
52c125a2 499c86d9 e7af3e7b e8f8b87e 18ae7841 6f83680d 75002a0a 82f24c78
1c4047da 77ac88a4 6a75afc0 82da5670 6d76f36b a228905c 47aece4b acf2042b
d21483fe ee5a630e cdd1f9a8 4b935cca 684805f2 6089837c 8c86bcb8 676fb5d1
bff0abac ee5862cf 9c13c1a0 073fed3c f9ff1378 274c7281 05dc4be3 a30246eb
34b5a092 6f96d22f c726eaa8 10218ed0 5b5d0f87 062aac0f 31935514 4fe4cc34
40ecfb2b 73996759 86175e65 df231a59 b7e78e83 952bfc44 47caf23f 3fd5a94a
a7808ccc 0571a69c cb838001 9091db4b 1d2be5cc 0d4aa726 9b3a8757 bebf515f
b1200693 bf11d3a9 7fbcbbf0 ed7425fd b09d5b41 a54e76ed d64cc5bc 49e24f23
19de5fe1 1b7c7370 802004c3 35f30966 30584999 46f33699 1ebb637e 7f4e9115
77f2118a 6e3aa43b 8906392e 8447aaac 876201b2 6cbca46c 0e045117 e3925507
d9d23af0 ff1eecd3 56e330d7 bcca1014 aa64dd4a 9edea3bb 491207e4 f94e5078
*/
