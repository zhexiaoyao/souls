#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "FragmentEffectManager.h"
#include "ObjectManager.h"
#include "SceneNode.h"
#include "Material.h"
#include "../geometry/Cube.h"
#include "../geometry/Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>

namespace SoulsEngine {

FragmentEffectManager::FragmentEffectManager(ObjectManager* objectManager)
    : m_objectManager(objectManager)
    , m_gen(m_rd()) {
}

FragmentEffectManager::~FragmentEffectManager() {
    // Clean up all fragments
    for (auto& fragment : m_fragments) {
        if (fragment.node && fragment.isActive) {
            m_objectManager->RemoveNode(fragment.node);
        }
    }
    m_fragments.clear();

    // Restore all fragmenting objects
    for (auto& state : m_fragmentingObjects) {
        if (state.isFragmenting) {
            RestoreObject(state);
        }
    }
    m_fragmentingObjects.clear();
}

void FragmentEffectManager::StartFragmentation(std::shared_ptr<SceneNode> node) {
    if (!node || !m_objectManager) return;

    // Check if already fragmenting
    for (auto& state : m_fragmentingObjects) {
        if (state.node == node && state.isFragmenting) {
            return;  // Already fragmenting
        }
    }

    // Save object state
    ObjectState state;
    state.node = node;
    state.position = node->GetPosition();
    state.rotation = node->GetRotation();
    glm::vec3 scaleVec = node->GetScale();
    state.scale = scaleVec;
    
    // Get original mesh and material
    state.originalMesh = node->GetMesh();
    state.originalMaterial = node->GetMaterial();
    
    state.isFragmenting = true;
    state.fragmentTimer = 0.0f;

    // Hide original object
    node->SetScale(0.0f, 0.0f, 0.0f);

    // Create fragments
    CreateFragments(state);

    // Add to fragmenting objects list
    m_fragmentingObjects.push_back(state);

    std::cout << "Started fragmentation for: " << node->GetName() << std::endl;
}

void FragmentEffectManager::CreateFragments(ObjectState& state) {
    if (!state.node) return;

    // Calculate object bounds (approximate)
    glm::vec3 objPos = state.position;
    
    // Estimate object size from scale - ensure minimum size
    float objSize = glm::max(glm::max(state.scale.x, state.scale.y), state.scale.z);
    if (objSize < 0.1f) objSize = 1.0f;  // Default size if too small
    
    // Fragment size should be visible - use larger fragments
    float fragmentSize = glm::max(objSize * 0.2f, 0.15f);  // At least 0.15 units
    
    // Create fragments in a spherical explosion pattern
    for (int i = 0; i < FRAGMENT_COUNT; ++i) {
        // Random spherical coordinates for explosion pattern
        std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265359f);
        std::uniform_real_distribution<float> elevationDist(-3.14159265359f / 3.0f, 3.14159265359f / 3.0f);  // -60 to 60 degrees
        std::uniform_real_distribution<float> radiusDist(0.1f * objSize, 0.3f * objSize);  // Start close to center
        
        float azimuth = angleDist(m_gen);  // Horizontal angle
        float elevation = elevationDist(m_gen);  // Vertical angle
        
        // Calculate direction vector (spherical to cartesian)
        float cosElev = cos(elevation);
        glm::vec3 direction(
            cosElev * cos(azimuth),
            sin(elevation),
            cosElev * sin(azimuth)
        );
        direction = glm::normalize(direction);
        
        // Start position slightly offset from center
        float startRadius = radiusDist(m_gen);
        glm::vec3 worldPos = objPos + direction * startRadius;
        
        // Create fragment mesh (small cube) - use object's color if available
        glm::vec3 fragmentColor(0.8f, 0.2f, 0.2f);  // Default reddish color
        if (state.originalMaterial) {
            fragmentColor = state.originalMaterial->GetDiffuse();
        }
        auto fragmentMesh = std::make_shared<Cube>(fragmentSize, fragmentColor);
        
        // Use object's name for fragment naming
        std::string fragmentName = "Fragment_" + state.node->GetName() + "_" + std::to_string(i);
        auto fragmentNode = m_objectManager->CreateNode(fragmentName, fragmentMesh);
        
        // Set initial position
        fragmentNode->SetPosition(worldPos);
        
        // Random initial rotation
        std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
        fragmentNode->SetRotation(rotDist(m_gen), rotDist(m_gen), rotDist(m_gen));
        
        // Calculate velocity (explosion effect from object center)
        std::uniform_real_distribution<float> speedDist(4.0f, 8.0f);  // Faster speed
        float speed = speedDist(m_gen);
        glm::vec3 velocity = direction * speed;
        
        // Add some random upward velocity for more dynamic effect
        std::uniform_real_distribution<float> upVelDist(1.5f, 4.0f);
        velocity.y += upVelDist(m_gen);
        
        // Random angular velocity (degrees per second)
        std::uniform_real_distribution<float> angVelDist(-720.0f, 720.0f);  // Faster rotation
        glm::vec3 angularVelocity(
            angVelDist(m_gen),
            angVelDist(m_gen),
            angVelDist(m_gen)
        );
        
        // Create fragment
        EditorFragment fragment;
        fragment.node = fragmentNode;
        fragment.position = worldPos;
        fragment.velocity = velocity;
        fragment.angularVelocity = angularVelocity;
        fragment.scale = 1.0f;
        fragment.lifetime = FRAGMENT_LIFETIME;
        fragment.isActive = true;
        
        m_fragments.push_back(fragment);
    }
    
