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

bool attempt_joining( KENKEN_CAGES *k, unsigned *n_of_this_size, const unsigned *maxima,
               const unsigned n_groups)
{
   unsigned x1, y1, x2, y2, r = rand( );
   uint8_t val1, val2;

   x1 = r % k->size;
   y1 = (r / k->size) % (k->size - 1);
   if( r & 0x1000)
      {
      const unsigned tval = x1;

      x1 = y1;
      y1 = tval;
      x2 = x1 + 1;
      y2 = y1;
      }
   else
      {
      x2 = x1;
      y2 = y1 + 1;
      }
   val1 = k->cages[x1][y1];
   val2 = k->cages[x2][y2];
   if( val1 != val2 && val2 != (uint8_t)(n_groups - 1)
                    && val1 != (uint8_t)(n_groups - 1))
      {
      unsigned count1 = 0, count2 = 0;
      unsigned i, j;

      for( i = 0; i < k->size; i++)
         for( j = 0; j < k->size; j++)
            {
            if( k->cages[i][j] == val1)
               count1++;
            if( k->cages[i][j] == val2)
               count2++;
            }
      if( n_of_this_size[count1 + count2] < maxima[count1 + count2])
         {
         for( i = 0; i < k->size; i++)
            for( j = 0; j < k->size; j++)
               {
               if( k->cages[i][j] == val2)
                  k->cages[i][j] = val1;
               if( k->cages[i][j] == (uint8_t)(n_groups - 1))
                  k->cages[i][j] = val2;
               }
         n_of_this_size[count1]--;
         n_of_this_size[count2]--;
         n_of_this_size[count1 + count2]++;
         return( true);
         }
      }
   return( false);
}

static void show_cages2( const KENKEN_CAGES *k)
{
   unsigned i, j, line;
   char blank[10], hline[10];

   memset( blank, ' ', 10);
   memset( hline, '-', 10);
   blank[4] = hline[4] = '\0';
   for( i = 0; i <= k->size; i++)
      {
      for( j = 0; j < k->size; j++)
         {
         printf( "+");
         if( !i || i == k->size || k->cages[j][i] != k->cages[j][i - 1])
            printf( "%s", hline);
         else
            printf( "%s", blank);
         }
      printf( "+\n");
      for( line = 0; i < k->size && line < 2; line++)
         {
         for( j = 0; j <= k->size; j++)
            {
            if( !j || j == k->size || k->cages[j][i] != k->cages[j - 1][i])
               printf( "|");
            else
               printf( " ");
            printf( "%s", blank);
            }
         printf( "\n");
         }
      }
}

void show_cages( const KENKEN_CAGES *k, const unsigned *n_in_group)
{
   unsigned i, j;

   for( i = 0; i < k->size; i++)
      {
      for( j = 0; j < k->size; j++)
         printf( "%c ", (char)(k->cages[j][i] + '0'));
      printf( "\n");
      }
   for( i = 1; i < 12; i++)
      if( n_in_group[i])
         printf( "%u: %u   ", i, n_in_group[i]);
   printf( "\n");
}

void set_cages( KENKEN_CAGES *k, const unsigned *maxima)
{
   unsigned n_of_this_size[40], i, j;
   unsigned n_groups = k->size * k->size;

   for( i = 0; i < k->size; i++)
      for( j = 0; j < k->size; j++)
         k->cages[i][j] = (uint8_t)( i + j * k->size);
   for( i = 1; i < sizeof( n_of_this_size) / sizeof( n_of_this_size[0]); i++)
      n_of_this_size[i] = 0;
   n_of_this_size[1] = n_groups;
   show_cages( k, n_of_this_size);
   for( i = n_groups * 3; i; i--)
      if( attempt_joining( k, n_of_this_size, maxima, n_groups))
         {
         show_cages2( k);
         show_cages( k, n_of_this_size);
         n_groups--;
         }
}

int main( const int argc, const char **argv)
{
   const unsigned grid_size = atoi( argv[1]);
   unsigned max_n_of_this_size[13] = { 0, 49, 10, 6, 2, 1, 0, 0, 0, 0, 0, 0, 0 };
   unsigned i;
   KENKEN_CAGES k;

   srand( time( NULL));
   for( i = 2; i < (unsigned)argc; i++)
      if( argv[i][0] == '-' && argv[i][1] == 's')
         srand( atoi( argv[i] + 2));
   for( i = 2; i < (unsigned)argc; i++)
      if( argv[i][0] == '-')
         switch( argv[i][1])
            {
            case 's':             /* handled above     */
               break;
            case 'c':
               sscanf( argv[i] + 2, "%u,%u,%u,%u,%u,%u",
                    max_n_of_this_size + 1,
                    max_n_of_this_size + 2,
                    max_n_of_this_size + 3,
                    max_n_of_this_size + 4,
                    max_n_of_this_size + 5,
                    max_n_of_this_size + 6);
               break;
            default:
               printf("'%s' ignored\n", argv[i]);
               break;
            }
   k.size = grid_size;
   set_cages( &k, max_n_of_this_size);
   return( 0);
}

