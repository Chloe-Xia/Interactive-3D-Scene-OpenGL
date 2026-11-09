#include "../../include/objects/Scene.h"
#include "../../include/effects/ParticleSystem.h"
#include <stdexcept>
#include <iostream>
#include <stb_image.h>

Scene::Scene() : paused(false) {
    // 移除对initWorldFloor的调用，因为我们已经不需要世界地板了
}

Scene::~Scene() {
    cleanupResources();
}

void Scene::update(float deltaTime) {
    if (paused) {
        return;
    }

    // 更新所有角色
    for (auto& [name, actor] : actors) {
        actor->update(deltaTime);
    }

    // 更新所有房间
    for (auto& room : rooms) {
        if (room) room->update(deltaTime);
    }

    // 更新所有椅子
    for (auto& chair : chairs) {
        if (chair) chair->update(deltaTime);
    }

    // 更新所有桌子
    for (auto& table : tables) {
        if (table) table->update(deltaTime);
    }

    // 更新所有足球
    for (auto& football : footballs) {
        if (football) {
            football->update(deltaTime);
            
            // 检查足球与房间的碰撞
            for (auto& room : rooms) {
                if (room) {
                    // 获取房间的边界
                    glm::vec3 roomPos = room->getPosition();
                    float roomWidth = room->getWidth();
                    float roomLength = room->getLength();
                    float roomHeight = room->getHeight();
                    
                    // 获取足球位置和半径
                    glm::vec3 ballPos = football->getPosition();
                    float ballRadius = football->getRadius();
                    
                    // 检查是否在房间内
                    if (ballPos.x + ballRadius > roomPos.x - roomWidth/2 &&
                        ballPos.x - ballRadius < roomPos.x + roomWidth/2 &&
                        ballPos.z + ballRadius > roomPos.z - roomLength/2 &&
                        ballPos.z - ballRadius < roomPos.z + roomLength/2) {
                        
                        // 如果足球碰到墙壁，反弹
                        if (ballPos.x + ballRadius > roomPos.x + roomWidth/2 ||
                            ballPos.x - ballRadius < roomPos.x - roomWidth/2) {
                            football->setVelocity(glm::vec3(-football->getVelocity().x * football->getBounciness(),
                                                           football->getVelocity().y,
                                                           football->getVelocity().z));
                        }
                        if (ballPos.z + ballRadius > roomPos.z + roomLength/2 ||
                            ballPos.z - ballRadius < roomPos.z - roomLength/2) {
                            football->setVelocity(glm::vec3(football->getVelocity().x,
                                                           football->getVelocity().y,
                                                           -football->getVelocity().z * football->getBounciness()));
                        }
                    }
                }
            }
        }
    }

    // 更新粒子系统
    this->particleSystem.update(deltaTime);
}

void Scene::render(Shader& shader) {
    // 保存当前纹理状态
    GLint activeTexture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
    
    // 渲染所有路径
    for (auto& [name, path] : paths) {
        path->render(shader);
    }

    // 渲染所有角色
    for (auto& [name, actor] : actors) {
        actor->render(shader);
    }

    // 渲染所有房间
    for (auto& room : rooms) {
        if (room) {
            // 在渲染每个房间前重置着色器状态
            shader.use();
            shader.setBool("material.useWorldFloorTexture", false);
            shader.setBool("material.useRoomTexture", false);
            shader.setBool("material.useTexture", false);
            shader.setVec3("material.color", glm::vec3(1.0f));
            room->render(shader);
        }
    }

    // 渲染所有椅子
    for (auto& chair : chairs) {
        if (chair) {
            // 在渲染每个椅子前重置着色器状态
            shader.use();
            shader.setBool("material.useWorldFloorTexture", false);
            shader.setBool("material.useRoomTexture", false);
            shader.setBool("material.useTexture", false);
            shader.setVec3("material.color", glm::vec3(1.0f));
            chair->render(shader);
        }
    }

    // 渲染所有桌子
    for (auto& table : tables) {
        if (table) {
            // 在渲染每个桌子前重置着色器状态
            shader.use();
            shader.setBool("material.useWorldFloorTexture", false);
            shader.setBool("material.useRoomTexture", false);
            shader.setBool("material.useTexture", false);
            shader.setVec3("material.color", glm::vec3(1.0f));
            table->render(shader);
        }
    }

    // 渲染所有足球
    for (auto& football : footballs) {
        if (football) {
            // 在渲染每个足球前重置着色器状态
            shader.use();
            shader.setBool("material.useWorldFloorTexture", false);
            shader.setBool("material.useRoomTexture", false);
            shader.setBool("material.useTexture", false);
            shader.setVec3("material.color", glm::vec3(1.0f));
            football->render(shader);
        }
    }

    // 渲染粒子系统
    this->particleSystem.render(shader);
}

