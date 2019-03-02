#include "Level.h"

#include <iostream>
using namespace std;

Level::Level() {}

Level::Level(PoolObjects::Camera *camera) : camera(camera) {
    InitTable();
    InitBalls();
    InitCue();
    InitPlayers();
    InitState();

    hasFinished = false;
}

Level::~Level() {
    Cushion **cushions = table->GetCushions();
    for (int i = 0; i < CUSHION_NUMBER; ++i) {
        delete cushions[i];
    }
    delete cushions;

    Pocket **pockets = table->GetPockets();
    for (int i = 0; i < POCKET_NUMBER; ++i) {
        delete pockets[i];
    }
    delete pockets;

    delete table;

    delete whiteBall;
    delete blackBall;
    for (int i = 0; i < BALL_COLOR_NUMBER; ++i) {
        delete redBalls[i];
        delete yellowBalls[i];
    }

    for (int i = 0; i < PLAYER_NUMBER; ++i) {
        delete players[i];
    }
}

void Level::PrintPlayerStats(Player *player) {
    cout << "Player " << player->GetId() + 1 << "\n"
         << "\t Color " << Ball::BallColorName(player->GetAssociatedColor()) << "\n";

    if (player->GetAssociatedColor() != Ball::BallColor::none) {
        cout << "\t Remaining balls " << player->GetRemainingBalls() << "\n";
    }

    cout << "\t Faults committed: " << player->GetFaults() << "\n";
}

void Level::PrintStats() {
    cout << endl;
    PrintPlayerStats(players[0]);
    PrintPlayerStats(players[1]);
    cout << "\n\n";
}

bool Level::HasFinished() {
    return hasFinished;
}

/**
 * Send update signal to the current state and check if it ended. If it ended
 * check if the the player made a fault, failed to pocket a ball, or succeeded
 * to pocket a ball. Based on that, change to an other state.
 */
