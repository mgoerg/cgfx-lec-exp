#ifndef MISCCOMPONENTS_H
#define MISCCOMPONENTS_H


#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include "entt.hpp"

#include "entity.h"
#include "transform.h"



// Impact
///////////////////////////////////////////////////////////
enum ImpactAnimationType {
    looping,
    once
};

struct ImpactAnimationKeyFrame {
    glm::vec3 position;
    float intensity = 1.0;
    float distance = 0.0;
    float width = 10.0;
};

struct SimpleImpactAnimation {
    ImpactAnimationType animationType;
    float t;
    std::vector<ImpactAnimationKeyFrame> keyFrames;
};

void updateKeyFrame(World world, Entity entity, float dt);


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