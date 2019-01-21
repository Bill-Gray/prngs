#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Code to create (in PostScript format) pages you can cut into
strips of paper to attach to junk CDs/DVDs to make a Jefferson
cipher wheel.  I take a stack junk disks, use some transparent
tape to make them a disk about a centimeter thick,  cut pairs of
strips generated by this program,  and tape said strips to the
disk.  Repeat a few dozen times,  put the results on a dowel, and
you have your Jefferson cipher wheel.

   I first did this to make a wheel so I could demonstrate a simple
crypto device to grade-school kids.

   The circumference of a DVD is about 14.92 inches,  which is why
we print pairs of strips, each with 13 letters on them, each strip
about 7.96 inches long.  Hence the 'if( j == 12 || j == 25)' line.

   You have to provide a number on the command line to seed
the PRNG.  That's just a safeguard against accidentally making
the same wheel twice.

   By default,  you get fifty pages,  each of which has sixteen
strips,  enough for eight cipher disks,  so 400 disks in total.
Overkill,  I realize.         */

int main( const int argc, const char **argv)
{
   FILE *ifile = fopen( "jeff_cipher.ps", "rb");
   char buff[200];
   unsigned i, j, page;
   unsigned n_pages = (argc < 3 ? 50 : atoi( argv[2]));;

   assert( ifile);
   assert( argc > 1);
   srand( atoi( argv[1]));
   while( fgets( buff, sizeof( buff), ifile) && memcmp( buff, "(A) (B) (C)", 11))
      {
      char *tptr = strstr( buff, "(seed)");

      if( !memcmp( buff, "%%Pages: ", 9))
         sprintf( buff + 9, "%u\n", n_pages);
      if( tptr)
         sprintf( tptr, "(%s) show\n", argv[1]);
      printf( "%s", buff);
      }
   strcpy( buff, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
   for( page = 1; page < n_pages; page++)
      {
      printf( "%%%%Page: %u %u\n", page, page);
      printf( "%u init_page\n", page);
      for( i = 0; i < 8; i++)
         {
         const unsigned len = 26;

         for( j = 0; j < len; j++)
            {
            int k = rand( ) % (len - j) + j;
            char tchar = buff[k];

            buff[k] = buff[j];
            buff[j] = tchar;
            printf( "(%c) ", tchar);
            if( j == 12 || j == 25)
               printf( "\nshow_disk\n");
            }
         }
      printf( "\nstroke\nshowpage\n");
      }
   return( 0);
}
