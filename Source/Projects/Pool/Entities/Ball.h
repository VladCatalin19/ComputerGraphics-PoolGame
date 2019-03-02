#pragma once

#include "Entity.h"
#include <Projects/Pool/Constants.h>
#include <algorithm>

/**
 * This class implemets the ball used in the scene. Balls will move on the
 * table and will collide with other balls or cushions, or will enter pcokets.
 */
class PoolObjects::Ball : public PoolObjects::Entity {
public:
    enum BallColor{none, white, black, red, yellow};

    static std::string BallColorName(BallColor b);

private:
    float radius;
    BallColor color;
    float speed;
    glm::vec3 direction;
    bool isInPocket;

public:
    Ball();
    Ball(glm::vec3 position, float radius, BallColor color,
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f));
    ~Ball();

    float GetRadius();
    BallColor GetColor();
    float GetSpeed();
    void SetSpeed(float speed);
    void DecreaseSpeed(float speed);
    glm::vec3 GetDirection();
    void SetDirection(glm::vec3 direction);
    bool IsInPocket();
    void SetIfInPocket(bool isInPocket);
};