#pragma once

class Graphics;
class Input_Manager;

namespace peria {
    void update(float dt, Graphics& g, Input_Manager& im);
    void draw(Graphics& g, Input_Manager& im);
}