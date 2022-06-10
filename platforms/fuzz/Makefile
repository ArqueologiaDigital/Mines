CC=clang
SOURCES=video.c input.c ../../common/main.c  ../../common/minefield.c
BUILDDIR=build/
CFLAGS=-g -O1 -fsanitize=address,fuzzer -Wall -pedantic -I../../common -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION=1

all: $(SOURCES)
	mkdir -p $(BUILDDIR)
	$(CC) -g -O1 $(SOURCES) $(CFLAGS) -o $(BUILDDIR)mines -fsanitize=address,fuzzer

clean:
	rm -rf $(BUILDDIR)
