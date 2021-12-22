# Alternative GNU Make project makefile autogenerated by Premake

ifndef config
  config=release
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild

SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

# Configurations
# #############################################

ifeq ($(origin CC), default)
  CC = clang
endif
ifeq ($(origin CXX), default)
  CXX = clang++
endif
ifeq ($(origin AR), default)
  AR = ar
endif
INCLUDES += -I../../contrib/lua/src
FORCE_INCLUDE +=
ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
LIBS +=
LDDEPS +=
ALL_LDFLAGS += $(LDFLAGS) -arch x86_64
LINKCMD = $(AR) -rcs "$@" $(OBJECTS)
define PREBUILDCMDS
endef
define PRELINKCMDS
endef
define POSTBUILDCMDS
endef

ifeq ($(config),release)
TARGETDIR = bin/Release
TARGET = $(TARGETDIR)/liblua-lib.a
OBJDIR = obj/Release/lua-lib
DEFINES += -DPREMAKE_COMPRESSION -DCURL_STATICLIB -DPREMAKE_CURL -DNDEBUG -DLUA_USE_MACOSX
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -O3 -w -arch x86_64
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -O3 -w -fno-stack-protector -arch x86_64

else ifeq ($(config),debug)
TARGETDIR = bin/Debug
TARGET = $(TARGETDIR)/liblua-lib.a
OBJDIR = obj/Debug/lua-lib
DEFINES += -DPREMAKE_COMPRESSION -DCURL_STATICLIB -DPREMAKE_CURL -D_DEBUG -DLUA_USE_MACOSX
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -g -w -arch x86_64
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -g -w -arch x86_64

endif

# Per File Configurations
# #############################################


# File sets
# #############################################

GENERATED :=
OBJECTS :=

GENERATED += $(OBJDIR)/lapi.o
GENERATED += $(OBJDIR)/lbaselib.o
GENERATED += $(OBJDIR)/lbitlib.o
GENERATED += $(OBJDIR)/lcode.o
GENERATED += $(OBJDIR)/lcorolib.o
GENERATED += $(OBJDIR)/lctype.o
GENERATED += $(OBJDIR)/ldblib.o
GENERATED += $(OBJDIR)/ldebug.o
GENERATED += $(OBJDIR)/ldo.o
GENERATED += $(OBJDIR)/ldump.o
GENERATED += $(OBJDIR)/lfunc.o
GENERATED += $(OBJDIR)/lgc.o
GENERATED += $(OBJDIR)/linit.o
GENERATED += $(OBJDIR)/liolib.o
GENERATED += $(OBJDIR)/llex.o
GENERATED += $(OBJDIR)/lmathlib.o
GENERATED += $(OBJDIR)/lmem.o
GENERATED += $(OBJDIR)/loadlib.o
GENERATED += $(OBJDIR)/lobject.o
GENERATED += $(OBJDIR)/lopcodes.o
GENERATED += $(OBJDIR)/loslib.o
GENERATED += $(OBJDIR)/lparser.o
GENERATED += $(OBJDIR)/lstate.o
GENERATED += $(OBJDIR)/lstring.o
GENERATED += $(OBJDIR)/lstrlib.o
GENERATED += $(OBJDIR)/ltable.o
GENERATED += $(OBJDIR)/ltablib.o
GENERATED += $(OBJDIR)/ltm.o
GENERATED += $(OBJDIR)/lundump.o
GENERATED += $(OBJDIR)/lutf8lib.o
GENERATED += $(OBJDIR)/lvm.o
GENERATED += $(OBJDIR)/lzio.o
OBJECTS += $(OBJDIR)/lapi.o
OBJECTS += $(OBJDIR)/lbaselib.o
OBJECTS += $(OBJDIR)/lbitlib.o
OBJECTS += $(OBJDIR)/lcode.o
OBJECTS += $(OBJDIR)/lcorolib.o
OBJECTS += $(OBJDIR)/lctype.o
OBJECTS += $(OBJDIR)/ldblib.o
OBJECTS += $(OBJDIR)/ldebug.o
OBJECTS += $(OBJDIR)/ldo.o
OBJECTS += $(OBJDIR)/ldump.o
OBJECTS += $(OBJDIR)/lfunc.o
OBJECTS += $(OBJDIR)/lgc.o
OBJECTS += $(OBJDIR)/linit.o
OBJECTS += $(OBJDIR)/liolib.o
OBJECTS += $(OBJDIR)/llex.o
OBJECTS += $(OBJDIR)/lmathlib.o
OBJECTS += $(OBJDIR)/lmem.o
OBJECTS += $(OBJDIR)/loadlib.o
OBJECTS += $(OBJDIR)/lobject.o
OBJECTS += $(OBJDIR)/lopcodes.o
OBJECTS += $(OBJDIR)/loslib.o
OBJECTS += $(OBJDIR)/lparser.o
OBJECTS += $(OBJDIR)/lstate.o
OBJECTS += $(OBJDIR)/lstring.o
OBJECTS += $(OBJDIR)/lstrlib.o
OBJECTS += $(OBJDIR)/ltable.o
OBJECTS += $(OBJDIR)/ltablib.o
OBJECTS += $(OBJDIR)/ltm.o
OBJECTS += $(OBJDIR)/lundump.o
OBJECTS += $(OBJDIR)/lutf8lib.o
OBJECTS += $(OBJDIR)/lvm.o
OBJECTS += $(OBJDIR)/lzio.o

