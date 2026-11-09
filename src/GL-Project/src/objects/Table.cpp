#include "../../include/objects/Table.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

void Table::validateParameters(float width, float length, float height, float thickness) {
    if (width <= 0.0f || length <= 0.0f || height <= 0.0f || thickness <= 0.0f) {
        throw std::invalid_argument("Table dimensions must be positive");
    }
    if (thickness >= height) {
        throw std::invalid_argument("Table thickness must be less than height");
    }
}

Table::Table(float width, float length, float height, float topThickness, float legThickness,
             const std::string& topTexture,
             const std::string& legTexture,
             const glm::vec3& topColor,
             const glm::vec3& legColor)
    : RenderableObject("Table"), width(width), length(length), height(height),
      topThickness(topThickness), legThickness(legThickness),
      useTextures(false), topColor(topColor), legColor(legColor) {
    
    // 验证参数
    validateParameters(width, length, height, topThickness);
    
    // 初始化变换
    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    
    // 尝试加载纹理
    if (!topTexture.empty() && !legTexture.empty()) {
        try {
            topTextureID = loadTexture(topTexture);
            legTextureID = loadTexture(legTexture);
            useTextures = true;
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to load textures, falling back to colors: " << e.what() << std::endl;
            useTextures = false;
        }
    }
    
    // 设置默认桌腿位置（以桌面中心为原点）
    float halfWidth = width * 0.5f;
    float halfLength = length * 0.5f;
    legPositions = {
        glm::vec2(-halfWidth + legThickness, -halfLength + legThickness), // 左下
        glm::vec2( halfWidth - legThickness, -halfLength + legThickness), // 右下
        glm::vec2( halfWidth - legThickness,  halfLength - legThickness), // 右上
        glm::vec2(-halfWidth + legThickness,  halfLength - legThickness)  // 左上
    };
}

Table::~Table() {
    cleanupResources();
}

void Table::cleanupResources() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &topTextureID);
    glDeleteTextures(1, &legTextureID);
}

