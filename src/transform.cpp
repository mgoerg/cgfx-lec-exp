#include "transform.h"

Transform::Transform(glm::vec3 pos/*=glm::vec3(0.0f, 0.0f, 0.0f)*/, glm::quat rot/*=glm::quat(1.0f, 0.0f, 0.0f, 0.0f)*/) {
    position = pos;
    rotation = rot;
}

glm::vec3 Transform::forwardVec() const { // In parent coordinates
    return glm::vec3(0, 0, -1) * rotation;
}
// glm::mat4 modelMatrix() const {
//     return scale * modelMatrixIsometric();
// }
glm::mat4 Transform::modelMatrixIsometric() const {
    glm::mat4 result = glm::mat4(1.0f);
    result = glm::translate(result, position);
    return result * glm::mat4_cast(rotation);
}

Transform operator*(Transform left, Transform right) {
    Transform result;
    result.position = left.position + left.rotation * right.position;
    result.rotation = left.rotation * right.rotation;

    return result;
}

Transform mix(const Transform& first, const Transform& second, float t) {
    Transform result;
    result.position = first.position * (1-t) + second.position * t;
    result.rotation = glm::mix(first.rotation, second.rotation, t);
    return result;
}


