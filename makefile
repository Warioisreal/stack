CFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations \
         -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body \
         -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline \
         -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls \
         -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default \
         -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
         -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector \
         -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer \
         -Wlarger-than=8192 -fPIE -Werror=vla \
         #-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
DEBUG_FLAGS = -DDEBUG
LDFLAGS =
CC = g++
SOURCES = main.cpp stack_func.cpp defender_system.cpp stack_attack.cpp

BUILD_TYPE ?= release

OBJDIR_RELEASE = obj/release
OBJDIR_DEBUG = obj/debug

EXECUTABLE_RELEASE = binary_file
EXECUTABLE_DEBUG = binary_file_debug

ifeq ($(BUILD_TYPE), debug)
    CFLAGS += $(DEBUG_FLAGS)
    OBJDIR = $(OBJDIR_DEBUG)
    EXECUTABLE = $(EXECUTABLE_DEBUG)
else
    OBJDIR = $(OBJDIR_RELEASE)
    EXECUTABLE = $(EXECUTABLE_RELEASE)
endif

OBJECTS = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))

all: release

debug:
	$(MAKE) BUILD_TYPE=debug $(EXECUTABLE_DEBUG)

release:
	$(MAKE) BUILD_TYPE=release $(EXECUTABLE_RELEASE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm -rf obj
	@ rm -f $(EXECUTABLE_RELEASE) $(EXECUTABLE_DEBUG)
