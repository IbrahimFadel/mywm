BIN=wm
SRC_FILE=wm.cpp
CC=g++
C_FLAGS=-Wall -pedantic
LD_FLAGS=-lX11

all:
		$(CC) $(C_FLAGS) $(LD_FLAGS) -o $(BIN) $(SRC_FILE)