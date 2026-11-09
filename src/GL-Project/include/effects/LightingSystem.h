#pragma once

#include "../Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

// 光源基类
class Light {
public:
    Light(const glm::vec3& position, const glm::vec3& color);
    virtual ~Light() = default;

    virtual void update(float deltaTime) = 0;
    virtual void applyToShader(Shader& shader, int index) = 0;

    // 设置光源属性
    void setPosition(const glm::vec3& position) { this->position = position; }
    void setColor(const glm::vec3& color) { this->color = color; }
    void setIntensity(float intensity) { this->intensity = intensity; }
    void setEnabled(bool enabled) { this->enabled = enabled; }

    // 获取光源属性
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getColor() const { return color; }
    float getIntensity() const { return intensity; }
    bool isEnabled() const { return enabled; }

protected:
    glm::vec3 position;
    glm::vec3 color;
    float intensity = 1.0f;
    bool enabled = true;
};

// 点光源
class PointLight : public Light {
public:
    PointLight(const glm::vec3& position, const glm::vec3& color);
    void update(float deltaTime) override;
    void applyToShader(Shader& shader, int index) override;

    // 设置点光源特定属性
    void setAttenuation(float constant, float linear, float quadratic);
    void setRange(float range);

private:
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float range = 50.0f;
};

// 方向光
class DirectionalLight : public Light {
public:
    DirectionalLight(const glm::vec3& direction, const glm::vec3& color);
    void update(float deltaTime) override;
    void applyToShader(Shader& shader, int index) override;

    void setDirection(const glm::vec3& direction) { this->direction = direction; }
    glm::vec3 getDirection() const { return direction; }

private:
    glm::vec3 direction;
};

// 聚光灯
class SpotLight : public Light {
public:
    SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color);
    void update(float deltaTime) override;
    void applyToShader(Shader& shader, int index) override;

    void setDirection(const glm::vec3& direction) { this->direction = direction; }
    void setCutOff(float cutOff) { this->cutOff = cutOff; }
    void setOuterCutOff(float outerCutOff) { this->outerCutOff = outerCutOff; }

private:
    glm::vec3 direction;
    float cutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(17.5f));
};

// 光照系统
class LightingSystem {
public:
    LightingSystem();
    ~LightingSystem();

    // 添加光源
    void addPointLight(const glm::vec3& position, const glm::vec3& color);
    void addDirectionalLight(const glm::vec3& direction, const glm::vec3& color);
    void addSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color);

    // 更新所有光源
    void update(float deltaTime);

    // 应用所有光源到着色器
    void applyToShader(Shader& shader);

    // 获取光源数量
    size_t getPointLightCount() const { return pointLights.size(); }
    size_t getDirectionalLightCount() const { return directionalLights.size(); }
    size_t getSpotLightCount() const { return spotLights.size(); }

private:
    std::vector<std::unique_ptr<PointLight>> pointLights;
    std::vector<std::unique_ptr<DirectionalLight>> directionalLights;
    std::vector<std::unique_ptr<SpotLight>> spotLights;

    // 最大光源数量
    static constexpr size_t MAX_POINT_LIGHTS = 32;
    static constexpr size_t MAX_DIRECTIONAL_LIGHTS = 4;
    static constexpr size_t MAX_SPOT_LIGHTS = 8;
}; 