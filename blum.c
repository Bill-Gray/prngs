#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

/* 'blum_blum_shub' simply computes x^2 mod pq.  To get a maximum
cycle length,  p and q should both be 2-safe primes.  (If N
and M=2N+1 are both prime,  then M is a "safe prime" and N is
a Sophie Germain prime.  If L=2M+1 is also prime,  then L is
a "2-safe" prime and M will be both a safe prime and a Sophie
Germain prime.  I suppose you could then call N a "2-Sophie
Germain prime" -- one for which N,  M=2N+1,  and L=4N+3 are all
primes -- but I've never seen that terminology.)

   Also,  one or both of p or q should be congruent to 7 modulo 16.
See 'prime.c' for code that searches for such primes.

   Let's call such primes "BBS-safe".  (In theory,  we could make
lists of primes that satify all the criteria,  plus those that
satify all but the p % 16 == 7 criterion.  You could then combine
a "BBS-safe" prime with a "BBS-nearly-safe" -- i.e.,  plain old
"2-safe" -- prime.  However, there are enough truly BBS-safe
primes that I've simply used them.)

   (Note that you can get pretty long cycles with just garden-variety
primes,  and cryptographers often just figure that getting even the
shortest cycles using 512-bit numbers will still be more than long
enough.  There's some discussion of this in "Simple Seed Selection
for BB&S",  at http://www.ciphersbyritter.com/NEWS6/BBS.HTM .  It
provides a prescription which reduces the likelihood of short cycles,
in which both primes have to be 2-safe,  and you can do a quick
check to make sure you aren't in a short cycle.

   However,  the provably maximal period requires that both primes
be 2-safe and that at least one of the two be congruent to 7 modulo
16,  i.e.,  "BBS-safe".  For the proof,  see

http://digital.csic.es/bitstream/10261/8874/1/001090.pdf

   Truthfully,  I didn't really understand the proof.  Just to
make sure I understood the results,  I wrote the 'blumtest.c'
code to check cycle lengths,  and verified that one does indeed
get the maximal cycle length if and only if the conditions are met.

 However,  in what follows, we'll be using 32-bit and 64-bit
numbers.)

   Anyway.  If p and q satisfy the above requirements,  and x is
not congruent to 0, 1, or -1 modulo p or q,  then the cycle length
will be (p-3)(q-3)/8.  If your platform supports 128-bit integers,
then you can fit p and q into 32-bit integers and not have
overflow problems,  and your cycle length will be approximately
2^61.  If your platform only supports 64-bit integers, though,
you can use the following method (call it "factored BBS") to
compute x^2 mod pq without overflowing :

a = x / p, b = x % p    (x = ap + b)
c = x / q, d = x % q    (x = cq + d)
x^2 = adp + bcq + bd    (modulo pq...i.e.,  we can omit acpq)
x^2 = (ad % q) * p + (bc % p) * q + bd

   We know that ad % q, q,  and d are all less than or equal
to q.  We also know that bc % p,  p,  and b are all less than
or equal to p.  So none of the three products done above will
be greater than pq.

   After adding the three 'parts',  there _would_ be a risk
of overflow.  To avoid that,  we need to make sure that
pq < MAX_INT / 3.  Since each of the three parts is less
than pq, adding them will result in something less than
MAX_INT. To get that,  I simply looked for p and q less
than 2^32 / sqrt(3) (or 2^64 / sqrt(3),  depending on the
size of our MAX_INT.)

   Note that if you just use this "as is",  you'll have a cycle
length of pq (slightly less than) 2^61 / 3.  However,  combine
the output from two different pq pairs,  and the cycle length
becomes (almost) 2^122/9,  and so forth.

   If you've access to 128-bit integer support,  you don't have
to use the above tricks to avoid overflow,  and p and q can be
up to 2^32 - 1, leading to a cycle length three times greater.
Or -- for the maximum available without going to 256-bit integers
-- you could use "factored BBS" with p, q < 2^64 / sqrt(3),
getting a cycle length of a little under 2^125 / 3.

   Following are the maximum BBS-safe primes for all three
scenarios,  plus the first few BBS-safe primes just in case you
want to experiment with more manageable cycle lengths.

2-Safe primes (p, (p-1)/2, (p-3)/4 are all prime), p % 16 == 7, are:

   Less than 2^32 / sqrt(3) (use with the "factored BBS" method if
   you don't have 128-bit integer support) :
2476701047 2476724567 2476738247 2476744727 2476753127 2476757399 2476807799
2476873319 2476875959 2476903127 2476953239 2477024519 2477042759 2477149127
2477180999 2477186087 2477196167 2477232119 2477282519 2477290199 2477364887
2477375207 2477400119 2477422919 2477568839 2477579207 2477593799 2477642759
2477644439 2477682839 2477687207 2477715959 2477722487 2477746247 2477787479
2477793239 2477833367 2477837639 2477872487 2477919047 2477937719 2477969927
2478006647 2478019367 2478020087 2478099479 2478169559 2478178679 2478194039
2478212279 2478216647 2478226967 2478272519 2478303239 2478328199 2478334679
2478382967 2478396359 2478405767 2478411527 2478433319 2478445559 2478456887
2478470759 2478487799 2478507719 2478554999 2478565319 2478626519 2478643127
2478650519 2478730967 2478748199 2478779879 2478787079 2478849239 2478860039
2478903767 2478924359 2478934919 2479009607 2479035239 2479071047 2479071527
2479135559 2479158359 2479164167 2479186679 2479231127 2479273367 2479274807
2479288247 2479298807 2479324007 2479338167 2479362407 2479369799 2479374167
2479392887 2479398599 2479421159 2479470167 2479482119 2479540247 2479554167
2479572647 2479578119 2479579559 2479608359 2479637879 2479662359

   Less than 2^32.  You can use this directly,  without having to use the
"factored BBS" method... but only if you have access to 128-bit integers!
At least at present,  the only system I know of with such support is gcc
on 64-bit platforms.  If you want compatibility with 32-bit systems,  you're
stuck with the above possibilities.
4293320519 4293338279 4293383399 4293466007 4293468359 4293474359 4293495527
4293500999 4293515399 4293522407 4293523847 4293554087 4293583079 4293594167
4293630359 4293646727 4293658199 4293752327 4293778487 4293809447 4293840119
4293842327 4293874727 4293919559 4293925319 4294005767 4294022807 4294030199
4294038887 4294052039 4294100087 4294105367 4294142759 4294177127 4294191719
4294239767 4294242119 4294247879 4294248887 4294260119 4294327319 4294399367
4294494167 4294504967 4294510439 4294517687 4294555319 4294598999 4294647047
4294656167 4294709207 4294728167 4294764647 4294801607 4294816679 4294879607
4294901687 4294956167

   Less than 2^64/sqrt(3) = 10650232656628343401.048.  This gives maximal
cycle length,  very close to 2^125 / 3.  But to get it,  you need to use
both the "factored BBS" method,  _and_ you need 128-bit integer support.

   This actually lists 2-safe primes,  with those congruent to 7 mod 16
asterisked.  So in picking a pair of primes for BBS,  one (or both) should
be asterisked,  as described at the beginning of these comments.

10650232656610726727* 10650232656610824359* 10650232656611686487*
10650232656611954207  10650232656612002759* 10650232656612014087*
10650232656612087359  10650232656612148079  10650232656612158399
10650232656612170159  10650232656612231407  10650232656612232367
10650232656612254207  10650232656612275087  10650232656612290999*
10650232656612337007  10650232656612661727  10650232656612711287*
10650232656612712679* 10650232656612841679  10650232656612862487*
10650232656612948527  10650232656612994727* 10650232656613164047
10650232656613291559* 10650232656613298447  10650232656613313087
10650232656613461839  10650232656613544999* 10650232656613843607*
10650232656614078327* 10650232656614291879* 10650232656614588567*
10650232656614639279  10650232656614698919* 10650232656614798399
10650232656614825999  10650232656614948759* 10650232656615010367
10650232656615099599  10650232656615324239  10650232656615432047
10650232656615551159* 10650232656615558719  10650232656615613679
10650232656615620639  10650232656615850919* 10650232656615882119*
10650232656615890519* 10650232656615922127  10650232656615927359
10650232656615971687* 10650232656616057199  10650232656616287599
10650232656616299359  10650232656616619687* 10650232656616769927*
10650232656616889279  10650232656616906127  10650232656616906919*
10650232656616941527* 10650232656617020487* 10650232656617110919*
10650232656617327087  10650232656617358527  10650232656617381879*
10650232656617476487* 10650232656617592407* 10650232656617596319
10650232656617607479* 10650232656617614079  10650232656617735879*
10650232656617845199  10650232656617935847* 10650232656617955119
10650232656618029039  10650232656618274199* 10650232656618347567
10650232656618609599  10650232656618616487* 10650232656618717167
10650232656618769199  10650232656618779639* 10650232656618781847*
10650232656618837287* 10650232656618855887  10650232656619197959*
10650232656619553447* 10650232656619573079* 10650232656619587959*
10650232656619659287* 10650232656619711007  10650232656619747487
10650232656619800887* 10650232656619959479* 10650232656620492087*
10650232656620530679* 10650232656620592887* 10650232656620773367*
10650232656620789567  10650232656620969879* 10650232656621009839
10650232656621133487  10650232656621226367  10650232656621272567*
10650232656621371279  10650232656621377087  10650232656621645359
10650232656621739367* 10650232656621742727* 10650232656621747767*
10650232656621757679  10650232656621989807  10650232656622058639
10650232656622153919  10650232656622237607* 10650232656622329119
10650232656622484207  10650232656622886807* 10650232656623238047
10650232656623419199  10650232656623710247* 10650232656623751287*
10650232656623777639* 10650232656623962967* 10650232656624014879
10650232656624607967  10650232656624675839  10650232656624764279*
10650232656624902567* 10650232656624905759  10650232656625002047
10650232656625123559* 10650232656625335167  10650232656625546199*
10650232656625594367  10650232656625595039  10650232656625750967*
10650232656625842719  10650232656625897607* 10650232656625903247
10650232656625922807* 10650232656625936079  10650232656625962119*
10650232656626081279  10650232656626100887* 10650232656626479727
10650232656626561927* 10650232656626613719* 10650232656626617007
10650232656626642567* 10650232656626760647* 10650232656627038087*
10650232656627124007* 10650232656627160679* 10650232656627188567*
10650232656627218639  10650232656627254567* 10650232656627307559*
10650232656627410807* 10650232656627422399  10650232656627465647
10650232656627502847  10650232656627513647  10650232656627557879*
10650232656627658079  10650232656627687239* 10650232656627713687*
10650232656627760607  10650232656628109159* 10650232656628286999*

   The first few BBS-safe primes :

     23     167     359    2039   4919    5639    5927    7559
   7607   13799   15287   20327  21767   23399   24407   25799
  29399   31607   35879   36887  42359   42839   46199   47207
  47639   48407   51287   52919  56039   56999   57287   58679
  58967   60647   64007   65687  72167   73847   78839   79559
  89399   93287   97127   98999 102407  104759  121559  129527
 134087  134999  146519  147047 148199  148727  165527  166487
 166679  166919  167879  168887 174599  182279  184727  192887
 195047  197927  200087  202967 209159  215399  219767  220919
 222647  223319  224327  235559 236087  240599  245639  247847
 250007  266999  268487  273047 274679  279767  283079  283607
 288647  300647  301319  302759 303287  303767  317399  321239
 322727  330887  337607  354647 371447  377687  381047  385079
 393287  393479  398759    */

