
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
  - *optimizations*
    - [custom asset formats](#optimizations) `cur goal`
    - [multithreading](#multithreading) `WIP` 
      - sample / study proj
      - write abstraction ?
      - asset loading via zip
      - sep. main, renderer, app, etc.
    - [precompute brdf, equirect, etc](#optimizations)
    - [batch renderer](#optimizations)
  - *base*
    - [structures](#base) `WIP`
  - *graphics*
    - [blending](#renderer)
    - [cascaded shadows](#renderer)
    - [particle system](#base)
    - [trail renderer](#base)
    - [missing post-fx, bloom, ssao, aa](#renderer)
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
  - [x] serialization or something adds pointlights every u play 
  - [x] play mode doesnt work anymore     
  - [x] deselecting doesnt clear the outline buffer
  - [ ] freeing cubemap doesnt seem to actually free any memory
  - [ ] brick material on groud isnt the same as the other cubes
    - they should all be referencing the same material_t 

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
      - uncompressed for faster load time 
    - [ ] shader spir-v ?
  - [ ] only clear outline buffer when deselecting
  - [ ] change zip archive for custom file format
    - all textures, etc. back to back in one file with a simple header

## sus amogus
  - when parenting broke and i fixed it by setting 'is_moved' in 'state_update_global_model()'
    it worked before and something changed in the code that made it no longer work that way
    what exactly changed ???
  - ENTITY_SET_POS() doesnt work properly in program_sync_physics(), need to set velocity to 0
  - zip archive not faster than straight files

## organization
  - [x] factor out cubemap creating from assetm, into sep. file
  - [x] factor out brdf creating from renderer
  - [x] factor out mouse_pick & outline from renderer into sep. file 
  - [ ] fix ale error on #include's
  - [ ] replace phys_act & scripts_act with flag
  - [ ] seperate save_sys.c into terrain & entities
  - [ ] use _dbg funcs for materials in assetm
  - [x] change idx to id in state / entity / etc.
  - [x] ! get game comliling again
  - [x] make serialization into generalized gitrepo
    - [x] seperate serialization & save_sys 
  - [ ] comment all .h files in core
    - [x] debug
      - [x] debug_draw.h
      - [x] debug_timer.h
    - [x] types
      - [x] framebuffer.h
      - [x] material.h
       - [ ] check @TODO: in mesh_t 
      - [x] mesh.h
      - [x] shader.h
      - [x] texture.h
      - [x] types.h
    - [ ] main
      - [x] assetm.h
      - [x] camera.h
      - [x] core_data.h
      - [x] event_sys.h
      - [x] file_io.h
      - [x] input.h
      - [ ] program.h
      - [ ] renderer.h
      - [ ] renderer_direct.h
      - [ ] serialization.h
      - [ ] state.h
      - [ ] str_util.h
      - [ ] terrain.h
      - [ ] window.h
    - [ ] data
      - [ ] entity_template.h
      - [ ] material_template.h
      - [ ] shader_template.h
    - [ ] editor
      - [ ] app.h
      - [ ] gizmo.h
      - [ ] gui.h
      - [ ] terrain_edit.h
    - [ ] phys
      - [ ] phys_collisions.h
      - [ ] phys_debug_draw.h
      - [ ] phys_dynamics.h
      - [ ] phys_resolution.h
      - [ ] phys_response.h
      - [ ] phys_types.h
      - [ ] phys_world.h

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
    - [ ] free assets
      - [x] textures
      - [ ] meshes
      - [ ] cubemaps [doesnt work](#buggs) `WIP`
      - [ ] shaders
    - [ ] load shaders from zip
    - [ ] load cubemaps from zip
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
    - [x] entity add, remove, parented
    - [x] collision / trigger
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
  - [x] serialize cube_map
  - [ ] add tags or equivalent to entities
    - for on_collision etc.
    - single or multiple ?

## renderer
  - [ ] blending
    - learnopengl weighted blending 'OIT'
  - [ ] cascaded shadows
    - from cherno video 
    - also on learnopengl
  - [x] tiling
  - [ ] ssao
  - [ ] bloom
  - [ ] custom anti aliasing
  - [ ] water 
  - [x] maybe do outlines ?     
  - [x] immediate mode renderer, for gui, etc.
  - [x] cubemap intensity

## entity system
  - [ ] structures ? (prefabs), [also mentioned](#base)

## level editor
  - [ ] structure editor
    - seperate ?
  - [ ] particle system editor
    - seperate ?
  - [ ] make gui less sh*t
  - [ ] ask to save if unsaved changes
  - [ ] undo
    - [ ] keep track of changes
    - [ ] john jackman or some on yt pixeleditor
  - [x] give trigger colliders a different color
  - [x] draw line between parent & child
    - [x] indicate which side is parent/child
  - [x] outline on selected entity
  - [x] make a .h file with all color macros
    - [x] parent / child lines
    - [x] move the color of collider debug draw from phys to editor
  - [x] check mouse over ui, doesnt work for debug, hierarchy, etc.

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
  - maybe make abstraction for thread stuff ?
  - add [profiler](#base) first
  - architecture
    - main (input, gameplay, etc.)
    - worker (asset-loading, terrain, etc.)
    - renderer
    - physics
  - [x] make example program
  - [ ] asset loading
    - [ ] also works with zip ?
    - [ ] textures
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
