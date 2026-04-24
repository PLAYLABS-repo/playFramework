#include "Entity.h"
#include "Sprite.h"
#include "Shape.h"
#include "TimelineAnimator.h"

#include <cmath>

// =========================
// CONSTRUCTOR
// =========================
Entity::Entity()
{
    parent = nullptr;
}

// =========================
// DESTRUCTOR
// =========================
Entity::~Entity()
{
    for (auto* c : children)
        delete c;
}

// =========================
// ADD CHILD
// =========================
void Entity::addChild(Entity* e)
{
    e->parent = this;
    children.push_back(e);
}

// =========================
// UPDATE
// =========================
void Entity::update(float dt)
{
    if (sprite) sprite->update(dt);
    if (anim) anim->update(dt);

    // =========================
    // UPDATE AABB (WORLD SPACE)
    // =========================
    if (hasAABB)
    {
        worldBox = AABB(
            position.x + localBox.x,
            position.y + localBox.y,
            localBox.w * scale.x,
            localBox.h * scale.y
        );
    }

    for (auto* c : children)
        c->update(dt);
}

// =========================
// DRAW (HIERARCHY TRANSFORM)
// =========================
void Entity::draw(Camera& cam,
                  Vec2 pPos,
                  float pRot,
                  Vec2 pScale)
{
    float rad = pRot * 3.14159265f / 180.0f;

    float cosR = cosf(rad);
    float sinR = sinf(rad);

    Vec2 worldPos = {
        pPos.x + cosR * position.x * pScale.x - sinR * position.y * pScale.y,
        pPos.y + sinR * position.x * pScale.x + cosR * position.y * pScale.y
    };

    float worldRot = pRot + rotation;

    Vec2 worldScale = {
        pScale.x * scale.x,
        pScale.y * scale.y
    };

    // =========================
    // SPRITE
    // =========================
    if (sprite)
    {
        sprite->position = worldPos;
        sprite->rotation = worldRot;
        sprite->scale    = worldScale;
        sprite->draw(cam);
    }

    // =========================
    // SHAPE
    // =========================
    if (shape)
    {
        shape->x = worldPos.x;
        shape->y = worldPos.y;
        shape->draw();
    }

    // =========================
    // ANIMATION
    // =========================
    if (anim)
    {
        anim->draw(&anim->texture, &anim->atlas, cam, worldPos);
    }

    // =========================
    // CHILDREN
    // =========================
    for (auto* c : children)
        c->draw(cam, worldPos, worldRot, worldScale);
}
