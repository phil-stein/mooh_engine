# silent echo calls
# MAKEFLAGS += --silent
# instead silencing individual calls with '@'


# compiler plus flags, -g: debug info, -Wall: all warnings
CC = gcc -g -Wall

NAME = binary_dump.exe

SRC_DIR      = src
MOOH_EXT_DIR = ../../external
MOOH_SRC_DIR = ../../src
# GLOBAL_DIR   = $(MOOH_SRC_DIR)/global
# MATH_DIR     = $(MOOH_SRC_DIR)/math

OBJ_DIR = bin

_OBJS =				\
	main.o

# put the OBJ_DIR in front of the obj names
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))
OBJS_WIN = $(patsubst %,$(OBJ_DIR)\\%,$(_OBJS))

OBJS +=																	\
	../../build/bin_core_editor/serialization.o	\
	../../build/bin_core_editor/file_io.o	

_INC = $(SRC_DIR)      \
			 $(MOOH_EXT_DIR) \
			 $(MOOH_SRC_DIR) # \
# 		 $(GLOBAL_DIR)   \
# 	   $(MATH_DIR)   	

INC = $(patsubst %,-I %,$(_INC))

LIB_LINK = $(INC) # -L $(LIB_DIR) $(LIBS)

all: $(NAME)
	@echo ---- done ----

# compile all object files to .exe
$(NAME): $(OBJS) 
	$(CC) -o $(NAME) $(OBJS) 


# compile all .c files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
# 	@echo auto .c to .o
	@echo $< -> $@	
	$(CC) -c $< -o $@ $(LIB_LINK)


# prevents make from fideling with files named clean
.PHONY: clean

clean:
	del $(NAME)
	del $(OBJS_WIN) 
	@echo ---- clean done ---- 

cleanm: clean $(NAME)

run: all
	$(NAME)

