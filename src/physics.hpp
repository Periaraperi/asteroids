#pragma once

#include <glm/glm.hpp>

struct AABB_Collider {
    glm::vec2 pos;
    glm::vec2 size;
};

// check if two axis aligned rectangles collide
bool aabb(const AABB_Collider& a, const AABB_Collider& b);
