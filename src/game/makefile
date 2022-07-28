# silent echo calls
# MAKEFLAGS += --silent
# instead silencing individual calls with '@'


# compiler plus flags, -g: debug info, -Wall: all warnings
#  -DEDITOR: compile with editor
#  -fcommon: ignore duplicate variable definitions
CC         = @gcc -g -Wall -DASSET_PATH=\"../../assets/\"	-DEDITOR -DASSETM_NO_ZIP -DDEBUG_TIMER -DDEBUG_DRAW -DPHYS_DEBUG 
CC_NO_WARN = @gcc

NAME = game.exe
OUT  = build\$(NAME)

SRC_DIR   = ..
GLOBL_DIR = $(SRC_DIR)/global
CORE_DIR  = $(SRC_DIR)/core
CORE_TYPES_DIR = $(SRC_DIR)/core/types
CORE_DEBUG_DIR = $(SRC_DIR)/core/debug
DATA_DIR  = $(SRC_DIR)/data
PHYS_DIR  = $(SRC_DIR)/phys
APP_DIR   = .

OBJ_DIR = bin

# core
_OBJS =					 			\
	glad.o				 			\
	core_data.o					\
	window.o			 			\
	program.o			 			\
	file_io.o			 			\
	shader.o			 			\
	input.o				 			\
	texture.o			 			\
	mesh.o				 			\
	renderer.o		 			\
	framebuffer.o	 			\
	camera.o						\
	assetm.o			 			\
	state.o				 			\
	material.o		 			\
	debug_draw.o   			\
	debug_timer.o				\
	terrain.o						\
	serialization.o
# data
_OBJS += 				 			\
	entity_template.o   \
	shader_template.o   \
	material_template.o \
	test_comp.o
_OBJS += 							\
	phys_world.o				\
	phys_dynamics.o			\
	phys_collision.o		\
	phys_resolution.o	  \
	phys_debug_draw.o
# app
_OBJS += 				 			\
	app.o					 		#	\
#	gui.o					 			\
# ufbx
_OBJS += 				 			\
	ufbx.o
# zip
_OBJS += 				 			\
	zip.o

# put the OBJ_DIR in front of the obj names
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))
OBJS_WIN = $(patsubst %,$(OBJ_DIR)\\%,$(_OBJS))

LIB_DIR = ../../libs/win64

_LIBS = 						\
	glfw3dll 					

LIBS     = $(patsubst %,-l %,$(_LIBS))

EXTERNAL_DIR = ../../external
APP_INC			 = .

_INC = 							  				\
		$(EXTERNAL_DIR)	  						\
 		$(SRC_DIR)								\
 		$(APP_INC)								\
		$(DATA_DIR)								\
		$(PHYS_DIR)
INC = $(patsubst %,-I %,$(_INC))

LIB_LINK = $(INC) -L $(LIB_DIR) $(LIBS) 


all: $(NAME)
	@echo ---- done ----

# compile all object files to .exe
$(NAME): $(OBJS) 
	$(CC) -o $(OUT) $(OBJS) $(LIB_LINK)


# compile all .c files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(CORE_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(CORE_TYPES_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(CORE_DEBUG_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(DATA_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(PHYS_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(APP_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(EXTERNAL_DIR)/ufbx/%.c
	echo $< -> $@	
	$(CC_NO_WARN) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(EXTERNAL_DIR)/zip/%.c
	echo $< -> $@	
	$(CC_NO_WARN) -c $< -o $@ $(INC)




# prevents make from fideling with files named clean
.PHONY: clean

clean:
	del $(OUT)
	del $(OBJS_WIN) 
	@echo ---- clean done ---- 

# clean and them make all
cleanm: clean all

run: all
	@echo --- $(NAME) --- 
	@$(OUT)

cleanrun: clean all
	@echo --- $(NAME) --- 
	@$(OUT)





