#include "../../include/objects/Room.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <iostream>

Room::Room(float width, float length, float height, 
           const std::string& wallTexture, 
           const std::string& floorTexture, 
           const std::string& ceilingTexture,
           const std::string& roofTexture,
           const std::string& name)
    : RenderableObject(name), width(width), length(length), height(height) {
    // 初始化变换
    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    
    // 设置房顶高度（默认为房间高度的一半）
    roofHeight = height * 0.5f;
    
    // 检查是否使用纹理
    useTextures = !wallTexture.empty() && !floorTexture.empty() && !ceilingTexture.empty() && !roofTexture.empty();
    
    // 加载纹理（如果有）
    if (useTextures) {
        wallTextureID = loadTexture(wallTexture);
        floorTextureID = loadTexture(floorTexture);
        ceilingTextureID = loadTexture(ceilingTexture);
        roofTextureID = loadTexture(roofTexture);
    } else {
        wallTextureID = 0;
        floorTextureID = 0;
        ceilingTextureID = 0;
        roofTextureID = 0;
    }
    
    // 初始化网格数据
    initMesh();
}

Room::~Room() {
    // 清理OpenGL资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    // 只在纹理ID有效时删除纹理
    if (wallTextureID != 0) glDeleteTextures(1, &wallTextureID);
    if (floorTextureID != 0) glDeleteTextures(1, &floorTextureID);
    if (ceilingTextureID != 0) glDeleteTextures(1, &ceilingTextureID);
    if (roofTextureID != 0) glDeleteTextures(1, &roofTextureID);
}

void Room::initMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    float halfWidth = width * 0.5f;
    float halfLength = length * 0.5f;

    // 地板顶点     
    vertices.push_back({{-halfWidth, 0.0f, -halfLength}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});  // 0 左下
    vertices.push_back({{ halfWidth, 0.0f, -halfLength}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}});  // 1 右下
    vertices.push_back({{ halfWidth, 0.0f,  halfLength}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}});  // 2 右上
    vertices.push_back({{-halfWidth, 0.0f,  halfLength}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});  // 3 左上

    // 天花板顶点
    vertices.push_back({{-halfWidth, height, -halfLength}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});  // 4 左下
    vertices.push_back({{ halfWidth, height, -halfLength}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}});  // 5 右下
    vertices.push_back({{ halfWidth, height,  halfLength}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}});  // 6 右上
    vertices.push_back({{-halfWidth, height,  halfLength}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}});  // 7 左上

    // 屋顶脊顶点
    vertices.push_back({{-halfWidth, height + roofHeight, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.5f}}); // 8 左脊
    vertices.push_back({{ halfWidth, height + roofHeight, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.5f}}); // 9 右脊

    // 墙壁顶点（每面4个）
    // 前墙
    vertices.push_back({{-halfWidth, 0.0f,  halfLength}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}); // 10
    vertices.push_back({{ halfWidth, 0.0f,  halfLength}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}); // 11
    vertices.push_back({{ halfWidth, height, halfLength}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}); // 12
    vertices.push_back({{-halfWidth, height, halfLength}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}); // 13
    // 后墙
    vertices.push_back({{-halfWidth, 0.0f, -halfLength}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}); // 14
    vertices.push_back({{ halfWidth, 0.0f, -halfLength}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}); // 15
    vertices.push_back({{ halfWidth, height, -halfLength}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}); // 16
    vertices.push_back({{-halfWidth, height, -halfLength}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}); // 17
    // 左墙
    vertices.push_back({{-halfWidth, 0.0f, -halfLength}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}); // 18
    vertices.push_back({{-halfWidth, 0.0f,  halfLength}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}); // 19
    vertices.push_back({{-halfWidth, height, halfLength}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}); // 20
    vertices.push_back({{-halfWidth, height, -halfLength}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}); // 21
    // 右墙
    vertices.push_back({{ halfWidth, 0.0f, -halfLength}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}); // 22
    vertices.push_back({{ halfWidth, 0.0f,  halfLength}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}); // 23
    vertices.push_back({{ halfWidth, height, halfLength}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}); // 24
    vertices.push_back({{ halfWidth, height, -halfLength}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}); // 25

    // 地板和天花板索引
    for (int i = 0; i < 2; ++i) {
        int baseIndex = i * 4;
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }

    // 屋顶索引（人字形，两侧各两个三角形）
    // 前坡面：左上(7), 右上(6), 左脊(8) 和 右上(6), 右脊(9), 左脊(8)
    indices.push_back(7); indices.push_back(6); indices.push_back(8);
    indices.push_back(6); indices.push_back(9); indices.push_back(8);
    // 后坡面：左下(4), 右下(5), 左脊(8) 和 右下(5), 9, 8
    indices.push_back(4); indices.push_back(5); indices.push_back(8);
    indices.push_back(5); indices.push_back(9); indices.push_back(8);

    // 山墙三角面
    // 左山墙
    indices.push_back(7); indices.push_back(4); indices.push_back(8);
    // 右山墙
    indices.push_back(6); indices.push_back(5); indices.push_back(9);

    // 墙壁索引（全部逆时针顺序，确保正面朝外）
    for (int i = 0; i < 4; ++i) {
        int baseIndex = 10 + i * 4;
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 3);
        indices.push_back(baseIndex + 2);
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

