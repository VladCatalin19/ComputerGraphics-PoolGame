#pragma once

#include <Core/World.h>
#include <Projects/Pool/PoolNamespace.h>

/**
 * This class implemets the entity used in the scene. An entity is any object from
 * the scene, it provides a position to the object.
 */
class PoolObjects::Entity {
private:
    glm::vec3 position;

public:
    Entity();
    Entity(glm::vec3 position);
    ~Entity();
    glm::vec3 GetPosition();
    void SetPosition(glm::vec3 position);
};