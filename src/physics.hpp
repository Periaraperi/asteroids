#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include "opengl_errors.hpp"
#include "peria_logger.hpp"
#include "transform.hpp"

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

// Simple polygon used for collider
class Polygon {
public:
    Polygon() = default;

    // polygon points are ordered in clockwise direction.
    // polygon points are in world space.
    explicit Polygon(const std::vector<glm::vec2>& ps)
        :_points{ps}
    { 
        //PERIA_LOG("poly ctor()");
        PERIA_ASSERT(_points.size() >= 3, "Poly size must be at least 3");
    }

    // polygon points are ordered in clockwise direction.
    // polygon points are in world space.
    explicit Polygon(std::vector<glm::vec2>&& ps)
        :_points{std::move(ps)}
    {
        //PERIA_LOG("poly move ctor()"); 
        PERIA_ASSERT(_points.size() >= 3, "Poly size must be at least 3");
    }

    [[nodiscard]]
    bool is_convex() const
    {
        PERIA_ASSERT(_points.size() >= 3, "Polygon must have at least 3 _points");
        if (_points.size() < 3) return false;

        auto p1 = _points[1] - _points[0];
        auto p2 = _points[2] - _points[1];
        auto direction = (p1.x*p2.y - p2.x*p1.y) < 0.0f;

        for (std::size_t i{}; i<_points.size(); ++i) {
            auto j = (i+1)%_points.size();
            auto k = (i+2)%_points.size();

            p1 = _points[j]-_points[i];
            p2 = _points[k]-_points[j];
            auto d = (p1.x*p2.y - p2.x*p1.y) < 0.0f;

            // direction changed ==> convex
            if (d != direction) {
                return false;
            }
        }
        return true;
    }

    // Simple Polygon triangulation using O(n^2) algorithm (Ear clipping).
    // Function Assumes that Polygon does not have holes or is self intersecting or contains 3 collinear points.
    // Pass bool::true to stop triangulating if sub-polygon becomes convex.
    // Returns std::vector of new Polygons which are triangles
    [[nodiscard]]
    std::vector<Polygon> triangulate(bool full_triangulation = true) const
    {
        auto N = _points.size();
        if (_points.size() == 3) return {*this};

        auto points = _points; // make a copy

        std::vector<Polygon> res; res.reserve(N-2);

        for (std::size_t c{}; c<N && N>3;) {
            auto p1 = points[1] - points[0];
            auto p2 = points[2] - points[1];
            auto direction = (p1.x*p2.y - p2.x*p1.y) < 0.0f;
            auto cut_ear = false;

            for (std::size_t i{}; i<N && N>3; ++i) {
                auto j = (i+1)%N;
                auto k = (i+2)%N;

                p1 = points[j]-points[i];
                p2 = points[k]-points[j];
                auto d = (p1.x*p2.y - p2.x*p1.y) < 0.0f;
                if (d != direction) {
                    res.emplace_back(std::vector{points[(i-1)%N], points[i], points[j]});
                    for (std::size_t ii=i; ii<N; ++ii) {
                        points[ii] = std::move(points[(ii+1)%N]);
                    }
                    --N;
                    cut_ear = true;
                    break;
                }
            }
            if (!cut_ear) break;
        }

        // if cannot cut more 'ears' do triangulation with triangle fan
        if (full_triangulation) { 
            for (std::size_t i=1; i<N-1; ++i) {
                res.emplace_back(std::vector{points[0], points[i], points[i+1]});
            }
        }
        else { // will separate ears until remaining part is convex
            std::vector<glm::vec2> ps; ps.reserve(N); // temp
            // at this point 'points' contains only remaining vertices
            // that form convex polygon
            for (std::size_t i=0; i<N; ++i) {
                ps.emplace_back(std::move(points[i]));
            }
            res.emplace_back(std::move(ps));
        }

        return res;
    }

    [[nodiscard]]
    const auto& points() const
    { return _points; }

    [[nodiscard]]
    auto& points()
    { return _points; }

private:
    std::vector<glm::vec2> _points;
};

// for debug
inline
std::vector<Line> normal_lines_a;
inline
std::vector<Line> normal_lines_b;

// check if two axis aligned rectangles collide
[[nodiscard]]
bool aabb(const AABB_Collider& a, const AABB_Collider& b);

// check if two circles collide
[[nodiscard]]
bool circle_circle(glm::vec2 a, float a_radius, glm::vec2 b, float b_radius);

// check if two convex polygons intersect
// TODO: delete this later
[[nodiscard]]
bool sat(const std::vector<glm::vec2>& a, const std::vector<glm::vec2>& b);

// Check if two convex polygons intersect.
// Function assumes that both polygons are simple and convex.
// Caller must triangulate or modify polygons before calling 'sat()'.
// Rotation of edge vector is in anti-clockwise direction.
// Algorithm Does not consider collision resolution
[[nodiscard]]
bool sat(const Polygon& a, const Polygon& b);

// check if any two simple polygons intersect.
// will triangulate polygons if concave and check sat on triangles.
[[nodiscard]]
bool concave_sat(const Polygon& a, const Polygon& b);

[[nodiscard]]
float lerp(float a, float b, float alpha);

[[nodiscard]]
Transform interpolate_state(const Transform& prev, const Transform& current, float alpha);
