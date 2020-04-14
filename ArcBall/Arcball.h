#pragma once

#include <iostream>
#include <algorithm>

#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_inverse.hpp>

class Arcball {
private:
    int windowWidth;
    int windowHeight;
    /*鼠标左键是否被按下
    0：未发生
    1：发生，记录prePos
    2：松开，记录curPos
    */
    int mouseEvent;
    GLfloat rollSpeed;
    glm::vec3 prePos;
    glm::vec3 curPos;
    glm::mat4 viewMatrix;
    glm::mat4 rotationMatrix;
    glm::mat4 preTrans;

public:
    Arcball(int window_width, int window_height, GLfloat roll_speed = 1.0f);
    glm::vec3 toScreenCoord(double x, double y);

    void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
    void cursorCallback(GLFWwindow *window, double x, double y);

    void setViewMatrix(glm::mat4 view);
    glm::mat4 getRotationMatrix();
};