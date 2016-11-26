#include "opengl.h"

#include <SDL.h>

int Abs(int num) {
	return num > 0 ? num : -num;
}

int PingPong(int num, int length) {
	return length - Abs((num % (length * 2)) - length);
}

int main() {

	const int WindowWidth = 720;
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


	int r = 0;
	int g = 0;
	int b = 0;
	glViewport(0, 0, WindowWidth, WindowHeight);
	glClearColor(0.39f, 0.58f, 0.92f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLfloat vertices[] = {
         0.5f,  0.5f, 0.0f,  // Top Right
         0.5f, -0.5f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,  // Bottom Left
        -0.5f,  0.5f, 0.0f   // Top Left 
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3,  // First Triangle
        1, 2, 3   // Second Triangle
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

	// Now the VBO..
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Finally the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Now give the attributes to the VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO


	// Vertex Attributes

	glVertexAttribPointer(0, // Layout
	                      3, // Size
						  GL_FLOAT, // Type
						  GL_FALSE, // Normalised
						  3 * sizeof(GLfloat), // Stride
						  (void*) 0); // Array Buffer Offset
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // Unbind the VOA

	bool running = true;
	SDL_Event event;

	float current = SDL_GetTicks();
	float last = current;
	float interval = 1000 / 60;

	while (running) {

		// Limit frame rate
		current = SDL_GetTicks();

		float dt = current - last;

		if (dt < interval) {
			SDL_Delay(interval - dt);
			continue;
		}

		last = current;

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

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		SDL_GL_SwapWindow(window);
	}
	return 0;
}