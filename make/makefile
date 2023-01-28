
# --- all ---

all: editor

clean: core_clean editor_clean

# --- core ---

# compile and link to .a
core:
	@$(MAKE) -f makefile_core


# clean .o and .a 
core_clean:
	$(MAKE) -f makefile_core clean

# --- editor ---

# compile and run editor
editor: 
	@$(MAKE) -f makefile_editor run


# clean .o and .exe
editor_clean:
	@$(MAKE) -f makefile_editor clean

# just make dont run
editor_make: 
	@$(MAKE) -f makefile_editor


