#pragma once

#include <Core/Engine.h>

#include <Projects/Pool/Entities/Entity.h>
#include <Projects/Pool/Entities/Ball.h>
#include <Projects/Pool/Entities/Cue.h>
#include <Projects/Pool/Entities/Table.h>
#include <Projects/Pool/Entities/Cushion.h>
#include <Projects/Pool/Entities/Pocket.h>

#include "Player.h"

#include "States/State.h"
#include "States/MovingWhiteBallState.h"
#include "States/WantToHitState.h"
#include "States/HittingState.h"

#include <Projects/Pool/PoolNamespace.h>

#include <Projects/Pool/Constants.h>

using namespace PoolObjects;

/**
 * This class implements the logic of a level. The level consists of three
 * possible states. The state of moving the white ball on the table, the
 * state of adjusting the cue and the force that will be applied to the
 * white ball, and the state where the balls are moving anc colliding with
 * each other, with the cushions, or are pocketed.
 * The level will switch states bases on how the states evolve and finish.
 */
class PoolObjects::Level {
private:
    Table *table;

    Ball *whiteBall;

    Ball *redBalls[BALL_COLOR_NUMBER];
    Ball *yellowBalls[BALL_COLOR_NUMBER];

    Ball *blackBall;

    Cue *cue;

    State *state;

    Player *players[PLAYER_NUMBER];
    Player *currentPlayer;

    PoolObjects::Camera *camera;

    bool hasFinished;

public:
    Level();
    Level(PoolObjects::Camera *camera);
    ~Level();

    bool HasFinished();

    void Update(float deltaTimeSeconds);

    void OnKeyPress(int key);
    void OnKeyHold(int key);
    void OnKeyRelease(int key);
    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
    void OnMouseBtnPress(int mouseX, int mouseY, int button);
    void OnMouseBtnRelease(int mouseX, int mouseY, int button);

    Table* GetTable();
    Ball* GetWhiteBall();
    Ball* GetBlackBall();
    Ball** GetRedBalls();
    Ball** GetYellowBalls();
    Cue* GetCue();
    State* GetState();
    Player* GetCurrentPlayer();
    PoolObjects::Camera* GetCamera();
    void SetCamera(PoolObjects::Camera *camera);

private:
    void PrintPlayerStats(Player *player);
    void PrintStats();

    void InitTable();
    void InitBalls();
    void InitCue();
    void InitState();
    void InitPlayers();
};