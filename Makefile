CXXFLAGS ?= -Wall -g
CXXFLAGS += -std=c++17
CXXFLAGS += `pkg-config --cflags x11`
LDFLAGS += `pkg-config --libs x11`

all: fluid_wm

HEADERS = \
		  util.hpp \
		  bsp.hpp \
		  master_stack.hpp \
		  priority_queue.hpp \
		  fluid_wm.hpp
SOURCES = \
		  util.cpp \
		  bsp.cpp \
		  master_stack.cpp \
		  priority_queue.cpp \
		  fluid_wm.cpp \
		  main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

${OBJECTS}: config.hpp

fluid_wm: $(HEADERS) $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f fluid_wm $(OBJECTS)
