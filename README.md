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

Automatically get the PID and inject for a specific exe currently running.

```powershell
              # <flag> <PID> <DLL Module>
dllinjector.exe auto foo.exe mymodule.dll
```


## Build

Requires GNU make. Just simply type `make` in a shell instance in the source directory, this will produce
a binary in the ./build directory.


## To-dos

- Configuration through an INI file.
- <del>Automatic function for getting a process ID by name.</del>
