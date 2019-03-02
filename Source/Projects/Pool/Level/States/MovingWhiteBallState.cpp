#include "MovingWhiteBallState.h"

using namespace PoolObjects;
using namespace std;

MovingWhiteBallState::MovingWhiteBallState() {}

MovingWhiteBallState::MovingWhiteBallState(Player **players, Player *currentPlayer,
    Ball *whiteBall, Ball *blackBall, Ball **redBalls, Ball **yellowBalls,
    Table *table, Cue *cue, PoolObjects::Camera *camera, bool firstTime)
        : State(players, currentPlayer, whiteBall, blackBall, redBalls,
                yellowBalls, table, cue, camera) {
    this->firstTime = firstTime;
    ChangeCameraTopDown();
    Init();
}

MovingWhiteBallState::~MovingWhiteBallState() {}

State::StateTypes MovingWhiteBallState::GetType() {
    return movingWhiteBall;
}

void MovingWhiteBallState::Init() {
    if (firstTime) {
        std::cout << "Player " << GetCurrentPlayer()->GetId() + 1
                  << " must move ball in first third of the table\n";
    } else {
        std::cout << "Player " << GetCurrentPlayer()->GetId() + 1
                  << " must move ball on the table\n";
    }

    GetWhiteBall()->SetPosition(WHITE_BALL_INIT_POS);

    minX = -1.53745f + GetWhiteBall()->GetRadius();
    minZ = -3.39406f + GetWhiteBall()->GetRadius();
    maxX = -minX;
    if (firstTime) {
        maxZ = -1.67588f;
    }
    else {
        maxZ = -minZ;
    }

    movingStep = 0.03f;
}

void MovingWhiteBallState::Update(float deltaTimeSeconds) {

}

void MovingWhiteBallState::OnKeyPress(int key) {

}

/**
 * Change ball's position when the WASD keys are pressed
 */
void MovingWhiteBallState::OnKeyHold(int key) {
    glm::vec3 ballPos = GetWhiteBall()->GetPosition();
    switch (key) {
    case GLFW_KEY_W:
        ballPos.x = min(max(ballPos.x + movingStep, minX), maxX);
        break;

    case GLFW_KEY_S:
        ballPos.x = min(max(ballPos.x - movingStep, minX), maxX);
        break;

    case GLFW_KEY_A:
        ballPos.z = min(max(ballPos.z - movingStep, minZ), maxZ);
        break;

    case GLFW_KEY_D:
        ballPos.z = min(max(ballPos.z + movingStep, minZ), maxZ);
        break;
    }
    GetWhiteBall()->SetPosition(ballPos);
}

/**
 * Verify if the white ball's position does not intersect other balls or if
 * it will fall into a pocket. If it is placed accordingly, the state will end
 * successfully.
 */
void MovingWhiteBallState::OnKeyRelease(int key) {
    if (key == GLFW_KEY_SPACE) {
        int rez = ValidBallPosition(GetWhiteBall());

        switch (rez) {
        case -1:
            cout << "Cannot place white ball here. It will fall in the pocket. ";
            cout << "Please choose other position.\n";
            break;

        case -2:
            cout << "Cannot place white ball here. It collides with other balls. ";
            cout << "Please choose other position.\n";
            break;

        case 1:
            EndSuccess();
            break;
        }
    }
}

void MovingWhiteBallState::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {

}

void MovingWhiteBallState::OnMouseBtnPress(int mouseX, int mouseY, int button) {

}

void MovingWhiteBallState::OnMouseBtnRelease(int mouseX, int mouseY, int button) {

}

/**
 * Verify if the white ball's position does not intersect other balls or if
 * it will fall into a pocket.
 * This function will return:
 *      -1 -> when ball will fall in a pocket
 *      -2 -> when ball collides with other balls
 *       1 -> when the position is valid
 */
int MovingWhiteBallState::ValidBallPosition(Ball* ball) {
    // Check if white ball is in hole
    if (BallEntersPockets(ball)) {
        return -1;
    }

    // Check if white ball collides with other balls
    if (!firstTime) {
        float rez = glm::length(GetWhiteBall()->GetPosition() - GetBlackBall()->GetPosition());
        if (rez <= GetWhiteBall()->GetRadius() * 2.0f) {
            return -2;
        }

        for (int i = 0; i < BALL_COLOR_NUMBER; ++i) {
            rez = glm::length(GetWhiteBall()->GetPosition() - GetRedBalls()[i]->GetPosition());
            if (!(GetRedBalls()[i]->IsInPocket()) && rez <= GetWhiteBall()->GetRadius() * 2.0f) {
                return -2;
            }

            rez = glm::length(GetWhiteBall()->GetPosition() - GetYellowBalls()[i]->GetPosition());
            if (!(GetYellowBalls()[i]->IsInPocket()) && rez <= GetWhiteBall()->GetRadius() * 2.0f) {
                return -2;
            }
        }
    }
    return 1;
}