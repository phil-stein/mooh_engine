
# -- mooh --

[engine](https://github.com/phil-stein/mooh_engine/tree/main/src/core), [game](https://github.com/phil-stein/mooh_engine/tree/main/src/game) and [level-editor](https://github.com/phil-stein/mooh_engine/tree/main/src/editor) for a simple 3d farming / exploration game. <br>
written in c <br>

**still under development | unstable**

using:      [glfw3](https://www.glfw.org/), [glad](https://glad.dav1d.de/), [stb_image & stb_ds](https://github.com/nothings/stb), [nuklear](https://github.com/Immediate-Mode-UI/Nuklear), [thread.h](https://github.com/mattiasgustavsson/libs/blob/main/docs/thread.md) <br>
and my own: [math-library](https://github.com/phil-stein/math), [global-defines](https://github.com/phil-stein/global), [serialization-library](https://github.com/phil-stein/serialization), [physics-engine](https://github.com/phil-stein/phys)

main resources:  
  - [learnopengl.com](https://learnopengl.com/)
  - "3D Math Primer for Graphics and Game Development" by Dunn Parberry
  - "Game Physics Engine Development" by Ian Millington
  - ['the cherno' youtube](https://www.youtube.com/c/TheChernoProject)
  - ['winterdev' youtube](https://www.youtube.com/c/Winterdev)


## current capabilities

<img src="https://github.com/phil-stein/mooh_engine/blob/main/assets/project_files/screenshot02.png" alt="logo" width="1000">

# -- todo --

## next steps
  - *organization*
    - [comment all .h files](#organization) `WIP` 
    - [release system](#organization)
    - [rest of organization](#organization) `WIP`
  - *buggs*
    - [all buggs](#buggs)
  - *optimizations* 
    - [custom asset archive files](#optimizations) `WIP`
    - [multithreading](#multithreading) `WIP` 
    - [batch renderer](#optimizations) `WIP`
  - *base*
    - [structures](#base) `WIP`
    - [local entity data](#base)
  - *editor*
    - [undo/redo](#level editor)
  - *graphics*
    - [blending](#renderer)
    - [cascaded shadows](#renderer)
    - [particle system](#base)
    - [trail renderer](#base)
    - [ssao](#renderer)
    - [aa](#renderer)
    - [bloom](#renderer)
  - *advanced*
    - [physics engine](#physics engine)
    - [chunking for ents/phys_objs](#optimizations) 

## buggs
  - [ ] glfw mouse button & scroll callbacks 
    - works in nuklear, look at that
    - actually nuklear is stealing our callback from glfw
  - [ ] obj sizes are different than in blender
    - there is a apply transform option but its experimental
  - [?] some shader sometimes buggs, my have been caused by the point below, weird matrices and such
  - [ ] terrain-chunks dont get culled properly 
  - [ ] minimizing window to sys tray causes framebuffer crash [also mentioned here]()
  - [ ] freeing cubemap doesnt seem to actually free any memory
  - [ ] changing WINDOW_MIN, _MAX, _FULL doesnt work
  - [ ] changing THREAD_MAX in threadm.c doesnt affect its speed [also mentioned](#multithreading)
  - [x] mesh loading blender coord sys to mine, gizmo is fcked [also mentioned](#base)
  - [ ] getting entity_t to 256bytes makes physics no longer work
    - le what mate
  - [ ] if sizeof(entity_t) > 255 physics break
  - [ ] vs19 project no longer works 

## optimizations
  - [ ] [multithreading](#multithreading) 
  - [ ] use textures with r = roughness, g = metallic, ...
  - [ ] change the white tex for no normal
  - [ ] occlusion culling 
  - [ ] batch renderer `WIP`
    - [x] keep track of draw calls 
    - [x] keep track of all template ents
    - [ ] mark static, in template / editor ?
    - [ ] instanced draw all static templates
  - [ ] lod system ?
  - [ ] octree or something for chunks, for entities / phys_objs
  - [ ] custom asset formats
    - [ ] cubemaps 
  - [ ] custom asset archive files | not in use
    - all textures, etc. back to back in one file with a simple header
    - type specific or agnostic ?
      - i.e. just serialize a bunch of buffers or have mesh/texture
    - screws with multithreading, kind of, also very big files
    - maybe just a couple at a time ?
    - [x] textures | not in use 
    - [ ] meshes
    - [ ] shaders
  - [ ] only clear outline buffer when deselecting
  - [ ] memory allocation optimization
    - [ ] read game engine architecture page 427-
    - [ ] stack allocator   ?
    - [ ] frame allocator   ?
    - [ ] pool allocator    ?
    - [ ] chunk allocator   ?
    - [ ] randy memory pool ?
  - [x] figure out why core_data_init() is taking so long: shaders
    - [ ] shader spir-v ?

## sus amogus
  - when parenting broke and i fixed it by setting 'is_moved' in 'state_update_global_model()'
    it worked before and something changed in the code that made it no longer work that way
    what exactly changed ???
  - ENTITY_SET_POS() doesnt work properly in program_sync_physics(), need to set velocity to 0
  - zip archive not faster than straight files

## organization
  - [ ] release system
    - [ ] package assets & exe
      - obv, just the custom / needed assets
      - archive if done yet 
    - [ ] set asset_path to cwd/assets
    - [ ] maybe check if folder build_01 exist and name build_02, etc.
    - [ ] put date in folder / exe name or .txt file or some
  - [ ] rename state_remove_entity() & state_duplicate_entity() to
        state_entity_remove() & state_duplicate_entity()
  - [ ] seperate save_sys.c into terrain & entities
  - [ ] implement glfw opengl debug context, learnopengl page 439 ?
  - [ ] check shaders via reference compiler, page 444, debug_opengl.h
  - [ ] framebuffer debug, page 444, debug_opengl.h
  - [ ] add missing P_TYPE() funcs to global.h
  - [ ] add icons to exe
    - [ ] texture_view
    - [ ] editor
  - [ ] make global submodule again, check if phys, serialization are still submodules
  - [ ] remove pos  maybe also cast_shadow, etc. from dir light
  - [ ] add [tracing](https://balau82.wordpress.com/2010/10/06/trace-and-profile-function-calls-with-gcc/)
  - [ ] add [profiling](https://ftp.gnu.org/old-gnu/Manuals/gprof-2.9.1/html_mono/gprof.html), [tutorial](https://www.thegeekstuff.com/2012/08/gprof-tutorial/)
  - [x] add CORE_ infront of all .h _H macro ifdef checks
  - [ ] make most funcs _dbg error checked ? 
    - [ ] use _dbg funcs for materials in assetm ?
    - [x] make _dbg check a macro to set easy and compile out ?
      - when compiled out _file & _line cause errors
      - are _file & _line impacting performance ?
    - [ ] state
    - [ ] etc.
  - [ ] go through all files check for unecessary header includes, to lower compile time / remove clutter
  - [ ] comment all .h files
    - [ ] math
      - [ ] math_mat4.h
      - [ ] math_space.h
    - [x] editor
      - [x] app.h
      - [x] gizmo.h
      - [x] gui.h
      - [x] terrain_edit.h

## tools
  - [ ] binary dump
    - [x] empty scene
    - [ ] empty terrain
  - [ ] "model-viewer/-editor" for shaders / materials / anim / particles
  - [x] precompute brdf / cubemap in software to load in game, [also mentioned](#optimizations)
    - [x] save framebuffer as tetxure, for brdf
    - [x] load instead of generate
  - [x] texture viewer for .tex files

## base
  - [ ] make debug_draw_circle() func use mat4 and vec3 instead of pythag
  - [x] load mesh
    - [ ] triangularize mesh ? (not really necessary as blender does it)
    - [x] blender coord sys to mine, gizmo is fcked [also mentioned](#buggs)
  - [ ] asset manager
    - [ ] free assets
      - [x] textures
      - [ ] meshes
      - [ ] cubemaps [doesnt work](#buggs) `WIP`
      - [ ] shaders
    - [ ] load shaders from zip
    - [ ] load cubemaps from zip
    - [ ] handle missing assets
  - [ ] hot-reload shaders
  - [x] make entities have 'local variables'
    - ents have init_f and update_f, but no local data
      if i set speed in on init_f, it affects all entities with that init_f
      i also cant set it from anywhere else, unless i make a function specific to that
    - maybe make void struct with X size and cast it to ones with actual members, all ents have more data though
    - struct { int[5] }data; struc{ u32 bool vec3 }data_player, 
      ((data_player)entity_t.data).member
    - [x] f.e. use it to set speed/orientation/target/color/.etc on projecticle
          from the players code, aka. spawn projecticle, set its data
    - [x] easy with macros      
  - [ ] make seperate editor map file so f.e. structures can be stored special and edited all at once ? 
  - [ ] setup 32bit (-m32) : [tdm docs](https://github.com/jmeubank/tdm-distrib/blob/master/tdm64/core/README-gcc-tdm64.md)
    - [ ] reset all dynamic objects ? 
      - this way we don't have to reload the entire thing 
      - not sure if needed in final game though
      - requires restructuring probably
      - reload suprisingly fast
    - [ ] game saves system
      - single map file, never changed
      - multiple variants of that with changes
        - i.e. 'save01.scene', 'mygame.scene', etc.
  - [ ] event system
    - [x] entity add, remove, parented
    - [x] collision / trigger
    - [ ] etc.
    - [ ] custom game events (day to night, etc.)
      - prob. need to be done in game
      - unless arbitrary func pointer, like void* for funcs
    - [ ] pooling
  - [ ] billboards
  - [ ] particle system
    - cpu / gpu side or mix ?
    - [ ] pooling
    - [ ] billboards
    - [ ] different spawn "shapes"
      - [ ] circle
      - [ ] cone
      - [ ] sphere
      - [ ] cylinder ?
    - [ ] animated textures
    - [ ] integrate trail renderer
  - [ ] trail renderer
  - [ ] debug tools
    - [ ] debug_draw_circle() with resolution
    - [ ] add profiler
  - [ ] controller support ?

## renderer
  - [ ] blending
    - learnopengl weighted blending 'OIT'
  - [ ] cascaded shadows
    - from cherno video 
    - also on learnopengl
  - [ ] ssao
  - [ ] bloom
  - [ ] custom anti aliasing
    - [x] try built in again 
      - dont work, white around objs
  - [ ] water 

## entity system
  - [ ] structures ? (prefabs), [also mentioned](#base)

## level editor
  - [ ] pause mid game
  - [ ] generate struct literal for 
    - [x] materials
    - [ ] entities
    - [ ] shaders
  - [x] structure browser
  - [ ] particle system editor
    - seperate ?
  - [ ] make gui less sh*t
  - [ ] ask to save if unsaved changes
  - [ ] undo
    - [ ] keep track of changes
    - [ ] john jackman or some on yt pixeleditor

## physics engine
  - [x] sphere
    - [ ] proper debug draw for sphere
  - [x] aabb
    - [ ] parented aabb's dont change pos correct
    - [ ] change aabb in editor
  - [ ] plane
  - [ ] obb (replace aabb, or use aabb as optimization)
  - [ ] fix buoancy in resolution, actually in phys_debug_draw() ? 
  - [ ] sweeping collision
  - [ ] organize in grid, octree, etc ?
    - only checking collisions in the same/neighbouring grids
    - dorment objs if camera to far away
  - [ ] cylinder / capsule
  - [ ] raycasts ?
    - [ ] octree & ray v. triangle ?

## multithreading
  - maybe make abstraction for thread stuff ?
  - add [profiler](#base) first
  - architecture
    - main (input, gameplay, etc.)
    - worker (asset-loading, terrain, etc.)
    - renderer
    - physics
  - [x] make example program
  - [ ] asset loading
    - [x] textures
      - [ ] changing THREAD_MAX in threadm.c doesnt affect its speed [also mentioned](#buggs)
      - [ ] experiment with thread count
      - [ ] load textures with pbo's, is supposed to be faster `WIP`
      - [ ] multithreaded when not loading arr
    - [ ] meshes
    - [ ] shaders
    - [ ] scene ?
    - [ ] terrain
  - [ ] physics
  - [ ] rendering ?
    - decouple entities from main-thread
    - duplicate world, etc. -> merge
  - [ ] terrain

## audio system
  - using miniaudio, soloud, etc.
  - [ ] clips
  - [ ] loops
  - [ ] sfx ?
  - [ ] system
    - queue bg music, ambient, etc.

## animation system
  - [ ] learnopengl.com
  - [ ] system
  - [ ] blending animations

## artstyle look-dev
  - [ ] draw sketch of scene
    - the base: old wooden house, trees, 
      rusty equipment, overgrown
  - [ ] make 3d polished scene 
    - [ ] stones
      - [ ] small
      - [x] middle
      - [ ] big
    - [ ] trees
      - [ ] normal
      - [x] big (f*cked normals)
    - [ ] bush
    - [ ] fence
    - [ ] old hut

## terrain generation
  - [ ] normals & tangents on edges
    - button in gui to calc for all chunks

## in-game ui
  - hook rendering into core
  - [ ] draw quads 
    - [ ] scale with screen
    - [ ] textured quads
  - [ ] aligned hierarchy
    - [ ] centered
    - [ ] left / right
    - [ ] up / down
  - [ ] text (already wrote lib)

## game design
  - [ ] game design doc
  - [ ] ...
