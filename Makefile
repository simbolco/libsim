# == Front porch ===================================================================================

version.major    = 0
version.minor    = 9
version.revision = 0

# General compilation flags
CFLAGS = -Iinclude -Isrc \
	-Wextra \
	-Wall \
	-Wno-ignored-qualifiers \
	-Wimplicit-fallthrough=0 \
	-Wundef \
	-Wcast-qual \
	-Wwrite-strings \
	-Wno-expansion-to-defined \
	-Wpointer-arith
ifndef ($(NO_SYMBOLS))
CFLAGS += -ggdb
endif
# General linker flags
LFLAGS += -Lbin -Llib

# Specific flags
CCFLAGS  = -std=c11
CXXFLAGS = -std=c++17
ifneq ($(DEBUG),"")
DFLAGS   = -DDEBUG=$(DFLAGS)
else
DFLAG    = -DDEBUG=1
endif
RFLAGS   = -O2

ifdef ($(DEBUG))
CFLAGS += $(DFLAGS)
else
CFLAGS += $(RFLAGS)
endif

# OS and processor detection
UNAME_S := $(shell uname -s 2> /dev/null)

ifeq ($(OS),Win32) # Win32 detection
	OS = Windows_NT
endif

ifeq ($(OS),Windows_NT) # Windows-specific file-extensions & vars
	SOEXT = dll
	AREXT ?= a

ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
	CFLAGS += -DAMD64
	PROCESSOR = x64
endif
ifeq ($(PROCESSOR_ARCHITECTURE),x86)
	CFLAGS += -DIA32
	PROCESSOR = x86
endif
#						  Unix vars
else
	SOEXT = so
	AREXT ?= a

ifeq ($(UNAME_S),Linux) # Linux
	CFLAGS += -DLinux
	OS = Unix
endif
ifeq ($(UNAME_S),Darwin) # MacOS
	CFLAGS += -DmacOS
	OS = Unix
endif

	UNAME_P := $(shell uname -p)
ifeq ($(UNAME_P),x86_64)
	CFLAGS += -DAMD64
	PROCESSOR = x64
endif
ifneq ($(filter %86,$(UNAME_P)),)
	CFLAGS += -DIA32
	PROCESSOR = x86
endif
ifneq ($(filter arm%,$(UNAME_P)),)
	CFLAGS += -DARM
	PROCESSOR = ARM
endif
endif

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW) # Test for MinGW
	MINGW ?= 1
endif
ifeq ($(findstring MSYS_NT,$(UNAME_S)),MSYS_NT) # Test for MSYS
	OS = Unix
	CFLAGS += -mno-cygwin
endif

ifeq ($(OS),Windows_NT) # Move Windows cflags to after MSYS detection
	CFLAGS += -DWIN32
ifeq ($(PROCESSOR),x64)
	CFLAGS += -DWIN64
endif
endif

ifeq ($(OS),Unix) # General Unix flag for all Unix-likes
	CFLAGS += -DUnix
endif

# Add Windows GUI build c/lflags when enabled and MinGW lflag when on MinGW
ifeq ($(OS),Windows_NT)
override LFLAGS += $(if $(WINDOWS_GUI_BUILD),-mwindows,) \
				   $(if $(MINGW),-lmingw32,)
override CFLAGS += $(if $(WINDOWS_GUI_BUILD),-DWIN32_GUI_BUILD,) \
			       $(if $(WINDOWS_UNICODE),-DUNICODE,)
endif

# Space
space :=
space +=

