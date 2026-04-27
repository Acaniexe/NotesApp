#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <typeindex>
#include <string>
#include "canvas.h"

// NODE TYPE
enum class NodeType {
    Note,
    Link,
    ToDo,
    Grid,
    Text,
    Line,
    Draw,
    Image,
    Colour,
    Comment,
    Code
};

inline Vec2 getNodeBaseSize(NodeType type) {
    switch (type) {
        case NodeType::Note:    return {200.0f, 150.0f};
        case NodeType::Text:    return {200.0f, 150.0f};
        case NodeType::Image:   return {200.0f, 150.0f};
        case NodeType::ToDo:    return {200.0f, 150.0f};
        case NodeType::Link:    return {150.0f, 50.0f};
        case NodeType::Grid:    return {300.0f, 300.0f};
        case NodeType::Line:    return {200.0f, 5.0f};
        case NodeType::Draw:    return {300.0f, 200.0f};
        case NodeType::Colour:  return {100.0f, 100.0f};
        case NodeType::Comment: return {200.0f, 100.0f};
        case NodeType::Code:    return {300.0f, 150.0f};
        default:                return {200.0f, 150.0f};
    }
}

struct NodeTypeComponent {
    NodeType type;

    NodeTypeComponent() = default;
    NodeTypeComponent(NodeType t) : type(t) {}
};

// COMPONENTS (NO BASE CLASS)
struct PositionComponent {
    float x = 0.0f;
    float y = 0.0f;

    PositionComponent() = default;
    PositionComponent(float x, float y) : x(x), y(y) {}
};

struct sizeComponent {
    float width = 100.0f;
    float height = 100.0f;

    sizeComponent() = default;
    sizeComponent(float w, float h) : width(w), height(h) {}
};

struct stateComponent {
    bool isSelected = false;
    bool isHovered = false;
    bool isDragging = false;

    Vec2 dragOffset;
};

struct TextComponent {
    std::string text;

    TextComponent() = default;
    TextComponent(const std::string& t) : text(t) {}
};

struct ImageComponent {
    std::string path;

    ImageComponent() = default;
    ImageComponent(const std::string& p) : path(p) {}
};

// ENTITY
struct Entity {
    uint32_t id = 0;
};

// ENTITY MANAGER
class EntityManager {
public:
    Entity createEntity();

    template<typename T, typename... Args>
    void addComponent(Entity entity, Args&&... args);

    template<typename T>
    T* getComponent(Entity entity);

    template<typename T>
    bool hasComponent(Entity entity);

    std::vector<Entity> getEntities() const;

private:
    uint32_t nextEntityId = 1;
    std::vector<Entity> entities;

    // IMPORTANT FIX:
    std::unordered_map<uint32_t,
        std::unordered_map<std::type_index, std::shared_ptr<void>>
    > componentStorage;
};

// TEMPLATE IMPLEMENTATION
template<typename T, typename... Args>
void EntityManager::addComponent(Entity entity, Args&&... args) {
    componentStorage[entity.id][typeid(T)] =
        std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
T* EntityManager::getComponent(Entity entity) {
    auto it = componentStorage.find(entity.id);
    if (it == componentStorage.end()) return nullptr;

    auto compIt = it->second.find(typeid(T));
    if (compIt == it->second.end()) return nullptr;

    return static_cast<T*>(compIt->second.get());
}

template<typename T>
bool EntityManager::hasComponent(Entity entity) {
    auto it = componentStorage.find(entity.id);
    if (it == componentStorage.end()) return false;

    return it->second.find(typeid(T)) != it->second.end();
}

// factory
Entity createNode(EntityManager& em, NodeType type, float x, float y);