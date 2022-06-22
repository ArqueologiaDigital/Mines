## Vectrex port

- Tested with MAME (you can use the `make run` for that)

![screenshot](screenshot.png)

### Build & runtime/debug dependencies

- cd devel/
- mkdir gitlab_dfffffff_gcc6809
- cd gitlab_dfffffff_gcc6809
- git clone https://gitlab.com/dfffffff/gcc6809.git
- cd gcc6809/     ; note: default branch is "dftools"
- cd build-6809/
- sudo apt install libgmp-dev libmpfr-dev
- make everything

- cd cd gitlab_dfffffff_gcc6809
- git clone https://gitlab.com/dfffffff/newlib-6809.git
- cd newlib-6809/
- cd build/
- ; edit the file `newlib.6809` substituting `target:-m6809-sim-none` by `target:-m6809-unknown-none`
- ; and also substituting `target_arg:-m6809sim` by `target_arg:-m6809unknown`
- ./newlib.6809 config  ; note: configure: WARNING: unrecognized options: --enable-multilib, --disable-malloc-debugging, --with-gnu-ld, --without-pic, --enable-target-optspace, --enable-newlib-iconv
- ./newlib.6809 make
- sudo ./newlib.6809 install