# Rules
# #############################################

all: $(TARGET)
	@:

$(TARGET): $(GENERATED) $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	$(PRELINKCMDS)
	@echo Linking lua-lib
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning lua-lib
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(GENERATED)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(GENERATED)) rmdir /s /q $(subst /,\\,$(GENERATED))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild: | $(OBJDIR)
	$(PREBUILDCMDS)

ifneq (,$(PCH))
$(OBJECTS): $(GCH) | $(PCH_PLACEHOLDER)
$(GCH): $(PCH) | prebuild
	@echo $(notdir $<)
	$(SILENT) $(CC) -x c-header $(ALL_CFLAGS) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
$(PCH_PLACEHOLDER): $(GCH) | $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) touch "$@"
else
	$(SILENT) echo $null >> "$@"
endif
else
$(OBJECTS): | prebuild
endif


# File Rules
# #############################################

$(OBJDIR)/lapi.o: ../../contrib/lua/src/lapi.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lbaselib.o: ../../contrib/lua/src/lbaselib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lbitlib.o: ../../contrib/lua/src/lbitlib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lcode.o: ../../contrib/lua/src/lcode.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lcorolib.o: ../../contrib/lua/src/lcorolib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lctype.o: ../../contrib/lua/src/lctype.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/ldblib.o: ../../contrib/lua/src/ldblib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/ldebug.o: ../../contrib/lua/src/ldebug.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/ldo.o: ../../contrib/lua/src/ldo.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/ldump.o: ../../contrib/lua/src/ldump.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lfunc.o: ../../contrib/lua/src/lfunc.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lgc.o: ../../contrib/lua/src/lgc.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/linit.o: ../../contrib/lua/src/linit.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/liolib.o: ../../contrib/lua/src/liolib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/llex.o: ../../contrib/lua/src/llex.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lmathlib.o: ../../contrib/lua/src/lmathlib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lmem.o: ../../contrib/lua/src/lmem.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/loadlib.o: ../../contrib/lua/src/loadlib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lobject.o: ../../contrib/lua/src/lobject.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lopcodes.o: ../../contrib/lua/src/lopcodes.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/loslib.o: ../../contrib/lua/src/loslib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lparser.o: ../../contrib/lua/src/lparser.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lstate.o: ../../contrib/lua/src/lstate.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lstring.o: ../../contrib/lua/src/lstring.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lstrlib.o: ../../contrib/lua/src/lstrlib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/ltable.o: ../../contrib/lua/src/ltable.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/ltablib.o: ../../contrib/lua/src/ltablib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/ltm.o: ../../contrib/lua/src/ltm.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lundump.o: ../../contrib/lua/src/lundump.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lutf8lib.o: ../../contrib/lua/src/lutf8lib.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lvm.o: ../../contrib/lua/src/lvm.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/lzio.o: ../../contrib/lua/src/lzio.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(PCH_PLACEHOLDER).d
endif