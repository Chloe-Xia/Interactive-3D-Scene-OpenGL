#define GLM_ENABLE_EXPERIMENTAL
#include "../../include/objects/Actor.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <random>

Actor::Actor(const std::string& name) 
    : RenderableObject(name)
    , modelVAO(0)
    , path(nullptr)
    , speed(2.0f)
    , turnSpeed(45.0f)
    , currentDistance(0.0f)
    , moving(false)
    , isPaused(false)
    , isFollowingPath(false)
    , pathProgress(0.0f)
    , currentState(AnimationState::IDLE)
    , animationTime(0.0f)
    , animationSpeed(1.0f)
    , VAO(0)
    , VBO(0)
    , modelPath("")
    , animationPaths({})
    , scale(1.0f, 1.0f, 1.0f)
    , baseColor(1.0f, 1.0f, 1.0f) {
    // 随机高矮胖瘦
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> scaleXDist(0.7f, 1.3f);
    std::uniform_real_distribution<float> scaleYDist(0.8f, 1.5f);
    std::uniform_real_distribution<float> scaleZDist(0.7f, 1.3f);
    scale = glm::vec3(scaleXDist(gen), scaleYDist(gen), scaleZDist(gen));
    // 随机颜色
    std::uniform_real_distribution<float> colorDist(0.3f, 0.85f);
    baseColor = glm::vec3(colorDist(gen), colorDist(gen), colorDist(gen));
}

Actor::~Actor() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    // 清理OpenGL资源
    glDeleteVertexArrays(1, &modelVAO);
    for (unsigned int texture : animationTextures) {
        glDeleteTextures(1, &texture);
    }
}

