#pragma once

#include "State.h"

/**
 * This class implements the state where the player can move the white ball on
 * the table at the beginning of the game or when a fault is committed.
 */
class PoolObjects::MovingWhiteBallState : public PoolObjects::State {
public:
    MovingWhiteBallState();
    MovingWhiteBallState(Player **players, Player *currentPlayer, Ball *whiteBall,
                         Ball *blackBall, Ball **redBalls, Ball **yellowBalls,
                         Table *table, Cue *cue, PoolObjects::Camera *camera,
                         bool firstTime = false);
    ~MovingWhiteBallState();

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
    int ValidBallPosition(Ball *ball);

    bool firstTime;
    float minX;
    float minZ;
    float maxX;
    float maxZ;
    float movingStep;
};