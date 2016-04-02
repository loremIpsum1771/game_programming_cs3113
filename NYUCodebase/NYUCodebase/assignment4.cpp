
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
#include <set>
#include <cstdlib>
#include <ctime>
#include <typeinfo>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define  LEVEL_HEIGHT 13
#define LEVEL_WIDTH 22
#define TILE_SIZE 0.215f
#define SPRITE_COUNT_X 8
#define SPRITE_COUNT_Y 16

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6


SDL_Window* displayWindow;

float randomGen(float low, int high) {
	srand((int)time(0));
	float r = (rand() % high) + low;
	return r;
}

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


	void DrawUniformSpr(ShaderProgram *program, int index, int spriteCountX, int spriteCountY) {
		float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
		float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
		float spriteWidth = 1.0 / (float)spriteCountX;
		float spriteHeight = 1.0 / (float)spriteCountY;
		GLfloat texCoords[] = {
			u, v + spriteHeight,
			u + spriteWidth, v,
			u, v,
			u + spriteWidth, v,
			u, v + spriteHeight,
			u + spriteWidth, v + spriteHeight
		};
		float vertices[] = { -0.25f, -0.25f, 0.25f, 0.25f, -0.25f, 0.25f, 0.25f, 0.25f, -0.25f,
			-0.25f, 0.25f, -0.25f };
		// our regular sprite drawing
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

float lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t*v1;
}

enum EntityType {ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_COIN};

