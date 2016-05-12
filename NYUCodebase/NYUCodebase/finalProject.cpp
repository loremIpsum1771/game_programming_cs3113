#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <ctime>
#include <vector>
#include "Matrix.h"
#include <SDL_mixer.h>
#include "ShaderProgram.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <cstdlib>
#include <ctime>
#include <typeinfo>
#include <cmath>


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define  LEVEL_HEIGHT 8
#define LEVEL_WIDTH 120
#define TILE_SIZE 0.315f
#define SPRITE_COUNT_X 24
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
		float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f };
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

int currentSet = 0;

std::set<int> solidTiles = { 4,6,15,25,26 };




struct Vec2 {
	float x;
	float y;
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

struct Color {
	float r;
	float g; 
	float b;
	float a;
	Color() {

	}
};

bool tileSolid(int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], Vec2 gCoordinates) {
	return solidTiles.find(levelData[(int)gCoordinates.y][(int)gCoordinates.x]) != solidTiles.end();
}



Vec2 worldToTileCoordinates(float worldX, float worldY) {
	Vec2 gridCoordinates;
	gridCoordinates.x = (int)(worldX / TILE_SIZE);
	gridCoordinates.y = (int)(-worldY / TILE_SIZE);
	//std::cout << "grid coordinates.x " << gridCoordinates.x << "grid coordinates.y " << gridCoordinates.y << std::endl;
	return gridCoordinates;
}

Vec2 TileToWorldCoordinates(float gridX, float gridY) {
	Vec2 worldCoordinates;
	worldCoordinates.x = (int)(gridX * TILE_SIZE);
	worldCoordinates.y = (int)(gridY * TILE_SIZE);
	//std::cout << "grid coordinates.x " << gridCoordinates.x << "grid coordinates.y " << gridCoordinates.y << std::endl;
	return worldCoordinates;
}

class Particle {
public:
	Vec2 position;
	Vec2 velocity;
	Vec2 velocityDeviation;
	float lifetime;
	Particle(float posX, float posY, float velX, float velY,/* float velDevX, float velDevY, */float lifespan) : lifetime(lifespan) {
		position.x = posX;
		position.y = posY;
		velocity.x = velX;
		velocity.y = velY;
		/*velocityDeviation.x = velDevX;
		velocityDeviation.y = velDevY;*/
	}
};

class ParticleEmitter {
public:
	Matrix modelM;
	Vec2 position;
	float gravity;
	Vec2 velocity;
	Color startColor;
	Color endColor;
	float maxLifetime;
	float width;
	float height;
	std::vector<Particle*> particles;
	std::vector<float> particleVertices;
	std::vector<float> particleColors;
	float numParticles;
	ParticleEmitter(unsigned int particleCount,float entityX, float entityY, Matrix &modelMatrix): 
		numParticles(particleCount),modelM(modelMatrix) {
		maxLifetime = randomGen(5, 15);
		startColor.r = 0.7;
		startColor.g = 0.1;
		startColor.b = 0.15;
		startColor.a = 0.0;

		endColor.r = 0.9;
		endColor.g = 0.1;
		endColor.b = 0.15;
		endColor.a = 1.0;
		position.x = entityX;
		position.y = entityY;
		gravity = -0.07;
		velocity.x = 0.05;
		velocity.y = 0.05;
		width = 0.3;
		height = 0.3;

	}
	//ParticleEmitter() {

	//}
	~ParticleEmitter() {

	}
	void Update(float elapsed) {
		if (numParticles >= 0) {
			Particle* aParticle = new Particle(position.x, position.y, velocity.x, velocity.y, maxLifetime);
			particles.push_back(aParticle);
			--numParticles;


			for (int i = 0; i < particles.size(); i++) {
				float m = (particles[i]->lifetime / maxLifetime);
				particleColors.push_back(lerp(startColor.r, endColor.r, m));
				particleColors.push_back(lerp(startColor.g, endColor.g, m));
				particleColors.push_back(lerp(startColor.b, endColor.b, m));
				particleColors.push_back(lerp(startColor.a, endColor.a, m));
			}

			for (Particle* eachParticle : particles) {
				eachParticle->position.y += gravity* elapsed;
				eachParticle->position.x += velocity.x * elapsed;
				eachParticle->position.y += velocity.y* elapsed;

			}
		}
	}
	void Render(ShaderProgram* program) {
		program->setModelMatrix(modelM);
		//program->setProjectionMatrix(projectionMatrix);
		//program->setViewMatrix(viewM);
		modelM.identity();
		//viewM.identity();
		modelM.Translate(position.x, position.y, 0.0);
		modelM.Scale(width, height, 0.0);
		for (int i = 0; i < particles.size(); i++) {
			particleVertices.push_back(particles[i]->position.x);
			particleVertices.push_back(particles[i]->position.y);
		}
		/*glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, particleVertices.data());
		glEnableVertexAttribArray(program->positionAttribute);
		glVertexAttribPointer(program->colorAttribute, 4, GL_FLOAT, false, 0, particleColors.data());
		glEnableVertexAttribArray(program->colorAttribute);
		
		glDrawArrays(GL_POINTS, 0, particleVertices.size() / 2);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->colorAttribute);*/

		float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f };
		 //our regular sprite drawing
		//glBindTexture(GL_TEXTURE_2D, textureID);
		//float vertices2[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);
		//float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program->colorAttribute, 4, GL_FLOAT, false, 0, particleColors.data());
		glEnableVertexAttribArray(program->colorAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->colorAttribute);





	}	
	
};

