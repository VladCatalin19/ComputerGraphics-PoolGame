#pragma once

#include <Core/Engine.h>

#include <Projects/Pool/PoolNamespace.h>
#include <Projects/Pool/Level/Player.h>
#include <Projects/Pool/Entities/Ball.h>
#include <Projects/Pool/Entities/Table.h>
#include <Projects/Pool/Entities/Cue.h>
#include <Projects/Pool/Entities/Pocket.h>
#include <Projects/Pool/Entities/Cushion.h>

#include <Projects/Pool/Camera.h>
#include <Projects/Pool/Constants.h>

#include <iostream>
#include <cmath>

/**
 * This abstract class implements the basic functionality of a state. It provides
 * the objects from the scene and algorithms used to detect collisions.
 */
class PoolObjects::State {
public:
    State();
    State(Player** players, Player *currentPlayer, Ball *whiteBall,
          Ball *blackBall, Ball **redBalls, Ball **yellowBalls, Table *table,
          Cue *cue, PoolObjects::Camera *camera);
    ~State();

    enum EndingStatus {notEnded, success, failure, fault};
    enum StateTypes {noState, hitting, movingWhiteBall, wantToHit};

    Player** GetPlayers();
    Player* GetCurrentPlayer();
    Player* GetOtherPlayer();
    Ball* GetWhiteBall();
    Ball* GetBlackBall();
    Ball** GetRedBalls();
    Ball** GetYellowBalls();
    Table* GetTable();
    Cue* GetCue();
    PoolObjects::Camera* GetCamera();
    EndingStatus HasFinished();
    virtual StateTypes GetType();

    virtual void Init();
    virtual void Update(float deltaTimeSeconds);
    virtual void OnKeyPress(int key);
    virtual void OnKeyHold(int key);
    virtual void OnKeyRelease(int key);
    virtual void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
    virtual void OnMouseBtnPress(int mouseX, int mouseY, int button);
    virtual void OnMouseBtnRelease(int mouseX, int mouseY, int button);
    void EndSuccess();
    void EndFailure();
    void EndFault();

    void ChangeCameraTopDown();
    void ChangeCameraThirdPerson();

    bool BallEntersPockets(Ball* ball);
    float BallsCollide(Ball *b1, Ball *b2, float deltaTimeSeconds = 0.0f);
    void BallsReflect(Ball *b1, Ball *b2);

    void BallWallsIntersecAndReflect(Ball *ball, float deltaTimeSeconds);

private:
    bool IsPointInEllipse(glm::vec3 point, Pocket *pocket);
    
    int Orientation(glm::vec3 p, glm::vec3 q, glm::vec3 r);
    bool OnSegment(glm::vec3 p, glm::vec3 q, glm::vec3 r);
    bool DoIntersect(glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2);

    Player **players;
    Player *currentPlayer;
    Ball *whiteBall;
    Ball *blackBall;
    Ball **redBalls;
    Ball **yellowBalls;
    Table *table;
    Cue *cue;

    PoolObjects::Camera *camera;

    EndingStatus hasFinished;
};