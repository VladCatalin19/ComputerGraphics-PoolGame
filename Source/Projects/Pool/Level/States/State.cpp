#include "State.h"

#include <iostream>
using namespace std;

using namespace PoolObjects;

State::State() {}

State::State(Player** players, Player *currentPlayer, Ball *whiteBall,
             Ball *blackBall, Ball **redBalls, Ball **yellowBalls, Table *table,
             Cue *cue, PoolObjects::Camera *camera) {

    this->players = players;
    this->currentPlayer = currentPlayer;
    this->whiteBall = whiteBall;
    this->blackBall = blackBall;
    this->redBalls = redBalls;
    this->yellowBalls = yellowBalls;
    this->table = table;
    this->cue = cue;
    this->camera = camera;
    hasFinished = notEnded;
}

State::~State() {}

Player** State::GetPlayers() {
    return players;
}

Player* State::GetCurrentPlayer() {
    return currentPlayer;
}

Player* State::GetOtherPlayer() {
    return players[!currentPlayer->GetId()];
}

Ball* State::GetWhiteBall() {
    return whiteBall;
}

Ball* State::GetBlackBall() {
    return blackBall;
}

Ball** State::GetRedBalls() {
    return redBalls;
}

Ball** State::GetYellowBalls() {
    return yellowBalls;
}

Table* State::GetTable() {
    return table;
}

Cue* State::GetCue() {
    return cue;
}

PoolObjects::Camera* State::GetCamera() {
    return camera;
}

State::EndingStatus State::HasFinished() {
    return hasFinished;
}

void State::Init() {

}

void State::Update(float deltaTimeSeconds) {

}

void State::OnKeyPress(int key) {

}

void State::OnKeyHold(int key) {

}

void State::OnKeyRelease(int key) {

}

void State::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {

}

void State::OnMouseBtnPress(int mouseX, int mouseY, int button) {

}

void State::OnMouseBtnRelease(int mouseX, int mouseY, int button) {

}

void State::EndSuccess() {
    hasFinished = success;
}

void State::EndFailure() {
    hasFinished = failure;
}

void State::EndFault() {
    hasFinished = fault;
}

State::StateTypes State::GetType() {
    return noState;
}

void State::ChangeCameraTopDown() {
    camera->SetPositionAndRotation(glm::vec3(0.0f, 3.4f, 0.0f),
                                   glm::vec3(RADIANS(-90), RADIANS(-90), 0.0f));
}

/**
 * Places the camera on the direction made by the white and black balls.
 * The camera will be at a certain distance from the white ball facing the
 * direction described above.
 * The function will also place the cue under the camera.
 */
void State::ChangeCameraThirdPerson() {
    glm::vec3 whiteBallPos = GetWhiteBall()->GetPosition();
    glm::vec3 blackBallPos = GetBlackBall()->GetPosition();
    // Temporary vector with the direction between the white and black balls
    glm::vec3 P = glm::normalize(whiteBallPos - blackBallPos);

    float distCameraBall = GetWhiteBall()->GetRadius() * 10.0f;

    // Calculate cue position and rotation
    glm::vec3 cuePos = whiteBallPos + P * cue->GetLength();
    cuePos.y = GetWhiteBall()->GetRadius() * 2.0f;
    // Angle between cue and OZ axis
    float angle = atan2(blackBallPos.x - whiteBallPos.x,
                        blackBallPos.z - whiteBallPos.z);

    // Calculate camera position and rotation
    glm::vec3 cameraPos = whiteBallPos + P * distCameraBall;
    cameraPos.y = GetWhiteBall()->GetRadius() * 6.0f;
    glm::vec3 cameraRot = glm::vec3(RADIANS(-30), M_PI + angle, 0.0f);

    // Set cue position and rotation
    GetCue()->SetPosition(cuePos);
    GetCue()->SetAngle(angle);

    // Set camera position and rotation
    camera->SetPositionAndRotation(cameraPos, cameraRot,
                                   glm::length(cameraPos - whiteBallPos));
}

/**
 * Check whether a point is inside an ellipse. Used to detect if a ball is inside
 * a pocket.
 * Algorith taken from: https://math.stackexchange.com/a/76463
 */
bool State::IsPointInEllipse(glm::vec3 point, Pocket *pocket) {
    float frac1 = ((point.z - pocket->GetPosition().z) 
                    * (point.z - pocket->GetPosition().z)) / 
                  (pocket->GetZRadius() * pocket->GetZRadius());
    float frac2 = ((point.x - pocket->GetPosition().x)
                    * (point.x - pocket->GetPosition().x)) /
                  (pocket->GetXRadius() * pocket->GetXRadius());

    return frac1 + frac2 <= 1.0f;
}

