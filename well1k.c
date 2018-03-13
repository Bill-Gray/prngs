#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "well1024.h"

int main( const int argc, const char **argv)
{
   int delays = 0;
   int i, n_to_show = 160, n_doubles_to_show = 80;
   uint32_t state[WELL1024_STATE_ARRAY_SIZE];
   clock_t t0;

   init_well1024( state, 0);
   for( i = 1; i < argc; i++)
      if( argv[i][0] == '-')
         switch( argv[i][1])
            {
            case 'd': case 'D':
               delays = atoi( argv[i] + 2);
               if( strchr( argv[i], 'M'))
                  delays *= 1000000;
               break;
            case 'n': case 'N':
               n_to_show = atoi( argv[i] + 2);
               break;
            case 'f': case 'F':
               n_doubles_to_show = atoi( argv[i] + 2);
               break;
            case 'a': case 'A':
               init_well1024( state, atoi( argv[i] + 2));
               break;
            case 'i': case 'I':
               memset( state, 0, sizeof( state));
               state[0] = (uint32_t)atoi( argv[i] + 2);
               break;
            }

   if( delays)
      {
      printf( "%d delays\n", delays);
      t0 = clock( );
      while( delays--)
         wellrng1024( state);
      t0 = clock( ) - t0;
      printf( "%.2f seconds elapsed\n",
                  (double)t0 / (double)CLOCKS_PER_SEC);
      }
   for( i = 0; i < n_to_show; i++)
      printf( "%08x%s", wellrng1024( state),
                  (i % 8 == 7 ? "\n" : " "));
   for( i = 1; i <= n_doubles_to_show; i++)
      printf( "%.17f%s", wellrng1024_d( state),
               (i & 3) ? " " : "\n");
   for( i = 1; i <= n_doubles_to_show; i++)
      printf( "%.20Lf%s", wellrng1024_ld( state),
               (i & 3) ? " " : "\n");
   return( 0);
}