void Table::generateTableTopVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    float halfWidth = width * 0.5f;
    float halfLength = length * 0.5f;
    float halfThickness = topThickness * 0.5f;
    
    // 桌面顶点（上下两个面）
    // 上面
    vertices.push_back({{-halfWidth, height, -halfLength}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, height, -halfLength}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, height,  halfLength}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, height,  halfLength}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 下面
    vertices.push_back({{-halfWidth, height - topThickness, -halfLength}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, height - topThickness, -halfLength}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, height - topThickness,  halfLength}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, height - topThickness,  halfLength}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 桌面边缘
    // 前边缘
    vertices.push_back({{-halfWidth, height, -halfLength}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, height, -halfLength}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, height - topThickness, -halfLength}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, height - topThickness, -halfLength}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}});
    
    // 后边缘
    vertices.push_back({{-halfWidth, height, halfLength}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, height, halfLength}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, height - topThickness, halfLength}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, height - topThickness, halfLength}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}});
    
    // 左边缘
    vertices.push_back({{-halfWidth, height, -halfLength}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{-halfWidth, height,  halfLength}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{-halfWidth, height - topThickness,  halfLength}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, height - topThickness, -halfLength}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 右边缘
    vertices.push_back({{ halfWidth, height, -halfLength}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, height,  halfLength}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, height - topThickness,  halfLength}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{ halfWidth, height - topThickness, -halfLength}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 生成桌面索引
    // 上下面的索引
    for (int i = 0; i < 2; ++i) {
        int baseIndex = i * 4;
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
    
    // 边缘的索引
    for (int i = 0; i < 4; ++i) {
        int baseIndex = 8 + i * 4;
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
}

void Table::generateTableLegsVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    float halfLeg = legThickness * 0.5f;
    float texScale = 2.0f; // 控制纹理重复次数
    // 为每个桌腿创建顶点和索引
    for (const auto& legPos : legPositions) {
        float x = legPos.x;
        float z = legPos.y;
        int base = vertices.size();
        // 桌腿的四个面
        // 前面
        vertices.push_back({{x - halfLeg, height - topThickness, z - halfLeg}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
        vertices.push_back({{x + halfLeg, height - topThickness, z - halfLeg}, {0.0f, 0.0f, -1.0f}, {texScale, 0.0f}});
        vertices.push_back({{x + halfLeg, 0.0f, z - halfLeg}, {0.0f, 0.0f, -1.0f}, {texScale, texScale}});
        vertices.push_back({{x - halfLeg, 0.0f, z - halfLeg}, {0.0f, 0.0f, -1.0f}, {0.0f, texScale}});
        // 后面
        vertices.push_back({{x - halfLeg, height - topThickness, z + halfLeg}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
        vertices.push_back({{x + halfLeg, height - topThickness, z + halfLeg}, {0.0f, 0.0f, 1.0f}, {texScale, 0.0f}});
        vertices.push_back({{x + halfLeg, 0.0f, z + halfLeg}, {0.0f, 0.0f, 1.0f}, {texScale, texScale}});
        vertices.push_back({{x - halfLeg, 0.0f, z + halfLeg}, {0.0f, 0.0f, 1.0f}, {0.0f, texScale}});
        // 左面
        vertices.push_back({{x - halfLeg, height - topThickness, z - halfLeg}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
        vertices.push_back({{x - halfLeg, height - topThickness, z + halfLeg}, {-1.0f, 0.0f, 0.0f}, {texScale, 0.0f}});
        vertices.push_back({{x - halfLeg, 0.0f, z + halfLeg}, {-1.0f, 0.0f, 0.0f}, {texScale, texScale}});
        vertices.push_back({{x - halfLeg, 0.0f, z - halfLeg}, {-1.0f, 0.0f, 0.0f}, {0.0f, texScale}});
        // 右面
        vertices.push_back({{x + halfLeg, height - topThickness, z - halfLeg}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
        vertices.push_back({{x + halfLeg, height - topThickness, z + halfLeg}, {1.0f, 0.0f, 0.0f}, {texScale, 0.0f}});
        vertices.push_back({{x + halfLeg, 0.0f, z + halfLeg}, {1.0f, 0.0f, 0.0f}, {texScale, texScale}});
        vertices.push_back({{x + halfLeg, 0.0f, z - halfLeg}, {1.0f, 0.0f, 0.0f}, {0.0f, texScale}});
        // 生成索引
        for (int face = 0; face < 4; ++face) {
            int faceBase = base + face * 4;
            indices.push_back(faceBase);
            indices.push_back(faceBase + 1);
            indices.push_back(faceBase + 2);
            indices.push_back(faceBase);
            indices.push_back(faceBase + 2);
            indices.push_back(faceBase + 3);
        }
    }
}

void Table::initMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    generateTableTopVertices(vertices, indices);
    generateTableLegsVertices(vertices, indices);
    
    // 创建并绑定VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    // 设置顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    
    // 设置索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // 设置顶点属性
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

void Table::render(Shader& shader) {
    shader.use();
    
    // 设置变换矩阵
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    
    shader.setMat4("model", model);
    
    // 绑定VAO
    glBindVertexArray(VAO);
    
    // 渲染桌面
    if (useTextures) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, topTextureID);
        shader.setInt("material.diffuse", 0);
        shader.setBool("material.useTexture", true);
    } else {
        shader.setBool("material.useTexture", false);
        shader.setVec3("material.color", topColor);
    }
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 桌面（上下和边缘）
    
    // 渲染桌腿
    if (useTextures) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, legTextureID);
        shader.setInt("material.diffuse", 0);
        shader.setBool("material.useTexture", true);
    } else {
        shader.setBool("material.useTexture", false);
        shader.setVec3("material.color", legColor);
    }
    for (int i = 0; i < 4; ++i) {
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)((36 + i * 24) * sizeof(unsigned int)));
    }
    
    glBindVertexArray(0);
}

void Table::update(float deltaTime) {
    // 桌子是静态的，不需要更新
}

void Table::setLegPositions(const std::vector<glm::vec2>& positions) {
    if (positions.size() != 4) {
        throw std::invalid_argument("Table must have exactly 4 legs");
    }
    
    // 验证位置是否在桌面范围内
    for (const auto& pos : positions) {
        if (pos.x < 0.0f || pos.x > width || pos.y < 0.0f || pos.y > length) {
            throw std::invalid_argument("Leg position must be within table dimensions");
        }
    }
    
    legPositions = positions;
    updateMesh();
}

void Table::setLegThickness(float thickness) {
    if (thickness <= 0.0f) {
        throw std::invalid_argument("Leg thickness must be positive");
    }
    legThickness = thickness;
    updateMesh();
}

void Table::setTopThickness(float thickness) {
    if (thickness <= 0.0f || thickness >= height) {
        throw std::invalid_argument("Top thickness must be positive and less than table height");
    }
    topThickness = thickness;
    updateMesh();
}

void Table::updateMesh() {
    cleanupResources();
    initMesh();
}

unsigned int Table::loadTexture(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (!data) {
        glDeleteTextures(1, &textureID);
        throw std::runtime_error("Failed to load texture: " + path);
    }
    
    GLenum format;
    if (nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 3)
        format = GL_RGB;
    else if (nrComponents == 4)
        format = GL_RGBA;
    else {
        stbi_image_free(data);
        glDeleteTextures(1, &textureID);
        throw std::runtime_error("Unsupported texture format: " + std::to_string(nrComponents) + " components");
    }
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_image_free(data);
    return textureID;
}

void Table::init() {
    // 初始化网格
    initMesh();
}

void Table::setTopColor(const glm::vec3& color) {
    topColor = color;
    useTextures = false;  // 切换到颜色模式
}

void Table::setLegColor(const glm::vec3& color) {
    legColor = color;
    useTextures = false;  // 切换到颜色模式
}

void Table::setUseTextures(bool use) {
    if (use) {
        // 只有在两个纹理都成功加载的情况下才启用纹理模式
        if (topTextureID != 0 && legTextureID != 0) {
            useTextures = true;
        } else {
            std::cerr << "Warning: Cannot enable textures - textures not properly loaded" << std::endl;
            useTextures = false;
        }
    } else {
        useTextures = false;
    }
} 