void Level::Update(float deltaTimeSeconds) {
    if (state != NULL) {
        state->Update(deltaTimeSeconds);
    }

    State::StateTypes   stateType = state->GetType();
    State::EndingStatus endStatus = state->HasFinished();

    if (endStatus == State::EndingStatus::notEnded) {
        return;
    }

    if (blackBall->IsInPocket()) {
        hasFinished = true;
    }

    switch (state->GetType()) {
        case State::StateTypes::noState:
            break;

        case State::StateTypes::movingWhiteBall:
            if (endStatus == State::EndingStatus::success) {
                delete state;
                state = new WantToHitState(players, currentPlayer, whiteBall,
                                           blackBall, redBalls, yellowBalls,
                                           table, cue, camera);
            }
            break;

        case State::StateTypes::wantToHit:
            if (endStatus == State::EndingStatus::success) {
                delete state;
                state = new HittingState(players, currentPlayer, whiteBall,
                                         blackBall, redBalls, yellowBalls,
                                         table, cue, camera);
            }
            break;

        case State::StateTypes::hitting:
            switch (endStatus) {
                case State::EndingStatus::fault:
                    if (stateType == State::StateTypes::hitting) {
                        PrintStats();
                        delete state;
                        whiteBall->SetIfInPocket(false);
                        currentPlayer = players[!currentPlayer->GetId()];
                        state = new MovingWhiteBallState(
                            players, currentPlayer, whiteBall, blackBall,
                            redBalls, yellowBalls, table, cue, camera);
                    }
                    break;

                case State::EndingStatus::failure:
                    if (blackBall->IsInPocket()) {
                        delete state;
                        std::cout << "Restarting!\n";
                        currentPlayer = players[!currentPlayer->GetId()];
                        state = new MovingWhiteBallState(
                            players, currentPlayer, whiteBall, blackBall,
                            redBalls, yellowBalls, table, cue, camera);
                    }
                    else {
                        PrintStats();
                        delete state;
                        currentPlayer = players[!currentPlayer->GetId()];
                        state = new WantToHitState(
                            players, currentPlayer, whiteBall, blackBall,
                            redBalls, yellowBalls, table, cue, camera);
                    }
                    break;

                case State::EndingStatus::success:
                    PrintStats();
                    delete state;
                    if (currentPlayer->GetRemainingBalls() == 0) {
                        cout << "Player " << currentPlayer->GetId() + 1
                             << " won the game!\nRestarting!\n";
                        currentPlayer = players[!currentPlayer->GetId()];
                        state = new MovingWhiteBallState(
                            players, currentPlayer, whiteBall, blackBall,
                            redBalls, yellowBalls, table, cue, camera);
                    }
                    else {
                        state = new WantToHitState(
                            players, currentPlayer, whiteBall, blackBall,
                            redBalls, yellowBalls, table, cue, camera);
                    }
                    break;
            }
            break;
    }
    /*
    switch (state->HasFinished()) {
    case State::EndingStatus::notEnded:

        break;

    case State::EndingStatus::fault:
        // Fault - the other player can place the ball anywhere on the table
        if (stateType == State::StateTypes::hitting) {
            PrintStats();
            delete state;
            whiteBall->SetIfInPocket(false);
            currentPlayer = players[!currentPlayer->GetId()];
            state = new MovingWhiteBallState(players, currentPlayer, whiteBall,
                                             blackBall, redBalls, yellowBalls,
                                             table, cue, camera);
        }
        break;

    case State::EndingStatus::failure:
        // Failed to pocket a ball - the other player will hit the white ball
        if (stateType == State::StateTypes::hitting) {
            if (blackBall->IsInPocket()) {
                delete state;
                std::cout << "Restarting!\n";
                currentPlayer = players[!currentPlayer->GetId()];
                state = new MovingWhiteBallState(players, currentPlayer, whiteBall,
                                                 blackBall, redBalls, yellowBalls,
                                                 table, cue, camera, true);
            }
            else {
                PrintStats();
                delete state;
                currentPlayer = players[!currentPlayer->GetId()];
                state = new WantToHitState(players, currentPlayer, whiteBall, blackBall,
                                           redBalls, yellowBalls, table, cue, camera);
            }
        }
        break;

    case State::EndingStatus::success:
        // Succeed to pocket a ball - the same player will hit the white ball
        if (stateType == State::StateTypes::movingWhiteBall) {
            delete state;
            state = new WantToHitState(players, currentPlayer, whiteBall, blackBall,
                                       redBalls, yellowBalls, table, cue, camera);

        }
        else if (stateType == State::StateTypes::wantToHit) {
            delete state;
            state = new HittingState(players, currentPlayer, whiteBall, blackBall,
                                     redBalls, yellowBalls, table, cue, camera);

        }
        else if (stateType == State::StateTypes::hitting) {
            PrintStats();
            delete state;
            if (currentPlayer->GetRemainingBalls() == 0) {
                std::cout << "Player " << currentPlayer->GetId() + 1
                    << " won the game!\nRestarting!\n";
                currentPlayer = players[!currentPlayer->GetId()];
                state = new MovingWhiteBallState(players, currentPlayer, whiteBall,
                                                 blackBall, redBalls, yellowBalls,
                                                 table, cue, camera, true);
            }
            else {
                state = new WantToHitState(players, currentPlayer, whiteBall, blackBall,
                                           redBalls, yellowBalls, table, cue, camera);
            }
        }
        break;
    }
    */
}

/**
 * Redirect mouse and keyboard input to the current state.
 */
void Level::OnKeyPress(int key) {
    if (state != NULL) {
        state->OnKeyPress(key);
    }
}

void Level::OnKeyHold(int key) {
    if (state != NULL) {
        state->OnKeyHold(key);
    }
}

void Level::OnKeyRelease(int key) {
    if (state != NULL) {
        state->OnKeyRelease(key);
    }
}

void Level::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    if (state != NULL) {
        state->OnMouseMove(mouseX, mouseY, deltaX, deltaY);
    }
}

void Level::OnMouseBtnPress(int mouseX, int mouseY, int button) {
    if (state != NULL) {
        state->OnMouseBtnPress(mouseX, mouseY, button);
    }
}

void Level::OnMouseBtnRelease(int mouseX, int mouseY, int button) {
    if (state != NULL) {
        state->OnMouseBtnRelease(mouseX, mouseY, button);
    }
}

Table* Level::GetTable() {
    return table;
}

Ball* Level::GetWhiteBall() {
    return whiteBall;
}

Ball* Level::GetBlackBall() {
    return blackBall;
}

Ball** Level::GetRedBalls() {
    return redBalls;
}

Ball** Level::GetYellowBalls() {
    return yellowBalls;
}

Cue* Level::GetCue() {
    return cue;
}

State* Level::GetState() {
    return state;
}

