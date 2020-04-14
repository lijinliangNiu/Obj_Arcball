#include "Arcball.h"

Arcball::Arcball(int window_width, int window_height, GLfloat roll_speed) {
    this->windowWidth = window_width;
    this->windowHeight = window_height;

    this->mouseEvent = 0;
    this->rollSpeed = roll_speed;
    this->viewMatrix = glm::mat4(1.0f);
    this->rotationMatrix = glm::mat4(1.0f);
    this->preTrans = glm::mat4(1.0f);
}

//映射到arcball
glm::vec3 Arcball::toScreenCoord(double x, double y) {
    glm::vec3 coord(0.0f);

    coord.x = (2 * x - windowWidth) / (2 * windowWidth);

    coord.y =  (windowHeight - 2 * y) / (2 * windowHeight);

    //不要溢出窗口
    coord.x = glm::clamp(coord.x, -1.0f, 1.0f);
    coord.y = glm::clamp(coord.y, -1.0f, 1.0f);

    coord.z = sqrt(1.0 - coord.x * coord.x + coord.y * coord.y);

    return glm::normalize(coord);
}

void Arcball::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
    this->mouseEvent = (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT);
}

void Arcball::cursorCallback(GLFWwindow *window, double x, double y) {
    if (mouseEvent == 1) {
        this->preTrans = this->rotationMatrix;
        prePos = toScreenCoord(x, y);
        mouseEvent = 2;
    }
    else if(mouseEvent == 2){
        curPos = toScreenCoord(x, y);

        float angle = acos(std::min(1.0f, glm::dot(prePos, curPos)));

        glm::vec3 camAxis = glm::cross(prePos, curPos);
        glm::vec3 axis = glm::inverse(glm::mat3(viewMatrix)) * camAxis;

        this->rotationMatrix = glm::rotate(glm::degrees(angle) * rollSpeed, axis) * this->preTrans;
    }
}

void Arcball::setViewMatrix(glm::mat4 view) {
    this->viewMatrix = view;
}
glm::mat4 Arcball::getRotationMatrix() {
    return this->rotationMatrix;
}