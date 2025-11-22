# ___________

A simple [raylib](https://www.raylib.com/index.html)-based implementation of ______________.

## Compiling and Running

Instructions assume you are (1) using Linux or macOS and (2) that you have [gcc (C99)](https://gcc.gnu.org/), [CMake](https://cmake.org/), and [git](https://git-scm.com/) installed. 

```bash
git clone _________________________
cd _________________
./setup.sh
```

[./setup.sh](./setup.sh) makes a `build` directory and makes an executable `build/bin/project`.

If you want to make changes and run, then from the `build` directory, run `cmake --build .` to build a new executable, then `./bin/project` to run it. Also, if you add new source files, you should run `cmake ..` before `cmake --build .`.
