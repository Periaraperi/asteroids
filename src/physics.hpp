#pragma once

#include <glm/glm.hpp>
#include <vector>

struct AABB_Collider {
    glm::vec2 pos;
    glm::vec2 size;
};

struct Line {
    glm::vec2 p1;
    glm::vec2 p2;

    // projected interval min max
    float mina, maxa;
    float minb, maxb;
};

inline
std::vector<Line> normal_lines;

// check if two axis aligned rectangles collide
bool aabb(const AABB_Collider& a, const AABB_Collider& b);

// check if two convex polygons intersect
bool sat(const std::vector<glm::vec2>& a, const std::vector<glm::vec2>& b);
