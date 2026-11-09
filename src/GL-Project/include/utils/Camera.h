#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 相机移动方向
enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// 相机类
class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f);

    // 获取视图矩阵
    glm::mat4 GetViewMatrix() const;

    // 处理键盘输入
    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    // 处理鼠标移动
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // 处理鼠标滚轮
    void ProcessMouseScroll(float yoffset);

    // 获取相机属性
    float GetZoom() const { return Zoom; }
    glm::vec3 GetPosition() const { return Position; }
    glm::vec3 GetFront() const { return Front; }
    glm::vec3 GetUp() const { return Up; }
    glm::vec3 GetRight() const { return Right; }

    // 设置相机属性
    void SetPosition(const glm::vec3& position) { Position = position; }
    void SetFront(const glm::vec3& front) { Front = front; }
    void SetUp(const glm::vec3& up) { Up = up; }
    void SetYaw(float yaw) { Yaw = yaw; }
    void SetPitch(float pitch) { Pitch = pitch; }

private:
    // 相机属性
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // 欧拉角
    float Yaw;
    float Pitch;

    // 相机选项
    float MovementSpeed = 2.5f;
    float MouseSensitivity = 0.1f;
    float Zoom = 45.0f;

    // 更新相机向量
    void updateCameraVectors();
}; 