Player* Level::GetCurrentPlayer() {
    return currentPlayer;
}

PoolObjects::Camera* Level::GetCamera() {
    return camera;
}

void Level::SetCamera(PoolObjects::Camera *camera) {
    this->camera = camera;
}

void Level::InitTable() {
    glm::vec3 tablePosition = glm::vec3(0, -2.18f, 0);

    Cushion **cushions = new Cushion*[CUSHION_NUMBER];

    cushions[0] = new Cushion(glm::vec3(-1.472f, 0.0f, -3.476f),
                              glm::vec3(-1.296f, 0.0f, -3.313f),
                              glm::vec3( 1.296f, 0.0f, -3.313f),
                              glm::vec3( 1.472f, 0.0f, -3.476f));

    cushions[1] = new Cushion(glm::vec3( 1.637f, 0.0f, -3.311f),
                              glm::vec3( 1.456f, 0.0f, -3.114f),
                              glm::vec3( 1.456f, 0.0f, -0.215f),
                              glm::vec3( 1.637f, 0.0f, -0.075f));

    cushions[2] = new Cushion(glm::vec3( 1.637f, 0.0f,  0.075f),
                              glm::vec3( 1.456f, 0.0f,  0.215f),
                              glm::vec3( 1.456f, 0.0f,  3.114f),
                              glm::vec3( 1.637f, 0.0f,  3.311f));

    cushions[3] = new Cushion(glm::vec3( 1.472f, 0.0f,  3.476f),
                              glm::vec3( 1.296f, 0.0f,  3.313f),
                              glm::vec3(-1.296f, 0.0f,  3.313f),
                              glm::vec3(-1.472f, 0.0f,  3.476f));

    cushions[4] = new Cushion(glm::vec3(-1.637f, 0.0f,  0.075f),
                              glm::vec3(-1.456f, 0.0f,  0.215f),
                              glm::vec3(-1.456f, 0.0f,  3.114f),
                              glm::vec3(-1.637f, 0.0f,  3.311f));

    cushions[5] = new Cushion(glm::vec3(-1.637f, 0.0f, -3.311f),
                              glm::vec3(-1.456f, 0.0f, -3.114f),
                              glm::vec3(-1.456f, 0.0f, -0.215f),
                              glm::vec3(-1.637f, 0.0f, -0.075f));

    Pocket **pockets = new Pocket*[POCKET_NUMBER];

    glm::vec3 P0 = glm::vec3(-1.64338f, 0.0f, -3.46148f);
    glm::vec3 P1 = glm::vec3( 1.64338f, 0.0f, -3.46148f);
    glm::vec3 P2 = glm::vec3( 1.65f,    0.0f,  0.0f);
    glm::vec3 P3 = glm::vec3( 1.64338f, 0.0f,  3.46148f);
    glm::vec3 P4 = glm::vec3(-1.64338f, 0.0f,  3.46148f);
    glm::vec3 P5 = glm::vec3(-1.65f,    0.0f,  0.0f);

    float xRadiusCenter = 0.184406f;
    float zRadiusCenter = 0.083455f;
    float xRadiusCorner = 0.227094f;
    float zRadiusCorner = 0.248181f;

    pockets[0] = new Pocket(P0, xRadiusCorner, zRadiusCorner);
    pockets[1] = new Pocket(P1, xRadiusCorner, zRadiusCorner);
    pockets[2] = new Pocket(P2, xRadiusCenter, zRadiusCenter);
    pockets[3] = new Pocket(P3, xRadiusCorner, zRadiusCorner);
    pockets[4] = new Pocket(P4, xRadiusCorner, zRadiusCorner);
    pockets[5] = new Pocket(P5, xRadiusCenter, zRadiusCenter);

    table = new Table(tablePosition, cushions, pockets);
}

