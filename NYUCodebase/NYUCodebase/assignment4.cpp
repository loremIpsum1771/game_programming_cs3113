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
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define  LEVEL_HEIGHT 16
#define LEVEL_WIDTH 22
#define TILE_SIZE 0.35f
#define SPRITE_COUNT_X 3
#define SPRITE_COUNT_Y 8


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
	}

	void Draw() {
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


enum EntityType {ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_COIN};

class Entity {

public:
	Entity(float xDir, float yDir, float width, float height, float speed, float x, float y, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite) :
		xDirection(xDir), yDirection(yDir), width(width), height(height), speed(speed), x(x), y(y), modelM(modelMatrix), program(program), mySprite(newSprite) {
		vertices = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
	}
	Matrix modelM;

	float acceleration_x;
	float acceleration_y;
	float velocity_x;
	float velocity_y;

	bool isStatic;
	EntityType entityType;
	float collidedTop = false;
	float collidedBottom = false;
	float collidedLeft = false;
	float collidedRight = false;

	float xDirection, yDirection, speed;
	// Textures
	int textureID;
	// Scaling
	float width, height;
	// Translation
	float x, y;
	// Vertices Vector
	std::vector<float> vertices;
	
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

	bool collidesWith(Entity *entity) {

	}



	void drawSprite() {
		program->setModelMatrix(modelM);
		//program->setProjectionMatrix(projectionMatrix);
		//program->setViewMatrix(viewMatrix);
		modelM.identity();
		modelM.Translate(x, y, 0.0);
		modelM.Scale(width, height, 0.0);
		mySprite.Draw();

	}
};

class Room  {
public:
	//grid coordinates for each corner of the room
	int x1, x2, y1, y2;

	//width and height of the room in terms of grid
	int width, height;

	//center point of the room
	int center;

	//constructor for creating new rooms
	Room(int x, int y, int wid, int hght) :width(wid), height(hght) {

	}

};

class Spaceship : public Entity {
public:


	Spaceship(float xDirect, float yDirect, float width, float height, float speed,
		float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite, bool movingLeft, bool movingRight) :
		Entity(xDirect, yDirect, width, height, speed, xPosition, yPosition, modelMatrix, program, newSprite), movingLeft(movingLeft), movingRight(movingRight) {}

	virtual void update(float elapsed) {

	}

	bool movingLeft;
	bool movingRight;
};

class Enemy : public Entity {
public:
	Enemy(float xDirect, float yDirect,  float width, float height, float speed,
		float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite);
	bool isSolid;
	bool floatingLeft;
	bool floatingRight;
	int leftmost;
	int rightmost;
	int vectPosX;
	int vectPosY;

};

class Bullet : public Entity {

public:

	Bullet(float xDirect, float yDirect, float width, float height, float speed,
		float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite, float angle, float timeAlive)
		: Entity(xDirect, yDirect, width, height, speed, xPosition, yPosition, modelMatrix, program, newSprite), angle(angle), timeAlive(timeAlive) {
	}

	float angle;
	float timeAlive;

};

Enemy::Enemy(float xDirect, float yDirect,  float width, float height, float speed,
	float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite) :
	Entity(xDirect, yDirect, width, height, speed, xPosition, yPosition, modelMatrix, program, newSprite) {
	leftmost = 0;
	rightmost = 10;
	vectPosX = 0;
	vectPosY = 0;
	isSolid = true;
	floatingLeft = false;
	floatingRight = true;
}
std::vector<Enemy*> entities;


void drawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing, Matrix &modelM) {
	program->setModelMatrix(modelM);
	modelM.identity();
	modelM.Translate(-2.9, 0.0, 0.0);
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









bool checkCollision(Enemy* anEnemy, Bullet* aBullet) {
	float enemyTop = anEnemy->y + (anEnemy->height / 2.0f);
	float enemyBottom = anEnemy->y - (anEnemy->height / 2.0f);
	float enemyRight = anEnemy->x + (anEnemy->width / 2.0f);
	float enemyLeft = anEnemy->x - (anEnemy->width / 2.0f);

	float bulletTop = aBullet->y + (aBullet->height / 2.0f);
	float bulletBottom = aBullet->y - (aBullet->height / 2.0f);
	float bulletLeft = aBullet->x - (aBullet->width / 2.0f);
	float bulletRight = aBullet->x + (aBullet->width / 2.0f);

	//This is supposed to check that if the objects are not intersecting, return false
	if (
		(bulletBottom > enemyTop) ||
		(bulletTop < enemyBottom) ||
		(bulletLeft > enemyRight) ||
		(bulletRight < enemyLeft)
		) {
		return false;
	}
	else {
		return true;
	}
}
void resetGame(ShaderProgram* program, Matrix &modelM) {
	
}


void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)(worldX / TILE_SIZE);
	*gridY = (int)(-worldY / TILE_SIZE);
}

