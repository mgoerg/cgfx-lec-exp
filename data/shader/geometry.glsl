#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 v_position[];
in vec3 v_normal[];
in vec4 v_color[];

out vec3 f_position;
out vec3 f_normal;
out vec4 f_color;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;


// The following sets how the vertex normals are transformed by the impact effect.

// #define calculate_normals_by_triangles 1

// if it is defined, the normals are generated from the vertices of the displaced triangle. 
// leading to intentionally hard edges between triangles
// otherwise the vertex normals are calculated using approximated derivatives of the displacement function
// which removes the visible edges



// Impact Data

const int IMPACT_VALID_BIT = 1 << 31;
const int IMPACT_ENABLED_BIT = 1 << 30;

const int MAX_IMPACTS = 256;

struct ImpactSourceData {
    vec3 position;
    int flags;

    vec3 direction;
    float intensity;

    float distance;
    float width;
    //float _pad[2];
};

uniform bool u_useImpact;
layout (std140) uniform ImpactBlock {
    int u_numImpactSources;
    //float _pad[3];
    ImpactSourceData impactSources[MAX_IMPACTS];
};


float easeInOut(float t) {
    return t*t * (-2*t + 3);
}

float clamp01(float t) {
    return min(1, max(0, t));
}

float clamp0m(float t, float upperBound) {
    return min(upperBound, max(0, t));
}

vec3 compute_displacement(vec3 position, ImpactSourceData impact) {
    float distance = length(impact.position - position);
    if (abs(distance - impact.distance) > 2 * impact.width) return vec3(0, 0, 0);

    float a = easeInOut(1-clamp01(abs(distance - impact.distance) / impact.width)) * impact.intensity;
    float intensity = max(0, a);
    return impact.direction * intensity;
}

vec3 arbitraryNormal(vec3 v) {
    if (v.x == 0 && v.y == 0) {
        return vec3(1, 0, 0);
    }

    return normalize(vec3(-v.y, v.x, 0));
}

uniform float time;

void main() {
    const float epsilon = 0.05; // a small positive number

    if (u_useImpact) {
        vec3 new_position[3];

        // Compute displaced position
        for (int k = 0; k < 3; k++) {
            #ifndef calculate_normals_by_triangles
            vec3 tangent = arbitraryNormal(v_normal[k]);
            vec3 bitangent = normalize(cross(v_normal[k], tangent));
            vec3 tangentPos = v_position[k] + tangent * epsilon;
            vec3 bitangentPos = v_position[k] + bitangent * epsilon;

            vec3 displacement_tangent = vec3(0, 0, 0); 
            vec3 displacement_bitangent = vec3(0, 0, 0); 
            #endif

            vec3 displacement = vec3(0, 0,0);

            vec3 vertexWorldPosition = v_position[k];

            for (int i = 0; i < u_numImpactSources; i++) {
                ImpactSourceData impact = impactSources[i];
                if (((impact.flags & IMPACT_VALID_BIT) == 0) || ((impact.flags & IMPACT_ENABLED_BIT) == 0)) {
                    continue;
                }

                displacement += compute_displacement(v_position[k], impact);

                #ifndef calculate_normals_by_triangles
                if (displacement != vec3(0, 0, 0)) {
                    displacement_tangent += compute_displacement(tangentPos, impact);
                    displacement_bitangent += compute_displacement(bitangentPos, impact);
                }
                #endif
            }

            new_position[k] = v_position[k] + displacement;
            #ifndef calculate_normals_by_triangles
            vec3 displacedisplacement_tangentent = (tangent * epsilon + displacement_tangent - displacement);
            vec3 displacedisplacement_bitangentent = (bitangent * epsilon + displacement_bitangent - displacement);
            vec3 normal;
            if (length(displacedisplacement_tangentent) > epsilon * epsilon && length(displacedisplacement_bitangentent) > epsilon * epsilon) {
                normal = normalize(cross(displacedisplacement_tangentent, displacedisplacement_bitangentent));
            } else {
                normal = v_normal[k];
            }

            f_position = new_position[k];
            gl_Position = u_projectionMatrix * u_viewMatrix * vec4(new_position[k], 1.0);
            f_color = v_color[k];
            f_normal = normal;
            EmitVertex();
            #endif
        }

        #ifdef calculate_normals_by_triangles 
        // Compute new normal
        vec3 normal = normalize(cross(new_position[1] - new_position[0], new_position[2] - new_position[0]));

        // Generate primitive
        for (int k = 0; k < 3; k++) {
            gl_Position = u_projectionMatrix * u_viewMatrix * vec4(new_position[k], 1.0);
            f_color = v_color[k];
            f_position = new_position[k];
            f_normal = normal;
            EmitVertex();
        }
        #endif

        EndPrimitive();

    } else {
        for (int k = 0; k < 3; k++) {
            gl_Position = gl_in[k].gl_Position;
            f_color = v_color[k];
            f_position = v_position[k];
            f_normal = v_normal[k];
            EmitVertex();
        }
        EndPrimitive();
    }
}  