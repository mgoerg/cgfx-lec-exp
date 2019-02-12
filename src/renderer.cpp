#include "renderer.h"



bool Renderer::Renderer::initUniformBlockBuffer(std::string name, GLint blockbinding, void* contentAddress) 
{
    UniformBlockBuffer block;
    block.binding = blockbinding;

    block.blockIndex = glGetUniformBlockIndex( shaderProgram->ID, name.c_str());

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
    block.contentAddress = contentAddress;
    ubs[name] = block;

    return true;
}

Renderer::UniformBlockBuffer* Renderer::Renderer::getBuffer(std::string name) 
{
    return &this->ubs[name];
}

Renderer::Renderer::Renderer(bool &success, const std::string &vertexShaderPath, 
    const std::string &fragmentShaderPath, 
    const std::string &geometryShaderPath /*=""*/
) {
    shaderProgram = std::make_shared<ShaderProgram>(success, vertexShaderPath, fragmentShaderPath, geometryShaderPath);
    if (!success) return;

    if (!initUniformBlockBuffer("LightBlock", 1, &lightBlock)) {
        success = false;
        std::cout << "Uniform block LightBlock not found!" << std::endl;
        return;
    }

    if (!initUniformBlockBuffer("ImpactBlock", 2, &impactBlock)) {
        success = false;
        std::cout << "Uniform block ImpactBlock not found!" << std::endl;
        return;
    }

    shaderProgram->use();
    shaderProgram->setUniform("u_useLighting", GL_TRUE);
    shaderProgram->setUniform("u_useImpact", GL_TRUE);
}



void Renderer::Renderer::initFrame() {
    meshInstances = std::map<MeshID, std::vector<MeshInstance>>();
    shaderProgram->use();
    lightBlock = LightBlock();
    impactBlock = ImpactBlock();
}



void Renderer::Renderer::renderMeshInstance(MeshInstance meshJob) {
    if (meshInstances.find(meshJob.meshID) == meshInstances.end()) {
        meshInstances[meshJob.meshID] = std::vector<MeshInstance>();
    }
    meshInstances[meshJob.meshID].push_back(meshJob);
}

bool Renderer::Renderer::renderLightSource(const LightSourceData& light) {
    if (lightBlock.numLights == MAX_LIGHTS) {
        return false;
    }
    lightBlock.lights[lightBlock.numLights] = light;
    lightBlock.numLights++;
    return true;
}

bool Renderer::Renderer::renderImpactSource(const ImpactSourceData& data) {
    if (impactBlock.numImpactSources == MAX_IMPACTS) {
        return false;
    }
    impactBlock.impactSources[impactBlock.numImpactSources] = data;
    impactBlock.numImpactSources++;
    return true;
}



void Renderer::Renderer::renderFrame(Camera camera, float aspect) {
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
    //     MeshInstance toRender;
    //     for (int i = 0; i < renderComponent->meshes.size(); i++) {
    //         MeshRenderObject mesh = renderComponent->meshes[i];
    //         if (!mesh.enabled) continue;

    //         MeshInstance meshJob;
    //         meshJob.orth = worldTransformIsometric(*obj) * mesh.transform.modelMatrixIsometric();
    //         //meshJob.scale = glm::mat4(1.0f);
    //         meshJob.meshID = mesh.meshID;
    //         meshes[meshJob.meshID].push_back(meshJob);
    //     }
    // }

    // Set uniform blocks
    for (auto const& x : Renderer::ubs) {
        glBindBuffer(GL_UNIFORM_BUFFER, x.second.buf);
        glBufferData(GL_UNIFORM_BUFFER, 
            x.second.size, 
            x.second.contentAddress, 
            GL_STATIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, x.second.binding, x.second.buf, 0, x.second.size);
    }

    // Send render calls to shader.
    for ( auto it = meshInstances.begin(); it != meshInstances.end(); it++ ) {
        unsigned int meshID = it->first;
        auto mesh = getMesh(meshID);

        mesh->prepareDraw(*shaderProgram);

        std::vector<MeshInstance> meshList = it->second;
        for (unsigned int i = 0; i < meshList.size(); i++) {
            MeshInstance meshJob = meshList[i];
            shaderProgram->setUniform("u_useLighting", meshJob.enableLighting);
            shaderProgram->setUniform("u_useImpact", meshJob.enableImpact);
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

