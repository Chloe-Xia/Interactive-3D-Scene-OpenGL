#pragma once

#include "RenderableObject.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <stdexcept>
#include <iostream>
#include "Vertex.h"

class Table : public RenderableObject {
public:
    // 构造函数，支持自定义桌子尺寸和纹理或颜色
    Table(float width, float length, float height, float topThickness, float legThickness,
          const std::string& topTexture = "",
          const std::string& legTexture = "",
          const glm::vec3& topColor = glm::vec3(0.8f, 0.6f, 0.4f),  // 默认桌面颜色（木色）
          const glm::vec3& legColor = glm::vec3(0.4f, 0.2f, 0.1f)); // 默认桌腿颜色（深木色）
    ~Table();

    // 实现基类的虚函数
    void init() override;
    void render(Shader& shader) override;
    void update(float deltaTime) override;

    // 桌子特有的方法
    void setLegPositions(const std::vector<glm::vec2>& positions);
    void setLegThickness(float thickness);
    void setTopThickness(float thickness);
    
    // 设置颜色和纹理
    void setTopColor(const glm::vec3& color);
    void setLegColor(const glm::vec3& color);
    void setUseTextures(bool use);
    bool isUsingTextures() const { return useTextures; }

    // 获取桌子属性
    float getWidth() const { return width; }
    float getLength() const { return length; }
    float getHeight() const { return height; }
    float getTopThickness() const { return topThickness; }
    float getLegThickness() const { return legThickness; }

private:
    // 桌子尺寸
    float width;
    float length;
    float height;
    float topThickness;
    float legThickness;

    // 桌腿位置（相对于桌面左下角的偏移）
    std::vector<glm::vec2> legPositions;

    // OpenGL对象
    unsigned int VAO, VBO, EBO;
    unsigned int topTextureID;
    unsigned int legTextureID;
    
    // 材质属性
    bool useTextures;
    glm::vec3 topColor;
    glm::vec3 legColor;

    // 辅助函数
    void initMesh();
    void generateTableTopVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    void generateTableLegsVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    unsigned int loadTexture(const std::string& path);
    void updateMesh(); // 当桌腿位置改变时更新网格
    void cleanupResources(); // 清理OpenGL资源
    void validateParameters(float width, float length, float height, float thickness); // 验证参数
}; 