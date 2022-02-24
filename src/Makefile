CC:=gcc
SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)
LDFLAGS:=-lmosquitto -lsqlite3 -luci -ljson-c -lcurl #-largp
BIN:=mqtt_subscriber

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) -g -o $(BIN) $(SRC) $(LDFLAGS)

install:
	install -m 666 ../files/events.config /etc/config/mqtt_sub_events
	install -m 666 ../files/topics.config /etc/config/mqtt_sub_topics

clean:
	$(RM) $(BIN) $(OBJS) 
