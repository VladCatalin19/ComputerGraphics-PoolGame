#include <Core/Engine.h>

#include "Pool.h"

#include <iostream>
using namespace std;

Pool::Pool() {
    camera = new Camera();
    level = new Level(camera);
}

Pool::~Pool() {
    delete level;
    delete camera;
}

Pool::LevelStates stateToEnum(State *state) {
    if (dynamic_cast<MovingWhiteBallState *>(state)) {
        return Pool::LevelStates::movingWhiteBall;
    }

    if (dynamic_cast<WantToHitState *>(state)) {
        return Pool::LevelStates::wantToHit;
    }

    if (dynamic_cast<HittingState *>(state)) {
        return Pool::LevelStates::hitting;
    }

    return Pool::LevelStates::none;
}


glm::vec3 enumToColor(Ball::BallColor color) {
    switch (color) {
    case Ball::BallColor::white:
        return glm::vec3(1.0f, 1.0f, 1.0f);

    case Ball::BallColor::none:
    case Ball::BallColor::black:
        return glm::vec3(0.0f, 0.0f, 0.0f);

    case Ball::BallColor::red:
        return glm::vec3(1.0f, 0.0f, 0.0f);

    case Ball::BallColor::yellow:
        return glm::vec3(1.0f, 1.0f, 0.0f);
    }
    return glm::vec3(1.0f, 1.0f, 1.0f);
}


void Pool::Init() {
    InitCamera();
    GenerateMeshes();
    GenerateShaders();
    InitKeysAllowedToPress();

    lightPosition = glm::vec3(0, 20.0f, 0.0f);
    materialShininess = 30;
    materialKd = 10.5;
    materialKs = 10.5;

    currentLevelState = stateToEnum(level->GetState());
}

void Pool::InitCamera() {
    projectionMatrix = glm::perspective(RADIANS(60), 2.0f, 0.01f, 200.0f);
}

void Pool::GenerateMeshes() {
    // Table and ball mesh taken from: https://free3d.com/3d-model/pool-table-v1--600461.html
    Mesh* mesh = new Mesh("table");
    mesh->LoadMesh(RESOURCE_PATH::MODELS + "Props", "table.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("ball");
    mesh->LoadMesh(RESOURCE_PATH::MODELS + "Props", "ball.obj");
    meshes[mesh->GetMeshID()] = mesh;

    // Cue mesh taken from: https://free3d.com/3d-model/pool-cue-v1--229730.html
    mesh = new Mesh("cue");
    mesh->LoadMesh(RESOURCE_PATH::MODELS + "Props", "cue.obj");
    meshes[mesh->GetMeshID()] = mesh;
}

void Pool::GenerateShaders() {
    // Create a shader program for drawing face polygon with the color
    // and lighting of the object
    Shader *shader = new Shader("Color&Light");
    shader->AddShader(RESOURCE_PATH::SHADERS + "Pool/VertexShaderLight.glsl",
                      GL_VERTEX_SHADER);
    shader->AddShader(RESOURCE_PATH::SHADERS + "Pool/FragmentShaderLight.glsl",
                      GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    // Create a shader program for drawing face polygon with the texture of
    // the object and position depending on the swinging status of the cue
    shader = new Shader("CueShader");
    shader->AddShader(RESOURCE_PATH::SHADERS + "Pool/VertexShaderCue.glsl",
                      GL_VERTEX_SHADER);
    shader->AddShader(RESOURCE_PATH::SHADERS + "Pool/FragmentShaderCue.glsl",
                      GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;
}

void Pool::InitKeysAllowedToPress() {
    int keys[] = { GLFW_KEY_W , GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE };
    keysAllowedToPress = std::vector<int>(keys, keys + sizeof(keys) / sizeof(int));
}

void Pool::FrameStart() {
    // clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

// Call Level update function and draw scene
void Pool::Update(float deltaTimeSeconds) {
    level->Update(deltaTimeSeconds);

    currentLevelState = stateToEnum(level->GetState());

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    DrawScene();
    glDisable(GL_CULL_FACE);

    if (level->HasFinished()) {
        cout << "Restarting Level\n";
        delete level;
        level = new Level(camera);
    }
}

void Pool::DrawScene() {
    glm::mat4 modelMatrix = glm::mat4(1);

    // Render table
    RenderMesh(meshes["table"], shaders["Simple"], level->GetTable()->GetPosition());

    // Render white ball if not pocketed
    Ball *whiteBall = level->GetWhiteBall();
    if (!whiteBall->IsInPocket()) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, whiteBall->GetPosition());
        RenderMeshColor(meshes["ball"], shaders["Color&Light"], 
                        modelMatrix, enumToColor(whiteBall->GetColor()));
    }

    // Render black ball if not pocketed
    Ball *blackBall = level->GetBlackBall();
    if (!blackBall->IsInPocket()) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, blackBall->GetPosition());
        RenderMeshColor(meshes["ball"], shaders["Color&Light"],
                        modelMatrix, enumToColor(blackBall->GetColor()));
    }

    // Render red balls if not pocketed
    Ball **redBalls = level->GetRedBalls();
    for (int i = 0; i < BALL_COLOR_NUMBER; ++i) {
        if (!redBalls[i]->IsInPocket()) {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, redBalls[i]->GetPosition());
            RenderMeshColor(meshes["ball"], shaders["Color&Light"],
                            modelMatrix, enumToColor(redBalls[i]->GetColor()));
        }
    }

    // Render yellow balls if not pocketed
    Ball **yellowBalls = level->GetYellowBalls();
    for (int i = 0; i < BALL_COLOR_NUMBER; ++i) {
        if (!yellowBalls[i]->IsInPocket()) {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, yellowBalls[i]->GetPosition());
            RenderMeshColor(meshes["ball"], shaders["Color&Light"],
                            modelMatrix, enumToColor(yellowBalls[i]->GetColor()));

        }
    }

    // Render cue if player has to hit ball
    Cue *cue = level->GetCue();
    if (currentLevelState == wantToHit) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, cue->GetPosition());
        modelMatrix = glm::rotate(modelMatrix, cue->GetAngle(), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(2), glm::vec3(1.0f, 0.0f, 0.0f));
        RenderMeshCue(meshes["cue"], shaders["CueShader"], modelMatrix,
                      cue->GetSwingTime(), cue->IsSwinging());
    }
}

