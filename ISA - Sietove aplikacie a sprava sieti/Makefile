CC=g++
CC_FLAGS=-std=c++11
CC_FILES=client.cpp client.hpp
DEST=client
CLEAN=client
TARF=tar -cvf xvalas10.tar *

all:
	$(CC) $(CC_FLAGS) $(CC_FILES) -o $(DEST)

clean:
	rm -f $(CLEAN)

tar:
	$(TARF)
