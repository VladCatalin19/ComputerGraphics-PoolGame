#include "Player.h"

using namespace PoolObjects;

Player::Player() {}

Player::Player(int id) : id(id), associatedColor(Ball::BallColor::none),
    remainingBalls(-1), faults(0) {}

Player::~Player() {}

int Player::GetId() {
    return id;
}

Ball::BallColor Player::GetAssociatedColor() {
    return associatedColor;
}

void Player::SetAssociatedColor(Ball::BallColor color) {
    associatedColor = color;
}

int Player::GetRemainingBalls() {
    return remainingBalls;
}

void Player::InitColorAndRemainingBalls(Ball::BallColor color, int remainingBalls) {
    this->associatedColor = color;
    this->remainingBalls = remainingBalls;
}

void Player::DecreaseRemainingBalls(int balls) {
    remainingBalls -= balls;
}

int Player::GetFaults() {
    return faults;
}

void Player::AddFault() {
    ++faults;
}