void Room::render(Shader& shader) {
    shader.use();
    
    // 保存当前纹理状态
    GLint activeTexture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
    
    // 计算模型矩阵
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    shader.setMat4("model", model);

    // 设置基本材质状态
    shader.setBool("material.useWorldFloorTexture", false);
    shader.setBool("material.useRoomTexture", useTextures);
    shader.setBool("material.useTexture", useTextures);

    glBindVertexArray(VAO);
    
    // 渲染地板
    shader.setVec3("material.color", floorColor);
    if (useTextures) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTextureID);
        shader.setInt("material.roomTexture", 0);
    }
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // 渲染天花板
    shader.setVec3("material.color", ceilingColor);
    if (useTextures) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ceilingTextureID);
        shader.setInt("material.roomTexture", 0);
    }
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
    
    // 渲染房顶
    shader.setVec3("material.color", roofColor);
    if (useTextures) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, roofTextureID);
        shader.setInt("material.roomTexture", 0);
    }
    // 渲染房顶坡面（4个三角形）
    for (int i = 0; i < 4; ++i) {
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)((12 + i * 3) * sizeof(unsigned int)));
    }
    // 渲染山墙三角面（2个三角形）
    for (int i = 0; i < 2; ++i) {
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)((24 + i * 3) * sizeof(unsigned int)));
    }
    
    // 渲染墙壁
    shader.setVec3("material.color", wallColor);
    if (useTextures) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTextureID);
        shader.setInt("material.roomTexture", 0);
    }
    // 渲染所有墙壁（每个墙壁6个索引，共4面墙）
    for (int i = 0; i < 4; ++i) {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((30 + i * 6) * sizeof(unsigned int)));
    }
    
    glBindVertexArray(0);
    
    // 重置纹理状态
    if (useTextures) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // 恢复之前的纹理单元
    glActiveTexture(activeTexture);
}

void Room::update(float deltaTime) {
    // 房间是静态的，不需要更新
}

unsigned int Room::loadTexture(const std::string& path) {
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
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}

void Room::setSize(float newWidth, float newLength, float newHeight) {
    width = newWidth;
    length = newLength;
    height = newHeight;
    initMesh(); // 重新初始化网格
}

void Room::init() {
    // 初始化网格数据
    initMesh();
    
    // 注意：不再在这里初始化变换，因为这些已经在构造函数中初始化
    // 如果需要在运行时重置变换，应该使用单独的方法
} 