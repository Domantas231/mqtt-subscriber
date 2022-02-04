CC:=gcc
SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)
LDFLAGS:=-lmosquitto -lsqlite3 -luci -ljson-c -lcurl #-largp
BIN:=mqtt_subscriber

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) -g -o $(BIN) $(SRC) $(LDFLAGS)

clean:
	$(RM) $(BIN) $(OBJS)
