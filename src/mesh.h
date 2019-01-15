

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
    /*  Mesh Data  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    //std::vector<Texture> textures;
    unsigned int VAO;

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

    // initialize buffer objects/arrays
    void setup(ShaderProgram& shaderProgram);

    void draw(ShaderProgram& shaderProgram);


private:
    /*  Render data  */
    unsigned int VBO, EBO;

};



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



#endif // MESH_H

