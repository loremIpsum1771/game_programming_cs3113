#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <ctime>
#include <vector>
#include "Matrix.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path){
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}
									// x     y
std::vector<float> translations = {  1.0, -1.0,
									-1.0, -1.0,
									-1.0, 1.0, };





int main(int argc, char *argv[])
{
	float lastFrameTicks = 0.0f;

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 640, 360);
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	GLuint texture1 = LoadTexture(RESOURCE_FOLDER"cardBack_blue2.png");
	GLuint texture2 = LoadTexture(RESOURCE_FOLDER"cardBack_blue2.png");
	GLuint texture3 = LoadTexture(RESOURCE_FOLDER"cardBack_blue2.png");
	GLuint texture4 = LoadTexture(RESOURCE_FOLDER"cardBack_blue2.png");
	Matrix projectionMatrix;
	Matrix modelMatrix;
	//modelMatrix.identity();
	Matrix viewMatrix;
	//viewMatrix.identity();
	//modelMatrix.Translate(1.0, 1.0, 0.0);
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}

			float ticks = (float)SDL_GetTicks() / 1000.0f;
			float elapsed = ticks - lastFrameTicks;
			lastFrameTicks = ticks;

			//translations[4] += elapsed;

			glClear(GL_COLOR_BUFFER_BIT);
			program.setModelMatrix(modelMatrix);
			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);

			modelMatrix.identity();
			

			glBindTexture(GL_TEXTURE_2D, texture1);
			float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
			glEnableVertexAttribArray(program.positionAttribute);
			float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
			glEnableVertexAttribArray(program.texCoordAttribute);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(program.positionAttribute);
			glDisableVertexAttribArray(program.texCoordAttribute);
			modelMatrix.Translate(1.0, 1.0, 0.0);

			
			program.setModelMatrix(modelMatrix);
			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);

			modelMatrix.identity();
			

			glBindTexture(GL_TEXTURE_2D, texture2);
			float vertices2[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
			glEnableVertexAttribArray(program.positionAttribute);
			float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
			glEnableVertexAttribArray(program.texCoordAttribute);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(program.positionAttribute);
			glDisableVertexAttribArray(program.texCoordAttribute);
			modelMatrix.Translate(translations[0], translations[1], 0.0);
			
			program.setModelMatrix(modelMatrix);
			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);
			modelMatrix.identity();
			
			
			

			glBindTexture(GL_TEXTURE_2D, texture3);
			float vertices3[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
			glEnableVertexAttribArray(program.positionAttribute);
			float texCoords3[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
			glEnableVertexAttribArray(program.texCoordAttribute);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(program.positionAttribute);
			glDisableVertexAttribArray(program.texCoordAttribute);
			modelMatrix.Translate(translations[2], translations[3], 0.0); 
			
			program.setModelMatrix(modelMatrix);
			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);
			modelMatrix.identity();
			

			glBindTexture(GL_TEXTURE_2D, texture4);
			float vertices4[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices4);
			glEnableVertexAttribArray(program.positionAttribute);
			float texCoords4[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords4);
			glEnableVertexAttribArray(program.texCoordAttribute);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(program.positionAttribute);
			glDisableVertexAttribArray(program.texCoordAttribute);
			modelMatrix.Translate(translations[4], translations[5], 0.0);


			if (translations[1] != 1.9) 
				translations[1] += (0.2 *elapsed) ;
			else{ translations[1] = -1.0; }
			if (translations[2] != 2.0) translations[2] += (0.2 *elapsed) ;
			else{ translations[2] = -1.0; }
			if (translations[3] != 1.8) translations[3] += (0.2 *elapsed) ;
			else{ translations[1] = -1.0; }
			if (translations[4] != 2.0) translations[4] += (0.2 *elapsed) ;
			else{ translations[4] = -1.0; }
			if (translations[5] != 1.7) translations[5] -= (0.3 *elapsed) ;
			else{ translations[1] = 1.0; }
			

			
/*
			translations = { 1.0, -1.0,
				-1.0, -1.0,
				-1.0, 1.0, };*/

			SDL_GL_SwapWindow(displayWindow);

		}
		
	}

	SDL_Quit();
	return 0;
}
