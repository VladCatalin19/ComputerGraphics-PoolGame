#pragma once

#include <Core/World.h>
#include <Projects/Pool/Entities/Ball.h>
#include <Projects/Pool/PoolNamespace.h>

/**
 * This class implemets the player of the game. A player has a unique ID number,
 * an associated color of the balls the player must pocket, the number of the
 * balls needed to be pocketed and the number of faults the player committed.
 */
class PoolObjects::Player {
private:
    int id;
    Ball::BallColor associatedColor;
    int remainingBalls;
    int faults;

public:
    Player();
    Player(int id);
    ~Player();

    int GetId();
    Ball::BallColor GetAssociatedColor();
    void SetAssociatedColor(Ball::BallColor color);
    int GetRemainingBalls();
    void InitColorAndRemainingBalls(Ball::BallColor color, int remainingBalls);
    void DecreaseRemainingBalls(int balls);
    int GetFaults();
    void AddFault();
};