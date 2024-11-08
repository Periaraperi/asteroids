#include "physics.hpp"

#include <glm/vec2.hpp>

namespace peria {

bool aabb(const AABB_Collider &a, const AABB_Collider &b)
{
    const auto& ax = a.pos.x;
    const auto& ay = a.pos.y;
    const auto& bx = b.pos.x;
    const auto& by = b.pos.y;
    const auto& aw = a.size.x;
    const auto& ah = a.size.y;
    const auto& bw = b.size.x;
    const auto& bh = b.size.y;
    
    bool overlap_x = (ax <= (bx+bw)) && ((ax+aw) >= bx);
    bool overlap_y = (ay >= (by-bh)) && ((ay-ah) <= by);

    return overlap_x && overlap_y;
}

bool circle_circle(glm::vec2 a, float a_radius, glm::vec2 b, float b_radius)
{
    auto x = a.x-b.x;
    auto y = a.y-b.y;
    auto dis = std::sqrt(x*x + y*y);
    return dis <= a_radius + b_radius;
}

// poly points are in clockwise order.
// rotation of edge vector is in anti-clockwise direction.
// for collision check direction does not matter.
// for resolution it may. So keep in mind.
bool sat(const std::vector<glm::vec2>& a, const std::vector<glm::vec2>& b)
{
    // debug lines
    normal_lines_a.clear();
    normal_lines_b.clear();

    // 1) get all edges of each polygon.
    // 2) get normal vectors for each edge. This will be our potential seperation axis.
    // 3) test all points on each polygon and take min and max value of projected points
    //    for each polygon.
    // 4) if there is not an overlap --> there is no collision
    // 5) if on all axis an overlap occurs --> there is a collision

    auto min_max_after_projection = [](const std::vector<glm::vec2>& points,
                                       glm::vec2 axis) -> std::pair<float, float> {
        auto mn = std::numeric_limits<float>::max();
        auto mx = std::numeric_limits<float>::lowest();
        
        for (const auto& p:points) {
            auto projected = glm::dot(axis, p);
            mn = std::min(mn, projected);
            mx = std::max(mx, projected);
        }

        return {mn, mx};
    };

    // test axis of polygon a
    for (std::size_t i{}; i<a.size(); ++i) {
        auto p1 = a[i];
        auto p2 = a[(i+1)%a.size()];
        
        auto edge = p2 - p1;
        glm::vec2 axis{-edge.y, edge.x}; // we rotate edge by 90 degrees to get normal vector

        auto [min_a, max_a] = min_max_after_projection(a, axis);
        auto [min_b, max_b] = min_max_after_projection(b, axis);

        // for debugging
        normal_lines_a.push_back({p1, axis+p1, min_a, max_a, min_b, max_b, {}});

        // no overlap
        if ((max_a < min_b) || (max_b < min_a)) {
            normal_lines_a.back().color = {0.5f, 1.0f, 0.5f, 1.0f};
            return false;
        }

    }
    
    // test axis of polygon b
    for (std::size_t i{}; i<b.size(); ++i) {
        auto p1 = b[i];
        auto p2 = b[(i+1)%b.size()];
        
        glm::vec2 edge = p2 - p1;
        glm::vec2 axis{-edge.y, edge.x}; // we rotate edge by 90 degrees to get normal vector

        auto [min_a, max_a] = min_max_after_projection(a, axis);
        auto [min_b, max_b] = min_max_after_projection(b, axis);
        
        // for debugging
        normal_lines_b.push_back({p1, axis+p1, min_a, max_a, min_b, max_b, {}});

        if ((max_a < min_b) || (max_b < min_a)) {
            normal_lines_b.back().color = {0.5f, 1.0f, 0.5f, 1.0f};
            return false;
        }

    }

    return true;
}

// Function assumes that both polygons are simple and convex.
// Caller must triangulate or modify polygons before calling 'sat()'.
// Rotation of edge vector is in anti-clockwise direction.
// Algorithm Does not consider collision resolution
bool sat(const Polygon& poly1, const Polygon& poly2)
{
    // helper lambda to find interval extrema after projection
    auto min_max_after_projection = [](const std::vector<glm::vec2>& points,
                                       glm::vec2 axis) -> std::pair<float, float> {
        auto mn = std::numeric_limits<float>::max();
        auto mx = std::numeric_limits<float>::lowest();
        
        for (const auto& p:points) {
            auto projected = glm::dot(axis, p);
            mn = std::min(mn, projected);
            mx = std::max(mx, projected);
        }

        return {mn, mx};
    };

    const auto& a = poly1.points();
    const auto& b = poly2.points();

    // test axis of polygon a
    for (std::size_t i{}; i<a.size(); ++i) {
        auto p1 = a[i];
        auto p2 = a[(i+1)%a.size()];
        
        auto edge = p2 - p1;
        glm::vec2 axis{-edge.y, edge.x}; // we rotate edge by 90 degrees to get normal vector

        auto [min_a, max_a] = min_max_after_projection(a, axis);
        auto [min_b, max_b] = min_max_after_projection(b, axis);

        // no overlap
        if ((max_a < min_b) || (max_b < min_a)) {
            return false;
        }

    }
    
    // test axis of polygon b
    for (std::size_t i{}; i<b.size(); ++i) {
        auto p1 = b[i];
        auto p2 = b[(i+1)%b.size()];
        
        glm::vec2 edge = p2 - p1;
        glm::vec2 axis{-edge.y, edge.x}; // we rotate edge by 90 degrees to get normal vector

        auto [min_a, max_a] = min_max_after_projection(a, axis);
        auto [min_b, max_b] = min_max_after_projection(b, axis);
        
        if ((max_a < min_b) || (max_b < min_a)) {
            return false;
        }

    }

    return true;
}

bool concave_sat(const Polygon& a, const Polygon& b)
{
    std::vector<Polygon> as {a};
    if (!a.is_convex()) {
        as = a.triangulate(false);
    }
    std::vector<Polygon> bs {b};
    if (!b.is_convex()) {
        bs = b.triangulate(false);
    }

    for (const auto& part_a:as) {
        for (const auto& part_b:bs) {
            if (sat(part_a, part_b)) 
                return true;
        }
    }
    
    return false;
}

float lerp(float a, float b, float alpha)
{ return (1.0f - alpha)*a + b*alpha; }

Transform interpolate_state(const Transform& prev, const Transform& current, float alpha)
{
    auto f = [&alpha](float a, float b) -> float {
        return lerp(a, b, alpha);
    };
    
    return {
        glm::vec2{f(prev.pos.x, current.pos.x),
                  f(prev.pos.y, current.pos.y)},
        glm::vec2{f(prev.scale.x, current.scale.x),
                  f(prev.scale.y, current.scale.y)},
        f(prev.angle, current.angle)
    };
}
}
