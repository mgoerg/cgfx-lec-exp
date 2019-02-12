#ifndef RENDER_H
#define RENDER_H

#include "shader.h"
#include "camera.h"
#include "utils.h"
#include "mesh.h"
#include "lightsource.h"
#include "impactsource.h"

#include <SDL2/SDL.h>
#include "GL/glew.h"
#include <SDL2/SDL_opengl.h>

#include <vector>


namespace Renderer {


struct MeshInstance {
    glm::mat4 orth = glm::mat4(1.0f);
    //glm::mat4 scale;
    
    bool enabled = true;
    bool enableLighting = true;
    bool enableImpact = true;

    MeshID meshID;
};


struct UniformBlockBuffer {
    GLuint buf;
    GLuint blockIndex;
    GLint size;
    GLint binding;
    std::string name;
    void* contentAddress;
};


class Renderer {
    public: 
        Renderer(bool &success, const std::string &vertexShaderPath, 
            const std::string &fragmentShaderPath, 
            const std::string &geometryShaderPath =""
        );

        void initFrame();
        void renderFrame(Camera camera, float aspect);

        void renderMeshInstance(MeshInstance meshJob);
        bool renderLightSource(const LightSourceData& light);
        bool renderImpactSource(const ImpactSourceData& data);


    private:
        std::map<MeshID, std::vector<MeshInstance>> meshInstances;
        std::shared_ptr<ShaderProgram> shaderProgram;
        std::map<std::string, UniformBlockBuffer> ubs;
        LightBlock lightBlock;
        ImpactBlock impactBlock;

        bool initUniformBlockBuffer(std::string name, GLint blockbinding, void* contentAddress);
        UniformBlockBuffer* getBuffer(std::string name);
};

} // namespace Renderer

#endif // RENDER_H