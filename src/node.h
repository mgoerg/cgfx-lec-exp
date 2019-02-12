
#ifndef NODE_H
#define NODE_H


#include <set>
#include <cassert>

#include "entt.hpp"

#include "transform.h"
#include "entity.h"


struct Node {
    bool isRoot = true;
    Entity parentID;
    std::set<Entity> children;
};


/* Get the parent of an entity.
    * Returns 0 if the entity is a root.
    */
Entity getParent(entt::registry<Entity>& world, Entity entity);

/* Detach an entity from its parent, leaving the former child as a root.
    * Will leave the node component in existence.
    */
void detachChild(entt::registry<Entity>& world, Entity child, Entity parent);

/* Set an entity as child of the other. 
    * Node components for both entities may be created if necessary.
    * If the entity already has a parent, it will be detached from the parent. 
    */
void attachChild(entt::registry<Entity>& world, Entity child, Entity parent);


#endif // NODE_H