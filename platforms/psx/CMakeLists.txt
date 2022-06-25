# PSn00bSDK example CMake script
# (C) 2021 spicyjpeg - MPL licensed

cmake_minimum_required(VERSION 3.20)

project(
	Arqueologia-Minesweeper
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "Minesweeper Arqueologia Digital"
	HOMEPAGE_URL ""
)

psn00bsdk_add_executable(mines STATIC main.c)

psn00bsdk_add_cd_image(
	iso      # Target name
	mines    # Output file name (= template.bin + template.cue)
	iso.xml  # Path to config file
	DEPENDS mines
)
