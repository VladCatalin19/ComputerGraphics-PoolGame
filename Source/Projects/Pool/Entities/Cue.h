#pragma once

#include "Entity.h"

/**
 * This class implemets the cue used in the scene. The cue will appear when
 * the white ball has to be hit and will move according to the animation
 * created in shaders.
 */
class PoolObjects::Cue : public PoolObjects::Entity {
private:
    float length;
    bool isSwinging;
    float angle;
    float swingTime;

public:
    Cue();
    Cue(glm::vec3 position, float length);
    ~Cue();

    float GetLength();
    bool IsSwinging();
    void StartSwinging();
    void StopSwinging();
    float GetAngle();
    void AddToAngle(float rads);
    void SetAngle(float angle);
    void ResetSwingTime();
    void AddSwingTime(float deltaTimeSeconds);
    float GetSwingTime();
};