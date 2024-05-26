#pragma once

#include "anthraxAI/vkdefines.h"

class Camera {
public:
    void setposition(glm::vec3 pos) { Position = pos; };
    void setdirections();

    glm::vec3 getposition() const { return Position; }
    glm::vec3 getfront() const { return Front; }
    glm::vec3 getup() const { return Up; }

    void checkmovement(xcb_keysym_t k, float delta);
    void checkdirection(Positions mousepos);

private:
	glm::vec3 Position;
    glm::vec3 Target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Direction;
    glm::vec3 Right;
    glm::vec3 Front;
    glm::vec3 Up;

    glm::vec3 WorldUp;

    glm::mat4 View;

    float yaw = -90.0f;
    float pitch = 0.0f;
};
