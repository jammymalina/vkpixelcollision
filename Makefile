TARGET   = vulkanapp

CC       = gcc
DEFINES  = -DVK_NO_PROTOTYPES -DDEBUG
# compiling flags here
# CFLAGS = -std=c11 -flto -O3 -march=native
CFLAGS = -std=c11 -Wall -g3

LINKER   = gcc -o
# linking flags here
LFLAGS   = -flto -O3 -march=native -lm -lSDL2

DEPEND = gcc -MM -MG -MF

GLSL_CC = glslc
GLSL_FLAGS =

# change these to set the proper directories where each files shoould be
SRCDIR   = src
OBJDIR   = build/objs
BINDIR   = build/bin
TSTOBJDIR = test/objs
TSTBINDIR = test/bin
TSTDEPDIR = test/depends

# path to unity testing framework
UNITYDIR = Unity/src
TSTDIR = test
TSTRSTDIR = test/results

SHADER_DIR = shaders
SHADER_SRC_DIR = $(SRCDIR)/$(SHADER_DIR)
SHADER_OBJ_DIR = $(BINDIR)/$(SHADER_DIR)

SOURCES  := $(wildcard $(SRCDIR)/*.c $(SRCDIR)/**/*.c $(SRCDIR)/**/**/*.c      \
	$(SRCDIR)/**/**/**/*.c $(SRCDIR)/**/**/**/**/*.c)
TEST_SOURCES := $(wildcard $(TSTDIR)/*.c)

UNITY_SOURCES := $(wildcard $(UNITYDIR)/*.c)

SHADER_SOURCES := $(wildcard $(SHADER_SRC_DIR)/basic/*.vert                    \
	$(SHADER_SRC_DIR)/basic/*.frag)

INCLUDE_DIRS :=
LIB_DIRS     :=
TEST_INCLUDE_DIRS := -I $(UNITYDIR)

OBJECTS         := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
SHADER_OBJECTS  := $(SHADER_SOURCES:$(SHADER_SRC_DIR)/%=$(SHADER_OBJ_DIR)/%.svm)

UNITY_OBJECTS   := $(UNITY_SOURCES:$(UNITYDIR)/%.c=$(TSTOBJDIR)/%.o)
TEST_OBJECTS    := $(TEST_SOURCES:$(TSTDIR)/%.c=$(TSTOBJDIR)/%.o)
TEST_DEPENDENCIES := $(TEST_SOURCES:$(TSTDIR)/%.c=$(TSTDEPDIR)/%.d)
TEST_BINARIES = $(patsubst $(TSTDIR)test_%.c, $(TSTRSTDIR)test_%, $(TEST_SOURCES))
TEST_RESULTS = $(patsubst $(TSTDIR)test_%.c, $(TSTRSTDIR)test_%.txt, $(TEST_SOURCES))

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

$(TEST_OBJECTS): $(TSTOBJDIR)/%.o : $(TSTDIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(DEFINES) $(TEST_INCLUDE_DIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(UNITY_OBJECTS): $(TSTOBJDIR)/%.o : $(UNITYDIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(DEFINES) $(TEST_INCLUDE_DIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(TEST_DEPENDENCIES): $(TSTDEPDIR)/%.d: $(TSTDIR)/%.c
	@mkdir -p $(dir $@)
	$(DEPEND) $@ $<

$(TSTRSTDIR)/%.txt: $(TEST_BINARIES)
	@mkdir -p $(dir $@)
	-./$< > $@ 2>&1

$(TEST_BINARIES): $(TEST_OBJECTS) $(OBJECTS) $(UNITY_OBJECTS) $(TEST_DEPENDENCIES)
	@mkdir -p $(TSTBINDIR)
	$(LINKER) $@ $^


test: $(TEST_OBJECTS) $(TEST_BINARIES) $(TEST_RESULTS)
	@echo $(TEST_RESULTS)
	@echo "-----------------------\nIGNORES:\n-----------------------"
	@echo `grep -s IGNORE $(TSTRSTDIR)*.txt`
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo `grep -s FAIL $(TSTRSTDIR)*.txt`
	@echo "\nDONE"


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