void Actor::init() {
    // 创建一个简单的立方体作为角色的临时模型
    vertices = {
        // 前面
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        // 后面
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    // 创建并配置 VAO 和 VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 初始化模型
    initModel();
    // 加载动画
    loadAnimations();

    // 初始化关节动画参数
    shoulder_r = elbow_r = shoulder_l = elbow_l = 0.0f;
    leg_r_hip = leg_r_knee = leg_l_hip = leg_l_knee = 0.0f;
    leftArmForward = false; rightArmForward = true;
    leftLegForward = true; rightLegForward = false;
}

void Actor::render(Shader& shader) {
    if (!path) return;

    shader.use();
    shader.setBool("useCustomColor", true);  // 启用自定义颜色模式

    // 计算角色全局变换
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 position = path->getPointAtDistance(currentDistance);
    position.y += 1.2f;
    glm::vec3 direction = path->getDirectionAtDistance(currentDistance);
    model = glm::translate(model, position);
    float angle = glm::degrees(glm::atan(direction.x, direction.z));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
    model = glm::scale(model, scale * 2.5f); // 应用高矮胖瘦

    // 颜色：优先用 baseColor
    glm::vec3 bodyColor = baseColor;
    glm::vec3 headColor = glm::clamp(baseColor * 1.2f, 0.0f, 1.0f);
    glm::vec3 limbColor = baseColor * 0.8f;

    // 身体
    glm::mat4 body = glm::translate(model, glm::vec3(0, 0.5f, 0));
    shader.setVec3("customColor", bodyColor);
    drawBodyPart(shader, body, glm::vec3(0.4f, 0.7f, 0.2f));

    // 头
    glm::mat4 head = glm::translate(model, glm::vec3(0, 1.1f, 0));
    shader.setVec3("customColor", headColor);
    drawBodyPart(shader, head, glm::vec3(0.25f, 0.25f, 0.25f));

    // 右臂
    glm::mat4 rightShoulder = glm::translate(model, glm::vec3(-0.35f, 0.85f, 0));
    rightShoulder = glm::rotate(rightShoulder, glm::radians(shoulder_r), glm::vec3(1, 0, 0));
    shader.setVec3("customColor", limbColor);
    drawBodyPart(shader, rightShoulder, glm::vec3(0.12f, 0.35f, 0.12f));
    glm::mat4 rightElbow = glm::translate(rightShoulder, glm::vec3(0, -0.35f, 0));
    rightElbow = glm::rotate(rightElbow, glm::radians(elbow_r), glm::vec3(1, 0, 0));
    drawBodyPart(shader, rightElbow, glm::vec3(0.12f, 0.3f, 0.12f));

    // 左臂
    glm::mat4 leftShoulder = glm::translate(model, glm::vec3(0.35f, 0.85f, 0));
    leftShoulder = glm::rotate(leftShoulder, glm::radians(shoulder_l), glm::vec3(1, 0, 0));
    drawBodyPart(shader, leftShoulder, glm::vec3(0.12f, 0.35f, 0.12f));
    glm::mat4 leftElbow = glm::translate(leftShoulder, glm::vec3(0, -0.35f, 0));
    leftElbow = glm::rotate(leftElbow, glm::radians(elbow_l), glm::vec3(1, 0, 0));
    drawBodyPart(shader, leftElbow, glm::vec3(0.12f, 0.3f, 0.12f));

    // 右腿
    glm::mat4 rightHip = glm::translate(model, glm::vec3(-0.13f, 0.1f, 0));
    rightHip = glm::rotate(rightHip, glm::radians(leg_r_hip), glm::vec3(1, 0, 0));
    drawBodyPart(shader, rightHip, glm::vec3(0.13f, 0.4f, 0.13f));
    glm::mat4 rightKnee = glm::translate(rightHip, glm::vec3(0, -0.4f, 0));
    rightKnee = glm::rotate(rightKnee, glm::radians(leg_r_knee), glm::vec3(1, 0, 0));
    drawBodyPart(shader, rightKnee, glm::vec3(0.13f, 0.35f, 0.13f));

    // 左腿
    glm::mat4 leftHip = glm::translate(model, glm::vec3(0.13f, 0.1f, 0));
    leftHip = glm::rotate(leftHip, glm::radians(leg_l_hip), glm::vec3(1, 0, 0));
    drawBodyPart(shader, leftHip, glm::vec3(0.13f, 0.4f, 0.13f));
    glm::mat4 leftKnee = glm::translate(leftHip, glm::vec3(0, -0.4f, 0));
    leftKnee = glm::rotate(leftKnee, glm::radians(leg_l_knee), glm::vec3(1, 0, 0));
    drawBodyPart(shader, leftKnee, glm::vec3(0.13f, 0.35f, 0.13f));

    shader.setBool("useCustomColor", false);
}

void Actor::update(float deltaTime) {
    if (!path || !moving || isPaused) return;
    currentDistance += speed * deltaTime;
    if (currentDistance >= path->getPathLength()) {
        currentDistance = 0.0f; // 走完一圈回到起点，继续走
    }
    if (isFollowingPath && !isPaused) updateMovement(deltaTime);
    updateAnimation(deltaTime);

    // 新增：动画
    if (currentState == AnimationState::WALKING) {
        walkAnimation(deltaTime);
    }
}

void Actor::updateMovement(float deltaTime) {
    // 计算移动距离
    float distanceToMove = speed * deltaTime;
    
    // 更新路径进度
    float pathLength = path->getPathLength();
    float progressDelta = distanceToMove / pathLength;
    pathProgress += progressDelta;
    
    // 如果到达路径终点，停止移动
    if (pathProgress >= 1.0f) {
        pathProgress = 1.0f;
        stopMoving();
        return;
    }
    
    // 获取当前位置和方向
    glm::vec3 currentPos = path->getPointAtDistance(currentDistance);
    glm::vec3 targetDirection = path->getDirectionAtDistance(currentDistance);
    
    // 更新位置和朝向
    position = currentPos;
    updateOrientation(targetDirection);
}

void Actor::updateOrientation(const glm::vec3& targetDirection) {
    // 计算目标朝向的四元数
    glm::quat targetRotation = glm::quatLookAt(targetDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    
    // 获取当前朝向的四元数
    glm::quat currentRotation = glm::quat(glm::radians(rotation));
    
    // 使用球面线性插值进行平滑旋转
    float rotationSpeed = glm::radians(turnSpeed);
    glm::quat newRotation = glm::slerp(currentRotation, targetRotation, rotationSpeed);
    
    // 更新欧拉角
    rotation = glm::degrees(glm::eulerAngles(newRotation));
}

void Actor::updateAnimation(float deltaTime) {
    // 更新动画时间
    animationTime += deltaTime * animationSpeed;
    
    // 根据当前状态设置动画速度
    switch (currentState) {
        case AnimationState::IDLE:
            animationSpeed = 1.0f;
            break;
        case AnimationState::WALKING:
            animationSpeed = 1.0f;
            break;
        case AnimationState::RUNNING:
            animationSpeed = 1.5f;
            break;
        case AnimationState::JUMPING:
            animationSpeed = 1.2f;
            break;
    }
}

void Actor::setPath(Path* newPath) {
    path = newPath;
    currentDistance = 0.0f;
    pathProgress = 0.0f;
}

void Actor::setSpeed(float newSpeed) {
    speed = newSpeed;
}

void Actor::setTurnSpeed(float newSpeed) {
    turnSpeed = newSpeed;
}

void Actor::setAnimationState(AnimationState state) {
    currentState = state;
    animationTime = 0.0f; // 重置动画时间
}

void Actor::startMoving() {
    if (path != nullptr) {
        moving = true;
        isFollowingPath = true;
        isPaused = false;
        setAnimationState(AnimationState::WALKING);
    }
}

void Actor::stopMoving() {
    moving = false;
    isFollowingPath = false;
    setAnimationState(AnimationState::IDLE);
}

void Actor::pauseMovement() {
    isPaused = true;
    setAnimationState(AnimationState::IDLE);
}

void Actor::resumeMovement() {
    if (isFollowingPath) {
        isPaused = false;
        setAnimationState(AnimationState::WALKING);
    }
}

void Actor::resetPosition() {
    currentDistance = 0.0f;
    pathProgress = 0.0f;
}

unsigned int Actor::loadTexture(const std::string& path) {
    // TODO: 实现纹理加载
    // 这里返回0作为占位符
    return 0;
}

void Actor::initModel() {
    // TODO: 使用模型加载库（如assimp）加载3D模型
    // 这里先创建一个简单的立方体作为占位符
    float vertices[] = {
        // 位置              // 法线              // 纹理坐标
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        // ... 其他面的顶点数据 ...
    };
    
    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,
        // ... 其他面的索引数据 ...
    };
    
    glGenVertexArrays(1, &modelVAO);
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(modelVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // 设置顶点属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    
    glBindVertexArray(0);
}

void Actor::loadAnimations() {
    // 加载动画纹理
    for (const auto& path : animationPaths) {
        unsigned int textureID = loadTexture(path);
        if (textureID != 0) {
            animationTextures.push_back(textureID);
        }
    }
}

void Actor::drawBodyPart(Shader& shader, const glm::mat4& model, const glm::vec3& scale) {
    glm::mat4 m = glm::scale(model, scale);
    shader.setMat4("model", m);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
    glBindVertexArray(0);
}

void Actor::walkAnimation(float deltaTime) {
    float shoulder_speed = 60.0f * deltaTime;
    float hip_speed = 60.0f * deltaTime;

    // 右臂
    if (rightArmForward) shoulder_r += shoulder_speed;
    else shoulder_r -= shoulder_speed;
    if (shoulder_r > 30) rightArmForward = false;
    if (shoulder_r < -30) rightArmForward = true;

    // 左臂
    if (leftArmForward) shoulder_l += shoulder_speed;
    else shoulder_l -= shoulder_speed;
    if (shoulder_l > 30) leftArmForward = false;
    if (shoulder_l < -30) leftArmForward = true;

    // 右腿
    if (rightLegForward) leg_r_hip += hip_speed;
    else leg_r_hip -= hip_speed;
    if (leg_r_hip > 30) rightLegForward = false;
    if (leg_r_hip < -30) rightLegForward = true;

    // 左腿
    if (leftLegForward) leg_l_hip += hip_speed;
    else leg_l_hip -= hip_speed;
    if (leg_l_hip > 30) leftLegForward = false;
    if (leg_l_hip < -30) leftLegForward = true;
}

void Actor::setScale(const glm::vec3& s) { scale = s; }
void Actor::setColor(const glm::vec3& c) { baseColor = c; } 