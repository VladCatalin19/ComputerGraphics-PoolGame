#pragma once

#include "Entity.h"

/**
 * This class implemets the pocket used in the scene. Pockets are ellipses.
 * A ball will enter a pocket when the ball's center is inside the ellipse.
 */
class PoolObjects::Pocket : public PoolObjects::Entity{
private:
    float xRadius;
    float zRadius;

public:
    Pocket();
    Pocket(glm::vec3 position, float xRadius, float zRadius);
    ~Pocket();
    float GetXRadius();
    float GetZRadius();
};