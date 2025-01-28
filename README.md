# Eriks-Wasp-Sim
This is an unrealistic wasp colony simulator that I mostly use to experiment with computational optimization.

## Dependencies
If you just want to run the program, clone the repo and use the [executable](Eriks_Wasp_Sim/build/vs2022/x64/Debug/Eriks_Wasp_Sim.exe) in the '/Eriks_Wasp_Sim/build/vs2022/x64/Debug/' directory.

The executable will not work if moved out of its directory.


If you want to compile the source code, you need to download the following dependencies and put them into the '/Eriks_Wasp_Sim/dependencies' directory:

- [tinyobjloader v1.0.6](https://github.com/tinyobjloader/tinyobjloader/releases/tag/v1.0.6): Put 'tiny_obj_loader.h' into '/Eriks_Wasp_Sim/dependencies'
- [glew-2.1.0](https://sourceforge.net/projects/glew/files/glew/2.1.0/): Put the 'glew-2.1.0' directory into '/Eriks_Wasp_Sim/dependencies'
- [freeglut](https://www.transmissionzero.co.uk/software/freeglut-devel/): Download the MinGW Package and then put the 'freeglut' directory into '/Eriks_Wasp_Sim/dependencies'
- [glm v1.0.0](https://github.com/g-truc/glm/releases): Download the glm folder and put it into '/Eriks_Wasp_Sim/dependencies'
- [nlohmann-json v3.11.3](https://github.com/nlohmann/json/releases): Download the json.hpp file and put it into '/Eriks_Wasp_Sim/dependencies'
- [imgui-1.90.4](https://github.com/ocornut/imgui/releases/tag/v1.90.4): Download the imgui folder and put it into '/Eriks_Wasp_Sim/dependencies'

## Commands
The Wasp-Sim uses a CLI that opens alongside the OpenGL window at startup. Type 'help' to see a list of available commands or look through [commands.json](Assets/Commands/Commands.json) for more information.

Each command has the following attributes: 
- Name and explanation, which are printed by the 'help' command
- Syntax, which can be displayed by typing ```syntax [command] {subcommand}*``` (e.g., ```syntax wasp sethp```)
- List of subcommands (e.g., 'kill', 'sethp', ... for the 'wasp' command)

Sometimes 'syntax' makes use of command elements (e.g., \<position\> in ```wasp setpos <position>```). These are placeholders that might need a more specific explanation. 
By using the 'element' command (e.g., ```element position```) you can print that explanation.

## Controls
#### Movement
- Use [W,A,S,D] to move
- Use the arrow keys to rotate the camera.

#### UI
- Use your mouse to interact with (and optionally move/scale) the GUI
- Left-click a wasp with your mouse to select it
- Press 'p' to pause/unpause the simulation loop

## Performance notes
The Wasp-Sim makes use of multiple performance optimizations, some of which have a difficult tradeoff balance. Here are a few of them along with some notes:
* **Hardware Instancing** for rendering wasp and food entities.
* **Memory/Cache locality**: Wasp and food entities are kept in a vector of fixed-size and are all initialized at startup. Spawning and killing them simply means activating/deactivating the already initialized objects.
  This makes loops over large amounts of objects faster due to high levels of locality but comes with the downsides of fixed-size vectors.
* **Max indices**:
  - To reduce the downsides caused by the fixed-size vector optimization the Wasp-Sim makes use of variables that keep track of the highest index of activated objects within the vector.
  These indices are then used to limit loops over the vector (i.e., if the wasp vector has a size of 100000 but only the first 5000 wasps are alive, wasp_maxIndex is set to 5000 and the other slots are not iterated over).
  While this has huge performance benefits it comes with the added responsibility of managing these indices carefully.
  - Whenever objects are de-/reactivated the indices should be updated properly. While
  this is easy when these actions occur in a centralized manner (e.g., through the 'kill' or 'spawn' commands), decentralized deactivations like wasps deactivating because their hp reach 0 could potentially cause
  a lot of redundant loop iterations if they all make a call to the index update procedure. This problem is addressed by running this procedure on a fixed schedule every few seconds, and only making extra calls to it
  whenever commands like the ones mentioned before are used.
  - To make sure the max indices are always as small as possible (while being unable to reorder elements within the vector), procedures that deactivate entities (e.g., the 'kill' command) should, if possible,
    kill entities at higher indices first. If there are exactly 100000 wasps, the max index is 100000 and ```kill wasp 95000``` is executed, the upper 95000 wasps should be killed so that the max index can be set to 5000.
  - There are some situations in which the max index can not be reduced as much as one would like. For example, if half of all wasps fail to find food, starve and are then deactivated, it is very unlikely that all of their
    indices line up perfectly for the max index to be cut in half as well.
* **Threading**: Not only does the OpenGL code run on a different thread than the simulation code, both of these threads also occasionally spawn more child threads.
  - The cpu needs to collect matrix information before sending it off to the gpu for hardware instancing. This is done with multiple threads looping over sections of the aforementioned localized entity vectors.
    Usually, dividing the task into 4 threads yields the best results, but that is highly situational.
  - Threading is achieved using a custom [ThreadPool](Eriks_Wasp_Sim/include/util/ThreadPool.h) class. Instead of creating new threads each time they are used, a pool of threads is constantly waiting (BLOCKED) for new tasks
    that can be enqueued
  - For now it seems that multithreading is hugely benefitial in the rendering loop but has mixed results in the simulation loop. Under most circumstances a multithreaded simulation loop is a tiny bit slower due to the
    overhead of the thread pool class. This is also made more noticable due to the demand for extra threads having an influence on the framerate of the render loop as well (whereas the single thread approach only results in slower
    simulation update speeds and thereby choppy wasp movement). However, when the computational load of the simulation gets very high (like when there are a lot of food entities and a lot of wasp objects at the same time and
    each wasp is searching the list for the closest food object), the multithreaded approach is way faster and ends up looking a lot smoother. Sadly, dynamically scaling the size of the thread pool does not seem to help. It seems
    to be all or nothing.
