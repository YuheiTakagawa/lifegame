CC=gcc
CFLAGS=-Wall

TARGET=lifegame_seq

all: $(TARGET)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ -lpthread 