void Level::InitBalls() {
    // White ball 
    Ball::BallColor color = Ball::BallColor::white;
    float radius = 0.081105f;
    whiteBall = new Ball(WHITE_BALL_INIT_POS, radius, color);

    // Black ball
    color = Ball::BallColor::black;
    blackBall = new Ball(BLACK_BALL_INIT_POS, radius, color);

    float spaceBetweenBalls = 0.0002f;
    // Red balls
    glm::vec3 ballPos[BALL_COLOR_NUMBER];
    color = Ball::BallColor::red;

    ballPos[0] = glm::vec3(BLACK_BALL_INIT_POS.x,
        0.0f,
        BLACK_BALL_INIT_POS.z - (radius * 4.0f + spaceBetweenBalls * 2.0f));

    ballPos[1] = glm::vec3(BLACK_BALL_INIT_POS.x - (radius + spaceBetweenBalls / 2.0f),
        0.0f,
        BLACK_BALL_INIT_POS.z - radius * 2.0f + spaceBetweenBalls);

    ballPos[2] = glm::vec3(BLACK_BALL_INIT_POS.x + radius * 2.0f + spaceBetweenBalls,
        0.0f,
        BLACK_BALL_INIT_POS.z);
    ballPos[3] = glm::vec3(BLACK_BALL_INIT_POS.x - (radius + spaceBetweenBalls / 2.0f),
        0.0f,
        BLACK_BALL_INIT_POS.z + (radius * 2.0f + spaceBetweenBalls));

    ballPos[4] = glm::vec3(BLACK_BALL_INIT_POS.x + (radius + spaceBetweenBalls / 2.0f),
        0.0f,
        BLACK_BALL_INIT_POS.z + (radius * 2.0f + spaceBetweenBalls));

    ballPos[5] = glm::vec3(BLACK_BALL_INIT_POS.x - radius * 2.0f + spaceBetweenBalls,
        0.0f,
        BLACK_BALL_INIT_POS.z + (radius * 4.0f + spaceBetweenBalls * 2.0f));

    ballPos[6] = glm::vec3(BLACK_BALL_INIT_POS.x + (radius * 4.0f + spaceBetweenBalls * 2.0f),
        0.0f,
        BLACK_BALL_INIT_POS.z + (radius * 4.0f + spaceBetweenBalls * 2.0f));

    for (int i = 0; i < BALL_COLOR_NUMBER; ++i) {
        redBalls[i] = new Ball(ballPos[i], radius, color);
    }

    // Yellow balls
    color = Ball::BallColor::yellow;
    ballPos[0] = glm::vec3(BLACK_BALL_INIT_POS.x + (radius + spaceBetweenBalls / 2.0f),
        0.0f,
        BLACK_BALL_INIT_POS.z - radius * 2.0f + spaceBetweenBalls);

    ballPos[1] = glm::vec3(BLACK_BALL_INIT_POS.x - (radius * 2.0f + spaceBetweenBalls),
        0.0f,
        BLACK_BALL_INIT_POS.z);
    ballPos[2] = glm::vec3(BLACK_BALL_INIT_POS.x - (radius * 3.0f + spaceBetweenBalls * 1.5f),
        0.0f,
        BLACK_BALL_INIT_POS.z + (radius * 2.0f + spaceBetweenBalls));

    ballPos[3] = glm::vec3(BLACK_BALL_INIT_POS.x + (radius * 3.0f + spaceBetweenBalls * 1.5f),
        0.0f,
        BLACK_BALL_INIT_POS.z + (radius * 2.0f + spaceBetweenBalls));

    ballPos[4] = glm::vec3(BLACK_BALL_INIT_POS.x - (radius * 4.0f + spaceBetweenBalls * 2.0f),
        0.0f,
        BLACK_BALL_INIT_POS.z + (radius * 4.0f + spaceBetweenBalls * 2.0f));

    ballPos[5] = glm::vec3(BLACK_BALL_INIT_POS.x,
        0.0f,
        BLACK_BALL_INIT_POS.z + (radius * 4.0f + spaceBetweenBalls * 2.0f));

    ballPos[6] = glm::vec3(BLACK_BALL_INIT_POS.x + radius * 2.0f + spaceBetweenBalls,
        0.0f,
        BLACK_BALL_INIT_POS.z + (radius * 4.0f + spaceBetweenBalls * 2.0f));

    for (int i = 0; i < BALL_COLOR_NUMBER; ++i) {
        yellowBalls[i] = new Ball(ballPos[i], radius, color);
    }
}

void Level::InitCue() {
    cue = new Cue(glm::vec3(0.0f, 0.0f, 0.0f), 4.6f);
}

void Level::InitState() {
    state = new MovingWhiteBallState(players, players[0], whiteBall, blackBall,
                                     redBalls, yellowBalls, table, cue,
                                     camera, true);
}

void Level::InitPlayers() {
    for (int i = 0; i < PLAYER_NUMBER; ++i) {
        players[i] = new Player(i);
    }

    currentPlayer = players[0];
}