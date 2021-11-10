# Cala
Rendering engine  with a hope of becoming a complete game engine.  

---

## Building instructions
### Requirements:
* C++ toolchain
* Modern CMake (version 3.0.0 or higher)
* OpenGL 3.3 or higher

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

*\* If you want to use different build system than the one chosen by CMake, use -G flag with build system in quotes (You can find the list of supported build systems using cmake --help).*

---

## FAQ
### What does the engine do?
For now Cala is only a basic rendering engine with lightweight ECS. It also supports native scripting and should support some basic physics soon. The overall plan is to develop have all things necessary to make a game in the future including audio system, AI system, networking etc., but there is also a lot more to work on rendering as well.

### Which graphics APIs Cala supports?
For now only OpenGL, but Calas Rendering System is written in the way that it supports simple addition of other APIs. Adding Vulkan and Direct3D support is planned for the future.

### Which operating systems Cala supports?
Most of the code is working on MacOS, Linux and Windows but not all three are fully supported yet. For now Windows is the only one fully supported. 

### Why did you write your own ECS where there are a number of great ECS libraries?
For the same reason I am writing this game engine and that is learning and developing my programming and engineering skills. It may not be the best ECS out there, but I wrote it, learnt a lot about ECS, and that was the idea.

### Why Cala and Calion?
Cala is an elvish (*Quenya*) word for "light". Similar to that Calion is an elvish (*Quenya*) word for "son of light". I love Tolkien's legendarium and I found this combination of names especially cool and beautiful. 
