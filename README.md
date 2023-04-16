# Cala
3D Rendering framework written in C++ currently supporting OpenGL only.

---

## Building instructions
### Requirements:
* C++ toolchain
* Modern CMake (version 3.0.0 or higher)
* OpenGL 4.3 or higher

### Cloning:
Cala is using git submodules so first clone the repository with:

```
git clone --recurse-submodules https://github.com/dominikcondric/Cala.git
```

### Building:
Cala is using CMake to simplify development of Cala using different build systems. To generate project files it is advised to create a new directory to where cmake will generate project files:
```
mkdir build
cd build
cmake ..
```

In case you don't want to build demo, set *BUILD_CALA_DEMO* option to *OFF*.

*\* If you want to use different build system than the one chosen by CMake, use -G flag with build system in quotes (You can find the list of supported build systems using cmake --help).*

---

## FAQ
### Why did you write this framework?
Primarily for learning purposes. At least it started like that. Current idea is to make a framework that is easy to use to avoid making my future graphics projects from scratch. Maybe it will become part of something even cooler in the future.

### What are the main features of Cala?
Some of the things Cala can do are:
* Basic 3D rendering of arbitrary meshes, both single-colored or textured
* Normal mapping
* Blinn-Phong lighting model with multiple lights of different kind (directional, point, spotlights)
* Dynamic shadows
* Basic post processing (blurs, bloom, negative, hdr)

### Which graphics APIs Cala supports?
For now only OpenGL, but Calas Rendering System is written in the way that it supports simple addition of other APIs. Adding Vulkan and Direct3D support is planned for the future.

### Which operating systems Cala supports?
Windows and Linux for sure, haven't tested on MacOS yet.

### Why Cala?
Cala is an elvish (*Quenya*) word for light. I love Tolkien's legendarium and I found this name especially cool and beautiful. 
