#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include "isaac64.h"

int main( const int argc, const char **argv)
{
  int i;
  int count = (argc > 1 ? atoi( argv[1]) : 0);

  isaac64_init( 1);
  if( count)
     {
     printf( "%d iterations: ", count);
     while( count--)
        isaac64( );
     printf( "%.2f seconds\n", (double)clock( ) / (double)CLOCKS_PER_SEC);
     }
  for( i = 0; i < 80; i++)
     {
     const uint64_t r = isaac64( );

//   printf("%.8x %.8x ",(uint32_t)(r >> 32), (uint32_t)r);
     printf( "%016" PRIx64 " ", r);
     if( (i&3) == 3)
         printf( "\n");
     }

  return( 0);
}

/* Sample output (compiled with gcc on my Xubuntu box) :

./test64 1000000000
1000000000 iterations: 8.67 seconds
2f9193df8106bfe0 625b8f809e33f89e 4265370380e4f01b 005f9d1854e814d5
f1608958ff837dca b4aee19ed981aba2 5fdc904a397950bd e6357a991c7b044a
0f66456edc07dc90 4d31f452565fa301 ce6d8f387ceb480f 1b0812e2a40650d2
9644d83bd2ac0917 83e42188c1460eb3 89f831b7ff5bce57 b5b6d3943ef15dab
4cda4563d31f0f4f 5b381fb607fda414 b7de23195fcf6f69 4950bcfbdca6905c
01c0e94d9af164bb 56d67f61791d6fe0 2f2b107935c83301 fd44737e23171dc6
9351ab40f2664b18 b109ffb455cd88e1 9e552d92ce3b1352 2b03b685463e1284
345e8fc6e74766cc 94fbd76863ef730e 57c0da6acd76c007 f4140e1572b13431
536f2fbc1479bb21 1b2371b2579ea189 96e34cdf56263a1e e2e0ab011d7dfd08
26c45fdf4a702943 21ead46166535937 31afc2e74965c32e 9db16a3fbd0d65bd
b1d355eaab2227ea a497607602578bad bf93b429c997f462 74510da0df8f5386
d422a169878e9d58 7ff64c3a352b6dd0 02e9f6b505dc6a60 c80f82099e927b8f
501207307204df9f 75c835bbd2da4796 3c6a29734c6c9eaa 12d24ce201c2af1b
2cd255389ec67da6 ad5c57dc40a9909c c078c41d761690f6 caedb64608a965af
4313c9270954617c c78c42e2d2faf3ba f5ab9973cf726f2c 06cb17356ea45598
d923fe2b508d6298 bf95a9bbf6ded87c 56107ff43ed15e4b 1b5609cd2802be2a
f114e6e67754291a 96b7c864e20d6dde 0fd415e1bcc7fce3 cfe62b197180128e
fb5edbdbeac3eaf1 6416328e67b667c1 4f7da0d93f863875 5ea01eae7bdc0776
a64b216d623660f9 ed75a4a3027af785 e25b9669751a0a05 3675e9a7d7568aeb
d66ad39b1b7ad4fa 02561be88a82235b 003c08373ba44364 b503ec49113700cb
*/
