
# @$(MAKE) -s
# @: silence call to make.exe
# $(MAKE): apparently better than just MAKE
# -s: silence the directory changes when calling sub-makes

# --- all ---

all: editor

clean: data_clean core_clean editor_clean game_clean

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
GAME_CC_ARGS     ="-DGLOBAL_DEBUG -DASSETM_NO_ZIP"
GAME_CORE_BIN		 ="bin_core_game"

DATA_NAME 	 		 ="lib_mooh_data.a" 
DATA_CC_ARGS     ="-DGLOBAL_DEBUG "
DATA_BIN		 		 ="bin_data"

TEX_VIEWER_CC_ARGS  ="-DGLOBAL_DEBUG -DASSETM_NO_ZIP"


ASSET_PATH			 ="/Workspace/C/mooh_engine/_assets/"

# compile and link to .a
core_editor:
	@$(MAKE) -s -C _make -f makefile_core NAME=$(EDITOR_CORE_NAME) CC_ARGS=$(EDITOR_CC_ARGS) BIN_DIR=$(EDITOR_CORE_BIN)
core_game:
	@$(MAKE) -s -C _make -f makefile_core NAME=$(GAME_CORE_NAME) CC_ARGS=$(GAME_CC_ARGS) BIN_DIR=$(GAME_CORE_BIN)

# clean .o and .a 
core_clean: core_clean_editor core_clean_game

core_clean_editor:
	@$(MAKE) -s -C _make -f makefile_core clean NAME=$(EDITOR_CORE_NAME) BIN_DIR=$(EDITOR_CORE_BIN)
core_clean_game:
	@$(MAKE) -s -C _make -f makefile_core clean NAME=$(GAME_CORE_NAME) BIN_DIR=$(GAME_CORE_BIN)

# --- data ---

# compile and link to .a
data:
	@$(MAKE) -s -C _make -f makefile_data clean all NAME=$(DATA_NAME) CC_ARGS=$(DATA_CC_ARGS) BIN_DIR=$(DATA_BIN)
# clean .o and .exe
data_clean:
	@$(MAKE) -s -C _make -f makefile_data NAME=$(DATA_NAME) BIN_DIR=$(DATA_BIN) clean


# --- editor ---

# compile and run editor
editor: 
	@$(MAKE) -s -C _make -f makefile_editor run ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(EDITOR_CC_ARGS)


# clean .o and .exe
editor_clean:
	@$(MAKE) -s -C _make -f makefile_editor clean ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(EDITOR_CC_ARGS)

# just make dont run
editor_make: 
	@$(MAKE) -s -C _make -f makefile_editor ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(EDITOR_CC_ARGS)


# --- game ---

# compile and run
game: 
	@$(MAKE) -s -C _make -f makefile_game run ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(GAME_CC_ARGS)


# clean .o and .exe
game_clean:
	@$(MAKE) -s -C _make -f makefile_game clean ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(GAME_CC_ARGS)

# just make dont run
game_make: 
	@$(MAKE) -s -C _make -f makefile_game ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(GAME_CC_ARGS)


# --- tex_viewer ---

# compile and run
tex_viewer: 
	@$(MAKE) -s -C _make -f makefile_tex_viewer run ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(TEX_VIEWER_CC_ARGS)


# clean .o and .exe
tex_viewer_clean:
	@$(MAKE) -s -C _make -f makefile_tex_viewer clean ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(TEX_VIEWER_CC_ARGS)

# just make dont run
tex_viewer_make: 
	@$(MAKE) -s -C _make -f makefile_tex_viewer ASSET_PATH=$(ASSET_PATH) CC_ARGS=$(TEX_VIEWER_CC_ARGS)

