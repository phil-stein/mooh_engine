
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

<img src="https://github.com/phil-stein/mooh_engine/blob/main/assets/project_files/screenshot01.png" alt="logo" width="1000">

# -- todo --

## buggs
 - [x] calc_dir_light() causes all black 
 - [x] high res textures cause flickering
  - especially with metallic or normal
 - [x] reflections are upsidedown 
 - [x] framebuffer resize not working
 - [x] input mouse callback not called
  - work around solution
 - [ ] since mat sys blank.png gets loaded twice 
 - [ ] glfw mouse button & scroll callbacks 
  - works in nuklear, look at that
  - actually nuklear is stealing our callback from glfw
 - [ ] obj sizes are different than in blender
  - there is a apply transform option but its experimental
 - [x] cant deselect objects
 - [x] terrain mesh culling accumulates memory, big boi words 
 - [ ] math_ivec2.h doesnt get included in terrain.c, maybe everywhere
 - [?] some shader sometimes buggs, my have been caused by the point below, weird matrices and such
 - [x] parenting doesn't work anymore ._.
 - [ ] phys_obj_t's pos, scl and velocity gets set weird or nan

## optimizations
 - [ ] multithreading [link](#multithreading) 
 - [ ] use textures with r = roughness, g = metallic, ...
 - [x] entity_update_global_mat() 'dirtyflag'
 - [ ] change the white tex for no normal
 - [x] make material files a table
 - [ ] occlusion culling 
 - [ ] batch renderer
 - [ ] lod system ?

## sus amogus
 - when parenting broke and i fixed it by setting 'is_moved' in 'state_update_global_model()'
   it worked before and something changed in the code that made it no longer work that way
   what exactly changed ??? 

## organization
 - [ ] factor out cubemap creating from assetm
 - [x] make sub-folder for types
 - [x] find better place for entity_table
 - [x] find better name for entity folder
 - [x] replace mat4_perspective() with cam_get_proj()
 - [ ] fix ale error on #include's
 - [ ] seperate draw funs from renderer
  - draw_quad(), draw_mesh(), ...
 - [x] make core_data.c load all non custom shaders, framebuffers, etc.
 - [x] rename template_t to entity_template_t

## base
 - [x] load shader
 - [x] load mesh
 - [x] multiple apps
 - [ ] triangularize mesh
 - [ ] asset manager
  - [x] guid
  - [x] dynamic array
  - [x] textures
  - [x] cubemaps
  - [x] meshes
  - [x] ASSETM_NO_ZIP everywhere
  - [ ] load shaders from zip
  - [x] materials 
  - [ ] handle missing assets
  - [x] use __ LINE __ and __ FILE __ in assetm to get better error msg's
  - [x] easy .zip archives generation
 - [x] use mega-struct for selected_id, gizmo, etc.
 - [x] 'dead flag' entity system
 - [x] material system 2.0
  - [x] shaders as tables
  - [x] materials as table
  - [x] materials have variable shaders
  - [x] custom shader uniforms
 - [ ] structures (prefabs)
  - [ ] create / load
  - [ ] store ?
 - [ ] setup 32bit (-m32)
 - [ ] level system ?
  - not sure if necessary since only 1 level
  - [ ] structure
  - [x] serialization
  - [x] deserialization
 - [ ] particle system
 - [x] set cam pos in program_start()
 - [ ] debug tools
  - [x] make debug timer 
  - [ ] add profiler

## renderer
 - [x] render mesh basic
  - [x] basics 
  - [x] indexed draw
  - [x] normals in vert
 - [x] deferred renderer
 - [x] toon lighting
 - [x] anti aliasing
 - [x] cubemap
 - [x] shadows
 - [x] pbr
  - [x] basics
  - [x] point lights
  - [x] dir lights
  - [x] ibl
    - [x] diffuse iradiance
    - [x] specular ibl
 - [x] anisotropic filtering
 - [x] debug draw
 - [x] value or texture system
 - [x] shadows again 
 - [x] shadowmap view mat follow camera
 - [x] line draw !!!
 - [ ] point lights ?
 - [ ] blending
  - learnopengl weighted blending 'OIT'
 - [ ] cascaded shadows
  - from cherno video 
  - also on learnopengl
 - [ ] tiling
 - [ ] ssao
 - [ ] bloom
 - [ ] custom anti aliasing
 - [ ] water 
 - [ ] maybe do outlines ?      

## entity system
 - [x] define entity struct
 - [x] array of entities
 - [x] add / remove / insert
 - [x] parent <-> child
 - [x] init & update

## level editor
 - [x] select entities
 - [x] dont run init & update at start
  - [x] play / pause
 - [x] properties
  - [x] basics
  - [x] init & update
 - [x] entity browser
  - [x] show names
  - [x] show preview
  - [x] spawn
  - [x] minimizable
 - [x] gizmos
  - [x] translate
  - [x] scale
  - [x] rotate
  - [x] global edit on children / rotated
  - [x] make scale gizmo better
  - [x] make rotate gizmo better
 - [x] entity hierarchy
 - [x] removing
 - [x] duplicating
 - [x] snapping
 - [x] terrain editor
  - [x] select
  - [x] edit height
  - [x] gui
  - [x] leveling tool
  - [x] smoothing tool
  - [x] same at edges
  - [x] saving & loading
  - [x] layouting
 - [x] saving
 - [x] loading
 - [ ] lights editor
  - [ ] set properties
  - [ ] remove lights
  - [ ] add lights
 - [ ] structure editor
  - seperate ?
 - [ ] particle system editor
  - seperate ?

## terrain generation
 - [x] generate mesh
 - [x] normals
 - [x] chunking
 - [x] layouting
 - [x] loading / removing
  - [x] nuklear bugg
    - [x] just generate the meshes and dont display them
          high cost on vram, but simple
          or
          find something thats efficient and ram friendly
 - [x] normals
 - [x] tangents
 - [ ] normals & tangents on edges
  - button in gui to calc for all chunks
 - [x] save & load
 - [x] vert color texture blending

## physics engine
- [x] dynamics
- [ ] sphere
- [ ] aabb
- [ ] plane
- [ ] resolution
- [ ] array of dynamic objs, for optimization
- [ ] sweeping collision
- [ ] cylinder / capsule
- [ ] raycasts ?

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
    - [x] big
  - [ ] bush
  - [ ] fence
  - [ ] old hut

## multithreading
 - pthreads
 - add profiler first
 - [ ] any multithreading
 - [ ] asset loading
 - [ ] physics
 - [ ] rendering
 - [ ] terrain

## audio system
  - using miniaudio
 - [ ] clips
 - [ ] loops
 - [ ] sfx ?
 - [ ] system
  - queue bg music, ambient, etc.

## animation system
 - unsure if necessary
 - [ ] learnopengl.com
 - [ ] system
 - [ ] blending animations

## game design
 - [x] basic game idea
 - [ ] game design doc
 - [ ] ...
 
## hud gui system
 - [ ] colored / img quad
  - [ ] new shader
 - [ ] hook logic like ents
 - [ ] button
 - [ ] simple list ordering


