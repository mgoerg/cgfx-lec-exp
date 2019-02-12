#ifndef IMPACT_H
#define IMPACT_H


namespace Renderer {
    
const int MAX_IMPACTS = 256;

const int IMPACT_VALID_BIT = 1 << 31;
const int IMPACT_ENABLED_BIT = 1 << 30;


struct ImpactSourceData {
    float position[3] = {0.0, 0.0, 0.0};
    int flags = IMPACT_VALID_BIT | IMPACT_ENABLED_BIT;

    float direction[3] = {1.0, 0.0, 0.0};
    float intensity = 1.0;

    float distance = 0.0;
    float width = 10;
    float _pad[2];
};

struct ImpactBlock {
    int numImpactSources;
    float _pad[3];
    ImpactSourceData impactSources[MAX_IMPACTS];
};

class ImpactSource {
    public:
        ImpactSource() {}
        ImpactSource(const ImpactSourceData& impact, Transform transform = Transform()) : data(impact), transform(transform) {}

        ImpactSourceData getDataStruct(Transform parentTransform = Transform()) {
            glm::vec3 pos = (parentTransform * transform).position;
            for (int i = 0; i<3; i++) {
                data.position[i] = pos[i];
            }
            return data;
        }

        bool isValid(){ return hasFlag(LIGHT_VALID_BIT); }

        bool hasFlag(int flag) { return (data.flags & flag) != 0; }

        float getDistance() {
            return data.distance;
        }

        void setDistance(float f) { data.distance = f; }

        void setTransform(Transform transform) { this->transform = transform; }
        Transform getTransform() { return transform; }


    private: 
        ImpactSourceData data;
        Transform transform;
};


}
#endif