# Security Guard

## Overview

A *secure log*, to describe the *state of an institute*: the guests and employees who have entered and left, and persons that are in campus in different buildings or rooms, is implemented in this project. The log will be used by *two programs*. One program, `logappend`, will append new information to this file, and the other, `logread`, will read from the file and display the state of the institute according to a given query over the log. Both programs will use an authentication token, supplied as a command-line argument, to authenticate each other. Specifications for these two programs are described in more detail here: [`logappend`](LOGAPPEND.md) and [`logread`](LOGREAD.md).

The project uses the `libsodium` library for cryptographic operations and `nlohmann-json` for JSON parsing.


## Programs

 * The [`logappend`](LOGAPPEND.md) program appends data to a log
 * The [`logread`](LOGREAD.md) program reads and queries data from the log


## Project Structure

The project directory structure is as follows:

All the source files and `Makefile` are located in the `build` directory. After building, the executables `logappend` and `logread` will be created within the `build` directory.

## Prerequisites

This project requires:
1. `libsodium` - for cryptographic functions.
2. `nlohmann-json` - for JSON parsing.

### Installing libsodium and nlohmann-json

#### Linux (Debian/Ubuntu)
```bash
sudo apt update
sudo apt install libsodium-dev
sudo apt install nlohmann-json3-dev
```
If you are a root user omit `sudo` from above commands.

#### Mac

```bash 
brew install libsodium
brew install nlohmann-json
```

#### Windows
For this project, we recommend setting up the required libraries (`libsodium` and `nlohmann-json`) using MSYS2 due to its simplicity of setup:
* Ensure that MSYS2 is installed on your system.
* Open the MSYS2 terminal and run:
    ```bash 
    pacman -Syu
    ```
* Install `libsodium`:
    ```bash
    pacman -S mingw-w64-x86_64-libsodium
    ```
* Install `nlohmann-json`:
    ```bash
    pacman -S mingw-w64-x86_64-nlohmann-json
    ```
* Compilation Instructions:
    To compile the project (before running `make`), ensure the correct include and library paths are specified for your compiler. The following flags are used:

    - **Include Path (`-I`)**: Specifies the directory for header files required by the project. (For exapmle, "C:/msys64/mingw64/include" for MinGW64)
    - **Library Path (`-L`)**: Specifies the directory containing the necessary libraries. (For example, "C:/msys64/mingw64/lib" for MinGW64)


### Building the Project

To build the project, follow these steps:

1. Navigate to the build directory:

```bash
cd build
```

2.Run make to compile the executables:
```bash
make
```
This will create two executable files in the build directory:

`logappend`
`logread`

### Running the Programs
After building the project, you can run the executables from the build directory.

Running logappend
```bash
./logappend [options]

```
Running logread
```bash
./logread [options]

```
Details about [options] is provided here: [`logappend`](LOGAPPEND.md) and [`logread`](LOGREAD.md).