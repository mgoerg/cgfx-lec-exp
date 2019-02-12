#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H


#include <vector>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>  // glm::value_ptr()

#include "transform.h"


namespace Renderer {

const int LIGHT_AMBIENT_BIT = 1;
const int LIGHT_DIRECTIONAL_BIT = 1 << 1;
const int LIGHT_POINT_BIT = 1 << 2;

const int LIGHT_VALID_BIT = 1 << 31;
const int LIGHT_ENABLED_BIT = 1 << 30;


const int MAX_LIGHTS = 256;

struct LightSourceData
{
    float color[3] = {1.0, 1.0, 0.1};
    int flags = LIGHT_VALID_BIT | LIGHT_ENABLED_BIT;
    float position[3] = {0.0, 0.0, 1.0};
    float intensity = 1;
    float attenuationConst = 1.0;
    float attenuationLin = 0.1;
    float attenuationSq = 0.1;
    float _pad1;
};

struct LightBlock {
    int numLights = 0;
    float _pad1, _pad2, _pad3;
    LightSourceData lights[MAX_LIGHTS];
};

class LightSource {
    public:
        LightSource(const LightSourceData& light, Transform transform = Transform());

        bool isValid();
        bool hasFlag(int flag);
        void setFlag(int flag, bool value);

        LightSourceData getDataStruct(Transform parentTransform = Transform());

        Transform getTransform();
        void setTransform(Transform transform);
        
    private:
        void setDataPosition(glm::vec3 position);
        LightSourceData data;
        Transform transform;
};


LightSource ambientLightSource(glm::vec3 color);

LightSource directionLightSource(glm::vec3 direction, glm::vec3 color, float intensity);

LightSource pointLightSource(
        glm::vec3 position, 
        glm::vec3 color, 
        float intensity, 
        float attenuationConst, 
        float attenuationLinear, 
        float attenuationSquare
);


} // namespace Light


#endif // LIGHTSOURCE_H