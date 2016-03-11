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

GLuint LoadTexture(const char *image_path) {
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

class SheetSprite {
public:

	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size, ShaderProgram* program)
		:textureID(textureID), u(u), v(v), width(width), height(height), size(size), program(program) {

		/*SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size)
			:textureID(textureID), u(u), v(v), width(width), height(height), size(size)*/


		u = 0;
		v = 0;
		width = 0;
		height = 0;
		size = 0;
	}

	void Draw() {
		//glBindTexture(GL_TEXTURE_2D, textureID);
		glUseProgram(program->programID);
		GLfloat texCoords[] = {
			u, v + height,
			u + width, v,
			u, v,
			u + width, v,
			u, v + height,
			u + width, v + height
		};
		float aspect = width / height;
		float vertices[] = {
			-0.5f * size * aspect, -0.5f * size,
			0.5f * size * aspect, 0.5f * size,
			-0.5f * size * aspect, 0.5f * size,
			0.5f * size * aspect, 0.5f * size,
			-0.5f * size * aspect, -0.5f * size ,
			0.5f * size * aspect, -0.5f * size };
		glBindTexture(GL_TEXTURE_2D, textureID);
		//float vertices2[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);
		//float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	ShaderProgram* program;
};

class Entity {

public:
	Entity(float xDir, float yDir, float rotation, float width, float height, float speed, float x, float y, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite) :
		xDirection(xDir), yDirection(yDir), rotation(rotation), width(width), height(height), speed(speed), x(x), y(y), modelM(modelMatrix), program(program), mySprite(newSprite) {
		vertices = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
	}
	Matrix modelM;
	float xDirection, yDirection, speed;
	// Textures
	int textureID;
	// Scaling
	float width, height;
	// Translation
	float x, y, rotation;
	// Vertices Vector
	std::vector<float> vertices;
	// RGB
	float r, g, b;
	SheetSprite mySprite;
	ShaderProgram* program;

	



	virtual void Update(float elapsed) {
		// move stuff and check for collisions
		//posX += elapsed;
		//posY += elapsed;


	}
	void Render() {
		// for all game elements
		// setup transforms, render sprites
	}

	void drawSprite() {
		modelM.Scale(3.0, 3.0, 0.0);
		mySprite.Draw();

	}

};


void drawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}


// x     y
std::vector<float> translations = { 1.0, -1.0,
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
	ShaderProgram* program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	

	Matrix projectionMatrix;
	Matrix modelMatrix;
	//modelMatrix.identity();
	Matrix viewMatrix;
	//viewMatrix.identity();
	//modelMatrix.Translate(1.0, 1.0, 0.0);
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program->programID);


	GLuint texture1 = LoadTexture(RESOURCE_FOLDER"cardBack_blue2.png");
	GLuint texture2 = LoadTexture(RESOURCE_FOLDER"cardBack_blue2.png");
	GLuint texture3 = LoadTexture(RESOURCE_FOLDER"cardBack_blue2.png");
	GLuint texture4 = LoadTexture(RESOURCE_FOLDER"cardBack_blue2.png");

	
	
	GLuint spriteSheetTexture = LoadTexture("sheet.png");
	SheetSprite  mySprite(spriteSheetTexture, 112.0f / 1024.0f, 791.0f / 1024.0f, 112.0f / 1024.0f, 75.0f / 1024.0f, 0.22, program);

	GLuint enemyBlack = LoadTexture("sheet.png");
	SheetSprite  spr_enemy1(enemyBlack, 423.0f / 1024.0f, 728.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.06, program);

	GLuint enemyBlue = LoadTexture("sheet.png");
	SheetSprite  spr_enemy2(enemyBlue, 425.0f / 1024.0f, 468.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.06, program);

	GLuint enemyGreen = LoadTexture("sheet.png");
	SheetSprite  spr_enemy3(enemyGreen, 425.0f / 1024.0f, 552.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.06, program);

	GLuint enemyRed1 = LoadTexture("sheet.png");
	SheetSprite  spr_enemy4(enemyRed1, 425.0f / 1024.0f, 384.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.06, program);

	GLuint enemyRed2 = LoadTexture("sheet.png");
	SheetSprite  spr_enemy5(enemyRed2, 120.0f / 1024.0f, 520.0f / 1024.0f, 104.0f / 1024.0f, 84.0f / 1024.0f, 0.06, program);

	

	Entity* anEntity = new Entity(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 3.0f, 0.0f, -1.67f, modelMatrix, program, mySprite);






	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		//translations[4] += elapsed;

		glClear(GL_COLOR_BUFFER_BIT);
		//program->setModelMatrix(modelMatrix);
		//program->setProjectionMatrix(projectionMatrix);
		//program->setViewMatrix(viewMatrix);

		//modelMatrix.identity();


		//glBindTexture(GL_TEXTURE_2D, texture1);
		//float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		//glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		//glEnableVertexAttribArray(program->positionAttribute);
		//float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		//glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		//glEnableVertexAttribArray(program->texCoordAttribute);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDisableVertexAttribArray(program->positionAttribute);
		//glDisableVertexAttribArray(program->texCoordAttribute);
		//modelMatrix.Translate(1.0, 1.0, 0.0);


		program->setModelMatrix(modelMatrix);
		program->setProjectionMatrix(projectionMatrix);
		program->setViewMatrix(viewMatrix);

		modelMatrix.identity();
		modelMatrix.Translate(1.0, -1.0, 0.0);
		//modelMatrix.Scale(anEntity->width, anEntity->height, 0.0);
		//modelMatrix.Scale(2.0, 2.0, 0.0);
		anEntity->drawSprite();

		/*if (translations[1] != 1.9)
		translations[1] += (0.2 *elapsed);
		else { translations[1] = -1.0; }
		if (translations[2] != 2.0) translations[2] += (0.2 *elapsed);
		else { translations[2] = -1.0; }
		if (translations[3] != 1.8) translations[3] += (0.2 *elapsed);
		else { translations[1] = -1.0; }
		if (translations[4] != 2.0) translations[4] += (0.2 *elapsed);
		else { translations[4] = -1.0; }
		if (translations[5] != 1.7) translations[5] -= (0.3 *elapsed);
		else { translations[1] = 1.0; }
		*/


		/*
		translations = { 1.0, -1.0,
		-1.0, -1.0,
		-1.0, 1.0, };*/

		SDL_GL_SwapWindow(displayWindow);



	}

	SDL_Quit();
	return 0;
}
