#pragma once

#include "./RenderableObject.h"
#include "./Vertex.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Football : public RenderableObject {
public:
    // Constructor with radius and texture
    Football(float radius = 1.0f, const std::string& texturePath = "");
    ~Football();

    // Implement base class virtual functions
    void init() override;
    void render(Shader& shader) override;
    void update(float deltaTime) override;

    // Football specific methods
    void setRadius(float newRadius);
    float getRadius() const;
    
    // Physics methods
    void applyForce(const glm::vec3& force);
    void setVelocity(const glm::vec3& newVelocity);
    const glm::vec3& getVelocity() const;
    
    // Material properties
    void setBounciness(float newBounciness);
    float getBounciness() const;
    void setFriction(float newFriction);
    float getFriction() const;

private:
    // Football properties
    float radius;
    glm::vec3 velocity;
    float bounciness;
    float friction;
    static constexpr float gravity = 9.8f;
    bool isGrounded;

    // OpenGL objects
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int textureID = 0;
    std::vector<unsigned int> indices;
    bool useTexture;

    // Helper functions
    void initMesh();
    void generateSphereVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    unsigned int loadTexture(const std::string& path);
    void updateMesh();
    void cleanupResources();
}; 