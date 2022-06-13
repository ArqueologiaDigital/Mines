# Mines
A MineSweeper clone implemented for many home consoles and computers.

Licensed under GNU General Public License version 2 (or later)

### Existing ports in this repo

For specific build & runtime/debug dependencies, please check the individual README.md files for each of the available ports below:

- [ncurses](platforms/ncurses/README.md) (partially working)
- [GunSmoke JAMMA board](platforms/gunsmoke/README.md) (partially working)
- [MSX2](platforms/msx2/README.md) (partially working)
- [C64](platforms/c64/README.md) (boilerplate)

### How to create your own

- Create a new subdirectory in `platforms` named after the new platform;
- Create at least one source file (`.c` suffix) to put your platform-specific functions and macros;
  - You might want to separate functions in files `input.c` and `video.c` (according to purpose) as commonly found in other platforms;
- Add `common.h` (for generic definitions) and `game.h` (for more specific game definitions) to your source files using the `#include` macro;
- Implement all functions and macros defined in `common.h` (follow the instructions for each in the file);
- Implement all functions and macros defined in `game.h` (follow the instructions for each in the file);

You can compare you implementation with a preexisting one if you are stuck. [ncurses](platforms/ncurses) should be easy to follow.
