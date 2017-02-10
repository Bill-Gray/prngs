#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/resource.h>
#include <time.h>

#define uint128_t __uint128_t
#define LATIN_SQUARE struct latin_square
#define MAX_SIZE 128
#define ls_mask uint128_t

LATIN_SQUARE
   {
   unsigned size;
   ls_mask mask[MAX_SIZE][MAX_SIZE];
   uint8_t soln[MAX_SIZE][MAX_SIZE];
   };

void init_grid( LATIN_SQUARE *ls, const unsigned size)
{
   unsigned i, j;
   ls_mask full_mask = ((ls_mask)1 << size) - (ls_mask)1;

         /* The '<<' operation apparently _rotates_ the bit back to 1 */
         /* if size == 32 (if ls_mask is uint32_t),  or size == 64    */
         /* (if ls_mask is uint64_t).  Which results in full_mask == 0, */
         /* instead of full_mask == all bits set.  This fixes it:       */
   if( !full_mask)
      full_mask = (ls_mask)-1;
   ls->size = size;
   for( i = 0; i < size; i++)
      for( j = 0; j < size; j++)
         {
         ls->mask[i][j] = full_mask;
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
            printf( " %04x", (unsigned)ls->mask[x][y]);
      printf( "\n");
      }
}

static int look_for_single_candidates( LATIN_SQUARE *ls,
         unsigned *xloc, unsigned *yloc)
{
   unsigned x, y;
   const ls_mask all_bits = ((ls_mask)1 << ls->size) - (ls_mask)1;

   for( y = 0; y < ls->size; y++)
      {
      ls_mask at_least_one_bit_set = 0;
      ls_mask at_least_two_bits_set = 0;
      ls_mask solved = 0;
      ls_mask found;

      for( x = 0; x < ls->size; x++)
         {
         at_least_two_bits_set |= (at_least_one_bit_set & ls->mask[x][y]);
         at_least_one_bit_set |= ls->mask[x][y];
         solved |= ((ls_mask)1) << (ls->soln[x][y] - 1);
         }
      if( (~at_least_one_bit_set) & all_bits)
         {
/*       printf( "Unsolvable in row %u: %lx, %lx\n",
                     y, solved, at_least_one_bit_set);
         show_grid( ls, true);         */
         return( -1);
         }
                  /* We're looking for numbers that haven't been found
                     yet,  and which can only appear in one cell: */
      found = ~solved & at_least_one_bit_set & ~at_least_two_bits_set;
      if( found)
         {
         unsigned i;

         found &= found - 1;     /* this clears all but the lowest bit */
         for( i = 0; i < ls->size; i++)
            if( (found >> i) & 1)
               {
               *yloc = y;
               for( x = 0; x < ls->size; x++)
                  if( found & ls->mask[x][y])
                     {
                     *xloc = x;
/*                   printf( "Only candidate in row at %u, %u is %u\n",
                              x, y, i + 1);
                     show_grid( ls, true);
*/                   return( i + 1);
                     }
               assert( 1);    /* shouldn't get here */
               }
         assert( 2);    /* shouldn't get here,  either */
         }
      }

   for( x = 0; x < ls->size; x++)
      {
      ls_mask at_least_one_bit_set = 0;
      ls_mask at_least_two_bits_set = 0;
      ls_mask solved = 0;
      ls_mask found;

      for( y = 0; y < ls->size; y++)
         {
         at_least_two_bits_set |= (at_least_one_bit_set & ls->mask[x][y]);
         at_least_one_bit_set |= ls->mask[x][y];
         solved |= ((ls_mask)1) << (ls->soln[x][y] - 1);
         }
      if( (~at_least_one_bit_set) & all_bits)
         {
/*       printf( "Unsolvable in col %u: %lx, %lx\n",
                     x, solved, at_least_one_bit_set);
         show_grid( ls, true);
*/       return( -1);
         }
                  /* We're looking for numbers that haven't been found
                     yet,  and which can only appear in one cell: */
      found = ~solved & at_least_one_bit_set & ~at_least_two_bits_set;
      if( found)
         {
         unsigned i;

         found &= found - 1;     /* this clears all but the lowest bit */
         for( i = 0; i < ls->size; i++)
            if( (found >> i) & 1)
               {
               *xloc = x;
               for( y = 0; y < ls->size; y++)
                  if( found & ls->mask[x][y])
                     {
                     *yloc = y;
/*                   printf( "Only candidate in col at %u, %u is %u\n",
                              x, y, i + 1);
                     show_grid( ls, true);
*/                   return( i + 1);
                     }
               assert( 3);    /* shouldn't get here */
               }
         assert( 4);    /* shouldn't get here,  either */
         }
      }
   return( 0);
}

static void set_value( LATIN_SQUARE *ls, const unsigned x, const unsigned y,
                  const unsigned value)
{
   unsigned i;
   const ls_mask mask = ~((ls_mask)1 << value);

   for( i = 0; i < ls->size; i++)
      {
      ls->mask[i][y] &= mask;
      ls->mask[x][i] &= mask;
      }
   ls->mask[x][y] = (ls_mask)1 << value;
   ls->soln[x][y] = value + 1;
}

