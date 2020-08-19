BIN=mywm
PREFIX=/usr
SRC_FILE=wm.cpp
CC=g++
LD_FLAGS=-lX11

all:
		$(CC) $(LD_FLAGS) -o $(BIN) $(SRC_FILE)

install:
		make
		install $(BIN) $(PREFIX)/bin