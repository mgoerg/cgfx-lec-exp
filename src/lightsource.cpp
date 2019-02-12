

#include "lightsource.h"
#include <iostream>





Renderer::LightSource::LightSource(const LightSourceData& lightData, Transform transform) {
    data = lightData;
    transform = transform;
}


bool Renderer::LightSource::isValid(){
    return hasFlag(Renderer::LIGHT_VALID_BIT);
}

bool Renderer::LightSource::hasFlag(int flag) {
    return (data.flags & flag) != 0;
}

void Renderer::LightSource::setFlag(int flag, bool value) {
    if (value) {
        data.flags |= flag;
    } else {
        data.flags &= (!flag);
    }
}

void Renderer::LightSource::setDataPosition(glm::vec3 position)  {
    data.position[0] = position[0];
    data.position[1] = position[1];
    data.position[2] = position[2];
}

Renderer::LightSourceData Renderer::LightSource::getDataStruct(Transform parentTransform) {
    if (!hasFlag(LIGHT_DIRECTIONAL_BIT)) {
        glm::vec3 pos = (parentTransform * transform).position;
        setDataPosition(pos);
    }
    return data;
}

Transform Renderer::LightSource::getTransform() {
    return this->transform;
}
void Renderer::LightSource::setTransform(Transform transform) {
    this->transform = transform;
}



Renderer::LightSource Renderer::ambientLightSource(glm::vec3 color) {
    Renderer::LightSourceData data;
    data.flags |= Renderer::LIGHT_AMBIENT_BIT;
    data.color[0] = color.r; data.color[1] = color.g; data.color[2] = color.b;
    
    Renderer::LightSource tmp (data);
    return tmp;
}

Renderer::LightSource Renderer::directionLightSource(glm::vec3 direction, glm::vec3 color, float intensity) {
    Renderer::LightSourceData data;
    data.flags |= Renderer::LIGHT_DIRECTIONAL_BIT;
    data.color[0] = color.r; data.color[1] = color.g; data.color[2] = color.b;
    data.position[0] = direction.x; data.position[1] = direction.y; data.position[2] = direction.z;
    data.intensity = intensity;

    return Renderer::LightSource(data);
}


Renderer::LightSource Renderer::pointLightSource(
        glm::vec3 position, 
        glm::vec3 color, 
        float intensity, 
        float attenuationConst, 
        float attenuationLinear, 
        float attenuationSquare
) {
    Renderer::LightSourceData data;
    data.flags |= Renderer::LIGHT_POINT_BIT;
    data.color[0] = color.r; data.color[1] = color.g; data.color[2] = color.b;
    data.position[0] = position.x; data.position[1] = position.y; data.position[2] = position.z;
    data.intensity = intensity;
    data.attenuationConst = attenuationConst;
    data.attenuationLin = attenuationLinear;
    data.attenuationSq = attenuationSquare;



    return Renderer::LightSource(data, Transform(position));
}
