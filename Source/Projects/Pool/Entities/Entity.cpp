#include "Entity.h"

using namespace PoolObjects;

Entity::Entity() : position(glm::vec3(0.0f, 0.0f, 0.0f)) {}

Entity::Entity(glm::vec3 position) : position(position) {}

Entity::~Entity() {}

glm::vec3 Entity::GetPosition() {
    return position;
}

void Entity::SetPosition(glm::vec3 position) {
    this->position = position;
}