#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "../Shader.h"

class RenderableObject {
public:
    RenderableObject(const std::string& name = "RenderableObject");
    virtual ~RenderableObject();

    // 初始化对象
    virtual void init() = 0;
    
    // 渲染对象
    virtual void render(Shader& shader) = 0;
    
    // 更新对象状态
    virtual void update(float deltaTime) = 0;

    // 设置变换
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);

    // 获取变换
    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getRotation() const { return rotation; }
    const glm::vec3& getScale() const { return scale; }

    // 获取对象名称
    const std::string& getName() const { return name; }

protected:
    std::string name;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    float rotationAngle = 0.0f;
    glm::vec3 scale = glm::vec3(1.0f);

    // OpenGL 对象
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    // 顶点数据
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // 材质属性
    struct Material {
        glm::vec3 ambient = glm::vec3(0.1f);
        glm::vec3 diffuse = glm::vec3(0.7f);
        glm::vec3 specular = glm::vec3(1.0f);
        float shininess = 32.0f;
    } material;

    // 纹理
    struct Texture {
        GLuint id = 0;
        std::string type;
        std::string path;
    };
    std::vector<Texture> textures;

    // 辅助函数
    void setupMesh();
    void loadTexture(const std::string& path, const std::string& type);
}; 