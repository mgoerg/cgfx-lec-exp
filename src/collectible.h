

#include "entity.h"
#include "entt.hpp"
#include <vector>
#include "transform.h"
#include "rendercomponent.h"
#include "impactsource.h"


struct Collectible {
    int scoreGain = 10;
    int type = 0;
    bool active = true;
};

struct Collector {
    float pickupRange = 1.0;
    int score;
};


void updateCollector(World& world, float dt) {
    std::vector<Entity> collectiblesToDelete;
    auto collectorView = world.template view<Transform, Collector>();
    auto collectibleView = world.template view<Transform, Collectible>();
    for (auto collectorEntity : collectorView) {
        for (auto collectibleEntity : collectibleView) {
            auto& collectibleTransform = collectibleView.template get<Transform>(collectibleEntity);
            auto& collectorTransform = collectorView.template get<Transform>(collectorEntity);
            Collector& collector = collectorView.template get<Collector>(collectorEntity);

            if (glm::distance(collectibleTransform.position, collectorTransform.position) < collector.pickupRange) {
                // Actual Collection takes place.
                Collectible& collectible = collectibleView.template get<Collectible>(collectibleEntity);
                collector.score += collectible.scoreGain;

                // Destroy 
                collectiblesToDelete.push_back(collectibleEntity);
            }
        }
    }

    for (Entity collectible : collectiblesToDelete) {
        createCollectibleCorpse(world, collectible);
        world.destroy(collectible);
    }
}


void createCollectibleCorpse(World& world, Entity collectible) {
    Entity corpse = world.create();
    auto& render = world.assign<RenderComponent>(corpse);
    auto& impact = world.assign<Impact>(corpse);
}