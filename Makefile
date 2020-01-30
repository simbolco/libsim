# == Front porch ===================================================================================

version.major    = 0
version.minor    = 3
version.revision = 5

ifneq ($(WINCMD),)
RMDIR = rmdir /Q $1; mkdir $1
RM    = del
MKDIR = if not exist "$1" mkdir "$1"
SHELL = cmd
else
RMDIR = rmdir --ignore-fail-on-non-empty $1
RM    = rm -f
MKDIR = mkdir -p $1
SHELL = sh
endif

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

# Compile C
OCC  = $(CC) -c -o $1 $2 $3
# Compile C++
OCXX = $(CXX) -c -o $1 $2 $3

# Dynmaic library linker
DLIB = $(CXX) -shared -o $1 $2 $3
# Static library linker / archiver
SLIB = ar rcs $1 $2
# Executable linker
EXE = $(CXX) -o $1 $2 $3

# General compilation flags
CFLAGS ?= -I$(IDIR) -I$(SDIR) \
	-Wextra \
	-Wall \
	-Wno-ignored-qualifiers \
	-Wimplicit-fallthrough=0 \
	-Wundef \
	-Wcast-qual \
	-Wwrite-strings \
	-Wno-expansion-to-defined \
	-Wpointer-arith \
	-fPIC
ifndef ($(NO_SYMBOLS))
CFLAGS += -ggdb
endif
# General linker flags
LFLAGS ?= -L$(BDIR) -L$(LDIR)

# Specific flags
CCFLAGS  ?= -std=c11
CXXFLAGS ?= -std=c++17
DFLAGS   ?= -DDEBUG
RFLAGS   ?= -O2

ifdef ($(DEBUG))
	CFLAGS += $(DFLAGS)
else
	CFLAGS += $(RFLAGS)
endif

# OS and processor detection
ifeq ($(OS),Win32)
	OS = Windows_NT
endif
ifeq ($(OS),Windows_NT)
	DLLEXT = dll
	AR_EXT ?= a
	EXEEXT = .exe

	CFLAGS += -DWIN32

	WIN32_PDB ?= TRUE

ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
	CFLAGS += -DAMD64 -DWIN64
endif
ifeq ($(PROCESSOR_ARCHITECTURE),x86)
	CFLAGS += -DIA32
endif

ifdef MINGW
	CFLAGS += -DMinGW32
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
	CFLAGS += -DMinGW64
endif
endif

else
	DLLEXT = so
	AR_EXT ?= a
	EXEEXT = 

ifeq ($(UNAME_S),Linux)
	CFLAGS += -DLinux
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -DmacOS
endif

	UNAME_P := $(shell uname -p)
ifeq ($(UNAME_P),x86_64)
	CFLAGS += -DAMD64
endif
ifneq ($(filter %86,$(UNAME_P)),)
	CFLAGS += -DIA32
endif
ifneq ($(filter arm%,$(UNAME_P)),)
	CFLAGS += -DARM
endif
endif

# Makefile location
location = $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
MAKEFILE := $(location)

