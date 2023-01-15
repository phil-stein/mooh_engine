
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
 - [x] phys_obj_t's pos, scl and velocity gets set weird or nan
 - [ ] deserializing scene sometimes adds pink point light infront of camera
 - [x] mouse-pick shader doesnt compile
 - [ ] terrain-chunks dont get culled properly 
 - [x] phys collision response extreme
 - [ ] point light entity *1* is not parented to the player, 
       but the player is it's parent
       aka. parenting is broken prob. in serialization
 - [ ] removing objects in editor causes crash

## optimizations
 - [ ] [multithreading](#multithreading) 
 - [ ] use textures with r = roughness, g = metallic, ...
 - [x] entity_update_global_mat() 'dirtyflag'
 - [ ] change the white tex for no normal
 - [x] make material files a table
 - [ ] occlusion culling 
 - [ ] batch renderer
 - [ ] lod system ?
 - [ ] precompute brdf, etc.

## sus amogus
 - when parenting broke and i fixed it by setting 'is_moved' in 'state_update_global_model()'
   it worked before and something changed in the code that made it no longer work that way
   what exactly changed ???
 - ENTITY_SET_POS() doesnt work properly in program_sync_physics(), need to set velocity to 0

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
 - [ ] replace phys_act & scripts_act with flag
 - [ ] seperate serialization.c into terrain & entities

## tools
 - [x] base project
 - [ ] binary dump
  - write empty scene, terrain, etc. to file
 - [ ] ... 

## base
 - [x] load shader
 - [x] multiple apps
 - [x] load mesh
  - [ ] triangularize mesh ?
  - [x] blender coord sys to mine
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
 - [ ] setup 32bit (-m32) : [tdm docs](https://github.com/jmeubank/tdm-distrib/blob/master/tdm64/core/README-gcc-tdm64.md)
 - [ ] serialization
  - [x] serialization
  - [x] deserialization
  - [x] reload
 - [ ] make lights entities !!!
  - to be abled to parent lights, also to use gizmos
  - sudo-components like bee-engine
    ```c
     
    ```
  - world points to empty all having flag as first thing
    that way entity_t doesn't need light pointers
      ```c
      enum flag_type_t
      {
        FLAG_MESH         = FLAG(0),
        FLAG_POINT_LIGHT  = FLAG(1),
        FLAG_DIR_LIGHT    = FLAG(2),
      }
      struct check_t
      { flag_type_t type; }
      struct entity_t
      {
        flag_type_t type;
        ...
      }
      struct point_light_t
      {
        flag_type_t type;
        ...
      }
      void** world;   // array with all references
      entity_t* ents; // array with all entities
      arrput(ents, entity);
      arrput(world, &ents[ents_len]);
      HAS_FLAG((check_t)world[3].type, FLAG_POINT_LIGHT);
      ```
    - have components and flags, aka. both
      ```c
      // is flag actually necessary ???
      struct entity_t
      {
        component_flag_t comp_flag;
        ...
        int point_light_idx;  // -1 if no point light   
      }
      if (HAS_FLAG(entity.comp_flag, FLAG_POINT_LIGHT) )
      {
        all_point_lights[entity.point_light_idx].intensity;
      }
      also works:
      if (entity.point_light_idx >= 0)
      {
        all_point_lights[entity.point_light_idx].intensity;
      }
      ```
  - [ ] reset all dynamic objects ? 
    - this way we don't have to reload the entire thing 
    - not sure if needed in final game though
    - requires restructuring probably
    - reload suprisingly fast
  - [ ] game saves system
    - single map file, never changed
    - multiple variants of that with changes
      - i.e. 'save01.scene', 'mygame.scene', etc.
 - [ ] event system ?
  - phys collisions ? (or when sync)
  - custom events (day to night, etc.)
 - [ ] particle system
 - [x] set cam pos in program_start()
 - [ ] debug tools
  - [x] make debug timer 
  - [ ] add profiler
 - [ ] controller support ? 

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
 - [x] point lights ?
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
 - [ ] immediate mode renderer, for gui, etc.

## entity system
 - [x] define entity struct
 - [x] array of entities
 - [x] add / remove / insert
 - [x] parent <-> child
 - [x] init & update
 - [ ] structures ?

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
  - [x] draw line for distance
  - [ ] draw line for rotation ?
  - [ ] draw line for scale ?
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
 - [x] lights editor
  - [x] set properties
  - [x] remove lights
  - [x] add lights
 - [x] go back to inital state after play
 - [ ] structure editor
  - seperate ?
 - [ ] particle system editor
  - seperate ?
 - [ ] make gui less sh*t 

## physics engine
 - [x] remove objs
 - [x] dynamics
 - [x] api for forces
 - [ ] sphere
 - [x] aabb
 - [ ] plane
 - [x] resolution
 - [ ] fix buoancy in resolution, actually in phys_debug_draw()
 - [x] array of dynamic objs, for optimization
 - [x] basic char controller in game for testing 
 - [ ] sweeping collision
 - [ ] cylinder / capsule
 - [ ] raycasts ?
  - [ ] octree & ray v. triangle ?

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

## multithreading
 - pthreads
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
 - [ ] batch rendered


