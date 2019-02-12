#ifndef TRANSFORM_H
#define TRANSFORM_H


#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>  // glm::value_ptr()


struct Transform {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // In parent coordinates
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    
    // glm::mat4 scale = glm::mat4(1.0f);

    Transform(glm::vec3 pos=glm::vec3(0.0f, 0.0f, 0.0f), glm::quat rot=glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    glm::vec3 forwardVec() const;
    // glm::mat4 modelMatrix() const {
    //     return scale * modelMatrixIsometric();
    // }
    glm::mat4 modelMatrixIsometric() const;
};

Transform operator*(Transform left, Transform right);
Transform mix(const Transform& first, const Transform& second, float t);

#endif