BIN=mywm
PREFIX=/usr
SRC_FILE=wm.c
CC=cc
LD_FLAGS=-lxcb -lxcb-keysyms -Wall -pedantic -std=c99

all:
		$(CC) $(LD_FLAGS) -o $(BIN) $(SRC_FILE)

install:
		make
		install $(BIN) $(PREFIX)/bin