S    = $(shell uname)
SRC   = src/
BIN   = bin/
FLAGS = -std=c++11 -Wall -pedantic -w
CC    = g++-4.7
OBJS  = $(shell ls src/*cpp | sed s/src/bin/g | sed s/cpp/o/g)
PLGNS = $(shell ls src/*cpp | sed s/src/bin/g | sed s/cpp/so/g)

.PHONY: all clean edit $(BIN)

all: $(PLGNS)

$(BIN)%.o: $(SRC)%.cpp
	    $(CC) -c $(FLAGS) $< -o $@

$(BIN)%.so: $(BIN)%.o
	    $(CC) $(FLAGS) -shared $^ -o $@

clean:
	rm -rf $(BIN)*.o
	rm -rf $(BIN)*.so
