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


// #define displacement_accurate_normals 1


// Impact

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

    float a = easeInOut(clamp01(abs(distance - impact.distance) / impact.width)) * impact.intensity;
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
    if (u_useImpact) {
        vec3 new_position[3];

        // Compute displaced position
        for (int k = 0; k < 3; k++) {

            #ifndef displacement_accurate_normals
            vec3 tangent = arbitraryNormal(v_normal[k]);
            vec3 bitangent = normalize(cross(v_normal[k], tangent));
            vec3 tangentPos = v_position[k] + tangent * 0.3;
            vec3 bitangentPos = v_position[k] + bitangent * 0.3;

            vec3 dtang = vec3(0, 0,0);
            vec3 dbitang = vec3(0, 0,0);
            #endif

            vec3 displacement = vec3(0, 0,0);

            vec3 vertexWorldPosition = v_position[k];

            for (int i = 0; i < u_numImpactSources; i++) {
                ImpactSourceData impact = impactSources[i];
                if (((impact.flags & IMPACT_VALID_BIT) == 0) || ((impact.flags & IMPACT_ENABLED_BIT) == 0)) {
                    continue;
                }

                displacement -= compute_displacement(v_position[k], impact);

                #ifndef displacement_accurate_normals
                dtang -= compute_displacement(tangentPos, impact);
                dbitang -= compute_displacement(bitangentPos, impact);
                #endif
            }

            new_position[k] = v_position[k] + displacement;
            #ifndef displacement_accurate_normals
            vec3 displacedTangent = (tangent * 0.3 + dtang - displacement);
            vec3 displacedBitangent = (bitangent * 0.3 + dbitang - displacement);
            vec3 normal;
            if (length(displacedTangent) > 0.05 && length(displacedBitangent) > 0.05) {
                normal = normalize(cross(displacedTangent, displacedBitangent));
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

        #ifdef displacement_accurate_normals 
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