TARGET   = vulkanapp

CC       = gcc
DEFINES  = -DVK_NO_PROTOTYPES -DDEBUG
# CFLAGS   = -std=c11 -flto -O3 -march=native
CFLAGS   = -std=c11 -Wall -g3

LINKER   = gcc -o
LFLAGS   = -flto -O3 -march=native -lm -lSDL2

DEPEND = gcc -MM -MG -MF

GLSL_CC    = glslc
GLSL_FLAGS =

# change these to set the proper directories where each files shoould be
SRCDIR   = src
OBJDIR   = build/obj
BINDIR   = build/bin

SHADER_DIR     = shaders
SHADER_SRC_DIR = $(SRCDIR)/$(SHADER_DIR)
SHADER_OBJ_DIR = $(BINDIR)/$(SHADER_DIR)

SOURCES := $(wildcard $(SRCDIR)/*.c $(SRCDIR)/**/*.c $(SRCDIR)/**/**/*.c       \
	$(SRCDIR)/**/**/**/*.c $(SRCDIR)/**/**/**/**/*.c)

SHADER_SOURCES := $(wildcard $(SHADER_SRC_DIR)/basic/*.vert                    \
	$(SHADER_SRC_DIR)/basic/*.frag)

INCLUDE_DIRS =
LIB_DIRS     =

OBJECTS         := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
SHADER_OBJECTS  := $(SHADER_SOURCES:$(SHADER_SRC_DIR)/%=$(SHADER_OBJ_DIR)/%.svm)

# TEST
TEST_OBJDIR = test/obj
TEST_BINDIR = test/bin
TEST_DEPDIR = test/depends

UNITYDIR = Unity/src
TEST_DIR = test
TEST_RESULTS_DIR = test/results

TEST_INCLUDE_DIRS := -I $(UNITYDIR)

TEST_SOURCES      := $(wildcard $(TEST_DIR)/*.c)
UNITY_SOURCES     := $(wildcard $(UNITYDIR)/*.c)
TEST_DEP_SOURCES  := $(wildcard $(SRCDIR)/**/*.c $(SRCDIR)/**/**/*.c           \
	$(SRCDIR)/**/**/**/*.c $(SRCDIR)/**/**/**/**/*.c)

UNITY_OBJECTS     := $(UNITY_SOURCES:$(UNITYDIR)/%.c=$(TEST_OBJDIR)/%.o)
TEST_OBJECTS      := $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_OBJDIR)/%.o)
TEST_DEP_OBJECTS  := $(TEST_DEP_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TEST_DEPENDENCIES := $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_DEPDIR)/%.d)
TEST_BINARIES     := $(patsubst $(TEST_DIR)/test_%.c, $(TEST_BINDIR)/test_%,   \
	$(TEST_SOURCES))
TEST_RESULTS      := $(patsubst $(TEST_DIR)/test_%.c,                          \
	$(TEST_RESULTS_DIR)/test_%.txt, $(TEST_SOURCES))

rm = rm -rf

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

run: $(BINDIR)/$(TARGET)
	-./$@

# TEST

$(TEST_OBJECTS): $(TEST_OBJDIR)/%.o : $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(DEFINES) $(TEST_INCLUDE_DIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(UNITY_OBJECTS): $(TEST_OBJDIR)/%.o : $(UNITYDIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(DEFINES) $(TEST_INCLUDE_DIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(TEST_DEPENDENCIES): $(TEST_DEPDIR)/%.d: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	$(DEPEND) $@ $<

$(TEST_BINARIES): $(TEST_BINDIR)/% : $(TEST_OBJDIR)/%.o $(TEST_DEPDIR)/%.d $(TEST_DEP_OBJECTS) $(UNITY_OBJECTS)
	@mkdir -p $(TEST_BINDIR)
	$(LINKER) $@ $(LIB_DIRS) $(LFLAGS) $(TEST_DEP_OBJECTS) $(UNITY_OBJECTS) $<

$(TEST_RESULTS): $(TEST_RESULTS_DIR)/%.txt : $(TEST_BINDIR)/%
	@mkdir -p $(dir $@)
	-./$< > $@ 2>&1


test: $(TEST_RESULTS)
	@echo -e $(TEST_RESULTS)
	@echo -e '-----------------------\nIGNORES:\n-----------------------'
	@echo -e `grep -s IGNORE $(TEST_RESULTS_DIR)/*.txt`
	@echo -e '-----------------------\nFAILURES:\n-----------------------'
	@echo -e `grep -s FAIL $(TEST_RESULTS_DIR)/*.txt`
	@echo -e '\nDONE'


.PHONEY: clean
clean:
	@$(rm) $(OBJDIR)
	@$(rm) $(SHADER_OBJ_DIR)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

.PHONEY: clean_test
clean_test:
	@$(rm) $(TEST_OBJDIR)
	@$(rm) $(TEST_DEPDIR)
	@$(rm) $(TEST_BINDIR)
	@$(rm) $(TEST_RESULTS_DIR)
	@echo "Test cleanup complete!"

valgrind: $(BINDIR)/$(TARGET)
	@valgrind --leak-check=yes --track-origins=yes $(BINDIR)/$(TARGET)
