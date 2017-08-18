ifdef CLANG
CC=clang
else
CC=g++
endif

#   'XCOMPILE' = cross-compile for Windows,  using MinGW,  on a Linux box
#   'MSWIN' = compile for Windows,  using MinGW,  on a Windows machine
#   'CLANG' = use clang instead of GCC;  Linux only
# None of these: compile using g++ on Linux,  for Linux

CC=g++
LIBSADDED=
EXE=

ifdef CLANG
	CC=clang -Weverything
endif

RM=rm -f

ifdef MSWIN
	EXE=.exe
else
	LIBSADDED=-lm
endif

ifdef XCOMPILE
	CC=x86_64-w64-mingw32-g++
	EXE=.exe
	LIBSADDED=
endif

CFLAGS=-Wall -O3 -pedantic -Wextra

all: blum$(EXE) blum128$(EXE) blum2$(EXE) blum64$(EXE)  \
 blumtest$(EXE) cages$(EXE) comb$(EXE) feist$(EXE)      \
 hashtest$(EXE) latin$(EXE) latin1$(EXE) monty$(EXE)    \
 mt64test$(EXE) prime$(EXE) primit$(EXE) random$(EXE)   \
 test64$(EXE) well1k$(EXE) well512$(EXE) welltest$(EXE) \
 ziggur$(EXE)

blum$(EXE): blum.c gcd.c
	$(CC) $(CFLAGS) -o blum blum.c gcd.c

blum128$(EXE): blum128.c
	$(CC) $(CFLAGS) -o blum128 blum128.c

blum64$(EXE): blum64.c
	$(CC) $(CFLAGS) -o blum64 blum64.c

blumtest$(EXE): blumtest.c
	$(CC) $(CFLAGS) -o blumtest blumtest.c

blum2$(EXE): blum2.c gcd.c
	$(CC) $(CFLAGS) -o blum2 blum2.c gcd.c

cages$(EXE): cages.c
	$(CC) $(CFLAGS) -o cages cages.c

comb$(EXE): comb.c gcd.c
	$(CC) $(CFLAGS) -o comb comb.c gcd.c

prime$(EXE): prime.c gcd.c
	$(CC) $(CFLAGS) -o prime prime.c gcd.c

primit$(EXE): primit.c
	$(CC) $(CFLAGS) -o primit primit.c

feist$(EXE): feist.c
	$(CC) $(CFLAGS) -o feist feist.c

hashtest$(EXE): hashtest.o hash.o
	$(CC) $(CFLAGS) -o hashtest$(EXE) hashtest.o hash.o

latin$(EXE): latin.c
	$(CC) $(CFLAGS) -o latin latin.c

latin1$(EXE): latin1.c
	$(CC) $(CFLAGS) -o latin1 latin1.c

monty$(EXE): monty.c mt64.o
	$(CC) $(CFLAGS) -o monty monty.c mt64.o -lm

mt64.o: mt64.c
	g++ $(CFLAGS) -c mt64.c

mt64test$(EXE): mt64test.c mt64.o
	$(CC) $(CFLAGS) -o mt64test mt64test.c mt64.o

random$(EXE): random.c
	$(CC) $(CFLAGS) -o random random.c

test64$(EXE): test64.c isaac64.c
	$(CC) $(CFLAGS) -o test64 test64.c isaac64.c

well1k$(EXE): well1k.c well1024.c
	$(CC) $(CFLAGS) -o well1k well1k.c well1024.c

welltest$(EXE): welltest.c well3.c
	$(CC) $(CFLAGS) -o welltest welltest.c well3.c

well512$(EXE): well512.c
	$(CC) $(CFLAGS) -o well512 well512.c

ziggur$(EXE): ziggur.c
	$(CC) $(CFLAGS) -o ziggur ziggur.c -lm

clean:
	$(RM) *.o
	$(RM) blum$(EXE)
	$(RM) blum128$(EXE)
	$(RM) blum2$(EXE)
	$(RM) blum64$(EXE)
	$(RM) blumtest$(EXE)
	$(RM) cages$(EXE)
	$(RM) comb$(EXE)
	$(RM) feist$(EXE)
	$(RM) hashtest$(EXE)
	$(RM) latin$(EXE)
	$(RM) latin1$(EXE)
	$(RM) monty$(EXE)
	$(RM) mt64test$(EXE)
	$(RM) prime$(EXE)
	$(RM) primit$(EXE)
	$(RM) random$(EXE)
	$(RM) test64
	$(RM) well1k$(EXE)
	$(RM) well512$(EXE)
	$(RM) welltest$(EXE)
	$(RM) ziggur$(EXE)
