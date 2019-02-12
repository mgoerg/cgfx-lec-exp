#version 450 core
in vec3 a_vertexPosition;
in vec3 a_vertexNormal;
in vec4 a_vertexColor;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;
uniform mat4 u_normalMatrix;

out vec3 v_position;
out vec3 v_normal;
out vec4 v_color;


void main(void) {
    vec4 vertexWorldPosition = u_modelMatrix * vec4(a_vertexPosition, 1.0);
    vec3 vertexWorldNormal = (u_normalMatrix * vec4(a_vertexNormal, 1.0)).xyz;

    gl_Position = u_projectionMatrix * u_viewMatrix * vertexWorldPosition;
    v_normal = vertexWorldNormal;
    v_position = vertexWorldPosition.xyz;
    v_color = a_vertexColor;
}
