CC = gcc-7
CXX = g++-7

SRCDIR = src/cglass
OBJDIR = obj
BINDIR = bin
SRCEXT = cpp

COMPILE_FLAGS = -std=c++11 
RCOMPILE_FLAGS = -D NDEBUG -O2 -march=native
DCOMPILE_FLAGS = -D DEBUG -O0 -g
#LINK_FLAGS = -gnu 

ifeq ($(THREADING),eomp)
	COMPILE_FLAGS += -fopenmp
	LINK_FLAGS += -fopenmp
endif

ifeq ($(NOGRAPH),true)
	COMPILE_FLAGS += -D NOGRAPH
	LINK_FLAGS += -D NOGRAPH
endif

UNAME_S:=$(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	INCLUDES = -I/opt/X11/include -I/usr/X11R6/include -I/usr/local/include -I/usr/local/include/gsl
	GLXLIBS = -L/opt/X11/lib -L/usr/local/lib -lglfw3 -framework OpenGL -lglew -framework Cocoa -framework IOKit -framework CoreVideo
	GSLLIBS = -lgsl -lgslcblas
	FFTLIBS = -lfftw3
	YAMLINCS = -I/usr/local/include
	YAMLLIBS = -L/usr/local/lib -lyaml-cpp
	LIBS = $(GLXLIBS) $(GSLLIBS) $(FFTLIBS) $(YAMLLIBS)
else
	GSLINCS = -I/usr/local/include
	GSLLIBS = -L/usr/local/lib -lgsl -lgslcblas -lm
	YAMLINCS = -I/usr/local/include
	YAMLLIBS = -L/usr/local/lib -lyaml-cpp
	INCLUDES = $(GLFW3INCS) $(YAMLINCS) $(GSLINCS)
	LIBS = $(GLFW3LIBS) $(YAMLLIBS) $(GSLLIBS)
endif

print-%: ; @echo $*=$($*)

SHELL = /bin/bash

.SUFFIXES:

ifneq ($(LIBS),)
	COMPILE_FLAGS += $(shell pkg-config --cflags $(LIBS))
	LINK_FLAGS += $(shell pkg-config --libs $(LIBS))
endif

# Special stuff for intel compiler
CC=$(CXX)
ifeq ($(CC),icpc)
	COMPILE_FLAGS += -Wno-deprecated
else
	COMPILE_FLAGS += -Wno-deprecated-declarations -Wno-deprecated -Wno-return-type -Wno-parentheses
endif

# Combine compiler and linker flags
ifeq ($(CFG),release)
	export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS) $(RCOMPILE_FLAGS)
	export LDFLAGS := $(LDFLAGS) $(LINK_FLAGS) 
else
	export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS) $(DCOMPILE_FLAGS)
	export LDFLAGS := $(LDFLAGS) $(LINK_FLAGS) 
endif

# build information on all sources
ifeq ($(UNAME_S),Darwin)
	SOURCES = $(shell find $(SRCDIR) -name '*.$(SRCEXT)' | sort -k 1nr | cut -f2-)
else
	SOURCES = $(shell find $(SRCDIR) -name '*.$(SRCEXT)' -printf '%T@\t%p\n' \
			  										| sort -k 1nr | cut -f2-)
endif

# fallback case
rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard,$d/,$2) \
									$(filter $(subst *,%,$2), $d))
ifeq ($(SOURCES),)
	SOURCES := $(call rwildcard, $(SRCDIR), *.$(SRCEXT))
endif

# Now we have to figure out which we are building of the program list, since that matters
# for things like not compiling more than one main
CGLASS_SOURCE = $(SRCDIR)/cglass_main.cpp
CONFIGURE_CGLASS_SOURCE = $(SRCDIR)/cglass_config.cpp
EXCLUDE_SOURCES = 

MAIN_SOURCES = $(CGLASS_SOURCE) $(CONFIGURE_CGLASS_SOURCE)

# These are the common sources
SRCS = $(filter-out $(MAIN_SOURCES) $(EXCLUDE_SOURCES), $(SOURCES))

OBJECTS = $(SRCS:$(SRCDIR)/%.$(SRCEXT)=$(OBJDIR)/%.o)
CGLASS_MAIN_OBJ = $(CGLASS_SOURCE:$(SRCDIR)/%.$(SRCEXT)=$(OBJDIR)/%.o)
CONFIGURE_CGLASS_OBJ = $(CONFIGURE_CGLASS_SOURCE:$(SRCDIR)/%.$(SRCEXT)=$(OBJDIR)/%.o)
DEPS = $(OBJECTS:.o=.d)

.PHONY: dirs
dirs:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)
	$(RM) -r $(BINDIR)

.PHONY : clean-output
clean-output :
	rm -f *.posit *.config *.thermo *.initial_config.* *.final_config *.final_config.* \
	*.checkpoint.* *.checkpoint *.thermo_ext sphero.crosslinks.* test-log

cglass: dirs $(BINDIR)/cglass;cp $(BINDIR)/cglass cglass

cglass_config: dirs $(BINDIR)/cglass_config;cp $(BINDIR)/cglass_config cglass_config

$(BINDIR)/cglass: $(OBJECTS) $(CGLASS_MAIN_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS) 

$(BINDIR)/cglass_config: $(CONFIGURE_CGLASS_OBJ)
	$(CXX) $(LDFLAGS) $(LIBS) $^ -o $@

# add dependencies
-include $(DEPS)


# source file rules
$(OBJDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@ 
