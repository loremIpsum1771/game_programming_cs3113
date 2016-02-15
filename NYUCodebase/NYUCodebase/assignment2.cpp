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

class Object{
	Matrix modelMatrix, projectionMatrix, viewMatrix;
	float posX, posY, posZ = 0.0;
public:

	void drawObject(ShaderProgram program){
		float vertices[] = { -0.5, -0.5,
							0.5, -0.5,
							0.5, 0.5,
							-0.5, -0.5,
							0.5, 0.5,
							-0.5, 0.5 };
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
		posZ = z;
		modelMatrix.Translate(posX, posY, posZ);
	}
	void scaleObj(float x, float y, float z){
		modelMatrix.Scale(x, y, z);
	}

	void resetMatrix(){
		modelMatrix.identity();
	}

	std::vector<float> getObjPos(){
		std::vector<float> posVect = { posX, posY, posZ };
		return posVect;
	}

	void setObjXpos(float xPos){
		posX = xPos;
	}

	void setObjYpos(float yPos){
		posY = yPos;
	}

};
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
	leftPaddle.scaleObj(0.10, 0.7, 1.0);

	Object rightPaddle;
	rightPaddle.setOrthoProj();
	rightPaddle.setObjMatrices(program);
	rightPaddle.translateObj(3.2, 1.5, 0.0);
	rightPaddle.scaleObj(0.10, 0.7, 1.0);

	Object ball;
	ball.setOrthoProj();
	ball.setObjMatrices(program);
	ball.translateObj(0.0, 0.0, 0.0);
	ball.scaleObj(0.10, 0.10, 0.0);

	float ballPosX = 0.0;
	float ballPosY = 0.0;
	
	float paddleSpeed = 0.0;
	

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

			glClear(GL_COLOR_BUFFER_BIT);
			
			leftPaddle.setObjMatrices(program);
			leftPaddle.drawObject(program);

			rightPaddle.setObjMatrices(program);
			rightPaddle.drawObject(program);

			ball.setObjMatrices(program);
			ball.drawObject(program);

			ballPosX += 0.1 *elapsed;
			ballPosY += 0.1 * elapsed;
			
			ball.translateObj(ballPosX, ballPosY, 0.0);
			
			paddleSpeed += 0.4 * elapsed;
			
			if ((ball.getObjPos())[0] == (rightPaddle.getObjPos())[0] && (ball.getObjPos())[1] == (rightPaddle.getObjPos())[1]){
				float newX = (ball.getObjPos())[0] * -1;
				ball.setObjYpos(newX);
			}

			if ((ball.getObjPos())[0] == (leftPaddle.getObjPos())[0] && (ball.getObjPos())[1] == (leftPaddle.getObjPos())[1]){
				float newX = (ball.getObjPos())[0] * -1;
				ball.setObjYpos(newX);
			}

			if ((ball.getObjPos())[1] == 1.6 || (ball.getObjPos())[1] == -1.6){
				float newX = (ball.getObjPos())[1] * -1;
				ball.setObjYpos(newX);
			}

			
			
			
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
			if ((ball.getObjPos())[0] > 3.5){
				glClearColor(1.0, 0.0, 0.0, 0.0);
			}

			//If right paddle wins
			if ((ball.getObjPos())[0]  < -3.5){
				glClearColor(0.0, 0.0, 1.0, 0.0);
			}

			
		}
		
		SDL_GL_SwapWindow(displayWindow);
		
	}
	

	SDL_Quit();
	return 0;

}


