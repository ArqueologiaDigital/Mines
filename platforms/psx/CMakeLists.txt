cmake_minimum_required(VERSION 3.20)

project(
	Arqueologia-Minesweeper
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "Minesweeper Arqueologia Digital"
	HOMEPAGE_URL ""
)

# Screen resolution macros
add_compile_definitions(MINEFIELD_X_OFFSET=6)
add_compile_definitions(MINEFIELD_Y_OFFSET=5)
# Other macros
add_compile_definitions(USE_DEBUG_MODE=true)
add_compile_definitions(DRAW_TITLE_SCREEN=true)
add_compile_definitions(MINEFIELD_ALLOCATION=overwrite)
add_compile_definitions(MAIN_LOOP_REIMPLEMENTED=true)

# Files to be included
file(GLOB _sources ../../common/*.c;*.c)
message("FILES ${_sources}")
psn00bsdk_add_executable(mines STATIC ${_sources})

psn00bsdk_add_cd_image(
	iso      # Target name
	mines    # Output file name (= mines.bin + mines.cue)
	iso.xml  # Path to config file
	DEPENDS mines
)