    std::cout << "Created " << FRAGMENT_COUNT << " fragments for: " << state.node->GetName() << std::endl;
}

void FragmentEffectManager::UpdateFragments(float deltaTime) {
    // Update each fragment
    for (auto it = m_fragments.begin(); it != m_fragments.end();) {
        if (!it->isActive) {
            ++it;
            continue;
        }
        
        // Update lifetime
        it->lifetime -= deltaTime;
        
        // Update position (apply velocity and gravity)
        it->velocity.y += GRAVITY * deltaTime;
        it->position += it->velocity * deltaTime;
        
        // Update rotation
        glm::vec3 currentRotation = it->node->GetRotation();
        currentRotation += it->angularVelocity * deltaTime;
        it->node->SetRotation(currentRotation);
        
        // Update scale (gradually shrink)
        it->scale = (std::max)(0.0f, it->scale - FRAGMENT_SHRINK_SPEED * deltaTime);
        it->node->SetScale(it->scale, it->scale, it->scale);
        
        // Update position
        it->node->SetPosition(it->position);
        
        // Remove fragment if lifetime expired or scale is 0
        if (it->lifetime <= 0.0f || it->scale <= 0.0f) {
            m_objectManager->RemoveNode(it->node);
            it = m_fragments.erase(it);
        } else {
            ++it;
        }
    }
}

void FragmentEffectManager::Update(float deltaTime) {
    // Update fragments
    UpdateFragments(deltaTime);
    
    // Check if any fragmenting objects should be restored
    for (auto it = m_fragmentingObjects.begin(); it != m_fragmentingObjects.end();) {
        if (!it->isFragmenting) {
            ++it;
            continue;
        }
        
        it->fragmentTimer += deltaTime;
        
        // Check if all fragments for this object are gone
        bool hasFragments = false;
        std::string objName = it->node->GetName();
        for (const auto& fragment : m_fragments) {
            std::string fragmentName = fragment.node->GetName();
            if (fragmentName.find("Fragment_" + objName + "_") == 0) {
                hasFragments = true;
                break;
            }
        }
        
        // If no fragments left and enough time has passed, restore object
        if (!hasFragments && it->fragmentTimer >= FRAGMENT_LIFETIME) {
            RestoreObject(*it);
            it->isFragmenting = false;
            std::cout << "Restored object: " << it->node->GetName() << std::endl;
            ++it;
        } else {
            ++it;
        }
    }
    
    // Remove non-fragmenting objects from list
    m_fragmentingObjects.erase(
        std::remove_if(m_fragmentingObjects.begin(), m_fragmentingObjects.end(),
            [](const ObjectState& state) { return !state.isFragmenting; }),
        m_fragmentingObjects.end()
    );
}

void FragmentEffectManager::RestoreObject(ObjectState& state) {
    if (!state.node) return;
    
    // Restore original transform
    state.node->SetPosition(state.position);
    state.node->SetRotation(state.rotation);
    state.node->SetScale(state.scale.x, state.scale.y, state.scale.z);
    
    // Restore original mesh and material if available
    if (state.originalMesh) {
        state.node->SetMesh(state.originalMesh);
    }
    if (state.originalMaterial) {
        state.node->SetMaterial(state.originalMaterial);
    }
    
    state.isFragmenting = false;
}

bool FragmentEffectManager::IsFragmenting(std::shared_ptr<SceneNode> node) const {
    if (!node) return false;
    
    for (const auto& state : m_fragmentingObjects) {
        if (state.node == node && state.isFragmenting) {
            return true;
        }
    }
    return false;
}

std::vector<std::shared_ptr<SceneNode>> FragmentEffectManager::GetFragmentingObjects() const {
    std::vector<std::shared_ptr<SceneNode>> result;
    for (const auto& state : m_fragmentingObjects) {
        if (state.isFragmenting) {
            result.push_back(state.node);
        }
    }
    return result;
}

} // namespace SoulsEngine

