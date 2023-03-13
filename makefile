
# @$(MAKE) -s
# @: silence call to make.exe
# $(MAKE): apparently better than just MAKE
# -s: silence the directory changes when calling sub-makes

# --- all ---

all: editor

clean: core_clean editor_clean game_clean

# --- core ---

EDITOR_CORE_NAME ="lib_mooh_core_editor.a" 
EDITOR_CC_ARGS   ="-DASSETM_NO_ZIP -DDEBUG_TIMER -DDEBUG_DRAW -DDEBUG_OPENGL -DPHYS_DEBUG -DEDITOR -DINCLUDE_PLAY_MODE"
# EDITOR_CC_ARGS   ="-DDEBUG_TIMER -DDEBUG_DRAW -DPHYS_DEBUG -DEDITOR -DINCLUDE_PLAY_MODE"
EDITOR_CORE_BIN		 ="bin_core_editor"

GAME_CORE_NAME 	 ="lib_mooh_core.a" 
# GAME_CC_ARGS     ="-DASSETM_NO_ZIP -DDEBUG_TIMER -DDEBUG_DRAW -DPHYS_DEBUG"
GAME_CC_ARGS     ="-DASSETM_NO_ZIP"
GAME_CORE_BIN		 ="bin_core_game"

# ASSET_PATH			 = -DASSET_PATH=\"../assets/\"
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
	@$(MAKE) -s -C make -f makefile_editor run ASSET_PATH=$(ASSET_PATH)


# clean .o and .exe
editor_clean:
	@$(MAKE) -s -C make -f makefile_editor clean ASSET_PATH=$(ASSET_PATH)

# just make dont run
editor_make: 
	@$(MAKE) -s -C make -f makefile_editor ASSET_PATH=$(ASSET_PATH)


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

