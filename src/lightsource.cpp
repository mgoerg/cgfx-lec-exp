

#include "lightsource.h"


std::vector<LightSourceData> LightSource::lightSources;



LightSource* LightSource::create(LightSourceData* light) {
    int id = -1;
    for (unsigned int i = 0; i < lightSources.size(); i++) {
        if ( LightSource::lightSources[id].flags & LIGHT_VALID_BIT) {
            id = i;
            break;
        }
    }


    if (id == -1) {
        id = LightSource::lightSources.size();
        LightSource::lightSources.push_back(*light);
    } else {
        LightSource::lightSources[id] = *light;
    }
    LightSource* newLight = new LightSource(id);

    return newLight;
}

void LightSource::remove(LightSource lightSource) {
    lightSource.setFlag(LIGHT_VALID_BIT, false);
    lightSource.setFlag(LIGHT_ENABLED_BIT, false);
}

bool LightSource::hasFlag(int flag) {
    return (lightSources[id].flags & flag) != 0;
}

void LightSource::setFlag(int flag, bool value) {
    if (value) {
        lightSources[id].flags |= flag;
    } else {
        lightSources[id].flags &= (!flag);
    }
}
