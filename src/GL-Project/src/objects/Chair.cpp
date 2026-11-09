#include "../../include/objects/Chair.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Chair::Chair(float width, float depth, float height,
             const std::string& seatTexture,
             const std::string& backrestTexture,
             const std::string& legTexture)
    : RenderableObject("Chair")  // 使用初始化列表调用基类构造函数
{
    if (width <= 0.0f || depth <= 0.0f || height <= 0.0f) {
        throw std::invalid_argument("Chair dimensions must be positive");
    }
    this->width = width;
    this->depth = depth;
    this->height = height;
    seatHeight = height * 0.4f;  // 座位高度为总高度的40%
    backrestAngle = 15.0f;      // 默认靠背角度15度
    legThickness = width * 0.05f; // 椅腿粗细为宽度的5%
    seatThickness = height * 0.05f; // 座位厚度为高度的5%
    backrestThickness = width * 0.05f; // 靠背厚度为宽度的5%
    
    // 初始化变换
    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    
    // 加载纹理
    seatTextureID = loadTexture(seatTexture);
    backrestTextureID = loadTexture(backrestTexture);
    legTextureID = loadTexture(legTexture);
    
    // 设置默认椅腿位置（四个角）
    legPositions = {
        glm::vec2(legThickness, legThickness),                    // 左下
        glm::vec2(width - legThickness, legThickness),            // 右下
        glm::vec2(width - legThickness, depth - legThickness),    // 右上
        glm::vec2(legThickness, depth - legThickness)             // 左上
    };
}

void Chair::init() {
    // 初始化网格
    initMesh();
}

Chair::~Chair() {
    // 清理OpenGL资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &seatTextureID);
    glDeleteTextures(1, &backrestTextureID);
    glDeleteTextures(1, &legTextureID);
}

