# == Front porch ===================================================================================

version.major    = 0
version.minor    = 7
version.revision = 2

# Directories
BDIR ?= bin
IDIR ?= include
LDIR ?= lib
ODIR ?= obj
SDIR ?= src

# Compilers
ifeq ($(CC),cc)
	CC = gcc
endif
CXX ?= g++

# Compile C; $1 = output; $2 = source file; $3 = cflags
OCC  = $(CC) -c -o $1 $2 $3
# Compile C++; $1 = output; $2 = source file; $3 = cflags
OCXX = $(CXX) -c -o $1 $2 $3

# Dynmaic library linker; $1 = output; $2 = source file; $3 = cflags
DLIB = $(CXX) -shared -o $1 $2 $3
# Static library linker / archiver; $1 = output; $2 = source file
SLIB = ar rcs $1 $2
# Executable linker; $1 = output; $2 = source file; $3 = lflags
EXE = $(CXX) -o $1 $2 $3

# General compilation flags
override CFLAGS += -I$(IDIR) -I$(SDIR) \
	-Wextra \
	-Wall \
	-Wno-ignored-qualifiers \
	-Wimplicit-fallthrough=0 \
	-Wundef \
	-Wcast-qual \
	-Wwrite-strings \
	-Wno-expansion-to-defined \
	-Wpointer-arith \
	-fpic
ifndef ($(NO_SYMBOLS))
override CFLAGS += -ggdb
endif
# General linker flags
override LFLAGS += -L$(BDIR) -L$(LDIR)

# Specific flags
override CCFLAGS  += -std=c11
override CXXFLAGS += -std=c++17
override DFLAGS   += -DDEBUG
override RFLAGS   += -O2

ifdef ($(DEBUG))
override CFLAGS += $(DFLAGS)
else
override CFLAGS += $(RFLAGS)
endif

# OS and processor detection
UNAME_S = $(shell uname -s 2> /dev/null)

ifeq ($(OS),Win32) # Win32 detection
	OS = Windows_NT
endif

ifeq ($(OS),Windows_NT) # Windows-specific file-extensions & vars
	DLLEXT = dll
	AR_EXT ?= a
	EXEEXT = .exe

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
	DLLEXT = so
	AR_EXT ?= a
	EXEEXT = 

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
	MINGW := 1
endif
ifeq ($(findstring MSYS_NT,$(UNAME_S)),MSYS_NT) # Test for MSYS
	OS = Unix
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

default: # Empty build target - direct users to 'make help'
	@echo -e "Makefile: missing build target\nTry 'make help' for more info."

# Directory structure
bdir:
	@mkdir -p "$(BDIR)"
ldir:
	@mkdir -p "$(LDIR)"
odir:
	@mkdir -p "$(ODIR)"

# == C compilation function =============================================================
define compile.c # $1 = build target type; $2 = build target name; $3 = source file
echo -e "\t\e[96mCompiling C source $(SDIR)/$3...\e[0m"; \
$(call OCC,"$(ODIR)/$(call MANGLE,$1,$2,$3)",$(SDIR)/$3,$(CFLAGS) \
	$(CCFLAGS) $($1.$2.cflags) $($1.$2.ccflags)); \
EXIT_CODE=$$?; \
if [ $$EXIT_CODE -eq 0 ]; then \
	echo -e "\t\t\e[0;32mSuccessfully compiled '$(SDIR)/$3'\e[0m"; \
else \
	echo -e "\t\t\e[0;31m$$EXIT_CODE error(s) compiling '$(SDIR)/$3'\e[0m"; \
fi
endef

# == C++ compilation function ===========================================================
define compile.cpp # $1 = build target type; $2 = build target name; $3 = source file
echo -e "\t\e[96mCompiling C++ source $(SDIR)/$3...\e[0m"; \
$(call OCXX,"$(ODIR)/$(call MANGLE,$1,$2,$3)",$(SDIR)/$3,$(CFLAGS) \
	$(CXXFLAGS) $($1.$2.cflags) $($1.$2.cxxflags)); \
