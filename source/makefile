CXX         = g++
CXXFLAGS    = -O3 -g -Wall -Wextra -std=c++1y
LDFLAGS     = -framework OpenGL -lGLEW -lSDL2 -lSDL2_image  
SDLCFLAGS   = $(shell sdl2-config --cflags)
SDLLIBFLAGS = $(shell sdl2-config --libs)

SOURCES   = run.cpp file_io.cpp shader.cpp texture.cpp material.cpp camera.cpp mesh_generator.cpp
OBJECTS   = $(SOURCES:.cpp=.o)
EXECNAME  = prog

all: $(EXECNAME)

$(EXECNAME): $(OBJECTS)
	$(CXX) $(OBJECTS) $(SDLLIBFLAGS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(SDLCFLAGS) $< -o $@
	
clean :
	-rm -f *.o *.core $(EXECNAME)

