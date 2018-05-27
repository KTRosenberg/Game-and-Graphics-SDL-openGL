CXX         = g++
CXXFLAGS    = -O3 -g -Wall -Wextra -std=c++1y
LDFLAGS     = -framework OpenGL -lSDL2 -lSDL2_image -lglfw
SDLCFLAGS   = $(shell sdl2-config --cflags) -I/usr/local/include -I/Users/synchronizer/Documents/Coding/Game_Projects/SDL_Game_Programming/source/glad/include
SDLLIBFLAGS = $(shell sdl2-config --libs) -L/usr/local/lib

SOURCES   = glad.cpp run.cpp file_io.cpp shader.cpp texture.cpp camera.cpp
OBJECTS   = $(SOURCES:.cpp=.o)

EXECNAME  = run

all: $(EXECNAME)

$(EXECNAME): $(OBJECTS)
	$(CXX) $(OBJECTS) $(SDLLIBFLAGS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(SDLCFLAGS) $< -o $@
%.o: %.c
	$(CXX) -c $(CXXFLAGS) $(SDLCFLAGS) $< -o $@
	
clean :
	-rm -f *.o *.core $(EXECNAME)