EXIT_CODE=$$?; \
if [ $$EXIT_CODE -eq 0 ]; then \
	echo -e "\t\t\e[0;32mSuccessfully compiled '$(SDIR)/$3'\e[0m"; \
else \
	echo -e "\t\t\e[0;31m$$EXIT_CODE error(s) compiling '$(SDIR)/$3'\e[0m"; \
fi
endef

# == Target declarations ===========================================================================

LIBS += sim
lib.sim.desc    = The SimSoft library (work in progress)
lib.sim.subdirs = libsim \
				  $(if $(filter-out Windows_NT,$(OS)),,libsim/win32) \
				  $(if $(filter-out Unix,$(OS)),,libsim/unix)
lib.sim.cflags  = -DSIM_BUILD \
				  $(if $(filter-out Unix,$(OS)),,-D_POSIX_C_SOURCE)
lib.sim.lflags  = $(if $(filter-out Windows_NT,$(OS)),,-ldbghelp)

EXES += simtest
exe.simtest.desc    = Unit tests for the SimSoft library (work in progress)
exe.simtest.subdirs = simtest simtest/tests
exe.simtest.lflags  = -lsim \
					  $(lib.sim.lflags)

#NEXT_TARGET

# == Back Porch ====================================================================================

RUNTIME_SHELL = $$("$1")

.PHONY: clean all bdir ldir odir help version lib exe

all: $(patsubst %,lib%,$(LIBS)) $(patsubst %,exe%,$(EXES))
	@echo -e "\e[1;92mBuilt all targets:\e[97m" $(patsubst %,"[\e[96m%\e[97m]",$^) "\e[0m"

help:
	@echo -e "SimSoft Makefile version $(version.major).$(version.minor).$(version.revision)\n" \
	"===========================================================================\n" \
	"\e[1;97mUsage:\e[0m\n" \
	"\t'make lib{target} [options]' - Build library target\n" \
	"\t'make exe{target} [options]' - Build executable target\n" \
	"\t'make all [options]' - Build all targets\n" \
	"\n" \
	"\t'make {clean|clean_all}' - Clean all targets\n" \
	"\t'make clean_{lib|exe}{target}' - Clean specific target\n" \
	"\n" \
	"\t'make version' - This Makefile's version number\n" \
	"\n" \
	"\t'make os' - The operating system detected by this Makefile\n" \
	"\t'make cflags' - List the default compiler flags for this Makefile\n" \
	"\t'make lflags' - List the default linker flags for this Makefile\n" \
	"\n" \
	"\e[1;97mOptions:\e[0m\n" \
	"\t\e[3;1mFORCE\e[0m - force all source files in target to recompile when building\n" \
	"\t\e[3;1mCLEAN\e[0m - clean before rebuilding a target\n" \
	"\t\e[3;1mDEBUG\e[0m - compile with debug compiler flags & settings\n" \
	$(if $(filter-out Windows_NT,$(OS)),, \
		"\t\e[3;1mMINGW\e[0m - force building with MinGW (Windows only)\n" \
	) \
	"\t\e[3;1mNO_DYNAMIC_LIB\e[0m - don't output dynamic libraries for library targets\n" \
	"\t\e[3;1mNO_STATIC_LIB\e[0m - don't output static library for library targets\n" \
	"\t\e[3;1mNO_SYMBOLS\e[0m - build without debugging symbols\n" \
	$(if $(filter-out Windows_NT,$(OS)),, \
		"\t\e[3;1mWINDOWS_NO_PDB\e[0m - don't generate a .pdb file (Windows only)\n" \
	) \
	$(if $(filter-out Windows_NT,$(OS)),, \
		"\t\e[3;1mWINDOWS_GUI_BUILD\e[0m - build Windows GUI applications (Windows only)\n" \
	) \
	$(if $(filter-out Windows_NT,$(OS)),, \
		"\t\e[3;1mWINDOWS_UNICODE\e[0m - build w/ Windows unicode support (Windows only)\n" \
	) \
	"\n" \
	"\e[1;97mExamples:\e[0m\n" \
	"\t\e[3mmake libsim NO_SYMBOLS=1 NO_DYNAMIC_LIB=1\e[0m - Build library target\n" \
	"\t\t'libsim' as a static library without debugging symbols\n" \
	"\t\e[3mmake help PAGER=\"less -r\"\e[0m - Pages the output of this help screen\n" \
	"\t\tto pager less\n" \
	"\n" \
	"\e[1;94mLibrary targets:\e[0m\n" \
	$(foreach name, $(LIBS),\
		$(if $(wildcard $(SDIR)/$(lib.$(name).subdir)/.*),\
			"\t[\e[1;96mlib$(name)\e[0m]" \
				$(if $(lib.$(name).desc), "- $(lib.$(name).desc)",) "\n",\
			"\t\e[31m[\e[1;91mlib$(name)\e[0;31m] - Missing source subdir\e[0m\n" \
		)\
	)\
	"\n" \
	"\e[1;94mExecutable targets:\e[0m\n" \
	$(foreach name, $(EXES),\
		$(if $(wildcard $(SDIR)/$(exe.$(name).subdir)/.*),\
			"\t[\e[1;96mexe$(name)\e[0m]" \
				$(if $(exe.$(name).desc), "- $(exe.$(name).desc)",) "\n",\
			"\t\e[31m[\e[1;91mexe$(name)\e[0;31m] - Missing source subdir\e[0m\n" \
		)\
	) \
	$(if $(PAGER),| $(PAGER),)

