#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#define LATIN_SQUARE struct latin_square
#define MAX_SIZE 64
#define ls_mask uint64_t
#define swap( A, B, TVAL)   { TVAL = A; A = B; B = TVAL; }

LATIN_SQUARE
   {
   unsigned size;
   ls_mask xmask[MAX_SIZE];
   ls_mask ymask[MAX_SIZE];
   uint8_t soln[MAX_SIZE][MAX_SIZE];
   };

void init_grid( LATIN_SQUARE *ls, const unsigned size)
{
   unsigned i, j;
   ls_mask full_mask = ((ls_mask)1 << size) - (ls_mask)1;

         /* The '<<' operation apparently _rotates_ the bit back to 1 */
         /* if size == 32 (if ls_mask is uint32_t),  or size == 64    */
         /* (if ls_mask is uint64_t).  Which results in full_mask == 0, */
         /* instead of full_mask == all bits set.  This fixes it,  so   */
         /* you can get 32x32 or 64x64 squares :                        */
   if( !full_mask)
      full_mask = (ls_mask)-1;
   ls->size = size;
   for( i = 0; i < size; i++)
      {
      ls->xmask[i] = ls->ymask[i] = full_mask;
      for( j = 0; j < size; j++)
         ls->soln[i][j] = 0;
      }
}

static unsigned count_bits( ls_mask ival)
{
   unsigned rval = 0;

   while( ival)
      {
      rval++;
      ival &= (ival - 1);
      }
   return( rval);
}

static void set_value( LATIN_SQUARE *ls, const unsigned x, const unsigned y,
                  const unsigned value)
{
   const ls_mask mask = ~((ls_mask)1 << value);

   ls->xmask[x] &= mask;
   ls->ymask[y] &= mask;
   ls->soln[x][y] = value + 1;
}

unsigned max_n_fails = 1000;
unsigned n_fails = 0, n_tries = 0, n_solutions = 0;

#define UNSOLVABLE -1
#define SOLUTION_FOUND -2
#define MAXED_OUT_TRIES -3

int solve_latin_square( LATIN_SQUARE *ls)
{
   unsigned x, y, best_x = 0, best_y = 0, best_count = 10000;
   unsigned count, i, j;
   unsigned *possibles = (unsigned *)alloca( ls->size * sizeof( unsigned));
   ls_mask xsaved, ysaved, tval;

   n_tries++;
   if( n_fails >= max_n_fails)         /* consider to be 'unsolvable'; */
      return( MAXED_OUT_TRIES);        /* re-initialize grid & start again */
   for( x = 0; x < ls->size && best_count > 1; x++)
      for( y = 0; y < ls->size; y++)
         if( !ls->soln[x][y] && best_count > (count = count_bits( ls->xmask[x] & ls->ymask[y])))
            {
            best_count = count;
            best_x = x;
            best_y = y;
            }
   x = best_x;
   y = best_y;
   if( !best_count)     /* we have a square that can't be solved */
      {
      n_fails++;
      return( UNSOLVABLE);
      }
   if( best_count == 10000)      /* This square is solved */
      return( SOLUTION_FOUND);

   tval = ls->xmask[x] & ls->ymask[y];
   for( i = j = 0; tval; tval >>= 1, j++)
      if( tval & 1)
         possibles[i++] = j;
                  /* randomly scramble the 'possibles' array: */
   for( i = best_count - 1; i; i--)
      {
      const unsigned tval = possibles[i];
      const unsigned k = rand( ) % (i + 1);

      possibles[i] = possibles[k];
      possibles[k] = tval;
      }
   xsaved = ls->xmask[x];
   ysaved = ls->ymask[y];
   for( i = 0; i < best_count; i++)
      {
      int rval;

      if( i)
         {
         ls->xmask[x] = xsaved;
         ls->ymask[y] = ysaved;
         }
      set_value( ls, x, y, possibles[i]);
      rval = solve_latin_square( ls);
      if( rval == SOLUTION_FOUND || rval == MAXED_OUT_TRIES)
         return( rval);
      }
   ls->xmask[x] = xsaved;
   ls->ymask[y] = ysaved;
   ls->soln[x][y] = 0;
   return( 0);
}

static int reduce_square( LATIN_SQUARE *ls)
{
   unsigned i, j;
   uint8_t tval;

   for( i = 0; i < ls->size; i++)
      while( (unsigned)ls->soln[i][0] - 1 != i)
         {
         const unsigned k = ls->soln[i][0] - 1;

         for( j = 0; j < ls->size; j++)
            swap( ls->soln[i][j], ls->soln[k][j], tval);
         }
   for( i = 0; i < ls->size; i++)
      while( (unsigned)ls->soln[0][i] - 1 != i)
         {
         const unsigned k = ls->soln[0][i] - 1;

         for( j = 0; j < ls->size; j++)
            swap( ls->soln[j][i], ls->soln[j][k], tval);
         }
   return( 0);
}

#include <stdio.h>
#include <sys/resource.h>
#include <time.h>

static void show_grid( const LATIN_SQUARE *ls, const bool show_mask)
{
   unsigned x, y;

   printf( "\n");
   for( y = 0; y < ls->size; y++)
      {
      for( x = 0; x < ls->size; x++)
         printf( (ls->size > 9 ? "%3u" : "%2u"), ls->soln[x][y]);
      if( show_mask)
         for( x = 0; x < ls->size; x++)
            printf( " %04x", (unsigned)(ls->xmask[x] | ls->ymask[y]));
      printf( "\n");
      }
}

int main( const int argc, const char **argv)
{
   const unsigned grid_size = atoi( argv[1]);
   LATIN_SQUARE *ls = (LATIN_SQUARE *)malloc( sizeof( LATIN_SQUARE));
   unsigned i;
   bool show_mask = false, reduce = false;
   int rval;

   srand( time( NULL));
   for( i = 2; i < (unsigned)argc; i++)
      if( argv[i][0] == '-' && argv[i][1] == 's')
         srand( atoi( argv[i] + 2));
   for( i = 2; i < (unsigned)argc; i++)
      if( argv[i][0] == '-')
         switch( argv[i][1])
            {
            case 'r':             /* show a 'reduced' LS */
               reduce = true;
               break;
            case 'm':
               show_mask = true;
               break;
            case 'f':
               max_n_fails = atoi( argv[i] + 2);
               break;
            case 's':      /* handled above */
               break;
            default:
               printf("'%s' ignored\n", argv[i]);
               break;
            }
   do
      {
      init_grid( ls, grid_size);
      n_fails = n_tries = n_solutions = 0;
      rval = solve_latin_square( ls);
      if( rval == MAXED_OUT_TRIES)
         printf( "Maxed out\n");
      }
      while( rval == MAXED_OUT_TRIES);
   if( reduce)
      reduce_square( ls);
   show_grid( ls, show_mask);
   printf( "%u tries; %u fails\n", n_tries, n_fails);
   return( 0);
}

/*
1 2 3 4 5 6 7
3 1 4 7 6 2 5
5 3 2 1 7 4 6
2 4 7 6 3 5 1
4 5 6 2 1 7 3
6 7 5 3 4 1 2
7 6 1 5 2 3 4 */
