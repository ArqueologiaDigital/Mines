## MSX2 port

![Screenshot](screenshot.jpg)

### Keys

* The arrow keys move the cursor;
* Space key opens the current cell under the cursor;
* `M` key toggles flag and question mark;
* `N` key opens multiple cells at once around the cursor;
* `Q` key reboots;

### Build & runtime/debug dependencies

* SDCC compiler (successfully tested with 4.0.0 and 4.1.0)
* hex2bin tool
* openMSX emulator with C-BIOS ROM

**Note:** The hex2bin tool can be compiled from sources available at https://gitlab.com/reidrac/ubox-msx-lib/-/blob/main/tools/hex2bin-2.0 or you can also use binaries downloaded from https://sourceforge.net/projects/hex2bin/files/
