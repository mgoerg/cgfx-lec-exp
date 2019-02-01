
#include "mesh.h"

std::map<Renderer::MeshID, std::shared_ptr<Renderer::Mesh>> Renderer::meshes;


void Renderer::Mesh::prepareDraw(ShaderProgram& shaderProgram) {
    if (needUpdate) setup(shaderProgram);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
}


void Renderer::Mesh::setup(ShaderProgram &shaderProgram)
{
    if (!needUpdate) return;

    // create buffers/arrays
    if (VAO == (unsigned int)-1) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    glBindVertexArray(VAO);
    // load data into buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    GLuint positionAttribute = shaderProgram.attributeID("a_vertexPosition");
    glEnableVertexAttribArray(positionAttribute);	
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    // vertex normals
    GLuint normalAttribute = shaderProgram.attributeID("a_vertexNormal");
    glEnableVertexAttribArray(normalAttribute);	
    glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // vertex color
    GLuint colorAttribute = shaderProgram.attributeID("a_vertexColor");
    glEnableVertexAttribArray(colorAttribute);	
    glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    glBindVertexArray(0);

    needUpdate = false;
}



Renderer::MeshID Renderer::newMesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices) {
    unsigned int i = 1;
    while (meshes.find(i) != meshes.end()) {
        i++;
    }

    meshes[i] = std::make_shared<Mesh>(vertices, indices);
    meshes[i]->id = i;
    return i;
}

void Renderer::updateMesh(MeshID id, std::vector<Vertex> &vertices, std::vector<unsigned int> &indices) {
    assert (meshes.find(id) != meshes.end());
    meshes[id] = std::make_shared<Mesh>(vertices, indices);
    meshes[id]->id = id;
}

void Renderer::deleteMesh(Renderer::MeshID id) {
    meshes.erase(id);
}

std::shared_ptr<Renderer::Mesh> Renderer::getMesh(unsigned int id) {
    return meshes[id];
}

void Renderer::Mesh::draw()
{
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}


Renderer::Mesh Renderer::createMesh(std::vector<glm::vec3> &vertexPoints, std::vector<std::vector<unsigned int>> &indices, glm::vec4 color) {
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

    return Renderer::Mesh(vertices, indicesRet);
}

