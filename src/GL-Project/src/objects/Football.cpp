#include "../../include/objects/Football.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <stb_image.h>
#include <iostream>

Football::Football(float radius, const std::string& texturePath)
    : RenderableObject("Football"), 
      radius(radius),
      velocity(0.0f),
      bounciness(0.8f),
      friction(0.1f),
      isGrounded(false) {
    if (!texturePath.empty()) {
        textureID = loadTexture(texturePath);
    }
    // 设置默认材质属性
    material.ambient = glm::vec3(0.2f);
    material.diffuse = glm::vec3(0.8f);
    material.specular = glm::vec3(1.0f);
    material.shininess = 32.0f;
}

Football::~Football() {
    cleanupResources();
}

void Football::init() {
    initMesh();
}

void Football::render(Shader& shader) {
    // 更新模型矩阵
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    
    // 应用旋转（基于速度方向）
    if (glm::length(velocity) > 0.1f) {
        glm::vec3 rotationAxis = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), velocity));
        float rotationAngle = glm::length(velocity) * 0.1f; // 根据速度调整旋转角度
        model = glm::rotate(model, rotationAngle, rotationAxis);
    }
    
    // 应用缩放
    model = glm::scale(model, scale);

    // 设置着色器uniforms
    shader.use();
    shader.setMat4("model", model);
    shader.setVec3("material.ambient", material.ambient);
    shader.setVec3("material.diffuse", material.diffuse);
    shader.setVec3("material.specular", material.specular);
    shader.setFloat("material.shininess", material.shininess);

    // 绑定纹理
    if (textureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        shader.setInt("material.diffuse", 0);
        shader.setBool("material.useTexture", true);
    }

    // 渲染球体
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Football::update(float deltaTime) {
    // 应用重力
    if (!isGrounded) {
        velocity.y -= gravity * deltaTime;
    }

    // 更新位置
    position += velocity * deltaTime;

    // 地面碰撞检测
    if (position.y - radius <= 0.0f) {
        position.y = radius; // 防止陷入地面
        if (velocity.y < 0) {
            velocity.y = -velocity.y * bounciness; // 反弹
            // 应用摩擦力
            velocity.x *= (1.0f - friction);
            velocity.z *= (1.0f - friction);
        }
        isGrounded = true;
    } else {
        isGrounded = false;
    }

    // 如果速度很小，停止运动
    if (glm::length(velocity) < 0.1f && isGrounded) {
        velocity = glm::vec3(0.0f);
    }
}

void Football::applyForce(const glm::vec3& force) {
    velocity += force;
}

void Football::setVelocity(const glm::vec3& newVelocity) {
    velocity = newVelocity;
}

const glm::vec3& Football::getVelocity() const {
    return velocity;
}

void Football::setBounciness(float newBounciness) {
    bounciness = glm::clamp(newBounciness, 0.0f, 1.0f);
}

float Football::getBounciness() const {
    return bounciness;
}

void Football::setFriction(float newFriction) {
    friction = glm::clamp(newFriction, 0.0f, 1.0f);
}

float Football::getFriction() const {
    return friction;
}

void Football::setRadius(float newRadius) {
    if (newRadius > 0.0f) {
        radius = newRadius;
        updateMesh();
    }
}

float Football::getRadius() const {
    return radius;
}

void Football::initMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    generateSphereVertices(vertices, indices);

    // 创建VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // 绑定顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // 绑定索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 设置顶点属性指针
    // 位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // 法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // 纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Football::generateSphereVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    const int sectors = 32;  // 经线数
    const int stacks = 16;   // 纬线数

    // 生成顶点
    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = glm::pi<float>() * i / stacks;
        float sinStack = std::sin(stackAngle);
        float cosStack = std::cos(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = 2 * glm::pi<float>() * j / sectors;
            float sinSector = std::sin(sectorAngle);
            float cosSector = std::cos(sectorAngle);

            // 计算顶点位置
            float x = sinStack * cosSector;
            float y = cosStack;
            float z = sinStack * sinSector;

            // 计算纹理坐标
            float s = (float)j / sectors;
            float t = (float)i / stacks;

            // 创建顶点
            Vertex vertex;
            vertex.Position = glm::vec3(x, y, z) * radius;
            vertex.Normal = glm::normalize(glm::vec3(x, y, z));
            vertex.TexCoords = glm::vec2(s, t);

            vertices.push_back(vertex);
        }
    }

    // 生成索引
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = i * (sectors + 1) + j;
            int second = first + sectors + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

unsigned int Football::loadTexture(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}

void Football::updateMesh() {
    // 清理旧的资源
    cleanupResources();
    // 重新初始化网格
    initMesh();
}

void Football::cleanupResources() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (textureID != 0) glDeleteTextures(1, &textureID);
    
    VAO = VBO = EBO = textureID = 0;
} 