float screenShakeValue = 0.7;
float screenShakeSpeed = 1.7;
float screenShakeIntensity = 1.25;

enum EntityType { ENTITY_PLAYER, ENTITY_ZOMBIE, ENTITY_COIN, ENTITY_BULLET };
enum State { PACING, FOLLOWING,DEAD };

class Entity {

public:
	Entity(float xDir, float yDir, float width, float height, float speed, float x, float y, Matrix &modelMatrix, Matrix &viewMatrix, ShaderProgram* program, ShaderProgram* ps_program,SheetSprite newSprite) :
		xDirection(xDir), yDirection(yDir), width(width), height(height), speed(speed), x(x), y(y), modelM(modelMatrix), program(program), mySprite(newSprite),sensorSprite(newSprite),  particle_program(ps_program){
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
		jumping = false;
		canRender = false;
		gridCoords.x = 0;
		gridCoords.y = 0;
		entityType = ENTITY_PLAYER;
		entityState = PACING;
		xDirection = 1;
		paceSpeed = 0.25;
		hitBottom = false;
		movingLeft = false;
		movingRight = false;
		detectionRadius = 1.0;
		particleSystem = new ParticleEmitter(20,x,y,modelMatrix);
		bleeding = false;
		followSpeed = 0.09;
		beingFollowed = false;
		timeAlive = 0;
		score = 0;
		health = 100;
	}
	Matrix modelM;
	Matrix viewM;
	float timeAlive;
	float score;
	float health;
	float detectionRadius;
	bool grounded;
	bool hitBottom;
	bool movingLeft;
	bool movingRight;
	float acceleration_x;
	float acceleration_y;
	float velocity_x;
	float velocity_y;
	float paceSpeed;
	float friction_x;
	float friction_y;
	bool gravityOn;
	float gravity;
	bool isStatic;
	EntityType entityType;
	State entityState;
	float collidedTop;
	float collidedBottom;
	float collidedLeft;
	float collidedRight;
	float jumping;
	float followSpeed;
	bool canRender;
	Vec2 gridCoords;
	bool bleeding;
	bool beingFollowed;

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
	SheetSprite sensorSprite;
	ShaderProgram* program;
	ShaderProgram* particle_program;
	ParticleEmitter* particleSystem;

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	bool checkCollisionBottom(Entity* anEntity, Vec2& gCoordinates, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
		gCoordinates = worldToTileCoordinates(anEntity->x, anEntity->y - anEntity->height / 2.0f);
		const bool tileIsSolid = tileSolid(levelData, gCoordinates);

		if (tileIsSolid && gCoordinates.isPositive() && gCoordinates.withinMap()) {
			//Entity collides with tile from above	
			//if ((-TILE_SIZE *gCoordinates.y) >= (anEntity->y - (anEntity->height) / 2)) {
				anEntity->collidedBottom = true;
				//std::cout << "collided" << std::endl;
				return true;
			//}
			//else return false;
		}
		else return false;
	}

	bool checkCollisionTop(Entity* anEntity, Vec2& gCoordinates, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
		gCoordinates = worldToTileCoordinates(anEntity->x, anEntity->y + anEntity->height / 2.0f);
		const bool tileIsSolid = tileSolid(levelData, gCoordinates);
		if (tileIsSolid && gCoordinates.isPositive() && gCoordinates.withinMap()) {
			//Entity collides with tile from below
			//if ((anEntity->y + (anEntity->height) / 2) <= ((-TILE_SIZE *gCoordinates.y) - TILE_SIZE)) {
				anEntity->collidedTop = true;
				return true;
			//}
			//else return false;
		}
		else return false;
	}

	bool checkCollisionLeft(Entity* anEntity, Vec2& gCoordinates, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
		gCoordinates = worldToTileCoordinates(anEntity->x - anEntity->width / 2, anEntity->y);
		const bool tileIsSolid = tileSolid(levelData, gCoordinates);
		if (tileIsSolid && gCoordinates.isPositive() && gCoordinates.withinMap()) {
			//Left side of Entity collides with tile
			//if ((anEntity->x + anEntity->width / 2) >= ((TILE_SIZE * gCoordinates.x) - TILE_SIZE)) {
				anEntity->collidedLeft = true;
				return true;
			//}
			//else return false;
		}
		else return false;
	}

