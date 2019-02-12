
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
#include "rendercomponent.h"
#include "assert.h"
#include <vector>
#include "utils.h"


struct Tile {
    glm::vec4 color;
};

typedef std::vector<Tile> Palette;

// tile palette at index 0 is reserved. palette value at 0 must be set but is ignored for mesh generation.
class TileMap3d {
    public:
        Renderer::MeshID meshID = 0;
        bool meshOutdated = true;
        bool makeMeshCentered = true;
        bool showBoundaries = true;

        //TileMap3d() : xSize(0), ySize(0), zSize(0), content(0) {};
        TileMap3d(const Palette palette, int xSize, int ySize, int zSize);
        TileMap3d(const Palette palette, int xSize);

        // TileMap3d(const TileMap3d &other);

        void setPalette(const Palette palette);
        const Palette getPalette();
        
        int index(int x, int y, int z);
        unsigned int get(int x, int y, int z);
        unsigned int get(glm::ivec3 k);

        Tile getTile(int x, int y, int z);
        Tile getTile(glm::ivec3 k);

        void set(int x, int y, int z, unsigned int value);
        void set(glm::ivec3 k, unsigned int v);

        void updateMesh();

        glm::vec3 center();

        int getXSize() {return xSize;}
        int getYSize() {return ySize;};
        int getZSize() {return zSize;};


    private:
        int xSize;
        int ySize;
        int zSize;
        std::vector<int> content;
    public:
        Palette palette;
};




struct TileInfo {
    unsigned short index;
    unsigned short rot;
};


template <class T>
class TileMap3dT {
    public:
        float tile_size;
        std::vector<MeshRenderObject> meshes;

        //TileMap3d() : xSize(0), ySize(0), zSize(0), content(0) {};
        TileMap3dT(const std::vector<T> palette, int xSize, int ySize, int zSize);
        TileMap3dT(const std::vector<T> palette, int xSize);

        // TileMap3dT(const TileMap3dT &other);
        
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
        void setRot(int x, int y, int z, unsigned short rot);
        void setRot(glm::ivec3 k, unsigned short rot);

        std::vector<MeshRenderObject> getRenderables();

        glm::vec3 center();
        int getXSize() {return xSize;}
        int getYSize() {return ySize;}
        int getZSize() {return zSize;}

    private:
        int xSize, ySize, zSize;
        bool needMeshUpdate = true;
        std::vector<TileInfo> content;

    public: 
        std::vector<T> palette;
};




TileMap3d* createExampleTileMap(std::vector<Tile> palette, int spacing = 0);




// TileMap with non-fixed tile.

template <class T>
TileMap3dT<T>::TileMap3dT(const std::vector<T> palette, int xSize, int ySize, int zSize) : \
        xSize(xSize), 
        ySize(ySize), 
        zSize(zSize), palette (palette)
{
    content.resize(xSize * ySize * zSize);
}


template <class T>
TileMap3dT<T>::TileMap3dT(const std::vector<T> palette, int xSize) : \
        xSize(xSize), 
        ySize(xSize), 
        zSize(xSize),
        palette(palette)
{
    content.resize(xSize * ySize * zSize);
}

// template <class T>
// TileMap3dT<T>::TileMap3dT(const TileMap3dT<T> &other) : 
//         xSize(other.xSize), 
//         ySize(other.ySize), 
//         zSize(other.zSize), 
//         palette(other.palette),
//         content(content)
// {
//     content.resize(xSize * ySize * zSize);
// }


template <class T>
int TileMap3dT<T>::index(int x, int y, int z) {
    x = x % xSize;
    if (x < 0) x += xSize;
    y = y % ySize;
    if (y < 0) y += ySize;
    z = z % zSize;
    if (z < 0) z += zSize;
    return x * ySize * zSize + y * zSize + z;
}

template <class T>
TileInfo TileMap3dT<T>::getRaw(int x, int y, int z) {
    return content[index(x, y, z)];
}


template <class T>
TileInfo TileMap3dT<T>::getRaw(glm::ivec3 k) {
    return getRaw(k.x, k.y, k.z);
}

template <class T>
unsigned short TileMap3dT<T>::get(int x, int y, int z) {
    return content[index(x, y, z)].index;
}

template <class T>
unsigned short TileMap3dT<T>::get(glm::ivec3 k) {
    return get(k.x, k.y, k.z);
}


