#include "Pocket.h"

using namespace PoolObjects;

Pocket::Pocket() {}

Pocket::Pocket(glm::vec3 position, float xRadius, float zRadius)
    : Entity(position), xRadius(xRadius), zRadius(zRadius) {}

Pocket::~Pocket() {}

float Pocket::GetXRadius() {
    return xRadius;
}

float Pocket::GetZRadius() {
    return zRadius;
}