	bool checkCollisionRight(Entity* anEntity, Vec2& gCoordinates, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
		gCoordinates = worldToTileCoordinates(anEntity->x + anEntity->width / 2, anEntity->y );
		const bool tileIsSolid = tileSolid(levelData, gCoordinates);
		if (tileIsSolid && gCoordinates.isPositive() && gCoordinates.withinMap()) {
			//Right side of entity collides with tile
			//if ((anEntity->x - anEntity->width / 2) <= ((TILE_SIZE * gCoordinates.x) + TILE_SIZE)) {
				anEntity->collidedRight = true;
				return true;
			//}
			//else return false;
		}
		else return false;
	}

	bool checkCollision(Entity* anEntity) {
		float thisTop = this->y + (this->height / 2.0f);
		float thisBottom = this->y - (this->height / 2.0f);
		float thisRight = this->x + (this->width / 2.0f);
		float thisLeft = this->x - (this->width / 2.0f);

		float anEntityTop = anEntity->y + (anEntity->height / 2.0f);
		float anEntityBottom = anEntity->y - (anEntity->height / 2.0f);
		float anEntityLeft = anEntity->x - (anEntity->width / 2.0f);
		float anEntityRight = anEntity->x + (anEntity->width / 2.0f);

		//This is supposed to check that if the objects are not intersecting, return false
		if (
			(anEntityBottom > thisTop) ||
			(anEntityTop < thisBottom) ||
			(anEntityLeft > thisRight) ||
			(anEntityRight < thisLeft)
			) {
			return false;
		}
		else {
			return true;
		}
	}


	bool isNear(Entity* otherEnt) {
		if (abs(gridCoords.x - otherEnt->gridCoords.x) <= 5 && abs(gridCoords.y - otherEnt->gridCoords.y) <= 5) return true;
		else return false;
				
	}
	//called every frame, used to move the zombie to the follow state
	void sensePlayer(Entity* anEntity) {
		if (anEntity->x < x + detectionRadius && anEntity->x > x - detectionRadius) {
			entityState = FOLLOWING;
			//bleeding = true;
			//std::cout << "state: " << entityState << std::endl;
		}
		else {
			//bleeding = false;
			entityState = PACING;
			//std::cout << "state: " << entityState << std::endl;
		}
	}
	//should be called when the zombies are in the pacing state
	void sensePlatform(int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
		Vec2 rightSensor = worldToTileCoordinates(x + 0.3, y - 0.3);
		Vec2 leftSensor = worldToTileCoordinates(x - 0.3, y - 0.3);
		
		bool leftSense = levelData[(int)rightSensor.y][(int)rightSensor.x] != 4;
		bool rightSense = levelData[(int)leftSensor.y][(int)leftSensor.x] !=  4;
		//if (hitBottom) {
			if (rightSense || leftSense) {
				//movingLeft = false;
				//movingRight = true;
				xDirection *= -1;
				//std::cout << "right sensor x: " << rightSensor.x << " right sensor y: " << rightSensor.y << "Level int: " << levelData[(int)rightSensor.y][(int)rightSensor.x] << std::endl;
			}
			
		//}


	}

	void followPlayer(Entity* player, Mix_Chunk *biteSound, float elapsed) {
		x = lerp(x, player->x, followSpeed);
		//timeAlive += elapsed;
		//std::cout << "time Alive " << timeAlive << std::endl;
		if (x <player->x + 0.2 && x > player->x - 0.2) {
			
			
			Mix_PlayChannel(-1, biteSound, 0);
			
			//std::cout << "health: " << health << std::endl;
		}
		
	}

