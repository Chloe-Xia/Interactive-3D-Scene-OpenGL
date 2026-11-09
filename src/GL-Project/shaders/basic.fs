#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

// 定义纹理单元
#define TEXTURE_UNIT_ROOM 0

struct Material {
    vec3 color;
    sampler2D roomTexture;        // 用于房间地板等普通纹理 (TEXTURE_UNIT_ROOM)
    bool useRoomTexture;
    bool useTexture;             // 是否使用任何纹理
};

uniform Material material;
uniform bool useCustomColor;  // 是否使用自定义颜色
uniform vec3 customColor;     // 自定义颜色

void main() {
    // 1. 首先检查是否使用自定义颜色
    if (useCustomColor) {
        FragColor = vec4(customColor, 1.0);
        return;
    }

    // 2. 检查纹理状态
    vec3 finalColor = material.color;  // 默认使用材质颜色
    
    // 严格检查纹理状态
    if (material.useTexture && material.useRoomTexture) {
        // 使用房间纹理
        finalColor = texture(material.roomTexture, TexCoord).rgb;
    }
    
    FragColor = vec4(finalColor, 1.0);
} 