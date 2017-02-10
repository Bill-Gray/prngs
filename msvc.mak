all:  well512.exe welltest.exe well1k.exe random2.exe random4.exe test64.exe \
    hashtest.exe mt64test.exe monty.exe

CC=cl

CFLAGS=-c -W4 -Ox -nologo
LINK=link /nologo

.cpp.obj:
   $(CC) $(CFLAGS) $<

.c.obj:
   $(CC) $(CFLAGS) $<

well512.exe: well512.obj
   $(LINK)      well512.obj

well1k.exe: well1k.obj well1024.obj
   $(LINK)     well1k.obj well1024.obj

welltest.exe: welltest.obj well3.obj
   $(LINK)       welltest.obj well3.obj

random2.exe: random2.obj
   $(LINK)      random2.obj

random4.exe: random4.c
   $(CC) $(CFLAGS) -DTEST_PROGRAM random4.c

test64.exe: test64.obj isaac64.obj
   $(LINK)     test64.obj isaac64.obj

hashtest.exe: hashtest.obj hash.obj
   $(LINK)       hashtest.obj hash.obj

mt64test.exe: mt64test.obj mt64.obj
   $(LINK)       mt64test.obj mt64.obj

monty.exe: monty.obj mt64.obj
   $(LINK)    monty.obj mt64.obj

mt19937-64.exe: mt19937-64.obj
   $(LINK)         mt19937-64.obj