# Get source filenames from directory
GET_C_SOURCE_FILES = $(foreach subdir, $($1.$2.subdirs), \
	$(patsubst $(SDIR)/%,%,$(wildcard $(SDIR)/$(subdir)/*.c)))
GET_CPP_SOURCE_FILES = $(foreach subdir, $($1.$2.subdirs), \
	$(patsubst $(SDIR)/%,%,$(wildcard $(SDIR)/$(subdir)/*.cpp)))
GET_SOURCE_FILES = $(filter-out \
	$(foreach subdir, $($1.$2.subdirs), $(patsubst %,$(subdir)/%,$($1.$2.excludes))), \
	$(call GET_C_SOURCE_FILES,$1,$2) \
	$(call GET_CPP_SOURCE_FILES,$1,$2) \
)

# Query file type
IS_C_SOURCE_FILE = $(if $(filter $3,$(call GET_C_SOURCE_FILES,$1,$2)),1,)
IS_CPP_SOURCE_FILE = $(if $(filter $3,$(call GET_CPP_SOURCE_FILES,$1,$2)),1,)

# Mangle source filename to object filename
MANGLE = $(subst $(space),,$(if $(call IS_C_SOURCE_FILE,$1,$2,$3), \
	$(patsubst %.c.,cc.%.o,$(subst $(space),,$(foreach word, $(subst /,  ,$3), $(word).))) \
,$(if $(call IS_CPP_SOURCE_FILE,$1,$2,$3), \
	$(patsubst %.cpp.,cxx.%.o,$(subst $(space),,$(foreach word, $(subst /,  ,$3), $(word).))) \
,)))

# Get object files
GET_OBJECT_FILES = $(foreach file, $(call GET_SOURCE_FILES,$1,$2), $(call MANGLE,$1,$2,$(file)))

default: all

# Directory structure
bin:
	mkdir -p bin
docs:
	mkdir -p docs
lib:
	mkdir -p lib
obj:
	mkdir -p obj
obj/%:
	mkdir -p "obj/$*"

# == Compilation targets ===========================================================================

obj/%.c.o: obj src/%.c
	gcc -c -o obj/$*.c.o src/$*.c $(CFLAGS) $(CCFLAGS)
obj/%.cpp.o: obj src/%.cpp
	g++ -c -o obj/$*.cpp.o src/$*.cpp $(CFLAGS) $(CXXFLAGS)

# == Build targets =================================================================================

libsim.description := The SimSoft Library v0.2.0
libsim.vendor := Simon Bolivar & Company Software Entertainment

libsim.objects := $(patsubst src/libsim/%.c,obj/libsim/%.c.o,$(wildcard src/libsim/*.c)) \
				  $(patsubst src/libsim/%.cpp,obj/libsim/%.cpp.o,$(wildcard src/libsim/*.cpp))
libsim.cflags = $(if $(filter-out Unix,$(OS)),,-D_POSIX_C_SOURCE) -Werror
ifeq ($(OS),Windows_NT)
libsim.lflags = -ldbghelp
endif
libsim.objects: CFLAGS += $(libsim.cflags)
libsim.objects: LFLAGS += $(libsim.lflags)
libsim.objects: obj/libsim $(libsim.objects)

libsim.dynamic.description := Builds libsim as a dynamic library.
libsim.dynamic.out := bin/libsim.${SOEXT}
libsim.dynamic.cflags = $(libsim.cflags) -fPIC -DDYNLIB_BUILD
libsim.dynamic: CFLAGS += $(libsim.dynamic.cflags)
ifeq ($(OS),Windows_NT)
libsim.dynamic: LFLAGS += $(libsim.lflags) -Wl,--subsystem,windows \
-Wl,--out-implib,$(libsim.dynamic.out).${AREXT}
else
libsim.dynamic: LFLAGS += $(libsim.lflags)
endif
libsim.dynamic: cflags bin libsim.objects
	gcc -shared -o $(libsim.dynamic.out) $(libsim.objects) $(LFLAGS)

libsim.static.description := Builds libsim as a static library.
libsim.static.out  := bin/libsim.${AREXT}
libsim.static: bin libsim.objects
	ar rcs $(libsim.static.out) $(libsim.objects)

libsim.docs.c.description   := Generates documentation for libsim's C API.
libsim.docs.cpp.description := Generates documentation for libsim's C++ API.
libsim.docs.%: docs
	doxygen $*.doxyfile

libsim.version.description := Prints the version of libsim being built.
libsim.version:
	@echo 0.2.0

libsim.clean.description := Cleans all files generated by libsim.
libsim.clean:
	rm -f $(libsim.objects) bin/libsim.$(AREXT) bin/libsim.$(SOEXT)
ifeq ($(OS),Windows_NT)
	rm -f $(libsim.dynamic.out).${AREXT}
endif
	rmdir obj/libsim

libsim.targets = static dynamic docs.c docs.cpp version clean
libsim: $(if $(NO_STATIC_LIB),,libsim.static) $(if $(NO_DYNAMIC_LIB),,libsim.dynamic)
TARGETS += libsim

#NEXT_TARGET

# == Back Porch ====================================================================================

RUNTIME_SHELL = $$($1)

.PHONY: clean all help version os cflags lflags \
$(foreach target,$(TARGETS),$(target) $(foreach t,$(target).targets,$(target).$(t)))

all: $(TARGETS)
clean: $(foreach target,$(TARGETS),$(target).clean)

help:
	@echo -e "SimSoft Makefile version $(version.major).$(version.minor).$(version.revision)\n" \
	"===========================================================================\n" \
	"\e[1;97mUsage:\e[0m\n" \
	"  'make {target} [options]' - Build target\n" \
	"  'make {target}.{subtarget} [options]' - Build subtarget\n" \
	"  'make all [options] || make [options]' - Build all targets\n" \
	"\n" \
	"  'make clean' - Clean all targets\n" \
	"  'make {target}.clean' - Clean specific target\n" \
	"\n" \
	"  'make version' - This Makefile's version number\n" \
	"\n" \
	"  'make os' - The operating system detected by this Makefile\n" \
	"  'make cflags' - List the default compiler flags for this Makefile\n" \
	"  'make lflags' - List the default linker flags for this Makefile\n" \
	"\n" \
	"\e[1;97mOptions:\e[0m\n" \
	"  \e[3;1mDEBUG\e[0m - compile with debug compiler flags & settings\n" \
	$(if $(filter-out Windows_NT,$(OS)),, \
		"  \e[3;1mMINGW\e[0m - force building with MinGW (Windows only)\n" \
	) \
	"  \e[3;1mNO_DYNAMIC_LIB\e[0m - don't output dynamic libraries for library targets\n" \
	"  \e[3;1mNO_STATIC_LIB\e[0m - don't output static library for library targets\n" \
	"  \e[3;1mNO_SYMBOLS\e[0m - build without debugging symbols\n" \
	$(if $(filter-out Windows_NT,$(OS)),, \
		"\n   \e[3;1mWINDOWS_NO_PDB\e[0m - don't generate a .pdb file (Windows only)\n" \
	) \
	$(if $(filter-out Windows_NT,$(OS)),, \
		"  \e[3;1mWINDOWS_GUI_BUILD\e[0m - build Windows GUI applications (Windows only)\n" \
	) \
	$(if $(filter-out Windows_NT,$(OS)),, \
		"  \e[3;1mWINDOWS_UNICODE\e[0m - build w/ Windows unicode support (Windows only)\n" \
	) \
	"\n" \
	"\e[1;94mBuild targets:\e[0m\n" \
	$(foreach target, $(TARGETS), \
		"  [\e[1;96m$(target)\e[0m]\n" \
		$(if $($(target).description),"    $($(target).description)\n",) \
		$(if $($(target).vendor),"    VENDOR: $($(target).vendor)\n",) \
		$(if $($(target).targets), \
			"    SUBTARGETS:\n" \
			$(foreach subtarget, $($(TARGETS).targets), \
				"      \e[3;1m$(subtarget)\e[0m\n" \
				$(if $($(TARGETS).$(subtarget).description),\
					"        $($(TARGETS).$(subtarget).description)\n" \
				) \
			) \
		,) \
	)

version:
	@echo $(version.major).$(version.minor).$(version.revision)

os:
	@echo "OS = '$(OS)'"; echo "UNAME_S = '$(UNAME_S)'"; echo "UNAME_P = '$(UNAME_P)'"

cflags:
	@echo "$(CFLAGS)"

lflags:
	@echo "$(LFLAGS)"