void Chair::initMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float halfWidth = width * 0.5f;
    float halfDepth = depth * 0.5f;
    float halfSeatThickness = seatThickness * 0.5f;
    float halfBackrestThickness = backrestThickness * 0.5f;
    
    // 创建座位顶点
    // 座位上面
    vertices.push_back({{-halfWidth, seatHeight, -halfDepth}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, seatHeight, -halfDepth}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, seatHeight,  halfDepth}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, seatHeight,  halfDepth}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 座位下面
    vertices.push_back({{-halfWidth, seatHeight - seatThickness, -halfDepth}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, seatHeight - seatThickness, -halfDepth}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, seatHeight - seatThickness,  halfDepth}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, seatHeight - seatThickness,  halfDepth}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 座位边缘
    // 前边缘
    vertices.push_back({{-halfWidth, seatHeight, -halfDepth}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, seatHeight, -halfDepth}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, seatHeight - seatThickness, -halfDepth}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, seatHeight - seatThickness, -halfDepth}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}});
    
    // 后边缘
    vertices.push_back({{-halfWidth, seatHeight, halfDepth}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, seatHeight, halfDepth}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, seatHeight - seatThickness, halfDepth}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, seatHeight - seatThickness, halfDepth}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}});
    
    // 左边缘
    vertices.push_back({{-halfWidth, seatHeight, -halfDepth}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{-halfWidth, seatHeight,  halfDepth}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{-halfWidth, seatHeight - seatThickness,  halfDepth}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, seatHeight - seatThickness, -halfDepth}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 右边缘
    vertices.push_back({{ halfWidth, seatHeight, -halfDepth}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, seatHeight,  halfDepth}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, seatHeight - seatThickness,  halfDepth}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{ halfWidth, seatHeight - seatThickness, -halfDepth}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 创建靠背顶点
    float backrestHeight = height - seatHeight;
    float backrestDepth = depth * 0.3f; // 靠背深度为座位深度的30%
    float backrestY = seatHeight + backrestHeight * 0.5f;
    float backrestZ = halfDepth - backrestDepth * 0.5f;
    
    // 靠背前面
    vertices.push_back({{-halfWidth, backrestY + backrestHeight * 0.5f, backrestZ}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, backrestY + backrestHeight * 0.5f, backrestZ}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, backrestY - backrestHeight * 0.5f, backrestZ}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, backrestY - backrestHeight * 0.5f, backrestZ}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}});
    
    // 靠背后面
    vertices.push_back({{-halfWidth, backrestY + backrestHeight * 0.5f, backrestZ + backrestThickness}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, backrestY + backrestHeight * 0.5f, backrestZ + backrestThickness}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, backrestY - backrestHeight * 0.5f, backrestZ + backrestThickness}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, backrestY - backrestHeight * 0.5f, backrestZ + backrestThickness}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}});
    
    // 靠背边缘
    // 上边缘
    vertices.push_back({{-halfWidth, backrestY + backrestHeight * 0.5f, backrestZ}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, backrestY + backrestHeight * 0.5f, backrestZ}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, backrestY + backrestHeight * 0.5f, backrestZ + backrestThickness}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, backrestY + backrestHeight * 0.5f, backrestZ + backrestThickness}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 下边缘
    vertices.push_back({{-halfWidth, backrestY - backrestHeight * 0.5f, backrestZ}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, backrestY - backrestHeight * 0.5f, backrestZ}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, backrestY - backrestHeight * 0.5f, backrestZ + backrestThickness}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, backrestY - backrestHeight * 0.5f, backrestZ + backrestThickness}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 左边缘
    vertices.push_back({{-halfWidth, backrestY + backrestHeight * 0.5f, backrestZ}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{-halfWidth, backrestY - backrestHeight * 0.5f, backrestZ}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{-halfWidth, backrestY - backrestHeight * 0.5f, backrestZ + backrestThickness}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-halfWidth, backrestY + backrestHeight * 0.5f, backrestZ + backrestThickness}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 右边缘
    vertices.push_back({{ halfWidth, backrestY + backrestHeight * 0.5f, backrestZ}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ halfWidth, backrestY - backrestHeight * 0.5f, backrestZ}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ halfWidth, backrestY - backrestHeight * 0.5f, backrestZ + backrestThickness}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{ halfWidth, backrestY + backrestHeight * 0.5f, backrestZ + backrestThickness}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}});
    
    // 为每个椅腿创建顶点
    for (const auto& legPos : legPositions) {
        float x = legPos.x - halfWidth;
        float z = legPos.y - halfDepth;
        float halfLeg = legThickness * 0.5f;
        
        // 椅腿的四个面
        // 前面
        vertices.push_back({{x - halfLeg, seatHeight - seatThickness, z - halfLeg}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
        vertices.push_back({{x + halfLeg, seatHeight - seatThickness, z - halfLeg}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}});
        vertices.push_back({{x + halfLeg, 0.0f, z - halfLeg}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}});
        vertices.push_back({{x - halfLeg, 0.0f, z - halfLeg}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}});
        
        // 后面
        vertices.push_back({{x - halfLeg, seatHeight - seatThickness, z + halfLeg}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
        vertices.push_back({{x + halfLeg, seatHeight - seatThickness, z + halfLeg}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}});
        vertices.push_back({{x + halfLeg, 0.0f, z + halfLeg}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}});
        vertices.push_back({{x - halfLeg, 0.0f, z + halfLeg}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}});
        
        // 左面
        vertices.push_back({{x - halfLeg, seatHeight - seatThickness, z - halfLeg}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
        vertices.push_back({{x - halfLeg, seatHeight - seatThickness, z + halfLeg}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}});
        vertices.push_back({{x - halfLeg, 0.0f, z + halfLeg}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}});
        vertices.push_back({{x - halfLeg, 0.0f, z - halfLeg}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}});
        
        // 右面
        vertices.push_back({{x + halfLeg, seatHeight - seatThickness, z - halfLeg}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
        vertices.push_back({{x + halfLeg, seatHeight - seatThickness, z + halfLeg}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}});
        vertices.push_back({{x + halfLeg, 0.0f, z + halfLeg}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}});
        vertices.push_back({{x + halfLeg, 0.0f, z - halfLeg}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}});
    }
    
    // 生成索引
    // 座位上下面的索引
    for (int i = 0; i < 2; ++i) {
        int baseIndex = i * 4;
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
    
    // 座位边缘的索引
    for (int i = 0; i < 4; ++i) {
        int baseIndex = 8 + i * 4;
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
    
    // 靠背的索引
    for (int i = 0; i < 6; ++i) {
        int baseIndex = 24 + i * 4;
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
    
    // 椅腿的索引
    for (int leg = 0; leg < legPositions.size(); ++leg) {
        for (int face = 0; face < 4; ++face) {
            int baseIndex = 48 + (leg * 16 + face * 4);
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 1);
            indices.push_back(baseIndex + 2);
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 2);
            indices.push_back(baseIndex + 3);
        }
    }
    
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

void Chair::render(Shader& shader) {
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
    
    // 渲染座位
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, seatTextureID);
    shader.setInt("material.diffuse", 0);
    shader.setBool("material.useTexture", true);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 座位（上下和边缘）
    
    // 渲染靠背
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backrestTextureID);
    shader.setInt("material.diffuse", 0);
    shader.setBool("material.useTexture", true);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)(36 * sizeof(unsigned int)));
    
    // 渲染椅腿
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, legTextureID);
    shader.setInt("material.diffuse", 0);
    shader.setBool("material.useTexture", true);
    glDrawElements(GL_TRIANGLES, 24 * legPositions.size(), GL_UNSIGNED_INT, (void*)(72 * sizeof(unsigned int)));
    
    glBindVertexArray(0);
}

void Chair::update(float deltaTime) {
    // 椅子是静态的，不需要更新
}

void Chair::setBackrestAngle(float angle) {
    backrestAngle = angle;
    updateMesh(); // 更新网格以反映新的靠背角度
}

void Chair::setSeatHeight(float height) {
    seatHeight = height;
    updateMesh(); // 更新网格以反映新的座位高度
}

void Chair::setLegThickness(float thickness) {
    legThickness = thickness;
    updateMesh(); // 更新网格以反映新的椅腿粗细
}

void Chair::updateMesh() {
    // 重新初始化网格以反映新的属性
    initMesh();
}

unsigned int Chair::loadTexture(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (!data) {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        // 可以在这里加载一个默认纹理
        return 0;  // 或者返回一个默认纹理ID
    }
    
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
    return textureID;
} 