#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <cstring>
#include "include/objects/Room.h"
#include "include/Shader.h"
#include "include/objects/Scene.h"
#include "include/objects/Chair.h"
#include "include/objects/Table.h"
#include "include/objects/Skybox.h"
#include "include/effects/ParticleSystem.h"
#include <random>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Shader* shader = nullptr;
Shader* skyboxShader = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void saveScreenshot(const char* filename, int width, int height);

glm::vec3 cameraPos   = glm::vec3(0.0f, 2.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 0.1f;

float yaw   = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float dayTime = 0.5f;
float daySpeed = 0.01f;

float roomHeightBase = 6.0f;
float roomHeightVar = 2.0f;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> heightDist(-roomHeightVar, roomHeightVar);

bool spacePressed = false;
bool lastSpacePressed = false;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG Project", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    shader = new Shader("shaders/basic.vs", "shaders/basic.fs");
    skyboxShader = new Shader("shaders/skybox.vs", "shaders/skybox.fs");

    std::unique_ptr<Skybox> skybox;
    try {
        std::vector<std::string> dayFaces = {
            "textures/skybox/day/right.png",
            "textures/skybox/day/left.png",
            "textures/skybox/day/top.png",
            "textures/skybox/day/bottom.png",
            "textures/skybox/day/front.png",
            "textures/skybox/day/back.png"
        };
        std::vector<std::string> nightFaces = {
            "textures/skybox/night/right.png",
            "textures/skybox/night/left.png",
            "textures/skybox/night/top.png",
            "textures/skybox/night/bottom.png",
            "textures/skybox/night/front.png",
            "textures/skybox/night/back.png"
        };
        skybox = std::make_unique<Skybox>(dayFaces, nightFaces);
    } catch (const std::exception& e) {
        std::cerr << "Failed to create skybox: " << e.what() << std::endl;
        std::cerr << "Continuing without skybox..." << std::endl;
    }

    Scene scene;
    
    auto football = std::make_unique<Football>(0.2f, "textures/football_texture.jpg");
    football->setPosition(glm::vec3(0.0f, 1.0f, 10.0f));
    football->setBounciness(0.8f);
    football->setFriction(0.1f);
    scene.addFootball(std::move(football));
    
    scene.getParticleSystem().addEmitter(
        std::make_unique<PointEmitter>(
            glm::vec3(0.0f, 1.0f, 0.0f),
            100.0f,
            0.5f,
            1.4f
        )
    );
    
    float outerRadius = 25.0f;
    int numOuterFires = 12;
    for (int i = 0; i < numOuterFires; ++i) {
        float angle = (2.0f * glm::pi<float>() * i) / numOuterFires;
        float x = outerRadius * cos(angle);
        float z = outerRadius * sin(angle);
        
        float emissionRate = 70.0f + (rand() % 50);
        float spread = 0.35f + (rand() % 25) / 100.0f;
        float speed = 1.1f + (rand() % 50) / 100.0f;
        
        scene.getParticleSystem().addEmitter(
            std::make_unique<PointEmitter>(
                glm::vec3(x, 1.0f, z),
                emissionRate,
                spread,
                speed
            )
        );
    }
    
    float middleRadius = 15.0f;
    int numMiddleFires = 8;
    for (int i = 0; i < numMiddleFires; ++i) {
        float angle = (2.0f * glm::pi<float>() * i) / numMiddleFires;
        float x = middleRadius * cos(angle);
        float z = middleRadius * sin(angle);
        
        float emissionRate = 60.0f + (rand() % 40);
        float spread = 0.3f + (rand() % 20) / 100.0f;
        float speed = 1.0f + (rand() % 40) / 100.0f;
        
        scene.getParticleSystem().addEmitter(
            std::make_unique<PointEmitter>(
                glm::vec3(x, 1.0f, z),
                emissionRate,
                spread,
                speed
            )
        );
    }
    
    float spacing = 15.0f;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 5; ++col) {
            float x = (col - 2) * spacing;
            float z = (row - 1) * spacing;
            
            if (rand() % 3 == 0) {
                float emissionRate = 50.0f + (rand() % 30);
                float spread = 0.25f + (rand() % 15) / 100.0f;
                float speed = 0.9f + (rand() % 30) / 100.0f;
                
                scene.getParticleSystem().addEmitter(
                    std::make_unique<PointEmitter>(
                        glm::vec3(x, 1.0f, z),
                        emissionRate,
                        spread,
                        speed
                    )
                );
            }
        }
    }
    
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            float x = (col - 1.5f) * spacing;
            float z = (row - 1.5f) * spacing;
            
            if (rand() % 3 == 0) {
                float emissionRate = 50.0f + (rand() % 30);
                float spread = 0.25f + (rand() % 15) / 100.0f;
                float speed = 0.9f + (rand() % 30) / 100.0f;
                
                scene.getParticleSystem().addEmitter(
                    std::make_unique<PointEmitter>(
                        glm::vec3(x, 1.0f, z),
                        emissionRate,
                        spread,
                        speed
                    )
                );
            }
        }
    }
    
    float roomWidth = 10.0f;
    float roomLength = 10.0f;
    
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 5; ++col) {
            float x = (col - 2) * spacing;
            float z = (row - 1.5f) * spacing;

            float roomHeight = roomHeightBase + heightDist(gen);

            auto room = std::make_unique<Room>(roomWidth, roomLength, roomHeight,
                                               "textures/wall.jpg",
                                               "textures/floor.jpg",
                                               "textures/ceiling.jpg",
                                               "textures/roof.jpg",
                                               "Room_" + std::to_string(row * 5 + col));
            room->setPosition(glm::vec3(x, 0.0f, z));
            room->setRoofColor(glm::vec3(0.4f, 0.4f, 0.4f));
            scene.addRoom(std::move(room));
            
            float chairWidth = 0.6f;
            float chairDepth = 0.6f;
            float chairHeight = 1.0f;
            float chairOffset = 1.0f;
            
            std::vector<glm::vec3> chairPositions = {
                glm::vec3(x - roomWidth/2 + chairOffset, 0.0f, z - roomLength/2 + chairOffset),
                glm::vec3(x + roomWidth/2 - chairOffset, 0.0f, z - roomLength/2 + chairOffset),
                glm::vec3(x + roomWidth/2 - chairOffset, 0.0f, z + roomLength/2 - chairOffset),
                glm::vec3(x - roomWidth/2 + chairOffset, 0.0f, z + roomLength/2 - chairOffset)
            };
            
            std::vector<float> chairRotations = {
                45.0f,
                135.0f,
                225.0f,
                315.0f
            };
            
            for (size_t i = 0; i < chairPositions.size(); ++i) {
                auto chair = std::make_unique<Chair>(chairWidth, chairDepth, chairHeight,
                                                   "textures/chair_seat.jpg",
                                                   "textures/chair_back.jpg",
                                                   "textures/chair_leg.jpg");
                
                chair->setPosition(chairPositions[i]);
                chair->setRotation(glm::vec3(0.0f, chairRotations[i], 0.0f));
                scene.addChair(std::move(chair));
            }

            float tableWidth = 1.2f;
            float tableLength = 1.2f;
            float tableHeight = 0.75f;
            float tableTopThickness = 0.05f;
            float tableLegThickness = 0.05f;

            auto table = std::make_unique<Table>(
                tableWidth, tableLength, tableHeight,
                tableTopThickness, tableLegThickness,
                "", "",
                glm::vec3(0.8f, 0.6f, 0.4f),
                glm::vec3(0.4f, 0.2f, 0.1f)
            );
            
            table->setPosition(glm::vec3(x, 0.0f, z));
            table->setRotation(glm::vec3(0.0f, (row + col) * 15.0f, 0.0f));
            scene.addTable(std::move(table));
        }
    }

    auto path = scene.createPath("actorPath");
    float L = 40.0f;
    path->addPoint(glm::vec3( L, 0,  L));
    path->addPoint(glm::vec3(-L, 0,  L));
    path->addPoint(glm::vec3(-L, 0, -L));
    path->addPoint(glm::vec3( L, 0, -L));
    path->addPoint(glm::vec3( L, 0,  L));
    path->init();

    for (int i = 0; i < 20; ++i) {
        std::string name = "robot" + std::to_string(i);
        Actor* actor = scene.createActor(name);
        actor->setPath(path);
        actor->startMoving();
        actor->resetPosition();
        actor->setSpeed(2.0f + 0.2f * i);
        actor->setCurrentDistance((path->getPathLength() / 20.0f) * i);
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        bool currentSpacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (currentSpacePressed && !lastSpacePressed) {
            for (auto& football : scene.footballs) {
                if (glm::distance(football->getPosition(), cameraPos) < 3.0f) {
                    glm::vec3 kickDirection = glm::normalize(cameraFront + glm::vec3(0.0f, 0.5f, 0.0f));
                    football->setVelocity(kickDirection * 10.0f);
                }
            }
        }
        lastSpacePressed = currentSpacePressed;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
        );
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f
        );

        shader->use();
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        
        float deltaTime = 0.016f;
        scene.update(deltaTime);
        scene.render(*shader);

        scene.getParticleSystem().render(*shader);

        dayTime += daySpeed * deltaTime;
        if (dayTime > 1.0f) dayTime -= 1.0f;
        if (skybox) {
            skybox->setTimeOfDay(dayTime);
        }

        if (skybox) {
            glDepthFunc(GL_LEQUAL);
            skybox->render(*skyboxShader, view, projection);
            glDepthFunc(GL_LESS);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete shader;
    delete skyboxShader;
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    static bool oPressed = false;
    static bool f11Pressed = false;
    static bool isFullscreen = false;
    static int windowedX = 0, windowedY = 0, windowedW = SCR_WIDTH, windowedH = SCR_HEIGHT;
    static GLFWmonitor* monitor = nullptr;
    static const GLFWvidmode* mode = nullptr;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        if (!oPressed) {
            saveScreenshot("screenshot.png", SCR_WIDTH, SCR_HEIGHT);
            std::cout << "Screenshot saved as screenshot.png" << std::endl;
            oPressed = true;
        }
    } else {
        oPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        if (!f11Pressed) {
            f11Pressed = true;
            if (!isFullscreen) {
                monitor = glfwGetPrimaryMonitor();
                mode = glfwGetVideoMode(monitor);
                glfwGetWindowPos(window, &windowedX, &windowedY);
                glfwGetWindowSize(window, &windowedW, &windowedH);
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                isFullscreen = true;
            } else {
                glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedW, windowedH, 0);
                isFullscreen = false;
            }
        }
    } else {
        f11Pressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        daySpeed = glm::min(daySpeed + 0.01f, 1.0f);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        daySpeed = glm::max(daySpeed - 0.01f, 0.0f);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void saveScreenshot(const char* filename, int width, int height) {
    std::vector<unsigned char> pixels(3 * width * height);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    std::vector<unsigned char> flipped(3 * width * height);
    for (int y = 0; y < height; ++y) {
        memcpy(&flipped[y * width * 3], &pixels[(height - 1 - y) * width * 3], width * 3);
    }
    stbi_write_png(filename, width, height, 3, flipped.data(), width * 3);
}

