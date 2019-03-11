#ifndef MISCCOMPONENTS_H
#define MISCCOMPONENTS_H


#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include "entt.hpp"

#include "entity.h"
#include "transform.h"
#include <cassert>



// Impact
///////////////////////////////////////////////////////////
enum ImpactAnimationType {
    IMPACT_LOOPING
};

struct ImpactAnimationKeyFrame {
    glm::vec3 position;
    float intensity = 1.0;
    float distance = 0.0;
    float duration = 1.0;
};

struct SimpleImpactAnimation {
    ImpactAnimationType animationType = IMPACT_LOOPING;
    float t = 0;
    std::vector<ImpactAnimationKeyFrame> keyFrames;
    glm::vec3 direction = glm::vec3(0, 0, 1);
    float width = 10;
};

void updateImpactAnim(SimpleImpactAnimation& anim, float dt) {
    anim.t += dt;
    float animLength = 0;
    for (int i = 0; i < anim.keyFrames.size() - 1; i++) {
        animLength += anim.keyFrames[i].duration;
    }

    assert(animLength > 0);

    while (anim.t > animLength) {
        anim.t -= animLength;
    }
}

Renderer::ImpactSource getImpactFromAnimation(SimpleImpactAnimation& anim) {
    int index = 0;
    float u = 0;
    for (auto kf : anim.keyFrames) {
        u += kf.duration;
        if (anim.t < u) { break; }
        index++;
    }

    auto currentKeyFrame = anim.keyFrames[index];
    auto nextKeyFrame = anim.keyFrames[(index + 1) % anim.keyFrames.size()];
    float t = (u - anim.t) / currentKeyFrame.duration;
    Renderer::ImpactSourceData resultData;
    glm::vec3 pos = t* currentKeyFrame.position + (1-t) * nextKeyFrame.position;
    for (int i = 0; i < 3; i++) {
        resultData.position[i] = pos[i];
        resultData.direction[i] = anim.direction[i];
    }
    resultData.intensity = t*currentKeyFrame.intensity + (1-t) * nextKeyFrame.intensity;
    resultData.distance = t*currentKeyFrame.distance + (1-t)*nextKeyFrame.distance;
    resultData.width = anim.width;
    return Renderer::ImpactSource(resultData, Transform(pos));
}


// Simple Patrol 
///////////////////////////////////////////////////////////


enum PatrolType {
    PATROL_LOOP, // After reaching last, go directly to first
    PATROL_LOOPTP, // After reaching last, teleport to first
    PATROL_ALTERNATE // After reaching last, go back to second to last.
};

struct SimplePatrolBehavior {
    std::vector<Transform> routePoints;
    unsigned int positionIndexRaw;
    float position;
    float velocity = -1.0;
    PatrolType patrolType = PATROL_LOOP;
};

unsigned int positionIndex(unsigned int positionIndexRaw, unsigned int maximum, PatrolType patrolType) 
{
    if (patrolType == PATROL_LOOP) {
        return positionIndexRaw % maximum;
    } else if (patrolType == PATROL_LOOPTP) {
        return positionIndexRaw % (maximum - 1);
    } else {
        if ((positionIndexRaw / (maximum-1)) % 2 == 0) {
            return positionIndexRaw % (maximum - 1);
        } else {
            return (maximum - 1) - (positionIndexRaw % (maximum - 1));
        }
    }
}

void updatePatrolBehavior(Transform& transform, SimplePatrolBehavior& patrol, float dt) {
    Transform from = patrol.routePoints[positionIndex(patrol.positionIndexRaw, patrol.routePoints.size(), patrol.patrolType)];
    Transform to = patrol.routePoints[positionIndex(patrol.positionIndexRaw + 1, patrol.routePoints.size(), patrol.patrolType)];
    if (patrol.velocity < 0) {
        patrol.position += dt;
    } else {
        patrol.position += dt * patrol.velocity / glm::distance(from.position, to.position);
    }

    if (patrol.position >= 1) {
        patrol.position -= 1.0;
        patrol.positionIndexRaw += 1;
        from = to;
        to = patrol.routePoints[positionIndex(patrol.positionIndexRaw + 1, patrol.routePoints.size(), patrol.patrolType)];
    }

    transform = mix(from, to, patrol.position);
}

void updatePatrolBehavior(World& world, Entity entity, float dt) {
    auto& transform = world.get<Transform>(entity);
    auto& patrol = world.get<SimplePatrolBehavior>(entity);

    updatePatrolBehavior(transform, patrol, dt);
}

#endif