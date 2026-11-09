#include "../../include/effects/LightingSystem.h"

// Light 基类实现
Light::Light(const glm::vec3& position, const glm::vec3& color)
    : position(position), color(color) {}

// PointLight 实现
PointLight::PointLight(const glm::vec3& position, const glm::vec3& color)
    : Light(position, color) {}

void PointLight::update(float deltaTime) {
    // 可以在这里添加点光源的动态效果
}

void PointLight::applyToShader(Shader& shader, int index) {
    std::string prefix = "pointLights[" + std::to_string(index) + "].";
    shader.setVec3(prefix + "position", position);
    shader.setVec3(prefix + "color", color * intensity);
    shader.setFloat(prefix + "constant", constant);
    shader.setFloat(prefix + "linear", linear);
    shader.setFloat(prefix + "quadratic", quadratic);
    shader.setFloat(prefix + "range", range);
    shader.setBool(prefix + "enabled", enabled);
}

void PointLight::setAttenuation(float constant, float linear, float quadratic) {
    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;
}

void PointLight::setRange(float range) {
    this->range = range;
}

// DirectionalLight 实现
DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec3& color)
    : Light(glm::vec3(0.0f), color), direction(glm::normalize(direction)) {}

void DirectionalLight::update(float deltaTime) {
    // 可以在这里添加方向光的动态效果
}

void DirectionalLight::applyToShader(Shader& shader, int index) {
    std::string prefix = "directionalLights[" + std::to_string(index) + "].";
    shader.setVec3(prefix + "direction", direction);
    shader.setVec3(prefix + "color", color * intensity);
    shader.setBool(prefix + "enabled", enabled);
}

// SpotLight 实现
SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color)
    : Light(position, color), direction(glm::normalize(direction)) {}

void SpotLight::update(float deltaTime) {
    // 可以在这里添加聚光灯的动态效果
}

void SpotLight::applyToShader(Shader& shader, int index) {
    std::string prefix = "spotLights[" + std::to_string(index) + "].";
    shader.setVec3(prefix + "position", position);
    shader.setVec3(prefix + "direction", direction);
    shader.setVec3(prefix + "color", color * intensity);
    shader.setFloat(prefix + "cutOff", cutOff);
    shader.setFloat(prefix + "outerCutOff", outerCutOff);
    shader.setBool(prefix + "enabled", enabled);
}

// LightingSystem 实现
LightingSystem::LightingSystem() {}

LightingSystem::~LightingSystem() {
    pointLights.clear();
    directionalLights.clear();
    spotLights.clear();
}

void LightingSystem::addPointLight(const glm::vec3& position, const glm::vec3& color) {
    if (pointLights.size() < MAX_POINT_LIGHTS) {
        pointLights.push_back(std::make_unique<PointLight>(position, color));
    }
}

void LightingSystem::addDirectionalLight(const glm::vec3& direction, const glm::vec3& color) {
    if (directionalLights.size() < MAX_DIRECTIONAL_LIGHTS) {
        directionalLights.push_back(std::make_unique<DirectionalLight>(direction, color));
    }
}

void LightingSystem::addSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color) {
    if (spotLights.size() < MAX_SPOT_LIGHTS) {
        spotLights.push_back(std::make_unique<SpotLight>(position, direction, color));
    }
}

void LightingSystem::update(float deltaTime) {
    for (auto& light : pointLights) {
        light->update(deltaTime);
    }
    for (auto& light : directionalLights) {
        light->update(deltaTime);
    }
    for (auto& light : spotLights) {
        light->update(deltaTime);
    }
}

void LightingSystem::applyToShader(Shader& shader) {
    // 设置光源数量
    shader.setInt("numPointLights", static_cast<int>(pointLights.size()));
    shader.setInt("numDirectionalLights", static_cast<int>(directionalLights.size()));
    shader.setInt("numSpotLights", static_cast<int>(spotLights.size()));

    // 应用所有光源
    for (size_t i = 0; i < pointLights.size(); ++i) {
        pointLights[i]->applyToShader(shader, static_cast<int>(i));
    }
    for (size_t i = 0; i < directionalLights.size(); ++i) {
        directionalLights[i]->applyToShader(shader, static_cast<int>(i));
    }
    for (size_t i = 0; i < spotLights.size(); ++i) {
        spotLights[i]->applyToShader(shader, static_cast<int>(i));
    }
} 