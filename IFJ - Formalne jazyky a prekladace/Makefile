CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -pedantic
TARGET=ifj20compiler
DEPS=const.h scanner.h myString.h parser.h grammar.h list.h generator.h symtable.h builtin.h semantics.h
OBJ=scanner.o controller.o myString.o list.o parser.o generator.o symtable.o builtin.o semantics.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ) 
	$(CC) $(OBJ) $(CFLAGS) -o $(TARGET)

.PHONY: clean
.PHONY: run

clean:
	rm -f $(OBJ)

run:
	make
	./$(TARGET)