version:
	@echo $(version.major).$(version.minor).$(version.revision)

os:
	@echo "OS = '$(OS)'"; echo "UNAME_S = '$(UNAME_S)'"

cflags:
	@echo "$(CFLAGS)"

lflags:
	@echo "$(LFLAGS)"

lib%: odir ldir bdir $(if $(CLEAN),clean_lib%,)
	@if [ -z "$(filter $*,$(LIBS))" ]; then \
		echo -e "\e[31mTarget [\e[1;91m$@\e[0m\e[31m] doesn't exist"; exit; fi; \
	if [ -z "$(call GET_SOURCE_FILES,lib,$*)" ]; then \
		echo "Target [$@] contains no source files"; exit; fi; \
	echo -e "\e[1;94mBuilding library [\e[1;96m$*\e[1;94m]...\e[0m"; \
	recombine=0; \
	$(foreach src_file, $(call GET_SOURCE_FILES,lib,$*), \
		$(if $(FORCE),, \
			if [ "$(SDIR)/$(src_file)" -nt "$(ODIR)/$(call MANGLE,lib,$*,$(src_file))" ] || \
				([ -z "$(NO_DYNAMIC_LIB)" ] && \
					[ "$(SDIR)/$(src_file)" -nt "$(BDIR)/$@.$(DLLEXT)" ]) || \
				([ -z "$(NO_STATIC_LIB)" ] && \
					[ "$(SDIR)/$(src_file)" -nt "$(LDIR)/$@.$(AR_EXT)" ]) || \
				([ ! -f "$(BDIR)/$@.$(DLLEXT)" ] && [ -z "$(NO_DYNAMIC_LIB)" ]) || \
				([ ! -f "$(LDIR)/$@.$(AR_EXT)" ] && [ -z "$(NO_STATIC_LIB)" ]) \
			; then \
		) \
			$(call compile.$(lastword $(subst ., ,$(src_file))),lib,$*,$(src_file)); \
			recombine=1; \
		$(if $(FORCE),, \
			else \
				$(if $(call IS_C_SOURCE_FILE,lib,$*,$(src_file)), \
					echo -e "\t\e[94mC source '$(SDIR)/$(src_file)' is up to date\e[0m";, \
				$(if $(call IS_CPP_SOURCE_FILE,lib,$*,$(src_file)), \
					echo -e "\t\e[94mC++ source '$(SDIR)/$(src_file)' is up to date\e[0m";, \
				)) \
			fi; \
		) \
	) \
	linked=1; archived=1; \
	if [ $$recombine -eq 1 ]; then \
		if [ -z "$(NO_DYNAMIC_LIB)" ]; then \
			echo -e "\e[1;96mLinking...\e[0m"; \
			$(call DLIB,$(BDIR)/$@.$(DLLEXT), \
				$(patsubst %,$(ODIR)/%,$(call GET_OBJECT_FILES,lib,$*)),$(LFLAGS) \
				$(lib.$*.lflags) \
			); \
			EXIT_CODE=$$?; \
			if [ $$EXIT_CODE -eq 0 ]; then \
				echo -e "\t\e[0;32mSuccessfully linked dynamic library:" \
					"'$(BDIR)/$@.$(DLLEXT)'\e[0m"; \
			else \
				echo -e "\t\e[0;31mError linking '$(BDIR)/$@.$(DLLEXT)':" \
					"error code $$EXIT_CODE\e[0m"; \
				linked=0; \
			fi; \
		fi; \
		if [ -z "$(NO_STATIC_LIB)" ]; then \
			echo -e "\e[1;96mArchiving...\e[0m"; \
			$(call SLIB,$(LDIR)/$@.$(AR_EXT), \
				$(patsubst %,$(ODIR)/%,$(call GET_OBJECT_FILES,lib,$*)) \
			); \
			EXIT_CODE=$$?; \
			if [ $$EXIT_CODE -eq 0 ]; then \
				echo -e "\t\e[0;32mSuccessfully archived static library:" \
					"'$(LDIR)/$@.$(AR_EXT)'\e[0m"; \
			else \
				echo -e "\t\e[0;31mError archiving '$(LDIR)/$@.$(AR_EXT)'\e[0m"; \
				archived=0; \
			fi; \
		fi; \
	else \
		echo -e "\e[1;90mDone building library [\e[1;96m$*\e[1;90m]; nothing to update\e[0m"; \
		exit; \
	fi; \
	if [ $$linked -eq 1 ] && [ $$archived -eq 1 ]; then \
		echo -e "\e[1;92mDone building library [\e[1;96m$*\e[1;92m]\e[0m"; \
	else \
		echo -e "\e[1;33mDone building library [\e[1;96m$*\e[1;33m]," \
			"with problems (see above)\e[0m"; \
	fi; echo;

