

#include "tilemap3d.h"

#include <vector>



TileMap3d::TileMap3d(const std::vector<Tile> palette, int xSize, int ySize, int zSize) : \
        xSize(xSize), \
        ySize(ySize), \
        zSize(zSize)
{
    content = new unsigned int[xSize * ySize * zSize];
    std::fill_n(content, xSize * ySize * zSize, 0); 
    this->palette = palette;
}


TileMap3d::TileMap3d(const std::vector<Tile> palette, int xSize) : \
        xSize(xSize), \
        ySize(xSize), \
        zSize(xSize)
{
    content = new unsigned int[xSize * ySize * zSize];
    std::fill_n(content, xSize * ySize * zSize, 0); 
    this->palette = palette;
}

TileMap3d::TileMap3d(const TileMap3d &other) : 
        xSize(other.xSize), 
        ySize(other.ySize), 
        zSize(other.zSize), 
        palette(other.palette), 
        showBoundaries(other.showBoundaries)
    {
    content = new unsigned int[xSize * ySize * zSize];
    memcpy(content, other.content, sizeof(unsigned int) * xSize * ySize * zSize);
}


TileMap3d::~TileMap3d() {
    delete[] content;
}

int TileMap3d::index(int x, int y, int z) {
    x = x % xSize;
    if (x < 0) x += xSize;
    y = y % ySize;
    if (y < 0) y += ySize;
    z = z % zSize;
    if (z < 0) z += zSize;
    return x * ySize * zSize + y * zSize + z;
}
unsigned int TileMap3d::get(int x, int y, int z) {
    return content[index(x, y, z)];
}
unsigned int TileMap3d::get(glm::ivec3 k) {
    return get(k.x, k.y, k.z);
}

Tile TileMap3d::getTile(int x, int y, int z) {
    return palette[get(x, y, z) - 1];
}

Tile TileMap3d::getTile(glm::ivec3 k) {
    return getTile(k.x, k.y, k.z);
}

void TileMap3d::set(int x, int y, int z, unsigned int value) {
    if (value >= palette.size()) {
        throw std::invalid_argument( "Tile index " + std::to_string(value) + " out of range: 0 - " + std::to_string(palette.size()) );
    }
    content[index(x, y, z)] = value;
}

void TileMap3d::set(glm::ivec3 k, unsigned int v) {
    set(k.x, k.y, k.z, v);
}


void TileMap3d::makeMesh()
{
    std::vector<Vertex> vertices;
    std::vector <unsigned int> indices;
    unsigned int index = 0;

    //    v6----- v5
	//   /|      /|
	//  v1------v0|
	//  | |     | |
	//  | v7----|-v4
	//  |/      |/
	//  v2------v3


    for (int x = 0; x < xSize; x++) {
        for (int y = 0; y < ySize; y++) {
            for (int z = 0; z < zSize; z++) {
                int tileState = get(x, y, z);
                if (tileState != 0) {

                    glm::vec4 color = palette[tileState].color;
                    glm::vec3 normal, tangent, bitangent;
                    glm::vec3 center = {x + 0.5, y + 0.5, z + 0.5};

                    auto add_face = [&](glm::vec3 normal, glm::vec3 tangent, glm::vec3 bitangent) { 
                        Vertex v0 = {
                            center + 0.5f * normal + 0.5f * tangent + 0.5f * bitangent, normal, color
                        };
                        Vertex v1 = {
                            center + 0.5f * normal - 0.5f * tangent + 0.5f * bitangent, normal, color
                        };
                        Vertex v2 = {
                            center + 0.5f * normal - 0.5f * tangent - 0.5f * bitangent, normal, color
                        };
                        Vertex v3 = {
                            center + 0.5f * normal + 0.5f * tangent - 0.5f * bitangent, normal, color
                        };

                        vertices.push_back(v0);
                        vertices.push_back(v1);
                        vertices.push_back(v2);
                        vertices.push_back(v3);

                        //if (glm::dot(glm::cross(v0.position - v1.position, v0.position - v2.position), normal) >= 0.0) {
                        indices.push_back(index);
                        indices.push_back(index + 1);
                        indices.push_back(index + 2);

                        indices.push_back(index);
                        indices.push_back(index + 2);
                        indices.push_back(index + 3);
                        /*} else {
                            std::cout << "B" << std::endl;
                            indices.push_back(index);
                            indices.push_back(index + 2);
                            indices.push_back(index + 1);

                            indices.push_back(index);
                            indices.push_back(index + 3);
                            indices.push_back(index + 2);
                        }*/

                        index += 4;
                    };

                    if ((x + 1 < xSize && get(x+1, y, z) == 0) || (x + 1 == xSize && showBoundaries)) {
                        normal = {1, 0, 0};
                        tangent = {0, 1, 0};
                        bitangent = {0, 0, 1};
                        add_face(normal, tangent, bitangent);
                    }

                    if ((x - 1 >= 0 && get(x-1, y, z) == 0) || (x == 0 && showBoundaries)) {
                        normal = {-1, 0, 0};
                        tangent = {0, -1, 0};
                        bitangent = {0, 0, 1};
                        add_face(normal, tangent, bitangent);
                    }
                    if ((y + 1 < ySize && get(x, y+1, z) == 0) || (y + 1 == ySize && showBoundaries)) {
                        normal = {0, 1, 0};
                        tangent = {0, 0, 1};
                        bitangent = {1, 0, 0};
                        add_face(normal, tangent, bitangent);
                    }
                    if ((y - 1 >= 0 && get(x, y-1, z) == 0) || (y == 0 && showBoundaries)) {
                        normal = {0, -1, 0};
                        tangent = {0, 0, -1};
                        bitangent = {1, 0, 0};
                        add_face(normal, tangent, bitangent);
                    }
                    if ((z + 1 < zSize && get(x, y, z+1) == 0) || (z + 1 == zSize && showBoundaries)) {
                        normal = {0, 0, 1};
                        tangent = {1, 0, 0};
                        bitangent = {0, 1, 0};
                        add_face(normal, tangent, bitangent);
                    }
                    if ((z - 1 >= 0 && get(x, y, z-1) == 0) || (z == 0 && showBoundaries)) {
                        normal = {0, 0, -1};
                        tangent = {-1, 0, 0};
                        bitangent = {0, 1, 0};
                        add_face(normal, tangent, bitangent);
                    }
                }
            }
        }
    }

    mesh = new Mesh(vertices, indices);
}


