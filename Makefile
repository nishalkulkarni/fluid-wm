all: fluid_wm

SOURCES = \
		  main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

fluid_wm: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS)

.PHONY: clean
clean:
	rm -f fluid_wm $(OBJECTS)

