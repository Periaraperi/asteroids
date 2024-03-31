#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

struct AABB_Collider {
    glm::vec2 pos;
    glm::vec2 size;
};

// debug lines for normal vectors
struct Line {
    glm::vec2 p1;
    glm::vec2 p2;

    // projected interval min max
    float mina, maxa;
    float minb, maxb;

    glm::vec4 color;

    friend std::ostream& operator<<(std::ostream& os, const Line& l)
    {
        os << "P1: (" << l.p1.x << ", " << l.p1.y << ") ";
        os << "P2: (" << l.p2.x << ", " << l.p2.y << ") ";

        os << "\nProjected Interval_A: ";
        os << "(" << l.mina << ", " << l.maxa << ")";
        os << "\nProjected Interval_B: ";
        os << "(" << l.minb << ", " << l.maxb << ")";

        return os;
    }

};

struct Polygon {
    std::vector<glm::vec2> points;
    Polygon() = default;

    // polygon points are ordered in clockwise direction.
    // polygon points are in world space.
    explicit Polygon(const std::vector<glm::vec2>& ps)
        :points{ps}
    {}
    
    // this will rotate polygon around its visual center
    // in counter-clockwise direction.
    // NOTE: polygon is assumed to be convex.
    // angle is in radians
    void rotate(float angle)
    {
        const auto c = visual_center();
        
        for (auto& p:points) {
            // 1) move to origin according to visual center
            p -= c;

            // 2) rotate in anti clockwise direction
            glm::vec2 u{std::cos(angle)*p.x - std::sin(angle)*p.y, std::sin(angle)*p.x + std::cos(angle)*p.y};
            p = u;

            // 3) move back
            p += c;
        }
    }

    void move(glm::vec2 dir)
    {
        for (auto& p:points) {
            p += dir;
        }
    }

    // NOTE: polygon is assumed to be convex.
    glm::vec2 visual_center() const noexcept
    {
        glm::vec2 c{0.0f, 0.0f};
        for (const auto& p:points) {
            c += p;
        }
        c.x /= static_cast<float>(points.size());
        c.y /= static_cast<float>(points.size());

        return c;
    }
};

// for debug
inline
std::vector<Line> normal_lines_a;
inline
std::vector<Line> normal_lines_b;

// check if two axis aligned rectangles collide
bool aabb(const AABB_Collider& a, const AABB_Collider& b);

// check if two convex polygons intersect
bool sat(const std::vector<glm::vec2>& a, const std::vector<glm::vec2>& b);
