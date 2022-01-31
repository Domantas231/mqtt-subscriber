CC:=gcc
SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)
LDFLAGS:=-lmosquitto -lsqlite3 -luci -largp
BIN:=mqtt_subscriber

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) -o $(BIN) $(SRC) $(LDFLAGS)

clean:
	$(RM) $(BIN) $(OBJS)