bool State::BallEntersPockets(Ball* ball) {

    for (int i = 0; i < POCKET_NUMBER; ++i) {
        if (IsPointInEllipse(ball->GetPosition(), GetTable()->GetPockets()[i])) {
            return true;
        }
    }
    return false;
}

/**
 * Check if two moving balls will collide.
 * Algorithm taken from: 
 *  http://www.gamasutra.com/view/feature/3015/pool_hall_lessons_fast_accurate_.php
 */
float State::BallsCollide(Ball *b1, Ball *b2, float deltaTimeSeconds) {
    if (b1 == b2) {
        return -1.0f;
    }

    if (b1->IsInPocket() || b2->IsInPocket()) {
        return -1.0f;
    }

    // Movement relative to b2
    glm::vec3 moveVec = b1->GetDirection() * b1->GetSpeed() * deltaTimeSeconds
                      - b2->GetDirection() * b2->GetSpeed() * deltaTimeSeconds;
    moveVec.y = 0.0f;

    // Early Escape test: if the length of the movevec is less
    // than distance between the centers of these circles minus 
    // their radii, there's no way they can hit. 
    float dist = glm::distance(b1->GetPosition(), b2->GetPosition());
    float sumRadii = b1->GetRadius() + b2->GetRadius();
    dist -= sumRadii;
    if (glm::length(moveVec) < dist) {
        return -1.0f;
    }

    // Normalize the movevec
    glm::vec3 N = moveVec;
    N = glm::normalize(N);

    // Find C, the vector from the center of the moving 
    // circle A to the center of B
    glm::vec3 C = b2->GetPosition() - b1->GetPosition();

    // D = N . C = ||C|| * cos(angle between N and C)
    double D = glm::dot(N, C);

    // Another early escape: Make sure that A is moving 
    // towards B! If the dot product between the movevec and 
    // B.center - A.center is less that or equal to 0, 
    // A isn't isn't moving towards B
    if (D <= 0.0) {
        return -1.0f;
    }

    // Find the length of the vector C
    double lengthC = glm::length(C);

    double F = (lengthC * lengthC) - (D * D);

    // Escape test: if the closest that A will get to B 
    // is more than the sum of their radii, there's no 
    // way they are going collide
    double sumRadiiSquared = sumRadii * sumRadii;
    if (F >= sumRadiiSquared) {
        return -1.0f;
    }

    // We now have F and sumRadii, two sides of a right triangle. 
    // Use these to find the third side, sqrt(T)
    double T = sumRadiiSquared - F;

    // If there is no such right triangle with sides length of 
    // sumRadii and sqrt(f), T will probably be less than 0. 
    // Better to check now than perform a square root of a 
    // negative number. 
    if (T < 0) {
        return -1.0f;
    }

    // Therefore the distance the circle has to travel along 
    // movevec is D - sqrt(T)
    double distance = D - sqrt(T);

    // Get the magnitude of the movement vector
    double mag = glm::length(moveVec);

    // Finally, make sure that the distance A has to move 
    // to touch B is not greater than the magnitude of the 
    // movement vector.
    if (mag < distance) {
        return -1.0f;
    }

    // Set the length of the movevec so that the circles will just 
    // touch
    moveVec = glm::normalize(moveVec);
    moveVec = moveVec * (float)(distance);

    return (float)(distance);
}

