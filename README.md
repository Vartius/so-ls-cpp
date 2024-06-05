# Custom `ls` Command
![Mood](https://img.shields.io/badge/mood-happy-brightgreen) ![Skill Level](https://img.shields.io/badge/skill-beginner-lightgrey) ![Pizza Lover](https://img.shields.io/badge/pizza-lover-ff69b4) ![Coffee Drinker](https://img.shields.io/badge/coffee-needed-9cf) ![Works on My Machine](https://img.shields.io/badge/works-on_my_machine-green)

This is a custom implementation of the `ls` command in C. It provides various functionalities similar to the UNIX `ls` command, such as displaying files in a directory, with options for long listing format, recursive listing, hidden files, human-readable sizes, and sorting by size or modification time.

## Features

- **Long Listing Format**: Display detailed information about files.
- **Recursive Listing**: List directories and their contents recursively.
- **Hidden Files**: Include hidden files in the listing.
- **Human-Readable Sizes**: Display file sizes in human-readable format (e.g., 1K, 234M, 2G).
- **Sorting**: Sort files by size or modification time.
- **Color-Coding**: Color-coded output for directories, executables, and regular files.

## Usage

```sh
./main [options] [path]
```

### Options

- `-l` : Use a long listing format.
- `-R` : List subdirectories recursively.
- `-a` : Include hidden files.
- `-h` : Human-readable sizes (e.g., 1K, 234M, 2G).
- `-S` : Sort by file size.
- `-t` : Sort by modification time.
- `--help` : Display this help and exit.
- `--version` : Output version information and exit.

### Examples

- **Basic usage**: List files in the current directory.
  ```sh
  ./main
  ```

- **Long listing format**:
  ```sh
  ./main -l
  ```

- **Recursive listing**:
  ```sh
  ./main -R
  ```

- **Include hidden files**:
  ```sh
  ./main -a
  ```

- **Human-readable sizes**:
  ```sh
  ./main -h
  ```

- **Sort by size**:
  ```sh
  ./main -S
  ```

- **Sort by modification time**:
  ```sh
  ./main -t
  ```

- **Combination of options**: Long listing format, include hidden files, and human-readable sizes.
  ```sh
  ./main -lah
  ```

- **Help and Version**:
  ```sh
  ./main --help
  ./main --version
  ```

## Compilation

This project uses CMake for building. Ensure you have CMake version 3.10 or higher.

### Steps to Compile

1. Create a `build` directory.
2. Run CMake to configure the project.
3. Compile the project.

```sh
mkdir build
cd build
cmake ..
make
```

## License

This program is open-source and available under the MIT License.

## Author

- Vartius
- mkasporpol@gmail.com
- https://github.com/Vartius

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue on GitHub.

## Acknowledgments

Inspired by the UNIX `ls` command.