template <class T> 
unsigned short TileMap3dT<T>::getRot(int x, int y, int z) {
    return content[index(x, y, z)].rot;
}


template <class T> 
unsigned short TileMap3dT<T>::getRot(glm::ivec3 k) {
    return get(k.x, k.y, k.z);
}


template <class T>
T* TileMap3dT<T>::getTile(int x, int y, int z) {
    return &(palette[get(x, y, z)]);
}

template <class T>
T* TileMap3dT<T>::getTile(glm::ivec3 k) {
    return getTile(k.x, k.y, k.z);
}



template <class T>
void TileMap3dT<T>::setRaw(int x, int y, int z, TileInfo value) {
    needMeshUpdate = true;
    if (value.index >= palette.size()) {
        throw std::invalid_argument( "Tile index " + std::to_string(value.index) + " out of range: 0 - " + std::to_string(palette.size()) );
    }
    content[index(x, y, z)] = value;
}

template <class T>
void TileMap3dT<T>::setRaw(glm::ivec3 k, TileInfo v) {
    set(k.x, k.y, k.z, v);
}

template <class T>
void TileMap3dT<T>::set(int x, int y, int z, unsigned short v) {
    needMeshUpdate = true;
    if (v >= palette.size()) {
        throw std::invalid_argument( "Tile index " + std::to_string(v) + " out of range: 0 - " + std::to_string(palette.size()) );
    }
    content[index(x, y, z)].index = v;
}
template <class T>
void TileMap3dT<T>::set(glm::ivec3 k, unsigned short v) {
    this->set(k.x, k.y, k.z, v);
}

template <class T>
void TileMap3dT<T>::setRot(int x, int y, int z, unsigned short rot) {
    needMeshUpdate = true;
    if (rot >= 48) {
        throw std::invalid_argument( "Rotation " + std::to_string(rot) + " out of range.");
    }
    content[index(x, y, z)].rot = rot;
}
template <class T>
void TileMap3dT<T>::setRot(glm::ivec3 k, unsigned short rot) {
    this->setRot(k.x, k.y, k.z, rot);
}

template <class T>
std::vector<MeshRenderObject> TileMap3dT<T>::getRenderables() {
    if (needMeshUpdate) {
        std::cout << "HI";
        meshes = std::vector<MeshRenderObject>();
        for (int x = 0; x < xSize; x++) {
            for (int y = 0; y < ySize; y++) {
                for (int z = 0; z < zSize; z++) {
                    MeshRenderObject meshInstance;
                    if (get(x, y, z) == 0) continue;
                    T* tile = getTile(x, y, z);
                    auto rot = getRot(x, y, z);
                    auto rot_quat = glm::quat_cast(utils::i2cs[rot]);
                    for (int i = 0; i < 4; i++) {
                        std::cout << rot_quat[0] << ", " << rot_quat[1];
                    }

                    if (tile->meshID == 0) {
                        tile->updateMesh();
                    }
                    std::cout << tile->meshID;
                    meshInstance.meshID = tile->meshID;
                    meshInstance.transform = Transform(glm::vec3(x * tile_size, y * tile_size, z * tile_size), rot_quat);
                    meshes.emplace_back(meshInstance);
                }
            }
        }
    }
    needMeshUpdate = false;
    return meshes;
}

// template <class T>
// void TileMap3dT<T>::draw(ShaderProgram& shaderProgram, glm::mat4 transform/* = glm::mat4(1.0f)*/) {
//     for (int x = 0; x < xSize; x++) {
//         for (int y = 0; y < ySize; y++) {
//             for (int z = 0; z < zSize; z++) {
//                 glm::mat4 model = glm::mat4(1.0f);
//                 T* tile = get(x, y, z);
//                 // glm::mat<3, 3, int, glm::mediump> mat;

//                 glm::translate(model, glm::vec3(x, y, z));
//                 model = model * transform;
                
//                 shaderProgram.setUniform("u_modelMatrix", glm::value_ptr(model));
//                 glm::mat4 normalmat = glm::transpose(glm::inverse(model));
//                 shaderProgram.setUniform("u_normalMatrix", glm::value_ptr(normalmat));
//                 tile->draw(shaderProgram);
//             }
//         }
//     }
// }




#endif // TILEMAP3D_H