# cdecl - C declarations translator

include config.mk

SRC = cdecl.c
OBJ = ${SRC:.c=.o}

all: options cdecl

options:
	@echo cdecl build options:
	@echo "CC       = ${CC}"
	@echo "CFLAGS   = ${CFLAGS}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

cdecl: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f cdecl ${OBJ}

.PHONY: all options clean