#define uint128_t __uint128_t

         /* Following is fastest method,  involving no */
         /* use of "factored BBS",  but you need 64-bit gcc */
         /* to get the required 128-bit integers : */
uint64_t blum_blum_shub( const uint64_t x)
{
   const uint64_t p = 4294901687;
   const uint64_t q = 4294956167;
   uint128_t x2 = (uint128_t)x * (uint128_t)x;
   const uint64_t pq = p * q;

   return( (uint64_t)( x2 % (uint128_t)pq));
}

#ifdef NO_128_BIT_INTS
uint64_t blum_blum_shub( const uint64_t x)
{
   const uint64_t p = 2479637879;
   const uint64_t q = 2479662359;
   const uint64_t b = x % p;
   const uint64_t d = x % q;
   const uint64_t part1 = (((x / p) * d) % q) * p;
   const uint64_t part2 = (((x / q) * b) % p) * q;
   const uint64_t part3 = b * d;
   uint64_t rval = (part1 + part2 + part3) % (p * q);

   return( rval);
/* return( (x * x) % (p * q));     mathematically right,  but it overflows */
}
#endif

/* The above should suffice to generate pseudo-random numbers with
the Blum-Blum-Shub algorithm.  One problem is that of cycle length.
Almost all cycles will be huge,  of length (p-3)(q-3)/8.  I haven't
proven this,  but seeds congruent to 0, 1, or -1 modulo p appear
to result in cycles of length (q-3)/2.  Similarly,  seeds congruent
to 0, 1, or -1 modulo q result in cycles of length (p-3)/2.  And
it's easy to show that seeds 0, 1,  and pq-1 will have cycle lengths
of one; i.e.,  you just get the same number over and over.

   To get around this,  the following 'safe_bbs' functions keep
track of the initially-computed value in state[1],  and store the
seed in state[0].  If we realize we've just completed a cycle,
we increment the seed (a.k.a. state[0]) and re-seed.  Thus,  even
if you seed with 0,  1,  or pq - 1,  you'll very quickly move to
a longer cycle.

   After writing this code,  I read a similar prescription at

http://www.ciphersbyritter.com/NEWS6/BBS.HTM#3b38de9b.1115567@news.io.com

   The author of this post makes certain claims about cycle
length that seem likely,  but not proven.  Also,  the context
is a cryptographic system in which p and q are large enough that
a full length of (p-3)(q-3)/8 would be nice,  but cycles of
length of order p or q would be just fine.  In the context of
_this_ code -- to produce good pseudorandom numbers for seeding
WELL or Mersenne-Twister PRNGs -- p and q are of order 31 or 63
bits,  and we really ought to perform the modest work required
to ensure full cycles of order 63 or 127 bits. */

