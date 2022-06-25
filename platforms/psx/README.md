# PSX Port

Using [PSn00bSDK](https://github.com/Lameguy64/PSn00bSDK).

## Installing the SDK

**Do it preferably on your /tmp**
**Steps written for Ubuntu/Debian**

- If you're on Ubuntu/Debian, run this to install deps

```sh
# Install deps
sudo apt install git build-essential make

# Warning: this will uninstall your current version of cmake! 

sudo apt purge cmake

# Installing cmake

wget https://github.com/Kitware/CMake/releases/download/v3.24.0-rc2/cmake-3.24.0-rc2-linux-x86_64.tar.gz
tar xzvf cmake-3.24.0-rc2-linux-x86_64.tar.gz
sudo cp cmake-3.24.0-rc2-linux-x86_64/bin/* /usr/local/bin
sudo cp -R cmake-3.24.0-rc2-linux-x86_64/share/* /usr/local/share

# Testing cmake
cmake --version
```

- Install mipsel

```sh
sudo mkdir /usr/local/mipsel-none-elf
cp /usr/local/mipsel-none-elf
sudo wget https://github.com/Lameguy64/PSn00bSDK/releases/download/v0.19/gcc-mipsel-none-elf-11.1.0-linux.zip
sudo unzip gcc-mipsel-none-elf-11.1.0-linux.zip
```

- Clone the repo for [PSn00bSDK](https://github.com/Lameguy64/PSn00bSDK) and init submodules and compile/install

```sh
git submodule update --init --recursive
cmake --preset default .
cmake --build ./build

# Don't worry, this will install to /usr/local
sudo cmake --install ./build
```

## Installing the emulator

Download the [pcsx-redux](https://github.com/grumpycoders/pcsx-redux) emulator from [here](https://install.appcenter.ms/orgs/grumpycoders/apps/pcsx-redux-linux64/distribution_groups/public)

Unzip and then run the .AppImage file.

## Building the project

```sh
export PSN00BSDK_LIBS=/usr/local/lib/libpsn00b/
make build
```

You can then open PCSX-Redux, click on File > Open ISO, then select the file on build/mines.bin. Then, click on Emulation > Run.

You can also burn the build to a CD or load it on your favorite emulator.