	void manageStates(int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], Entity* anEntity, Mix_Chunk *biteSound,float elapsed) {
		if (entityType == ENTITY_ZOMBIE) {
			sensePlayer(anEntity);
			if (entityState == PACING) {
				/*for (int y = 1; y < LEVEL_HEIGHT; y++) {
					for (int x = 0; x < LEVEL_WIDTH; x++) {
						if (levelData[y][x] == 4) {
							std::cout << "4 found at x: " << x << " y: " << y << std::endl;
						}
					}*/
				bleeding = false;
				//std::cout << "Zombie state: " << entityState << " bleeding = " << bleeding << std::endl;
				sensePlatform(levelData);
			}
			else if (entityState == FOLLOWING) {
				bleeding = true;
				//std::cout << "Zombie state: " << entityState << " bleeding = " << bleeding << std::endl;
				followPlayer(anEntity,biteSound,elapsed);
			}

			
		}
	}

	void manageParticles(float elapsed, std::vector<Entity*> zombies) {
		if (entityType == ENTITY_PLAYER) {
			for (Entity* eachZombie : zombies) {
				if (eachZombie != nullptr) {
					if (checkCollision(eachZombie) && FOLLOWING)
						//bleeding = true;
						//std::cout << "check collision: " << checkCollision(eachZombie) <<  std::endl;
						if (bleeding) {


							particleSystem->Update(elapsed);
							particleSystem->Render(particle_program);
							//std::cout << "number of particles after update: " << particleSystem->particles.size()  << std::endl;
						}
					/*if (particleSystem->particles.size() == 0)
						bleeding = false;*/
				}
			}
		}
	}

	virtual void Update(float elapsed, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
		Vec2 tileCoords;
		if (entityType == ENTITY_PLAYER) {
			
			//move on y-axis

			velocity_y += gravity; //apply downward movement
			y += 1 * velocity_y * elapsed;

			//for (int y = 0; y < LEVEL_HEIGHT; y++) {
			//for (int x = 0; x < LEVEL_WIDTH; x++) {
			if (jumping) {  //apply upward movement
				velocity_y = 1.65;
				y += 1 * velocity_y * elapsed;
				//player->velocity_y += player->acceleration_y * FIXED_TIMESTEP;
				//player->y += player->velocity_y * FIXED_TIMESTEP;
				collidedBottom = false;
				//player->grounded = false;
				gravityOn = true;
				jumping = false;
			}

			if (checkCollisionBottom(this, tileCoords, levelData)) {
				hitBottom = true;
				//player->gravityOn = false;
				//float tilebyGcoord = -TILE_SIZE *tileCoords.y;
				//float entityBottom = player->y - (player->height) / 2;
				y += fabs((-TILE_SIZE *tileCoords.y) - (y - ((height)) / 2.0));

				//player->y = 0.5;
				velocity_y = 0.0;
				//player->collidedBottom = false;
				//player->grounded = true;
			}



			if (checkCollisionTop(this, tileCoords, levelData)) {
				y -= (y + (height) / 2) - ((-TILE_SIZE *tileCoords.y) - TILE_SIZE);
				//y += fabs((y + (height) / 2) - ((-TILE_SIZE *tileCoords.y) - TILE_SIZE));
				velocity_y = 0.0;
				//fabs((-TILE_SIZE *tileCoords.y) - (y - ((height)) / 2.0))
			}

			//movement along x axis
			float minX = -3.5f;
			float maxX = 3.5f;
			if (keys[SDL_SCANCODE_LEFT] /*&& !collidedLeft&& (x - (width / 2.0) >= minX)*/) { //apply leftward movemnt
				//movingLeft = true;
				//movingRight = false;
				//velocity_x = lerp(velocity_x, 0.0f, elapsed * friction_x);
				//velocity_x += acceleration_x * elapsed;
				if(xDirection > 0)
					xDirection *= -1;
				x += xDirection * velocity_x * elapsed;


			}


			if (keys[SDL_SCANCODE_RIGHT]/*&& (x + (width / 2.0) <= maxX)*/) {//apply rightward movement
				/*movingLeft = false;
				movingRight = true;*/
				//velocity_x = lerp(velocity_x, 0.0f, elapsed * friction_x);
				//velocity_x += acceleration_x * elapsed;
				if (xDirection < 0)
					xDirection *= -1;
				x += xDirection * velocity_x * elapsed;
			}
			if (checkCollisionLeft(this, tileCoords, levelData)) {
				x += fabs(((TILE_SIZE * tileCoords.x) + TILE_SIZE) - (x - width / 2));
				//velocity_x = 0.0;
			}
			if (checkCollisionRight(this, tileCoords, levelData)) {
				x -= (x + width / 2) - ((TILE_SIZE * tileCoords.x)) + 0.002;
				//velocity_x = 0.0;
			}
			//scroll screen
			//viewM.Translate(x, y, 0.0);
		}
		else if (entityType == ENTITY_ZOMBIE) {
			//move on y-axis

			velocity_y += gravity; //apply downward movement
			y += 1 * velocity_y * elapsed;
			if (checkCollisionBottom(this, tileCoords, levelData)) {
				//player->gravityOn = false;
				//float tilebyGcoord = -TILE_SIZE *tileCoords.y;
				//float entityBottom = player->y - (player->height) / 2;
				y += fabs((-TILE_SIZE *tileCoords.y) - (y - ((height)) / 2.0));

				//player->y = 0.5;
				velocity_y = 0.0;
				//player->collidedBottom = false;
				//player->grounded = true;
			}
			x += paceSpeed * xDirection * elapsed;

			if (checkCollisionLeft(this, tileCoords, levelData)) {
				x += fabs(((TILE_SIZE * tileCoords.x) + TILE_SIZE) - (x - width / 2));
				//velocity_x = 0.0;
			}
			if (checkCollisionRight(this, tileCoords, levelData)) {
				x -= (x + width / 2) - ((TILE_SIZE * tileCoords.x)) + 0.002;
				//velocity_x = 0.0;
			}
		}
		
	}

	void Render() {
		
	}

	void drawParticles() {
		if (bleeding) {
			particleSystem->Render(program);
		}
	}

	void drawSprite() {
		//drawParticles();
		program->setModelMatrix(modelM);
		//program->setProjectionMatrix(projectionMatrix);
		//program->setViewMatrix(viewM);
		modelM.identity();
		//viewM.identity();
		modelM.Translate(x, y, 0.0);
		modelM.Scale(width, height, 0.0);
		if (entityType == ENTITY_BULLET && xDirection > 0) {
			modelM.Rotate(80);
		}
		else if (entityType == ENTITY_BULLET && xDirection < 0) {
			modelM.Rotate(-80);
		}

		mySprite.Draw();
	}

	void drawUniformSprite(int index, int sprcount_x, int sprcount_y) {
		program->setModelMatrix(modelM);
		modelM.identity();
		modelM.Translate(x, y, 0.0);
		modelM.Scale(width, height, 0.0);
		mySprite.DrawUniformSpr(program, index, sprcount_x, sprcount_y);
	}

	void drawUniformSensor(int index, int sprcount_x, int sprcount_y) {
		program->setModelMatrix(modelM);
		modelM.identity();
		modelM.Translate(x+0.3, y-0.3, 0.0);
		modelM.Scale(width, height, 0.0);
		sensorSprite.DrawUniformSpr(program, index, sprcount_x, sprcount_y);
	}
};

