#include "physics.hpp"

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
