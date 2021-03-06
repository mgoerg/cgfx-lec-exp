

#include "tilemap3d.h"



TileMap3d::TileMap3d(const std::vector<Tile> palette, int xSize, int ySize, int zSize) : \
        xSize(xSize), 
        ySize(ySize), 
        zSize(zSize), 
        palette(palette)
{
    content.resize(xSize * ySize * zSize);
}


TileMap3d::TileMap3d(const std::vector<Tile> palette, int xSize) : \
        xSize(xSize), 
        ySize(xSize), 
        zSize(xSize), 
        palette(palette)
{
    content.resize(xSize * ySize * zSize);
    this->palette = palette;
}

void TileMap3d::setPalette(const Palette palette) {
    this->palette = palette;
}

const Palette TileMap3d::getPalette() {
    return this->palette;
}

int TileMap3d::index(int x, int y, int z) {
    x = x % xSize;
    if (x < 0) x += xSize;
    y = y % ySize;
    if (y < 0) y += ySize;
    z = z % zSize;
    if (z < 0) z += zSize;
    assert(x * ySize * zSize + y * zSize + z < xSize * ySize * zSize);
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
    meshOutdated = true;
    if (value >= palette.size()) {
        throw std::invalid_argument( "Tile index " + std::to_string(value) + " out of range: 0 - " + std::to_string(palette.size()) );
    }
    content[index(x, y, z)] = value;
}

void TileMap3d::set(glm::ivec3 k, unsigned int v) {
    set(k.x, k.y, k.z, v);
}


void TileMap3d::updateMesh()
{
    if (!meshOutdated) {
        return;
    }
    std::vector<Renderer::Vertex> vertices;
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
                        Renderer::Vertex v0 = {
                            center + 0.5f * normal + 0.5f * tangent + 0.5f * bitangent, normal, color
                        };
                        Renderer::Vertex v1 = {
                            center + 0.5f * normal - 0.5f * tangent + 0.5f * bitangent, normal, color
                        };
                        Renderer::Vertex v2 = {
                            center + 0.5f * normal - 0.5f * tangent - 0.5f * bitangent, normal, color
                        };
                        Renderer::Vertex v3 = {
                            center + 0.5f * normal + 0.5f * tangent - 0.5f * bitangent, normal, color
                        };

                        if (makeMeshCentered) {
                            v0.position -= this->center();
                            v1.position -= this->center();
                            v2.position -= this->center();
                            v3.position -= this->center();
                        }

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

    // TODO
    std::cout << "Mesh created with " << vertices.size() << " vertices, " << indices.size() << " indices." << std::endl;


    if (meshID == 0) {
        meshID = Renderer::newMesh(vertices, indices);
    } else {
        Renderer::updateMesh(meshID, vertices, indices);
    }

    meshOutdated = false;

}


// void TileMap3d::draw(Renderer::Renderer renderer) { 
//     if (meshOutdated)
//         updateMesh();
    
//     // gRenderer.draw()
    
//     // mesh->draw(shaderProgram);
// }


glm::vec3 TileMap3d::center() {
    return glm::vec3(xSize, ySize, zSize) * 0.5f;
}


TileMap3d* createPaletteTileMap(std::vector<Tile> palette, int spacing, int width=-1) {
    int s = spacing + 1;
    if (width > 0) {
        TileMap3d* ret = new TileMap3d(palette, width, (palette.size() * s) / width + 1, 1);
        for (unsigned int i = 0; i < palette.size(); i++) {
            ret->set( (i * s) % width, (i*s) / width, 0, i );
        }
        return ret;
    } else {
        TileMap3d* ret = new TileMap3d(palette, palette.size() * (s), 1, 1);
        for (unsigned int i = 0; i < palette.size(); i++) {
            ret->set(i * s, 0, 0, i);
        }
        return ret;
    }
}

