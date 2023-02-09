
# -- mooh --

[engine](https://github.com/phil-stein/mooh_engine/tree/main/src/core), [game](https://github.com/phil-stein/mooh_engine/tree/main/src/game) and [level-editor](https://github.com/phil-stein/mooh_engine/tree/main/src/editor) for a simple 3d farming / exploration game. <br>
written in c <br>

**still under development | unstable**

using:      [glfw3](https://www.glfw.org/), [glad](https://glad.dav1d.de/), [stb_image & stb_ds](https://github.com/nothings/stb), [nuklear](https://github.com/Immediate-Mode-UI/Nuklear)<br>
and my own: [math-library](https://github.com/phil-stein/math) & [global-defines](https://github.com/phil-stein/global)

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
  - *optimizations*
    - [multithreading](#multithreading) `WIP | cur goal`
      - sample / study proj
      - write abstraction ?
      - asset loading via zip
      - sep. main, renderer, app, etc.
    - [custom asset formats](#optimizations)
    - [precompute brdf, equirect, etc](#optimizations)
    - [batch renderer](#optimizations)
  - *base*
    - [structures](#base) `WIP`
  - *graphics*
    - [blending](#renderer)
    - [cascaded shadows](#renderer)
    - [particle system](#base)
    - [trail renderer](#base)
    - [missing vfx, bloom, ssao, aa](#renderer)
  - *advanced*
    - [physics engine](#physics-engine)
      - sweept collisions
      - obb's
      - spheres
    - [chunking for ents/phys_objs](#optimizations) 
  - *organization*
    - [comment all .h files in core](#organization) 

## buggs
  - [ ] since mat sys blank.png gets loaded twice (hdr / no hdr) 
  - [ ] glfw mouse button & scroll callbacks 
    - works in nuklear, look at that
    - actually nuklear is stealing our callback from glfw
  - [ ] obj sizes are different than in blender
    - there is a apply transform option but its experimental
  - [ ] math_ivec2.h doesnt get included in terrain.c, maybe everywhere
  - [?] some shader sometimes buggs, my have been caused by the point below, weird matrices and such
  - [ ] deserializing scene sometimes adds pink point light infront of camera
  - [ ] terrain-chunks dont get culled properly 
  - [ ] point light entity *1* is not parented to the player, 
        but the player is it's parent
        aka. parenting is broken prob. in serialization
  - [ ] removing objects in editor causes crash
  - [ ] minimizing window to sys tray causes framebuffer crash [also mentioned here]()
  - [ ] serialization or something adds pointlights every u play 

## optimizations
  - [ ] [multithreading](#multithreading) 
  - [ ] use textures with r = roughness, g = metallic, ...
  - [ ] change the white tex for no normal
  - [ ] occlusion culling 
  - [ ] batch renderer
  - [ ] lod system ?
  - [ ] octree or something for chunks, for entities / phys_objs
  - [ ] precompute brdf, etc., [also mentioned](#tools)
  - [ ] custom asset formats
    - [ ] mesh
      - pure verts, nothing else
      - 3:pos + 3:norm + 2:uv + 3:tan  =  11: f32, 44: bytes 
      - quad: fbx:11.1kb / custom:176bytes = 1.5% 
      - demon02: fbx:1.68mb / custom:87kb  = 5.1%
      - prob. not realistic to be this drastic, but say 10% would still be amazing
    - [ ] texture ?
    - [ ] shader spir-v ?

## sus amogus
  - when parenting broke and i fixed it by setting 'is_moved' in 'state_update_global_model()'
    it worked before and something changed in the code that made it no longer work that way
    what exactly changed ???
  - ENTITY_SET_POS() doesnt work properly in program_sync_physics(), need to set velocity to 0

## organization
  - [ ] factor out cubemap creating from assetm, into sep. tool
  - [ ] factor out brdf creating from renderer, into sep. tool
  - [ ] fix ale error on #include's
  - [ ] replace phys_act & scripts_act with flag
  - [ ] seperate serialization.c into terrain & entities
  - [ ] comment all .h files in core
    - [ ] debug
      - [ ] debug_draw.h
      - [ ] debug_timer.h
    - [ ] types
      - [ ] framebuffer.h
      - [ ] material.h
      - [ ] mesh.h
      - [ ] shader.h
      - [ ] texture.h
      - [ ] types.h
    - [ ] main
      - [ ] assetm.h
      - [ ] camera.h
      - [ ] core_data.h
      - [ ] event_sys.h
      - [ ] file_io.h
      - [ ] input.h
      - [ ] program.h
      - [ ] renderer.h
      - [ ] renderer_direct.h
      - [ ] serialization.h
      - [ ] state.h
      - [ ] str_util.h
      - [ ] terrain.h
      - [ ] window.h

## tools
  - [ ] binary dump
    - write empty scene, terrain, etc. to file
  - [ ] "model-viewer/-editor" for shaders / materials / anim / particles
  - [ ] precompute brdf / cubemap in software to load in game, [also mentioned](#optimizations)
  - [ ] ... 

## base
  - [x] load mesh
    - [ ] triangularize mesh ? (not really necessary as blender does it)
    - [x] blender coord sys to mine
  - [ ] asset manager
    - [ ] load shaders from zip
    - [ ] handle missing assets
    - [ ] custom asset formats [descr. here](#optimization)
  - [ ] structures (prefabs)
    ```c
      // this way structures can be added regardless of world id's
      // also entities need a reference to which structure they are a part of 
      struct structure_def_t
      {
        entity_t* e;
        int parent_idx; // idx into structure_t, not world
      };
      struct structure_t
      {
        structure_def_t* arr;
        int len;
      };
    ```
    - [ ] create / load
    - [ ] store ?
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
    - [ ] entity add, remove, parented
    - [ ] etc.
    - [ ] custom game events (day to night, etc.)
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
    - [ ] add profiler
  - [ ] controller support ? 

## renderer
  - [ ] blending
    - learnopengl weighted blending 'OIT'
  - [ ] fix normal shadows (werid pos ?)
  - [ ] cascaded shadows
    - from cherno video 
    - also on learnopengl
  - [ ] tiling
  - [ ] ssao
  - [ ] bloom
  - [ ] custom anti aliasing
  - [ ] water 
  - [ ] maybe do outlines ?     
  - [ ] immediate mode renderer, for gui, etc.

## entity system
  - [ ] structures ? (prefabs), [also mentioned](#base)

## level editor
  - [x] gizmos
    - [ ] draw line for rotation ?
    - [ ] draw line for scale ?
  - [ ] structure editor
    - seperate ?
  - [ ] particle system editor
    - seperate ?
  - [ ] make gui less sh*t
  - [ ] ask to save if unsaved changes
  - [ ] undo
    - [ ] keep track of changes
    - [ ] john jackman or some on yt pixeleditor

## physics engine
  - [ ] sphere
  - [x] aabb
    - [ ] parented aabb's dont change pos correct
    - [ ] "rotate" aabb's in 90° steps
    - [ ] change aabb in editor
  - [ ] plane
  - [ ] obb (replace aabb, or use aabb as optimization)
  - [ ] fix buoancy in resolution, actually in phys_debug_draw()
  - [ ] sweeping collision
  - [ ] organize in grid, octree, etc ?
    - only checking collisions in the same/neighbouring grids
    - dorment objs if camera to far away
  - [ ] cylinder / capsule
  - [ ] raycasts ?
    - [ ] octree & ray v. triangle ?

## multithreading
  - decide on win32, C11, single header lib
  - make example program
  - maybe make abstraction for thread stuff ?
  - add [profiler](#base) first
  - architecture
    - main (input, gameplay, etc.)
    - worker (asset-loading, terrain, etc.)
    - renderer
    - physics
  - [ ] any multithreading
  - [ ] asset loading
  - [ ] physics
  - [ ] rendering
    - decouple entities from main-thread
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
