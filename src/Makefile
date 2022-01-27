CC:=gcc
SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)
LDFLAGS:=-lmosquitto -lsqlite3
BIN:=main

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) -o $(BIN) $(SRC) $(LDFLAGS)

clean:
	$(RM) $(BIN) $(OBJS)