PREFIX   = /usr/local
DESTDIR  =

# I use C99
CC       = gcc -std=c99
AR       = ar

CPPFLAGS =
CFLAGS   = -g
LDFLAGS  =
ARFLAGS  = -rv

all: mini libmini.a
clean:
	rm -f mini libmini.a *.o
install: all uninstall
	mkdir -p $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/include $(DESTDIR)$(PREFIX)/lib
	cp mini $(DESTDIR)$(PREFIX)/bin
	cp mini.h $(DESTDIR)$(PREFIX)/include
	cp libmini.a $(DESTDIR)$(PREFIX)/lib
uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/bin/mini $(DESTDIR)$(PREFIX)/include/mini.h $(DESTDIR)$(PREFIX)/lib/libmini.a

getdelim.o: getdelim.c getdelim.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c getdelim.c
strtok_r.o: strtok_r.c strtok_r.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c strtok_r.c
mini.o: mini.c mini.h getdelim.h strtok_r.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c mini.c
asarg.o: asarg.c asarg.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c asarg.c
main.o: main.c mini.h asarg.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c main.c
mini: main.o mini.o getdelim.o strtok_r.o asarg.o
	$(CC) $(LDFLAGS) -o mini main.o mini.o getdelim.o strtok_r.o asarg.o -ldl -lreadline
libmini.a: mini.o getdelim.o strtok_r.o
	rm -f libmini.a && $(AR) $(ARFLAGS) libmini.a mini.o getdelim.o # TO.DO: -ldl -lreadline могут требовать др. либ + их надо подключать при компиляции своих прог
