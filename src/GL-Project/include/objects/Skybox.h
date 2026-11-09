#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../Shader.h"

class Skybox {
public:
    Skybox(const std::vector<std::string>& dayFaces, const std::vector<std::string>& nightFaces);
    ~Skybox();

    void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    void setTimeOfDay(float time); // 0.0 到 1.0，0.0 是午夜，0.5 是正午

private:
    unsigned int dayCubemapTexture;
    unsigned int nightCubemapTexture;
    unsigned int skyboxVAO, skyboxVBO;
    float timeOfDay = 0.5f; // 默认是白天

    void loadCubemap(const std::vector<std::string>& faces, unsigned int& textureID);
    void setupSkybox();
}; 