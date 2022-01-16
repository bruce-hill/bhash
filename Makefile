NAME=bhash
PREFIX=/usr/local
CC=cc
CFLAGS=-std=c99 -Werror -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -flto -fPIC
CWARN=-Wall -Wextra \
  -Wpedantic -Wsign-conversion -Wtype-limits -Wunused-result -Wnull-dereference \
	-Waggregate-return -Walloc-zero -Walloca -Warith-conversion -Wcast-align -Wcast-align=strict \
	-Wdangling-else -Wdate-time -Wdisabled-optimization -Wdouble-promotion -Wduplicated-branches \
	-Wduplicated-cond -Wexpansion-to-defined -Wfloat-conversion -Wfloat-equal -Wformat-nonliteral \
	-Wformat-security -Wformat-signedness -Wframe-address -Winline -Winvalid-pch -Wjump-misses-init \
	-Wlogical-op -Wlong-long -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn \
	-Wnull-dereference -Woverlength-strings -Wpacked -Wpacked-not-aligned -Wpointer-arith \
	-Wredundant-decls -Wshadow -Wshadow=compatible-local -Wshadow=global -Wshadow=local \
	-Wsign-conversion -Wstack-protector -Wsuggest-attribute=const -Wswitch-default -Wswitch-enum \
	-Wsync-nand -Wtrampolines -Wundef -Wunsuffixed-float-constants -Wunused -Wunused-but-set-variable \
	-Wunused-const-variable -Wunused-local-typedefs -Wunused-macros -Wvariadic-macros -Wvector-operation-performance \
	-Wvla -Wwrite-strings
EXTRA=
G=
O=-O3
ALL_FLAGS=$(CFLAGS) $(OSFLAGS) $(EXTRA) $(CWARN) $(G) $(O)

LIBFILE=lib$(NAME).so
CFILES=hashset.c hashmap.c intern.c
OBJFILES=$(CFILES:.c=.o)

all: $(LIBFILE)

clean:
	rm -f $(LIBFILE) $(OBJFILES)

$(LIBFILE): $(OBJFILES)
	$(CC) $^ -Wl,-soname,$(LIBFILE) -shared -o $@

%.o: %.c %.h
	$(CC) -c $(ALL_FLAGS) -o $@ $<

install: $(LIBFILE) bhash.h
	mkdir -p -m 755 "$(PREFIX)/lib" "$(PREFIX)/include"
	cp $(LIBFILE) "$(PREFIX)/lib"
	cp bhash.h "$(PREFIX)/include"

uninstall:
	rm -vf "$(PREFIX)/lib/$(LIBFILE)"

.PHONY: all install uninstall clean