/* Sample output:

100000000 delays
2.53 seconds elapsed
3b967be7 80da1b05 1143c373 eeaee265 e02c7b9c ade4af57 72f763e2 2f40b23c
4a26d2f9 01436d46 0f437f39 69136d1d 4d41f8bb fa0cdf99 944146db 03ad2c67
7f314563 fdc17866 4594b430 9e12f9b1 b3c6c069 132902de d0ea91c5 27e1f653
833dcf3b 31620dfc d029c2f4 43e46a8b 1b4658ab fb731ad4 e2b25d50 cc9b0c45
5c3bcaa3 fe8c689d 13c7c7b3 a48d5959 593c0f53 9e55582e 99f04f63 d16e9564
f87d4af8 7035c8da da905640 ffc23aa5 48caaf14 875eead0 ba876de8 262d38af
fb8d1171 7dc78485 ce70719d f84b95d5 c3ec152e 463a2d40 45544356 a8771a60
7be0cd1b d5d75db2 eb60b477 a63cbc1d 4607347c 25b84fdf 12bf9c55 2b78dc36
ee6b3afa 825699c6 128e9bd2 1af2a2b2 9d315eb7 7b9c362e 6714f937 34c2e790
7c919136 dc13c421 7ce6ba27 39c6096d 445c6cc5 eb5e7038 ce16494c 2138e347
c72182f8 7cfd9e27 b9ff96db 68179fb2 7a6aad39 1ecda354 dcecdcc3 36e5d74e
c6e0b9e5 553a52e9 c11de47e 6d0924f8 2a02e470 01272e1d f963d166 aa1ed727
4b283561 b3433b6d 1ed7fac2 d8872f90 7bac2588 b0350e30 92ccb16c 6af05320
d6cf0b86 b1f40344 9456207c 89566c62 f809ea03 a565bff1 1228e66d a278c80b
a41c9733 ab56a519 0dd63239 34a5dc11 b3fc4a1a 060139f6 0eab9eb4 25796a26
bfdde45c e35c2f52 9b72a913 328abbd9 9cf72fbb 98444a90 d19403cd 0df9985f
3dc0f14c 6f404d27 ac579c3e e09a5936 a2c88658 5abb3485 976c12b8 1d237109
0c456288 76a28541 3b63bec8 147c3fb5 d69d51a7 12163edb 37a6387f abeed12f
7405cb37 7a350ce3 61d43643 460cc015 ddaa476b 4815a7a0 26238367 ca9985e9
e4ff42e8 30a38622 14791f01 1dddd8a4 e1a65800 027e549d b1b91d48 7d315091
0.60688430137317062 0.54809571999626039 0.05823321699958983 0.73232845921503831
0.38116986116219165 0.01602086102123512 0.68472451029958603 0.52915945746326187
0.60397614511628905 0.93216358627750306 0.78105120836342645 0.39414669207828745
0.24564589289204286 0.45089118987472654 0.16535938102137759 0.11529328315330134
0.03320071781574741 0.52282745005712083 0.02327950605361728 0.44129156884784737
0.51894352119000398 0.69518463574328782 0.14002914591627524 0.02156010933924968
0.55211865882567968 0.59465649930284803 0.50474470887135792 0.06054507476001314
0.00397270662908267 0.03353456303413394 0.05475252750200399 0.91254708167430909
0.30518581071557849 0.93699496675165372 0.21251863678791505 0.48575326388001483
0.87102197053231478 0.32469686887949334 0.52752322259177653 0.53632006704452118
0.07744574199979959 0.53770564757006745 0.42767071264239021 0.90706541764009407
0.10484725705493414 0.18894406026313781 0.18215393160984350 0.49621206712419275
0.20648460205142150 0.66063699886815752 0.79195587989390370 0.85117913702897408
0.46691396910750105 0.96498939757642965 0.65927870914637943 0.03905648160554587
0.31751981980328181 0.17304000611920456 0.13642789550169954 0.76720051268509293
0.72856028148777585 0.57375735204165856 0.08562134705758995 0.00132190603334559
0.02560086715134591 0.02366542382042658 0.49624017052999059 0.01684569533277514
0.24128293769468634 0.69115007906538661 0.28998630722242891 0.11721322741620388
0.51638234230765989 0.81311129153805695 0.84750179125040392 0.06730363648700100
0.38098749839635970 0.71801972307149819 0.94624231212534116 0.57434163807364169
0.61772378319567584578 0.49345880006170797017 0.73627689673532573632 0.89140120298886791055
0.85625404935809433992 0.62930044415365228171 0.20207627086796624836 0.58888903492026952401
0.01594848403120083008 0.10392764466827432120 0.24085384047170548232 0.02551492838599501794
0.64141450876436341562 0.74981979881134092666 0.09118560601513551740 0.65496559261718273167
0.78551809973597488067 0.97584632616470441875 0.90856381222483388659 0.39717674577146376699
0.29461461098266611750 0.59256478574524171752 0.44062674356663439443 0.53747073471682761030
0.05995675851396874330 0.16037328696853099693 0.58449295834006727702 0.89131173341756148596
0.94537311914349356180 0.21956501540642352687 0.01026898777145252104 0.31581784731079306388
0.73484647884728553449 0.82244039383778115809 0.39749659099196355168 0.78976881342534906454
0.41089970929076180539 0.06549933478455961572 0.17564817066881429177 0.41796195166805782712
0.51520171296734167067 0.98519440222175292093 0.07845692413874695722 0.51185542399124032769
0.93350973769366237182 0.33568685166512215675 0.42976401618334332728 0.42093570693504454524
0.56864430713111474656 0.24068640078706236970 0.41702156913081460464 0.07812188321964748047
0.42345735780655892658 0.43298125316928685478 0.79825998300998944352 0.13515455278034951771
0.95318268130076194689 0.82808083879179143012 0.09942672443777607132 0.46369069672232897568
0.70240774897434169605 0.75374222695224650428 0.73479421779130297610 0.53171419431510859191
0.71233111412315735676 0.76362968906986527899 0.51455760913245337195 0.63191128773333078803
0.53541922575290291605 0.51938874030946595233 0.90811484126229912654 0.58027316784750322285
0.35666124140358239608 0.51511349816667306029 0.35503409978550610332 0.37051094822185468144
0.53690991381680621298 0.30565251387012020898 0.15822302428458248625 0.94448399161878566631
*/
