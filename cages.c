#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/resource.h>
#include <time.h>

#define KENKEN_CAGES struct kenken_cages
#define MAX_SIZE 64

KENKEN_CAGES
   {
   unsigned size;
   uint8_t cages[MAX_SIZE][MAX_SIZE];
   };

void show_cages( const KENKEN_CAGES *k)
{
   unsigned i, j;

   for( i = 0; i < k->size; i++)
      {
      for( j = 0; j < k->size; j++)
         printf( "%c ", (char)(k->cages[j][i] + '0'));
      printf( "\n");
      }
   printf( "\n");
}

void set_cages( KENKEN_CAGES *k)
{
   unsigned i, j, iter = 0;
   unsigned n_cells = k->size * k->size;
   unsigned n_groups = 0;
   unsigned initial_runs = n_cells * 5;

   for( i = 0; i < k->size; i++)
      for( j = 0; j < k->size; j++)
         k->cages[i][j] = 0;
   while( n_cells)
      {
      unsigned i1, j1;

      if( iter & 1)     /* vertical */
         {
         i = i1 = rand( ) % k->size;
         j = rand( ) % (k->size - 1);
         j1 = j + 1;
         }
      else              /* horiz : swap i & j */
         {
         j = j1 = rand( ) % k->size;
         i = rand( ) % (k->size - 1);
         i1 = i + 1;
         }
      if( !k->cages[i][j] && !k->cages[i1][j1])
         {
         n_groups++;
         k->cages[i][j] = k->cages[i1][j1] = (uint8_t)n_groups;
         n_cells -= 2;
         }
      else if( !k->cages[i][j] && iter > initial_runs)
         {
         k->cages[i][j] = k->cages[i1][j1];
         n_cells--;
         }
      else if( !k->cages[i1][j1] && iter > initial_runs)
         {
         k->cages[i1][j1] = k->cages[i][j];
         n_cells--;
         }
      iter++;
      printf( "Iter %u; %u groups\n", iter, n_groups);
      show_cages( k);
      }
   show_cages( k);
}

int main( const int argc, const char **argv)
{
   const unsigned grid_size = atoi( argv[1]);
   KENKEN_CAGES k;

   srand( time( NULL));
   k.size = grid_size;
   set_cages( &k);
   return( 0);
}

