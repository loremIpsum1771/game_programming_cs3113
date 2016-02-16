#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <ctime>
#include <vector>
#include <iostream>
#include "Matrix.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class Object{
	Matrix modelMatrix, projectionMatrix, viewMatrix;
	
	
public:
	float top,bottom,innerBound, outerBound = 0.0;
	float xDir, yDir = 0.0;
	float posX, posY = 0.0;
	void drawObject(ShaderProgram program, float vertices[] ){
		/*float vertices[] = { -0.5, -0.5,
							0.5, -0.5,
							0.5, 0.5,
							-0.5, -0.5,
							0.5, 0.5,
							-0.5, 0.5 };*/
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
	}
	void setOrthoProj(){
		projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	}
	void setObjMatrices(ShaderProgram program){
		glUseProgram(program.programID);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
			
	}

	void translateObj(float x, float y, float z){
		posX = x;
		posY = y;
		
		modelMatrix.Translate(posX, posY,0.0);
	}
	void scaleObj(float x, float y, float z){
		modelMatrix.Scale(x, y, z);
	}

	void resetMatrix(){
		modelMatrix.identity();
	}

	

	void setObjXpos(float xPos){
		posX = xPos;
	}

	void setObjYpos(float yPos){
		posY = yPos;
	}

};

//GLboolean CheckCollision(Object &one, Object& two){
//	// Collision x-axis?
//	bool collisionX = (one.getObjPos())[0] + one.width >= (two.getObjPos())[0] && (two.getObjPos())[0] + two.width >= (one.getObjPos())[0];
//	// Collision y-axis?
//	bool collisionY = (one.getObjPos())[1] + one.height >= (two.getObjPos())[1] && (two.getObjPos())[1] + two.height >= (one.getObjPos())[1];
//	// Collision only if on both axes
//	return collisionX && collisionY;
//}




SDL_Window* displayWindow;


