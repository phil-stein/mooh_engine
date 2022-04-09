# silent echo calls
# MAKEFLAGS += --silent
# instead silencing individual calls with '@'


# compiler plus flags, -g: debug info, -Wall: all warnings
#  -DEDITOR: compile with editor
CC         = @gcc -g -Wall -DASSET_PATH=\"../../assets/\"	-DEDITOR -DASSETM_NO_ZIP
CC_NO_WARN = @gcc 

NAME = game.exe
OUT  = build\$(NAME)

SRC_DIR   = ..
GLOBL_DIR = $(SRC_DIR)/global
CORE_DIR  = $(SRC_DIR)/core
CORE_TYPES_DIR = $(SRC_DIR)/core/types
ENTITY_DIR  = $(SRC_DIR)/entity
APP_DIR   = .

OBJ_DIR = bin

# core
_OBJS =					 \
	glad.o				 \
	window.o			 \
	file_io.o			 \
	input.o				 \
	renderer.o		 \
	shader.o			 \
	texture.o			 \
	mesh.o				 \
	framebuffer.o	 \
	material.o 		 \
	assetm.o			 \
	state.o				 \
	program.o			 \
	debug_draw.o	
# entity
_OBJS += 				 \
	template.o     \
	test_comp.o
# app
_OBJS += 				 \
	app.o					 \
	camera.o			 \
	gui.o					 
# ufbx
_OBJS += 				 \
	ufbx.o
# zip
_OBJS += 				 \
	zip.o

# put the OBJ_DIR in front of the obj names
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))
OBJS_WIN = $(patsubst %,$(OBJ_DIR)\\%,$(_OBJS))

LIB_DIR = ../../libs

_LIBS = 						\
	glfw3dll 					\
	assimp-vc142-mt		

LIBS     = $(patsubst %,-l %,$(_LIBS))

EXTERNAL_DIR = ../../external
APP_INC			 = .

_INC = 							  				\
		$(EXTERNAL_DIR)	  				\
 		$(SRC_DIR)								\
 		$(APP_INC)				
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

$(OBJ_DIR)/%.o: $(ENTITY_DIR)/%.c
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

run: clean all
	@echo --- $(NAME) --- 
	@$(OUT)



