
#include "camera.h"



glm::mat4 Camera::viewMatrix() {
    return glm::mat4_cast(rotation) * glm::translate(glm::mat4(1.0f), -eyePosition);
}

glm::mat4 Camera::projectionMatrix(float aspect) {
    return glm::perspective(fov, aspect, near, far);
}

glm::vec3 Camera::forwardVec() {
    return glm::vec3(0, 0, -1) * rotation;
}