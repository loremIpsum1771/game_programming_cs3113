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
	float width ;
	float height;
	ShaderProgram* program;
};



class Entity {

public:
	Entity( float xDirect, float yDirect,
		float xPosition, float yPosition, float speed, float rState, ShaderProgram* program, SheetSprite newSprite) : 
		xDir(xDirect), yDir(yDirect), posX(xPosition), posY(yPosition), objSpeed(speed), rotState(rState), program(program), mySprite(newSprite)
	{	 
		 xDir = 0.0;
		 yDir = 0.0;
		 posX = 0.0;
		 posY = 0.0;
		 objSpeed = 0.0;
		 rotState = 0.0;	
	}
	Matrix modelMatrix, projectionMatrix, viewMatrix;
	float xDir, yDir = 0.0;
	float posX, posY = 0.0;
	float objSpeed = 0.0;
	float rotState = 0.0;
	
	unsigned int textureID;
	ShaderProgram* program;
	//GLuint spriteSheetTexture = LoadTexture("sheet.png");
	SheetSprite mySprite;
	
	
	
	virtual void Update(float elapsed) {	
		// move stuff and check for collisions
		//posX += elapsed;
		//posY += elapsed;


	}
	void Render() {
		// for all game elements
		// setup transforms, render sprites
	}
	//void drawEntity() {
	//	float entvertices[] = {
	//		-0.5, -0.5,
	//		0.5, -0.5,
	//		0.5, 0.5,
	//		-0.5, -0.5,
	//		0.5, 0.5,
	//		-0.5, 0.5 };
	//	/*glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	//	glEnableVertexAttribArray(program.positionAttribute);
	//	glDrawArrays(GL_TRIANGLES, 0, 6);
	//	glDisableVertexAttribArray(program.positionAttribute);*/

	//	glBindTexture(GL_TEXTURE_2D, mySprite->textureID);
	//	float entvertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	//	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, entvertices);
	//	glEnableVertexAttribArray(program.positionAttribute);
	//	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
	//	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	//	glEnableVertexAttribArray(program.texCoordAttribute);
	//	glDrawArrays(GL_TRIANGLES, 0, 6);
	//	glDisableVertexAttribArray(program.positionAttribute);
	//	glDisableVertexAttribArray(program.texCoordAttribute);
	//	//modelMatrix.Translate(1.0, 1.0, 0.0);

	//}
	void setOrthoProj() {
		projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	}
	void setObjMatrices() {
		glUseProgram(program->programID);
		program->setModelMatrix(modelMatrix);
		program->setProjectionMatrix(projectionMatrix);
		program->setViewMatrix(viewMatrix);

	}

	void translateObj(float x, float y, float z) {
		posX = x;
		posY = y;
		modelMatrix.Translate(posX, posY, 0.0);
	}
	void scaleObj(float x, float y, float z) {
		modelMatrix.Scale(x, y, z);
	}

	void resetMatrix() {
		modelMatrix.identity();
	}

	/*std::vector<float> getObjPos(){
	std::vector<float> posVect = { posX, posY, posZ };
	return posVect;
	}*/

	void setObjXpos(float xPos) {
		posX = xPos;
	}

	void setObjYpos(float yPos) {
		posY = yPos;
	}

};

class Spaceship : public Entity {
public:
	Spaceship(float xDirect, float yDirect,
		float xPosition, float yPosition, float speed, float rState, ShaderProgram* program, SheetSprite newSprite, bool movingLeft, bool movingRight): 
		Entity(xDirect, yDirect, xPosition, yPosition, speed, rState, program, newSprite), movingLeft(movingLeft), movingRight(movingRight){
		posY = 0.0;
		//translateObj(posX, posY, 0.0);
	}

	virtual void Update(float elapsed) {
		// move stuff and check for collisions
		
		if (movingLeft) {
			posX -= elapsed* 0.001;
		}

		else if (movingRight) {
			posX += elapsed * 0.001;
		}
	}

	virtual void Render() {
		setOrthoProj();
		setObjMatrices();
		translateObj(posX, 0.0, 0.0);
		mySprite.Draw();
	}
	bool movingLeft;
	bool movingRight;
};


std::vector<Entity*> entities;

class Enemy;

