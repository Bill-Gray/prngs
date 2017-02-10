#include <stdio.h>
#include <stdlib.h>
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

int main( const int argc, const char **argv)
{

   FILE *ifile;
   const char *ifilename = "/dev/random";
   unsigned i, j;
   unsigned count = 30, n_lines = 10;

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
               printf( "Unrecognized option '%s'\n", argv[i]);
               break;
            }
   show_poolsize( );
   ifile = fopen( ifilename, "rb");
   assert( ifile);
   for( i = 0; i < n_lines; i++)
      {
      for( j = 0; j < count; j++)
         {
         const int c = fgetc( ifile);

         assert( c != EOF);
         printf( "%02x", (unsigned)c);
         }
      printf( "\n");
      }
   show_poolsize( );
   return( 0);
}
