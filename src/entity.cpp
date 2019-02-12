
#include "entity.h"



Transform getWorldTransform(entt::registry<Entity>& world, Entity entity) {
    Transform transform;
    if (world.has<Transform>(entity)) {
        transform = world.get<Transform>(entity);
    }

    Entity parent = getParent(world, entity);
    if (world.valid(parent)) {
        transform = getWorldTransform(world, parent) * transform;
    }

    return transform;
}