Actor* Scene::createActor(const std::string& name) {
    if (!validateActorName(name)) {
        throw std::invalid_argument("Invalid actor name: " + name);
    }
    auto actor = std::make_unique<Actor>(name);
    actor->init();
    Actor* ptr = actor.get();
    actors[name] = std::move(actor);
    return ptr;
}

void Scene::removeActor(const std::string& name) {
    actors.erase(name);
}

Actor* Scene::getActor(const std::string& name) {
    auto it = actors.find(name);
    return it != actors.end() ? it->second.get() : nullptr;
}

void Scene::setActorPath(const std::string& actorName, const std::string& pathName) {
    Actor* actor = getActor(actorName);
    Path* path = getPath(pathName);
    
    if (!actor) {
        throw std::invalid_argument("Actor not found: " + actorName);
    }
    if (!path) {
        throw std::invalid_argument("Path not found: " + pathName);
    }
    
    actor->setPath(path);
}

Path* Scene::createPath(const std::string& name) {
    if (!validatePathName(name)) {
        throw std::invalid_argument("Invalid path name: " + name);
    }
    auto path = std::make_unique<Path>(name);
    path->init();
    Path* ptr = path.get();
    paths[name] = std::move(path);
    return ptr;
}

void Scene::removePath(const std::string& name) {
    paths.erase(name);
}

Path* Scene::getPath(const std::string& name) {
    auto it = paths.find(name);
    return it != paths.end() ? it->second.get() : nullptr;
}

void Scene::updatePath(const std::string& name, const std::vector<glm::vec3>& controlPoints) {
    Path* path = getPath(name);
    if (!path) {
        throw std::invalid_argument("Path not found: " + name);
    }
    // 更新路径的控制点
    for (const auto& point : controlPoints) {
        path->addPoint(point);
    }
}

void Scene::startAllActors() {
    for (auto& [name, actor] : actors) {
        actor->startMoving();
    }
}

void Scene::stopAllActors() {
    for (auto& [name, actor] : actors) {
        actor->stopMoving();
    }
}

void Scene::pauseAllActors() {
    for (auto& [name, actor] : actors) {
        actor->pauseMovement();
    }
}

void Scene::resumeAllActors() {
    for (auto& [name, actor] : actors) {
        actor->resumeMovement();
    }
}

bool Scene::validateActorName(const std::string& name) const {
    return !name.empty() && actors.find(name) == actors.end();
}

bool Scene::validatePathName(const std::string& name) const {
    return !name.empty() && paths.find(name) == paths.end();
}

void Scene::cleanupResources() {
    actors.clear();
    paths.clear();
    rooms.clear();
    chairs.clear();
    tables.clear();
    footballs.clear();
}

void Scene::addRoom(std::unique_ptr<Room> room) {
    if (room) {
        room->init();
        rooms.push_back(std::move(room));
    }
}

void Scene::addChair(std::unique_ptr<Chair> chair) {
    if (chair) {
        chair->init();
        chairs.push_back(std::move(chair));
    }
}

void Scene::addTable(std::unique_ptr<Table> table) {
    if (table) {
        table->init();
        tables.push_back(std::move(table));
    }
}

void Scene::addFootball(std::unique_ptr<Football> football) {
    if (football) {
        football->init();
        footballs.push_back(std::move(football));
    }
} 