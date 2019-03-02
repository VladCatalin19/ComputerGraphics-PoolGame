#pragma once

#include "Entity.h"
#include <Projects/Pool/Constants.h>

/**
 * This class implemets the cushion used in the scene. A cushion is a trapezium.
 * It has 4 points and 3 normals of each side the balls can collide with.
 */
class PoolObjects::Cushion : public PoolObjects::Entity {
private:
    glm::vec3 points[POINTS_NUMBER];
    glm::vec3 normals[NORMALS_NUMBER];

    glm::vec3 CalculateNormal(glm::vec3 P1, glm::vec3 P2);
public:
    Cushion();
    Cushion(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 P4);
    ~Cushion();
    glm::vec3* GetPointsArray();
    glm::vec3* GetNormals();
};