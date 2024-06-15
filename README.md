# A Simple DLL Injector

This is a simple DLL injector designed to inject a specified DLL module into a given process as a thread Written in C
(standard c99), please see the usage below.

This is specifically for Windows use.


## Usage

Inject a DLL module into a specific process ID (PID).

```powershell
              # <PID> <DLL Module>
dllinjector.exe 23212 mymodule.dll
```


## Build

If you have GNU make installed, just simply type `make` in a shell instance in the source directory, this will produce
a binary in the ./build directory.

Without make, you can type the following (swapping out clang for your compiler of choice):

```powershell
clang -std=c99 -Wall -Wextra main.c -o dllinjector.exe
```


## To-dos

- Configuration through an INI file.
- Automatic function for getting a process ID by name.
