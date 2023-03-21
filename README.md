# nv - Directory save/navigation tool

A program written in c to save and navigate directories easily. Directories can be saved with an access name to be navigated to at a later time. Directories and names are saved in a `.nvsave` file in the user's home directory.

## Run

```sh
$ nv
```

## Install

Compile:
```sh
gcc -o bin/nv main.c
```

Copy to /usr/bin (to exectute like `$ nv`)
```sh
cp ./bin/nv /usr/bin
```

Change file permissions
```sh
chmod 755 /usr/bin/nv
```

## Usage

### `store`

Stores current directory with an incremented number as the key

#### Options

 + `-a "NAME"` Adds the current directory with NAME as the key
 <br>
 &emsp;`--set-default`: Sets added directory to default position
 + `-d` "NAME": Deletes directory with key NAME
 + `-l`: Lists all stored directories and keys
 + `-f`: Prints save file location
<br>

### `move`

Changes directory to default or first listed directory

#### Options

 + `-n "NAME"`  Changes directory to stored value with key NAME

## Todo
- Convert to bash script?

## Acknoledgements
Created by Nasser Kessas

## Contributing
PRs accepted, feel free to dive in! [Open an issue](https://github.com/nasserkessas/directory-navigation-tool/issues/new) or submit PRs.

Directory Navigation Tool (`nv`) follows the [Contributor Covenant](http://contributor-covenant.org/version/1/3/0/) Code of Conduct.
