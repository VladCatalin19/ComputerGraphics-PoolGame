#include "HittingState.h"

using namespace PoolObjects;

HittingState::HittingState() {}

HittingState::HittingState(Player **players, Player *currentPlayer, Ball *whiteBall,
                           Ball *blackBall, Ball **redBalls, Ball **yellowBalls,
                           Table *table, Cue *cue, PoolObjects::Camera *camera)
        : State(players, currentPlayer, whiteBall, blackBall, redBalls,
                yellowBalls, table, cue, camera) {
    Init();
}

HittingState::~HittingState() {}

State::StateTypes HittingState::GetType() {
    return hitting;
}

void HittingState::Init() {
    ChangeCameraTopDown();

    // Make array with all balls to make things easy
    allBalls = new Ball*[BALLS_NUMBER];
    allBalls[0] = GetWhiteBall();
    allBalls[1] = GetBlackBall();

    for (int i = 0; i < BALL_COLOR_NUMBER; ++i) {
        allBalls[i + 2] = GetRedBalls()[i];
        allBalls[i + 2 + BALL_COLOR_NUMBER] = GetYellowBalls()[i];
    }

    firstHitColor = Ball::BallColor::none;
    firstPocketedColor = Ball::BallColor::none;
    redBallsPocketed = 0;
    yellowBallsPocketed = 0;
}

/**
 * Find if there are balls which will collide with each other and reflect
 * them accordingly.
 */
void HittingState::CheckBallBallCollisionAndReflect(float deltaTimeSeconds) {
    float collideDist;
    float minDist;
    Ball* toCollide;

    for (int i = 0; i < BALLS_NUMBER - 1; ++i) {
        minDist = 1000.0f;
        toCollide = NULL;

        // Find closest ball which will collide with current ball in case of
        // speed vector being too big.
        for (int j = i + 1; j < BALLS_NUMBER; ++j) {
            collideDist = BallsCollide(allBalls[i], allBalls[j], deltaTimeSeconds);
            if (collideDist != -1.0f && collideDist < minDist) {
                minDist = collideDist;
                toCollide = allBalls[j];

                // Save the ball that hit the white ball to check if it was a fault
                if (i == 0 && firstHitColor == Ball::BallColor::none) {
                    firstHitColor = allBalls[j]->GetColor();
                }
            }
        }

        if (toCollide != NULL) {
            BallsReflect(allBalls[i], toCollide);
        }
    }
}

/**
 * Find if there are balls which will collide with cushions and reflect
 * them accordingly.
 */
void HittingState::CheckBallWallCollisionAndReflect(float deltaTimeSeconds) {
    for (int i = 0; i < BALLS_NUMBER; ++i) {
        if (allBalls[i]->GetSpeed() <= 0.0f) {
            continue;
        }
        BallWallsIntersecAndReflect(allBalls[i], deltaTimeSeconds);
    }
}

/**
 * Move balls on the table acoording to their speeds and check if they fall
 * into pockets.
 */
void HittingState::UpdateBalls(float deltaTimeSeconds) {
    for (int i = 0; i < BALLS_NUMBER; ++i) {

        if (BallEntersPockets(allBalls[i])) {
            if (!allBalls[i]->IsInPocket()) {
                switch (allBalls[i]->GetColor()) {
                case Ball::BallColor::red:
                    std::cout << "Red ball pocketed.\n";
                    ++redBallsPocketed;
                    if (firstPocketedColor == Ball::BallColor::none) {
                        firstPocketedColor = Ball::BallColor::red;
                    }
                    break;

                case Ball::BallColor::yellow:
                    std::cout << "Yellow ball pocketed.\n";
                    ++yellowBallsPocketed;
                    if (firstPocketedColor == Ball::BallColor::none) {
                        firstPocketedColor = Ball::BallColor::yellow;
                    }
                    break;

                case Ball::BallColor::white:
                    std::cout << "White ball pocketed.\n";
                    break;

                case Ball::BallColor::black:
                    std::cout << "Black ball pocketed.\n";
                    break;

                default:
                    break;
                }
            }

            allBalls[i]->SetIfInPocket(true);
            allBalls[i]->SetSpeed(0.0f);
        }

        glm::vec3 pos = allBalls[i]->GetDirection()
                      * (allBalls[i]->GetSpeed() * deltaTimeSeconds);
        pos += allBalls[i]->GetPosition();
        pos.y = 0.0f;
        allBalls[i]->SetPosition(pos);
        allBalls[i]->DecreaseSpeed(deltaTimeSeconds);
    }
}

