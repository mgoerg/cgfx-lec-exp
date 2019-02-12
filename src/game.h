
#ifndef GAME_H
#define GAME_H

#include "node.h"

class Game {
    public:
        std::vector<Node> gameObjects;
        Node world;

        Node& createNode(Node &parent);
};

#endif // GAME_H