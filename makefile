# Generic build details
CXX = g++
OPTS= -std=c++14 -g
SRC := $(wildcard *.c)
EXE = CSPT.exe

$(EXE):
	$(CXX) $(OPTS) $(SRC) -o $(EXE)

.PHONY: clean build
.default: build

build: $(EXE)
clean:
	rm $(EXE)
