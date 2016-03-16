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


		/*	u = 0;
		v = 0;
		width = 0;
		height = 0;
		size = 0;*/
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
		program->setModelMatrix(modelM);
		//program->setProjectionMatrix(projectionMatrix);
		//program->setViewMatrix(viewMatrix);
		modelM.identity();
		modelM.Translate(x, y, 0.0);
		modelM.Scale(width, height, 0.0);
		mySprite.Draw();

	}
};


class Spaceship : public Entity {
public:


	Spaceship(float xDirect, float yDirect, float rState, float width, float height, float speed,
		float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite, bool movingLeft, bool movingRight) :
		Entity(xDirect, yDirect, rState, width, height, speed, xPosition, yPosition, modelMatrix, program, newSprite), movingLeft(movingLeft), movingRight(movingRight) {}

	virtual void update(float elapsed) {

	}

	bool movingLeft;
	bool movingRight;
};

class Enemy : public Entity {
public:
	Enemy(float xDirect, float yDirect, float rState, float width, float height, float speed,
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

	Bullet(float xDirect, float yDirect, float rState, float width, float height, float speed,
		float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite, float angle, float timeAlive)
		: Entity(xDirect, yDirect, rState, width, height, speed, xPosition, yPosition, modelMatrix, program, newSprite), angle(angle), timeAlive(timeAlive) {
		//angle = 0.0;
		//timeAlive = 0.0;
	}

	float angle;
	float timeAlive;

};

Enemy::Enemy(float xDirect, float yDirect, float rState, float width, float height, float speed,
	float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite) :
	Entity(xDirect, yDirect, rState, width, height, speed, xPosition, yPosition, modelMatrix, program, newSprite) {
	leftmost = 0;
	rightmost = 10;
	vectPosX = 0;
	vectPosY = 0;
	isSolid = true;
	floatingLeft = false;
	floatingRight = true;
}
std::vector<Enemy*> entities;


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

std::vector<Bullet*> bullets;

void shootBullet(ShaderProgram* program, SheetSprite b_sprite, Spaceship* aShip, Matrix &modelM) {
	//Bullet(float xDirect, float yDirect, float rState, float width, float height, float speed, float xPosition, float yPosition, Matrix &modelMatrix, ShaderProgram* program, SheetSprite newSprite, float angle, float timeAlive)
	Bullet* newBullet = new Bullet(1.0, 1.0, 0.0, 0.5, 0.5, 4.0, aShip->x, aShip->y, modelM, program, b_sprite, 0.0, 0.0);

	bullets.push_back(newBullet);
}

bool shouldRemoveBullet(Bullet* bullet) {
	float maxY = 2.0;

	if ((bullet->y) >= maxY) {
		return true;
	}
	else {
		return false;
	}
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



void UpdateGameLevel(ShaderProgram* program,  float &elapsed,  Spaceship* spaceship, std::vector<Enemy*>& entities, Matrix &modelM, std::string &text) {
	
	float minX = -3.50f;
	float maxX = 3.50f;

	//move the enemies
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i] != nullptr) {
			entities[i]->x += elapsed * 0.3 * entities[i]->xDirection;
			if (entities[i]->x >= maxX || entities[i]->x <= minX) {
				for (int k = 0; k < entities.size(); k++) {
					if (entities[k] != nullptr) {
						entities[k]->xDirection *= -1;
					}
				}
				for (int j = 0; j < entities.size(); j++) {
					if (entities[j] != nullptr) {
						entities[j]->y -= 0.02;
					}
				}
			}
		}
	}

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
				bullets[i]->y += elapsed * bullets[i]->speed * 1;

			}
			else if (bullets[i]->y >= 0) {
				bullets[i]->y += elapsed * bullets[i]->speed;
				//std::cout << "bullets position: " << bullets[i]->y << std::endl;
			}
		}
		}
	//Check if each of the bullets has hit any of the enemies
	for (int i = 0; i < bullets.size(); i++) {
		for (int j = 0; j < entities.size(); j++) {
			if (entities[j] != nullptr && bullets[i] != nullptr) {
				if (checkCollision(entities[j], bullets[i])) {
					std::cout << "bullets y position: " << bullets[i]->y << std::endl;
					std::cout << " hit enemy y position: " << entities[j]->y << std::endl;
					std::cout << "bullets x position: " << bullets[i]->x << std::endl;
					std::cout << " hit enemy x position: " << entities[j]->x << std::endl;
					delete entities[j];
					entities[j] = nullptr;
					delete bullets[i];
					bullets[i] = nullptr;
				}
			}
		}
	}


}

