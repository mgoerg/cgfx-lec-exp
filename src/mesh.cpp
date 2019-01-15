
#include "mesh.h"



void Mesh::setup(ShaderProgram&shaderProgram)
{
    // create buffers/arrays
    if (VAO == (unsigned int)-1) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }


    glBindVertexArray(VAO);
    // load data into buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    GLuint positionAttribute = shaderProgram.attributeID("a_vertexPosition");
    glEnableVertexAttribArray(positionAttribute);	
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    GLuint normalAttribute = shaderProgram.attributeID("a_vertexNormal");
    glEnableVertexAttribArray(normalAttribute);	
    glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // vertex color
    GLuint colorAttribute = shaderProgram.attributeID("a_vertexColor");
    glEnableVertexAttribArray(colorAttribute);	
    glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    // vertex texture coords
    //glEnableVertexAttribArray(2);	
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    //glEnableVertexAttribArray(4);
    //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    glBindVertexArray(0);

    needUpdate = false;
}



void Mesh::draw(ShaderProgram& shader)
{
    if (needUpdate) {
        this->setup(shader);
    }
    // bind appropriate textures
    // unsigned int diffuseNr  = 1;
    // unsigned int specularNr = 1;
    // unsigned int normalNr   = 1;
    // unsigned int heightNr   = 1;
    /*for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if(name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to stream
            else if(name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to stream

                                                    // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }*/
    

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


Mesh createMesh(std::vector<glm::vec3> &vertexPoints, std::vector<std::vector<unsigned int>> &indices, glm::vec4 color) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indicesRet;
    int index = 0;

    srand(1);

    for (unsigned int i = 0; i < indices.size(); i++) {
        std::vector<unsigned int> face = indices[i];
        int ipivot = face[0];
        glm::vec3 pivot = vertexPoints[ipivot];

        for (unsigned int j = 1; j < face.size() - 1; j++) {
            int i1 = face[j], i2 = face[j+1];
            glm::vec3 p1 = vertexPoints[i1];
            glm::vec3 p2 = vertexPoints[i2];

            glm::vec3 normal = glm::normalize(glm::cross(p1 - pivot, p2 - pivot));

            color = glm::vec4((rand() % 20) / 20.0, (rand() % 20) / 20.0, (rand() % 20) / 20.0, 1.0);
            Vertex v0 = {pivot, normal, color};
            color = glm::vec4((rand() % 20) / 20.0, (rand() % 20) / 20.0, (rand() % 20) / 20.0, 1.0);
            Vertex v1 = {p1, normal, color};
            color = glm::vec4((rand() % 20) / 20.0, (rand() % 20) / 20.0, (rand() % 20) / 20.0, 1.0);
            Vertex v2 = {p2, normal, color};

            vertices.push_back(v0);
            vertices.push_back(v1);
            vertices.push_back(v2);

            indicesRet.push_back(index++);
            indicesRet.push_back(index++);
            indicesRet.push_back(index++);

            // std::cout << pivot.x << " " << pivot.y << " " << pivot.z << endl;
            // std::cout << p1.x << " " << p1.y << " " << p1.z << endl;
            // std::cout << p2.x << " " << p2.y << " " << p2.z << endl;
        }
    }

    return Mesh(vertices, indicesRet);
}

