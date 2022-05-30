SOURCES=video.c input.c ../../common/main.c  ../../common/minefield.c
BUILDDIR=build/

all: $(SOURCES)
	mkdir -p $(BUILDDIR)
	gcc $(SOURCES) -lncurses -o $(BUILDDIR)mines

clean:
	rm -r $(BUILDDIR)
