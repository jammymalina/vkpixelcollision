TARGET   = vulkanapp

CC       = gcc
DEFINES  = -DVK_NO_PROTOTYPES -DDEBUG
# compiling flags here
# CFLAGS = -std=c11 -flto -O3 -march=native
CFLAGS = -std=c11 -Wall -g3

LINKER   = gcc -o
# linking flags here
LFLAGS   = -flto -O3 -march=native -lm -lSDL2

GLSL_CC = glslc
GLSL_FLAGS =

# change these to set the proper directories where each files shoould be
SRCDIR   = src
OBJDIR   = build/objs
BINDIR   = build/bin

# path to unity testing framework
UNITYDIR = unity/src
TSTDIR = test

SHADER_DIR = shaders
SHADER_SRC_DIR = $(SRCDIR)/$(SHADER_DIR)
SHADER_OBJ_DIR = $(BINDIR)/$(SHADER_DIR)

SOURCES  := $(wildcard $(SRCDIR)/*.c $(SRCDIR)/**/*.c $(SRCDIR)/**/**/*.c      \
	$(SRCDIR)/**/**/**/*.c $(SRCDIR)/**/**/**/**/*.c)

TEST_SOURCES := $(wildcard $(TSTDIR)/*.c $(TSTDIR)/**/*.c $(TSTDIR)/**/**/*.c  \
	$(TSTDIR)/**/**/**/*.c $(TSTDIR)/**/**/**/**/*.c)

SHADER_SOURCES := $(wildcard $(SHADER_SRC_DIR)/basic/*.vert                    \
	$(SHADER_SRC_DIR)/basic/*.frag)

INCLUDE_DIRS := -I $(UNITYDIR)
LIB_DIRS     :=

OBJECTS         := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
SHADER_OBJECTS  := $(SHADER_SOURCES:$(SHADER_SRC_DIR)/%=$(SHADER_OBJ_DIR)/%.svm)

rm       = rm -rf

default: $(BINDIR)/$(TARGET)
all: default

$(BINDIR)/$(TARGET): $(OBJECTS) $(SHADER_OBJECTS)
	@mkdir -p $(BINDIR)
	@$(LINKER) $@ $(LIB_DIRS) $(LFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE_DIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(SHADER_OBJECTS): $(SHADER_OBJ_DIR)/%.svm : $(SHADER_SRC_DIR)/%
	@mkdir -p $(dir $@)
	@$(GLSL_CC) $(GLSL_FLAGS) $< -o $@
	@echo "Compiled "$<" successfully!"


.PHONEY: clean
clean:
	@$(rm) $(OBJDIR)
	@$(rm) $(SHADER_OBJ_DIR)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

valgrind: $(BINDIR)/$(TARGET)
	@valgrind --leak-check=yes --track-origins=yes $(BINDIR)/$(TARGET)
