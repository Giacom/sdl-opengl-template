LDLIBS = -lSDL2
CXXFLAGS = -Wall -Wextra -Werror -g

all: main.cpp
	${CXX} ${LDLIBS} ${CXXFLAGS} -o main main.cpp opengl.cpp

clean: 
	$(RM) main