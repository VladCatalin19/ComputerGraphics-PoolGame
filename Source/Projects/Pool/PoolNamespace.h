#pragma once

/**
 * This namespace contains the names of all the objects used in a scene. It is
 * used not to create conflicts with other classes with the same name.
 */
namespace PoolObjects {
    class Camera;

    class Entity;
    class Ball;
    class Cue;

    class Table;
    class Pocket;
    class Cushion;

    class Level;
    class Player;

    class State;
    class MovingWhiteBallState;
    class HittingState;
    class WantToHitState;
}