# Get objects
GETOBJS = $(patsubst $(SDIR)/$($1.$2.subdir)/%.c,$($1.$2.subdir)/cc.%.o,$(filter-out $(patsubst %,\
	$($1.$2.subdir)/%,$($1.$2.excludes)), $(wildcard $(SDIR)/$($1.$2.subdir)/*.c)))\
$(patsubst $(SDIR)/$($1.$2.subdir)/%.cpp,$($1.$2.subdir)/cxx.%.o,$(filter-out $(patsubst %,\
	$($1.$2.subdir)/%,$($1.$2.excludes)), $(wildcard $(SDIR)/$($1.$2.subdir)/*.cpp)))

default:
	@echo -e "Makefile: missing build target\nTry 'make help' for more info."

bdir:
	@$(call MKDIR,$(BDIR))
ldir:
	@$(call MKDIR,$(LDIR))
odir:
	@$(call MKDIR,$(ODIR))
odir/%:
	@$(call MKDIR,$(ODIR)/$*)

cc.%.o: odir
	@$(if $(wildcard $(SDIR)/$*.c),,$(error Source file $(SDIR)/$*.c not found))
	@echo -e "\t\e[96mCompiling source $(SDIR)/$*.c...\e[0m"
	@$(call OCC,$(ODIR)/$@,$(SDIR)/$*.c,$(CFLAGS) $(CCFLAGS)); \
		EXIT_CODE=$$?; \
		if [ $$EXIT_CODE -eq 0 ]; then \
			echo -e "\t\e[0;32mSuccessfully compiled C source '$(SDIR)/$*.c'\e[0m"; \
		else \
			echo -e "\t\e[0;31m$$EXIT_CODE error(s) compiling '$(SDIR)/$*.c'\e[0m"; \
		fi; exit $$EXIT_CODE

cxx.%.o: odir
	@$(if $(wildcard $(SDIR)/$*.cpp),,$(error Source file $(SDIR)/$*.cpp not found))
	@echo -e "\t\e[96mCompiling source $(SDIR)/$*.cpp...\e[0m"
	@$(call OCXX,$(ODIR)/$@,$(SDIR)/$*.cpp,$(CFLAGS) $(CXXFLAGS)); \
		EXIT_CODE=$$?; \
		if [ $$EXIT_CODE -eq 0 ]; then \
			echo -e "\t\e[0;32mSuccessfully compiled C++ source '$(SDIR)/$*.cpp'\e[0m"; \
		else \
			echo -e "\t\e[0;31m$$EXIT_CODE error(s) compiling '$(SDIR)/$*.cpp'\e[0m"; \
		fi; exit $$EXIT_CODE

# == Target declarations ===========================================================================

LIBS += sim
lib.sim.desc   = The SimSoft library
lib.sim.subdir = libsim
lib.sim.cflags = -DSIM_BUILD
ifeq ($(OS),Windows_NT)
ifdef MINGW
	lib.sim.lflags += -lmingw32
endif
lib.sim.lflags += -ldbghelp
endif


EXES += simtest
exe.simtest.subdir = simtest
ifeq ($(OS),Windows_NT)
ifdef MINGW
	exe.simtest.lflags += -lmingw32
endif
endif
exe.simtest.lflags += -lsim


EXES += claysol
exe.claysol.subdir = claysoldiers
ifeq ($(OS),Windows_NT)
ifdef MINGW
	exe.claysol.lflags += -lmingw32
endif
endif
exe.claysol.lflags += -lsim
exe.claysol.lflags += -ldbghelp


# == Back Porch ====================================================================================

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
	"\e[1;97mOptions:\e[0m\n" \
	"\t\e[3;1mDEBUG\e[0m - compile with debug compiler flags & settings\n" \
	"\t\e[3;1mMINGW\e[0m - build using MinGW (for Windows only)\n" \
	"\t\e[3;1mNO_DYNAMIC_LIB\e[0m - don't output dynamic libraries for library targets\n" \
	"\t\e[3;1mNO_STATIC_LIB\e[0m - don't output static library for library targets\n" \
	"\t\e[3;1mNO_SYMBOLS\e[0m - build without debugging symbols\n" \
	"\n" \
	"\e[1;97mExamples:\e[0m\n" \
	"\t\e[3mmake libsim NO_SYMBOLS=1 NO_DYNAMIC_LIB=1\e[0m - Build library target\n" \
	"\t\t'libsim' as a static library without debugging symbols\n" \
	"\n" \
	"\e[1;94mLibrary targets:\e[0m\n" \
	$(foreach name, $(LIBS),\
		$(if $(wildcard $(SDIR)/$(lib.$(name).subdir)/.*),\
			"\t[\e[1;96mlib$(name)\e[0m]" \
				$(if $(lib.$(name).desc), "- $(lib.$(name).desc)",),\
			"\t\e[31m[\e[1;91mlib$(name)\e[0;31m] - Missing source subdir\e[0m\n" \
		)\
	)\
	"\n" \
	"\e[1;94mExecutable targets:\e[0m\n" \
	$(foreach name, $(EXES),\
		$(if $(wildcard $(SDIR)/$(exe.$(name).subdir)/.*),\
			"\t[\e[1;96mexe$(name)\e[0m]\n" \
				$(if $(exe.$(name).desc), "- $(exe.$(name).desc)",),\
			"\t\e[31m[\e[1;91mexe$(name)\e[0;31m] - Missing source subdir\e[0m\n" \
		)\
	) \
	> makefile_usage.txt; \
	less -r makefile_usage.txt; \
	if [ $$? -eq 127 ]; then \
		cat makefile_usage.txt; \
	fi; $(RM) makefile_usage.txt

version:
	@echo $(version.major).$(version.minor).$(version.revision)

clean:
	@echo Cleaning all build targets...
	@$(call RMDIR,$(ODIR));$(RM) $(patsubst %,$(BDIR)/lib%.$(DLLEXT),$(LIBS)); \
	$(RM) $(patsubst %,$(LDIR)/lib%.$(AR_EXT),$(LIBS)); \
	$(RM) $(patsubst %,$(BDIR)/%$(EXEEXT),$(EXES)); \
	$(RM) $(patsubst %,$(BDIR)/%.pdb,$(EXES)); \
	echo Done cleaning

clean_all: clean

lib%: odir ldir bdir
	@if [ -d $(filter $*,$(LIBS)) ]; then \
		echo -e "\e[31mTarget [\e[1;91m$@\e[0m\e[31m] doesn't exist"; exit; fi; \
	$(if $(lib.$*.subdir),$(MAKE) -s -f '$(MAKEFILE)' odir/$(lib.$*.subdir);,) \
	$(if $(call GETOBJS,lib,$*),,echo Target [$@] contains no source files; exit;) \
	echo -e "\e[1;94mBuilding library [\e[1;96m$*\e[1;94m]...\e[0m"; \
	$(MAKE) -s -f '$(MAKEFILE)' $(call GETOBJS,lib,$*) 'CFLAGS=$(CFLAGS) $(lib.$*.cflags)'; \
	SUCCESS1=1; SUCCESS2=1; \
	if [ -d $(NO_DYNAMIC_LIB) ]; then \
		echo -e "\e[1;96mLinking...\e[0m"; \
		$(call DLIB,$(BDIR)/$@.$(DLLEXT),$(patsubst %,$(ODIR)/%,$(call GETOBJS,lib,$*)),$(LFLAGS) \
			$(lib.$*.lflags)); \
		EXIT_CODE=$$?; \
		if [ $$EXIT_CODE -eq 0 ]; then \
			echo -e "\t\e[0;32mSuccessfully linked dynamic library: '$(BDIR)/$@.$(DLLEXT)'\e[0m"; \
		else \
			echo -e "\e[0;31mError linking '$(BDIR)/$@.$(DLLEXT)': error code $$EXIT_CODE\e[0m"; \
			SUCCESS1=0; \
		fi; \
	fi; \
	if [ -d $(NO_STATIC_LIB) ]; then \
		echo -e "\e[1;96mArchiving...\e[0m"; \
		$(call SLIB,$(LDIR)/$@.$(AR_EXT),$(patsubst %,$(ODIR)/%,$(call GETOBJS,lib,$*))); \
		EXIT_CODE=$$?; \
		if [ $$EXIT_CODE -eq 0 ]; then \
			echo -e "\t\e[0;32mSuccessfully archived static library: '$(LDIR)/$@.$(AR_EXT)'\e[0m"; \
		else \
			echo -e "\t\e[0;31mError archiving '$(LDIR)/$@.$(AR_EXT)'\e[0m"; \
			SUCCESS2=0; \
		fi; \
	fi; \
	if [ $$SUCCESS1 -eq 1 ] && [ $$SUCCESS2 -eq 1 ]; then \
		echo -e "\e[1;94mDone building library [\e[1;96m$*\e[1;94m]\e[0m"; \
	else \
		echo -e "\e[1;33mDone building library [\e[1;96m$*\e[1;33m]," \
			"with problems (see above)\e[0m"; \
	fi;  echo

clean_lib%:
	@if [ -d $(filter $*,$(LIBS)) ]; then \
		echo -e "\e[31mTarget [\e[1;91mlib$*\e[0;31m] doesn't exist"; exit; \
	else \
		echo -e "Cleaning library [\e[1;96mlib$*\e[0m]..."; fi; \
	@$(RM) $(ODIR)/$(lib.$*.subdir)/*.o; \
	$(if $(lib.$*.subdir),$(call RMDIR,$(ODIR)/$(lib.$*.subdir));,) \
	$(RM) $(LDIR)/lib$*.$(AR_EXT); $(RM) $(BDIR)/lib$*.$(DLLEXT); \
	echo -e "Done cleaning library [\e[1;96m$*\e[0m]"

exe%: odir ldir bdir
	@if [ -d $(filter $*,$(EXES)) ]; then \
		echo -e "\e[31mTarget [\e[1;91m$@\e[0m\e[31m] doesn't exist"; exit; fi; \
	$(if $(exe.$*.subdir),$(MAKE) -s -f '$(MAKEFILE)' odir/$(exe.$*.subdir);,) \
	$(if $(call GETOBJS,exe,$*),,echo Target [$@] contains no source files; exit;) \
	echo -e "\e[1;94mBuilding executable [\e[1;96m$*\e[1;94m]...\e[0m"; \
	$(MAKE) -s -f '$(MAKEFILE)' $(call GETOBJS,exe,$*) CFLAGS='$(CFLAGS) $(exe.$*.cflags)'; \
	echo -e "\e[1;96mLinking...\e[0m"; \
	SUCCESS=1; \
	$(call EXE,$(BDIR)/$*$(EXEEXT), \
		$(patsubst %,$(ODIR)/%,$(call GETOBJS,exe,$*)),$(LFLAGS) $(exe.$*.lflags) \
	); \
		EXIT_CODE=$$?; \
		if [ $$EXIT_CODE -eq 0 ]; then \
			echo -e "\t\e[0;32mSuccessfully linked executable: '$(BDIR)/$*$(EXEEXT)'\e[0m"; \
		else \
			echo -e "\t\e[0;31mError linking '$(BDIR)/$*$(EXEEXT)': error code $$EXIT_CODE\e[0m"; \
			SUCCESS=0; \
		fi; \
	if [ $(OS) = Windows_NT ] && [ -d $(NO_SYMBOLS) ] && [ ! -d $(wildcard cv2pdb$(EXEEXT)) ]; \
	then \
		echo -e "\e[1;96mGenerating .pdb...\e[0m"; \
		./cv2pdb -C $(BDIR)/$*$(EXEEXT); \
		echo -e "\t\e[0;32mSuccessfully generated symbol database"; fi; \
	if [ $$SUCCESS -eq 1 ]; then \
		echo -e "\e[1;94mDone building executable [\e[1;96m$*\e[1;94m]\e[0m"; \
	else \
		echo -e "\e[1;33mDone building executable [\e[1;96m$*\e[1;33m]," \
			"with problems (see above)\e[0m"; \
	fi;  echo

clean_exe%:
	@if [ -d $(filter $*,$(EXES)) ]; then \
		echo -e "\e[31mTarget [\e[1;91mexe$*\e[0;31m] doesn't exist"; exit; \
	else \
		echo -e "Cleaning executable [\e[1;96mexe$*\e[0m]..."; fi; \
	$(RM) $(ODIR)/$(exe.$*.subdir)/*.o; \
	$(if $(exe.$*.subdir),$(call RMDIR,$(ODIR)/$(exe.$*.subdir));,) \
	$(RM) $(BDIR)/$*$(EXEEXT); $(RM) $(BDIR)/$*.pdb; \
	echo -e "Done cleaning executable [\e[1;96m$*\e[0m]"
