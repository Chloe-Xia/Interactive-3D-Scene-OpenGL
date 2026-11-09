#version 330 core
layout (location = 0) in vec3 aPos;      // 粒子四边形顶点位置
layout (location = 1) in vec2 aTexCoord; // 纹理坐标
layout (location = 2) in vec3 aParticlePos; // 粒子位置（实例化）
layout (location = 3) in vec4 aParticleColor; // 粒子颜色（实例化）
layout (location = 4) in float aParticleSize; // 粒子大小（实例化）

out vec2 TexCoord;
out vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
    // 计算粒子在世界空间中的位置
    vec3 pos = aParticlePos + aPos * aParticleSize;
    
    // 应用视图和投影变换
    gl_Position = projection * view * vec4(pos, 1.0);
    
    // 传递纹理坐标和颜色到片段着色器
    TexCoord = aTexCoord;
    ParticleColor = aParticleColor;
} 