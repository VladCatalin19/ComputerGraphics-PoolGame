#pragma once

#include "Entity.h"

/**
 * This class implemets the table used in the scene. The table 6 cushions and
 * 6 pockets.
 */
class PoolObjects::Table : public PoolObjects::Entity {
private:
    Cushion **cushions;
    Pocket **pockets;

public:
    Table();
    Table(glm::vec3 position, Cushion **cushions, Pocket **pockets);
    ~Table();

    Cushion** GetCushions();
    Pocket** GetPockets();
};