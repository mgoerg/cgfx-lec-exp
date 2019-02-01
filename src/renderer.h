#ifndef RENDER_H
#define RENDER_H

#include "shader.h"
#include "camera.h"
// #include "node.h"
#include "utils.h"
#include "mesh.h"
#include "lightsource.h"

#include <SDL2/SDL.h>
#include "GL/glew.h"
#include <SDL2/SDL_opengl.h>

#include <vector>


namespace Renderer {


struct MeshRenderable {
    glm::mat4 orth = glm::mat4(1.0f);
    //glm::mat4 scale;
    MeshID meshID;
};


struct UniformBlockBuffer {
    GLuint buf;
    GLuint blockIndex;
    GLint size;
    GLint binding;
    std::string name;
};


class Renderer {
    public: 
        std::shared_ptr<ShaderProgram> shaderProgram;

        std::map<std::string, UniformBlockBuffer> ubs;

        bool initUniformBlockBuffer(std::string name, GLint blockbinding) {
            UniformBlockBuffer block;
            block.binding = blockbinding;

            block.blockIndex = glGetUniformBlockIndex( shaderProgram->ID,   name.c_str());

            if (block.blockIndex == GL_INVALID_INDEX) {
                return false;
            }

            glGetActiveUniformBlockiv(shaderProgram->ID, block.blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &block.size);

            glUniformBlockBinding(shaderProgram->ID, block.blockIndex, block.binding );

            glGenBuffers(1, &block.buf);
            glBindBuffer(GL_UNIFORM_BUFFER, block.buf);

            // glBufferData(GL_UNIFORM_BUFFER, 
            //     blockSize, 
            //     &lights, 
            //     GL_STATIC_DRAW);
            
            glBindBufferRange(GL_UNIFORM_BUFFER, blockbinding, block.buf, 0, block.size);
            //glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockIndex, lightUniformBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            ubs[name] = block;

            return true;
        }

        UniformBlockBuffer getBuffer(std::string name) {
            return ubs[name];
        }

        Renderer() { }

        Renderer(bool &success, const std::string &vertexShaderPath, 
            const std::string &fragmentShaderPath, 
            const std::string &geometryShaderPath =""
        ) {
            shaderProgram = std::make_shared<ShaderProgram>(success, vertexShaderPath, fragmentShaderPath, geometryShaderPath);
            if (!success) return;

            if (!initUniformBlockBuffer("LightBlock", 1)) {
                success = false;
                std::cout << "Uniform block LightBlock not found!" << std::endl;
                return;
            }

            shaderProgram->use();
            shaderProgram->setUniform("u_useLighting", GL_FALSE);
        }



        void initFrame() {
            meshesToRender = std::map<MeshID, std::vector<MeshRenderable>>();
            shaderProgram->use();
        }



        void renderMesh(MeshRenderable meshJob) {
            if (meshesToRender.find(meshJob.meshID) == meshesToRender.end()) {
                meshesToRender[meshJob.meshID] = std::vector<MeshRenderable>();
            }
            meshesToRender[meshJob.meshID].push_back(meshJob);
        }

        void renderFrame(Camera camera, float aspect) {
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            shaderProgram->use();

            glm::mat4 projection = camera.projectionMatrix(aspect);
            glm::mat4 view = camera.viewMatrix();

            shaderProgram->setUniform("u_viewMatrix", glm::value_ptr(view));
            shaderProgram->setUniform("u_projectionMatrix", glm::value_ptr(projection));

            // Collect meshes to render

            // for (int i = 0; i < numObjects; i++) {
            //     Node* obj = objects[i];
            //     obj->prepareRender();
            //     if (obj == NULL || obj->renderComponent == NULL) continue;
            //     RenderComponent* renderComponent = obj->renderComponent;
            //     MeshRenderable toRender;
            //     for (int i = 0; i < renderComponent->meshes.size(); i++) {
            //         MeshRenderObject mesh = renderComponent->meshes[i];
            //         if (!mesh.enabled) continue;

            //         MeshRenderable meshJob;
            //         meshJob.orth = worldTransformIsometric(*obj) * mesh.transform.modelMatrixIsometric();
            //         //meshJob.scale = glm::mat4(1.0f);
            //         meshJob.meshID = mesh.meshID;
            //         meshes[meshJob.meshID].push_back(meshJob);
            //     }
            // }

            // Set lights
            // UniformBlockBuffer lightbuf = ubs["LightBlock"];
            // glBindBuffer(GL_UNIFORM_BUFFER, lightbuf.buf);
            // glBufferData(GL_UNIFORM_BUFFER, 
            //     lightbuf.size, 
            //     &Light::block, 
            //     GL_STATIC_DRAW);
            // glBindBufferRange(GL_UNIFORM_BUFFER, lightbuf.binding, lightbuf.buf, 0, lightbuf.size);


            // Send render calls to shader.
            for ( auto it = meshesToRender.begin(); it != meshesToRender.end(); it++ ) {
                unsigned int meshID = it->first;
                auto mesh = getMesh(meshID);

                mesh->prepareDraw(*shaderProgram);

                std::vector<MeshRenderable> meshList = it->second;
                for (unsigned int i = 0; i < meshList.size(); i++) {
                    MeshRenderable meshJob = meshList[i];
                    shaderProgram->setUniform("u_modelMatrix", glm::value_ptr(meshJob.orth));
                    glm::mat4 normalmat = glm::transpose(glm::inverse(meshJob.orth));
                    shaderProgram->setUniform("u_normalMatrix", glm::value_ptr(normalmat));
                    mesh->draw();
                }
            }
            //gShaderProgram->setUniform("u_useCelShading", GL_FALSE);

            // glBindVertexArray( 0 );
            // glUseProgram( 0 );
        }

    private:
        std::map<MeshID, std::vector<MeshRenderable>> meshesToRender;

};

} // namespace Renderer

#endif // RENDER_H