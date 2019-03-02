#include "WantToHitState.h"

using namespace PoolObjects;

WantToHitState::WantToHitState() {}

WantToHitState::~WantToHitState() {}

WantToHitState::WantToHitState(Player **players, Player *currentPlayer,
                               Ball *whiteBall, Ball *blackBall,
                               Ball **redBalls, Ball **yellowBalls,
                               Table *table, Cue *cue, PoolObjects::Camera *camera)
    : State(players, currentPlayer, whiteBall, blackBall, redBalls, yellowBalls,
            table, cue, camera) {

    ChangeCameraThirdPerson();
    Init();
}

State::StateTypes WantToHitState::GetType() {
    return wantToHit;
}

void WantToHitState::Init() {
    GetCue()->ResetSwingTime();
    std::cout << "Player " << GetCurrentPlayer()->GetId() + 1 << " must hit "
              << Ball::BallColorName(GetCurrentPlayer()->GetAssociatedColor()) << " ball.\n";
}

void WantToHitState::Update(float deltaTimeSeconds) {
    if (GetCue()->IsSwinging()) {
        GetCue()->AddSwingTime(deltaTimeSeconds);
    }
}

void WantToHitState::OnKeyPress(int key) {

}

void WantToHitState::OnKeyHold(int key) {

}

void WantToHitState::OnKeyRelease(int key) {

}

void WantToHitState::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    float sensivityOX = 0.001f;
    float sensivityOY = 0.001f;
    // Move camera around white ball and reposition cue
    if (RMBHold) {
        GetCamera()->RotateThirdPerson_OX(sensivityOX * -deltaY);
        GetCamera()->RotateThirdPerson_OY(sensivityOY * -deltaX);

        glm::vec3 whiteBallPos = GetWhiteBall()->GetPosition();
        glm::vec3 cameraPos = GetCamera()->GetPosition();
        glm::vec3 cuePos = GetCue()->GetPosition();

        glm::vec3 P = cameraPos - whiteBallPos;
        P.y = 0.0f;
        // If camera's projection on XOZ plane is very close to the projection of
        // white ball's projection on XOZ, normalization will fail and cue's 
        // position will go wild
        if (glm::length(P) < 0.25f) {
            P = cuePos;
        } else {
            P = whiteBallPos + glm::normalize(P) * GetCue()->GetLength();
        }
 
        GetCue()->SetPosition(glm::vec3(P.x, cuePos.y, P.z));
        GetCue()->SetAngle(atan2(whiteBallPos.x - P.x, whiteBallPos.z - P.z));
    }
}

void WantToHitState::OnMouseBtnPress(int mouseX, int mouseY, int button) {
    switch (button) {
    case LMB:
        LMBHold = true;
        GetCue()->StartSwinging();
        break;

    case RMB:
        RMBHold = true;
        break;
    }
}

void WantToHitState::OnMouseBtnRelease(int mouseX, int mouseY, int button) {
    switch (button) {
    case LMB: {
        // Apply force to white ball
        LMBHold = false;
        GetCue()->StopSwinging();
        float force = -(cos(2.0f * GetCue()->GetSwingTime()) - 1.0f) * 5.0f;

        glm::vec3 direction = glm::normalize(GetWhiteBall()->GetPosition()
                                            - GetCue()->GetPosition());

        GetWhiteBall()->SetSpeed(force);
        GetWhiteBall()->SetDirection(direction);
        EndSuccess();
        break;
    }

    case RMB:
        RMBHold = false;
        break;
    }
}
