#pragma once

#include "./RenderableObject.h"
#include "./Path.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

// 定义动画状态
enum class AnimationState {
    IDLE,
    WALKING,
    RUNNING,
    JUMPING
};

class Actor : public RenderableObject {
public:
    // 构造函数，支持自定义模型和动画
    Actor(const std::string& name = "Actor");
    virtual ~Actor();

    // 实现基类的虚函数
    void init() override;
    void render(Shader& shader) override;
    void update(float deltaTime) override;

    // Actor特有的方法
    void setPath(Path* path); // 设置要跟随的路径
    void setSpeed(float speed); // 设置移动速度
    void setTurnSpeed(float speed); // 设置转向速度
    void setAnimationState(AnimationState state); // 设置动画状态
    void setScale(const glm::vec3& s);
    void setColor(const glm::vec3& c);
    
    // 获取Actor属性
    float getSpeed() const { return speed; }
    float getTurnSpeed() const { return turnSpeed; }
    AnimationState getAnimationState() const { return currentState; }
    bool isMoving() const { return moving; }

    // 控制方法
    void startMoving(); // 开始沿路径移动
    void stopMoving(); // 停止移动
    void pauseMovement();
    void resumeMovement();
    void resetPosition(); // 重置位置
    void setCurrentDistance(float d) { currentDistance = d; }

private:
    // 模型和动画相关
    unsigned int modelVAO;
    std::vector<unsigned int> animationTextures;
    std::string modelPath;
    std::vector<std::string> animationPaths;
    
    // 移动相关
    Path* path;
    float speed;
    float turnSpeed;
    float currentDistance;  // 当前在路径上的距离
    bool moving;           // 改名为 moving
    bool isPaused;
    bool isFollowingPath;   // 是否正在跟随路径
    float pathProgress;     // 路径进度（0-1）
    
    // 动画相关
    AnimationState currentState;
    float animationTime;
    float animationSpeed;
    
    // OpenGL 相关
    unsigned int VAO, VBO;
    std::vector<float> vertices;  // 用于渲染的顶点数据

    // 机器人关节动画相关
    float shoulder_r, elbow_r, shoulder_l, elbow_l;
    float leg_r_hip, leg_r_knee, leg_l_hip, leg_l_knee;
    bool leftArmForward, rightArmForward, leftLegForward, rightLegForward;

    // 内部方法
    void initModel();
    void loadAnimations();
    void updateAnimation(float deltaTime);
    void updateVertices();  // 更新顶点数据
    void updateMovement(float deltaTime);
    void updateOrientation(const glm::vec3& targetDirection);
    unsigned int loadTexture(const std::string& path);
    void walkAnimation(float deltaTime); // 新增动画函数
    void drawBodyPart(Shader& shader, const glm::mat4& model, const glm::vec3& scale);

    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 baseColor = glm::vec3(1.0f);
}; 