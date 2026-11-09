#include "../../include/objects/Path.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp> 

Path::Path(const std::string& name) : RenderableObject(name), totalLength(0.0f) {
    VAO = VBO = 0;
}

Path::~Path() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

void Path::init() {
    // 创建并配置 VAO 和 VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Path::render(Shader& shader) {
    if (points.size() < 2) return;  // 至少需要两个点才能形成路径

    shader.use();
    
    // 设置模型矩阵
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);

    // 设置路径颜色
    shader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));  // 红色路径

    // 绘制路径
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, points.size());
    glBindVertexArray(0);
}

void Path::update(float deltaTime) {
    // 路径本身不需要更新
}

void Path::addPoint(const glm::vec3& point) {
    points.push_back(point);
    updateVertices();
    calculateLengths();
}

void Path::removePoint(size_t index) {
    if (index < points.size()) {
        points.erase(points.begin() + index);
        updateVertices();
        calculateLengths();
    }
}

void Path::updatePoint(size_t index, const glm::vec3& newPoint) {
    if (index < points.size()) {
        points[index] = newPoint;
        updateVertices();
        calculateLengths();
    }
}

void Path::clear() {
    points.clear();
    vertices.clear();
    segmentLengths.clear();
    totalLength = 0.0f;
    updateVertices();
}

float Path::getPathLength() const {
    return totalLength;
}

glm::vec3 Path::getPointAtDistance(float distance) const {
    if (points.size() < 2 || distance <= 0.0f) return points.front();
    if (distance >= totalLength) return points.back();

    // 找到距离所在的线段
    float accumulatedLength = 0.0f;
    for (size_t i = 0; i < segmentLengths.size(); ++i) {
        if (distance <= accumulatedLength + segmentLengths[i]) {
            // 计算在线段上的插值参数
            float t = (distance - accumulatedLength) / segmentLengths[i];
            return glm::mix(points[i], points[i + 1], t);
        }
        accumulatedLength += segmentLengths[i];
    }
    return points.back();
}

glm::vec3 Path::getDirectionAtDistance(float distance) const {
    if (points.size() < 2 || distance <= 0.0f) {
        return glm::normalize(points[1] - points[0]);
    }
    if (distance >= totalLength) {
        return glm::normalize(points.back() - points[points.size() - 2]);
    }

    // 找到距离所在的线段
    float accumulatedLength = 0.0f;
    for (size_t i = 0; i < segmentLengths.size(); ++i) {
        if (distance <= accumulatedLength + segmentLengths[i]) {
            return glm::normalize(points[i + 1] - points[i]);
        }
        accumulatedLength += segmentLengths[i];
    }
    return glm::normalize(points.back() - points[points.size() - 2]);
}

void Path::updateVertices() {
    vertices.clear();
    for (const auto& point : points) {
        vertices.push_back(point.x);
        vertices.push_back(point.y);
        vertices.push_back(point.z);
    }

    // 更新 VBO
    if (VBO) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Path::calculateLengths() {
    segmentLengths.clear();
    totalLength = 0.0f;

    for (size_t i = 0; i < points.size() - 1; ++i) {
        float length = glm::length(points[i + 1] - points[i]);
        segmentLengths.push_back(length);
        totalLength += length;
    }
}

void Path::updatePathData() {
    calculateLengths();
    updateVertices();
} 