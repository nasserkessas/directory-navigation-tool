# nv - Directory save/navigation tool

A program written in c to save and navigate directories easily. Directories can be saved with an access name to be navigated to at a later time. Directories and names are saved in a `.nvsave` file in the user's home directory.

## Run

```sh
$ nv
```

## Install

Compile:
```sh
gcc -o nv main.c
```

Copy to /usr/bin (to exectute like `$ nv`)
```sh
cp ./nv /usr/bin
```

Change file permissions
```sh
chmod 755 /usr/bin/nv
```

## Usage