int main(int argc, char** argv){
	float lastFrameTicks = 0.0f;

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 690, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glClearColor(1.0, 0.0, 2.0, 0.0);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 1280, 690);
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	
	Object leftPaddle;
	leftPaddle.setOrthoProj();
	leftPaddle.setObjMatrices(program);
	leftPaddle.translateObj(-3.2, -1.5, 0.0);
	//leftPaddle.scaleObj(0.10, 0.7, 1.0);
	leftPaddle.top = leftPaddle.posY + 0.35;
	leftPaddle.bottom = leftPaddle.posY - 0.35;
	leftPaddle.innerBound = -2.9f;
	float lpVerts[] = { -0.05f, -0.35f,
						0.05f, 0.35f,
						-0.05f, 0.35f,
						0.05f, 0.35f,
						-0.05f, -0.35f,
						0.05f, -0.35f };

	/*leftPaddle.width = 0.10 * 1;
	leftPaddle.height = 0.7 * 1;*/

	Object rightPaddle;
	rightPaddle.setOrthoProj();
	rightPaddle.setObjMatrices(program);
	rightPaddle.translateObj(3.2, 1.5, 0.0);
	//rightPaddle.scaleObj(0.10, 0.7, 1.0);
	rightPaddle.top = rightPaddle.posY + 0.7;
	rightPaddle.bottom = rightPaddle.posY - 0.7;
	rightPaddle.innerBound = 3.15;
	float rpVerts[] = { -0.05f, -0.35f,
		0.05f, 0.35f,
		-0.05f, 0.35f,
		0.05f, 0.35f,
		-0.05f, -0.35f,
		0.05f, -0.35f };

	Object ball;
	ball.setOrthoProj();
	ball.setObjMatrices(program);
	ball.translateObj(0.0, -2.0, 0.0);
	//.scaleObj(0.10, 0.10, 0.0);
	ball.top = ball.posY + 0.1;
	ball.bottom = ball.posY - 0.1;
	ball.innerBound = ball.posX - 0.1;
	ball.innerBound = ball.posX + 0.1;
	ball.xDir = 1.0f;
	ball.yDir = 1.0f;
	float ballVerts[] = { -0.1f, -0.1f,
		0.1f, 0.1f,
		-0.1f, 0.1f,
		0.1f, 0.1f,
		-0.1f, -0.1f,
		0.1f, -0.1f };

	float ballPosX = 0.0;
	float ballPosY = 0.0;
	
	float paddleSpeed = 0.0;

	float topBound = 2.0f;
	float lowBound = -2.0f;
	
	float rightOuterX = 3.25f;
	float leftOuterX = -3.25f;

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
			
			leftPaddle.setObjMatrices(program);
			leftPaddle.drawObject(program,lpVerts);

			rightPaddle.setObjMatrices(program);
			rightPaddle.drawObject(program,rpVerts);

			ball.setObjMatrices(program);
			ball.drawObject(program,ballVerts);

			ballPosX += 0.0025 *elapsed* ball.xDir;
			ballPosY += 0.0025 * elapsed * ball.yDir;
			
			ball.translateObj(ballPosX, ballPosY, 0.0);
			
			paddleSpeed += 0.0015 * elapsed;
			

			
			////If the ball hits the top of the screen, reverse y direction
			if (ballPosY + 0.25 > topBound || ballPosY - 0.25 < lowBound){
				ball.yDir *= -1;
			}

			//If the ball has passed either paddle
			if (ball.posX > rightOuterX || ball.posX < leftOuterX){
				ball.setObjXpos(0.0);
				ball.setObjYpos(0.0);
			}
			

			//if the ball is on the inner side of the paddle
			if ((ball.posX > rightPaddle.innerBound - 0.025
				&& (rightPaddle.bottom < ball.posY)
				&& ball.posY < rightPaddle.top))
			{
				ball.xDir *= -1;
			}
			else if ((ball.posX < leftPaddle.innerBound + 0.025)
				&& (leftPaddle.bottom < ball.posY)
				&& (ball.posY < leftPaddle.top))
			{
				ball.xDir *= -1;
			}
			


			/*if ((ball.getObjPos())[0] == (rightPaddle.getObjPos())[0] && (ball.getObjPos())[1] == (rightPaddle.getObjPos())[1]){ */
			/*if(CheckCollision(ball,rightPaddle)){
				float newX = (ball.getObjPos())[0] * -1;
				float newY = (ball.getObjPos())[1] * -1;
				ball.setObjXpos(newX);
				ball.setObjYpos(newY);
				std::cout << "collision detected" << std::endl;
			}
			if (CheckCollision(ball, leftPaddle)){
				float newX = (ball.getObjPos())[0] * -1;
				float newY = (ball.getObjPos())[1] * -1;
				ball.setObjXpos(newX);
				ball.setObjYpos(newY);
				std::cout << "collision detected" << std::endl;
			}*/
			/*
			if ((ball.getObjPos())[0] == (leftPaddle.getObjPos())[0] && (ball.getObjPos())[1] == (leftPaddle.getObjPos())[1]){
				float newX = (ball.getObjPos())[0] * -1;
				ball.setObjYpos(newX);
			}

			if ((ball.getObjPos())[1] == 1.6 || (ball.getObjPos())[1] == -1.6){
				float newX = (ball.getObjPos())[1] * -1;
				ball.setObjYpos(newX);
			}

			if ((rightPaddle.getObjPos())[1] == 1.6 ){
				rightPaddle.setObjYpos(1.6);
			}

			if ((rightPaddle.getObjPos())[1] == -1.6){
				rightPaddle.setObjYpos(-1.6);
			}
			if ((leftPaddle.getObjPos())[1] == 1.6){
				leftPaddle.setObjYpos(1.6);
			}

			if ((leftPaddle.getObjPos())[1] == -1.6){
				leftPaddle.setObjYpos(-1.6);
			}*/
			
			
			
			const Uint8 *keys = SDL_GetKeyboardState(NULL);

			if (keys[SDL_SCANCODE_UP]) {
				rightPaddle.setObjMatrices(program);
				rightPaddle.translateObj(0.0, paddleSpeed, 0.0);
			}
			else if (keys[SDL_SCANCODE_DOWN]) {
				rightPaddle.setObjMatrices(program);
				rightPaddle.translateObj(0.0, -paddleSpeed, 0.0);
			}

			if (keys[SDL_SCANCODE_W]) {
				leftPaddle.setObjMatrices(program);
				leftPaddle.translateObj(0.0, paddleSpeed, 0.0);
			}
			else if (keys[SDL_SCANCODE_S]) {
				leftPaddle.setObjMatrices(program);
				leftPaddle.translateObj(0.0, -paddleSpeed, 0.0);
			} 

			//If left paddle wins
			if (ball.posX > 3.5){
				glClearColor(1.0, 0.0, 0.0, 0.0);
			}

			//If right paddle wins
			if (ball.posX  < -3.5){
				glClearColor(0.0, 0.0, 1.0, 0.0);
			}

			
		
		
		SDL_GL_SwapWindow(displayWindow);
		
	}
	

	SDL_Quit();
	return 0;

}