std::vector<std::vector<Enemy*>> enemies(5, std::vector<Enemy*>(11));

class Bullet : public Entity {

public:
	//Entity(float xDirect, float yDirect, float xPosition, float yPosition, float speed, float rState, ShaderProgram* program, SheetSprite newSprite)
	Bullet(float xDirect, float yDirect, float xPosition, float yPosition, float speed, float rState, ShaderProgram* program, SheetSprite newSprite, float angle, float timeAlive)
	 : Entity(xDirect,yDirect,xPosition,yPosition,speed,rState,program, newSprite), angle(angle), timeAlive(timeAlive) {
		angle = 0.0;
		timeAlive = 0.0;
	}
	
	float angle;
	float timeAlive;

};

class Enemy : public Entity {
public:
	Enemy(float xDirect, float yDirect,
		float xPosition, float yPosition, float speed, float rState, ShaderProgram* program, SheetSprite newSprite) :
		Entity(xDirect, yDirect, xPosition, yPosition, speed, rState, program, newSprite) {
		leftmost = 0;
		rightmost = 10;
		vectPosX = 0;
		vectPosY = 0;
		isSolid = true;
		floatingLeft = false;
		floatingRight = true;
	}
	virtual void Update(float elapsed) {
		if(enemies[0][leftmost]->posX <= -3.2){
			for (int i = 0; i < enemies.size(); i++) {
				for (int j = 0; j < enemies[i].size(); j++) {
					enemies[i][j]->posY -= 0.5; //move each row of enemies down to the next level
				}
			}
			floatingRight = true;
			floatingLeft = false;
		}
		else if (enemies[0][rightmost]->posX >= 3.2) {
			for (int i = 0; i < enemies.size(); i++) {
				for (int j = 0; j < enemies[i].size(); j++) {
					enemies[i][j]->posY -= 0.5; //move each row of enemies down to the next level
				}
			}
			floatingRight = false;
			floatingLeft = true;
		}

		if (floatingRight) {
			for (int i = 0; i < enemies.size(); i++) {
				for (int j = 0; j < enemies[i].size(); j++) {
					enemies[i][j]->posX += 0.12; //shift enemies to the right by a constant factor
				}
			}
		}
		else if (floatingLeft) {
			for (int i = 0; i < enemies.size(); i++) {
				for (int j = 0; j < enemies[i].size(); j++) {
					enemies[i][j]->posX -= 0.12; //shift enemies to the left by a constant factor
				}
			}
		}
	}

	virtual void Render() {
		setOrthoProj();
		setObjMatrices();
		translateObj(posX, posY, 0.0);
		mySprite.Draw();
	}

	bool isSolid;
	bool floatingLeft;
	bool floatingRight;
	int leftmost;
	int rightmost;
	int vectPosX;
	int vectPosY;

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


void UpdateGameLevel(float elapsed) {
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->Update(elapsed);
	}
}

void RenderGameLevel() {
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->Render();
	}
}

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL };
int state;

void Update(float elapsed) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	switch (state) {
	case STATE_MAIN_MENU:


		if (keys[SDL_SCANCODE_SPACE]) {
			state = STATE_GAME_LEVEL;
		}

		//UpdateMainMenu();
		break;

	case STATE_GAME_LEVEL:
		UpdateGameLevel(elapsed);
		break;
	}


}

void Render(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) {
	case STATE_MAIN_MENU:
		//RenderMainMenu();
		drawText(program, fontTexture, text, size, spacing);
		break;

	case STATE_GAME_LEVEL:
		RenderGameLevel();

		break;
	}

}



//void shootBullet(ShaderProgram* program, SheetSprite b_sprite) {
//	//Bullet(ShaderProgram program, float wid, float hght, float xDirect, float yDirect, float xPosition, float yPosition, float speed, float rState, float angle, float timeAlive)
//	Bullet newBullet(program,b_sprite);
//	newBullet.posX = -1.2;
//	newBullet.posY = 0.0;
//	newBullet.angle = (float)(45 - (rand() % 90));
//	newBullet.objSpeed = 2.0;
//	bullets.push_back(newBullet);
//}
//
//bool shouldRemoveBullet(Bullet bullet) {
//	if (bullet.timeAlive > 0.4) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}

