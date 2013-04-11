# SRTP module for EEICT
#
# author: Jan Wozniak



OS    = $(shell uname)
PROJ  = srtp_stack
SRC   = src/
BIN   = bin/
FLAGS = -std=c++11 -Wall -pedantic -w 
CC    = g++-4.7
OBJS  = $(shell ls src/*cpp | sed s/src/bin/g | sed s/cpp/o/g)


ifeq ($(OS), Darwin)
	LIBS=-framework OpenCL -ldl
	CL_FLAGS=-I/usr/local/share/cuda/include/
	BOOST_FLAGS=-lboost_thread-mt -lboost_system-mt
else
ifeq ($(OS), CYGWIN_NT-5.1)
	LIBS=
	CL_FLAGS=
else
	LIBS=-lOpenCL -ldl 
	CL_FLAGS=-L/opt/AMDAPP/lib/x86/ -I/opt/AMDAPP/include/
	OBJ_CL_FLAGS=-I/opt/AMDAPP/include/
	OBJ_LIBS=-lOpenCL
	BOOST_FLAGS=-l boost_thread-mt -lboost_system-mt
endif
endif

.PHONY: all clean edit $(BIN)

all: $(BIN)$(PROJ)

$(BIN)%.o: $(SRC)%.cpp
	$(CC) -c $(FLAGS) $(OBJ_CL_FLAGS) $< -o $@ $(OBJ_LIBS)

$(BIN)$(PROJ): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(BIN)$(PROJ) $(CL_FLAGS) $(BOOST_FLAGS) $(LIBS)

$(BIN):
	$(shell mkdir $(BIN))


docu: doc/$(DOC).tex
	cd doc; pdflatex $(DOC)
	cd doc; pdflatex $(DOC)


edit:
	vim -S ses.vim

run:
	cd $(BIN); ./$(PROJ)

clean:
	rm -rf $(BIN)*
