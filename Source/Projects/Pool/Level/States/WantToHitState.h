#pragma once

#include "State.h"
#include <cmath>

/**
 * This class implemetns the state where the player moves the cue around the
 * white ball and hits it.
 */
class PoolObjects::WantToHitState : public PoolObjects::State {
public:
    WantToHitState();
    WantToHitState(Player **players, Player *currentPlayer, Ball *whiteBall,
                   Ball *blackBall, Ball **redBalls, Ball **yellowBalls,
                   Table *table, Cue *cue, PoolObjects::Camera *camera);
    ~WantToHitState();

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
    bool LMBHold;
    bool RMBHold;
};