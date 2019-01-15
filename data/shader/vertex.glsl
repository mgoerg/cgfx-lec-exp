#version 330 core
layout (location=1) in vec3 a_vertexPosition;
layout (location=2) in vec3 a_vertexNormal;
layout (location=3) in vec4 a_vertexColor;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;
uniform mat4 u_normalMatrix;

out vec3 v_normal;
out vec3 v_position;
out vec4 v_color;

void main(void) {
    gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_vertexPosition, 1.0);
    v_normal = (u_normalMatrix * vec4(a_vertexNormal, 1.0)).xyz;
    v_position = (u_modelMatrix * vec4(a_vertexPosition, 1.0)).xyz;
    v_color = a_vertexColor;
}
