#pragma once

#include <glm/gtc/matrix_transform.hpp>

struct Transform {
    glm::vec2 pos{};
    glm::vec2 scale{};
    float angle{}; // degrees

    // returns model matrix constructed from scale, pos, and optional angle
    static glm::mat4 model(glm::vec2 p, glm::vec2 s, float a = 0.0f /*in degrees*/)
    {
        // rotate around z-axis
        return glm::translate(glm::mat4(1.0f), glm::vec3(p, 0.0f))*
               glm::rotate(glm::mat4(1.0f), glm::radians(a), glm::vec3(0.0f, 0.0f, 1.0f))*
               glm::scale(glm::mat4(1.0f), glm::vec3(s, 1.0f));
    }

    static void clamp_angle(float& angle)
    {
        if (angle >= FULL_ROT_ANGLE) {
            auto k {static_cast<int>(angle / FULL_ROT_ANGLE)};
            angle -= k*FULL_ROT_ANGLE;
        }
        else if (angle <= -FULL_ROT_ANGLE) {
            auto k {static_cast<int>(angle / (-FULL_ROT_ANGLE))};
            angle += k*FULL_ROT_ANGLE;
        }
    }

private:
    constexpr static float FULL_ROT_ANGLE = 360.0f;
};
