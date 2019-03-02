#pragma once

#include "State.h"

/**
 * This class implements the state when the balls will collide with each other
 * or with the cushions or will fall into pockets
 */
class PoolObjects::HittingState : public PoolObjects::State {
public:
    HittingState();
    HittingState(Player** players, Player *currentPlayer, Ball *whiteBall,
                 Ball *blackBall, Ball **redBalls, Ball **yellowBalls,
                 Table *table, Cue *cue, PoolObjects::Camera *camera);
    ~HittingState();

    StateTypes GetType() override;
    void Init() override;
    void Update(float deltaTimeSeconds) override;
    void OnKeyPress(int key) override;
    void OnKeyHold(int key) override;
    void OnKeyRelease(int key) override;
    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
    void OnMouseBtnPress(int mouseX, int mouseY, int button) override;
    void OnMouseBtnRelease(int mouseX, int mouseY, int button) override;

private:
    Ball **allBalls;
    Ball::BallColor firstHitColor;
    Ball::BallColor firstPocketedColor;
    int redBallsPocketed;
    int yellowBallsPocketed;

    void CheckBallBallCollisionAndReflect(float deltaTimeSeconds);
    void CheckBallWallCollisionAndReflect(float deltaTimeSeconds);

    void UpdatePlayersBalls();

    void UpdateBalls(float deltaTimeSeconds);
};