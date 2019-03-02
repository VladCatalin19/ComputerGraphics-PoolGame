#include "Ball.h"

using namespace PoolObjects;

Ball::Ball() {}

Ball::Ball(glm::vec3 position, float radius, BallColor color,
    glm::vec3 direction) : Entity(position), 
    radius(radius), color(color), direction(direction), isInPocket(false) {}

Ball::~Ball() {}

float Ball::GetRadius() {
    return radius;
}

Ball::BallColor Ball::GetColor() {
    return color;
}

float Ball::GetSpeed() {
    return speed;
}

void Ball::SetSpeed(float speed) {
    this->speed = speed;
}

void Ball::DecreaseSpeed(float speed) {
    this->speed = std::max(0.0f, this->speed - speed);
}

glm::vec3 Ball::GetDirection() {
    return direction;
}

void Ball::SetDirection(glm::vec3 direction) {
    this->direction = direction;
}

bool Ball::IsInPocket() {
    return isInPocket;
}

void Ball::SetIfInPocket(bool isInPocket) {
    this->isInPocket = isInPocket;
}

std::string Ball::BallColorName(Ball::BallColor b) {
    switch (b) {
    case white:
        return "white";

    case black:
        return "black";

    case red:
        return "red";

    case yellow:
        return "yellow";

    default:
    case none:
        return "any";
    }
}