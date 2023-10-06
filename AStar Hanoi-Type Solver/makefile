compiler = g++
compiler_flags = -std=c++11

heuristic ?= 3
file ?= ""
executable = blocksworld

.PHONY: all clean

all: $(executable)

$(executable): *.cpp 
	$(compiler) $(compiler_flags) -o $@ $^

clean:
	rm -f *.cpp

run: $(executable)
	./blocksworld -f $(file) -H $(heuristic)
