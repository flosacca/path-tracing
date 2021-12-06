# Path Tracing

## Description

This is a path tracing implementation adapted from [smallpt](https://www.kevinbeason.com/smallpt/).

The following features are implemented:
- support of mesh objects
- arbitrary scene settings
- various configurable options
- BVH acceleration for ray-mesh intersection
- texture sampling
- normal interpolation

## Compilation

Run `make` in base directory. CMake is also required for building a dependency.

If AVX2 and FMA are not supported, you should remove flags `-mavx2` and `-mfma` in `Makefile`.

In Windows, you need to compile under MSYS2 or WSL.

## Usage

Run `./main [CONFIG]`, where `CONFIG` is the configuration file in YAML format, default to `config.yml`.

See `example.yml` for the detail of configuration.
