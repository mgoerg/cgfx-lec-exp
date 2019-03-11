

#ifndef MESH_H
#define MESH_H

// #include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/type_ptr.hpp>  // glm::value_ptr()
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>

namespace Renderer {

class Mesh;

typedef unsigned int MeshID;

extern std::map<MeshID, std::shared_ptr<Mesh>> meshes;



struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
    //glm::vec2 TexCoords;
    //glm::vec3 Tangent;
    //glm::vec3 Bitangent;
};

/*struct Texture {
    unsigned int id;
    string type;
    string path;
};*/


class Mesh {
public:
    unsigned int id = -1;

    bool needUpdate = true;


    Mesh() :VAO((unsigned int)-1) {}
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices/*, std::vector<Texture> textures*/) : VAO(-1)
    {
        this->vertices = vertices;
        this->indices = indices;
        //this->textures = textures;
    }
    ~Mesh() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

    unsigned int indexCount() { return this->indices.size(); }
    unsigned int vertexCount() { return this->vertices.size(); }

    // initialize buffer objects/arrays. Needs to be called before prepareDraw whenever vertices or indices were changed.
    void setup(ShaderProgram& shaderProgram);

    // bind gl buffers needed to draw the mesh. Needs to be called before draw if other buffers are bound.
    void prepareDraw(ShaderProgram& shaderProgram);

    // actual draw call.
    void draw();

    private: 
        /*  Mesh Data  */
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        //std::vector<Texture> textures;
        unsigned int VAO;
        unsigned int VBO, EBO;

};


// Mesh ids start at 1. ID 0 is reserved.
MeshID newMesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices);
void updateMesh(MeshID id, std::vector<Vertex> &vertices, std::vector<unsigned int> &indices);
void deleteMesh(MeshID meshID);
std::shared_ptr<Mesh> getMesh(unsigned int id);



Mesh createMesh(std::vector<glm::vec3> &vertexPoints, \
    std::vector<std::vector<unsigned int>> &indices, \
    glm::vec4 color);



	// Tetrahedron
	//  v0-----------v1
	//    \        / /
	//     \     / /
	//       \ / /
	//       v2-v3 
	
	// float s2 = 1/glm::sqrt(2);
	// glm::vec4 color = {0.8, 1, 0, 1};
	// std::vector<glm::vec3> vertexPoints = {
	// 	{ 1,  s2, 0},
	// 	{-1,  s2, 0},
	// 	{ 0, -s2, -1},
	// 	{ 0, -s2,  1}
	// };
	// std::vector<std::vector<unsigned int>> indices = {
	// 	 { 0,  2,  1 },
	// 	 { 0,  1,  3 },
	// 	 { 0,  3,  2 },
	// 	 { 1,  2,  3 }	
	// };
	// gTetraMesh = createMesh(vertexPoints, indices, color);



} // namespace Renderer

#endif // MESH_H

