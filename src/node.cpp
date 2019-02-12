#include "node.h"


Entity getParent(entt::registry<Entity>& world, Entity entity)
{
    if (!(world.has<Node>(entity))) { return 0; }
    auto& node = world.get<Node>(entity);
    if (node.isRoot) { return 0; }
    return node.parentID;
}

void detachChild(entt::registry<Entity>& world, Entity child, Entity parent) 
{
    assert(world.has<Node>(child));
    if (!world.has<Node>(parent)) return;

    auto& childNode = world.get<Node>(child);
    childNode.isRoot = true;

    auto& parentNode = world.get<Node>(parent);

    assert(childNode.parentID == parent);
    parentNode.children.erase(child);
}

void attachChild(entt::registry<Entity>& world, Entity child, Entity parent) 
{
    if (world.has<Node>(parent)) { world.assign<Node>(parent); }
    auto& parentNode = world.get<Node>(parent);

    parentNode.children.insert(child);

    if (!world.has<Node>(child)) 
    {
        world.assign<Node>(child);
    }
    auto& childNode = world.get<Node>(child);
    if (!childNode.isRoot) {
        Entity currentParent = childNode.parentID;
        if (world.valid(currentParent)) {
            detachChild(world, child, currentParent);
        }
    }
    childNode.isRoot = false;
    childNode.parentID = parent;
}


#ifdef FOOOO
void NodeSystem::configure(ECS::World* world) 
{
    world->subscribe<ECS::Events::OnEntityDestroyed>(this);
    world->subscribe<ECS::Events::OnComponentRemoved<Node>>(this);
}

void NodeSystem::unconfigure(ECS::World* world) 
{
    world->unsubscribeAll(this);
}

void NodeSystem::receive(class ECS::World* world, const ECS::Events::OnEntityDestroyed& event) 
{
    if (!event.entity->has<Node>()) return;

    auto node = event.entity->get<Node>();
    auto parentID = node->parentID;
    ECS::Entity* parent = (world->getById(parentID));

    if (parent != nullptr) { 
        detachChild(event.entity, parent);
    }
}


void NodeSystem::receive(class ECS::World* world, const ECS::Events::OnComponentRemoved<Node>& event) {
    auto node = event.entity->get<Node>();
    auto parentID = node->parentID;
    ECS::Entity* parent = world->getById(parentID);

    if (parent != nullptr) {
        detachChild(event.entity, parent);
    }
}
#endif


