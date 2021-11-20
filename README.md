# Path Tracing

## Description

This is a path tracing implementation adapted from [smallpt](https://www.kevinbeason.com/smallpt/).

The following features are implemented:
- scene configuration
- loading of arbitrary mesh object
- BVH acceleration for ray-mesh intersection
- texture sampling

See `example.yml` for the detail of configuration.

## Compilation

Run `make`. CMake is also required. In Windows, you need to compile under MSYS2 or WSL.

## Usage

Run `./main [CONFIG]`, where `CONFIG` is the configuration file in YAML format, default to `config.yml`.