std::vector<Entity*> zombies;
int numZombies = 10;

//class Bullet : public Entity {
//
//public:
//
//	Bullet(float xDirect, float yDirect, float rState, float width, float height, float speed,
//		float xPosition, float yPosition, Matrix &modelMatrix, Matrix &viewMatrix, ShaderProgram* program, ShaderProgram* ps_program, SheetSprite newSprite, float angle, float timeAlive)
//		: Entity(xDirect,  yDirect,  width,  height,  speed,  x,  y,  &modelMatrix, &viewMatrix, program,  ps_program, newSprite)  {
//			
//	}
//	//Entity(float xDir, float yDir, float width, float height, float speed, float x, float y, Matrix &modelMatrix, Matrix &viewMatrix, ShaderProgram* program, ShaderProgram* ps_program, SheetSprite newSprite)
//	
//
//};
std::vector<Entity*> bullets;




void shootBullet(ShaderProgram* program, ShaderProgram* ps_program, SheetSprite b_sprite, Entity* player, Matrix &modelM, Matrix &viewM, Mix_Chunk *bulletSound) {
	Entity* newBullet = new Entity(1.0f,1.0f,0.1f,0.1f,3.0f,player->x,player->y,modelM,viewM,program,ps_program,b_sprite);
	newBullet->entityType = ENTITY_BULLET;
	Mix_PlayChannel(-1, bulletSound, 0);
	bullets.push_back(newBullet);
}

bool shouldRemoveZombie(Entity* zombie) {
	if (zombie->y <= -5.0) {
		return true;

	}
	else return false;
}

bool shouldRemoveBullet(Entity* bullet) {
	
	if (bullet->timeAlive > 2) {
		return true;
	}
	else {
		return false;
	}

	/*if ((bullet->x) >= maxX) {
		return true;
	}
	else if (bullet->x <= minX) {
		return true;
	}
	else {
		return false;
	}*/
	return false;
}


bool checkBulletCollision(Entity* anEnemy, Entity* aBullet) {
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


void drawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing, Matrix &modelM) {
	//modelM.identity();
	program->setModelMatrix(modelM);
	
	//modelM.Translate(-2.9, 0.0, 0.0);
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





void resetGame(ShaderProgram* program, Matrix &modelM) {

}




void UpdateGameLevel(ShaderProgram* program, float &elapsed, Matrix &modelM, std::string &text, Entity* player, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], Matrix &viewM, Mix_Chunk *biteSound) {
	player->Update(elapsed,levelData);

	if (player->timeAlive > 4)
		player->entityState = DEAD;

	for (int i = 0; i < zombies.size(); i++) {

		if (zombies[i] != nullptr) {
			if (shouldRemoveZombie(zombies[i])) {
				delete zombies[i];
				zombies[i] = nullptr;
			}
		}
	}
	

	for (Entity* eachZombie : zombies) {
		//if (eachZombie->isNear(player)) {
		if (eachZombie != nullptr) {
			eachZombie->canRender = true;
			eachZombie->manageStates(levelData, player,biteSound,elapsed);
			eachZombie->Update(elapsed, levelData);
			if (eachZombie->entityState == PACING) {
				player->beingFollowed = false;
			}
			else {
				player->beingFollowed = true;
			}
			//}
		}
	}
	player->manageParticles(elapsed, zombies);
	if (player->beingFollowed)
		screenShakeValue += elapsed;




	//move the bullets
	float bulletTop, bulletBottom, bulletLeft, bulletRight;
	for (int i = 0; i < bullets.size(); i++) {

		if (bullets[i] != nullptr) {
			if (shouldRemoveBullet(bullets[i])) {
				delete bullets[i];
				bullets[i] = nullptr;
			}
		}
		if (bullets[i] != nullptr) {
			if (bullets[i]->y < 0) {
				bullets[i]->x += elapsed * bullets[i]->speed * player->xDirection;

			}
			else if (bullets[i]->y >= 0) {
				bullets[i]->x += elapsed * bullets[i]->speed * player->xDirection;
				//std::cout << "bullets position: " << bullets[i]->y << std::endl;
			}
		}
	}
	//Check if each of the bullets has hit any of the enemies
	for (int i = 0; i < bullets.size(); i++) {
		for (int j = 0; j < zombies.size(); j++) {
			if (zombies[j] != nullptr && bullets[i] != nullptr) {
				if (checkBulletCollision(zombies[j], bullets[i])) {
					//std::cout << "bullets y position: " << bullets[i]->y << std::endl;
					//std::cout << " hit enemy y position: " << zombies[j]->y << std::endl;
					//std::cout << "bullets x position: " << bullets[i]->x << std::endl;
					//std::cout << " hit enemy x position: " << zombies[j]->x << std::endl;
					delete bullets[i];
					bullets[i] = nullptr;
					delete zombies[j];
					zombies[j] = nullptr;

				}
			}
		}
	}
	for (int i = 0; i < bullets.size(); i++) {
		if (bullets[i] != nullptr) {
			if (bullets[i]->entityType == ENTITY_BULLET) {
				bullets[i]->timeAlive += elapsed;
				std::cout << "bullets index: " << i << " time alive: " << bullets[i]->timeAlive << std::endl;
			}
		}
	}
}

