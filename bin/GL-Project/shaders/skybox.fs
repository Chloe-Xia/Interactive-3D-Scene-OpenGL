#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

// 定义纹理单元
#define TEXTURE_UNIT_DAY_SKYBOX 2
#define TEXTURE_UNIT_NIGHT_SKYBOX 3

uniform samplerCube daySkybox;   // 使用纹理单元2
uniform samplerCube nightSkybox; // 使用纹理单元3
uniform float blendFactor;       // 0.0 到 1.0，控制昼夜混合

void main() {
    vec4 dayColor = texture(daySkybox, TexCoords);
    vec4 nightColor = texture(nightSkybox, TexCoords);
    
    // 使用平滑的混合函数
    float smoothBlend = smoothstep(0.0, 1.0, blendFactor);
    FragColor = mix(nightColor, dayColor, smoothBlend);
} 