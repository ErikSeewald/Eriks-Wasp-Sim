# Eriks-Wasp-Sim
This is a simulation of a contract-based, tax-monarchical wasp society. Wasps explore, eat food, pay taxes to their queen and establish formal contracts with each other, all rendered using OpenGL.

Currently, the simulation supports an upper limit of 100 000 wasps. It is possible to increase the limit
further by changing a single variable within the [code](/src/simulation/Wasp/WaspSlots.cpp), but I would not recommend it. All my testing (to ensure 60FPS on my machine) is done with this upper limit (see [performance notes](#performance-notes)). 

## Wasps
A wasp is an entity in 3D space that has a position, a viewing direction and, sometimes, a goal that it is heading toward. Its behavior is defined by desires related to attributes like health and hunger saturation.
Every wasp knows its [queen](#queen), is born with specific [genes](#genes), and has the ability to create or enter [contracts](#contracts) with other wasps.

The simulation has a fixed-size array of wasp objects. Therefore, each wasp has a *w_Index* within the array and thereby a unique identifier. A living wasp is represented by a boolean *isAlive* being set to true. Spawning a new wasp (within the limit of the maximum amount of wasps) is done by taking one of the wasp objects that has *isAlive* set to false and 'respawning' it with new attributes and genes. For more information on how and why this is done, see [performance notes](#performance-notes).

#### Wasp attributes
The following section describes the most important attributes that define a wasp.
- **Position**: This 3D vector is the definitive location of the wasp and is used for all of its interactions.
- **Viewing vector**: This is a normalized vector that defines where, from its position, the wasp is currently looking/heading. Forward movement is based on this vector.
- **Current goal**: A 3D vector that defines the location that the wasp is currently flying towards (e.g., food, another wasp). This vector may be *null* if the wasp is currently exploring randomly.
- **Genes**: Static attributes that the wasp is born with, see the [genes](#genes) section.
- **Contracts**: The [contracts](#contracts) that the wasp is currently a partner in.
- **Turn- and ascend speed**: Factors on top of the wasp's flying speed gene that affect how quickly the wasp is turning or ascending/descending respectively at the current point in time. 
- **HP**: The number of health points that the wasp currently has. Once these reach zero, it dies. As long as the wasp is not starving, these points can slowly regenerate.
- **Hunger saturation**: The amount of hunger points the wasp currently has. The more it has, the less hungry it is. These points decrease over time but can be replenished by finding or receiving food. Once these points reach zero, the wasp begins to starve, gradually losing hp.

#### Behavior
By default, wasps are randomly exploring their surroundings. If their food saturation falls below their maximum, they start becoming more likely to choose food entities as their current goal and fly towards them.

This basic behavior can be affected by contracts, loyalty to the queen, and other unique situations that are explained later.

To understand what a wasp is doing, make use of the [selected-wasp-window](#selected-wasp-window) in the UI. 

#### Queen
The queen is a special wasp outside the wasp array. Its w_Index is -1. Only one queen can exist within the simulation and all wasps store a reference to it. The following behaviors are unique to the queen.
- **Receiving food**: Other wasps, depending on their loyalty, may choose to gift some of their food to the queen. For this, they need to be in her interaction range. She can choose to accept or reject that gift.
- **Worker scores**: The queen assigns every worker wasp that it interacts with a score. This score can be affected by actions like the worker giving her a certain amount of food. Wasps with high worker scores may receive special treatment from the queen like higher share in the food redistribution.
- **Distributing food**: When the queen's hunger saturation grows beyond her maximum due to gifts from worker wasps, it goes into a special 'pot' (the QueenFoodStorage, see [Hive-window](#hive-window)) from which she regularly distributes the food points among her favorite workers based on their worker score.
- **Swarming**: If the queen comes close to starving (or is dying for other reasons), loyal worker wasps will start heading for her location and swarming around her.

## Resources
Outside of wasps, another type of entity that exists in the simulation is a resource entity.
Items that provide something to the wasps or represent some sort of desire.

These resources are spawned automatically (or manually) based on various resource settings (see [commands.json](assets/commands/Commands.json)).

#### Food
The main resource in the simulation is food. A single food entity is represented by a point in space and can restore a varying number of hunger points. 

## Genes
The simulation uses a sort of gene structure to define randomizable (inheritable?) traits of individual wasps.
These genes fit into two types: **BalancedGenes**, which can be mutated only through a trade-off mutation in another gene, and **UnboundGenes**, which can mutate freely.
For now, mutation happens randomly at the start of a wasp's lifespan, with the gene values remaining constant for the rest of it.

#### BalancedGenes
Balanced genes are genes that can be mutated only through a trade-off mutation in another gene.
Every gene has a cost and an orientation associated with it. The former defines how costly a mutation +-1.0 is, and the latter defines whether the (assumed) beneficial direction of change is positive or negative.
For example, a mutation to the maximum hp of the wasp has a positive orientation because it is assumed that more hp are beneficial. This matters for the balancing function. A good change of a specific cost needs
to be balanced by a bad change of equivalent cost in another gene. For two positively oriented genes, *geneA* with a cost of 1.0 and *geneB* with a cost of 20.0, a good change in *geneB* needs to be balanced out by a bad
change in *geneA* with 20 times the intensity.

The following balanced genes currently exist in the simulation (for costs, orientation and default values, see [WaspGenes.h](/src/simulation/Wasp/WaspGenes.h) and [WaspGenes.cpp](/src/simulation/Wasp/WaspGenes.cpp)):
- **maxHP**: The maximum health points of a wasp (up to which it can be healed and regenerated).
- **maxHungerSaturation**: The maximum amount of "stored" food a wasp can eat or receive.
- **flyingSpeed**: A multiplier applied to the speed of most movement actions of a wasp.

#### UnboundGenes
Unbound genes are genes that can be mutated freely without consideration for other genes. Every unbound gene has a "range" associated with it. This range defines how strongly the gene value can change in a single mutation.

The following unbound genes currently exist in the simulation (for costs, orientation and default values, see [WaspGenes.h](/src/simulation/Wasp/WaspGenes.h) and [WaspGenes.cpp](/src/simulation/Wasp/WaspGenes.cpp)):
- **queenLoyalty**: This loyalty factor affects how devoted the wasp is to its queen (e.g., how likely is it to choose to fly close to the queen; how much food is it willing to gift to the queen).
- **contractDesire**: A factor defining how likely a wasp is to both propose and accept contracts.

## Contracts
A contract is an agreement between two or more wasps with the following properties:
- A validity period after which the contract expires
- An ordered list of contractual partners
- A set of rules and properties that are specific to the type of contract

Wasps can propose contracts to other wasps, which are then allowed to accept or reject the proposal.
Such a proposal can either be for inviting a wasp to an existing contract, thereby expanding the list of partners,
or for establishing a new contract, in which case the terms of the contract need to be negotiated.

As long as a contract is valid, every partner is obligated by the basic laws of the universe (my assumptions in the code base) to follow its
rules. Not only the expiration of the validity period, but also the death of partners, leading to less than two partners, can cause a contract to be
invalidated.

(Note that the simulation only clears invalid contracts from memory in discrete intervals, meaning that the GUI may show a contract with a negative "Valid for (s)" timer for a short moment. Such a contract is already being treated as invalid, it just has not been cleared yet.)

(The *WaspRenderMode* selection in the [Options-window](#options-window) features a mode called *IsContractPartner* that shows all partners of the currently selected wasp in green.) 

#### Contract types
What follows is a short summary of the contract types that currently exist in the simulation. The properties shared between all contracts (e.g., validity period) are not listed again.

1. **FoodSharingContract**: A contract that stipulates a minimum level of hunger saturation (allowance) beyond which a certain percentage of acquired food is shared between all partners.
   - *hungerSaturationAllowance*: This negotiable parameter dictates how much food saturation a parter is allowed to have before it needs to share newly acquired food.
   - *sharingRate*: This negotiable parameter describes the relative amount of any new chunk of acquired food that needs to be shared with the other partners.
   - The chunk of food that needs to be shared is divided among all other partners evenly.
2. **SwarmContract**: A contract that requires all the involved partners to stay within a certain range of partner 1 (the first partner in the partners list - can change depending on deaths, etc.).
   - *range*: When further away from partner 1 than this negotiable parameter dictates, a partner must set its current goal to be the goal of partner 1 (or its position if it currently has no goal).
   - Through this goal sharing, a sort of swarming behavior is created.

## Commands
The Wasp-Sim uses a CLI that opens alongside the OpenGL window at startup. Type 'help' to see a list of available commands or look through [commands.json](assets/commands/Commands.json) for more information.

Each command has the following attributes: 
- Name and explanation, which are printed by the 'help' command
- Syntax, which can be displayed by typing ```syntax [command] {subcommand}*``` (e.g., ```syntax wasp sethp```)
- List of subcommands (e.g., 'kill', 'sethp', ... for the 'wasp' command)
)
Sometimes 'syntax' makes use of command elements (e.g., \<position\> in ```wasp setpos <position>```). These are placeholders that may require a more specific explanation.
By using the 'element' command (e.g., ```element position```) you can print that explanation.

## Controls
#### Movement
- Use [W,A,S,D] to move
- Hold SPACE to move more quickly
- Use the arrow keys to rotate the camera.

## UI
- Use your mouse to interact with, and optionally move/scale, the GUI
- Left-click a wasp with your mouse to select it
- Press 'p' to pause/unpause the simulation loop

#### Selected-wasp-window
When you left-click a wasp with your mouse, that wasp is selected.
It is now being drawn with a red wireframe and a "Selected wasp" UI window will have opened.
Here you can see some of the following:
- The wasp's w_Index (unique identifier, index in the wasp array)
- The worker score that the queen has assigned to this wasp
- The wasp's position, viewing vector and movement speed
- The wasp's current goal/target (and an option to render a line from the wasp to that position)
- The wasp's health and hunger points
- The values of the wasp's [genes](#genes)
- The contracts that the wasp is a partner in. Clicking one of them opens the [Contract-window](#contract-window)

#### Contract-window
When a contract has been selected from the [Selected-wasp-window](#selected-wasp-window), a window containing information on this specific contract opens with the following information:
- How many remaining seconds of validity does it have
- How many contractual partners are there
- A list of these partners (clicking on one of them selects that wasp)
- The type of the contract and specific information regarding it (see [Contract types](#contract-types))

#### Performance-window
At the top right of the screen, there is the performance window. It displays the following information:
- **FPS**: The current frame rate of the simulation visualizer
- **Max w_index**: The highest wasp index containing a living wasp (see [Performance notes](#performance-notes))
- **Max f_index**: The highest food index containing uneaten food

#### Hive-window
Near the top right of the screen, there is the performance window. It displays the following information:
- **Alive**: The number of living wasps at the current moment
- **Dead**: The total number of wasps that have died since the start of the simulation. Note that this number can be greater than the maximum size of the wasp array as slots of dead wasps can be used to spawn new wasps.
- **Total**: The total number of wasps that have been spawned since the start of the simulation.
- **QueenFoodStorage**: The amount of food points that the queen currently holds beyond her own hunger saturation, ready to be distributed to her favorite workers.
- **Active Contracts**: The number of active contracts in the simulation. This number is not guaranteed to always match the number of valid contracts as the contract cleanup routine runs in discrete intervals.
- **Undeleted contracts**: The number of contracts that have been scheduled for deletion but still remain in memory.

#### Camera-window
On the right side of the screen, there is the camera window. It displays the current position and viewing vector of the camera.

#### Options-window
On the right side of the screen, there is the options window. It displays the following settings:
- **Pause sim**: Allows pausing any updates to the simulation. Camera movement and UI interaction are still available.
- **Draw grid**: Allows turning the rendering of the little grid at the center of the simulation on or off.
- **WaspRenderMode**: Selects the mode with which the wasps are displayed:

  | Mode | Description |
  |------|-------------|
  | **UniformFlat** | All wasps are orange and completely flat-shaded. |
  | **UniformColor** | All wasps are orange but use triangle-normal-based shading (all rendering modes use normals for shading unless specified otherwise). |
  | **RandomOranges** | Individual wasps are rendered with unique orange colors. |
  | **FullRandomColors** | Every wasp has a random color. |
  | **GreenIfHasGoal** | Wasps are red if they do not currently have an active goal and green if they do. |
  | **RelativeWorkerScore** | Wasps range from blue (low worker score) to red (high worker score) relative to the wasp with the highest worker score. The queen is green. |
  | **RelativeHunger** | Wasps range from green (fully saturated) to red (starving). |
  | **RelativeHealth** | Wasps range from green (full HP) to red (about to die). |
  | **IsContractualPartner** | Wasps that are partners in contracts of the currently selected wasp are green. Others are bright red if they have contracts and dark red if they have no contracts. |

## Build instructions
#### Linux
1. Install the required libraries:
    ```
    sudo apt install libgl1-mesa-dev libglew-dev freeglut3-dev libglfw3-dev libglm-dev libreadline-dev
    ```

2. Follow the instructions in the [Dependencies](#Dependencies) subsection.
3. Open the [build](build) directory and run the following commands:
    ```
    mkdir linux; cd linux; cmake ..
    ```

    ```
    make
    ```

4. Run the executable:
    ```
    ./EriksWaspSim
    ```
    (The executable needs to remain within the Eriks-Wasp-Sim project folder. Otherwise it will not work)

#### Dependencies
Download the following dependencies and put them into `dependencies/` (create the directory if it doesn't already exist):

- [tinyobjloader v1.0.6](https://github.com/tinyobjloader/tinyobjloader/releases/tag/v1.0.6): Put `tiny_obj_loader.h` into `dependencies/`
- [nlohmann-json v3.11.3](https://github.com/nlohmann/json/releases): Download the `json.hpp` file and put it into `dependencies/`
- [imgui-1.90.4](https://github.com/ocornut/imgui/releases/tag/v1.90.4): Download the `imgui` folder and put it into `dependencies/`

## Architecture
#### Overview
The architecture of this project can be broadly split into three parts, each running on its own thread and spawned in [eriks_wasp_sim.cpp](/src/eriks_wasp_sim.cpp).
- **Simulation thread**: Handles the update loop of the simulation logic.
- **Visualization thread**: Handles rendering and GUI input. Reads and modifies the simulation state.
- **Console thread**: Handles console input from the user. Reads and modifies the simulation state.

![Base thread architecture diagram](/docs/img/base_thread_architecture.svg)

#### Simulation
TODO

#### Visualization
TODO

#### Console
TODO

## Performance notes
The Wasp-Sim makes use of multiple performance optimizations. This section is where I keep track of their trade-offs and why I chose them.

(This approach is limited by its stateful, CPU-in-loop nature. For even larger entity counts, check out [Pronicula-Splesh](https://github.com/ErikSeewald/Pronicula?tab=readme-ov-file#splesh)) 

* **Hardware Instancing** for rendering wasp and food entities.
  - Additionally, some computations (e.g., transforming the wasp model based on position and viewing vector) are also only performed in the shader code on the GPU. This 
    gives a massive performance boost to computations that are well suited for the GPU and do not need to return anything to the CPU (e.g., rendering transformations for thousands of instanced wasps).
  - To eliminate all code based on immediate rendering and fully focus on a single instanced rendering pipeline with a single .obj loader that only loads faces, debug lines connecting two points are rendered using a little workaround. 
    A line between vert A and B is actually a face **f A B B** in the .obj file. This collapses the triangle onto the line. If the mesh is drawn using wireframe mode, it looks like a normal line.
* **Memory/Cache locality**: 
  - Wasp and food entities are kept in a vector of fixed-size and are all initialized at startup. Spawning and killing them simply means activating/deactivating the already initialized objects.
  This makes loops over large amounts of objects faster due to high levels of locality but comes with the downsides of fixed-size vectors.
  - It is better to use a vector on the heap instead of an array despite the fixed size because having this much data constantly stored on the stack and accessed through it introduces stutter. 
  Since the vector only grows at the start of the program and the memory stays locally fixed in one continuous block on the heap afterward, the initialization overhead can be ignored. 
  (At more than 100000 wasps an array can be ~20% faster, but this is not worth the extra stutter at lower amounts)
* **Max indices**:
  - To reduce the downsides caused by the fixed-size vector optimization, the Wasp-Sim makes use of variables that keep track of the highest index of activated objects within the vector.
  These indices are then used to limit loops over the vector (i.e., if the wasp vector has a size of 100000 but only the first 5000 wasps are alive, wasp_maxIndex is set to 5000 and the other slots are not iterated over).
  While this has huge performance benefits, it comes with the added responsibility of managing these indices carefully.
  - Whenever objects are de-/reactivated the indices should be updated properly. While
  this is easy when these actions occur in a centralized manner (e.g., through the 'kill' or 'spawn' commands), decentralized deactivations like wasps deactivating because their hp reach 0 could potentially cause
  a lot of redundant loop iterations if they all make a call to the index update procedure. This problem is addressed by running this procedure on a fixed schedule every few seconds and only making extra calls to it
  whenever commands like the ones mentioned before are used.
  - To make sure the max indices are always as small as possible (while being unable to reorder elements within the vector), procedures that deactivate entities (e.g., the 'kill' command) should, if possible,
    kill entities at higher (indices) first. If there are exactly 100000 wasps, the max index is 100000, and ```kill wasp 95000``` is executed, the upper 95000 wasps should be killed so that the max index can be set to 5000.
  - There are some situations in which the max index can not be reduced as much as one would like. For example, if half of all wasps fail to find food, starve, and are then deactivated, it is very unlikely that all of their
    indices line up perfectly for the max index to be cut in half as well.
* **Threading**: Not only does the OpenGL code run on a different thread than the simulation code, but both of these threads also occasionally use more child threads.
  - The CPU needs to collect matrix information before sending it off to the GPU for hardware instancing. This is done with multiple threads looping over sections of the aforementioned localized entity vectors.
  - Threading is achieved using a custom [ThreadPool](src/util/ThreadPool.h) class. Instead of creating new threads each time they are used, a pool of threads is constantly waiting (BLOCKED) for new tasks
    that can be enqueued
  - For now it seems that multithreading is hugely beneficial in the rendering loop but has mixed results in the simulation loop. Under most circumstances a multithreaded simulation loop is a tiny bit slower due to the
    overhead of the thread pool class. This is also made more noticeable due to the demand for extra threads having an influence on the frame rate of the render loop as well (whereas the single thread approach only results in slower
    simulation update speeds and thereby choppy wasp movement). However, when the computational load of the simulation gets very high (like when there are a lot of food entities and a lot of wasp objects at the same time and
    each wasp is searching the list for the closest food object), the multithreaded approach is way faster and ends up looking a lot smoother. Sadly, dynamically scaling the size of the thread pool does not seem to help. It seems
    to be all or nothing.
  - Another quirk of thread pooling is specific to battery devices like laptops that have situational power saving modes: Using thread pooling in the simulation loop is slower than the single threaded approach when running on battery
    but a lot faster when connected to power.
* **No Chunks**: Originally, I intended to make use of dynamically loaded chunks to, for example, minimize the amount of entities the code needs to loop through when finding neighbors. However, there were significant downsides that made me
    decide against it:
  - Dynamically growing and shrinking the list of entities within a chunk is very expensive and necessitates a data structure that would eliminate all the benefits of locality from the earlier sections.
  - If thousands of entities left their chunk at the same time (not very rare) and thereby caused the chunks' data structures to be updated, the simulation ended up coming to a screeching halt until all chunks had been updated.
  - Performance would be way too dependent on how many entities are in the same chunk.
  - While accessing a chunk based on coordinates using an unordered map should theoretically be very fast, the reality of having to add and remove chunks from the map all the time prevents hashed access from reaching its full potential.
    Using a static set of chunks instead of dynamically growing and shrinking the map might lead to a small improvement in this aspect, but it would also severely limit how dynamic the simulation can be. 
