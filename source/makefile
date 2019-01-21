CXX         = g++
CC          = gcc
CXXDISABLEDFLAGS = -Wno-unused-function -Wno-unused-parameter
CDISABLEDFLAGS = -Wno-unused-function -Wno-unused-parameter
CXXFLAGS    = -O3 -Wall -Wextra $(CXXDISABLEDFLAGS) -std=c++17
CFLAGS      = -O3 -Wall -Wextra $(CDISABLEDFLAGS) -std=c11
LDFLAGS     = -framework OpenGL -framework CoreFoundation -framework CoreAudio -framework AudioToolbox -lGLEW -lSDL2 -lSDL2_image -lm -lpthread  
SDLCFLAGS   = $(shell sdl2-config --cflags)
SDLLIBFLAGS = $(shell sdl2-config --libs)

CFLAGS += -MD -MP
CXXFLAGS += -MD -MP
DEPS = $(shell find . -name "*.d") 


SOURCES_C   = c_lib.c
SOURCES_CPP = run.cpp
OBJECTS_C   = $(SOURCES_C:.c=.o)
OBJECTS_CPP = $(SOURCES_CPP:.cpp=.o)

EXECNAME  = run

all: $(EXECNAME)

-include $(DEPS)

$(EXECNAME): $(OBJECTS_C) $(OBJECTS_CPP)  
	$(CXX) $^ $(SDLLIBFLAGS) $(LDFLAGS) -o $@

$(OBJECTS_CPP): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(SDLCFLAGS) $< -o $@

$(OBJECTS_C): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	
clean :
	-rm -f *.o *.core *.dylib $(EXECNAME) 
	if [ -f "./logs/log.txt" ]; then rm ./logs/log.txt; fi 
clean_all :
	-rm -f *.o *.core *.dylib $(EXECNAME) *.d
	if [ -f "./logs/log.txt" ]; then rm ./logs/log.txt; fi 
	if [ -d "./logs" ]; then rmdir ./logs; fi
