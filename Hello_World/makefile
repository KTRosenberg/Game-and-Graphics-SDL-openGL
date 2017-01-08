CXX         = g++
CXXFLAGS    = -O3 -g -Wall -Wextra -std=c++1y
#LDFLAGS    = -lSDL2_image
SDLCFLAGS   = $(shell sdl2-config --cflags)
SDLLIBFLAGS = $(shell sdl2-config --libs)

SOURCES   = hello_main.cpp hello.cpp 
OBJECTS   = $(SOURCES:.cpp=.o)
EXECNAME  = hello

all: $(EXECNAME)

$(EXECNAME): $(OBJECTS)
	$(CXX) $(OBJECTS) $(SDLLIBFLAGS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(SDLCFLAGS) $< -o $@
	
clean :
	-rm -f *.o *.core $(EXECNAME)

