.PHONY: build run

build:
	cmake --preset default .
	cmake --build ./build

run:
	pcsx-redux -run -loadiso build/mines.bin -stdout -debugger
