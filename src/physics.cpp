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
    
    bool overlap_x = (ax <= bx && bx <= (ax+aw)) || (ax <= (bx+bw) && (bx+bw) <= (ax+aw)) || 
                     (ax <= bx && (bx+bw) <= (ax+aw)) || (ax >= bx && (bx+bw) >= (ax+aw));
    bool overlap_y = (ay >= by && by >= (ay-ah)) || (ay >= (by-bh) && (by-bh) >= (ay-ah)) ||
                     (ay >= by && (by-bh) >= (ay-ah)) || (ay <= by && (by-bh) <= (ay-ah));

    return overlap_x && overlap_y;
}