class Entity {

public:
	Entity(float xDir, float yDir, float width, float height, float speed, float x, float y, Matrix &modelMatrix,Matrix &viewMatrix ,ShaderProgram* program, SheetSprite newSprite) :
		xDirection(xDir), yDirection(yDir), width(width), height(height), speed(speed), x(x), y(y), modelM(modelMatrix), program(program), mySprite(newSprite) {
		vertices = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		friction_x = -0.25;
		friction_y = -0.25;
		velocity_x = 2.25;
		velocity_y = 0.0;
		acceleration_x = 0.0;
		acceleration_y = 0.0;
		collidedTop = false;
		collidedBottom = false;
		collidedLeft = false;
		collidedRight = false;
		grounded = false;
		gravityOn = true;
		gravity = -0.07;
	}
	Matrix modelM;
	Matrix viewM;
	bool grounded;
	float acceleration_x;
	float acceleration_y;
	float velocity_x;
	float velocity_y;
	float friction_x;
	float friction_y;
	bool gravityOn;
	float gravity;
	bool isStatic;
	EntityType entityType;
	float collidedTop;
	float collidedBottom;
	float collidedLeft;
	float collidedRight;

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

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	virtual void Update(float elapsed) {
		/*if (collidedBottom) {
			gravityOn = false;

		}*/
		float minX = -3.5f;
		float maxX = 3.5f;
		if (keys[SDL_SCANCODE_LEFT] /*&& !collidedLeft&& (x - (width / 2.0) >= minX)*/) {
			//movingLeft = true;
			//movingRight = false;
			//velocity_x = lerp(velocity_x, 0.0f, elapsed * friction_x);
			//velocity_x += acceleration_x * elapsed;
			x += -1 * velocity_x * elapsed;
		}
		if (keys[SDL_SCANCODE_RIGHT]/*&& (x + (width / 2.0) <= maxX)*/) {
			/*movingLeft = false;
			movingRight = true;*/
			//velocity_x = lerp(velocity_x, 0.0f, elapsed * friction_x);
			//velocity_x += acceleration_x * elapsed;
			x += 1 * velocity_x * elapsed;
		}
		//if (gravityOn) {
			velocity_y += gravity;
		//}

		y += 1 * velocity_y * elapsed;
		//scroll screen
		//viewM.Translate(x, y, 0.0);
		
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

	void drawUniformSprite(int index, int sprcount_x, int sprcount_y ) {
		program->setModelMatrix(modelM);
		modelM.identity();
		modelM.Translate(x, y, 0.0);
		modelM.Scale(width, height, 0.0);
		mySprite.DrawUniformSpr(program, index, sprcount_x, sprcount_y);
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

//class Spaceship : public Entity {
//public:
//	Spaceship(float xDirect, float yDirect, float width, float height, float speed,
//		float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite, bool movingLeft, bool movingRight) :
//		Entity(xDirect, yDirect, width, height, speed, xPosition, yPosition, modelMatrix, program, newSprite), movingLeft(movingLeft), movingRight(movingRight) {}
//
//	virtual void update(float elapsed) {
//
//	}
//
//	bool movingLeft;
//	bool movingRight;
//};

//class Enemy : public Entity {
//public:
//	Enemy(float xDirect, float yDirect,  float width, float height, float speed,
//		float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite);
//	bool isSolid;
//	bool floatingLeft;
//	bool floatingRight;
//	int leftmost;
//	int rightmost;
//	int vectPosX;
//	int vectPosY;
//
//};

//class Bullet : public Entity {
//
//public:
//
//	Bullet(float xDirect, float yDirect, float width, float height, float speed,
//		float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite, float angle, float timeAlive)
//		: Entity(xDirect, yDirect, width, height, speed, xPosition, yPosition, modelMatrix, program, newSprite), angle(angle), timeAlive(timeAlive) {
//	}
//
//	float angle;
//	float timeAlive;
//
//};
//
//Enemy::Enemy(float xDirect, float yDirect,  float width, float height, float speed,
//	float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite) :
//	Entity(xDirect, yDirect, width, height, speed, xPosition, yPosition, modelMatrix, program, newSprite) {
//	leftmost = 0;
//	rightmost = 10;
//	vectPosX = 0;
//	vectPosY = 0;
//	isSolid = true;
//	floatingLeft = false;
//	floatingRight = true;
//}
//std::vector<Enemy*> entities;


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

std::set<int> solidTiles = { 6,9,15,20,22 };




struct Vec2 {
	int x;
	int y;
	Vec2() {
		//x = 0;
		//y = 0;
	}
	bool isPositive() {
		if (x >= 0 && y >= 0)return true;
		else return false;
	}
	bool withinMap() {
		if (x <= LEVEL_WIDTH && y <= LEVEL_HEIGHT)return true;
		else return false;
	}
};

bool tileSolid(int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], Vec2 gCoordinates) {
	return solidTiles.find(levelData[gCoordinates.y][gCoordinates.x]) != solidTiles.end();
}



Vec2 worldToTileCoordinates(float worldX, float worldY) {
	Vec2 gridCoordinates;
	gridCoordinates.x = (int)(worldX / TILE_SIZE);
	gridCoordinates.y = (int)(-worldY / TILE_SIZE);
	//std::cout << "grid coordinates.x " << gridCoordinates.x << "grid coordinates.y " << gridCoordinates.y << std::endl;
	return gridCoordinates;
}

bool checkCollisionBottom(Entity* anEntity, Vec2& gCoordinates, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
	gCoordinates = worldToTileCoordinates(anEntity->x , anEntity->y - anEntity->height/2.0f);

	const bool tileIsSolid = tileSolid(levelData, gCoordinates);

	if (tileIsSolid && gCoordinates.isPositive() && gCoordinates.withinMap()) {
		//Entity collides with tile from above	
		if((-TILE_SIZE *gCoordinates.y) >= (anEntity->y - (anEntity->height) / 2)){
			anEntity->collidedBottom = true;
			//std::cout << "collided" << std::endl;
			return true;
		}
		else return false;
	}
	else return false;


}

bool checkCollisionTop(Entity* anEntity, Vec2& gCoordinates, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
	gCoordinates = worldToTileCoordinates(anEntity->x, anEntity->y - anEntity->height / 2.0f);
	const bool tileIsSolid = tileSolid(levelData, gCoordinates);
	if (tileIsSolid && gCoordinates.isPositive() && gCoordinates.withinMap()) {
		//Entity collides with tile from below
		if ((anEntity->y + (anEntity->height) / 2) <= ((-TILE_SIZE *gCoordinates.y)- TILE_SIZE)) {
		anEntity->collidedTop = true;
		return true;
		}
		else return false;
	}
	else return false;
}

bool checkCollisionLeft(Entity* anEntity, Vec2& gCoordinates, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
	gCoordinates = worldToTileCoordinates(anEntity->x, anEntity->y - anEntity->height / 2.0f);
	const bool tileIsSolid = tileSolid(levelData, gCoordinates);
	if (tileIsSolid && gCoordinates.isPositive() && gCoordinates.withinMap()) {
		//Left side of Entity collides with tile
		if ((anEntity->x + anEntity->width / 2) >= ((TILE_SIZE * gCoordinates.x) - TILE_SIZE)) {
			anEntity->collidedLeft = true;
			return true;
		}
		else return false;
	}
	else return false;
}

bool checkCollisionRight(Entity* anEntity, Vec2& gCoordinates, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
	gCoordinates = worldToTileCoordinates(anEntity->x, anEntity->y - anEntity->height / 2.0f);
	const bool tileIsSolid = tileSolid(levelData, gCoordinates);
	if (tileIsSolid && gCoordinates.isPositive() && gCoordinates.withinMap()) {
		//Right side of entity collides with tile
		if ((anEntity->x - anEntity->width / 2) <= ((TILE_SIZE * gCoordinates.x) + TILE_SIZE)) {
			anEntity->collidedRight = true;
			return true;
		}
		else return false;
	}
	else return false;
}




void resetGame(ShaderProgram* program, Matrix &modelM) {
	
}




void UpdateGameLevel(ShaderProgram* program, float &elapsed, Matrix &modelM, std::string &text, Entity* player, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], Matrix &viewM) {
	player->Update(elapsed);

	Vec2 tileCoords;
	//for (int y = 0; y < LEVEL_HEIGHT; y++) {
		//for (int x = 0; x < LEVEL_WIDTH; x++) {

	if (checkCollisionBottom(player, tileCoords, levelData)) {
		//player->gravityOn = false;
		//float tilebyGcoord = -TILE_SIZE *tileCoords.y;
		//float entityBottom = player->y - (player->height) / 2;

		player->y += fabs((-TILE_SIZE *tileCoords.y) - (player->y - ((player->height)) / 2.0));
		//player->y = 0.5;
		player->velocity_y = 0.0;
		//player->collidedBottom = false;
		//player->grounded = true;

	}

	else if (checkCollisionTop(player,tileCoords,levelData)) {
		player->y += (player->y + (player->height) / 2) - ((-TILE_SIZE *tileCoords.y) - TILE_SIZE);
		player->velocity_y = 0.0;
	}

	else if (checkCollisionLeft(player, tileCoords, levelData)) {
		player->x += ((TILE_SIZE * tileCoords.x) + TILE_SIZE) - (player->x - player->width / 2);
		player->velocity_x = 0.0;
	}
	else if (checkCollisionRight(player, tileCoords, levelData)) {
		player->x += (player->x + player->width / 2) - ((TILE_SIZE * tileCoords.x) - TILE_SIZE);
		player->velocity_x = 0.0;
	}

			
		/*player->collidedLeft = false;
		player->collidedRight = false;
		player->collidedTop = false;*/
		//}
	//}

	
	

}

void RenderGameLevel(ShaderProgram* program, Matrix &modelM, std::string &text, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], std::vector<GLuint> sSheetIds,Entity* player, Matrix& viewM) {	
	program->setModelMatrix(modelM);
	program->setViewMatrix(viewM);
	viewM.identity();
	viewM.Translate(-player->x, -player->y, 0.0);

	//program->setModelMatrix(viewM);

	//modelM.identity();
	//modelM.Translate(-3.5, 2.0, 0.0);

	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {		
			if (levelData[y][x] != -1) {
				float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
				float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;
				float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
				float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;

				u *= spriteWidth * 2;
				//v *= spriteHeight * 2;
				vertexData.insert(vertexData.begin(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					TILE_SIZE * x, -TILE_SIZE * y,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
				});

				texCoordData.insert(texCoordData.begin(), {
					u, v,
					u, v + (spriteHeight),
					u + spriteWidth, v + (spriteHeight),
					u, v,
					u + spriteWidth, v + (spriteHeight),
					u + spriteWidth, v
				});
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, sSheetIds[0]);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glDrawArrays(GL_TRIANGLES, 0, vertexData.size() / 2);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	
}

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_PLAYER_WINS };
int state;





