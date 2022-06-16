CFLAGS := $(CFLAGS) -I. -I..
CC := gcc
BINARIES := $(patsubst %.cpp,%,$(wildcard *.cpp)) \
            $(patsubst %.c,%,$(wildcard *.c))

.PHONY: all clean
all: $(BINARIES)

% : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

SCREEN5 : SCREEN5.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BINARIES)
