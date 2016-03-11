#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <ctime>
#include <vector>
#include <SDL_mixer.h>
#include "Matrix.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class Object{
	
public:
	Matrix modelMatrix, projectionMatrix, viewMatrix;
	float width, height = 1.0;
	float xDir, yDir = 0.0;
	float posX, posY = 0.0;
	float objSpeed = 0.0;
	float rotState = 0.0;
	Object(float wid, float hght, float xDirect, float yDirect,
		float xPosition, float yPosition, float speed, float rState) : width(wid), height(hght),
		xDir(xDirect), yDir(yDirect), posX(xPosition), posY(yPosition), objSpeed(speed), rotState(rState){}

	void drawObject(ShaderProgram program){
		float vertices[] = { 
			-0.5, -0.5,
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
		modelMatrix.Translate(posX, posY, 0.0);
	}
	void scaleObj(float x, float y, float z){
		modelMatrix.Scale(x, y, z);
	}

	void resetMatrix(){
		modelMatrix.identity();
	}

	/*std::vector<float> getObjPos(){
		std::vector<float> posVect = { posX, posY, posZ };
		return posVect;
	}*/

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

	int Mix_OpenAudio(int frequency, Uint16 format, int channels, int chunksize);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	
	Mix_Music *music;
	music = Mix_LoadMUS("hyruletemple.mp3");
	Mix_PlayMusic(music, -1);

	//Mix_FreeChunk(someSound);
	//Mix_FreeMusic(music);
	//SDL_Quit();


#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 1280, 690);
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	Object leftPaddle(0.1f, 0.7f, 1.0f, 1.0f, -5.1f, 0.0f, 3.0f, 0.0f);
	leftPaddle.setOrthoProj();
	leftPaddle.setObjMatrices(program);
	leftPaddle.translateObj(-3.2, -1.5, 0.0);
	leftPaddle.scaleObj(leftPaddle.width, leftPaddle.height, 1.0);
	
	
	Object rightPaddle(0.1f, 0.7f, 1.0f, 1.0f, 5.1f, 0.0f, 3.0f, 0.0);
	rightPaddle.setOrthoProj();
	rightPaddle.setObjMatrices(program);
	rightPaddle.translateObj(3.2, 1.5, 0.0);
	rightPaddle.scaleObj(rightPaddle.width, rightPaddle.height, 1.0);
	

	Object ball(0.1f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	ball.setOrthoProj();
	ball.setObjMatrices(program);
	ball.translateObj(0.0, 0.0, 0.0);
	ball.scaleObj(ball.width, ball.height, 1.0);
	
	Object topWall(7.65f, 0.25f, 1.0f, 1.0f, 0.0f, 1.95f, 0.0f, 0.0f);
	topWall.setOrthoProj();
	topWall.setObjMatrices(program);
	topWall.translateObj(topWall.posX, topWall.posY, 0.0);
	topWall.scaleObj(topWall.width, topWall.height, 1.0);


	Object bottomWall(7.65f, 0.25f, 1.0f, 1.0f, 0.0f, -1.95f, 0.0f, 0.0f);
	bottomWall.setOrthoProj();
	bottomWall.setObjMatrices(program);
	bottomWall.translateObj(bottomWall.posX, bottomWall.posY, 0.0);
	bottomWall.scaleObj(bottomWall.width, bottomWall.height, 1.0);

	

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

			leftPaddle.setObjMatrices(program);
			leftPaddle.drawObject(program);

			rightPaddle.setObjMatrices(program);
			rightPaddle.drawObject(program);

			ball.setObjMatrices(program);
			ball.drawObject(program);

			topWall.setObjMatrices(program);
			topWall.drawObject(program);

			bottomWall.setObjMatrices(program);
			bottomWall.drawObject(program);

			ball.posX += 0.0025 *elapsed * ball.xDir;
			ball.posY += 0.0001 * elapsed * ball.yDir;

			ball.translateObj(ball.posX, ball.posY, 0.0);

			paddleSpeed += 0.0015 * elapsed;

			
			// Ball dimensions
			float ballTop, ballBottom, ballLeft, ballRight;
			ballTop = ball.posY + (ball.height / 2.0f);
			ballBottom = ball.posY - (ball.height / 2.0f);
			ballLeft = ball.posX - (ball.width / 2.0f);
			ballRight = ball.posX + (ball.width / 2.0f);

			// Wall Dimension
			float topWallBottom = topWall.posY - (topWall.height / 2.0);
			float topWallTop = topWall.posY + (topWall.height / 2.0);
			float bottomWallBottom = bottomWall.posY - (bottomWall.height / 2.0);
			float bottomWallTop = bottomWall.posY + (bottomWall.height / 2.0);

			// Left Paddle Dimensions
			float leftPaddleTop = leftPaddle.posY + (leftPaddle.height / 2.0f);
			float leftPaddleBottom = leftPaddle.posY - (leftPaddle.height / 2.0f);
			float leftPaddleRight = leftPaddle.posX + (leftPaddle.width / 2.0f);
			float leftPaddleLeft = leftPaddle.posX - (leftPaddle.width / 2.0f);
			// Right Paddle Dimensions
			float rightPaddleTop = rightPaddle.posY + (rightPaddle.height / 2.0f);
			float rightPaddleBottom = rightPaddle.posY - (rightPaddle.height / 2.0f);
			float rightPaddleRight = rightPaddle.posX + (rightPaddle.width / 2.0f);
			float rightPaddleLeft = rightPaddle.posX - (rightPaddle.width / 2.0f);

			// Collision with the top
			if ((ballTop >= topWallBottom) || (ballBottom <= bottomWallTop)) {
				ball.yDir = -ball.yDir;
			}

			// Collisions Left Paddle - Ball
			if ((
				(leftPaddleBottom > ballTop) ||
				(leftPaddleTop < ballBottom) ||
				(leftPaddleLeft > ballRight) ||
				(leftPaddleRight < ballLeft))) {
				
			}
			else{
				ball.xDir = -ball.xDir;
				ball.yDir = -ball.yDir;
			}
			if ((
				(rightPaddleBottom < ballTop) ||
				(rightPaddleTop > ballBottom) ||
				(rightPaddleLeft > ballRight) ||
				(rightPaddleRight < ballLeft))) {
				
			}
			else{
				ball.xDir = -ball.xDir;
				ball.yDir = -ball.yDir;
			}


			/*if ((ball.getObjPos())[0] == (rightPaddle.getObjPos())[0] && (ball.getObjPos())[1] == (rightPaddle.getObjPos())[1]){
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


			////If left paddle wins
			//if ((ball.getObjPos())[0] > 3.5){
			//	glClearColor(1.0, 0.0, 0.0, 0.0);
			//}

			////If right paddle wins
			//if ((ball.getObjPos())[0]  < -3.5){
			//	glClearColor(0.0, 0.0, 1.0, 0.0);
			//}


		

		SDL_GL_SwapWindow(displayWindow);

	}


	SDL_Quit();
	return 0;

}