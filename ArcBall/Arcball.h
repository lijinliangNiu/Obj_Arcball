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
    /*�������Ƿ񱻰���
    0��δ����
    1����������¼prePos
    2���ɿ�����¼curPos
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