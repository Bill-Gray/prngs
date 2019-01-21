#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void show_poolsize( void)
{
   FILE *ifile = fopen( "/proc/sys/kernel/random/entropy_avail", "rb");
   int c;

   printf( "Entropy available: ");
   while( (c = fgetc( ifile)) != EOF)
      printf( "%c", c);
   printf( "\n");
   fclose( ifile);

   ifile = fopen( "/proc/sys/kernel/random/poolsize", "rb");
   printf( "Pool size: ");
   while( (c = fgetc( ifile)) != EOF)
      printf( "%c", c);
   printf( "\n");
   fclose( ifile);
}

static const char *digits =
   "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
   "+/!\"#$%&'()*,-.:;<=>?@[\\]^_`{|}";

int main( const int argc, const char **argv)
{

   FILE *ifile;
   const char *ifilename = "/dev/random";
   unsigned i, j;
   unsigned count = 64, n_lines = 10;
   unsigned radix = 16;   /* default to hex output */
   unsigned x = 0, mod = 1;
   unsigned bytes_read = 0;

   setbuf( stdout, NULL);
   for( i = 1; i < (unsigned)argc; i++)
      if( argv[i][0] == '-')
         switch( argv[i][1])
            {
            case 'l':
               n_lines = atoi( argv[i] + 2);
               break;
            case 'u':
               ifilename = "/dev/urandom";
               break;
            default:
               if( atoi( argv[i] + 1))
                  radix = (unsigned)atoi( argv[i] + 1);
               else
                  printf( "Unrecognized option '%s'\n", argv[i]);
               break;
            }
   show_poolsize( );
   ifile = fopen( ifilename, "rb");
   assert( ifile);
   assert( radix <= strlen( digits));
   for( i = 0; i < n_lines; i++)
      {
      j = 0;
      while( j < count)
         {
         const int c = fgetc( ifile);

         mod *= 256;
         x += (unsigned)c;
         bytes_read++;
         assert( c != EOF);
         assert( c >=0 && c <= 256);
         while( j < count)
            if( x / radix < mod / radix)
               {
               printf( "%c", digits[x % radix]);
               mod /= radix;
               x /= radix;
               j++;
               }
            else
               {
               x %= radix;
               mod %= radix;
               break;
               }
         }
      printf( "\n");
      }
   show_poolsize( );
   printf( "%u random bytes read\n", bytes_read);
   return( 0);
}
