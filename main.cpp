#include "opengl.h"

#include <SDL.h>

int Abs(int num) {
	return num > 0 ? num : -num;
}

int PingPong(int num, int length) {
	return length - Abs((num % (length * 2)) - length);
}

int main() {

	const int WindowWidth = 960;
	const int WindowHeight = 720;

	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("SDL OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
	glViewport(0, 0, WindowWidth, WindowHeight);
	glClearColor(0.39f, 0.58f, 0.92f, 1.0f);

	// Triangle data
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	static const GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f,
	};

	GLuint vertexBufferObjects; // store the buffer in here
	glGenBuffers(1, &vertexBufferObjects); // generated the buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects); // bind our buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Shaders
	const GLchar* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
		"}\0";

	const GLchar* fragmentShaderSource = "#version 330 core\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";

	GLint success;
	GLchar logBuffer[512];

	// Vertex
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, logBuffer);
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Vertex Compile Error: %s", logBuffer);
	}

	// Fragment
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, logBuffer);
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fragment Compile Error: %s", logBuffer);
	}

	// Program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, logBuffer);
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Shader Linking Error: %s", logBuffer);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	bool running = true;
	SDL_Event event;
	while (running) {

		// Input
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}

			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
				int x = event.window.data1;
				int y = event.window.data2;
				glViewport(0, 0, x, y);
			}
		}


		r = (r + 2) % (255 * 2);
		g = (g + 4) % (255 * 2);
		b = (b + 8) % (255 * 2);

		//Rendering
		glClearColor((float)PingPong(r, 255) / 255, (float)PingPong(g, 255) / 255, (float)PingPong(b, 255) / 255, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw Triangle
		glUseProgram(shaderProgram);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
		glVertexAttribPointer(0 /* layout */, /* size */ 3, /* type */ GL_FLOAT, /* normalised? */ GL_FALSE, /* stride */ 0, /* array buffer offset */ (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0, draw 3 vertices

		glDisableVertexAttribArray(0); 

		SDL_GL_SwapWindow(window);
	}
	return 0;
}