void State::BallsReflect(Ball *b1, Ball *b2) {
    // First, find the normalized vector n from the center of 
    // circle1 to the center of circle2
    glm::vec3 v1 = b1->GetDirection() * b1->GetSpeed();
    v1.y = 0.0f;
    glm::vec3 v2 = b2->GetDirection() * b2->GetSpeed();
    v2.y = 0.0f;
    glm::vec3 n = (n == glm::vec3(0.0f, 0.0f, 0.0f)) ? glm::vec3(0.0f, 0.0f, 0.0f)
                : glm::normalize(b1->GetPosition() - b2->GetPosition());
    // Find the length of the component of each of the movement
    // vectors along n. 
    // a1 = v1 . n
    // a2 = v2 . n
    float a1 = glm::dot(v1, n);
    //cout << "Dot 1: " << a1 << endl;
    float a2 = glm::dot(v2, n);
    //cout << "Dot 2: " << a2 << endl;

    // Using the optimized version, 
    //               2(a1 - a2)
    // optimizedP = -----------
    //                m1 + m2
    float optimizedP = (a1 - a2);

    // Calculate v1', the new movement vector of circle1
    // v1' = v1 - optimizedP * m2 * n
    glm::vec3 v1p = v1 - optimizedP * n;
    v1p.y = 0.0f;
    //cout << "Ball 1 v1p: " << v1p << endl;

    // Calculate v2', the new movement vector of circle1
    // v2' = v2 + optimizedP * m1 * n
    glm::vec3 v2p = v2 + optimizedP * n;
    v2p.y = 0.0f;
    //cout << "Ball 2 v1p: " << v2p << endl;

    b1->SetSpeed(glm::length(v1p));
    v1p = (v1p == glm::vec3(0.0f, 0.0f, 0.0f)) ? glm::vec3(0.0f, 0.0f, 0.0f) 
                                               : glm::normalize(v1p);
    v1p.y = 0.0f;
    b1->SetDirection(v1p);
    //cout << "Set ball1 direction: " << v1p << endl;

    b2->SetSpeed(glm::length(v2p));
    v2p = (v2p == glm::vec3(0.0f, 0.0f, 0.0f)) ? glm::vec3(0.0f, 0.0f, 0.0f)
                                               : glm::normalize(v2p);
    v2p.y = 0.0f;
    b2->SetDirection(v2p);
    //cout << "Set ball2 direction: " << v2p << endl << endl;
}

/**
 * Given three colinear points p, q, r, the function checks if point q lies
 * on line segment 'pr'
 */
bool State::OnSegment(glm::vec3 p, glm::vec3 q, glm::vec3 r) {
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
        q.z <= std::max(p.z, r.z) && q.z >= std::min(p.z, r.z)) {
        return true;
    }
    return false;
}

/**
 * Find orientation of ordered triplet (p, q, r). 
 * The function returns following values 
 *      0 --> p, q and r are colinear 
 *      1 --> Clockwise 
 *      2 --> Counterclockwise 
 * See https://www.geeksforgeeks.org/orientation-3-ordered-points/
 * for details of below formula. 
 */
int State::Orientation(glm::vec3 p, glm::vec3 q, glm::vec3 r) {
    float val = (q.z - p.z) * (r.x - q.x) - (q.x - p.x) * (r.z - q.z);
    if (val == 0.0f) {
        return 0;  // colinear 
    }
    return (val > 0.0f) ? 1 : 2; // clock or counterclock wise 
}
/**
 * Check whether two segments [p1q1] and [p2q2] intersect.
 * Algorithm taken from:
 *  http://www.gamasutra.com/view/feature/3015/pool_hall_lessons_fast_accurate_.php
 */
bool State::DoIntersect(glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2)
{
    // Find the four orientations needed for general and 
    // special cases 
    int o1 = Orientation(p1, q1, p2);
    int o2 = Orientation(p1, q1, q2);
    int o3 = Orientation(p2, q2, p1);
    int o4 = Orientation(p2, q2, q1);

    // General case 
    if (o1 != o2 && o3 != o4) {
        return true;
    }

    // Special Cases 
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && OnSegment(p1, p2, q1)) {
        return true;
    }

    // p1, q1 and q2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && OnSegment(p1, q2, q1)) {
        return true;
    }

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && OnSegment(p2, p1, q2)) {
        return true;
    }

    // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && OnSegment(p2, q1, q2)) {
        return true;
    }

    return false; // Doesn't fall in any of the above cases 
}

/**
 * Check if a ball will collide with one of the cushions and, if true,
 * reflect ball.
 */
void State::BallWallsIntersecAndReflect(Ball *ball, float deltaTimeSeconds) {
    // Position of the ball in the next frame if it would not collide
    glm::vec3 afterMove = ball->GetPosition() + ball->GetDirection()
                                              * ball->GetSpeed()
                                              * deltaTimeSeconds;
    // Check collision with all cushions
    for (int i = 0; i < CUSHION_NUMBER; ++i) {
        glm::vec3* points = GetTable()->GetCushions()[i]->GetPointsArray();
        for (int j = 0; j < POINTS_NUMBER - 1; ++j) {

            // Reflect ball if the segment between the current position of the
            // ball and the next position intersect one of the margin of the cushion
            if (DoIntersect(ball->GetPosition(), afterMove,
                            points[j], points[j + 1])) {
                // Reflection taken from : https://math.stackexchange.com/a/13263
                glm::vec3 d = ball->GetDirection();
                glm::vec3 n = GetTable()->GetCushions()[i]->GetNormals()[j];    // already normalized
                glm::vec3 r = d - 2.0f * glm::dot(d, n) * n;

                r.y = 0.0f;

                ball->SetDirection(r);
                return;
            }
        }
    }
}