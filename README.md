# Eriks-Wasp-Sim
This is an unrealistic wasp colony simulator that I mostly use to experiment with computational optimization.

## Build instructions

#### Windows
If you just want to run the program, clone the repo and use the [executable](Eriks_Wasp_Sim/build/vs2022/x64/Debug/Eriks_Wasp_Sim.exe) in the '/Eriks_Wasp_Sim/build/vs2022/x64/Debug/' directory. Do not move the executable to another folder, otherwise it will not work

If you want to build manually, use the Visual Studio Solution in the [vs2022](Eriks_Wasp_Sim/build/vs2022) folder. 
You will also need to follow the instructions in the [Dependencies](#Dependencies) subsection before building.

#### Linux
1. Install the required OpenGL libraries:
    ```
    sudo apt install libgl1-mesa-dev libglew-dev freeglut3-dev libglfw3-dev libglm-dev
    ```

2. Follow the instructions in the [Dependencies](#Dependencies) subsection. Skip glew, freeglut and glm, as they were already installed in step 1.
3. Open the [build/cmake](Eriks_Wasp_Sim/build/cmake) directory and run the following commands:
    ```
    mkdir build; cd build; cmake ..
    ```

    ```
    make
    ```

4. Run the executable:
    ```
    ./Eriks_Wasp_Sim
    ```
    (The executable needs to remain within the Eriks-Wasp-Sim project folder, otherwise it will not work)

#### Dependencies
Download the following dependencies and put them into '/Eriks_Wasp_Sim/dependencies' (create the directory if it does not exist yet):

- [tinyobjloader v1.0.6](https://github.com/tinyobjloader/tinyobjloader/releases/tag/v1.0.6): Put 'tiny_obj_loader.h' into '/Eriks_Wasp_Sim/dependencies'
- [nlohmann-json v3.11.3](https://github.com/nlohmann/json/releases): Download the json.hpp file and put it into '/Eriks_Wasp_Sim/dependencies'
- [imgui-1.90.4](https://github.com/ocornut/imgui/releases/tag/v1.90.4): Download the imgui folder and put it into '/Eriks_Wasp_Sim/dependencies'
- [glew-2.1.0](https://sourceforge.net/projects/glew/files/glew/2.1.0/): Put the 'glew-2.1.0' directory into '/Eriks_Wasp_Sim/dependencies'
- [freeglut](https://www.transmissionzero.co.uk/software/freeglut-devel/): Download the MinGW Package and then put the 'freeglut' directory into '/Eriks_Wasp_Sim/dependencies'
- [glm v1.0.0](https://github.com/g-truc/glm/releases): Download the glm folder and put it into '/Eriks_Wasp_Sim/dependencies'

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
  - Additionally, some computations (e.g. transforming the wasp model based on position and viewing vector) are also only performed in the shader code on the GPU. This
    gives a massive performance boost on computations that are well suited for the GPU and do not need to return anything to the CPU (e.g. rendering transformations for thousands of instanced wasps).
  - To eliminate all code based on immediate rendering and fully focus on a single instanced rendering pipeline with a single .obj loader that only loads faces, debug lines connecting two points are rendered using a little workaround.
    A line between vert A and B is actually a face **f A B B** in the .obj file. This collapses the triangle onto the line. If the mesh is drawn using wireframe mode, it looks like a normal line.
* **Memory/Cache locality**: 
  - Wasp and food entities are kept in a vector of fixed-size and are all initialized at startup. Spawning and killing them simply means activating/deactivating the already initialized objects.
  This makes loops over large amounts of objects faster due to high levels of locality but comes with the downsides of fixed-size vectors.
  - It is better to use a vector on the heap instead of an array despite the fixed size because having this much data constantly stored on the stack and accessed through it introduces stutter. 
  Since the vector only grows at the start of the program and the memory stays locally fixed in one continuous block on the heap afterwards, the initialization overhead can be ignored. 
  (At more than 100000 wasps an array can be ~20% faster, but this is not worth the extra stutter at lower amounts)
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
  - Another quirk of thread pooling is specific to battery devices like laptops that have situational power saving modes: Using thread pooling in the simulation loop is slower than the single threaded approach when running on battery
    but a lot faster when connected to power.
* **No Chunks**: Originally, I intended to make use of dynamically loaded chunks to, for example, minimize the amount of entities the code needs to loop through when finding neighbours. However, there were significant downsides that made me
    decide against it:
  - Dynamically growing and shrinking the list of entities within a chunk is very expensive and necessitates a datastructure that would eliminate all the benefits of locality from the earlier sections.
  - If thousands of entities left their chunk at the same time (not very rare) and thereby caused the chunks' datastructures to be updated, the simulation ended up coming to a screeching halt until all chunks had been updated.
  - Performance would be way too dependant on how many entities are in the same chunk.
  - While accessing a chunk based on coordinates using an unordered map should theoretically be very fast, the reality of having to add and remove chunks from the map all the time prevents hashed access from reaching its full potential.
    Using a static set of chunks instead of dynamically growing and shrinking the map might lead to a small improvement in this aspect, but it would also severely limit how dynamic the simulation can be.
  - Erasing the benefits of locality and performance consistency for a small increase in performance in the best case and a huge decrease in performance (even with heavy optimization) in the worst case is simply not worth it.
