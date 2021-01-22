# Computer graphics in Game development course

This repo contains a template for Computer graphics in Game development labs (spring 2021)

## Pre requirements

- [Premake5](https://premake.github.io/download.html#v5)
- [Visual studio 2019 Community](https://visualstudio.microsoft.com/ru/vs/community/) or another version

For convenience, I'd like to recommend installing the next extensions for Visual Studio:

- [SwitchStartupProject for VS 2019](https://marketplace.visualstudio.com/items?itemName=vs-publisher-141975.SwitchStartupProjectForVS2019)
- [Smart Command Line Arguments](https://marketplace.visualstudio.com/items?itemName=MBulli.SmartCommandlineArguments)

## How to prepare Visual Studio solution

Run `git submodule update --init --recursive` after the first clone

Go to the project folder and run:

```sh
premake5 vs2019
```

## Third-party tools and data

- [STB](https://github.com/nothings/stb) by Sean Barrett (Public Domain)
- [Linalg.h](https://github.com/sgorsten/linalg) by Sterling Orsten (The Unlicense)
- [Tinyobjloader](https://github.com/syoyo/tinyobjloader) by Syoyo Fujita (MIT License)
- [Catch2](https://github.com/catchorg/Catch2) by Phil Nash (BSL-1.0 License)
- [Cxxopts](https://github.com/jarro2783/cxxopts) by jarro2783 (MIT License)
- [Cornell Box models](https://casual-effects.com/g3d/data10/index.html#) by Morgan McGuire (CC BY 3.0 License)
- [Cube model](https://casual-effects.com/g3d/data10/index.html#) by Morgan McGuire (CC BY 3.0 License)