void Update(ShaderProgram* program, float &elapsed, Matrix &modelM, std::string &text, Entity* player, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], Matrix &viewM) {
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
		UpdateGameLevel(program, elapsed, modelM, text, player,levelData, viewM);

		
	case STATE_PLAYER_WINS:
		modelM.identity();
		if (keys[SDL_SCANCODE_R]) {
			state = STATE_GAME_LEVEL;
			resetGame(program, modelM);
		}
		break;
	}


}




void Render(ShaderProgram* program, Matrix &modelM, std::string &text, int fontTexture, float size, float spacing, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], std::vector<GLuint> sSheetIds, Entity* player, Matrix& viewM) {

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
		//modelM.Translate(-3.5, 2.0, 0.0);
		player->drawUniformSprite(0, 12, 8);
		RenderGameLevel(program, modelM, text, levelData,sSheetIds,player,viewM);
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



void pgMap(int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
	Vec2 coordsDown;
	Vec2 coordsLeft;
	Vec2 coordsRight;
	Vec2 coordsUp;
	Vec2 currentCoords;
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			std::vector<int> tiles(solidTiles.size());
			std::set<int>::iterator it;
			int idx;
			for (it = solidTiles.begin(), idx = 0; it != solidTiles.end(); ++it, ++idx) {
				int settile = *(it);
				tiles[idx] = settile;

			}
			std::set<int>::iterator iter = solidTiles.find(tiles[randomGen(0, solidTiles.size())]);
			coordsUp.x = x;
			coordsUp.y = y - 2;
			coordsLeft.x = x - 2;
			coordsLeft.y = y;
			coordsRight.x = x + 2;
			coordsRight.y = y;
			coordsDown.x = x;
			coordsDown.y = y + 1;
			if (!tileSolid(levelData, coordsDown) && !tileSolid(levelData, coordsLeft) && !tileSolid(levelData, coordsRight) && !tileSolid(levelData, coordsUp)) {
					if ( iter != solidTiles.end())
					{
						int setint = *iter;
						levelData[y][x] = setint;
						//std::cout << "set int " << setint << std::endl;
					}
				else {
					levelData[y][x] = -1;
				}
			}
			else {
				levelData[y][x] = -1;
			}
		
	}
	}
	//check to see if there are paths through which the player can travel
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			currentCoords.x = x+1;
			currentCoords.y = y-1;
			coordsUp.x = x;
			coordsUp.y = y - 2;
			coordsLeft.x = x - 2;
			coordsLeft.y = y;
			coordsRight.x = x + 2;
			coordsRight.y = y;
			coordsDown.x = x;
			coordsDown.y = y + 1;
			if (!tileSolid(levelData, coordsDown) 
				&& !tileSolid(levelData, coordsLeft)
				&& !tileSolid(levelData, coordsRight) 
				&& !tileSolid(levelData, coordsUp) 
				&& tileSolid(levelData, currentCoords)) {
				levelData[currentCoords.y][currentCoords.x] = -1;
			}
		}
	}
	//Left,right, and top collisions are only detected between these blocks and the player for some reason
	//If these lines are commented out, the collision detection will cause the player's y position to be increased in the positiv direction
	//When the collision is detected, the player can then only jump
	levelData[0][0] = -1;
	levelData[0][1] = -1;
	levelData[0][2] = -1;
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

	GLuint spritesheet = LoadTexture("arne_sprites.png");

	GLuint fontTexture = LoadTexture("font1.png");

	GLuint player_S_Sheet = LoadTexture("characters_1.png");

	SheetSprite  spr_player(player_S_Sheet, 0.0f , 0.0f, 0.0f, 0.0f, 0.22, program);

	Entity* player = new Entity(0.0, 0.0, 0.4, 0.4, 2.0, randomGen(0.3,1.0), 0.7, modelMatrix, viewMatrix,program, spr_player);

	//viewMatrix.Translate(-1.2, 1.2, 0.0);

	std::vector<GLuint> textureVect = { spritesheet };

	std::set<int>::iterator it;
	//for (it = solidTiles.begin(); it != solidTiles.end(); ++it) {
		//std::cout << 'type' << typeid(*it).name() << std::endl;
		
		//*(solidTiles.find(randomGen(0, solidTiles.size())));
	//}

	int idx;
	for (it = solidTiles.begin(), idx = 0;  it != solidTiles.end(); ++it,++idx) {
		
		std:: cout << " current iteration " << idx << std::endl;
	}
	std::cout << "solidtiles size: " << solidTiles.size();
	
	int levelData[LEVEL_HEIGHT][LEVEL_WIDTH] ;

	//procedurally generate map
	pgMap(levelData);


	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);

	SDL_Event event;
	bool done = false;

	while (!done) {
		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			if (keys[SDL_SCANCODE_SPACE] && state == STATE_GAME_LEVEL ) {
				if (player->collidedBottom) {
					player->velocity_y = 1.55;
					player->y += 1 * player->velocity_y * elapsed;
					//player->velocity_y += player->acceleration_y * FIXED_TIMESTEP;
					//player->y += player->velocity_y * FIXED_TIMESTEP;
					player->collidedBottom = false;
					//player->grounded = false;
					player->gravityOn = true;
				}
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);


		float minX = -3.5f;
		float maxX = 3.5f;
		/*if (keys[SDL_SCANCODE_LEFT] ) {
			
		}
		if (keys[SDL_SCANCODE_RIGHT] ) {
			
		}*/
		

		float fixedElapsed = elapsed;
		if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
			fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
		}
		//while (fixedElapsed >= FIXED_TIMESTEP) {
			//fixedElapsed -= FIXED_TIMESTEP;
			Update(program, fixedElapsed, modelMatrix, text, player,levelData,viewMatrix);
			Render(program, modelMatrix, text, fontTexture, 0.15f, 0.009f, levelData, textureVect, player,viewMatrix);
		//}
		





		SDL_GL_SwapWindow(displayWindow);



	}

	SDL_Quit();
	return 0;
}
