CXXFLAGS ?= -Wall -g
CXXFLAGS += -std=c++17
CXXFLAGS += `pkg-config --cflags x11`
LDFLAGS += `pkg-config --libs x11`

all: fluid_wm

HEADERS = \
		  util.hpp \
		  priority_queue.hpp \
		  window_manager.hpp
SOURCES = \
		  util.cpp \
		  priority_queue.cpp \
		  window_manager.cpp \
		  main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

fluid_wm: $(HEADERS) $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f fluid_wm $(OBJECTS)
