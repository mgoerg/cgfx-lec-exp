

#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H


#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>  // glm::value_ptr()

#include "mesh.h"
#include "lightsource.h"
#include "transform.h"
#include "impactsource.h"

#include <vector>



struct MeshRenderObject {
    Renderer::MeshID meshID;
    bool enabled = true;
    bool enableLighting = true;
    bool enableImpact = true;
    Transform transform;
};

struct RenderComponent {
    std::vector<MeshRenderObject> meshes;
    std::vector<Renderer::LightSource> lights;
    std::vector<Renderer::ImpactSource> impacts;
};


#endif // RENDERCOMPONENT_H