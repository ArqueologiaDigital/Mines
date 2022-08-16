all: mines.html common-ref.html game-ref.html msx2-ref.html msdos-ref.html ncurses-ref.html opengl-ref.html gunsmoke-ref.html

mines.md: ../README.md
	./fixurl.py "platforms/([^/]+)/README.md" "\1-ref.html" $< > $@

mines.html: mines.md footer.md header.html
	cat footer.md >> mines.md
	pandoc -s -t html5 -c water.css -c monokai.css --toc --no-highlight -B header.html $< -o $@

common-ref.html: ../common/common.h hdoc.py footer.md header.html
	./hdoc.py --footer footer.md "Common library reference" < $< > common-ref.md
	pandoc -s -t html5 -c water.css -c monokai.css --toc --no-highlight -F pandocfilter-pygments.py -B header.html common-ref.md -o $@

game-ref.html: ../common/game.h hdoc.py footer.md header.html
	./hdoc.py --footer footer.md "Game library reference" < $< > game-ref.md
	pandoc -s -t html5 -c water.css -c monokai.css --toc --no-highlight -F pandocfilter-pygments.py -B header.html game-ref.md -o $@

msx2-ref.md: ../platforms/msx2/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

msx2-ref.html: msx2-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --toc --no-highlight -B header.html $< -o $@

msdos-ref.md: ../platforms/msdos/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

msdos-ref.html: msdos-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --toc --no-highlight -B header.html $< -o $@

ncurses-ref.md: ../platforms/ncurses/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

ncurses-ref.html: ncurses-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --toc --no-highlight -B header.html $< -o $@

opengl-ref.md: ../platforms/opengl/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

opengl-ref.html: opengl-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --toc --no-highlight -B header.html $< -o $@

gunsmoke-ref.md: ../platforms/gunsmoke/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

gunsmoke-ref.html: gunsmoke-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --toc --no-highlight -B header.html $< -o $@

clean:
	rm -f *-ref.html *-ref.md mines.md mines.html

.PHONY: clean all
