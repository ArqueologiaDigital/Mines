XPM_INCLUDE := -DXPM_FILE="\"../$(XPM)\""
XPM_DATA := -DXPM_DATA=$(subst .,_,$(XPM))
CFLAGS := $(CFLAGS) -I. -I..
CC := gcc
BINARIES := $(patsubst %.cpp,%,$(wildcard *.cpp)) \
            $(patsubst %.c,%,$(wildcard *.c))

.PHONY: all clean
all: $(BINARIES)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.DELETE_ON_ERROR:
SCREEN5: SCREEN5.c
	$(CC) $(CFLAGS) $(XPM_INCLUDE) $(XPM_DATA) -o $@ $<

clean:
	rm -rf $(BINARIES)
