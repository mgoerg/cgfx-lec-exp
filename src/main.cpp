// main.cpp

#include <SDL2/SDL.h>
#include "GL/glew.h"
#include <SDL2/SDL_opengl.h>


#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>
#include <memory>
#include <chrono>
#include <thread>
#include <random>
#include <functional>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>  // glm::value_ptr()

#include "mesh.h"
#include "tilemap3d.h"
#include "importMagicaVoxel.h"
#include "utils.h"
#include "camera.h"
#include "lightsource.h"
#include "transform.h"

#include "entt.hpp"
#include "entity/registry.hpp"

//#include "game.h"
#include "renderer.h"
#include "rendercomponent.h"
#include "misccomponents.h"
#include "entity.h"
#include "utils.h"

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

std::string g_vertexShaderFile = "data/shader/vertex.glsl";
std::string g_fragmentShaderFile = "data/shader/fragment.glsl";
std::string g_geometryShaderFile = "data/shader/geometry.glsl";

const char* g_teapotFile = "data/model/monu1.vox";
const char* gCastleTileMapFile = "data/model/mycastle.vox";
const char* g_pacmanTerrainFile = "data/model/pacman_walls.vox";


//Mesh gTetraMesh;
TileMap3d* g_axisTileMap;
TileMap3d* g_cubeTileMap;
TileMap3d* g_teaPotTileMap;
std::vector<TileMap3d*> gCastleTiles;
std::vector<TileMap3d*> g_pacmanTiles;

float gCameraA = 0;
float gCameraB = 0;
float gR = 20;


std::vector<Renderer::LightSource> g_lights;

bool g_gameIsRunning = true;
std::shared_ptr<Renderer::Renderer> gRenderer = nullptr;

Camera gCamera;

entt::registry g_world;


const char pacman_map_string[] = ""
"############################"
"#............##............#"
"#.####.#####.##.#####.####.#"
"#.####.#####.##.#####.####.#"
"#.####.#####.##.#####.####.#"
"#..........................#"
"#.####.##.########.##.####.#"
"#.####.##.########.##.####.#"
"#......##....##....##......#"
"######.##### ## #####.######"
"######.##          ##.######"
"######.## ##----## ##.######"
"######.## #      # ##.######"
"      .   #      #   .      "
"######.## ######## ##.######"
"######.##          ##.######"
"######.## ######## ##.######"
"######.## ######## ##.######"
"#............##............#"
"#.####.#####.##.#####.####.#"
"#.####.#####.##.#####.####.#"
"#...##................##...#"
"###.##.##.########.##.##.###"
"###.##.##.########.##.##.###"
"#......##....##....##......#"
"#.##########.##.##########.#"
"#.##########.##.##########.#"
"#..........................#"
"############################";
const int pacman_map_size = 28;
const int pacman_map_boundary = 5;
const int full_width = 2 * pacman_map_boundary + pacman_map_size;
const int full_height = 2 * pacman_map_boundary + pacman_map_size;
int pacman_map[full_width * full_height];

int get_tilemap_value (int x, int y) {
	x = utils::clampInt(x, 0, full_width);
	y = utils::clampInt(y, 0, full_height);
	return pacman_map[y + x * full_width];
}

