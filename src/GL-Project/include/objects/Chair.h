#pragma once

#include "./RenderableObject.h"
#include "./Vertex.h"  // 包含共享的 Vertex 定义
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Chair : public RenderableObject {
public:
    // 构造函数，支持自定义椅子尺寸和纹理
    Chair(float width, float depth, float height,
          const std::string& seatTexture,
          const std::string& backrestTexture,
          const std::string& legTexture);
    ~Chair();

    // 实现基类的虚函数
    void init() override;  // 添加 init 函数声明
    void render(Shader& shader) override;  // 移除 const 限定符
    void update(float deltaTime) override;

    // 椅子特有的方法
    void setBackrestAngle(float angle); // 设置靠背角度
    void setSeatHeight(float height);   // 设置座位高度
    void setLegThickness(float thickness); // 设置椅腿粗细

private:
    // 椅子尺寸
    float width;
    float depth;
    float height;
    float seatHeight;      // 座位高度
    float backrestAngle;   // 靠背角度
    float legThickness;    // 椅腿粗细
    float seatThickness;   // 座位厚度
    float backrestThickness; // 靠背厚度

    // 椅腿位置（相对于座位左下角的偏移）
    std::vector<glm::vec2> legPositions;

    // OpenGL对象
    unsigned int VAO, VBO, EBO;
    unsigned int seatTextureID;
    unsigned int backrestTextureID;
    unsigned int legTextureID;

    // 辅助函数
    void initMesh();
    unsigned int loadTexture(const std::string& path);
    void updateMesh(); // 当椅子属性改变时更新网格
}; 