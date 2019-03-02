#include "Cushion.h"

using namespace PoolObjects;

Cushion::Cushion() {}

Cushion::Cushion(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 P4)
    : Entity(glm::vec3((P1.x + P2.x + P3.x + P4.x) / 4.0f,
             0.0f,
             (P1.z + P2.z + P3.z + P4.z) / 4.0f)) {
    points[0] = P1;
    points[1] = P2;
    points[2] = P3;
    points[3] = P4;
    
    normals[0] = CalculateNormal(P1, P2);
    normals[1] = CalculateNormal(P2, P3);
    normals[2] = CalculateNormal(P3, P4);
}

Cushion::~Cushion() {}

// Normal = (-dz, 0, dx)
glm::vec3 Cushion::CalculateNormal(glm::vec3 P1, glm::vec3 P2) {
    return glm::normalize(glm::vec3(P1.z - P2.z, 0.0f, P2.x - P1.x));
}

glm::vec3* Cushion::GetPointsArray() {
    return points;
}

glm::vec3* Cushion::GetNormals() {
    return normals;
}