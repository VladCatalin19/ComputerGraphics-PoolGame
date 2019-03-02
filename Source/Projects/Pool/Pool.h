#pragma once

#include <Component/SimpleScene.h>
#include "Level/Level.h"
#include "Constants.h"
#include "PoolNamespace.h"
#include "Camera.h"
#include <iostream>

class Pool : public SimpleScene {
public:
    Pool();
    ~Pool();
    void Init() override;

    enum LevelStates {none, movingWhiteBall, hitting, wantToHit};

private:
    PoolObjects::Camera* camera;
    glm::mat4 projectionMatrix;

    //Light & material properties
    glm::vec3 lightPosition;
    unsigned int materialShininess;
    float materialKd;
    float materialKs;

    PoolObjects::Level *level;
    LevelStates currentLevelState;
    std::vector<int> keysAllowedToPress;

    void InitCamera();
    void GenerateMeshes();
    void GenerateShaders();
    void InitKeysAllowedToPress();
    void FrameStart() override;
    void Update(float deltaTimeSeconds) override;
    void FrameEnd() override;

    void DrawScene();
    void RenderMesh(Mesh * mesh, Shader * shader, glm::vec3 position,
                    glm::vec3 scale = glm::vec3(1.0f)) override;
    void RenderMeshColor(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix,
                         const glm::vec3 &color);
    void RenderMeshCue(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix,
                       const float & time, const bool isActive);

    void OnInputUpdate(float deltaTime, int mods) override;
    void OnKeyPress(int key, int mods) override;
    void OnKeyRelease(int key, int mods) override;
    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
    void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
};