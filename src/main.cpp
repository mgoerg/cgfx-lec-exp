// main.cpp

#include <SDL2/SDL.h>
#include "GL/glew.h"
#include <SDL2/SDL_opengl.h>


#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>
#include <memory>


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

#include "game.h"
#include "renderer.h"
#include "mesh.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initialize renderable content
bool initScene();

//Input handler
// void handleKeys( unsigned char key, int x, int y );

//Per frame update
void update();

//Renders quad to the screen
void renderFrame();

//Frees media and shuts down SDL
void closeProgram();

//Shader loading utility
void printProgramLog( GLuint program );


SDL_Window* gWindow = nullptr;
SDL_GLContext gContext;
bool gRenderQuad = true;

std::string gVertexShaderFile = "data/shader/vertex.glsl";
std::string gFragmentShaderFile = "data/shader/fragment.glsl";
const char* gModelFile = "data/model/teapot.vox";
const char* gCastleTileMapFile = "data/model/mycastle.vox";


//Mesh gTetraMesh;
TileMap3d* gTileMap;
TileMap3d* gModelTileMap;
std::vector<TileMap3d*> gCastleTiles;

float gCameraA = 0;
float gCameraB = 0;
float gR = 20;
glm::vec3 gCameraPosition;
glm::vec3 gCameraTargetPosition = {0, 0, 0};

bool gGameIsRunning = true;
std::shared_ptr<Renderer::Renderer> gRenderer = nullptr;

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
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	//Create window
	gWindow = SDL_CreateWindow( "Title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
	if( gWindow == nullptr )
	{
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	//Create context
	gContext = SDL_GL_CreateContext( gWindow );
	if( gContext == nullptr )
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


	const GLubyte* renderer = glGetString( GL_RENDERER ); /* get renderer string */
	const GLubyte* version = glGetString( GL_VERSION );	 /* version as a string */
	printf( "Renderer: %s\n", renderer );
	printf( "OpenGL version supported %s\n", version );


	//Use Vsync
	if(SDL_GL_SetSwapInterval( 1 ) < 0)
	{
		std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError();
	}

	// int unibl;
	// glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &unibl);
	// std::cout << "Max uniform blocks " << unibl;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor( 0.f, 0.0f, 0.1f, 1.f );
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	return true;
}


bool initScene() 
{
	bool success;

	auto tileMaps = MV::makeTileMapsFromFile(gModelFile, true, success);
	gModelTileMap = tileMaps[0];
	if (!success) {
		return false;
	}


	gCastleTiles = MV::makeTileMapsFromFile(gCastleTileMapFile, true, success);
	if (!success) {
		return false;
	}

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
	gTileMap->updateMesh();




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
		int num_lights = 1;
		float _pad1;
		float _pad2;
		float _pad3;
		LightSource light1;
		LightSource light2;
		LightSource light3;
	} lights;


	LightSource ambient;
	ambient.flags |= 1;
	ambient.color[0] = 1.0;
	ambient.color[1] = 1.0;
	ambient.color[2] = 1.0;

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


	// GLuint uniformBlockIndex = glGetUniformBlockIndex( gRenderer.shaderProgram.ID, "LightBlock");

	// GLint blockSize;
	// glGetActiveUniformBlockiv(gRenderer.shaderProgram.ID, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

	// int blockbinding = 1;
	// glUniformBlockBinding(gRenderer.shaderProgram.ID, uniformBlockIndex, blockbinding );

	//GLuint ubo;
	//glGenBuffers(1, &ubo);
	//std::cout << blockSize;


	// glBindBuffer(GL_UNIFORM_BUFFER, gRenderer.getBuffer("LightBlock").buf);

	// glBufferData(GL_UNIFORM_BUFFER, 
	// 	sizeof(LightData), //blockSize, 
	// 	&lights, 
	// 	GL_STATIC_DRAW);
	
	//glBindBufferRange(GL_UNIFORM_BUFFER, blockbinding, ubo, 0, blockSize);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);


	// const int num = 25;
	// int foo[num];
	// GLuint indices[num];
	// for (int i = 0; i < num; i++) {
	// 	indices[i] = i;
	// }
 	// glGetActiveUniformsiv( gRenderer.shaderProgram.ID, num, indices, GL_UNIFORM_OFFSET, &foo[0]);
	// for (int i = 0; i < num; i++) {
	// 	std::cout << i << ": " << foo[i] << std::endl;
	// }

	return true;
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
	const Uint8* inputState = SDL_GetKeyboardState(nullptr);
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
	gRenderer->initFrame();

	float window_width = SDL_GetWindowSurface(gWindow)->w;
	float window_height = SDL_GetWindowSurface(gWindow)->h;
	float aspect = window_width / window_height;

	Renderer::MeshRenderable meshJob;


	for (int i = 1; i < 21; i++) {
		meshJob.orth = glm::translate(glm::mat4(1.0f), glm::vec3(i*10, 0,0));
		meshJob.meshID = gCastleTiles[i]->meshID;
		gRenderer->renderMesh(meshJob);
	}
	

	meshJob.meshID = gTileMap->meshID;
	for (int i=0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			for (int k = 0; k < 10; k++) {
				meshJob.orth = glm::translate(glm::mat4(1.0f), glm::vec3(i*10, j*10, k*10));
				gRenderer->renderMesh(meshJob);
			}
		}
	}

	meshJob.meshID = gModelTileMap->meshID;
	gRenderer->renderMesh(meshJob);

	gRenderer->renderFrame(gCamera, aspect);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "GL error: " << error << std::endl;
	}

	//glUseProgram( 0 );
}

void closeProgram()
{
	
	//Deallocate program

	//Destroy window
	if (gWindow != 0)	
		SDL_DestroyWindow( gWindow );
	gWindow = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}


int main( int argc, char* args[] )
{
	setbuf(stdout, nullptr);

	if(!init())
	{
		printf( "Failed to initialize!\n" );
		closeProgram();
		return 0;
	}

	bool success;
	gRenderer = std::make_shared<Renderer::Renderer>(success, gVertexShaderFile, gFragmentShaderFile);
	if (!success) {
		std::cout << "Could not initialize Renderer!" << std::endl;
		return false;
	}

	if (!initScene()) {
		std::cout << "Initializing scene failed!" << std::endl;
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