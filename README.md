# File Manager – CS3307 Assignment

**Author**: Parneet Baidwan
**Date**: February 3, 2026  

## Description

This is a simple file manager built using C++ and wxWidgets. It allows users to browse directories and perform basic file operations that include:

- Opening files
- Renaming, deleting, copying, and moving files
- Creating new directories
- Refreshing the current view
- Navigating directories by a text bar

## Features

- Built with wxWidgets 3.2+
- Cross-platform support on Linux/Mac via wxWidgets
- Uses C++17 filesystem library
- GUI includes file listing, menu bar, status bar, and dialogs

## Dependencies

Make sure the following are installed on your system:

- `wxWidgets` version 3.0 or higher (recommended: 3.2+)
- `g++` version 7 or later (C++17 support required)
- `make` (build tool)
- `wx-config` (comes with wxWidgets)

You can check if `wx-config` is available with:

```bash
which wx-config
```

## Building

To build the file manager, open a terminal in the project directory and run:

```bash
make
```

If your system cannot find `wx-config`, edit the `Makefile` and manually specify its path:

```make
WX_CONFIG = /usr/local/bin/wx-config
```

This will compile the program using the proper C++17 and wxWidgets flags.

## Running

After building, run the application with:

```bash
./filemanager
```

Make sure you are in a Unix-like environment with graphical support (e.g., Linux desktop, WSL with X server, or XQuartz on macOS).

## Notes

- Only one file is operated on at a time
- File operations use a virtual clipboard
- Double-clicking a folder navigates into it
- Supports keyboard shortcuts and right-click menus
