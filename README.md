# Directory Watcher 📁

This is a simple daemon for making logs of changes made in a directory.
I'm still working on this project, so it's still under development :)

## Usage

Requires c++20.
```bash
g++ -std=c++20 directory_watcher_daemon.cpp -o watch
```

Usage:

```bash
./watch path/to/watch
```
## Features

Currently this application logs the changes for 'File created', 'File modified', 'File deleted'.
