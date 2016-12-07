CXXFLAGS = -Wall -Wextra -Werror -std=c++14
DEBUGFLAGS = -g
RELEASEFLAGS = -O2

SDL_FLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs)

BIN_NAME = main
SOURCES = main.cpp opengl.cpp

all: main.cpp
	${CXX}  -o ${BIN_NAME} ${SOURCES} ${LDLIBS} ${CXXFLAGS} ${DEBUGFLAGS} ${SDL_FLAGS} ${SDL_LIBS}
release:
	${CXX}  -o ${BIN_NAME} ${SOURCES} ${LDLIBS} ${CXXFLAGS} ${RELEASEFLAGS} ${SDL_FLAGS} ${SDL_LIBS}
clean: 
	$(RM) main
run: all
	./${BIN_NAME}
