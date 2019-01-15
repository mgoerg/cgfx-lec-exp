#version 330 core

const int MAX_LIGHTS = 3;


const int LIGHT_AMBIENT_BIT =       1 << 0;
const int LIGHT_DIRECTIONAL_BIT =   1 << 1;
const int LIGHT_POINT_BIT =         1 << 2;

const int LIGHT_VALID_BIT =         1 << 31;
const int LIGHT_ENABLED_BIT =       1 << 30;


struct LightSource
{
    vec3 color;
    int flags;
    vec3 position;
    float intensity;
    float attenuationConst;
    float attenuationLin;
    float attenuationSq;
};







// Light
uniform bool u_useLighting;
layout (std140) uniform LightBlock {
    int u_numLights;
    LightSource u_lightSource[MAX_LIGHTS];
};


uniform bool u_useCelShading;


in vec3 v_position;
in vec3 v_normal;
in vec4 v_color;

out vec4 FragColor;


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// precision mediump float;


void main(void) {
    vec3 resultColor = vec3(0, 0, 0);
    if (u_useLighting) {
        vec3 totalLight = vec3(0, 0, 0);
        for (int i = 0; i < u_numLights; i++) {
            LightSource light = u_lightSource[i];
            
            if (((light.flags & LIGHT_ENABLED_BIT) == 0 ) || ((light.flags & LIGHT_VALID_BIT) == 0))
               continue;
            
            // Ambient
            if ((light.flags & LIGHT_AMBIENT_BIT) != 0) {
                totalLight += light.color;
            }
            
            vec3 normal = normalize(v_normal);
            
            // Directional
            if ((light.flags & LIGHT_DIRECTIONAL_BIT) != 0) {
                vec3 direction = light.position;
                float nDotL = max(dot(normal, direction), 0.0);
                vec3 diffuseLight = light.color * nDotL * light.intensity;
                totalLight += diffuseLight;	
                // No specular for now.
            }
            
            // Point
            if ((light.flags & LIGHT_POINT_BIT) != 0) {                    
                vec3 lightDirection = v_position - light.position;
                float dist = length(lightDirection);
                lightDirection = normalize(lightDirection);
                float nDotL = max(-dot(normal, lightDirection), 0.0);
                vec3 diffuseLight = light.color * nDotL * light.intensity;
                float attenuation = light.attenuationConst + light.attenuationLin * dist + light.attenuationSq * dist * dist;
                diffuseLight = diffuseLight / attenuation;
                totalLight += diffuseLight;
            }
        }
        // Cel Shading
        if (u_useCelShading) {
            float r = totalLight.r;
            float g = totalLight.g;
            float b = totalLight.b;
            float max_c = max(b, max(r, g));
            float min_c = min(r, min(g, b));
            float lightness = (max_c + min_c) / 2.0;
            float new_lightness = lightness;
            
            new_lightness += (rand(gl_FragCoord.xy) - 0.5) / 256.0;

            if (new_lightness < 0.05) {
                new_lightness = 0.0;
            } else if (new_lightness < 0.1) {
                new_lightness = 0.07;
            } else if (new_lightness < 0.2) {
                new_lightness = 0.15;
            } else if (new_lightness < 0.4) {
                new_lightness = 0.3;
            } else if (new_lightness < 0.7) {
                new_lightness = 0.6;
            } else if (new_lightness < 0.9) {
                new_lightness = 0.8;			
            } else {
                new_lightness = 1.0;
            }
            totalLight = totalLight * (new_lightness / lightness);
        }
        
        resultColor = v_color.rgb * totalLight;
    } else {
        resultColor = v_color.rgb;
        //resultColor = vec3(1.0, 0.0, 0.5);
    }

    //if (u_useLowPalette) {
    //	float m = 16.0;
    //	float r = floor((resultColor.r) * m) / m;
    //	float g = floor((resultColor.g) * m) / m;
    //	float b = floor((resultColor.b) * m) / m;
    //	resultColor = vec3(r, g, b);
    //}

    FragColor = vec4(resultColor, 1.0);
    //if (int(gl_FragCoord[0]) /2*2 - int(gl_FragCoord[0]) == 0 || int(gl_FragCoord[1]) / 2 * 2 - int(gl_FragCoord[1]) == 0) {
    //	gl_FragColor = vec4(v_position * 0.1, 1.0);
    //}
}
