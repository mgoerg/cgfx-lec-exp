// main.cpp

#include <SDL2/SDL.h>
#include "GL/glew.h"
#include <SDL2/SDL_opengl.h>


#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>


#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>  // glm::value_ptr()

#include "shader.h"
#include "mesh.h"
#include "tilemap3d.h"
#include "importMagicaVoxel.h"
#include "utils.h"
#include "camera.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
bool initGL();

//Input handler
// void handleKeys( unsigned char key, int x, int y );

//Per frame update
void update();

//Renders quad to the screen
void renderFrame();

//Frees media and shuts down SDL
void closeProgram();

//Shader loading utility programs
void printProgramLog( GLuint program );

SDL_Window* gWindow = NULL;
SDL_GLContext gContext;
bool gRenderQuad = true;

std::string gVertexShaderFile = "data/shader/vertex.glsl";
std::string gFragmentShaderFile = "data/shader/fragment.glsl";
const char* gModelFile = "data/model/teapot.vox";

//Mesh gTetraMesh;
TileMap3d* gTileMap;
TileMap3d* gModelTileMap;

float gCameraA = 0;
float gCameraB = 0;
float gR = 20;
glm::vec3 gCameraPosition;
glm::vec3 gCameraTargetPosition = {0, 0, 0};

bool gGameIsRunning = true;
ShaderProgram* gShaderProgram = NULL;

MV::Model model;

unsigned int VBO, VAO;


Camera gCamera;