typedef TileMap3dT<TileMap3d> TerrainTileMap;



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
	gWindow = SDL_CreateWindow( "Computer Graphics!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
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
	// if(SDL_GL_SetSwapInterval( 1 ) < 0)
	// {
	// 	std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError();
	// }

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

bool setupMap() {
	for (int i = 0; i < full_width; i++) {
		for (int j = 0; j < full_height; j++) {

			int x = utils::clampInt(i - pacman_map_boundary, 0, pacman_map_size);
			int y = utils::clampInt(j - pacman_map_boundary, 0, pacman_map_size);
			int value = pacman_map_string[x + y * pacman_map_size];

			int index = j + i * full_width;

			switch(value) {
				case '#':
					pacman_map[index] = 1;
					break;
				case '.': 
					pacman_map[index] = 2;
					break;
				case '-':
					pacman_map[index] = 3;
					break;
				case '*':
					pacman_map[index] = 4;
					break;
				case ' ':
				default:
					pacman_map[index] = 0;
					break;
			}
		}
	}

	bool success;
	std::vector<TileMap3d*> terrain_models = MV::makeTileMapsFromFile(g_pacmanTerrainFile, true, success);
	if (!success) {
		return false;
	}

	std::vector<TileMap3d> palette;
	palette.push_back(*(terrain_models[0])); // Unused
	palette.push_back(*(terrain_models[1])); // Straight
	palette.push_back(*(terrain_models[0])); // Corner
	palette.push_back(*(terrain_models[2])); // Full

	auto map_entity = g_world.create();
	auto& terrain = g_world.assign<TileMap3dT<TileMap3d>>(map_entity, palette, full_width, full_height, 2);

	terrain.tile_size = 20;
	for (int x = 0; x < full_width; x++) {
		for (int y = 0; y < full_height; y++) {
			int tile_val = get_tilemap_value(x, y);
			if (tile_val == 1) {
				// Set Terrain tile
				// Corners
				if (get_tilemap_value(x, y+1) != 1 && get_tilemap_value(x+1, y) != 1) {
					terrain.set(x, y, 1, 2);
				} else if (get_tilemap_value(x, y-1) != 1 && get_tilemap_value(x+1, y) != 1) {
					terrain.set(x, y, 1, 2);
					terrain.setRot(x, y, 1, utils::rotZ(1));
				} else if (get_tilemap_value(x, y-1) != 1 && get_tilemap_value(x-1, y) != 1) {
					terrain.set(x, y, 1, 2);
					terrain.setRot(x, y, 1, utils::rotZ(2));
				} else if (get_tilemap_value(x, y+1) != 1 && get_tilemap_value(x-1, y) != 1) {
					terrain.set(x, y, 1, 2);
					terrain.setRot(x, y, 1, utils::rotZ(3));
				}
				// Straight
				else if (get_tilemap_value(x+1, y) != 1) {
					terrain.set(x, y, 1, 1);
				}
				else if (get_tilemap_value(x, y-1) != 1) {
					terrain.set(x, y, 1, 1);
					terrain.setRot(x, y, 1, utils::rotZ(1));
				}
				else if (get_tilemap_value(x-1, y) != 1) {
					terrain.set(x, y, 1, 1);
					terrain.setRot(x, y, 1, utils::rotZ(2));
				}
				else if (get_tilemap_value(x, y+1) != 1) {
					terrain.set(x, y, 1, 1);
					terrain.setRot(x, y, 1, utils::rotZ(3));
				} else {
					terrain.set(x, y, 1, 3);
				}
			} else if (tile_val == 2) {
				// Create Collectible
				// Entity collectible = g_world.create();
				
			}
		}
	}

	auto& renderable = g_world.assign<RenderComponent>(map_entity);
	renderable.meshes = terrain.getRenderables();
	g_world.assign<Transform>(map_entity);

	return true;
}


bool initScene() 
{
	bool success;

	auto tileMaps = MV::makeTileMapsFromFile(g_teapotFile, true, success);
	g_teaPotTileMap = tileMaps[0];
	if (!success) {
		return false;
	}


	gCastleTiles = MV::makeTileMapsFromFile(gCastleTileMapFile, true, success);
	if (!success) {
		return false;
	}

	auto teapot = g_world.create();
	auto& renderComponent = g_world.assign<RenderComponent>(teapot);
	MeshRenderObject mesh;
	mesh.meshID = g_teaPotTileMap->meshID;
	std::cout << g_teaPotTileMap->meshID;
	renderComponent.meshes.emplace_back(mesh);
	auto& tr = g_world.assign<Transform>(teapot);

	//setupMap();

	gCamera.fov = glm::radians(45.f);
	gCamera.near = 0.1f;
	gCamera.far = 10000;
	gCamera.eyePosition = {0, 0, 0};

	Palette palette = {
		{glm::vec4()}, 
		{glm::vec4(0.5, 0.1, 0.1, 1.0)},
		{glm::vec4(0.1, 0.5, 0.1, 1.0)},
		{glm::vec4(0.1, 0.1, 0.5, 1.0)}
	};

	g_axisTileMap = new TileMap3d(palette, 4);
	g_axisTileMap->set(1, 0, 0, 1);
	g_axisTileMap->set(2, 0, 0, 1);
	g_axisTileMap->set(0, 1, 0, 2);
	g_axisTileMap->set(0, 2, 0, 2);
	g_axisTileMap->set(0, 0, 1, 3);
	g_axisTileMap->set(0, 0, 2, 3);
	g_axisTileMap->updateMesh();

	Palette palette2 = {
		{glm::vec4()},
		{glm::vec4(1.0, 1.0, 1.0, 1.0)}
	};
	g_cubeTileMap = new TileMap3d(palette2, 1);
	g_cubeTileMap->set(0, 0, 0, 1);
	g_cubeTileMap->updateMesh();


	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-1.0, 1.0);
	auto randfloat = std::bind ( distribution, generator );

	const int count = 5;
	for (int i=0; i < count; i++) {
		for (int j = 0; j < count; j++) {
			for (int k = 0; k < count; k++) {
				auto ent = g_world.create();
				auto& renderComponent = g_world.assign<RenderComponent>(ent);
				MeshRenderObject mesh;
				mesh.meshID = g_axisTileMap->meshID;
				renderComponent.meshes.push_back(mesh);

				if ((i + j + k) % 10 > 0) {
					auto& patrolComponent = g_world.assign<SimplePatrolBehavior>(ent);

					glm::vec3 offsets[] = {
						glm::vec3{0, 0, 0},
						glm::vec3{0, 0, 0},
						glm::vec3{0, 0, 0}
					};

					Transform pt;
					for (int u = 0; u < 3; u++) {
						pt.position = glm::vec3(i*10, j*10, k*10 + 3000) + offsets[u];
						patrolComponent.routePoints.push_back(pt);
						glm::quat quat {
							randfloat(), 
							randfloat(), 
							randfloat(), 
							randfloat(), 
						};
						pt.rotation = glm::normalize(quat);
					}
				}

				g_world.assign<Transform>(ent);
			}
		}
	}

	// Entity impact = g_world.create();
	// g_world.assign<Transform>(impact);
	// auto& patrol = g_world.assign<SimplePatrolBehavior>(impact);
	// Transform pt;
	// pt.position = {0, 0, 10};
	// patrol.routePoints.push_back(pt);
	// pt.position = {100, 100, 10};
	// patrol.routePoints.push_back(pt);
	// patrol.velocity = 1.0;
	// auto& render = g_world.assign<RenderComponent>(impact);
	// Renderer::ImpactSourceData data;
	// data.intensity = 3;
	// data.width = 10;
	// data.distance = 30;
	// data.direction[0] = 0;
	// data.direction[2] = 1;
	// Renderer::ImpactSource imp(data);
	// render.impacts.push_back(imp);
	// MeshRenderObject mesh_;
	// mesh_.meshID = g_axisTileMap->meshID;
	// render.meshes.push_back(mesh_);

	glm::vec3 offsets[] = {
		// 1
		{20, 0, 0},
		{20, -40, 0},
		{10, -40, 30},
		// 2
		{ 30, -30, -30},
		{ 30, -30, 30}, 
		{ 30, -20, 5},
		// 3
		{10, -40, -30},
		{30, -5, -5},
		{10, -40, 30}
	};

	glm::vec3 colors[] = {
		glm::vec3(1.0, 1.0, 0.2),
		glm::vec3(0.1333, 0.9529, 0.0588),
		glm::vec3(0.8706, 0.6157, 0.3255),
	};
	for (int i = 0; i < 3; i++) {
		Entity floatyLight = g_world.create();
		auto& ftlgtrender = g_world.assign<RenderComponent>(floatyLight);
		auto pointLight = Renderer::pointLightSource(glm::vec3(), colors[i], 1.0, 1.0, 0.1, 0.0);
		ftlgtrender.lights.emplace_back(pointLight);
		MeshRenderObject cubeMesh;
		cubeMesh.meshID = g_cubeTileMap->meshID;
		cubeMesh.enableLighting = false;
		ftlgtrender.meshes.emplace_back(cubeMesh);
		g_world.assign<Transform>(floatyLight);
		auto& ftlgtpatrol = g_world.assign<SimplePatrolBehavior>(floatyLight);
		Transform pt2;
		// ftlgtpatrol.patrolType = PATROL_ALTERNATE;
		for (int k = 0; k < 3; k++) {
			pt2.position = offsets[i*3 + k];
			ftlgtpatrol.routePoints.push_back(pt2);
		}
		ftlgtpatrol.velocity = 10.0;
	}

	g_lights.emplace_back(Renderer::ambientLightSource(glm::vec3(0.3, 0.3, 0.3)));
	//g_lights.emplace_back(Renderer::directionLightSource(glm::vec3(1.0, 2.0, 3.0), glm::vec3(1.0, 1.0, 1.0), 0.3));

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
// 			g_gameIsRunning = false;
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

void update(float dt)
{
	const Uint8* inputState = SDL_GetKeyboardState(nullptr);
	if (inputState[SDL_SCANCODE_Q] || inputState[SDL_SCANCODE_ESCAPE]) {
		g_gameIsRunning = false;
	}

	float angleStep = 2.5f * glm::radians(1.0);
	float posStep = 3;
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

    g_world.view<Transform, SimplePatrolBehavior>().each([dt](const auto, auto &transform, auto &patrol) {
		updatePatrolBehavior(transform, patrol, dt);
    });

	g_world.view<RenderComponent>().each([dt] (const auto, auto& render) {
		if (render.impacts.size() > 0) {
			for (auto& impact : render.impacts) {
				impact.setDistance(impact.getDistance() + dt * 80.0);
				if ( impact.getDistance() > 200) {
					impact.setDistance(0);
				}
			}
		}
	});
}



void renderFrame()
{
	auto start = std::chrono::high_resolution_clock::now();

	gRenderer->initFrame();

	float window_width = SDL_GetWindowSurface(gWindow)->w;
	float window_height = SDL_GetWindowSurface(gWindow)->h;
	float aspect = window_width / window_height;

	Renderer::MeshInstance meshJob;

	auto init = std::chrono::high_resolution_clock::now();
	// std::cout << "Render Init: " << (init - start).count() / 1000 << std::endl;

	// Render Calls for light sources
	for(auto& light: g_lights) {
		gRenderer->renderLightSource(light.getDataStruct());
	}

	auto lights = std::chrono::high_resolution_clock::now();
	// // std::cout << "Render Lights: " << (lights - start).count() / 1000 << std::endl;

	auto renderTilemapView = g_world.view<RenderComponent, TerrainTileMap>();
	for (auto entity : renderTilemapView) {
		auto& tilemap = renderTilemapView.template get< TerrainTileMap >(entity);
		auto& renderable = renderTilemapView.template get< RenderComponent >(entity);
		renderable.meshes = tilemap.getRenderables();
	}

	// Render Calls for meshes

	auto view = g_world.view<Transform, RenderComponent>();
    for(auto entity : view) {
		auto& rc = view.get<RenderComponent>(entity);
		auto& tr = view.get<Transform>(entity);

		for (auto mesh : rc.meshes) {
			if (!mesh.enabled) continue;
			meshJob.orth = (mesh.transform * tr).modelMatrixIsometric();

			meshJob.meshID = mesh.meshID;
			meshJob.enabled = mesh.enabled;
			meshJob.enableLighting = mesh.enableLighting;
			meshJob.enableImpact = mesh.enableImpact;
			gRenderer->renderMeshInstance(meshJob);
		}

		for (auto lightRenderable : rc.lights) {
			if (!lightRenderable.isValid() || !lightRenderable.hasFlag(Renderer::LIGHT_ENABLED_BIT)) 
				continue;

			auto lightStruct = lightRenderable.getDataStruct();
			glm::vec4 pos = ((lightRenderable.getTransform() * tr).modelMatrixIsometric() * glm::vec4(lightStruct.position[0], lightStruct.position[1], lightStruct.position[2], 1.0));
			for (int i = 0; i < 3; i++) {
				lightStruct.position[i] = pos[i] / pos[3];
			}
			gRenderer->renderLightSource(lightStruct);
		}

		for (auto impactRenderable : rc.impacts) {
			if (!impactRenderable.isValid() || !impactRenderable.hasFlag(Renderer::LIGHT_ENABLED_BIT)) 
				continue;
			
			auto impactStruct = impactRenderable.getDataStruct();
			glm::vec4 pos = ((impactRenderable.getTransform() * tr).modelMatrixIsometric() * glm::vec4(impactStruct.position[0], impactStruct.position[1], impactStruct.position[2], 1.0));
			for (int i = 0; i < 3; i++)
			{
				impactStruct.position[i] = pos[i] / pos[3];
			}
			gRenderer->renderImpactSource(impactStruct);
		}
	}

	int i = 0;
	for (auto tilemap : g_pacmanTiles) {
		i++;
		meshJob.orth = glm::translate(glm::mat4(1.0f), glm::vec3(i*10, 0,0));
		meshJob.meshID = tilemap->meshID;
		gRenderer->renderMeshInstance(meshJob);
	}
	
	auto all_meshes = std::chrono::high_resolution_clock::now();
	// std::cout << "Render all meshes: " << (all_meshes - start).count() / 1000 << std::endl;

	gRenderer->renderFrame(gCamera, aspect);

	auto finish = std::chrono::high_resolution_clock::now();
	// std::cout << "Render finish: " << (finish - start).count() / 1000 << std::endl;

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "GL error: " << error << std::endl;
	}
	SDL_GL_SwapWindow( gWindow );

	auto swap = std::chrono::high_resolution_clock::now();
	// std::cout << "Render Swap: " << (swap - start).count() / 1000 << std::endl;


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
	gRenderer = std::make_shared<Renderer::Renderer>(success, g_vertexShaderFile, g_fragmentShaderFile, g_geometryShaderFile);
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

	const int fps_target = 60;
	const float frame_time_float = 1.0 / 60;
	const auto frame_time = std::chrono::microseconds(1000000 / fps_target);


    g_gameIsRunning = true;
    while( g_gameIsRunning ) {

		auto start = std::chrono::high_resolution_clock::now();
		while( SDL_PollEvent( &e ) != 0 )
		{
			if( e.type == SDL_QUIT )
			{
				g_gameIsRunning = false;
			}
			// else if( e.type == SDL_KEYDOWN || e.type == SDL_KEYUP )
			// {
			// 	int x = 0, y = 0;
			// 	SDL_GetMouseState( &x, &y );
			// 	handleKeys( e.key, x, y );
			// }
		} 
		update(frame_time_float);
		auto updateTime = std::chrono::high_resolution_clock::now();
		// std::cout << "Update: " << (updateTime - start).count() / 1000 << std::endl;

        renderFrame();

		auto render = std::chrono::high_resolution_clock::now();
		// std::cout << "Render: " << (render - start).count() / 1000 << std::endl;

		auto difference = std::chrono::high_resolution_clock::now() - start;
		// std::cout << "Wait SDL: " << difference.count() / 1000 << std::endl;

		auto to_wait = frame_time - difference;
		if (to_wait.count() > 0) {
			std::this_thread::sleep_for(frame_time - difference);
		} else {
			// std::cout << "Frame computation time exceeded: " << (to_wait).count() / 1000 << std::endl;
		}
	}
	
	closeProgram();

	std::string foo;
	std::cin >> foo;

	return 0;
}