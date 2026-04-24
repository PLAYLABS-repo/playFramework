#pragma once

#include <vector>
#include "Vec2.h"
#include "AABB.h"
#include "Camera.h"

class Sprite;
class Shape;
class TimelineAnimator;

class Entity
{
public:
    Entity();
    ~Entity();

    // =========================
    // TRANSFORM
    // =========================
    Vec2 position = {0, 0};
    Vec2 scale    = {1, 1};
    float rotation = 0.0f;

    // =========================
    // PARENTING
    // =========================
    Entity* parent = nullptr;
    std::vector<Entity*> children;

    void addChild(Entity* e);

    // =========================
    // OPTIONAL ATTACHMENTS
    // =========================
    Sprite* sprite = nullptr;
    Shape* shape = nullptr;
    TimelineAnimator* anim = nullptr;

    // =========================
    // COLLISION
    // =========================
    bool hasAABB = false;
    AABB localBox;
    AABB worldBox;

    // =========================
    // CORE
    // =========================
    void update(float dt);
    void draw(Camera& cam,
              Vec2 parentPos = {0,0},
              float parentRot = 0.0f,
              Vec2 parentScale = {1,1});
};