clean_lib%:
	@if [ -z $(filter $*,$(LIBS)) ]; then \
		echo -e "\e[31mTarget [\e[1;91mlib$*\e[0;31m] doesn't exist"; exit; \
	else \
		echo -e "Cleaning library [\e[1;96mlib$*\e[0m]..."; fi; \
	rm -f $(patsubst %,$(ODIR)/%,$(call GET_OBJECT_FILES,lib,$*)); \
	$(if $(lib.$*.subdir),rm -r -f $(ODIR)/$(lib.$*.subdir);,) \
	rm -f $(LDIR)/lib$*.$(AR_EXT); rm -f $(BDIR)/lib$*.$(DLLEXT); \
	echo -e "Done cleaning library [\e[1;96m$*\e[0m]"

exe%: odir ldir bdir $(if $(CLEAN),clean_exe%,)
	@if [ -z $(filter $*,$(EXES)) ]; then \
		echo -e "\e[31mTarget [\e[1;91m$@\e[0m\e[31m] doesn't exist"; exit; fi; \
	$(if $(call GET_SOURCE_FILES,exe,$*),,echo Target [$@] contains no source files; exit;) \
	echo -e "\e[1;94mBuilding executable [\e[1;96m$*\e[1;94m]...\e[0m"; \
	recombine=0; \
	$(foreach src_file, $(call GET_SOURCE_FILES,exe,$*), \
		$(if $(FORCE),, \
			if [ "$(SDIR)/$(src_file)" -nt "$(ODIR)/$(call MANGLE,exe,$*,$(src_file))" ] || \
				[ $(SDIR)/$(src_file) -nt $(BDIR)/$*$(EXEEXT) ] || \
				[ ! -f $(BDIR)/$*$(EXEEXT) ]; then \
		) \
		$(call compile.$(lastword $(subst ., ,$(src_file))),exe,$*,$(src_file)); \
		exit_code=$$?; \
		if [ $$exit_code -ne 0 ]; then \
			echo -e \
				"\e[1;31mCouldn't build executable [\e[1;96m$*\e[1;31m]; see above\e[0m" \
			; \
			exit; \
		fi; \
		recombine=1; \
		$(if $(FORCE),, \
			else \
				$(if $(call IS_C_SOURCE_FILE,exe,$*,$(src_file)), \
					echo -e "\t\e[94mC source '$(SDIR)/$(src_file)' is up to date\e[0m";, \
				$(if $(call IS_CPP_SOURCE_FILE,exe,$*,$(src_file)), \
					echo -e "\t\e[94mC++ source '$(SDIR)/$(src_file)' is up to date\e[0m";, \
				)) \
			fi; \
		) \
	) \
	linked=1; \
	if [ $$recombine -eq 1 ]; then \
	echo -e "\e[1;96mLinking...\e[0m"; \
	$(call EXE,$(BDIR)/$*$(EXEEXT), \
		$(patsubst %,$(ODIR)/%,$(call GET_OBJECT_FILES,exe,$*)), \
		$(LFLAGS) $(exe.$*.lflags) \
	); \
		EXIT_CODE=$$?; \
		if [ $$EXIT_CODE -eq 0 ]; then \
			echo -e "\t\e[0;32mSuccessfully linked executable: '$(BDIR)/$*$(EXEEXT)'\e[0m"; \
		else \
			echo -e "\t\e[0;31mError linking '$(BDIR)/$*$(EXEEXT)': error code $$EXIT_CODE\e[0m"; \
			linked=0; \
		fi; \
	if [ $(OS) = Windows_NT ] && [ -z "$(NO_SYMBOLS)" ] && [ -f $(wildcard cv2pdb$(EXEEXT)) ] && \
		[ -z "$(WINDOWS_NO_PDB)" ]; \
	then \
		echo -e "\e[1;96mGenerating .pdb...\e[0m"; \
		./cv2pdb -C $(BDIR)/$*$(EXEEXT); \
		echo -e "\t\e[0;32mSuccessfully generated symbol database"; fi; \
	else \
		echo -e "\e[1;90mDone building executable [\e[1;96m$*\e[1;90m];" \
			"nothing to update\e[0m"; exit; \
	fi; \
	if [ $$linked -eq 1 ]; then \
		echo -e "\e[1;92mDone building executable [\e[1;96m$*\e[1;92m]\e[0m"; \
	else \
		echo -e "\e[1;33mDone building executable [\e[1;96m$*\e[1;33m]," \
			"with problems (see above)\e[0m"; \
	fi; echo;

clean_exe%:
	@if [ -z $(filter $*,$(EXES)) ]; then \
		echo -e "\e[31mTarget [\e[1;91mexe$*\e[0;31m] doesn't exist"; exit; \
	else \
		echo -e "Cleaning executable [\e[1;96mexe$*\e[0m]..."; fi; \
	rm -f $(patsubst %,$(ODIR)/%,$(call GET_OBJECT_FILES,exe,$*)); \
	$(if $(exe.$*.subdir),rm -r -f $(ODIR)/$(exe.$*.subdir);,) \
	rm -f $(BDIR)/$*$(EXEEXT); rm -f $(BDIR)/$*.pdb; \
	echo -e "Done cleaning executable [\e[1;96m$*\e[0m]"

clean: $(foreach lib, $(LIBS), clean_lib$(lib)) $(foreach exe, $(EXES), clean_exe$(exe))

clean_all: clean
