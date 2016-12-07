#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "opengl.h"

// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 position;\n"
	"layout (location = 1) in vec3 color;\n"
	"layout (location = 2) in vec2 texCoord;\n"
	"out vec3 ourColor;\n"
	"out vec2 TexCoord;\n"
	"void main()\n"
	"{\n"
	"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
	"TexCoord = texCoord;\n"
	"ourColor = color;\n"
	"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
	"in vec3 ourColor;\n"
	"in vec2 TexCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D ourTexture;\n"
	"void main()\n"
	"{\n"
	"color = texture(ourTexture, TexCoord);\n"
	"color.rgb *= ourColor;\n"
	"}\n\0";

int Abs(int num) {
	return num > 0 ? num : -num;
}

int PingPong(int num, int length) {
	return length - Abs((num % (length * 2)) - length);
}

#define SIZE_OF_ARRAY(_array) (sizeof(_array) / sizeof(_array[0]))

std::vector<GLfloat> vertices;

/*
const GLuint indices[] = {
	0, 1, 2,
	2, 3, 0,
};
*/

struct Sprite {
	float x;
	float y;
};

Sprite sprites[255];

float get_rand() {
	return ((float)(std::rand() % 200) * 0.005f) - 0.25f;
}

void draw_sprites(GLuint shaderProgram, GLuint texture, GLuint VAO) {
		// Draw Triangle
		glUseProgram(shaderProgram);
		
		// Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
}

int main(int /*argc*/, char** /*argv[]*/) {

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

	std::srand(std::time(nullptr)); // use current time as seed for random generator

	printf("%f\n", get_rand());

	//const int vertice_batch_size = 8 * 6;
	std::size_t size = SIZE_OF_ARRAY(sprites);
	for (std::size_t i = 0; i < size; i++) {
		float x = get_rand();
		float y = get_rand();
		sprites[i] = (Sprite){ .x = x, .y = y };

		float c = 1.0f - (float)i  / (float)size;

		vertices.insert(vertices.end(),
		{	
			-0.5f + x,  0.5f + y, 0.0f,  1.0, c, c,   0.0f, 0.0f,  // Top Left 
			-0.5f + x, -0.5f + y, 0.0f,  c, 1.0, c,   0.0f, 1.0f, // Bottom Left
			0.5f + x, -0.5f + y, 0.0f,   c, c, 1.0,   1.0f, 1.0f, // Bottom Right
			
			0.5f + x, -0.5f + y, 0.0f,   c, c, 1.0,   1.0f, 1.0f, // Bottom Right
			0.5f + x,  0.5f + y, 0.0f,   c, 1.0, c,   1.0f, 0.0f, // Top Right
			-0.5f + x,  0.5f + y, 0.0f,  1.0, c, c,   0.0f, 0.0f  // Top Left 
		});
	}


	glViewport(0, 0, WindowWidth, WindowHeight);
	glClearColor(0.39f, 0.58f, 0.92f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glEnable(GL_CULL_FACE);


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

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);
	{
		// Now the VBO..
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		{
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);

			// Vertex Attributes

			glVertexAttribPointer(0, // Layout
								3, // Size
								GL_FLOAT, // Type
								GL_FALSE, // Normalised
								8 * sizeof(GLfloat), // Stride
								(void*) 0); // Array Buffer Offset
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
		}
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

		//Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw_sprites(shaderProgram, texture, VAO);

		SDL_GL_SwapWindow(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	return 0;
}