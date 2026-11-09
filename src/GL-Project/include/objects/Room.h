#pragma once

#include "./RenderableObject.h"
#include "./Vertex.h"  // 包含共享的 Vertex 定义
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Room : public RenderableObject {
public:
    Room(float width, float length, float height, 
         const std::string& wallTexture = "", // 墙纹理
         const std::string& floorTexture = "", // 地板纹理
         const std::string& ceilingTexture = "", // 天花板纹理
         const std::string& roofTexture = "",  // 房顶纹理
         const std::string& name = "Room");
    ~Room();

    // 实现基类的虚函数
    void init() override;
    void render(Shader& shader) override;
    void update(float deltaTime) override;

    // Room特有的方法
    void setSize(float width, float length, float height);
    
    // 获取房间尺寸
    float getWidth() const { return width; }
    float getLength() const { return length; }
    float getHeight() const { return height; }
    
    // 设置颜色
    void setWallColor(const glm::vec3& color) { wallColor = color; }
    void setFloorColor(const glm::vec3& color) { floorColor = color; }
    void setCeilingColor(const glm::vec3& color) { ceilingColor = color; }
    void setRoofColor(const glm::vec3& color) { roofColor = color; }  // 添加设置房顶颜色的方法

private:
    // 房间尺寸
    float width;
    float length;
    float height;
    float roofHeight;  // 房顶高度（从天花板到房顶最高点）
    
    // 颜色
    glm::vec3 wallColor = glm::vec3(0.8f, 0.8f, 0.8f);    // 默认灰色
    glm::vec3 floorColor = glm::vec3(0.5f, 0.5f, 0.5f);   // 默认深灰色
    glm::vec3 ceilingColor = glm::vec3(0.9f, 0.9f, 0.9f); // 默认浅灰色
    glm::vec3 roofColor = glm::vec3(0.7f, 0.3f, 0.3f);    // 默认红褐色
    
    // OpenGL相关
    unsigned int VAO, VBO, EBO;
    unsigned int wallTextureID;
    unsigned int floorTextureID;
    unsigned int ceilingTextureID;
    unsigned int roofTextureID;  // 添加房顶纹理ID
    bool useTextures;

    void initMesh();
    unsigned int loadTexture(const std::string& path);
}; 