void seed_safe_bbs( uint64_t *state, const uint64_t seed)
{
   printf( "\nSeeding %lu\n", seed);
   *state = seed;
   state[1] = state[2] = blum_blum_shub( seed);
}

uint64_t safe_bbs( uint64_t *state)
{
   state[2] = blum_blum_shub( state[2]);
   if( state[2] == state[1])     /* cycle completed */
      seed_safe_bbs( state, state[0] + 1);
   return( state[2]);
}

int main( const int argc, const char **argv)
{
   uint64_t x0 = 3, state[3];
   int count = 0;

   if( argc == 2)
      x0 = (uint64_t)atol( argv[1]);
   seed_safe_bbs( state, x0);
   for( count = 0; count < 1000000; count++)
      {
      printf( "%20" PRIu64, safe_bbs( state));
      if( !(count % 4))
         printf( "\n");
      }
   return( 0);
}

#ifdef OLD_CYCLE_TEST_CODE
int main( const int argc, const char **argv)
{
   uint64_t x0 = 3, x;
   int count = 0;

   if( argc == 2)
      x0 = (uint64_t)atol( argv[1]);
   x0 = blum_blum_shub( x0);
   x = x0;
   do
      {
      printf( "%20" PRIu64, x);
      x = blum_blum_shub( x);
      count++;
      if( !(count % 4))
         printf( "\n");
      }
   while( x != x0);
   printf( "\n%d iterations\n", count);
   return( 0);
}
#endif
