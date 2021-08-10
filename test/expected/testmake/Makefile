CC = gcc
CFLAGS = -Wall -pedantic -g
TARGET = HT
OBJ = $(patsubst %.c, %.o, $(wildcard *.c))

.PHONY: default all clean

all: default

default: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