void RenderGameLevel(ShaderProgram* program, Matrix &modelM, std::string &text, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], std::vector<GLuint> sSheetIds, Entity* player, Matrix& viewM) {
	
	
	

	//program->setModelMatrix(viewM);

	//modelM.identity();
	//modelM.Translate(-3.5, 2.0, 0.0);

	

	program->setModelMatrix(modelM);
	program->setViewMatrix(viewM);
	viewM.identity();
	viewM.Translate(-player->x, -player->y, 0.0);
	
	if (player->beingFollowed)
		viewM.Translate(0.0f, sin(screenShakeValue * screenShakeSpeed)* screenShakeIntensity, 0.0f);
	

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
				v *= spriteHeight * 2;
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


	for (Entity* eachZombie : zombies) {
		if (eachZombie != nullptr) {
			if (eachZombie->canRender) {
				//program->setViewMatrix(viewM);
				//viewM.identity();
				//program->setModelMatrix(modelM);
				//modelM.identity();
				//eachZombie->viewM.identity();
				//eachZombie->viewM.Translate(-player->x, -player->y, 0.0);
				//eachZombie->drawUniformSensor(0, 5, 3);
				eachZombie->drawSprite();

			}
		}
	}

	

}

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_PLAYER_WINS, STATE_PLAYER_LOSES };
int state;

float textScrollRate = -0.0005;
float timer = 0.0;
bool start = true;


void Update(ShaderProgram* program, float &elapsed, Matrix &modelM, std::string &text, Entity* player, int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], Matrix &viewM, Mix_Chunk *biteSound) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	int numNull = 0;
	Vec2 currentCoords;
	switch (state) {
	case STATE_MAIN_MENU:
		
		if (keys[SDL_SCANCODE_S]) {
			modelM.identity();
			state = STATE_GAME_LEVEL;
		}

		//UpdateMainMenu();
		break;

	case STATE_GAME_LEVEL:
		UpdateGameLevel(program, elapsed, modelM, text, player, levelData, viewM, biteSound);
		
		currentCoords = worldToTileCoordinates(player->x, player->y);
		if (currentCoords.x >= LEVEL_WIDTH-1) {
			state = STATE_PLAYER_WINS;
		}
		//std::cout << player->entityState << std::endl;
		

		if (!(player->health >= 0) || player->y <= -5.0) {
			state = STATE_PLAYER_LOSES;
		}
		/*for (Entity* eachZombie : zombies) {
			if (eachZombie->y <= -5.0) {
				delete eachZombie ;
			}
		}*/
		
		//std::cout << "current coordinates: " << currentCoords.x << std::endl;
		break;
	case STATE_PLAYER_WINS:
		std::cout << "current state: " << state << std::endl;
		modelM.identity();
		if (keys[SDL_SCANCODE_R]) {
			state = STATE_MAIN_MENU;
			resetGame(program, modelM);
		}
		break;
	case STATE_PLAYER_LOSES:
		std::cout << "current state: " << state << std::endl;
		modelM.identity();
		if (keys[SDL_SCANCODE_R]) {
			state = STATE_MAIN_MENU;
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
		text = "Zombie Chase! Press 'S' to play";
		if (start) {
			modelM.Translate(-2.5, 0.0, 0.0);
			start = false;
		}
		//std::cout << "Main Menu" << std::endl;
		//modelM.identity();
		
		drawText(program, fontTexture, text, size, spacing, modelM);
		break;

	case STATE_GAME_LEVEL:
		//modelM.Translate(-3.5, 2.0, 0.0);
		//viewM.identity();
		
		
		//player->drawUniformSprite(0, 5, 3);
		//player->drawSprite();
		//modelM.identity();
		//program->setModelMatrix(modelM);
		//program->setViewMatrix(viewM);
		//viewM.Translate(-player->x, -player->y, 0.0);
		//viewM.identity();
		
		RenderGameLevel(program, modelM, text, levelData, sSheetIds, player, viewM);
		player->drawUniformSprite(0, 12, 8);
		
		for (int i = 0; i < bullets.size(); i++) {
			if (bullets[i] != nullptr) {
				bullets[i]->drawSprite();
			}
		}
		
		text = static_cast<std::ostringstream*>(&(std::ostringstream() << player->health))->str();
		//modelM.Translate(player->x+3.3, player->y +1.9, 0.0);
		//viewM.identity();
		//drawText(program, fontTexture, text, size, spacing, modelM);
		break;

	case STATE_PLAYER_WINS:
		text = "You've won!  Press 'R' to retry!";
		modelM.Translate(player->x-3.3, player->y, 0.0);
		//viewM.identity();
		drawText(program, fontTexture, text, size, spacing, modelM);
		break;
	case STATE_PLAYER_LOSES:
		text = "You've lost! Press 'R' to retry! ";
		//std::string text2 = "to return to main menu!";
		//modelM.Translate(-2.5, 0.0, 0.0);
		modelM.Translate(player->x-3.3, player->y, 0.0);
		
		drawText(program, fontTexture, text, size, spacing, modelM);
		
		/*modelM.Translate(player->x - 3.7, player->y - 0.3, 0.0);
		drawText(program, fontTexture, text2, size, spacing, modelM);*/
		
		break;
	}
}

