#pragma once

#include "./RenderableObject.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Path : public RenderableObject {
public:
    Path(const std::string& name = "Path");
    ~Path();

    // 实现基类的虚函数
    void init() override;
    void render(Shader& shader) override;
    void update(float deltaTime) override;

    // 路径操作方法
    void addPoint(const glm::vec3& point);
    void removePoint(size_t index);
    void updatePoint(size_t index, const glm::vec3& newPoint);
    void clear();

    // 路径查询方法
    glm::vec3 getPointAtDistance(float distance) const;
    glm::vec3 getDirectionAtDistance(float distance) const;
    float getPathLength() const;
    size_t getPointCount() const { return points.size(); }
    const glm::vec3& getPoint(size_t index) const { return points[index]; }
    const std::vector<glm::vec3>& getPoints() const { return points; }
    float getTotalLength() const { return totalLength; }

private:
    std::vector<glm::vec3> points;
    std::vector<float> segmentLengths;  // 每个线段的长度
    float totalLength;                  // 路径总长度
    unsigned int VAO, VBO;              // OpenGL 对象
    std::vector<float> vertices;

    void updatePathData();              // 更新路径数据（长度等）
    void updateVertices();              // 更新顶点数据
    void calculateLengths();            // 计算路径长度
}; 