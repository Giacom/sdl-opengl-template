#include <SDL.h>
#include <vector>

#include "opengl.h"

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
		"layout (location = 1) in vec2 texCoord;\n"
		"out vec2 TexCoord;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
		"TexCoord = texCoord;\n"
		"}\0";

	const GLchar* fragmentShaderSource = "#version 330 core\n"
		"in vec2 TexCoord;\n"
		"out vec4 color;\n"
		"uniform sampler2D ourTexture;\n"
		"void main()\n"
		"{\n"
		"color = texture(ourTexture, TexCoord);\n"
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
		// Positions          // Colors           	  // Texture Coords
		0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Top Right
		0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Bottom Right
		-0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
    };

	GLuint indices[] = {
        0, 1, 3,
        1, 2, 3,
	};

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
	{
		// Now the VBO..
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Finally the EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Vertex Attributes

		glVertexAttribPointer(0, // Layout
							3, // Size
							GL_FLOAT, // Type
							GL_FALSE, // Normalised
							8 * sizeof(GLfloat), // Stride
							(void*) 0); // Array Buffer Offset
		glEnableVertexAttribArray(0);

		// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		// glEnableVertexAttribArray(1);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
	}

	glBindVertexArray(0);

	// Texture loading

	SDL_Surface* image = SDL_LoadBMP("texture.bmp");
	if (!image) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load texture.bmp! %s", SDL_GetError());
		return 1;
	}

	GLuint texture = 0;
	glGenTextures(1, &texture);
	{
		glBindTexture(GL_TEXTURE_2D, texture); // All texture functions will now operate on this texture

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // X wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Y wrapping

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Far away
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Close up

		// Use GL_BGR because bitmaps are silly
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_BGR, GL_UNSIGNED_BYTE, image->pixels);
		SDL_FreeSurface(image);
	}
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind

	for(GLenum err; (err = glGetError()) != GL_NO_ERROR;) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "OpenGL Init: 0x%x", err);
	}


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
		
		// Bind texture
		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		SDL_GL_SwapWindow(window);
	}
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
	return 0;
}