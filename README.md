# Pixelboy

Pixelboy is a GameBoy emulator written in modern(ish) C++.  All official opcodes are supported, and most of Blargg's tests pass.
It has a tiny binary file size, at just 151kb (Windows 64-bit in release mode). Pixelboy also has an in-built debugger.
**Game support is lacking**, owing to the project's lack of development, and **Audio is not yet supported**.

## Screenshots
<img width=40% align=right src="https://raw.githubusercontent.com/TheUltimateKerbonaut/Pixelboy/master/Screenshots/WindowsMario.png" alt="Mario screenshot on Windows">
<img width=40% src="https://raw.githubusercontent.com/TheUltimateKerbonaut/Pixelboy/master/Screenshots/WindowsTetris.png" alt="Tetris screenshot on Windows">
<img width=40% src="https://raw.githubusercontent.com/TheUltimateKerbonaut/Pixelboy/master/Screenshots/Linux.png" alt="Tetris screenshot on Linux">

## Supported games
* **Tetris** (World) (Rev A)
* **Super Mario Land** (World)

## Debugger
Pixelboy has an in-built debugger, which is included when compiled under debug mode (see below).
<img width=70% src="https://raw.githubusercontent.com/TheUltimateKerbonaut/Pixelboy/master/Screenshots/WindowsDebug.png" alt="Mario screenshot on Windows with debugger">

## Usage
* On Windows, compile by opening the solution in Visual Studio then building. If you wish to include the in-built debugger, compile under debug mode.
* On Linux, run these commands (for debug mode, append a `#define _DEBUG 1` to the top of main.cpp):
```
cd Pixelboy
g++ -o Pixelboy *.cpp *.c -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -fpermissive
./Pixelboy
```
* On MacOS, which at the time of writing only has experimental support, do the following:
```
cd Pixelboy
clang++ -arch x86_64 -std=c++17 -mmacosx-version-min=10.15 -Wall -framework OpenGL -framework GLUT -lpng *.cpp -o Pixelboy
./Pixelboy
```
Simply run the application and select the ROM which you would like to run. 

## Supported platforms
* Windows builds on Visual Studio with minimal effort and runs perfectly
* Full Linux support too (except for a bug with V-Sync)
* MacOS works, but is experimental

## Dependencies
All dependencies are included with the source code. These include:
* [Pixel Game Engine](https://github.com/OneLoneCoder/olcPixelGameEngine) - graphics and input
* [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/)

## Blargg's instruction tests

|#|name|state|
|-|-|-|
|01|special|:white_check_mark:|
|02|interrupts|:white_check_mark:|
|03|op sp,hl|:white_check_mark:|
|04|op r,imm|:white_check_mark:|
|05|op rp|:white_check_mark:|
|06|ld r,r|:white_check_mark:|
|07|jr,jp,call,ret,rst|:white_check_mark:|
|08|misc instrs|:white_check_mark:|
|09|op r,r|:white_check_mark:|
|10|bit ops|:white_check_mark:|
|11|op a,(hl)|:white_check_mark:|

Memory Timing:

|#|access type|state|
|-|-|-|
|1|read|:x:|
|2|write|:x:|
|3|modify|:x:|

|name|state|
|-|-|
|instr_timing|:x:|
|halt_bug|:x:|
