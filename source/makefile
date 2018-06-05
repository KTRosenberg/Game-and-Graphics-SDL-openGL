CXX         = g++
CC          = gcc
CXXFLAGS    = -O3 -g -Wall -Wextra -std=c++1y
CFLAGS      = -O3 -g -Wall -Wextra
LDFLAGS     = -framework OpenGL -lGLEW -lSDL2 -lSDL2_image  
SDLCFLAGS   = $(shell sdl2-config --cflags)
SDLLIBFLAGS = $(shell sdl2-config --libs)

SOURCES_C   = test.c
SOURCES_CPP = run.cpp file_io.cpp shader.cpp texture.cpp camera.cpp
OBJECTS_C   = $(SOURCES_C:.c=.o)
OBJECTS_CPP = $(SOURCES_CPP:.cpp=.o)

EXECNAME  = run

all: $(EXECNAME)

$(EXECNAME): $(OBJECTS_C) $(OBJECTS_CPP)  
	$(CXX) $^ $(SDLLIBFLAGS) $(LDFLAGS) -o $@

$(OBJECTS_CPP): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(SDLCFLAGS) $< -o $@

$(OBJECTS_C): %.o: %.c
	$(CC) -c $(CFLAGS) $(SDLCFLAGS) $< -o $@
	
clean :
	-rm -f *.o *.core $(EXECNAME)