void RenderGameLevel(ShaderProgram* program, Matrix &modelM, std::string &text,  Spaceship* spaceship, std::vector<Enemy*> entities) {
	spaceship->drawSprite();



	for (int i = 0; i < entities.size(); i++) {
		if (entities[i] != nullptr) {
			entities[i]->drawSprite();
		}
	}

	for (int i = 0; i < bullets.size(); i++) {
		if (bullets[i] != nullptr) {
			bullets[i]->drawSprite();
		}
	}
}

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL };
int state;


void Update(ShaderProgram* program, float &elapsed,  Spaceship* spaceship, std::vector<Enemy*>& entities, Matrix &modelM, std::string &text) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	switch (state) {
	case STATE_MAIN_MENU:

		//std::cout << "Game Started" << std::endl;
		if (keys[SDL_SCANCODE_S]) {
			state = STATE_GAME_LEVEL;
		}

		//UpdateMainMenu();
		break;

	case STATE_GAME_LEVEL:
		//UpdateGameLevel(ShaderProgram* program, float &elapsed, Spaceship* spaceship, std::vector<Enemy*>& entities, Matrix &modelM, std::string &text) {
		UpdateGameLevel(program, elapsed,  spaceship, entities, modelM, text);
		break;
	}


}

void Render(ShaderProgram* program, Matrix &modelM,  std::string &text,  Spaceship* spaceship, std::vector<Enemy*> entities, int fontTexture, float size, float spacing) {
	text = "Space Invaders\nPress space to play";
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) {
	case STATE_MAIN_MENU:
		//RenderMainMenu();
		std::cout << "Main Menu" << std::endl;
		drawText(program, fontTexture, text, size, spacing);
		break;

	case STATE_GAME_LEVEL:
		
		RenderGameLevel(program, modelM, text, spaceship, entities);

		break;
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
	
	Matrix projectionMatrix;
	Matrix modelMatrix;
	
	Matrix viewMatrix;
	
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program->programID);

	std::string text = "";

	GLuint spritesheet = LoadTexture("sheet.png");

	GLuint fontTexture = LoadTexture("font1.png");

	SheetSprite  mySprite(spritesheet, 112.0f / 1024.0f, 791.0f / 1024.0f, 112.0f / 1024.0f, 75.0f / 1024.0f, 0.22, program);

	SheetSprite  spr_enemy1(spritesheet, 423.0f / 1024.0f, 728.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.06, program);

	SheetSprite  spr_enemy2(spritesheet, 425.0f / 1024.0f, 468.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.06, program);

	SheetSprite  spr_enemy3(spritesheet, 425.0f / 1024.0f, 552.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.06, program);

	SheetSprite  spr_enemy4(spritesheet, 425.0f / 1024.0f, 384.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.06, program);

	SheetSprite  spr_enemy5(spritesheet, 120.0f / 1024.0f, 520.0f / 1024.0f, 104.0f / 1024.0f, 84.0f / 1024.0f, 0.06, program);

	SheetSprite spr_bullet(spritesheet, 856.0f / 1024.0f, 421.0f / 1024.0f, 9.0f / 1024.0f, 54.0f / 1024.0f, 0.2, program);

	Spaceship* spaceship = new Spaceship(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 2.0f, 0.0f, -1.67f, modelMatrix, program, mySprite, false, false);

	float xoffset = -3.2;
	float yoffset = 1.85;

	for (int i = 0; i < 11; i++) {
		Enemy* enemy_black = new Enemy(1.0f, 1.0f, 0.0, 2.0f, 2.0f, 0.0f, xoffset, yoffset, modelMatrix, program, spr_enemy1);

		//enemyMatrix.enemyVect[0][i] = enemy_black;
		entities.push_back(enemy_black);
		xoffset += 0.3;
	}
	xoffset = -3.2;
	yoffset -= 0.2;
	//for (int i = 0; i < 11; i++) {
	//	Enemy* enemy_blue = new Enemy(1.0f, 1.0f, 0.0, 2.0f, 2.0f, 0.0f, xoffset, yoffset, modelMatrix, program, spr_enemy2);
	//	//enemy_blue->posX = xoffset;
	//	//enemy_blue->posY = yoffset;
	//	//enemy_blue->Render();

	//	//enemyMatrix.enemyVect[1][i] = enemy_blue;
	//	entities.push_back(enemy_blue);

	//	xoffset += 0.3;
	//}
	//xoffset = -3.2;
	//yoffset -= 0.2;
	//for (int i = 0; i < 11; i++) {
	//	Enemy* enemy_green = new Enemy(1.0f, 1.0f, 0.0, 2.0f, 2.0f, 0.0f, xoffset, yoffset, modelMatrix, program, spr_enemy3);

	//	//enemyMatrix.enemyVect[2][i] = enemy_green;
	//	entities.push_back(enemy_green);
	//	xoffset += 0.3;
	//}

	//xoffset = -3.2;
	//yoffset -= 0.2;
	//for (int i = 0; i < 11; i++) {
	//	Enemy* enemy_red1 = new Enemy(1.0f, 1.0f, 0.0, 2.0f, 2.0f, 0.0f, xoffset, yoffset, modelMatrix, program, spr_enemy4);

	//	//enemyMatrix.enemyVect[3][i] = enemy_red1;
	//	entities.push_back(enemy_red1);
	//	xoffset += 0.3;
	//}

	//xoffset = -3.2;
	//yoffset -= 0.2;

	//for (int i = 0; i < 11; i++) {
	//	Enemy* enemy_red2 = new Enemy(1.0f, 1.0f, 0.0, 2.0f, 2.0f, 0.0f, xoffset, yoffset, modelMatrix, program, spr_enemy5);

	//	entities.push_back(enemy_red2);
	//	//enemyMatrix.enemyVect[4][i] = enemy_red2;
	//	xoffset += 0.3;
	//}
	float angle = 0;

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
			if (keys[SDL_SCANCODE_SPACE]) {
				shootBullet(program, spr_bullet, spaceship, modelMatrix);
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;


		glClear(GL_COLOR_BUFFER_BIT);


		float minX = -3.5f;
		float maxX = 3.5f;
		if (keys[SDL_SCANCODE_LEFT] && (spaceship->x - (spaceship->width / 2.0) >= minX)) {
			spaceship->movingLeft = true;
			spaceship->movingRight = false;
			spaceship->x += (-1 * elapsed * spaceship->speed);
		}
		if (keys[SDL_SCANCODE_RIGHT] && (spaceship->x + (spaceship->width / 2.0) <= maxX)) {
			spaceship->movingLeft = false;
			spaceship->movingRight = true;
			spaceship->x += (1 * elapsed * spaceship->speed);
		}
		
		
		Update(program, elapsed,  spaceship, entities, modelMatrix, text);
		Render(program, modelMatrix, text, spaceship, entities, fontTexture, 2.3f, 0.05f);





		SDL_GL_SwapWindow(displayWindow);



	}

	SDL_Quit();
	return 0;
}