void buildLevel(int levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
	int level1Data[LEVEL_HEIGHT][LEVEL_WIDTH];
	memcpy(levelData, level1Data, LEVEL_HEIGHT*LEVEL_WIDTH);
}



void pgMap(int levelData[LEVEL_HEIGHT][LEVEL_WIDTH], ShaderProgram* program, ShaderProgram* particle_program, Matrix &modelM, Matrix &viewM, SheetSprite&  spr_zombie, SheetSprite&  spr_sensor) {
	int blockCount = 0;
	int ranChance = randomGen(0, 100);
	int randomFloor = 2;
	int randomPlatLen = randomGen(5, 10);
	int zombieProb = randomGen(0, 50);
	int platSpace = randomGen(1, 3);

	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			levelData[y][x] = -1;
		}
	}
	levelData[2][2] = 4;
	levelData[LEVEL_HEIGHT][LEVEL_WIDTH] = 4;

	while (blockCount < LEVEL_WIDTH && currentSet < 70) {
		for (int i = currentSet; i < randomPlatLen + currentSet; i++) {
			//if (levelData[randomFloor][i] != -1) {
				randomFloor = randomGen(2, 4);
				levelData[randomFloor][i] = 4;
				blockCount++;
				//currentSet++;
				//std::cout << "block count: " << blockCount << "random floor: " << randomFloor << "currentSet: " << currentSet << std::endl;
			//}
		}
		for (int j = currentSet; j < platSpace + currentSet; j++) {
			levelData[randomFloor][j] = -1;
		}
		//for (int y = 1; y < LEVEL_HEIGHT; y++) {
		//	for (int x = 0; x < LEVEL_WIDTH; x++) {
		//		//std::cout << "level data (before zombies): " << levelData[y][x] << "at x: " << x << " y: " << y << std::endl;
		//	}
		//}

		platSpace = randomGen(1, 2);
		currentSet += randomPlatLen;
		/*if(ranChance > 30 && ranChance < 70)
			randomFloor = 4;	
		else if(ranChance < 30)	
			randomFloor = 5;
		else if (ranChance > 70 && ranChance < 85)
			randomFloor = 6;
		else 
			randomFloor = 7;
		*/
		randomFloor = randomGen(2, 4);
		randomPlatLen = randomGen(5, 10);
			
	}
	Vec2 coordsUp;
	Vec2 coordsInMap;
	Vec2 coordsBelow;
	Vec2 zomCoordPair;
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 11; x < LEVEL_WIDTH; x++) {
			coordsUp.x = x;
			coordsUp.y = y-1;
			coordsInMap.x = x ;
			coordsInMap.y = y;
			coordsBelow.x = x ;
			coordsBelow.y = y + 1;
			if (/*!tileSolid(levelData, coordsUp) && tileSolid(levelData, coordsInMap) && */tileSolid(levelData, coordsBelow) && numZombies > 0) {
				zomCoordPair =  TileToWorldCoordinates(x, y - 1);
				Entity* zombie = new Entity(0.0, 0.0, 0.5, 0.5, 2.0, zomCoordPair.x, zomCoordPair.y, modelM, viewM, program, particle_program, spr_zombie);
				zombie->sensorSprite = spr_sensor;
				zombie->canRender = false;
				zombie->entityType = ENTITY_ZOMBIE;
				zombies.push_back(zombie);
				std::cout << "num zombies: " << numZombies << " Zombie coord x: " << coordsUp.x << " Zombie coord y: " << coordsUp.y << std::endl;
				numZombies -= 1;
			}
			//std::cout << "level data (after zombies): " << levelData[y][x] << "at x: " << x << " y: " << y << std::endl;
		}
	}

	
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
	ShaderProgram* particle_program = new ShaderProgram(RESOURCE_FOLDER"particle_vertex_shader.glsl", RESOURCE_FOLDER"particle_fragment_shader.glsl");

	int Mix_OpenAudio(int frequency, Uint16 format, int channels, int chunksize);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	Mix_Music *music;
	music = Mix_LoadMUS("hyruletemple.mp3");
	Mix_PlayMusic(music, -1);

	Mix_Chunk *bulletSound;
	bulletSound = Mix_LoadWAV("bulletSound.wav");

	Mix_Chunk *biteSound;
	biteSound = Mix_LoadWAV("zombieAttack.wav");

	Matrix projectionMatrix;
	Matrix modelMatrix;

	Matrix viewMatrix;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program->programID);

	std::string text = "";

	GLuint spritesheet = LoadTexture("dirt-tiles.png");

	GLuint bullet_spritesheet = LoadTexture("sheet.png");

	GLuint fontTexture = LoadTexture("font1.png");

	GLuint player_S_Sheet = LoadTexture("characters_1.png");

	SheetSprite  spr_player(player_S_Sheet, 0.0f, 0.0f, 0.5f, 0.5f, 0.5, program);

	GLuint zombie_S_Sheet = LoadTexture("Zombie_2_Running.png");

	GLuint sensor_S_Sheet = LoadTexture("spritesheet_aliens.png");

	SheetSprite  spr_zombie(zombie_S_Sheet, 0.0f, 0.0f, 0.5f, 0.5f, 0.5, program);

	SheetSprite  spr_Sensor(sensor_S_Sheet, 0.0f, 0.0f, 0.2f, 0.2f, 0.5, program);

	SheetSprite spr_bullet(bullet_spritesheet, 856.0f / 1024.0f, 421.0f / 1024.0f, 9.0f / 1024.0f, 54.0f / 1024.0f, 1.0, program);
	int levelData[LEVEL_HEIGHT][LEVEL_WIDTH];



	//procedurally generate map
	pgMap(levelData, program, particle_program, modelMatrix, viewMatrix, spr_zombie, spr_Sensor);
	Vec2 startPos;

	for (int y = 1; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			if (levelData[y][x] == 4) {
				//startPos = TileToWorldCoordinates(y+1,x+1);
			}
		}
	}
	
	startPos = TileToWorldCoordinates(4, 4);
	

	Entity* player = new Entity(0.0, 0.0, 0.3, 0.3, 2.0, startPos.x, startPos.y, modelMatrix, viewMatrix, program, particle_program, spr_player);


	std::vector<GLuint> textureVect = { spritesheet };



	/*int levelData[LEVEL_HEIGHT][LEVEL_WIDTH] = {
		{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,6,-1,6,-1,-1,-1,6,6,6,6,-1,-1,-1,-1,-1,-1,6 },
		{ 6,6,6,6,6,6,6,6,-1,-1,-1,-1,6,6,6,6,6,6,6,6,6,6 },
		{ 6,6,6,6,6,-1,-1,-1,-1,-1,6,6,6,6,6,6,6,6,6,6,6,6 },
		{ 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6 },
		{ 6,-1,-1,-1,-1,-1,-1,6,6,6,6,6,6,6,6,-1,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,6,6,6,6,6,-1,-1,-1,-1,-1,-1,-1,-1,6,6,6,6,6,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,-1,6,6,6,6,6,6,6,6,-1,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }

	};*/

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
			if (keys[SDL_SCANCODE_SPACE] && state == STATE_GAME_LEVEL) {
				if (player->collidedBottom) {
					player->jumping = true;

				}
			}
			if (keys[SDL_SCANCODE_B] && state == STATE_GAME_LEVEL) {
				shootBullet(program, particle_program, spr_bullet, player, modelMatrix,viewMatrix,bulletSound);
			}
			if (keys[SDL_SCANCODE_Q] && state == STATE_GAME_LEVEL) {
				Mix_FreeChunk(bulletSound);
				Mix_FreeChunk(biteSound);
				Mix_FreeMusic(music);
				state = STATE_MAIN_MENU;
				SDL_Quit();
			}
			if (keys[SDL_SCANCODE_R] && state == STATE_GAME_LEVEL) {
				Mix_FreeChunk(bulletSound);
				Mix_FreeChunk(biteSound);
				Mix_FreeMusic(music);
				SDL_Quit();
				resetGame(program, modelMatrix);
			}

		}

		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
		Update(program, fixedElapsed, modelMatrix, text, player, levelData, viewMatrix,biteSound);
		Render(program, modelMatrix, text, fontTexture, 0.15f, 0.009f, levelData, textureVect, player, viewMatrix);
		//}






		SDL_GL_SwapWindow(displayWindow);



	}

	SDL_Quit();
	return 0;
}

