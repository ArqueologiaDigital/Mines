all: mines.html common-ref.html game-ref.html msx2-ref.html msdos-ref.html ncurses-ref.html opengl-ref.html gunsmoke-ref.html psx-ref.html vectrex-ref.html c64-ref.html

mines.tmp: ../README.md
	./fixurl.py "platforms/([^/]+)/README.md" "\1-ref.html" $< > $@

mines.md: mines.tmp
	./fixurl.py "platforms/ncurses" "ncurses-ref.html" $< > $@

mines.html: mines.md footer.md header.html
	cat footer.md >> mines.md
	pandoc -s -t html5 -c water.css -c monokai.css --no-highlight -B header.html --metadata title:"Mines documentation" -V title:"" $< -o $@

common-ref.html: ../common/common.h hdoc.py footer.md header.html
	./hdoc.py --footer footer.md "Common library reference" < $< > common-ref.md
	pandoc -s -t html5 -c water.css -c monokai.css --toc --no-highlight -F pandocfilter-pygments.py -B header.html common-ref.md -o $@

game-ref.html: ../common/game.h hdoc.py footer.md header.html
	./hdoc.py --footer footer.md "Game library reference" < $< > game-ref.md
	pandoc -s -t html5 -c water.css -c monokai.css --toc --no-highlight -F pandocfilter-pygments.py -B header.html --metadata title:"Game library reference" game-ref.md -o $@

msx2-ref.md: ../platforms/msx2/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

msx2-ref.html: msx2-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --no-highlight -B header.html --metadata title="MSX2 port" -V title:"" $< -o $@

msdos-ref.md: ../platforms/msdos/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

msdos-ref.html: msdos-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --no-highlight -B header.html --metadata title="MSDOS port" -V title:"" $< -o $@

ncurses-ref.md: ../platforms/ncurses/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

ncurses-ref.html: ncurses-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --no-highlight -B header.html --metadata title="ncurses port" -V title:"" $< -o $@

opengl-ref.md: ../platforms/opengl/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

opengl-ref.html: opengl-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --no-highlight -B header.html --metadata title="OpenGL port" -V title:"" $< -o $@

gunsmoke-ref.md: ../platforms/gunsmoke/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

gunsmoke-ref.html: gunsmoke-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --no-highlight -B header.html --metadata title="Gunsmoke port" -V title:"" $< -o $@

psx-ref.md: ../platforms/psx/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

psx-ref.html: psx-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --no-highlight -B header.html --metadata title="Playstation port" -V title:"" $< -o $@

vectrex-ref.md: ../platforms/vectrex/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

vectrex-ref.html: vectrex-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --no-highlight -B header.html --metadata title="Vectrex port" -V title:"" $< -o $@

c64-ref.md: ../platforms/c64/README.md footer.md
	./fixurl.py "../../docs/" "" $< > $@
	cat footer.md >> $@

c64-ref.html: c64-ref.md footer.md header.html
	pandoc -s -t html5 -c water.css -c monokai.css --no-highlight -B header.html --metadata title="Commodore 64 port" -V title:"" $< -o $@

clean:
	rm -f *.tmp *-ref.html *-ref.md mines.md mines.html

.PHONY: clean all
