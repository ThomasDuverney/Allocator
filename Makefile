CC=gcc

# uncomment to compile in 32bits mode (require gcc-*-multilib packages
# on Debian/Ubuntu)
#HOST32= -m32

CFLAGS= $(HOST32) -Wall -Werror -std=c99 -g -DMEMORY_SIZE=128000
CFLAGS+= -DDEBUG
LDFLAGS= $(HOST32)
TESTS+=test_init
PROGRAMS=memshell $(TESTS)

_IRED=$'\x1b[41m'
_IGREEN=$'\x1b[42m'
_END=$'\x1b[0m'

.PHONY: clean all test_ls

all: $(PROGRAMS)
	for file in $(TESTS);do ./$$file; done

# dépendences des binaires
$(PROGRAMS): %: mem.o common.o

# dépendances des fichiers objects
$(patsubst %.c,%.o,$(wildcard *.c)): %.o: .%.deps

.%.deps: %.c
	$(CC) $(CPPFLAGS) -MM $< | sed -e 's/\(.*\).o: /.\1.deps \1.o: /' > $@

-include $(wildcard .*.deps)

# seconde partie du sujet
libmalloc.so: malloc_stub.o mem.o
	$(CC) -shared -Wl,-soname,$@ $^ -o $@

test_ls: libmalloc.so
	LD_PRELOAD=./libmalloc.so ls

# nettoyage
clean:
	rm -f *.o $(PROGRAMS) libmalloc.so .*.deps

test:
	echo " ${_IRED}Lancement des tests sur l'allocateur${_END}"
	./memshell < file_test
	echo 'Test ok'