bool init()
{
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 )
	{
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	//Create window
	gWindow = SDL_CreateWindow( "Title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
	if( gWindow == NULL )
	{
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	//Create context
	gContext = SDL_GL_CreateContext( gWindow );
	if( gContext == NULL )
	{
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	//Initialize GLEW
	glewExperimental = GL_TRUE; 
	GLenum glewError = glewInit();
	if( glewError != GLEW_OK )
	{
		std::cout << "Error initializing GLEW! " << glewGetErrorString(glewError);
		return false;
	}

	//Use Vsync
	if( SDL_GL_SetSwapInterval( 1 ) < 0 )
	{
		std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError();
	}

	//Initialize OpenGL
	if( !initGL() )
	{
		std::cout << "Unable to configure OpenGL!" << std::endl;
		return false;
	}


	bool success;
	gShaderProgram = new ShaderProgram(success, gVertexShaderFile, gFragmentShaderFile, "", true);
	if (!success) {
		std::cout << "Could not initialize Shaders!" << std::endl;
		return false;
	}

	gShaderProgram->use();

	gModelTileMap = MV::makeTileMaps(gModelFile, true, success)[0];
	if (!success) {
		return false;
	}
	gModelTileMap->mesh->setup(*gShaderProgram);

	gCamera.fov = glm::radians(45.f);
	gCamera.near = 0.1f;
	gCamera.far = 10000;
	gCamera.eyePosition = {0, 0, 0};

	Palette palette = {
		{glm::vec4()}, 
		{glm::vec4(1.0, 0.4, 0.0, 1.0)},
		{glm::vec4(0.0, 0.6, 1.0, 1.0)},
		{glm::vec4(0.2, 0.0, 0.5, 1.0)}
	};

	gTileMap = new TileMap3d(palette, 4);
	gTileMap->set(1, 0, 0, 1);
	gTileMap->set(2, 0, 0, 1);
	gTileMap->set(0, 1, 0, 2);
	gTileMap->set(0, 2, 0, 2);
	gTileMap->set(0, 0, 1, 3);
	gTileMap->set(0, 0, 2, 3);
	// gTileMap->set(1, 1, 1, 1);
	// gTileMap->set(1, 1, 2, 2);
	// gTileMap->set(1, 2, 2, 1);
	// gTileMap->set(0, 2, 2, 2);
	// gTileMap->set(3, 2, 2, 3);
	//gTileMap->set(0, 0, 0, 3);
	//gTileMap->set(0, 1, 0, 3);
	gTileMap->makeMesh();

	gTileMap->mesh->setup(*gShaderProgram);


	struct LightSource
	{
		float color[3] = {1.0, 1.0, 0.1};
		int flags = (3 << 30);
		float position[3] = {-1.0, -2.0, -5};
		float intensity = 1;
		float attenuationConst = 1.0;
		float attenuationLin = 1.0;
		float attenuationSq = 1.0;
		float _pad1;
	};

	struct LightData 
	{
		int num_lights = 3;
		float _pad1;
		float _pad2;
		float _pad3;
		LightSource light1;
		LightSource light2;
		LightSource light3;
	} lights;


	LightSource ambient;
	ambient.flags |= 1;
	ambient.color[0] = 0.3;
	ambient.color[1] = 0.3;
	ambient.color[2] = 0.3;

	LightSource pointLight;
	pointLight.intensity = 50;
	pointLight.flags |= 4;
	pointLight.position[3] = 5;

	LightSource directionLight;
	directionLight.intensity = 0.3;
	directionLight.flags |= 2;


	lights.light1 = ambient;
	lights.light2 = directionLight;
	lights.light3 = pointLight;


	GLuint uniformBlockIndex = glGetUniformBlockIndex( gShaderProgram->ID, "LightBlock");

	GLint blockSize;
	glGetActiveUniformBlockiv(gShaderProgram->ID, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

	int blockbinding = 1;
	glUniformBlockBinding(gShaderProgram->ID, uniformBlockIndex, blockbinding );

	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);

	glBufferData(GL_UNIFORM_BUFFER, 
		blockSize, 
		&lights, 
		GL_STATIC_DRAW);
	
	glBindBufferRange(GL_UNIFORM_BUFFER, blockbinding, ubo, 0, blockSize);
	//glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockIndex, ubo);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	gShaderProgram->setUniform("u_useLighting", GL_TRUE);


	// const int num = 25;
	// int foo[num];
	// GLuint indices[num];
	// for (int i = 0; i < num; i++) {
	// 	indices[i] = i;
	// }
 	// glGetActiveUniformsiv( gShaderProgram->ID, num, indices, GL_UNIFORM_OFFSET, &foo[0]);
	// for (int i = 0; i < num; i++) {
	// 	std::cout << i << ": " << foo[i] << std::endl;
	// }

	return true;
}


bool initGL()
{
	//Success flag
	bool success = true;

	const GLubyte* renderer = glGetString( GL_RENDERER ); /* get renderer string */
	const GLubyte* version = glGetString( GL_VERSION );	 /* version as a string */
	printf( "Renderer: %s\n", renderer );
	printf( "OpenGL version supported %s\n", version );
	// int unibl;
	// glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &unibl);
	// std::cout << "Max uniform blocks " << unibl;

	// Config
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor( 0.f, 0.0f, 0.1f, 1.f );
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	return success;
}

// void handleKeys( SDL_KeyboardEvent e, int x, int y )
// {
// 	SDL_Keysym keysym = e.keysym;
// 	SDL_Keycode sym = keysym.sym;

// 	float step = 5.0f * glm::radians(1.0);
// 	switch (sym) {
// 		case SDLK_q:
// 		case SDLK_ESCAPE:
// 			gGameIsRunning = false;
// 			break;

// 		case SDLK_r:
// 			gRenderQuad = !gRenderQuad;
// 			break;
// 		case SDLK_w:
// 			gCameraB = std::min(gCameraB + step, glm::radians(89.0f));
// 			break;
// 		case SDLK_s:
// 			gCameraB = std::max(gCameraB - step, glm::radians(0.1f));
// 			break;
// 		case SDLK_a:
// 			gCameraA += step;
// 			break;
// 		case SDLK_d:
// 			gCameraA -= step;
// 			break;
// 	}
// }

void update()
{
	const Uint8* inputState = SDL_GetKeyboardState(NULL);
	if (inputState[SDL_SCANCODE_Q] || inputState[SDL_SCANCODE_ESCAPE]) {
		gGameIsRunning = false;
	}

	float angleStep = 2.5f * glm::radians(1.0);
	float posStep = 0.4;
	glm::vec3 axis = glm::vec3(0, 0, 1);
	glm::vec3 screenRight = glm::vec3(1, 0, 0);
	glm::vec3 screenUp = glm::vec3(0, 1, 0);

	if (inputState[SDL_SCANCODE_A]) {
		gCamera.rotation = glm::normalize(gCamera.rotation * glm::angleAxis(-angleStep, axis));
	}
	if (inputState[SDL_SCANCODE_D]) {
		gCamera.rotation = glm::normalize(gCamera.rotation * glm::angleAxis(angleStep, axis));
	}
	if (inputState[SDL_SCANCODE_W]) {
		glm::quat rotation = gCamera.rotation;
		gCamera.rotation = glm::normalize(gCamera.rotation * glm::angleAxis(angleStep, glm::normalize(glm::cross(axis, gCamera.forwardVec()))));
		if (glm::distance(axis, gCamera.forwardVec()) < 0.1)
			gCamera.rotation = rotation;
	}
	if (inputState[SDL_SCANCODE_S]) {
		glm::quat rotation = gCamera.rotation;
		gCamera.rotation = glm::normalize(gCamera.rotation * glm::angleAxis(-angleStep, glm::normalize(glm::cross(axis, gCamera.forwardVec()))));
		if (glm::distance(-axis, gCamera.forwardVec()) < 0.1)
			gCamera.rotation = rotation;
	}

	if (inputState[SDL_SCANCODE_SPACE]) {
		gCamera.eyePosition += gCamera.forwardVec() * posStep;
	}
	if (inputState[SDL_SCANCODE_LSHIFT]) {
		gCamera.eyePosition -= gCamera.forwardVec() * posStep;
	}
	if (inputState[SDL_SCANCODE_UP]) {
		gCamera.eyePosition += (screenUp * gCamera.rotation) * posStep;
	}
	if (inputState[SDL_SCANCODE_DOWN]) {
		gCamera.eyePosition -= (screenUp * gCamera.rotation) * posStep;
	}
	if (inputState[SDL_SCANCODE_RIGHT]) {
		gCamera.eyePosition += (screenRight * gCamera.rotation) * posStep;
	}
	if (inputState[SDL_SCANCODE_LEFT]) {
		gCamera.eyePosition -= (screenRight * gCamera.rotation) * posStep;
	}
}

void renderFrame()
{
	//Clear color buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Render quad
	if( !gRenderQuad )
		return;

	float window_width = SDL_GetWindowSurface(gWindow)->w;
	float window_height = SDL_GetWindowSurface(gWindow)->h;
	float aspect = window_width / window_height;

	glm::mat4 model = glm::mat4(1.0f);
	//glm::mat4 mvp = projection * view * model;

	gShaderProgram->use();

	gShaderProgram->setUniform("u_viewMatrix", glm::value_ptr(gCamera.viewMatrix()));
	gShaderProgram->setUniform("u_projectionMatrix", glm::value_ptr(gCamera.projectionMatrix(aspect)));
	gShaderProgram->setUniform("u_modelMatrix", glm::value_ptr(model));
	glm::mat4 normalmat = glm::transpose(glm::inverse(model));
	gShaderProgram->setUniform("u_normalMatrix", glm::value_ptr(normalmat));

	//gShaderProgram.setUniform("u_useCelShading", GL_FALSE);

	//glDrawArrays(GL_TRIANGLES, 0, 3);

	gTileMap->draw(*gShaderProgram);
	gModelTileMap->draw(*gShaderProgram);
	//gTetraMesh.draw(*gShaderProgram);

	//glUseProgram( 0 );
}

void closeProgram()
{
	
	//Deallocate program
	if (gShaderProgram->ID != NULL) {
		glDeleteProgram(gShaderProgram->ID);
		delete gShaderProgram;
	}

	//Destroy window
	if (gWindow != 0)	
		SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}


int main( int argc, char* args[] )
{
	setbuf(stdout, NULL);
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
		closeProgram();
		return 0;
	}

	//Event handler
	SDL_Event e;
	

    gGameIsRunning = true;
    while( gGameIsRunning ) {

		while( SDL_PollEvent( &e ) != 0 )
		{
			if( e.type == SDL_QUIT )
			{
				gGameIsRunning = false;
			}
			// else if( e.type == SDL_KEYDOWN || e.type == SDL_KEYUP )
			// {
			// 	int x = 0, y = 0;
			// 	SDL_GetMouseState( &x, &y );
			// 	handleKeys( e.key, x, y );
			// }
		} 
		update();

        renderFrame();
		SDL_GL_SwapWindow( gWindow );


		SDL_Delay(20);
	}
	
	closeProgram();

	std::string foo;
	std::cin >> foo;

	return 0;
}