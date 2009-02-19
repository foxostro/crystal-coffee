
# 15-462 project makefile
# Authors: Eric Butler, Frank Palermo

# EDIT THIS VARIABLE TO ADD ADDITIONAL SOURCE FILES
SRCS = app.cpp \
       gui.cpp \
       ldr.cpp \
       staffldr.cpp \
       project.cpp \
       scene.cpp \
       imageio.cpp \
       raytrace.cpp \
       vec/vec.cpp \
       vec/quat.cpp \
       vec/mat.cpp \
       vec/462math.cpp \
       geom/sphere.cpp \
       geom/triangle.cpp \
       geom/watersurface.cpp \
	   effect.cpp

.PHONY: clean
TARGET = project
CXX = g++
UNAME := $(shell uname)
UNAME_M := $(shell uname -m)
CXXFLAGS = -g -O2 -Wall -I"$(shell pwd)"
LDFLAGS =
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

# platform specific flags
ifeq ($(UNAME),Darwin)
CXXFLAGS += -D_OS_X_ -I/usr/X11/include
LDFLAGS += -L/usr/X11/lib
LDFLAGS += -Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib
LDFLAGS += -framework OpenGL -framework GLUT -lpng
else
CXXFLAGS +=
LDFLAGS += -lGL -lGLU -lglut -lpng
endif

# check for '.cpp' suffix to avoid accidentally blasting over source files
TMP_SRCS_NOT_CPP = $(filter-out %.cpp,$(SRCS))
ifneq (,$(TMP_SRCS_NOT_CPP))
$(error "Feeling nervous about '$(TMP_SRCS_NOT_CPP)'; I only know how to \
	 build .cpp files")
endif

all: $(TARGET)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM -MP -c -o $@ $^

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

