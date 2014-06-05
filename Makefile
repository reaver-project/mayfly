CC=clang++
LD=clang++
CFLAGS=-c -Os -Wall -Wextra -pedantic -Werror -std=c++1y -stdlib=libc++ -g -MD -pthread -fPIC -Wno-unused-private-field
LDFLAGS=-stdlib=libc++ -lc++abi -lboost_system -lboost_program_options -lboost_iostreams -pthread
SOURCES=$(shell find tests/ -type f -name "*.cpp" ! -path "./tests/main.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=mayfly-test

all: $(SOURCES) $(LIBRARY) $(EXECUTABLE)

library: $(LIBRARY)

install:
	@sudo mkdir -p /usr/local/include/reaver/mayfly
	@find . -name "*.h" ! -path "*-old" ! -name "mayfly.h" | sudo cpio -pdm /usr/local/include/reaver/mayfly 2> /dev/null
	@sudo cp mayfly.h /usr/local/include/reaver

$(EXECUTABLE): install tests/main.o
	$(LD) $(LDFLAGS) -o $@ tests/main.o -lreaver -ldespayre

$(LIBRARY): $(OBJECTS)
	$(LD) $(SOFLAGS) -o $@ $(OBJECTS) -lreaver

%.o: %.cpp install
	$(CC) $(CFLAGS) $< -o $@

clean:
	@find . -name "*.o" -delete
	@find . -name "*.d" -delete
	@rm -rf $(EXECUTABLE)

-include $(SOURCES:.cpp=.d)
-include main.d