//#define MAX_BULLETS 30
//std::vector<Bullet> bullets(MAX_BULLETS);
//int bulletIndex = 0;
////Bullet bullets[MAX_BULLETS];
//
//void shootBullet(float elapsed) {
//	bullets[bulletIndex].posX = -1.2;
//	bullets[bulletIndex].posY = 0.0;
//	bulletIndex++;
//	if (bulletIndex > MAX_BULLETS - 1) {
//		bulletIndex = 0;
//	}
//	for (int i = 0; i < MAX_BULLETS; i++) {
//		bullets[i].Update(elapsed);
//	}
//}

SDL_Window* displayWindow;

int main(int argc, char** argv) {
	float lastFrameTicks = 0.0f;

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 690, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	//glClearColor(1.0, 0.0, 2.0, 0.0);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 1280, 690);
	ShaderProgram* program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

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
	
	
	


	Spaceship* spaceship = new Spaceship( 1.0f, 1.0f, -5.1f, 0.0f, 3.0f, 0.0f, program,mySprite, false, false);
	spaceship->Render();
	//spaceship->posY = 0.0001;
	spaceship->objSpeed = 10;
	spaceship->translateObj(0.0, -1.6, 0.0);
	spaceship->posY = -1.6;
	entities.push_back(spaceship);
	//spaceship->posY = 0.01;
	
	//spaceship->Render();

	//Entity(float xDirect, float yDirect, float xPosition, float yPosition, float speed, float rState, ShaderProgram* program, SheetSprite newSprite)


	//float xoffset = -3.1;
	//float yoffset = 0.0;
	std::vector<std::vector<Enemy*>> enemies2(5, std::vector<Enemy*>(11));
	Enemy* testEnemy = new Enemy(1.0f, 1.0f, 0.0, 0.0, 3.0f, 0.0f, program, spr_enemy5);
	testEnemy->posX = -0.6;
	testEnemy->posY = 0.9;
	testEnemy->translateObj(-0.6, 0.9, 0.0);
	enemies2[0][0] = testEnemy;

	Enemy* testEnemy2 = new Enemy(1.0f, 1.0f, 0.0, 0.0, 3.0f, 0.0f, program, spr_enemy4);
	testEnemy2->posX = -0.52;
	testEnemy2->posY = 0.82;
	testEnemy2->translateObj(-0.52, 0.82, 0.0);
	enemies2[0][1] = testEnemy2;
	
	float xoffset = -0.6;
	float yoffset = 0.9;

	for (int i = 0; i < 11; i++) {
		Enemy* enemy_black = new Enemy(1.0f, 1.0f, 0.0, 0.0f, 3.0f, 0.0f, program, spr_enemy1);
		//enemy_black->posX = xoffset;
		//enemy_black->Render();
		enemy_black->posX = xoffset;
		enemy_black->posY = yoffset;
		enemy_black->translateObj(xoffset, yoffset, 0.0);
		enemies[0][i] = enemy_black;
		xoffset += 0.08;
	}
	xoffset = -0.6;
	yoffset -= 0.08;
	for (int i = 0; i < 11; i++) {
		Enemy* enemy_blue = new Enemy(1.0f, 1.0f, 0.0, 0.0, 3.0f, 0.0f, program, spr_enemy2);
		//enemy_blue->posX = xoffset;
		//enemy_blue->posY = yoffset;
		//enemy_blue->Render();
		enemy_blue->posX = xoffset;
		enemy_blue->posY = yoffset;
		enemy_blue->translateObj(xoffset, yoffset, 0.0);
		enemies[1][i] = enemy_blue;
		xoffset += 0.08;
	}
	xoffset = -0.6;
	yoffset -= 0.08;
	for (int i = 0; i < 11; i++) {
		Enemy* enemy_green = new Enemy(1.0f, 1.0f, 0.0, 0.0, 3.0f, 0.0f, program, spr_enemy3);
		enemy_green->posX = xoffset;
		enemy_green->posY = yoffset;
		enemy_green->translateObj(xoffset, yoffset, 0.0);
		enemies[2][i] = enemy_green;
		xoffset += 0.08;
	}

	xoffset = -0.6;
	yoffset -= 0.08;
	for (int i = 0; i < 11; i++) {
		Enemy* enemy_red1 = new Enemy(1.0f, 1.0f, 0.0, 0.0, 3.0f, 0.0f, program, spr_enemy4);
		enemy_red1->posX = xoffset;
		enemy_red1->posY = yoffset;
		enemy_red1->translateObj(xoffset, yoffset, 0.0);
		enemies[3][i] = enemy_red1;
		xoffset += 0.08;
	}

	xoffset = -0.6;
	yoffset -= 0.08;
	
	for (int i = 0; i < 11; i++) {
		Enemy* enemy_red2 = new Enemy(1.0f, 1.0f, 0.0, 0.0, 3.0f, 0.0f, program, spr_enemy5);
		enemy_red2->posX = xoffset;
		enemy_red2->posY = yoffset;
		enemy_red2->translateObj(xoffset, yoffset, 0.0);
		enemies[4][i] = enemy_red2;
		xoffset += 0.08;
	}



	/*for (int i = 0; i < MAX_BULLETS; i++) {
		bullets[i].posX = -2000.0f;
	}*/

	//should prob go in the render method
	/*spaceship.setOrthoProj();
	spaceship.setObjMatrices();
	spaceship.translateObj(-3.2, -1.5, 0.0);*/
	//spaceship.scaleObj(spaceship.mySprite.width, spaceship.mySprite.height, 1.0);

	glUseProgram(program->programID);


	//Set initial Game State
	//state = STATE_MAIN_MENU;


	GLuint fontTexture = LoadTexture(RESOURCE_FOLDER"font1.png");
	std::string text = "Space Invaders\n Press spacebar to play";
	float size = 2.0f;
	float spacing = 1.0f;


	float paddleSpeed = 0.0;
	//float xoffset = 0.0;

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

		glClear(GL_COLOR_BUFFER_BIT);
		//enemies2[0][0]->setObjMatrices();
		//enemies2[0][0]->mySprite.Draw();
		
		//enemies2[0][0]->setObjMatrices();
		//enemies2[0][0]->mySprite.Draw();

		//enemies2[0][0]->posX = -1.002;
		//enemies[0][0]->modelMatrix.Translate(-1.02, 1.0, 0.0);
		//enemies2[0][0]->translateObj(-0.002, 0.0, 0.0);
		

		//enemies2[0][1]->setObjMatrices();
		//enemies2[0][1]->posX = -1.0;
		//enemies2[0][1]->translateObj(-1.0, 0.0, 0.0);
		//enemies2[0][1]->mySprite.Draw();


		/*enemies[0][0]->setObjMatrices();
		enemies[0][0]->mySprite.Draw();*/

		entities[0]->setObjMatrices();
		entities[0]->mySprite.Draw();

		for (int i = 0; i < enemies.size(); i++) {
			for (int j = 0; j < enemies[0].size(); j++) {
				enemies[i][j]->setObjMatrices();
				enemies[i][j]->mySprite.Draw();


				/*enemies[0][j]->translateObj(xoffset, 0.0, 0.0);
				enemies[0][j]->posX = xoffset;
				enemies[0][j]->setObjMatrices();
				enemies[0][j]->mySprite.Draw();*/

				//xoffset += 0.01;
				/*enemies[2][j]->setObjMatrices();
				enemies[2][j]->mySprite.Draw();*/
			}
		}

		/*enemies[0][0]->setObjMatrices();
		enemies[0][0]->mySprite.Draw();*/

		/*spaceship->setObjMatrices();
		spaceship->mySprite.Draw();*/
		//spaceship->setObjMatrices();
		//spaceship->setOrthoProj();
		//spaceship.drawEntity();
		
		//spaceship->mySprite.Draw();


		/*leftPaddle.setObjMatrices();
		leftPaddle.drawEntity();

		rightPaddle.setObjMatrices();
		rightPaddle.drawEntity();

		ball.setObjMatrices();
		ball.drawEntity();

		topWall.setObjMatrices();
		topWall.drawEntity();

		bottomWall.setObjMatrices();
		bottomWall.drawEntity();
*/
	/*	ball.posX += 0.0025 *elapsed * ball.xDir;
		ball.posY += 0.0001 * elapsed * ball.yDir;

		ball.translateObj(ball.posX, ball.posY, 0.0);

		paddleSpeed += 0.0015 * elapsed;
*/

		// Ball dimensions
		//float ballTop, ballBottom, ballLeft, ballRight;
		//ballTop = ball.posY + (ball.height / 2.0f);
		//ballBottom = ball.posY - (ball.height / 2.0f);
		//ballLeft = ball.posX - (ball.width / 2.0f);
		//ballRight = ball.posX + (ball.width / 2.0f);

		//// Wall Dimension
		//float topWallBottom = topWall.posY - (topWall.height / 2.0);
		//float topWallTop = topWall.posY + (topWall.height / 2.0);
		//float bottomWallBottom = bottomWall.posY - (bottomWall.height / 2.0);
		//float bottomWallTop = bottomWall.posY + (bottomWall.height / 2.0);

		//// Left Paddle Dimensions
		//float leftPaddleTop = leftPaddle.posY + (leftPaddle.height / 2.0f);
		//float leftPaddleBottom = leftPaddle.posY - (leftPaddle.height / 2.0f);
		//float leftPaddleRight = leftPaddle.posX + (leftPaddle.width / 2.0f);
		//float leftPaddleLeft = leftPaddle.posX - (leftPaddle.width / 2.0f);
		//// Right Paddle Dimensions
		//float rightPaddleTop = rightPaddle.posY + (rightPaddle.height / 2.0f);
		//float rightPaddleBottom = rightPaddle.posY - (rightPaddle.height / 2.0f);
		//float rightPaddleRight = rightPaddle.posX + (rightPaddle.width / 2.0f);
		//float rightPaddleLeft = rightPaddle.posX - (rightPaddle.width / 2.0f);

		//// Collision with the top
		//if ((ballTop >= topWallBottom) || (ballBottom <= bottomWallTop)) {
		//	ball.yDir = -ball.yDir;
		//}

		//// Collisions Left Paddle - Ball
		//if ((
		//	(leftPaddleBottom > ballTop) ||
		//	(leftPaddleTop < ballBottom) ||
		//	(leftPaddleLeft > ballRight) ||
		//	(leftPaddleRight < ballLeft))) {

		//}
		//else {
		//	ball.xDir = -ball.xDir;
		//	ball.yDir = -ball.y		Dir;
		//}
		//if ((
		//	(rightPaddleBottom < ballTop) ||
		//	(rightPaddleTop > ballBottom) ||
		//	(rightPaddleLeft > ballRight) ||
		//	(rightPaddleRight < ballLeft))) {

		//}
		//else {
		//	ball.xDir = -ball.xDir;
		//	ball.yDir = -ball.yDir;
		//}

		//glGetError();
		paddleSpeed += 0.0005 * elapsed;

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_LEFT]) {
			/*spaceship->movingRight = false;
			spaceship->movingLeft = true;
			spaceship->Update(elapsed);*/
			spaceship->setObjMatrices();
			spaceship->posX = -paddleSpeed;
			spaceship->translateObj( -paddleSpeed,0.0, 0.0);

		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			/*spaceship->movingLeft = false;
			spaceship->movingRight = true;
			spaceship->Update(elapsed);*/
			spaceship->setObjMatrices();
			spaceship->posX = paddleSpeed;
			spaceship->translateObj( paddleSpeed, 0.0, 0.0);
		}

		if (keys[SDL_SCANCODE_SPACE]) {
			//shootBullet(elapsed);
			/*leftPaddle.setObjMatrices();
			leftPaddle.translateObj(0.0, paddleSpeed, 0.0);*/
		}
		/*for (int i = 0; i < enemies.size(); i++) {
			for (int j = 0; j < enemies[0].size(); j++) {
				enemies[i][j]->Update(elapsed);
			}
		}
		for (int i = 0; i < enemies.size(); i++) {
			for (int j = 0; j < enemies[0].size(); j++) {
				enemies[i][j]->Render();
			}
		}*/
		
		
		//entities[0]->translateObj(spaceship->posX, spaceship->posY, 0.0);
		//spaceship->Render();

		SDL_GL_SwapWindow(displayWindow);

	}


	SDL_Quit();
	return 0;

}