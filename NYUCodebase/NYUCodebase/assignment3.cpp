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

	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size, ShaderProgram program)
		:textureID(textureID), u(u), v(v), width(width), height(height), size(size), program(program) {
		
		
		u = 0;
		v = 0;
		width = 0;
		height = 0;
		size = 0;
	}

	void Draw() {
		//glBindTexture(GL_TEXTURE_2D, textureID);
		glUseProgram(program.programID);
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
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		//float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
	}
	float size;
	unsigned int textureID;
	float u;
	float v;
	float width ;
	float height;
	ShaderProgram program;
};



class Entity {

public:
	Entity( float xDirect, float yDirect,
		float xPosition, float yPosition, float speed, float rState, ShaderProgram program, SheetSprite newSprite) : 
		xDir(xDirect), yDir(yDirect), posX(xPosition), posY(yPosition), objSpeed(speed), rotState(rState), program(program), mySprite(newSprite) {
		 
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
	ShaderProgram program;
	//GLuint spriteSheetTexture = LoadTexture("sheet.png");
	SheetSprite mySprite;
	
	
	
	void Update(float elapsed) {	
		// move stuff and check for collisions
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
		glUseProgram(program.programID);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

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



std::vector<Entity> entities;

class Bullet : public Entity {
public:

	Bullet(ShaderProgram program, SheetSprite newSprite, float angle = 0.0, float timeAlive = 0.0,  float xDirect = 0.0, float yDirect = 0.0,
		float xPosition = 0.0, float yPosition = 0.0, float speed = 0.0, float rState = 0.0) : Entity(xDirect,yDirect,xPosition,yPosition,speed,rState,program, newSprite), 
		angle(angle), timeAlive(timeAlive) {}

	float angle;
	float timeAlive;

};

class Enemy : public Entity {

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
		entities[i].Update(elapsed);
	}
}

void RenderGameLevel() {
	for (int i = 0; i < entities.size(); i++) {
		entities[i].Render();
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


std::vector<Bullet> bullets;
void shootBullet(ShaderProgram program, SheetSprite b_sprite) {
	//Bullet(ShaderProgram program, float wid, float hght, float xDirect, float yDirect, float xPosition, float yPosition, float speed, float rState, float angle, float timeAlive)
	Bullet newBullet(program,b_sprite);
	newBullet.posX = -1.2;
	newBullet.posY = 0.0;
	newBullet.angle = (float)(45 - (rand() % 90));
	newBullet.objSpeed = 2.0;
	bullets.push_back(newBullet);
}

bool shouldRemoveBullet(Bullet bullet) {
	if (bullet.timeAlive > 0.4) {
		return true;
	}
	else {
		return false;
	}
}



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
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	GLuint spriteSheetTexture = LoadTexture("sheet.png");
	
	
	
	SheetSprite  mySprite(spriteSheetTexture, 112.0f / 1024.0f, 791.0f / 1024.0f, 112.0f / 1024.0f, 75.0f / 1024.0f, 0.3, program);
	Entity spaceship(  1.0f, 1.0f, -5.1f, 0.0f, 3.0f, 0.0f, program,mySprite);
	spaceship.objSpeed = 10;

	
	entities.push_back(spaceship);


	spaceship.setOrthoProj();
	spaceship.setObjMatrices();
	spaceship.translateObj(-3.2, -1.5, 0.0);
	spaceship.scaleObj(spaceship.mySprite.width, spaceship.mySprite.height, 1.0);

	glUseProgram(program.programID);


	////Set initial Game State
	//state = STATE_MAIN_MENU;


	GLuint fontTexture = LoadTexture(RESOURCE_FOLDER"font1.png");
	std::string text = "Space Invaders\n Press spacebar to play";
	float size = 2.0f;
	float spacing = 1.0f;

	




	/*Entity leftPaddle(0.1f, 0.7f, 1.0f, 1.0f, -5.1f, 0.0f, 3.0f, 0.0f,program);
	leftPaddle.setOrthoProj();
	leftPaddle.setObjMatrices();
	leftPaddle.translateObj(-3.2, -1.5, 0.0);
	leftPaddle.scaleObj(leftPaddle.width, leftPaddle.height, 1.0);


	Entity rightPaddle(0.1f, 0.7f, 1.0f, 1.0f, 5.1f, 0.0f, 3.0f, 0.0,program);
	rightPaddle.setOrthoProj();
	rightPaddle.setObjMatrices();
	rightPaddle.translateObj(3.2, 1.5, 0.0);
	rightPaddle.scaleObj(rightPaddle.width, rightPaddle.height, 1.0);


	Entity ball(0.1f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, program);
	ball.setOrthoProj();
	ball.setObjMatrices();
	ball.translateObj(0.0, 0.0, 0.0);
	ball.scaleObj(ball.width, ball.height, 1.0);

	Entity topWall(7.65f, 0.25f, 1.0f, 1.0f, 0.0f, 1.95f, 0.0f, 0.0f, program);
	topWall.setOrthoProj();
	topWall.setObjMatrices();
	topWall.translateObj(topWall.posX, topWall.posY, 0.0);
	topWall.scaleObj(topWall.width, topWall.height, 1.0);


	Entity bottomWall(7.65f, 0.25f, 1.0f, 1.0f, 0.0f, -1.95f, 0.0f, 0.0f, program);
	bottomWall.setOrthoProj();
	bottomWall.setObjMatrices();
	bottomWall.translateObj(bottomWall.posX, bottomWall.posY, 0.0);
	bottomWall.scaleObj(bottomWall.width, bottomWall.height, 1.0);
*/


	float paddleSpeed = 0.0;


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




		spaceship.setObjMatrices();
		//spaceship.drawEntity();
		mySprite.Draw();

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
		//	ball.yDir = -ball.yDir;
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

		glGetError();


		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_UP]) {
			/*rightPaddle.setObjMatrices();
			rightPaddle.translateObj(0.0, paddleSpeed, 0.0);*/
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			/*rightPaddle.setObjMatrices();
			rightPaddle.translateObj(0.0, -paddleSpeed, 0.0);*/
		}

		if (keys[SDL_SCANCODE_W]) {
			/*leftPaddle.setObjMatrices();
			leftPaddle.translateObj(0.0, paddleSpeed, 0.0);*/
		}
		else if (keys[SDL_SCANCODE_S]) {
			/*leftPaddle.setObjMatrices();
			leftPaddle.translateObj(0.0, -paddleSpeed, 0.0);*/
		}





		SDL_GL_SwapWindow(displayWindow);

	}


	SDL_Quit();
	return 0;

}