#pragma once

#include "SceneNode.h"
#include "ObjectManager.h"
#include <memory>
#include <vector>
#include <random>
#include <glm/glm.hpp>

namespace SoulsEngine {

// Fragment information structure (for object fragmentation effect in editor)
struct EditorFragment {
    std::shared_ptr<SceneNode> node;
    glm::vec3 position;
    glm::vec3 velocity;         // Fragment velocity
    glm::vec3 angularVelocity;  // Fragment rotation speed
    float scale;                // Current scale (starts at 1.0, decreases to 0)
    float lifetime;             // Time remaining before removal
    bool isActive;              // Is active
};

// Object state for restoration
struct ObjectState {
    std::shared_ptr<SceneNode> node;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::shared_ptr<Mesh> originalMesh;
    std::shared_ptr<Material> originalMaterial;
    bool isFragmenting;
    float fragmentTimer;  // Time elapsed since fragmentation started
};

// Fragment Effect Manager class (for editor mode)
class FragmentEffectManager {
public:
    FragmentEffectManager(ObjectManager* objectManager);
    ~FragmentEffectManager();

    // Start fragmentation effect for a selected object
    void StartFragmentation(std::shared_ptr<SceneNode> node);

    // Update fragment effects
    void Update(float deltaTime);

    // Check if an object is currently fragmenting
    bool IsFragmenting(std::shared_ptr<SceneNode> node) const;

    // Get all fragmenting objects
    std::vector<std::shared_ptr<SceneNode>> GetFragmentingObjects() const;

private:
    // Create fragments from an object
    void CreateFragments(ObjectState& state);

    // Update fragments (movement, rotation, scaling)
    void UpdateFragments(float deltaTime);

    // Restore object to original state
    void RestoreObject(ObjectState& state);

    ObjectManager* m_objectManager;

    // Fragmenting objects and their states
    std::vector<ObjectState> m_fragmentingObjects;

    // Active fragments
    std::vector<EditorFragment> m_fragments;

    // Fragment parameters
    static constexpr float FRAGMENT_LIFETIME = 2.5f;  // 2.5 seconds
    static constexpr float FRAGMENT_SHRINK_SPEED = 0.5f;  // Shrink rate per second
    static constexpr float GRAVITY = -9.8f;  // Gravity acceleration
    static constexpr int FRAGMENT_COUNT = 12;  // Number of fragments per object

    // Random number generator
    std::random_device m_rd;
    std::mt19937 m_gen;
};

} // namespace SoulsEngine

