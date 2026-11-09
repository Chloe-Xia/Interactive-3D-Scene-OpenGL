#pragma once

#include "./Actor.h"
#include "./Path.h"
#include "./Room.h"
#include "./Chair.h"
#include "./Table.h"
#include "./Football.h"
#include "./effects/ParticleSystem.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

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
    void addRoom(std::unique_ptr<Room> room);
    std::vector<std::unique_ptr<Room>> rooms;
    
    // Chair管理
    void addChair(std::unique_ptr<Chair> chair);
    std::vector<std::unique_ptr<Chair>> chairs;
    
    // Table管理
    void addTable(std::unique_ptr<Table> table);
    std::vector<std::unique_ptr<Table>> tables;
    
    // Football管理
    void addFootball(std::unique_ptr<Football> football);
    std::vector<std::unique_ptr<Football>> footballs;
    
    // 场景控制
    void startAllActors();
    void stopAllActors();
    void pauseAllActors();
    void resumeAllActors();
    
    // 场景状态
    bool isPaused() const { return paused; }
    void setPaused(bool pause) { paused = pause; }

    ParticleSystem& getParticleSystem() { return particleSystem; }

private:
    // 场景对象
    std::unordered_map<std::string, std::unique_ptr<Actor>> actors;
    std::unordered_map<std::string, std::unique_ptr<Path>> paths;
    
    // 场景状态
    bool paused;
    
    unsigned int worldFloorVAO = 0, worldFloorVBO = 0, worldFloorEBO = 0, worldFloorTexture = 0;
    void initWorldFloor();
    
    // 辅助方法
    bool validateActorName(const std::string& name) const;
    bool validatePathName(const std::string& name) const;
    void cleanupResources();

    ParticleSystem particleSystem;

    unsigned int loadTexture(const std::string& path);
}; 