#include "opengl.h"

#include <SDL.h>

int Abs(int num) {
	return num > 0 ? num : -num;
}

int PingPong(int num, int length) {
	return length - Abs((num % (length * 2)) - length);
}

int main() {

	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("SDL OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!window) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: %s\n", SDL_GetError());
		return 1;
	}

	//// OpenGL Setup

	// Options
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Connect window
	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!context) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: %s\n", SDL_GetError());
		return 1;
	}

	OpenGL_Init();

	{
		int value = 0;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
		SDL_Log("SDL_GL_CONTEXT_MAJOR_VERSION: %d\n", value);

		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
		SDL_Log("SDL_GL_CONTEXT_MINOR_VERSION: %d\n", value);
	}


	int r, g, b = 0;
	glClearColor(0.39f, 0.58f, 0.92f, 1.0f);

	bool running = true;
	SDL_Event event;
	while (running) {

		// Input
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}
		}


		r = (r + 2) % (255 * 2);
		g = (g + 4) % (255 * 2);
		b = (b + 8) % (255 * 2);

		//Rendering
		glClearColor((float)PingPong(r, 255) / 255, (float)PingPong(g, 255) / 255, (float)PingPong(b, 255) / 255, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
	}
	return 0;
}