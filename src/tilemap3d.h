
#ifndef TILEMAP3D_H
#define TILEMAP3D_H


#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>  // glm::value_ptr()

#include <stdexcept>
#include <memory>

#include "shader.h"
#include "mesh.h"


struct Tile {
    glm::vec4 color;
};

typedef std::vector<Tile> Palette;

// tile palette at index 0 is reserved. palette value at 0 must be set but is ignored for mesh generation.
class TileMap3d {
    public:
        const int xSize, ySize, zSize;
        Palette palette;
        Renderer::MeshID meshID = 0;
        bool meshOutdated = true;
        bool showBoundaries = true;

        //TileMap3d() : xSize(0), ySize(0), zSize(0), content(0) {};
        TileMap3d(const Palette palette, int xSize, int ySize, int zSize);
        TileMap3d(const Palette palette, int xSize);

        TileMap3d(const TileMap3d &other);

        ~TileMap3d();

        int index(int x, int y, int z);
        unsigned int get(int x, int y, int z);
        unsigned int get(glm::ivec3 k);

        Tile getTile(int x, int y, int z);
        Tile getTile(glm::ivec3 k);

        void set(int x, int y, int z, unsigned int value);
        void set(glm::ivec3 k, unsigned int v);

        void updateMesh();
        //void draw(ShaderProgram& shaderProgram);

        glm::vec3 center();

    private:
        unsigned int* content;
};





struct TileInfo {
    unsigned short index;
    unsigned short rot;
};


template <typename T>
class TileMap3dT {
    public:
        const int xSize, ySize, zSize;
        std::vector<T> palette;
        bool showBoundaries = true;

        //TileMap3d() : xSize(0), ySize(0), zSize(0), content(0) {};
        TileMap3dT(const std::vector<T> palette, int xSize, int ySize, int zSize);
        TileMap3dT(const std::vector<T> palette, int xSize);

        ~TileMap3dT();
        
        int index(int x, int y, int z);
        unsigned short get(int x, int y, int z);
        unsigned short get(glm::ivec3 k);
        unsigned short getRot(int x, int y, int z);
        unsigned short getRot(glm::ivec3 k);
        TileInfo getRaw(int x, int y, int z);
        TileInfo getRaw(glm::ivec3 k);

        T* getTile(int x, int y, int z);
        T* getTile(glm::ivec3 k);

        void setRaw(int x, int y, int z, TileInfo value);
        void setRaw(glm::ivec3 k, TileInfo v);
        void set(int x, int y, int z, unsigned short value);
        void set(glm::ivec3 k, unsigned short v);

        void draw(ShaderProgram& shaderProgram, glm::mat4 transform = glm::mat4(1.0f));

        glm::vec3 center();

    private:
        TileInfo* content;
};




TileMap3d* createExampleTileMap(std::vector<Tile> palette, int spacing = 0);


#endif // TILEMAP3D_H