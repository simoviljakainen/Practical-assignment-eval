CC = gcc
CFLAGS = -Wall -pedantic -I. -I'/usr/include/libxml2/' -lxml2 -g
TARGET = evaluate
OFOLD = obj
SRC = src

OBJ = $(patsubst $(SRC)/%.c, $(OFOLD)/%.o, $(wildcard $(SRC)/*.c))

.PHONY: default all clean

all: default
default: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
$(OFOLD)/%.o: $(SRC)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OFOLD)/*.o
	rm -f $(TARGET)

cm:
	make clean&&make

run:
	make cm&&valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all --verbose ./$(TARGET) -n000000