void Pool::RenderMesh(Mesh * mesh, Shader * shader, glm::vec3 position, glm::vec3 scale) {
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glm::mat4 model(1);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
    mesh->Render();
}

void Pool::RenderMeshColor(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix,
                            const glm::vec3 &color) {
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light & material properties
    GLint light_position = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(light_position, 1, glm::value_ptr(lightPosition));

    // Set material property uniforms (shininess, kd, ks, object color)
    GLint material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    GLint material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    GLint material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    GLint object_pos = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(object_pos, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);

}

void Pool::RenderMeshCue(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix,
                          const float & time, const bool isActive) {
    if (!mesh || !shader || !shader->GetProgramID())
        return;
    
    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE,
                       glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE,
                       glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE,
                       glm::value_ptr(modelMatrix));

    // Set time uniform
    GLint time_aux = glGetUniformLocation(shader->program, "time");
    glUniform1f(time_aux, time);

    // Set if is active uniform
    GLboolean is_active_aux = glGetUniformLocation(shader->program, "isActive");
    glUniform1i(is_active_aux, isActive);
    mesh->Render();
}

void Pool::FrameEnd() {

}

/**
 * Redirect mouse and keyboard input to the level.
 */
void Pool::OnInputUpdate(float deltaTime, int mods) {
    for (std::vector<int>::iterator it = keysAllowedToPress.begin();
        it != keysAllowedToPress.end();
        ++it) {

        if (window->KeyHold(*it)) {
            level->OnKeyHold(*it);
        }
    }
}


void Pool::OnKeyPress(int key, int mods) {
    if (std::find(keysAllowedToPress.begin(), keysAllowedToPress.end(), key)
        != keysAllowedToPress.end()) {
        level->OnKeyPress(key);
    }
}

void Pool::OnKeyRelease(int key, int mods) {
    if (std::find(keysAllowedToPress.begin(), keysAllowedToPress.end(), key)
        != keysAllowedToPress.end()) {
        level->OnKeyRelease(key);
    }
}

void Pool::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    level->OnMouseMove(mouseX, mouseY, deltaX, deltaY);
}

void Pool::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    // La mine 2 = LMB si 3 = RMB, am incercat si alte mouse-uri, acelasi lucru
    if (button == LMB|| button == RMB) {
        level->OnMouseBtnPress(mouseX, mouseY, button);
    }
}

void Pool::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
    if (button == LMB || button == RMB) {
        level->OnMouseBtnRelease(mouseX, mouseY, button);
    }
}

void Pool::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {

}