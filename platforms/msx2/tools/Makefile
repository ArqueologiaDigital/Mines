SOURCE := $(wildcard *.cpp)
BINARIES := $(patsubst %.cpp,%,$(SOURCE))

.PHONY: all clean
all: $(BINARIES)

% : %.cpp
	$(CXX) -o $@ $< -I.

clean:
	rm -rf $(BINARIES)