void HittingState::UpdatePlayersBalls() {
    for (int i = 0; i < PLAYER_NUMBER; ++i) {
        if (GetPlayers()[i]->GetAssociatedColor() == Ball::BallColor::red) {
            GetPlayers()[i]->DecreaseRemainingBalls(redBallsPocketed);
        }
        else {
            GetPlayers()[i]->DecreaseRemainingBalls(yellowBallsPocketed);
        }
    }
}

void HittingState::Update(float deltaTimeSeconds) {
    CheckBallBallCollisionAndReflect(deltaTimeSeconds);
    CheckBallWallCollisionAndReflect(deltaTimeSeconds);

    UpdateBalls(deltaTimeSeconds);

    bool hasEnded = true;
    for (int i = 0; i < BALLS_NUMBER; ++i) {
        if (allBalls[i]->GetSpeed() != 0.0f) {
            hasEnded = false;
        }
    }

    // If all balls have stopped moving, decide if the move was a fault, failure,
    // or success
    if (hasEnded) {
        UpdatePlayersBalls();

        if (GetWhiteBall()->IsInPocket()) {
            std::cout << "Fault! White ball has been pocketed.\n";
            GetCurrentPlayer()->AddFault();
            EndFault();
            return;
        }

        if (firstHitColor == Ball::BallColor::none) {
            std::cout << "Fault! No ball has been hit.\n";
            GetCurrentPlayer()->AddFault();
            EndFault();
            return;
        }

        if (GetBlackBall()->IsInPocket() && GetCurrentPlayer()->GetRemainingBalls() > 1) {
            std::cout << "Fault! Black ball has not been pocketed last.\n";
            GetCurrentPlayer()->AddFault();
            EndFault();
            return;
        }

        if (GetCurrentPlayer()->GetAssociatedColor() == Ball::BallColor::none) {
            // Decide players' colors if first valid move
            if (firstPocketedColor != Ball::BallColor::none) {
                if (firstPocketedColor == Ball::BallColor::red) {
                    GetCurrentPlayer()->InitColorAndRemainingBalls(
                        Ball::BallColor::red,
                        BALL_COLOR_NUMBER + 1 - redBallsPocketed);

                    GetOtherPlayer()->InitColorAndRemainingBalls(
                        Ball::BallColor::yellow,
                        BALL_COLOR_NUMBER + 1 - yellowBallsPocketed);
                } else {
                    GetCurrentPlayer()->InitColorAndRemainingBalls(
                        Ball::BallColor::yellow,
                        BALL_COLOR_NUMBER + 1 - yellowBallsPocketed);

                    GetOtherPlayer()->InitColorAndRemainingBalls(
                        Ball::BallColor::red,
                        BALL_COLOR_NUMBER + 1 - redBallsPocketed);
                }
                EndSuccess();
                return;
            }
            EndFailure();
            return;

        } else if (GetCurrentPlayer()->GetAssociatedColor() != firstHitColor){
            std::cout << "Fault! Wrong color has been hit first.\n";
            GetCurrentPlayer()->AddFault();
            EndFault();
            return;
        } else {

            for (int i = 0; i < PLAYER_NUMBER; ++i) {
                if (GetPlayers()[i]->GetRemainingBalls() == 1) {
                    GetPlayers()[i]->SetAssociatedColor(Ball::BallColor::black);
                }
            }

            switch (GetCurrentPlayer()->GetAssociatedColor()) {
                case Ball::BallColor::red:
                    if (redBallsPocketed == 0) {
                        EndFailure();
                        return;
                    }
                    else {
                        EndSuccess();
                        return;
                    }
                    break;

                case Ball::BallColor::yellow:
                    if (yellowBallsPocketed == 0) {
                        EndFailure();
                        return;
                    }
                    else {
                        EndSuccess();
                        return;
                    }
                    break;

                case Ball::BallColor::black:
                    if (GetBlackBall()->IsInPocket()) {
                        std::cout << "Player "
                                  << GetCurrentPlayer()->GetId() + 1 << " won!\n";
                        EndSuccess();
                        return;
                    } else {
                        EndFailure();
                        return;
                    }
                    break;
            }

            EndSuccess();
            return;
        }
    }
}


void HittingState::OnKeyPress(int key) {

}

void HittingState::OnKeyHold(int key) {

}

void HittingState::OnKeyRelease(int key) {

}

void HittingState::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {

}

void HittingState::OnMouseBtnPress(int mouseX, int mouseY, int button) {

}

void HittingState::OnMouseBtnRelease(int mouseX, int mouseY, int button) {

}
