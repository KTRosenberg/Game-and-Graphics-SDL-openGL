CXX         = g++
CC          = gcc
CXXDISABLEDFLAGS = -Wno-unused-function -Wno-unused-parameter
CDISABLEDFLAGS = -Wno-unused-function -Wno-unused-parameter
CXXFLAGS    = -O3 -g -Wall -Wextra $(CXXDISABLEDFLAGS) -std=c++1y
CFLAGS      = -O3 -g -Wall -Wextra $(CDISABLEDFLAGS) -std=c11
LDFLAGS     = -framework OpenGL -lGLEW -lSDL2 -lSDL2_image  
SDLCFLAGS   = $(shell sdl2-config --cflags)
SDLLIBFLAGS = $(shell sdl2-config --libs)

SOURCES_C   = c_lib.c
SOURCES_CPP = run.cpp
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