void TileMap3d::draw(ShaderProgram& shaderProgram) { 
    if (mesh->needUpdate)
        makeMesh();
    mesh->draw(shaderProgram);
}


glm::vec3 TileMap3d::center() {
    return glm::vec3(xSize, ySize, zSize) * 0.5f;
}


TileMap3d* createExampleTileMap(std::vector<Tile> palette, int spacing) {
    int s = spacing + 1;
    TileMap3d* ret = new TileMap3d(palette, palette.size() * (s), 1, 1);
    for (int i = 0; i < ret->xSize; i++) {
        ret->set(i * s, 0, 0, i);
    }
    return ret;
}







// TileMap with non-fixed tile.




template <typename T>
TileMap3dT<T>::TileMap3dT(const std::vector<T> palette, int xSize, int ySize, int zSize) : \
        xSize(xSize), \
        ySize(ySize), \
        zSize(zSize)
{
    content = new TileInfo[xSize * ySize * zSize];
    std::fill_n(content, xSize * ySize * zSize, 0); 
    this->palette = palette;
}


template <typename T>
TileMap3dT<T>::TileMap3dT(const std::vector<T> palette, int xSize) : \
        xSize(xSize), \
        ySize(xSize), \
        zSize(xSize)
{
    content = new TileInfo[xSize * ySize * zSize];
    std::fill_n(content, xSize * ySize * zSize, 0); 
    this->palette = palette;
}

template <typename T>
TileMap3dT<T>::~TileMap3dT() {
    delete[] content;
}

template <typename T>
int TileMap3dT<T>::index(int x, int y, int z) {
    x = x % xSize;
    if (x < 0) x += xSize;
    y = y % ySize;
    if (y < 0) y += ySize;
    z = z % zSize;
    if (z < 0) z += zSize;
    return x * ySize * zSize + y * zSize + z;
}

template <typename T>
TileInfo TileMap3dT<T>::getRaw(int x, int y, int z) {
    return content[index(x, y, z)];
}


template <typename T>
TileInfo TileMap3dT<T>::getRaw(glm::ivec3 k) {
    return getRaw(k.x, k.y, k.z);
}

template <typename T>
unsigned short TileMap3dT<T>::get(int x, int y, int z) {
    return content[index(x, y, z)].index;
}

template <typename T>
unsigned short TileMap3dT<T>::get(glm::ivec3 k) {
    return get(k.x, k.y, k.z);
}


template <typename T> 
unsigned short TileMap3dT<T>::getRot(int x, int y, int z) {
    return content[index(x, y, z)].rot;
}


template <typename T> 
unsigned short TileMap3dT<T>::getRot(glm::ivec3 k) {
    return get(k.x, k.y, k.z);
}


template <typename T>
T* TileMap3dT<T>::getTile(int x, int y, int z) {
    return &palette[get(x, y, z) - 1];
}

template <typename T>
T* TileMap3dT<T>::getTile(glm::ivec3 k) {
    return getTile(k.x, k.y, k.z);
}



template <typename T>
void TileMap3dT<T>::setRaw(int x, int y, int z, TileInfo value) {
    if (value.index >= palette.size()) {
        throw std::invalid_argument( "Tile index " + std::to_string(value.index) + " out of range: 0 - " + std::to_string(palette.size()) );
    }
    content[index(x, y, z)] = value;
}

template <typename T>
void TileMap3dT<T>::setRaw(glm::ivec3 k, TileInfo v) {
    set(k.x, k.y, k.z, v);
}

template <typename T>
void TileMap3dT<T>::set(int x, int y, int z, unsigned short v) {
    if (v >= palette.size()) {
        throw std::invalid_argument( "Tile index " + std::to_string(v) + " out of range: 0 - " + std::to_string(palette.size()) );
    }
    content[index(x, y, z)].index = v;
}
template <typename T>
void TileMap3dT<T>::set(glm::ivec3 k, unsigned short v) {
    this->set(k.x, k.y, k.z, v);
}

template <typename T>
void TileMap3dT<T>::draw(ShaderProgram& shaderProgram, glm::mat4 transform/* = glm::mat4(1.0f)*/) {
    for (int x = 0; x < xSize; x++) {
        for (int y = 0; y < ySize; y++) {
            for (int z = 0; z < zSize; z++) {
                glm::mat4 model = glm::mat4(1.0f);
                T* tile = get(x, y, z);
                // glm::mat<3, 3, int, glm::mediump> mat;

                glm::translate(model, glm::vec3(x, y, z));
                model = model * transform;
                
                shaderProgram.setUniform("u_modelMatrix", glm::value_ptr(model));
                glm::mat4 normalmat = glm::transpose(glm::inverse(model));
                shaderProgram.setUniform("u_normalMatrix", glm::value_ptr(normalmat));
                tile->draw(shaderProgram);
            }
        }
    }
}