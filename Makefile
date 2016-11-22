CXXFLAGS = -Wall -Wextra -Werror -g
SDL_FLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs)

all: main.cpp
	${CXX}  -o main main.cpp opengl.cpp${LDLIBS} ${CXXFLAGS} ${SDL_FLAGS} ${SDL_LIBS}
clean: 
	$(RM) main