
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>  // glm::value_ptr()
#include <glm/gtc/quaternion.hpp>

class Camera {
	public: 
        glm::vec3 eyePosition;
        glm::quat rotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0)); /* = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0)) * glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 0, 1));*/

		float fov;
		float near;
		float far;

        Camera() {};
	
		glm::mat4 viewMatrix();
		glm::mat4 projectionMatrix(float aspect);
        glm::vec3 forwardVec();
};


#endif // CAMERA_H