unsigned n_fails = 0, n_tries = 0, n_solutions = 0;
bool show_all_squares = false;

static void restore_masks( LATIN_SQUARE *ls, const ls_mask *saved,
            const unsigned x, const unsigned y)
{
   unsigned i;

   for( i = 0; i < ls->size; i++)
      {
      ls->mask[i][y] = saved[i];
      ls->mask[x][i] = saved[i + ls->size];
      }
}

static void store_masks( const LATIN_SQUARE *ls, ls_mask *saved,
            const unsigned x, const unsigned y)
{
   unsigned i;

   for( i = 0; i < ls->size; i++)
      {
      saved[i]            = ls->mask[i][y];
      saved[i + ls->size] = ls->mask[x][i];
      }
}

#define SOLUTION_FOUND -2

int solve_latin_square( LATIN_SQUARE *ls)
{
   unsigned i, j, best_i = 0, best_j = 0, best_count = 10000;
   unsigned count;
   unsigned *possibles = (unsigned *)alloca( ls->size * sizeof( unsigned));
   ls_mask *saved_masks = (ls_mask *)alloca( ls->size * 2 * sizeof( ls_mask));
   bool single_candidate = false;

   n_tries++;

#if 1
   i = look_for_single_candidates( ls, &best_i, &best_j);
   if( i == (unsigned)-1)      /* can't solve this LS */
      return( 0);
   if( i)
      {
      possibles[0] = i - 1;
      best_count = 1;
      single_candidate = true;
      }
#endif
   for( i = 0; i < ls->size && best_count > 1; i++)
      for( j = 0; j < ls->size; j++)
         if( !ls->soln[i][j] && best_count > (count = count_bits( ls->mask[i][j])))
            {
            best_count = count;
            best_i = i;
            best_j = j;
            }
   if( !best_count)     /* we have a square that can't be solved */
      {
      n_fails++;
      return( -1);
      }
   if( best_count == 10000)      /* This square is solved */
      {
      if( show_all_squares)
         {
         show_grid( ls, false);
         n_solutions++;
         return( 0);
         }
      else
         return( SOLUTION_FOUND);
      }
   if( !single_candidate)
      {
      ls_mask tval = ls->mask[best_i][best_j];

      for( i = j = 0; tval; tval >>= 1, j++)
         if( tval & 1)
            possibles[i++] = j;
      }
                  /* should scramble 'possibles' here... */
   for( i = best_count - 1; i; i--)
      {
      const unsigned tval = possibles[i];
      const unsigned k = rand( ) % (i + 1);

      possibles[i] = possibles[k];
      possibles[k] = tval;
      }
   store_masks( ls, saved_masks, best_i, best_j);
   for( i = 0; i < best_count; i++)
      {
      if( i)
         restore_masks( ls, saved_masks, best_i, best_j);
      set_value( ls, best_i, best_j, possibles[i]);
      if( solve_latin_square( ls) == SOLUTION_FOUND)
         return( SOLUTION_FOUND);
      }
   restore_masks( ls, saved_masks, best_i, best_j);
   ls->soln[best_i][best_j] = 0;
   return( 0);
}

int main( const int argc, const char **argv)
{
   const unsigned grid_size = atoi( argv[1]);
   LATIN_SQUARE *ls = (LATIN_SQUARE *)malloc( sizeof( LATIN_SQUARE));
   unsigned i;
   bool show_mask = false;
   const rlim_t kStackSize = 90 * 1024 * 1024;   /* min stack size = 16 MB */
   struct rlimit rl;
   int result;

   result = getrlimit(RLIMIT_STACK, &rl);
   if (result == 0)
   {
       if (rl.rlim_cur < kStackSize)
       {
           rl.rlim_cur = kStackSize;
           result = setrlimit(RLIMIT_STACK, &rl);
           if (result != 0)
           {
               fprintf(stderr, "setrlimit returned result = %d\n", result);
           }
       }
   }

   srand( time( NULL));
   for( i = 2; i < (unsigned)argc; i++)
      if( argv[i][0] == '-' && argv[i][1] == 's')
         srand( atoi( argv[i] + 2));
   init_grid( ls, grid_size);
   for( i = 2; i < (unsigned)argc; i++)
      if( argv[i][0] == '-')
         switch( argv[i][1])
            {
            case 'r':             /* show a 'reduced' LS */
               {
               unsigned k;

               for( k = 0; k < grid_size; k++)
                  {
                  set_value( ls, k, 0, k);
                  set_value( ls, 0, k, k);
                  }
               }
               break;
            case 'm':
               show_mask = true;
               break;
            case 'a':
               show_all_squares = true;
               break;
            case 's':      /* handled above */
               break;
            default:
               printf("'%s' ignored\n", argv[i]);
               break;
            }
   solve_latin_square( ls);
   if( !show_all_squares)
      show_grid( ls, show_mask);
   else
      printf( "%u solutions found\n", n_solutions);
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
