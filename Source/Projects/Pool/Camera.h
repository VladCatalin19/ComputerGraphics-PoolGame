#pragma once
#include <include/glm.h>
#include <include/math.h>

#include "PoolNamespace.h"

using namespace PoolObjects;

/**
 * This class implemets the camera used to draw objects on. It can be moved in
 * all 6 directions, rotated in first person, and rotated in third person
 * relative to a certain point.
 */
class PoolObjects::Camera {
public:
    Camera() {
        position = glm::vec3(0, 0, 0);
        forward = glm::vec3(0, 0, -1);
        up = glm::vec3(0, 1, 0);
        right = glm::vec3(1, 0, 0);
        distanceToTarget = 0.8f;
    }

    Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up) {
        Set(position, center, up);
    }

    ~Camera() {
    
    }

    void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up) {
        this->position = position;
        forward = glm::normalize(center - position);
        right = glm::cross(forward, up);
        this->up = glm::cross(right, forward);
    }

    void SetPositionAndRotation(glm::vec3 position, glm::vec3 rotation,
                                float distanceToTarget = 2.0f) {
        this->position = position;
        forward = glm::vec3(0, 0, -1);
        up = glm::vec3(0, 1, 0);
        right = glm::vec3(1, 0, 0);
        RotateFirstPerson_OX(rotation.x);
        RotateFirstPerson_OY(rotation.y);
        RotateFirstPerson_OZ(rotation.z);
        this->distanceToTarget = distanceToTarget;
    }

    void TranslateForward(float distance) {
        this->position += glm::normalize(forward) * distance;
    }

    void TranslateUpword(float distance) {
        this->position += glm::normalize(up) * distance;
    }

    void TranslateRight(float distance) {
        this->position += glm::normalize(right) * distance;
    }

    void RotateFirstPerson_OX(float angle) {
        forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, right)
                                                     * glm::vec4(forward, 0)));
        up = glm::cross(right, forward);
    }

    void RotateFirstPerson_OY(float angle) {
        forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle,
                                           glm::vec3(0, 1, 0))
                               * glm::vec4(forward, 0)));
        right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle,
                                         glm::vec3(0, 1, 0))
                             * glm::vec4(right, 0)));
        up = glm::cross(right, forward);
    }

    void RotateFirstPerson_OZ(float angle) {
        right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, up)
                                       * glm::vec4(right, 0)));
        up = glm::cross(right, forward);
    }

    void RotateThirdPerson_OX(float angle) {
        TranslateForward(distanceToTarget);
        RotateFirstPerson_OX(angle);
        TranslateForward(-distanceToTarget);
    }

    void RotateThirdPerson_OY(float angle) {
        TranslateForward(distanceToTarget);
        RotateFirstPerson_OY(angle);
        TranslateForward(-distanceToTarget);
    }

    void RotateThirdPerson_OZ(float angle) {
        TranslateForward(distanceToTarget);
        RotateFirstPerson_OZ(angle);
        TranslateForward(-distanceToTarget);
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(position, position + forward, up);
    }

    glm::vec3 GetTargetPosition() {
        return position + forward * distanceToTarget;
    }

    glm::vec3 GetPosition() {
        return position;
    }

private:
    float distanceToTarget;
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;
};
