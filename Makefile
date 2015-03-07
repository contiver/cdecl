# cdecl - C declarations translator

include config.mk

SRC = cdecl.c
OBJ = ${SRC:.c=.o}

all: options cdecl

options:
	@echo cdecl build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

cdecl: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ}
