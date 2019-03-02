#include "Cue.h"

using namespace PoolObjects;

Cue::Cue() {}

Cue::Cue(glm::vec3 position, float length) : Entity(position), 
    length(length), isSwinging(false), swingTime(0.0f) {
}

Cue::~Cue() {}

float Cue::GetLength() {
    return length;
}

bool Cue::IsSwinging() {
    return isSwinging;
}

void Cue::StartSwinging() {
    isSwinging = true;
}

void Cue::StopSwinging() {
    isSwinging = false;
}

float Cue::GetAngle() {
    return angle;
}

void Cue::AddToAngle(float rads) {
    angle += rads;
}

void Cue::SetAngle(float angle) {
    this->angle = angle;
}

void Cue::ResetSwingTime() {
    swingTime = 0.0f;
}

void Cue::AddSwingTime(float deltaTimeSeconds) {
    swingTime += deltaTimeSeconds;
}

float Cue::GetSwingTime() {
    return swingTime;
}