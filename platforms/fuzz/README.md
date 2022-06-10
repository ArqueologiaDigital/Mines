## fuzzer target

This fuzzes the game with libfuzzer/fuzzer sanitizer.

Input contains letters, each one specifying a key that the
game engine should handle:

* L: move left
* R: move right
* U: move up
* D: move down
* B: open block
* F: mark flag
* Q: quit
* \n: open

Save a list of commands in a text file and pass it to the built
executable.  If the environment variable `ASAN_OPTIONS` is set to
`abort_on_error=1`, you can easily use a debugger like gdb to figure
out what happened where.

### Build & runtime/debug dependencies

* clang compiler
