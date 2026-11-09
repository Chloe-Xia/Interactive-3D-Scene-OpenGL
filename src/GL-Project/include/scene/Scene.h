#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

// 前向声明
class Actor;
class Path;
class Room;
class Shader;
class ParticleEmitter;

// 包含完整的 ParticleSystem 定义
#include "../effects/ParticleSystem.h"

// 包含其他必要的头文件
#include "../objects/Actor.h"
#include "../objects/Path.h"
#include "../objects/Room.h"
#include "../Shader.h"

class Scene {
public:
    Scene();
    ~Scene();

    // 场景管理方法
    void update(float deltaTime);
    void render(Shader& shader);
    
    // Actor管理
    Actor* createActor(const std::string& name = "Actor");
    void removeActor(const std::string& name);
    Actor* getActor(const std::string& name);
    void setActorPath(const std::string& actorName, const std::string& pathName);
    
    // Path管理
    Path* createPath(const std::string& name = "Path");
    void removePath(const std::string& name);
    Path* getPath(const std::string& name);
    void updatePath(const std::string& name, const std::vector<glm::vec3>& controlPoints);
    
    // Room管理
    void setRoom(std::unique_ptr<Room> room);
    Room* getRoom() { return room.get(); }
    
    // 粒子系统管理
    ParticleSystem& getParticleSystem() { return particleSystem; }
    void addParticleEmitter(std::unique_ptr<ParticleEmitter> emitter) {
        particleSystem.addEmitter(std::move(emitter));
    }
    
    // 场景控制
    void startAllActors();
    void stopAllActors();
    void pauseAllActors();
    void resumeAllActors();
    
    // 场景状态
    bool isPaused() const { return paused; }
    void setPaused(bool pause) { paused = pause; }

private:
    // 场景对象
    std::unordered_map<std::string, std::unique_ptr<Actor>> actors;
    std::unordered_map<std::string, std::unique_ptr<Path>> paths;
    std::unique_ptr<Room> room;
    ParticleSystem particleSystem;  // 添加粒子系统
    
    // 场景状态
    bool paused;
    
    // 辅助方法
    bool validateActorName(const std::string& name) const;
    bool validatePathName(const std::string& name) const;
    void cleanupResources();
}; 