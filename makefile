
# @$(MAKE) -s
# @: silence call to make.exe
# $(MAKE): apparently better than just MAKE
# -s: silence the directory changes when calling sub-makes

# --- all ---

all: editor

clean: core_clean editor_clean game_clean

# --- core ---

# CC_ARGS: 
#		-DEDITOR 							adds a lot like mouse_picking, outline, etc.
#		-DINCLUDE_PLAY_MODE		include play mode pause/play for editor
#		-DGLOBAL_DEBUG				activates P_/PF_ macros and MALLOC()/FREE()/ etc.
# 	-DPF_PRINT_LOCATION 	prints the file and place of all P_/PF_ calls
#		-DDEBUG_TIMER					acitave TIMER_START.../TIMER_STOP.. macros
# 	-DDEBUG_TIMER_PRINT		prints all started & stoppped timers
#		-DDEBUG_DRAW					acivate the debug drawing functionality
#		-DDEBUG_OPENGL				adds glGetError wrapper around all _gl... function macros
#		-DPHYS_DEBUG					debug draw velocity, colliders, etc.
#
#		-DASSETM_NO_ZIP				disable loading from zip file, no longer in use anyway

EDITOR_CORE_NAME ="lib_mooh_core_editor.a" 
EDITOR_CC_ARGS   ="-DGLOBAL_DEBUG -DASSETM_NO_ZIP -DDEBUG_TIMER -DDEBUG_DRAW -DDEBUG_OPENGL -DPHYS_DEBUG -DEDITOR -DINCLUDE_PLAY_MODE"
EDITOR_CORE_BIN		 ="bin_core_editor"

GAME_CORE_NAME 	 ="lib_mooh_core.a" 
GAME_CC_ARGS     ="-DASSETM_NO_ZIP"
GAME_CORE_BIN		 ="bin_core_game"

ASSET_PATH			 ="/Workspace/C/mooh_engine/assets/"

# compile and link to .a
core_editor:
	@$(MAKE) -s -C make -f makefile_core NAME=$(EDITOR_CORE_NAME) CC_ARGS=$(EDITOR_CC_ARGS) BIN_DIR=$(EDITOR_CORE_BIN)
core_game:
	@$(MAKE) -s -C make -f makefile_core NAME=$(GAME_CORE_NAME) CC_ARGS=$(GAME_CC_ARGS) BIN_DIR=$(GAME_CORE_BIN)

# clean .o and .a 
core_clean: core_clean_editor core_clean_game

core_clean_editor:
	@$(MAKE) -s -C make -f makefile_core clean NAME=$(EDITOR_CORE_NAME) BIN_DIR=$(EDITOR_CORE_BIN)
core_clean_game:
	@$(MAKE) -s -C make -f makefile_core clean NAME=$(GAME_CORE_NAME) BIN_DIR=$(GAME_CORE_BIN)

# --- editor ---

# compile and run editor
editor: 
	@$(MAKE) -s -C make -f makefile_editor run ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(EDITOR_CC_ARGS)


# clean .o and .exe
editor_clean:
	@$(MAKE) -s -C make -f makefile_editor clean ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(EDITOR_CC_ARGS)

# just make dont run
editor_make: 
	@$(MAKE) -s -C make -f makefile_editor ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(EDITOR_CC_ARGS)


# --- game ---

# compile and run
game: 
	@$(MAKE) -s -C make -f makefile_game run ASSET_PATH=$(ASSET_PATH)


# clean .o and .exe
game_clean:
	@$(MAKE) -s -C make -f makefile_game clean ASSET_PATH=$(ASSET_PATH)

# just make dont run
game_make: 
	@$(MAKE) -s -C make -f makefile_game ASSET_PATH=$(ASSET_PATH)


# --- tex_viewer ---

# compile and run
tex_viewer: 
	@$(MAKE) -s -C make -f makefile_tex_viewer run ASSET_PATH=$(ASSET_PATH)


# clean .o and .exe
tex_viewer_clean:
	@$(MAKE) -s -C make -f makefile_tex_viewer clean ASSET_PATH=$(ASSET_PATH)

# just make dont run
tex_viewer_make: 
	@$(MAKE) -s -C make -f makefile_tex_viewer ASSET_PATH=$(ASSET_PATH)

