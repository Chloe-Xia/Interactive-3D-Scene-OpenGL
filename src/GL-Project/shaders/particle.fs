#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec4 ParticleColor;

uniform sampler2D particleTexture; // 粒子纹理（可选）

void main() {
    // 使用纹理（如果有的话）和粒子颜色
    vec4 texColor = texture(particleTexture, TexCoord);
    FragColor = ParticleColor * texColor;
    
    // 如果粒子太透明，丢弃片段
    if(FragColor.a < 0.1)
        discard;
} 