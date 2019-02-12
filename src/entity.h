
#ifndef ENTITY_H
#define ENTITY_H

typedef unsigned int Entity;

#include "entt.hpp"

typedef entt::registry<Entity> World;

#include "transform.h"
#include "node.h"



/* Get World coordinate transform of entity 
 */
Transform getWorldTransform(entt::registry<Entity>& world, Entity entity);



#endif // ENTITY_H