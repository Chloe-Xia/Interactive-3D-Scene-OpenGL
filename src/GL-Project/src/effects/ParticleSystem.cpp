#include "../../include/effects/ParticleSystem.h"
#include <random>
#include <stb_image.h>

// ParticleEmitter 实现
ParticleEmitter::ParticleEmitter(const glm::vec3& position, float emissionRate)
    : position(position), emissionRate(emissionRate), active(true), emissionAccumulator(0.0f) {}

void ParticleEmitter::update(float deltaTime) {
    if (!active) return;
    
    emissionAccumulator += deltaTime;
    float emissionInterval = 1.0f / emissionRate;
    
    while (emissionAccumulator >= emissionInterval) {
        emit();
        emissionAccumulator -= emissionInterval;
    }
}

// PointEmitter 实现
PointEmitter::PointEmitter(const glm::vec3& position, float emissionRate, float spread, float speed)
    : ParticleEmitter(position, emissionRate), spread(spread), speed(speed) {}

void PointEmitter::update(float deltaTime) {
    // 调用基类的 update 函数
    ParticleEmitter::update(deltaTime);
}

void PointEmitter::emit() {
    // 创建随机数生成器
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> spreadDist(-spread, spread);
    static std::uniform_real_distribution<float> speedDist(speed * 0.8f, speed * 1.2f);
    
    // 生成主要向上的方向
    glm::vec3 direction(
        spreadDist(gen) * 0.3f, // x轴扩散小
        1.0f,                   // y轴主要向上
        spreadDist(gen) * 0.3f  // z轴扩散小
    );
    direction = glm::normalize(direction);
    
    // 创建新粒子
    Particle particle;
    particle.position = position;
    particle.velocity = direction * speedDist(gen);
    particle.startColor = glm::vec4(1.0f, 0.7f, 0.2f, 1.0f); // 橙黄
    particle.endColor   = glm::vec4(0.2f, 0.0f, 0.0f, 0.0f); // 深红透明
    particle.color = particle.startColor;
    particle.size = 0.18f;  // 火焰粒子初始较大
    particle.life = 1.0f;
    particle.active = true;
    
    // 将粒子添加到系统
    if (system) {
        system->addParticle(particle);
    }
}

// ParticleSystem 实现
ParticleSystem::ParticleSystem(size_t maxParticles)
    : maxParticles(maxParticles), particleSize(0.5f), particleLife(2.0f),  // 增大粒子大小和生命周期
      particleColor(0.2f, 0.6f, 1.0f, 0.7f), hasTexture(false) {  // 更亮的蓝色
    // 初始化粒子数组
    particles.resize(maxParticles);
    
    // 设置默认粒子属性
    for (auto& particle : particles) {
        particle.active = false;
        particle.life = 0.0f;
    }
    
    // 初始化着色器
    shader = std::make_unique<Shader>("shaders/particle.vs", "shaders/particle.fs");
    
    // 初始化OpenGL缓冲区
    initBuffers();
}

ParticleSystem::~ParticleSystem() {
    // 清理OpenGL资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &instanceVBO);
    if (hasTexture) {
        glDeleteTextures(1, &particleTexture);
    }
}

void ParticleSystem::initBuffers() {
    // 创建顶点数据（一个简单的四边形）
    float quadVertices[] = {
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f
    };
    
    unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    // 创建VAO和VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);
    
    glBindVertexArray(VAO);
    
    // 设置顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    // 设置索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    
    // 设置顶点属性
    // 位置和纹理坐标
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    // 设置实例化数据
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    
    // 位置
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    glVertexAttribDivisor(2, 1);
    
    // 颜色
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    glVertexAttribDivisor(3, 1);
    
    // 大小
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
    glVertexAttribDivisor(4, 1);
    
    glBindVertexArray(0);
}

void ParticleSystem::addEmitter(std::unique_ptr<ParticleEmitter> emitter) {
    emitter->setSystem(this);
    emitters.push_back(std::move(emitter));
}

void ParticleSystem::addParticle(const Particle& particle) {
    // 查找第一个非活动粒子
    for (auto& p : particles) {
        if (!p.active) {
            p = particle;
            return;
        }
    }
}

void ParticleSystem::update(float deltaTime) {
    // 更新所有发射器
    for (auto& emitter : emitters) {
        emitter->update(deltaTime);
    }
    
    // 更新所有粒子
    for (auto& particle : particles) {
        if (particle.active) {
            // 更新位置
            particle.position += particle.velocity * deltaTime;
            // 更新生命周期
            particle.life -= deltaTime / particleLife;
            // 颜色插值（火焰效果）
            float t = 1.0f - particle.life; // 0~1
            t = glm::clamp(t, 0.0f, 1.0f);
            particle.color = glm::mix(particle.startColor, particle.endColor, t);
            // 粒子大小随生命周期变小
            particle.size = glm::mix(0.18f, 0.05f, t);
            // 检查是否应该停用粒子
            if (particle.life <= 0.0f) {
                particle.active = false;
            }
        }
    }
    // 更新实例化数据
    updateInstanceData();
}

void ParticleSystem::updateInstanceData() {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());
}

void ParticleSystem::setParticleTexture(const std::string& texturePath) {
    // 加载纹理
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        if (hasTexture) {
            glDeleteTextures(1, &particleTexture);
        }
        
        glGenTextures(1, &particleTexture);
        glBindTexture(GL_TEXTURE_2D, particleTexture);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        GLenum format = GL_RGB;
        if (nrChannels == 4) format = GL_RGBA;
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        stbi_image_free(data);
        hasTexture = true;
    }
}

void ParticleSystem::render(Shader& shader) {
    // 保存当前纹理状态
    GLint activeTexture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
    
    // 使用粒子系统的着色器
    this->shader->use();
    
    // 设置着色器uniform变量
    this->shader->setMat4("view", shader.getViewMatrix());
    this->shader->setMat4("projection", shader.getProjectionMatrix());
    
    // 启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if (hasTexture) {
        // 绑定粒子纹理到纹理单元4（避免与其他纹理冲突）
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, particleTexture);
        this->shader->setInt("particleTexture", 4);
    }
    
    // 绑定VAO并绘制
    glBindVertexArray(VAO);
    
    // 计算活动粒子数量
    size_t activeCount = 0;
    for (const auto& particle : particles) {
        if (particle.active) ++activeCount;
    }
    
    // 使用实例化绘制
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(activeCount));
    
    glBindVertexArray(0);
    
    // 重置纹理状态
    if (hasTexture) {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // 恢复之前的纹理单元
    glActiveTexture(activeTexture);
    
    glDisable(GL_BLEND);
} 