void UpdateGameLevel(ShaderProgram* program, float &elapsed,  Matrix &modelM, std::string &text) {

	////Check if each of the bullets has hit any of the enemies
	//for (int i = 0; i < bullets.size(); i++) {
	//	for (int j = 0; j < entities.size(); j++) {
	//		if (entities[j] != nullptr && bullets[i] != nullptr) {
	//			if (checkCollision(entities[j], bullets[i])) {
	//				std::cout << "bullets y position: " << bullets[i]->y << std::endl;
	//				std::cout << " hit enemy y position: " << entities[j]->y << std::endl;
	//				std::cout << "bullets x position: " << bullets[i]->x << std::endl;
	//				std::cout << " hit enemy x position: " << entities[j]->x << std::endl;
	//				delete bullets[i];
	//				bullets[i] = nullptr;
	//				delete entities[j];
	//				entities[j] = nullptr;

	//			}
	//		}
	//	}
	//}


}

void RenderGameLevel(ShaderProgram* program, Matrix &modelM, std::string &text, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], std::vector<GLuint> sSheetIds) {	
	program->setModelMatrix(modelM);
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {		
			if (levelData[y][x] != 0) {
				float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
				float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;
				float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
				float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;
				float vertices[] = {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					TILE_SIZE * x, -TILE_SIZE * y,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
				};
				GLfloat texCoords[] =  {
					u, v,
					u, v + (spriteHeight),
					u + spriteWidth, v + (spriteHeight),
					u, v,
					u + spriteWidth, v + (spriteHeight),
					u + spriteWidth, v
				};
				glBindTexture(GL_TEXTURE_2D, sSheetIds[0]);
				glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
				glEnableVertexAttribArray(program->positionAttribute);
				glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
				glEnableVertexAttribArray(program->texCoordAttribute);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program->positionAttribute);
				glDisableVertexAttribArray(program->texCoordAttribute);
			}
		}
	}
	
}

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_PLAYER_WINS };
int state;

std::vector<bool> isTileSolid;


void Update(ShaderProgram* program, float &elapsed, Matrix &modelM, std::string &text) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	int numNull = 0;
	switch (state) {
	case STATE_MAIN_MENU:

		//std::cout << "Game Started" << std::endl;
		if (keys[SDL_SCANCODE_S]) {
			state = STATE_GAME_LEVEL;
		}

		//UpdateMainMenu();
		break;

	case STATE_GAME_LEVEL:
		
		
		UpdateGameLevel(program, elapsed, modelM, text);

		
	case STATE_PLAYER_WINS:
		modelM.identity();
		if (keys[SDL_SCANCODE_R]) {
			state = STATE_GAME_LEVEL;
			resetGame(program, modelM);
		}
		break;
	}


}




void Render(ShaderProgram* program, Matrix &modelM, std::string &text, int fontTexture, float size, float spacing, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], std::vector<GLuint> sSheetIds) {

	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) {
	case STATE_MAIN_MENU:
		//RenderMainMenu();
		text = "Single screen scrolling platformer Press 'P' to play";
		//std::cout << "Main Menu" << std::endl;
		//modelM.identity();
		drawText(program, fontTexture, text, size, spacing, modelM);
		break;

	case STATE_GAME_LEVEL:
		modelM.Translate(-3.5, 2.0, 0.0);
		RenderGameLevel(program, modelM, text, levelData,sSheetIds);
		break;

	case STATE_PLAYER_WINS:
		text = "You've won! Press 'R' to continue!";
		modelM.Translate(-2.5, 0.0, 0.0);
		drawText(program, fontTexture, text, size, spacing, modelM);
		break;
	}
}

void buildLevel(int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
	int level1Data[LEVEL_HEIGHT][LEVEL_WIDTH];
	memcpy(levelData, level1Data, LEVEL_HEIGHT*LEVEL_WIDTH);
}


int main(int argc, char *argv[])
{
	float lastFrameTicks = 0.0f;

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 690, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 1280, 690);
	ShaderProgram* program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	Matrix projectionMatrix;
	Matrix modelMatrix;

	Matrix viewMatrix;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program->programID);

	std::string text = "";

	GLuint spritesheet = LoadTexture("platformertiles.png");

	GLuint fontTexture = LoadTexture("font1.png");

	std::vector<GLuint> textureVect = { spritesheet };

	
	int levelData[LEVEL_HEIGHT][LEVEL_WIDTH] = {
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4 },
		{ 4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0 },
		{ 0,5,6,6,6,6,6,0,0,0,0,0,0,0,0,6,6,6,6,6,5,0 },
		{ 0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0 },
		{ 0,5,0,0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,0,5,0 }
		
		
	};




	



	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);

	SDL_Event event;
	bool done = false;

	while (!done) {
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			if (keys[SDL_SCANCODE_SPACE] && state == STATE_GAME_LEVEL) {

			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;


		glClear(GL_COLOR_BUFFER_BIT);


		float minX = -3.5f;
		float maxX = 3.5f;
		if (keys[SDL_SCANCODE_LEFT] ) {
			
		}
		if (keys[SDL_SCANCODE_RIGHT] ) {
			
		}


		Update(program, elapsed,  modelMatrix, text);
		Render(program, modelMatrix, text, fontTexture, 0.15f, 0.009f,levelData,textureVect);





		SDL_GL_SwapWindow(displayWindow);



	}

	SDL_Quit();
	return 0;
}
