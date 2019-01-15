#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H


#include <vector>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>  // glm::value_ptr()


const int LIGHT_AMBIENT_BIT = 1;
const int LIGHT_DIRECTIONAL_BIT = 1 << 1;
const int LIGHT_POINT_BIT = 1 << 2;

const int LIGHT_VALID_BIT = 1 << 31;
const int LIGHT_ENABLED_BIT = 1 << 30;

struct LightSourceData
{
    int flags;
    glm::vec3 color;
    float intensity;
    glm::vec3 position;
    glm::vec3 eyePosOrDir;
    float attenuationConst;
    float attenuationLin;
    float attenuationSq;
};


class LightSource {
    static std::vector<LightSourceData> lightSources;
    static LightSource* create(LightSourceData* light);
    static void remove(LightSource lightSource);

    LightSource(int id) {
        id = id;
    }
    int id;

    bool hasFlag(int flag);
    void setFlag(int flag, bool value);
};

void setLightUniforms();

#endif // LIGHTSOURCE_H