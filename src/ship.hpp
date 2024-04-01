#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

class Input_Manager;
class Graphics;

class Ship {
public:
    Ship(Graphics& graphics, Input_Manager& input_manager);
    void update(float dt);
    void draw();

private:
    glm::mat4 transform_mat();

private:
    Graphics& _graphics;
    Input_Manager& _input_manager;
    std::vector<glm::vec2> _ship_model;

    float _scale;
    float _angle; // in degrees
    glm::vec2 _pos;
    glm::vec2 _velocity;



};
