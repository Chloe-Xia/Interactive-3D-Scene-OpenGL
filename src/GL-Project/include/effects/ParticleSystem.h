#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "../Shader.h"

// 粒子结构
struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;  // 当前颜色
    glm::vec4 startColor; // 火焰起始色
    glm::vec4 endColor;   // 火焰结束色
    float size;
    float life;
    bool active;
};

// 粒子发射器
class ParticleSystem;  // 前向声明

class ParticleEmitter {
public:
    ParticleEmitter(const glm::vec3& position, float emissionRate = 10.0f);
    virtual ~ParticleEmitter() = default;

    virtual void update(float deltaTime) = 0;
    virtual void emit() = 0;  // 改为无参数版本

    void setPosition(const glm::vec3& position) { this->position = position; }
    void setEmissionRate(float rate) { emissionRate = rate; }
    void setActive(bool active) { this->active = active; }
    void setSystem(ParticleSystem* system) { this->system = system; }  // 添加设置系统的方法

    glm::vec3 getPosition() const { return position; }
    float getEmissionRate() const { return emissionRate; }
    bool isActive() const { return active; }

protected:
    glm::vec3 position;
    float emissionRate;
    bool active;
    float emissionAccumulator = 0.0f;  // 添加发射累积器
    ParticleSystem* system = nullptr;  // 添加系统指针
};

// 点发射器
class PointEmitter : public ParticleEmitter {
public:
    PointEmitter(const glm::vec3& position, float emissionRate = 10.0f, float spread = 1.0f, float speed = 1.0f);
    void update(float deltaTime) override;
    void emit() override;  // 改为无参数版本

    void setSpread(float spread) { this->spread = spread; }
    void setSpeed(float speed) { this->speed = speed; }

private:
    float spread;
    float speed;
};

// 粒子系统
class ParticleSystem {
public:
    ParticleSystem(size_t maxParticles = 1000);
    ~ParticleSystem();

    // 添加发射器
    void addEmitter(std::unique_ptr<ParticleEmitter> emitter);
    void addParticle(const Particle& particle);  // 添加粒子方法

    // 更新粒子系统
    void update(float deltaTime);

    // 渲染粒子
    void render(Shader& shader);  // 移除 const 限定符

    // 设置粒子属性
    void setParticleSize(float size) { particleSize = size; }
    void setParticleLife(float life) { particleLife = life; }
    void setParticleColor(const glm::vec4& color) { particleColor = color; }
    void setParticleTexture(const std::string& texturePath); // 新增：设置粒子纹理

private:
    // 粒子数据
    std::vector<Particle> particles;
    size_t maxParticles;

    // 发射器
    std::vector<std::unique_ptr<ParticleEmitter>> emitters;

    // OpenGL 对象
    unsigned int VAO, VBO, EBO, instanceVBO;  // 添加 EBO

    // 着色器
    std::unique_ptr<Shader> shader;
    unsigned int particleTexture; // 粒子纹理
    bool hasTexture;

    // 粒子属性
    float particleSize = 0.1f;
    float particleLife = 1.0f;
    glm::vec4 particleColor = glm::vec4(0.6f, 0.8f, 1.0f, 0.5f); // 默认淡蓝色，半透明

    // 辅助函数
    void initBuffers();
    void updateInstanceData();  // 改为 updateInstanceData
}; 