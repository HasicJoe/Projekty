CC=g++
CFLAGS=-Wall -std=c++11 -Wextra -pedantic -lm
TARGET=inflation 
DEPS=inflation.hpp firm.hpp bank.hpp household.hpp arguments.hpp crisis.hpp
OBJ=inflation.o firm.o bank.o household.o arguments.o crisis.o

%.o: %.cpp $(DEPS)
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
